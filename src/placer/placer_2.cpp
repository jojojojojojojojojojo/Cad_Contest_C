#include "placer.h"
#include <cfloat>
#include <climits>
#include <algorithm>

//used in sorting intervals
bool pair_compare(pair<double,double>& _p1, pair<double,double>& _p2)
{
    return (_p1.second<_p2.second);
}

void Placer::clear()
{
    fill(_rowIdClusterMap.begin(),_rowIdClusterMap.end(),(Cluster*)(0));
    fill(_cellIdClusterMap.begin(),_cellIdClusterMap.end(),(Cluster*)(0));
    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        fill(prev_cells[i].begin(),prev_cells[i].end(),-1);
        fill(next_cells[i].begin(),next_cells[i].end(),-1);
    }
    _intervals.clear();
    _intervals.resize(_cir->numRows());
}

void Placer::clear_fail()
{
    clear();
    vector<Cluster*> _to_delete;
    for(map<int,Cluster*>::iterator iter = _clusters.begin() ; iter != _clusters.end() ; ++iter)
    {
        if(iter->second->_fence_id == _fence_id)
        {
            _to_delete.push_back(iter->second);
        }
    }
    for(unsigned i = 0 ; i < _to_delete.size() ; i++)
    {
        _clusters.erase(_to_delete[i]->id);
        delete _to_delete[i];
    }
}

void Placer::init_fence(int fence_id)
{
    cout<<"Init Fence..."<<endl;
    _fence_id = fence_id;
    set_intervals();
    _utilization = find_utilization();
}

