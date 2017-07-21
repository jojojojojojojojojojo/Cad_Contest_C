#include "placer.h"
#include "../common/GnuplotPlotter.h"
#include <cfloat>
#include <climits>
#include <algorithm>

using namespace std;

void Placer::fence() {
    for(unsigned i = 0; i < 1; i++) fence_place(_cir->fregion(i));
}

bool pair_compare_fence(pair<int,double>& _p1, pair<int,double>& _p2)
{
    return (_p1.second<_p2.second);
}

void Placer::sort_cells_fence(Fregion& _fregion)
{
    cell_order.clear();
    vector< pair<int,double> > index_x_pair;
    index_x_pair.reserve(_fregion.numModues());
    for( unsigned  moduleId = 0 ; moduleId < _fregion.numModues() ; moduleId++ )
    {
        index_x_pair.push_back(make_pair(_fregion.mod(moduleId).dbId(), _modPLPos[0][_fregion.mod(moduleId).dbId()].x() ));
    }

    sort(index_x_pair.begin(), index_x_pair.end(), pair_compare_fence);

    cell_order.reserve(_fregion.numModues());
    for( unsigned  moduleId = 0 ; moduleId < _fregion.numModues() ; moduleId++ )
    {
        cell_order.push_back(index_x_pair[moduleId].first);
    }
}

void Placer::set_intervals_for_fregion(Fregion& _fregion, double& top, double& bot)
{
    _row.clear();
    _row.resize(_cir->numRows());
    for(unsigned i = 0;i < _cir->numRows();i++) _row[i].setPosition(0,i*_cir->rowHeight());
    for(unsigned i = 0;i < _fregion.numRects(); i++)
    {
        if(_fregion.rect(i).top() > top) top = _fregion.rect(i).top();
        if(_fregion.rect(i).bottom() < bot) bot = _fregion.rect(i).bottom();

        for(unsigned j = _fregion.rect(i).bottom() ; j <= _fregion.rect(i).top() ; j+=_cir->rowHeight() )
        {
            int rowNum = _cir->y_2_row_id(j);
            if(_row[rowNum].numInterval()==0)
                _row[rowNum].addInterval(_fregion.rect(i).left(),_fregion.rect(i).right());
            else
            {
                int right = _fregion.rect(i).right();
                int left = _fregion.rect(i).left();
                int num = _row[rowNum].numInterval();
                for(int k = 0; k < num; k++)
                {
                    if(left <= _row[rowNum].interval(k).second && left >=_row[rowNum].interval(k).first)
                    {
                        left = _row[rowNum].interval(k).first;
                        _row[rowNum].eraseInterval(k);
                    }
                    else if(right >= _row[rowNum].interval(k).first && right <= _row[rowNum].interval(k).second)
                    {
                        right = _row[rowNum].interval(k).second;
                        _row[rowNum].eraseInterval(k);  
                    }
                }
                _row[rowNum].addInterval(left,right);
            }
            
        }
    }

    GnuplotPlotter plotter;

    plotter.setTitle("placement figure");
    for(unsigned i = 0; i < _row.size(); i++)
    {
        if(_row[i].numInterval()==0) continue;
        for(unsigned j = 0; j < _row[i].numInterval(); j++)
        {
            plotter.addRectangle(_row[i].rectInterval(j));
        }
    }
    // add rectangle of placement core reigon
    plotter.addRectangle(_cir->chipRect());
    Module &module = _cir->module(cell_order[0]);
    plotter.addRectangle(Rect(module.x(),module.y(),module.x()+module.width(),module.y()+module.height()));

    plotter.outputPlotFile("Interval.plt");
}

