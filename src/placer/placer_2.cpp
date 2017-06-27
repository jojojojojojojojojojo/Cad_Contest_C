#include "placer.h"
#include <cfloat>
#include <climits>
#include <algorithm>

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
    int maxX = INT_MIN;
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
    }
    if(_cluster == 0) // new cluster
    {
        _cluster = new Cluster();
        _clusters[_cluster->id] = _cluster;
        AddCell(_cluster, _cell, rowNum, true);
        _cluster = Collapse(_cluster);//,i==18363);
        _cluster->_cost = RenewCost(*_cluster);
    }
    else              // add to _cluster
    {
        AddCell(_cluster, _cell, rowNum, false);
        _cluster = Collapse(_cluster);//,i==18363);
        _cluster->_cost = RenewCost(*_cluster);
    }
    /*
    if(Is_Cluster_Block_Overlap(_cluster))
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
        double cost = RenewCost(*_clus) - _clus->_cost;


        if(Is_Cluster_Block_Overlap(_clus)){ cost = DBL_MAX; }
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
        double cost = RenewCost(*temp) - temp->_cost;

        if(Is_Cluster_Block_Overlap(temp)){ cost = DBL_MAX; }
        delete temp;
        return cost;
    }
}

void Placer::AddCell_trial(Cluster* _clus,Module* _cell, int _rowNum)
{
    //cout<<"AddCell\n";
    assert(_clus->_modules.size() != 0);
    assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    Node* _newNode = new Node(_cell, rowHeight, _rowNum);
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
    _clus->_q += (_cell->weight())*(_modPLPos[0][_cell->dbId()].x()-delta_x);

    // renew _clus->_lastnode and _rowIdClusterMap
    /*for(int i = 0; i < rowHeight ; i++)   
    {
        // renew _clus->_lastnode
        _clus->_lastNode[_rowNum+i] = (_clus->_modules.size()-1);
    }*/

    // renew _x_ref
    set_x_to_site(_clus);
}

Cluster* Placer::Collapse_trial(Cluster* _clus)
{
    //cout<<"Collapse\n";
    pair<int,int> _overlap = CheckOverlap_trial(_clus);
    //cout<<"CheckOverlap done\n";
    if(get<0>(_overlap)!=0 || get<1>(_overlap)!=0){
        _clus = AddCluster_trial(&_cir->module(get<0>(_overlap)),&_cir->module(get<1>(_overlap)),_clus);
        _clus = Collapse_trial(_clus);
    }
    return _clus;
}

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
    _clus->_q += _prevClus->_q + (_prevClus->_e)*(ref_dist);
    for(unsigned i = 0 ; i < _prevClus->_modules.size() ; i++)
    {
        _clus->_modules.push_back(_prevClus->_modules[i]);
        _clus->_delta_x.push_back(_prevClus->_delta_x[i]-ref_dist);
        //_clus->_q += _prevClus->_modules[i]->_module->weight()*(_modPLPos[0][_prevClus->_modules[i]->_module->dbId()].x()-(_prevClus->_delta_x[i]-ref_dist));
        _clus->_cellIdModuleMap[_prevClus->_modules[i]->_module->dbId()] = _clus->_modules.size()-1;
    }
    //store _cost of _prevClus
    _clus->_cost += _prevClus->_cost;

    //re-evaluate position
    set_x_to_site(_clus);

    return _clus;
}

//try putting a "single" row height cell in the white space of _rowNum
//put the target cell in the first placeable dead space (i.e. first area that can accommodate the cell)
//return the cost (increased displacement - alpha*cell_area)(dead space removed == cell area)
//return DBL_MAX when no such dead space
double Placer::reduce_DeadSpace_trial(Module* _cell, int _rowNum, double _alpha)
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
        if(last_cell_left_x >= _cell->width())
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
                if(last_cell_left_x >= _cell->width())
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

//return true if _clus overlap with preplaced block / IO pins
bool Placer::Is_Cluster_Block_Overlap(Cluster* _clus)
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
        
        for(unsigned j = 0 ; j < _cir->row(i).numInterval() ; j++)
        {
            //inside row interval -> good, no overlap wth preplaced block
            if(clus_interval[i].first >= (int)_cir->row(i).interval(j).first && clus_interval[i].second <= (int)_cir->row(i).interval(j).second )
            {
                break;
            }
            else if(clus_interval[i].first < (int)_cir->row(i).interval(j).first || j == _cir->row(i).numInterval()-1 )
            {
                //if(clus_interval[i].second == 69400){ cout<<"69400true\n";cin.get(); }
                return true;
            }
        }
    }
    return false;
}

double Placer::find_utilization()
{
    double emtpy_area = _cir->chipRect().area();
    double occupied_area = 0;
    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        if(!_cir->module(i).isStdCell())
        {
            emtpy_area -= _cir->module(i).width()*_cir->module(i).height();
        }
        else
        {
            occupied_area += _cir->module(i).width()*_cir->module(i).height();
        }
    }
    //cout<<"Utilization rate = "<<(occupied_area/emtpy_area)<<endl;
    return (occupied_area/emtpy_area);
}