double Placer::Multi_PlaceRow(Module* _cell, int rowHeight, int rowNum)
{
    /*
    _cell->setIsBottomVss();
    if(rowHeight%2 == 0){
        if(_cell->isBottomVss() != _cir->isRowBottomVss(rowNum)) return INT_MAX;
    }
    */
    // setIsBottomVss() is useful only after the cell was flipped
    if(rowHeight%2 == 0 && _cell->isBottomVss() != _cir->isRowBottomVss(rowNum)) 
    { 
        assert(false); 
    }

    Cluster* _cluster = 0;
    double maxX = DBL_MIN;
    for(int j = 0 ; j < rowHeight ; j++ )
    {
        /*if(_cell->name() == "g284559_u0")
        {
            cout<<"Row = "<<(rowNum+j)<<"\n";
        }*/
        if(_rowIdClusterMap[rowNum+j] == 0) continue;
        Cluster* _lastClus = _rowIdClusterMap[rowNum+j];
        int index = (_lastClus->_lastNode.find(rowNum+j))->second;
        double rightmost_x = _lastClus->_x_ref + _lastClus->_delta_x[index] + _lastClus->_modules[index]->_module->width();
        if(rightmost_x > maxX ) 
        {
            maxX = rightmost_x;
            if(rightmost_x > (_modPLPos[0][_cell->dbId()].x()))
            {
                _cluster = _lastClus;
            }
        }
        /*
        if(_cell->name() == "g284559_u0")
        {
            cout<<"rightmost_x = "<<rightmost_x<<endl;
        }*/
    }
    //if(_cell->name() == "g284559_u0") cin.get();
    if(_cluster == 0) // new cluster
    {
        /*if(_cell->name() == "g57484_u3")
        {
            cout<<"new";cin.get();
        }*/
        _cluster = new Cluster(_fence_id);
        _clusters[_cluster->id] = _cluster;
        AddCell(_cluster, _cell, rowNum, true);
        while(CheckOverlap(_cluster) != make_pair(0,0) || CheckOverlap_right(_cluster) != make_pair(0,0))
        {
            _cluster = Collapse(_cluster);//,i==18363);
            _cluster = Collapse_right(_cluster);
        }        
        _cluster->_cost = RenewCost(*_cluster);
    }
    else              // add to _cluster
    {
        /*if(_cell->name() == "g57484_u3")
        {
            cout<<"add in cluster\n";
            print_delta_x(_cluster);
            double x = _cir->g_x_on_site((_cluster->_q/_cluster->_e), 0, Circuit::ALIGN_HERE);
            cout<<"\nQ over E == "<<x<<endl;

            Cluster* _clus = _cluster;
            _clus->_x_ref = x;
            vector<double> rightmosts, leftmosts;
            rightmosts.resize(_cir->numRows(), DBL_MIN);
            leftmosts.resize(_cir->numRows(), DBL_MAX);
            for(unsigned i = 0 ; i < _clus->_modules.size() ; i++)
            {
                for(int j = _clus->_modules[i]->_rowId ; j < _clus->_modules[i]->_rowId+_clus->_modules[i]->_degree ; j++)
                {
                    if(_clus->_x_ref+_clus->_delta_x[i]+_clus->_modules[i]->_module->width() > rightmosts[j])
                    {
                        rightmosts[j] = _clus->_x_ref+_clus->_delta_x[i]+_clus->_modules[i]->_module->width();
                    }
                    if(_clus->_x_ref+_clus->_delta_x[i] < leftmosts[j])
                    {
                        leftmosts[j] = _clus->_x_ref+_clus->_delta_x[i];
                    }
                }        
            }
            for(unsigned i = 0 ; i < _cir->numRows() ; i++)
            {
                if(rightmosts[i]!=DBL_MIN)
                {
                    cout<<"\nRow Num = "<<i<<endl;
                    cout<<">>rightmosts[i] = "<<rightmosts[i]<<endl;
                    cout<<">>leftmosts[i] = "<<leftmosts[i]<<endl;
                }
            }
            
            double rightshift = 0;
            double leftshift  = 0;
            for(unsigned i = 0 ; i < _cir->numRows() ; i++)
            {
                if(_intervals[i].empty() || (rightmosts[i]==DBL_MIN && leftmosts[i]==DBL_MAX)) continue;
                for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
                {
                    if(_intervals[i][j].first <= leftmosts[i] && _intervals[i][j].second >= rightmosts[i]) { break; }
                    //if(j != 0 && _intervals[i][j-1].second > leftmosts[i]) { break; }
                    if(_intervals[i][j].first > leftmosts[i] )
                    {
                        cout<<"(_intervals[i][j].second-_intervals[i][j].first) = "<<(_intervals[i][j].second-_intervals[i][j].first)<<endl;
                        cout<<"rightmosts[i]-leftmosts[i] = "<<rightmosts[i]-leftmosts[i]<<endl;
                        if((_intervals[i][j].second-_intervals[i][j].first) < rightmosts[i]-leftmosts[i]) continue;
                        if(_intervals[i][j].first - leftmosts[i] > rightshift)
                        {
                            rightshift = (_intervals[i][j].first - leftmosts[i]);
                        } 
                        break;
                    }
                }  
                for(int j = (int)_intervals[i].size()-1 ; j >= 0 ; j--)
                { 
                    if(_intervals[i][j].first <= leftmosts[i] && _intervals[i][j].second >= rightmosts[i]) { break; }
                    //if(j != (int)_intervals[i].size()-1 && _intervals[i][j+1].first < rightmosts[i]) { break; }
                    if(rightmosts[i] > _intervals[i][j].second )
                    { 
                        if((_intervals[i][j].second-_intervals[i][j].first) < rightmosts[i]-leftmosts[i]) continue;
                        if(rightmosts[i] - _intervals[i][j].second > leftshift)
                        {
                            leftshift = rightmosts[i] - _intervals[i][j].second;
                        }
                        break;
                    } 
                }
            }
            cout<<" Right Shift = "<<rightshift<<endl;
            cout<<" Left Shift = "<<leftshift<<endl;
            cin.get();

            set_x_to_site(_clus);
            print_delta_x(_clus);
            cin.get();
        }*/
        AddCell(_cluster, _cell, rowNum, false);
        while(CheckOverlap(_cluster) != make_pair(0,0) || CheckOverlap_right(_cluster) != make_pair(0,0))
        {
            _cluster = Collapse(_cluster);//,i==18363);
            _cluster = Collapse_right(_cluster);
        }
        _cluster->_cost = RenewCost(*_cluster);
        /*if(_cell->name() == "g74926_u0")
        {
            cout<<"AFter Adding in CLuster\n";
            print_delta_x(_cluster);
            cout<<"\nQ over E == "<<_cir->g_x_on_site((_cluster->_q/_cluster->_e), 0, Circuit::ALIGN_HERE);
            cin.get();
        }*/
    }
    /*
    if(Is_Cluster_Block_Overlap(_cluster,true))
    {
        cout<<"overlap\n";
        cin.get();
    }*/
    return 0;
}