void Placer::fence_place(Fregion& _fregion)
{
    sort_cells_fence(_fregion);
    double top = DBL_MIN;
    double bot = DBL_MAX;
    set_intervals_for_fregion(_fregion,top,bot);


    double _alpha = (find_utilization()>0.8)?0.005:0.000; // a function of the "density" of the design (subject to change)
    cout<<"Number Of modules = "<<_fregion.numModues()<<endl;
    //cin.get();
    for(unsigned i = 0 ; i < _fregion.numModues() ; i++)
    {
        { cout<<"Placing Module number : "<<i<<endl;}
        cout<<"Placing Module id: "<<cell_order[i]<<endl;
        if(!_cir->module(cell_order[i]).isStdCell()) continue;

        Module* _cell = &_cir->module(cell_order[i]);
        int rowHeight = (int)(_cell->height()/_cir->rowHeight());
        int rowNum = _cir->y_2_row_id(_cell->y());
        double cost_best = DBL_MAX;
        int row_best = rowNum;
        bool placeInDeadSpace = false;

        
        //find previous cluster (with max x position) if one exist
        //upward
        
        for(int counter = 0 ; counter <= (int)(_cir->y_2_row_id(top)-rowNum-rowHeight ) ; counter++)
        {
            assert(rowNum+counter <= (int)(_cir->y_2_row_id(top)-rowHeight));
            //cout<<"RowNum+counter = "<<rowNum+counter<<endl;
            if(abs(_cell->y()-_cir->row_id_2_y(rowNum+counter)) > cost_best) break;
            //if(counter > 15) break;
            if(rowHeight % 2 == 0 && counter % 2 == 1) continue;
            double cost;

            cost = Multi_PlaceRow_trial_fence(_cell,rowHeight,rowNum+counter);
            cout<<"cost = "<<cost<<endl;
            //cout<<"upward\n";
            if(cost < cost_best){
                cost_best = cost;
                row_best = rowNum+counter;
                placeInDeadSpace = false;
            }
            
            /*if(rowHeight == 1 && _row[rowNum+counter].numInterval()!=0)     //try placing in dead space
            {
                cost = reduce_DeadSpace_trial_fence(_cell, rowNum+counter, _alpha);
                if(cost < cost_best){
                    cost_best = cost;
                    row_best = rowNum+counter;
                    placeInDeadSpace = true;
                }
            }*/
        }
        //downward
        for(int counter = 1 ; counter <= rowNum-_cir->y_2_row_id(bot) ; counter++)
        {
            assert(rowNum-counter >= _cir->y_2_row_id(bot));
            //cout<<"RowNum+counter = "<<rowNum+counter<<endl;
            if(abs(_cell->y()-_cir->row_id_2_y(rowNum+counter)) > cost_best) break;
            //if(counter > 15) break;
            if(rowHeight % 2 == 0 && counter % 2 == 1) continue;
            double cost;

            cost = Multi_PlaceRow_trial_fence(_cell,rowHeight,rowNum-counter);
            //cout<<"downward\n";
            if(cost < cost_best){
                cost_best = cost;
                row_best = rowNum-counter;
                placeInDeadSpace = false;
            }
            
            /*if(rowHeight == 1)     //try placing in dead space
            {
                cost = reduce_DeadSpace_trial_fence(_cell, rowNum-counter, _alpha);
                if(cost < cost_best){
                    cost_best = cost;
                    row_best = rowNum-counter;
                    placeInDeadSpace = true;
                }
            }*/
        }
        /*if(cost_best == DBL_MAX)
        {
            cout<<"QQ\n";
            cin.get();
        }*/
        //cout<<"cost_best = "<<cost_best<<endl;
        //cout<<"Place In row = "<<row_best<<endl;
        //cout<<"Place again, cost = "<<Multi_PlaceRow_trial(_cell,rowHeight,row_best)<<endl;
        //cout<<"Done!"<<endl;
        //assert(cost_best!=DBL_MAX);
        if( !placeInDeadSpace)
        {
            Multi_PlaceRow(_cell,rowHeight,row_best);
        }
        else
        {
            assert(reduce_DeadSpace(_cell,row_best));
        }
    }
    //assert(!check_all(_cir->numModules()-1));

    cout<<"Now renew Position\n";
    //cout<<"Cluster number = "<<_clusters.size()<<endl;
    //int counter = 0;
    for(map<int,Cluster*>::iterator iter = _clusters.begin() ; iter != _clusters.end() ; ++iter)
    {
        //cout<<"counter = "<<counter;
        //counter++;
        RenewPosition(*(iter->second));
    }
    
    cout<<"Cluster number = "<<_clusters.size()<<endl;
}