double Placer::Multi_PlaceRow_trial(Module* _cell, int rowHeight, int rowNum)
{
    if(rowHeight%2 == 0 && _cell->isBottomVss() != _cir->isRowBottomVss(rowNum)) 
    { 
        assert(false); 
    }
    Cluster* _cluster = 0;
    double maxX = DBL_MIN;
    vector<double> _rightmosts;
    _rightmosts.resize(rowHeight,DBL_MIN);
    for(int j = 0 ; j < rowHeight ; j++ )
    {
        if(_rowIdClusterMap[rowNum+j] == 0) continue;
        Cluster* _lastClus = _rowIdClusterMap[rowNum+j];
        int index = (_lastClus->_lastNode.find(rowNum+j))->second;
        double rightmost_x = _lastClus->_x_ref + _lastClus->_delta_x[index] + _lastClus->_modules[index]->_module->width();
        if(rightmost_x > maxX ) 
        {
            maxX = rightmost_x;
            if(rightmost_x > (_modPLPos[0][_cell->dbId()].x()))
            {
                _cluster = _lastClus;
            }
        }
        _rightmosts.push_back(rightmost_x);
    }
    if(_cluster == 0) // new cluster
    {
        //cout<<"new cluster\n";
        Node* _todelete = 0;
        Cluster *_clus = new Cluster(_fence_id);
        if(!AddCell_trial(_clus, _cell, rowNum, true, _todelete)) 
        { 
            delete _clus;
            delete _todelete;
            return DBL_MAX; 
        }
        while(CheckOverlap_trial(_clus)!=make_pair(0,0) || CheckOverlap_trial_right(_clus)!=make_pair(0,0))
        {
            _clus = Collapse_trial(_clus);
            _clus = Collapse_trial_right(_clus);
        }
        double cost = RenewCost(*_clus) - _clus->_cost;


        if(Is_Cluster_Block_Overlap(_clus)){ cost = DBL_MAX; }
        for(int i = 0; i < rowHeight ; i++)   
        {
            int prevCellId = prev_cells[rowNum+i][_cell->dbId()];
            if(prevCellId != -1)         
            {
                prev_cells[rowNum+i][_cell->dbId()] = -1;   
                next_cells[rowNum+i][prevCellId] = -1;  
            }                   
        }
        delete _clus;
        delete _todelete;
        return cost;
    }
    else              // add to _cluster
    {
        //cout<<"Collapsed\n";
        Node* _todelete = 0;
        Cluster *temp = new Cluster(*_cluster);
        if(!AddCell_trial(temp, _cell, rowNum, false, _todelete)) 
        { 
            delete temp;
            delete _todelete;
            return DBL_MAX; 
        }
        while(CheckOverlap_trial(temp)!=make_pair(0,0) || CheckOverlap_trial_right(temp)!=make_pair(0,0))
        {
            temp = Collapse_trial(temp);//,i==18363);
            temp = Collapse_trial_right(temp);
        }
        double cost = RenewCost(*temp) - temp->_cost;

        if(Is_Cluster_Block_Overlap(temp)){ cost = DBL_MAX; }
        for(int i = 0; i < rowHeight ; i++)   
        {
            int prevCellId = prev_cells[rowNum+i][_cell->dbId()];
            if(prevCellId != -1)         
            {
                prev_cells[rowNum+i][_cell->dbId()] = -1;   
                next_cells[rowNum+i][prevCellId] = -1;  
            }          
        }
        delete temp;
        delete _todelete;
        return cost;
    }
}

bool Placer::AddCell_trial(Cluster* _clus,Module* _cell, int _rowNum, bool _firstCell, Node* &_todelete)
{
    //cout<<"AddCell\n";
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    if(_firstCell)
    {
        Node* _newNode = new Node(_cell, rowHeight, _rowNum);
        _todelete = _newNode;
        _newNode->set_x_pos(get_valid_pos(_cell,_rowNum));    
        if(_newNode->_x_pos == DBL_MAX) { return false; }
        _clus->_e += _cell->weight();
        _clus->_modules.push_back(_newNode);
        _clus->_cellIdModuleMap[_cell->dbId()] = _clus->_modules.size()-1;
        _clus->_ref_module = _newNode;
        _clus->_delta_x.push_back(0);      // delta_x == 0 if module == ref module
        _clus->_q += (_cell->weight())*(_newNode->_x_pos);    //q <- q + e*(x'(i)-delta_x(i))
        
    }
    else
    {
        assert(_clus->_modules.size() != 0);
        assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
        int rowHeight = (int)(_cell->height()/_cir->rowHeight());
        Node* _newNode = new Node(_cell, rowHeight, _rowNum);
        _todelete = _newNode;
        _newNode->set_x_pos(get_valid_pos(_cell,_rowNum));    
        if(_newNode->_x_pos == DBL_MAX) { return false; }
        _clus->_e += _cell->weight();   //numPins()

        //add module to _modules, _cellIdModuleMap
        _clus->_modules.push_back(_newNode);
        _clus->_cellIdModuleMap[_cell->dbId()] = _clus->_modules.size()-1;

        map<int, int>::iterator _iter;

        int delta_x = INT_MIN;
        // find previous node in cluster ( set "all"(not just adjacent ones) previous cells to FIs)->already modified 

        // renew prev_cells at the same time
        for(int i = 0; i < rowHeight ; i++)   
        {
            _iter = _clus->_lastNode.find(_rowNum+i);
            if(_iter != _clus->_lastNode.end())    //find previous node
            {
                Node* _prevNode = _clus->_modules[_iter->second];
                if(_clus->_delta_x[_iter->second]+_prevNode->_module->width() > delta_x)
                {
                    delta_x = _clus->_delta_x[_iter->second]+_prevNode->_module->width();
                }
            }
        }

        assert(delta_x != INT_MIN);
        // renew _clus->_delta_x
        _clus->_delta_x.push_back(delta_x);

        // renew q
        _clus->_q += (_cell->weight())*(_newNode->_x_pos-delta_x);
    }

    for(int i = 0; i < rowHeight ; i++)   
    {
        Cluster* _prevClus = _rowIdClusterMap[_rowNum+i];  //find previous cluster
        if(_prevClus != 0 )       //if do find a cluster
        {
            assert(_prevClus != _clus);         
            int nodeIndex = (_prevClus->_lastNode.find(_rowNum+i))->second;
            Node* _prevNode = _prevClus->_modules[nodeIndex];
            prev_cells[_rowNum+i][_cell->dbId()] = _prevNode->_module->dbId();
            next_cells[_rowNum+i][_prevNode->_module->dbId()] = _cell->dbId();
        }
    }

    set_x_to_site(_clus);
    //double cost = RenewCost(*_clus);

    return true;
}