double Placer::Multi_PlaceRow_trial_fence(Module* _cell, int rowHeight, int rowNum)
{
    if(rowHeight%2 == 0 && _cell->isBottomVss() != _cir->isRowBottomVss(rowNum)) 
    { 
        assert(false); 
    }
    Cluster* _cluster = 0;
    int maxX = INT_MIN;
    //vector<int> _rightmosts;
    //_rightmosts.resize(rowHeight,INT_MIN);
    for(int j = 0 ; j < rowHeight ; j++ )
    {
        if(_rowIdClusterMap[rowNum+j] == 0) continue;
        Cluster* _lastClus = _rowIdClusterMap[rowNum+j];
        int index = (_lastClus->_lastNode.find(rowNum+j))->second;
        int rightmost_x = _lastClus->_x_ref + _lastClus->_delta_x[index] + _lastClus->_modules[index]->_module->width();
        if(rightmost_x > maxX ) 
        {
            maxX = rightmost_x;
            if(rightmost_x > (_modPLPos[0][_cell->dbId()].x()))
            {
                _cluster = _lastClus;
            }
        }
        //_rightmosts.push_back(rightmost_x);
    }
    if(_cluster == 0) // new cluster
    {
        //cout<<"new cluster\n";
        Cluster *_clus = new Cluster();
        Node* _newNode = new Node(_cell, rowHeight, rowNum);
        _clus->_e += _cell->weight();
        _clus->_modules.push_back(_newNode);
        _clus->_cellIdModuleMap[_cell->dbId()] = _clus->_modules.size()-1;
        _clus->_ref_module = _newNode;
        _clus->_delta_x.push_back(0);      // delta_x == 0 if module == ref module
        _clus->_q += (_cell->weight())*(_modPLPos[0][_cell->dbId()].x());    //q <- q + e*(x'(i)-delta_x(i))
        set_x_to_site(_clus);
        //double cost = RenewCost(*_clus);

        _clus = Collapse_trial(_clus);
        _clus = Collapse_trial_right(_clus);
        double cost = RenewCost(*_clus) - _clus->_cost;
        cout<<"temp cost = "<<cost<<endl;


        if(Is_Cluster_Block_Overlap_fence(_clus)){ cost = DBL_MAX; }
        delete _clus;
        delete _newNode;
        return cost;
    }
    else              // add to _cluster
    {
        //cout<<"Collapsed\n";
        Cluster *temp = new Cluster(*_cluster);
        AddCell_trial(temp, _cell, rowNum);
        temp = Collapse_trial(temp);//,i==18363);
        temp = Collapse_trial_right(temp);
        double cost = RenewCost(*temp) - temp->_cost;

        if(Is_Cluster_Block_Overlap_fence(temp)){ cost = DBL_MAX; }
        delete temp;
        return cost;
    }
}

bool Placer::Is_Cluster_Block_Overlap_fence(Cluster* _clus)
{
    //record lowest and highest x position of the cluster in every row 
    vector< pair<int,int> > clus_interval;
    clus_interval.resize(_cir->numRows(), make_pair(INT_MAX,INT_MIN));

    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++)
    {
        Node* _node = _clus->_modules[i];
        for(int j = 0 ; j < _node->_degree ; j++)
        {
            if(_clus->_x_ref+_clus->_delta_x[i]+_node->_module->width() > clus_interval[_node->_rowId+j].second)
            {
                clus_interval[_node->_rowId+j].second = _clus->_x_ref+_clus->_delta_x[i]+_node->_module->width();
            }
            if(_clus->_x_ref+_clus->_delta_x[i] < clus_interval[_node->_rowId+j].first)
            {
                clus_interval[_node->_rowId+j].first  = _clus->_x_ref+_clus->_delta_x[i];
            }
        }
    }

    for(unsigned i = 0 ; i< _cir->numRows() ; i++)
    {
        if(clus_interval[i].first == INT_MAX && clus_interval[i].second == INT_MIN) { continue; }
        //check if cluster interval in row interval
        
        for(unsigned j = 0 ; j < _row[i].numInterval() ; j++)
        {
            cout<<"rowNum = "<<i<<endl;
            cout<<"clus_interval[i].first = "<<clus_interval[i].first<<endl;
            cout<<"clus_interval[i].second = "<<clus_interval[i].second<<endl;
            cout<<"(int)_row[i].interval(j).first = "<<(int)_row[i].interval(j).first<<endl;
            cout<<"(int)_row[i].interval(j).second = "<<(int)_row[i].interval(j).second<<endl;
            //inside row interval -> good, no overlap wth preplaced block
            if(clus_interval[i].first >= (int)_row[i].interval(j).first && clus_interval[i].second <= (int)_row[i].interval(j).second )
            {
                break;
            }
            else if(clus_interval[i].first < (int)_row[i].interval(j).first || j == _row[i].numInterval()-1 )
            {
                //if(clus_interval[i].second == 69400){ cout<<"69400true\n";cin.get(); }
                return true;
            }
        }
    }
    return false;
}

double Placer::reduce_DeadSpace_trial_fence(Module* _cell, int _rowNum, double _alpha)
{
    double cost = DBL_MAX;
    assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    assert((int)(_cell->height()/_cir->rowHeight()) == 1);
    bool find_deadSpace = false;

    //find last cell in row
    if(_rowIdClusterMap[_rowNum] == 0) { return DBL_MAX; }
    Cluster* _lastClus = _rowIdClusterMap[_rowNum];
    int last_cell_id = _lastClus->_modules[(_lastClus->_lastNode.find(_rowNum))->second]->_module->dbId();
    int last_cell_left_x = _lastClus->_x_ref + _lastClus->_delta_x[_lastClus->_cellIdModuleMap.find(last_cell_id)->second];


    //find prev cell of last cell
    int prev_cell_id = prev_cells[_rowNum][last_cell_id];
    if( prev_cell_id == -1) 
    { 
        if(last_cell_left_x-leftbound(last_cell_left_x,_rowNum) >= _cell->width())
        {
            cost = abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(_rowNum))+abs(_modPLPos[0][_cell->dbId()].x()-(last_cell_left_x-_cell->width()));
            cost -= _alpha * (_cell->width()*_cell->height());
            return true;
        }
        else { return DBL_MAX; }
    }
    Cluster* _prevClus = _cellIdClusterMap[prev_cell_id];

    //find their respective position
    int prev_cell_right_x= _prevClus->_x_ref + _prevClus->_delta_x[_prevClus->_cellIdModuleMap.find(prev_cell_id)->second];
    prev_cell_right_x += _cir->module(prev_cell_id).width();

    //search the dead space cells by cells from the last cell in _rowNum
    while(!find_deadSpace)
    {
        /*
        if(last_cell_left_x < prev_cell_right_x)
        {
            cout<<"last_cell_left_x = "<<last_cell_left_x<<endl;
            cout<<"prev_cell_right_x= "<<prev_cell_right_x<<endl;
            cout<<"last_cell_name = "<<_cir->module(last_cell_id).name()<<endl;
            cout<<"prev_cell_name = "<<_cir->module(prev_cell_id).name()<<endl;
            cout<<"Real prev cell x = "<<_prevClus->_x_ref + _prevClus->_delta_x[_prevClus->_cellIdModuleMap.find(prev_cell_id)->second]<<endl;
            print_delta_x(_lastClus);
            print_delta_x(_prevClus);
            //assert(false);
            //(_cell->name()=="g267993_u0" && _cir->module(last_cell_id).name() == "g267993_u1")
        }*/
        assert(last_cell_left_x >= prev_cell_right_x);
        if(last_cell_left_x-prev_cell_right_x >= _cell->width())
        {
            find_deadSpace = true;

            cost = abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(_rowNum))+abs(_modPLPos[0][_cell->dbId()].x()-prev_cell_right_x);
            cost -= _alpha * (_cell->width()*_cell->height());
        }
        else
        {
            _lastClus = _prevClus;
            last_cell_id = prev_cell_id;
            last_cell_left_x = prev_cell_right_x - _cir->module(prev_cell_id).width();

            prev_cell_id = prev_cells[_rowNum][last_cell_id];
            if( prev_cell_id == -1) 
            {
                if(last_cell_left_x-leftbound(last_cell_left_x,_rowNum) >= _cell->width())
                {
                    cost = abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(_rowNum))+abs(_modPLPos[0][_cell->dbId()].x()-(last_cell_left_x-_cell->width()));
                    cost -= _alpha * (_cell->width()*_cell->height());
                    return true;
                }
                else { return DBL_MAX; }
            }
            _prevClus = _cellIdClusterMap[prev_cell_id];
            prev_cell_right_x= _prevClus->_x_ref + _prevClus->_delta_x[_prevClus->_cellIdModuleMap.find(prev_cell_id)->second];
            prev_cell_right_x += _cir->module(prev_cell_id).width();
        }
    }

    return cost;
}