Cluster* Placer::Collapse_trial(Cluster* _clus)
{
    //cout<<"Collapse\n";
    pair<int,int> _overlap = CheckOverlap_trial(_clus);
    //cout<<"CheckOverlap done\n";
    if(_overlap.first!=0 || _overlap.second!=0){
        _clus = AddCluster_trial(&_cir->module(_overlap.first),&_cir->module(_overlap.second),_clus);
        _clus = Collapse_trial(_clus);
    }
    return _clus;
}

Cluster* Placer::Collapse_trial_right(Cluster* _clus)
{
    //cout<<"Collapse\n";
    pair<int,int> _overlap = CheckOverlap_trial_right(_clus);
    //cout<<"CheckOverlap done\n";
    if(_overlap.first!=0 || _overlap.second!=0){
        _clus = AddCluster_trial_right(&_cir->module(_overlap.first),&_cir->module(_overlap.second),_clus);
        _clus = Collapse_trial_right(_clus);
    }
    return _clus;
}

//add _prevClus to _clus (_cell belongs to _clus)
Cluster* Placer::AddCluster_trial(Module* _prevCell, Module* _cell, Cluster* _clus)
{
    //cout<<"AddCluster\n";
    Cluster* _prevClus = _cellIdClusterMap[_prevCell->dbId()];
    assert(_prevClus != 0 && _clus != 0 && _prevClus != _clus);
    int _prevNodeIndex = _prevClus->_cellIdModuleMap.find(_prevCell->dbId())->second;
    int _nodeIndex = _clus->_cellIdModuleMap.find(_cell->dbId())->second;

    //assert overlap is true
    assert(_prevClus->_x_ref+_prevClus->_delta_x[_prevNodeIndex]+_prevCell->width() > _clus->_x_ref+_clus->_delta_x[_nodeIndex]);

    double ref_dist = _prevClus->_delta_x[_prevNodeIndex]+_prevCell->width()-_clus->_delta_x[_nodeIndex];


    //renew _clus id
    //_clus->id = _prevClus->id;
    // why should the id be renewed?

    //renew e, q, delta_x, _prevClus->_modules, _cellIdModuleMap
    _clus->_e += _prevClus->_e;
    //_clus->_q += _prevClus->_q + (_prevClus->_e)*(ref_dist);
    for(unsigned i = 0 ; i < _prevClus->_modules.size() ; i++)
    {
        _clus->_modules.push_back(_prevClus->_modules[i]);
        _clus->_delta_x.push_back(_prevClus->_delta_x[i]-ref_dist);
        //_clus->_q += _prevClus->_modules[i]->_module->weight()*(_prevClus->_modules[i]->_x_pos-(_prevClus->_delta_x[i]-ref_dist));
        _clus->_q += _prevClus->_modules[i]->_module->weight()*(_modPLPos[0][_prevClus->_modules[i]->_module->dbId()].x()-(_prevClus->_delta_x[i]-ref_dist));
        _clus->_cellIdModuleMap[_prevClus->_modules[i]->_module->dbId()] = _clus->_modules.size()-1;
    }

    //store _cost of _prevClus
    _clus->_cost += _prevClus->_cost;

    //re-evaluate position
    set_x_to_site(_clus);

    return _clus;
}