int Placer::leftbound(int x, int rowNum)
{
    for(unsigned i = 0;i < _row[rowNum].numInterval(); i++)
    {
        cout<<"x = "<<x<<endl;
        cout<<"l = "<<_row[rowNum].interval(i).first;
        cout<<"r = "<<_row[rowNum].interval(i).second;
        if(x >= _row[rowNum].interval(i).first && x <= _row[rowNum].interval(i).second) 
            return (int)_row[rowNum].interval(i).first;
    }
}

int Placer::rightbound(int x, int rowNum)
{
    for(unsigned i = 0;i < _row[rowNum].numInterval(); i++)
    {
        if(x >= _row[rowNum].interval(i).first && x <= _row[rowNum].interval(i).second) return _row[rowNum].interval(i).second;
    }
}

void Placer::set_x_to_site_fence(Cluster* _clus)
{
    cout<<"set_x_to_site_fence\n";
    double x = (_clus->_q/_clus->_e);
    _clus->_x_ref = _cir->g_x_on_site(x, 0, Circuit::ALIGN_HERE);   //assert to be be >0, but might not < right boundary

    //assert to be within right boundary
    double rightmost = DBL_MIN;
    double leftmost  = DBL_MAX;
    int right_row, left_row;
    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++)
    {
        if(_clus->_x_ref+_clus->_delta_x[i]+_clus->_modules[i]->_module->width() > rightmost)
        {
            rightmost = _clus->_x_ref+_clus->_delta_x[i]+_clus->_modules[i]->_module->width();
            right_row = _cir->y_2_row_id(_clus->_modules[i]->_module->y());
        }
        if(_clus->_x_ref+_clus->_delta_x[i] < leftmost)
        {
            leftmost = _clus->_x_ref+_clus->_delta_x[i];
            left_row = _cir->y_2_row_id(_clus->_modules[i]->_module->y());
        }
    }
    cout<<"leftbound = "<<_row[left_row].interval(0).first;
    cout<<"lestmost = "<<leftmost<<endl; 
    if(leftmost < _row[left_row].interval(0).first)
    {
        //cout<<"leftbound = "<<leftbound(leftmost,left_row)<<endl;
        _clus->_x_ref += _row[left_row].interval(0).first - leftmost;
        //_clus->_x_ref = _cir->g_x_on_site(_clus->_x_ref, 0, Circuit::ALIGN_HERE);
        cout<<"_clus->_x_ref ="<< _clus->_x_ref<<endl;
    }
    if(rightmost > rightbound(rightmost,right_row))
    { 
        _clus->_x_ref -= rightmost-rightbound(rightmost,right_row); 
        _clus->_x_ref = _cir->g_x_on_site(_clus->_x_ref, 0, Circuit::ALIGN_LEFT);
    }
}