// _prevCell belongs to _clus
Cluster* Placer::AddCluster_trial_right(Module* _prevCell, Module* _cell, Cluster* _clus)
{
    Cluster* _newClus = new Cluster(*(_cellIdClusterMap[_cell->dbId()]));

    //cout<<"AddCluster\n";
    //Cluster* _prevClus = _cellIdClusterMap[_prevCell->dbId()];
    //assert(_prevClus != 0 && _clus != 0 && _prevClus != _clus);
    int _prevNodeIndex = _clus->_cellIdModuleMap.find(_prevCell->dbId())->second;
    int _nodeIndex = _newClus->_cellIdModuleMap.find(_cell->dbId())->second;

    //assert overlap is true
    assert(_clus->_x_ref+_clus->_delta_x[_prevNodeIndex]+_prevCell->width() > _newClus->_x_ref+_newClus->_delta_x[_nodeIndex]);
    
    double ref_dist = _clus->_delta_x[_prevNodeIndex]+_prevCell->width()-_newClus->_delta_x[_nodeIndex];


    //renew _clus id
    //_clus->id = _prevClus->id;
    // why should the id be renewed?

    //renew e, q, delta_x, _prevClus->_modules, _cellIdModuleMap
    _newClus->_e += _clus->_e;
    //_clus->_q += _prevClus->_q + (_prevClus->_e)*(ref_dist);
    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++)
    {
        _newClus->_modules.push_back(_clus->_modules[i]);
        _newClus->_delta_x.push_back(_clus->_delta_x[i]-ref_dist);
        //_newClus->_q += _clus->_modules[i]->_module->weight()*(_clus->_modules[i]->_x_pos-(_clus->_delta_x[i]-ref_dist));
        _newClus->_q += _clus->_modules[i]->_module->weight()*(_modPLPos[0][_clus->_modules[i]->_module->dbId()].x()-(_clus->_delta_x[i]-ref_dist));
        _newClus->_cellIdModuleMap[_clus->_modules[i]->_module->dbId()] = _newClus->_modules.size()-1;
    }

    //store _cost of _prevClus
    _newClus->_cost += _clus->_cost;

    //re-evaluate position
    set_x_to_site(_newClus);
    delete _clus;
    return _newClus;
}

//try putting a "single" row height cell in the white space of _rowNum
//put the target cell in the first placeable dead space (i.e. first area that can accommodate the cell)
//return the cost (increased displacement - alpha*cell_area)(dead space removed == cell area)
//return DBL_MAX when no such dead space
double Placer::reduce_DeadSpace_trial(Module* _cell, int _rowNum, double _alpha)
{
    //(int)_cir->row(i).interval(j).first
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
        if(last_cell_left_x >= _cell->width())
        {
            if(Is_Interval_Block_Overlap(make_pair(last_cell_left_x-_cell->width(),last_cell_left_x), _rowNum))
            {
                return DBL_MAX;
            }
            cost = abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(_rowNum))+abs(_modPLPos[0][_cell->dbId()].x()-(last_cell_left_x-_cell->width()));
            cost -= _alpha * (_cell->width()*_cell->height());
            return cost;
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
            //cout<<"Real prev cell x = "<<_prevClus->_x_ref + _prevClus->_delta_x[_prevClus->_cellIdModuleMap.find(prev_cell_id)->second]<<endl;
            print_delta_x(_lastClus);
            print_delta_x(_prevClus);
            assert(false);
            //(_cell->name()=="g267993_u0" && _cir->module(last_cell_id).name() == "g267993_u1")
        }//*/
        assert(last_cell_left_x >= prev_cell_right_x);
        if(last_cell_left_x-prev_cell_right_x >= _cell->width() && !Is_Interval_Block_Overlap(make_pair(last_cell_left_x-_cell->width(),last_cell_left_x), _rowNum))
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
                if(last_cell_left_x >= _cell->width())
                {
                    if(Is_Interval_Block_Overlap(make_pair(last_cell_left_x-_cell->width(),last_cell_left_x), _rowNum))
                    {
                        return DBL_MAX;
                    }
                    cost = abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(_rowNum))+abs(_modPLPos[0][_cell->dbId()].x()-(last_cell_left_x-_cell->width()));
                    cost -= _alpha * (_cell->width()*_cell->height());
                    return cost;
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

pair<int,int> Placer::CheckOverlap_trial(Cluster* _clus)
{
    //cout<<"CheckOverlap\n";
    pair<int,int> overlap;
    overlap = make_pair(0,0);
    int _x_max = 0;
    //map<int,int>::iterator iter ;

    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++){
        //cout<<"size = "<<_clus->_modules.size()<<endl;
        //cout<<"i = "<<i<<endl;
        int ref = _clus->_modules[i]->_module->dbId(); //overlap cell id in _clus
        int ref_prev = -1;                             //overlap cell id in prev clus
        int row = _clus->_modules[i]->_rowId;
        for(int j = row ; j < row + _clus->_modules[i]->_degree ; j++){
            //iter = prev_cells[j].find(ref);
            //if(iter == prev_cells[j].end()) { /*cout<<"no prev"<<endl;*/continue; }
            //ref_prev = iter->second;    // if this cell is the first cell in the row ? ans: then find will return map::end()
            if(prev_cells[j][ref] == -1) { continue; }
            ref_prev = prev_cells[j][ref];

            //check if ref_prev is in the same cluster (_clus) or not
            if(_clus->_cellIdModuleMap.find(ref_prev) != _clus->_cellIdModuleMap.end()) { /*cout<<"same"<<endl;*/ continue;}
            //if(_cellIdClusterMap[ref_prev]->id != _clus->id){
            else{
                //cout<<"else"<<endl;
                int x_ref = _cellIdClusterMap[ref_prev]->_x_ref;
                x_ref += _cellIdClusterMap[ref_prev]->_delta_x[_cellIdClusterMap[ref_prev]->_cellIdModuleMap.find(ref_prev)->second];
                x_ref += _cir->module(ref_prev).width();

                //int x_ref_next = _clus->_delta_x[_clus->_cellIdModuleMap.find(ref_next)->second] + _clus->_x_ref;
                int x_ref_next = _clus->_delta_x[i] + _clus->_x_ref;

                if(x_ref_next < x_ref && (x_ref-x_ref_next) > _x_max){
                    _x_max = x_ref-x_ref_next;
                    overlap = make_pair(ref_prev,ref);
                } 
            }
        }
    }
    if(_x_max != 0){
        //cout<<"Overlap = "<<_x_max<<endl;
        //cout<<"prev = "<<get<0>(overlap)<<endl;
        //cout<<"ref = "<<get<1>(overlap)<<endl;
        }
    return overlap; // return (0,0) if no overlap occurs
}

pair<int,int> Placer::CheckOverlap_trial_right(Cluster* _clus)
{
    pair<int,int> overlap;
    overlap = make_pair(0,0);
    int _x_max = 0;
    //map<int,int>::iterator iter ;

    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++){
        int ref = _clus->_modules[i]->_module->dbId(); //overlap cell id in _clus
        int ref_next = -1;                             //overlap cell id in next clus
        int row = _clus->_modules[i]->_rowId;
        for(int j = row ; j < row + _clus->_modules[i]->_degree ; j++){
            //iter = prev_cells[j].find(ref);
            //if(iter == prev_cells[j].end()) { continue; }
            //ref_prev = iter->second;    // if this cell is the first cell in the row ? ans: then find will return map::end()
            if(next_cells[j][ref] == -1) { continue; }
            ref_next = next_cells[j][ref];
            if(_clus->_cellIdModuleMap.find(ref_next) != _clus->_cellIdModuleMap.end()) continue;
            //if(_cellIdClusterMap[ref_next]->id != _clus->id){
            else
            {
                int x_ref_next = _cellIdClusterMap[ref_next]->_x_ref;
                x_ref_next += _cellIdClusterMap[ref_next]->_delta_x[_cellIdClusterMap[ref_next]->_cellIdModuleMap.find(ref_next)->second];
                

                //int x_ref_next = _clus->_delta_x[_clus->_cellIdModuleMap.find(ref_next)->second] + _clus->_x_ref;
                int x_ref = _clus->_delta_x[i] + _clus->_x_ref + _cir->module(ref).width();

                if(x_ref_next < x_ref && (x_ref-x_ref_next) > _x_max){
                    _x_max = x_ref-x_ref_next;
                    overlap = make_pair(ref,ref_next);
                }
            }
        }
    }
    if(_x_max != 0){/*cout<<"Overlap = "<<_x_max<<endl;*/}
    return overlap; // return (0,0) if no overlap occurs
}

//return true if _clus overlap with preplaced block / IO pins
bool Placer::Is_Cluster_Block_Overlap(Cluster* _clus, bool output) const
{
    //record lowest and highest x position of the cluster in every row 
    vector< vector<pair<double,double> > > clus_interval;
    clus_interval.resize(_cir->numRows());

    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++)
    {
        Node* _node = _clus->_modules[i];
        for(int j = 0 ; j < _node->_degree ; j++)
        {
            clus_interval[_node->_rowId+j].push_back(make_pair(_clus->_x_ref+_clus->_delta_x[i],_clus->_x_ref+_clus->_delta_x[i]+_node->_module->width()));
        }
    }

    for(unsigned i = 0 ; i< _cir->numRows() ; i++)
    {
        //if(clus_interval[i][0].first == INT_MAX && clus_interval[i][0].second == INT_MIN) { continue; }
        //check if cluster interval in row interval
        for(unsigned k = 0 ; k < clus_interval[i].size() ; k++)
        {
            if(Is_Interval_Block_Overlap(clus_interval[i][k],i,output)) 
            { 
                if(output)
                {
                    //print_delta_x(_clus);
                }
                return true; 
            }
        }
    }
    return false;
}

//change to interval related
bool Placer::Is_Interval_Block_Overlap(pair<double,double> _interval, int _rowNum, bool output) const
{
    //for(unsigned j = 0 ; j < _cir->row(_rowNum).numInterval() ; j++)
    if(_intervals[_rowNum].empty()){ return true; }
    for(unsigned j = 0 ; j < _intervals[_rowNum].size() ; j++)
    {
        //inside row interval -> good, no overlap wth preplaced block
        if(_interval.first >= _intervals[_rowNum][j].first && _interval.second <= _intervals[_rowNum][j].second )
        {
            return false;
        }
        else if(_interval.first < _intervals[_rowNum][j].first || j == _intervals[_rowNum].size()-1 )
        {
            if(output)
            {
                cout<<"Row y = "<<_rowNum<<endl;
                cout<<"clus_interval[i].first = "<<_interval.first<<endl;
                cout<<"clus_interval[i].second = "<<_interval.second<<endl;
                cout<<"overlapped interval = ["<<_intervals[_rowNum][j].first<<", "<<_intervals[_rowNum][j].second<<"]"<<endl;
            }
            return true;
        }
    }
    assert(false);
    return true;
}

//find utilzation according to _intervals
double Placer::find_utilization()
{
    //double emtpy_area = _cir->chipRect().area();
    double emtpy_area = 0;
    double occupied_area = 0;
    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            emtpy_area += (_intervals[i][j].second-_intervals[i][j].first)*_cir->rowHeight();
        }
    }
    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        /*if(!_cir->module(i).isStdCell())
        {
            emtpy_area -= _cir->module(i).width()*_cir->module(i).height();
        }else*/
        if(_cir->module(i).isStdCell())
        {
            if(_fence_id == -1 && _cir->cellRegion(_cir->module(i).dbId()) == 0)
            {
                occupied_area += _cir->module(i).width()*_cir->module(i).height();
            }
            else if(_fence_id != -1 && _cir->cellRegion(_cir->module(i).dbId()) != 0&&(int)_cir->cellRegion(_cir->module(i).dbId())->id() == _fence_id)
            {
                occupied_area += _cir->module(i).width()*_cir->module(i).height();
            }
        }        
    }
    /*
    cout<<"Occupied area = "<<occupied_area<<endl;
    cout<<"Empty area = "<<emtpy_area<<endl;
    cout<<"Utilization rate = "<<(occupied_area/emtpy_area)<<endl;*/
    return (occupied_area/emtpy_area);
}


void Placer::set_intervals()
{
    assert(_intervals.size() == _cir->numRows());
    int _id = _fence_id;
    for(unsigned i = 0 ; i < _cir->numRows(); i++) { _intervals[i].clear(); }
    if(_id == -1)
    {
        _cir->remove_sites_fence_region_all();
        for(unsigned i = 0; i < _cir->numRows(); i++)
        {
            for(unsigned j = 0;j < _cir->row(i).numInterval(); j++)
            {
                _intervals[i].push_back(_cir->row(i).interval(j));
            }
        }
    }
    else
    {
        Fregion* _fregion =  &_cir->fregion(_id);
        for(unsigned i = 0;i < _fregion->numRects(); i++)
        {
            for(unsigned j = _fregion->rect(i).bottom() ; j < _fregion->rect(i).top() ; j+=_cir->rowHeight() )
            {
                int rowNum = _cir->y_2_row_id(j);
                if(_intervals[rowNum].empty())
                {
                    _intervals[rowNum].push_back(make_pair(_fregion->rect(i).left(),_fregion->rect(i).right()));
                }
                else
                {
                    double right = _fregion->rect(i).right();
                    double left = _fregion->rect(i).left();
                    right = (right > _cir->chipRect().right())?_cir->chipRect().right():right;
                    left = (left < _cir->chipRect().left())?_cir->chipRect().left():left;
                    int num = _intervals[rowNum].size();
                    for(int k = 0; k < num; k++)
                    {
                        //if(left == _intervals[rowNum][k].first && right ==_intervals[rowNum][k].second) break;
                        if(left <= _intervals[rowNum][k].second && left >=_intervals[rowNum][k].first)
                        {
                            left = _intervals[rowNum][k].first;
                            _intervals[rowNum].erase( _intervals[rowNum].begin()+k );
                        }
                        else if(right >= _intervals[rowNum][k].first && right <= _intervals[rowNum][k].second)
                        {
                            right = _intervals[rowNum][k].second;
                            _intervals[rowNum].erase( _intervals[rowNum].begin()+k );  
                        }
                    }
                    _intervals[rowNum].push_back(make_pair(left,right));
                }                
            }
            
            for( size_t i=0; i<_cir->numModules(); ++i )
            {
                Module &mod = _cir->module(i);
                if( mod.width() == 0 || !mod.isFixed() ) { continue; }
                for( size_t j=0; j<mod.numRects(); ++j ){
                    Rect rect = mod.rect(j);
                    size_t lowRowId = _cir->y_2_row_id( rect.bottom() );
                    size_t topRowId = _cir->y_top_2_row_id( rect.top() );

                    for( size_t k=lowRowId; k<=topRowId; ++k )
                    {
                        addBlockedInterval( rect.left(), rect.right(), k);
                    }
                }
            }
        }
        for(unsigned i = 0 ; i < _cir->numRows() ; i++)
        {
            sort(_intervals[i].begin(), _intervals[i].end(), pair_compare);
        }
    }
}

void Placer::addBlockedInterval(double lBlk, double rBlk, unsigned rowNum)
{
    for( unsigned i=0; i<_intervals[rowNum].size(); ++i ){
        double &lInt = _intervals[rowNum][i].first, &rInt = _intervals[rowNum][i].second;
        if( lInt >= rBlk || rInt <= lBlk ) { continue; }

        if( lInt >= lBlk && rInt <= rBlk ){
            //  ---
            // BBBBB
            _intervals[rowNum].erase( vector<pair<double,double> >::iterator( &(_intervals[rowNum][i]) ) );
            --i;
        }else if( lInt >= lBlk && rInt > rBlk ){
            // -----
            // BBB
            _intervals[rowNum][i].first = rBlk;
        }else if( lInt < lBlk && rInt <= rBlk ){
            // -----
            //   BBB
            _intervals[rowNum][i].second = lBlk;
        }else if( lInt < lBlk && rInt > rBlk ){
            // -----
            //  BBB
            _intervals[rowNum].insert( vector<pair<double,double> >::iterator( &(_intervals[rowNum][i+1]) ), make_pair( rBlk, rInt ) );
            _intervals[rowNum][i].second = lBlk;
            ++i;
        }else{
            cout << "[ERROR] Row::addBlockedInterval:: site under blockage is not removed..." << endl;
        }

    }
}

//return DBL_MAX if the row not placeable
double Placer::get_valid_pos(Module* _cell, int _rowId)
{
    double x_pos = _modPLPos[0][_cell->dbId()].x();
    int _rowNum = (int)(_cell->height()/_cir->rowHeight()); 

    if(_rowId < 0 || (_rowId+_rowNum) >= (int)(_cir->numRows())) { return DBL_MAX; }

    // find candidate position for left and right
    double left_cand = x_pos, right_cand = x_pos;
    bool left_valid = true, right_valid = true;
    //find left_cand
    for(int i = _rowId ; i < _rowId+_rowNum ; i++)
    {
        if(_intervals[i].empty()) { return DBL_MAX; }     //not a valid row to put in
        vector<pair<double,double> > &_inter = _intervals[i];
        for(int j = (int)_inter.size()-1 ; j >= 0  ; j--)
        {
            if(left_cand >= _inter[j].first && left_cand+_cell->width() <= _inter[j].second) { break; }
            if(left_cand+_cell->width() > _inter[j].second)
            {
                left_cand = _inter[j].second-_cell->width();
                break;
            }
        }
    }
    //find right_cand
    for(int i = _rowId ; i < _rowId+_rowNum ; i++)
    {
        vector<pair<double,double> > &_inter = _intervals[i];
        for(unsigned j = 0 ; j < _inter.size() ; j++)
        {
            if(right_cand >= _inter[j].first && right_cand+_cell->width() <= _inter[j].second) { break; }
            if(right_cand < _inter[j].first)
            {
                right_cand = _inter[j].first;
                break;
            }
        }
    }
    //cout<<" left_cand = "<<left_cand<<"; right_cand = "<<right_cand<<endl;

    //determine validity
    for(int i = _rowId ; i < _rowId+_rowNum ; i++)
    {
        vector<pair<double,double> > &_inter = _intervals[i];
        if(right_valid)
        {
            for(unsigned j = 0 ; j < _inter.size() ; j++)
            {
                if(right_cand >= _inter[j].first && right_cand+_cell->width() <= _inter[j].second) { break; }
                if(j == _inter.size()-1) { right_valid = false; }
            }
        }
        if(left_valid)
        {
            for(unsigned j = 0 ; j < _inter.size() ; j++)
            {
                if(left_cand >= _inter[j].first && left_cand+_cell->width() <= _inter[j].second) { break; }
                if(j == _inter.size()-1) { left_valid = false; }
            }
        }
    }
    //return position
    if(!left_valid && !right_valid) { return DBL_MAX; }
    else if(left_valid && !right_valid) { return left_cand; }
    else if(right_valid && !left_valid) { return right_cand; }
    else
    {
        return (abs(right_cand-x_pos)>abs(x_pos-left_cand))?left_cand:right_cand;
    }
}

void Placer::Renew_All_Position()
{
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
