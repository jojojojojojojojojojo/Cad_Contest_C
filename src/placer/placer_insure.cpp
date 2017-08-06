#include "placer.h"
#include <cfloat>
#include <climits>
#include <algorithm>

//used in sorting intervals
bool pair_compare_insure(pair<double,double>& _p1, pair<double,double>& _p2)
{
    return (_p1.first<_p2.first);
}

bool pair_compare_insure_dt(pair<double,int>& _p1, pair<double,int>& _p2)
{
    return (_p1.first<_p2.first);
}

bool Placer::legalize_very_dumb()
{
    vector<vector<double> > _rightBound;//right bound of each interval
    _rightBound.resize(_cir->numRows());

    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        _rightBound[i].resize(_intervals[i].size());
    }
    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            _rightBound[i][j] = _cir->g_x_on_site(_intervals[i][j].first,0,Circuit::ALIGN_RIGHT);
        }
    }

    for(unsigned h = 4 ; h >= 1 ; h--)
    {
        unsigned iteration = (h % 2 == 0)?2:1;
        for(unsigned iter = 0; iter < iteration ; iter++)
        {
            for(unsigned i = 0 ; i < _cir->numModules() ; i++)
            {
                if(i % 10000 == 0 && h == 1){ cout<<"Placing Module number : "<<i<<endl;}
                if(!_cir->module(cell_order[i]).isStdCell()) continue;
                if(_fence_id == -1 && _cir->cellRegion(cell_order[i]) != 0) continue;
                if(_fence_id != -1 && (_cir->cellRegion(cell_order[i]) == 0 || (int)_cir->cellRegion(cell_order[i])->id() != _fence_id)) continue;
                //if(_cir->cellRegion(cell_order[i]) != 0) continue;

                Module* _cell = &_cir->module(cell_order[i]);
                bool success = false;
                int rowHeight = (int)(_cell->height()/_cir->rowHeight()); 
                int rowNum = _cir->y_2_row_id(_cell->y());

                if(rowHeight != (int)h) continue;       
                if(h % 2 == 0)
                {
                    // place bottom==Vdd cells first
                    if(_cell->isBottomVss()==(iter==0)) continue;
                }
                for(unsigned j = 0 ; j < _cir->numRows() ; j++)
                {
                    if(rowHeight % 2 == 0 && (int)(abs((int)j-rowNum)) % 2 == 1) continue;
                    if(Multi_PlaceRow_very_dumb(_cell,j,_rightBound))
                    {
                        success = true;
                        break;
                    }
                }
                if(!success)
                {
                    return false;
                }
            }
        }        
    }
    return true;
}

bool Placer::legalize_slight_dumb(bool _forcePlace)
{
	//double _alpha = (_utilization > 0.8)?0.005:0.000; // a function of the "density" of the design (subject to change)
    vector<vector<double> > _rightBound;//right bound of each interval
    _rightBound.resize(_cir->numRows());

    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        _rightBound[i].resize(_intervals[i].size());
    }
    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            _rightBound[i][j] = _cir->g_x_on_site(_intervals[i][j].first,0,Circuit::ALIGN_RIGHT);
        }
    }

    vector<int> _cellRow;
    vector<pair<double,int> > _position_id_order;
    _cellRow.resize(_cir->numModules(),-1);
    _position_id_order.resize(_cir->numModules(),make_pair(DBL_MIN,-1));

    for(unsigned h = 4 ; h >= 1 ; h--)
    {
        for(unsigned i = 0 ; i < _cir->numModules() ; i++)
        {
            if(!_cir->module(cell_order[i]).isStdCell()) continue;
            if(_fence_id == -1 && _cir->cellRegion(cell_order[i]) != 0) continue;
            if(_fence_id != -1 && (_cir->cellRegion(cell_order[i]) == 0 || (int)_cir->cellRegion(cell_order[i])->id() != _fence_id)) continue;
            //if(_cir->cellRegion(cell_order[i]) != 0) continue;

            Module* _cell = &_cir->module(cell_order[i]);
            int rowHeight = (int)(_cell->height()/_cir->rowHeight()); 
            int rowNum = _cir->y_2_row_id(_cell->y());
            //bool placeInDeadSpace = false;

            if(rowHeight != (int)h) continue;

            int row_best = rowNum;
            double cost_best = DBL_MAX;
            //cout<<"rowHeight = "<<rowHeight<<endl;
            //upward
		    for(int counter = 0 ; counter <= (int)(_cir->numRows()-rowNum-rowHeight-1 ) ; counter++)
		    {
		        assert(rowNum+counter <= (int)(_cir->numRows()-rowHeight));
		        //cout<<"RowNum+counter = "<<rowNum+counter<<endl;
		        if(abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(rowNum+counter)) > cost_best) break;
		        //if(counter > 15) break;
		        if(rowHeight % 2 == 0 && counter % 2 == 1) continue;
		        double cost ;
		        cost = Multi_PlaceRow_trial_slight_dumb(_cell,rowNum+counter,_rightBound);
		        //cout<<"upward\n";
		        if(cost < cost_best){
		            cost_best = cost;
		            row_best = rowNum+counter;
		        }
		    }
		    //downward
		    for(int counter = 1 ; counter <= rowNum ; counter++)
		    {
		        assert(rowNum-counter >= 0);
		        //cout<<"RowNum-counter = "<<rowNum-counter<<endl;
		        if(abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(rowNum-counter)) > cost_best) break;
		        //if(counter > 15) break;
		        if(rowHeight % 2 == 0 && counter % 2 == 1) continue;
		        double cost ;
		        cost = Multi_PlaceRow_trial_slight_dumb(_cell,rowNum-counter,_rightBound);
		        //cout<<"downward\n";
		        if(cost < cost_best){
		            cost_best = cost;
		            row_best = rowNum-counter;
		        }
		    }
            if(cost_best == DBL_MAX)
            {
            	cout<<"QQ again\n";// cin.get();
            	return false;
            }
            //put cell in 
            vector<int> _interId;      // to store the interval index for each row
		    _interId.resize(_cir->numRows(), -1);
		    double _position = get_valid_pos(_cell,row_best);
		    for(int i = row_best ; i < row_best+rowHeight ; i++)
		    {
		        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
		        {
		            if(_intervals[i][j].first <= _position && _intervals[i][j].second >= _position + _cell->width())
		            {
		                _interId[i] = j;
		                break;
		            }
		        }
		    }
		    double _rightmost = 0;
		    for(int i = row_best ; i < row_best+rowHeight ; i++)
		    {
		        if(_interId[i] == -1) continue;
		        if(_rightmost < _rightBound[i][_interId[i]])
		        {
		            _rightmost = _rightBound[i][_interId[i]];
		        }
		    }
		    for(int i = row_best ; i < row_best+rowHeight ; i++)
		    {
		        if(_interId[i] == -1) continue;
		        _rightBound[i][_interId[i]] = _rightmost + _cell->width();
		    }
		    /*if(_cell->name() == "h1a/FE_OFC3459_n_69051")
		    {
		    	cout<<"get_valid_pos = "<<get_valid_pos(_cell,row_best)<<endl;
		    	cout<<"Rightmost = "<<_rightmost<<endl;
		    	cin.get();
		    }*/
		    _cellRow[_cell->dbId()] = row_best;
		    _position_id_order[_cell->dbId()] = make_pair(_rightmost,_cell->dbId());
            //Multi_PlaceRow(_cell,rowHeight,row_best);
            //continue;
		    
		    if(_forcePlace)
		    {
				assert(_rightmost == _cir->g_x_on_site(_rightmost,0,Circuit::ALIGN_HERE));
			    Cluster* _cluster = new Cluster(_fence_id);
			    _clusters[_cluster->id] = _cluster;
			    Node* _newNode = new Node(_cell, rowHeight, row_best);
			    _newNode->set_x_pos(_rightmost);    
			    _cluster->_e += _cell->weight();   //numPins()
			    //add module to _modules, _cellIdModuleMap
			    _cluster->_modules.push_back(_newNode);
			    _cluster->_cellIdModuleMap[_cell->dbId()] = _cluster->_modules.size()-1;
			    _cluster->_ref_module = _newNode;
			    _cluster->_delta_x.push_back(0);      // delta_x == 0 if module == ref module
			    _cluster->_q += (_cell->weight())*(_newNode->_x_pos);    //q <- q + e*(x'(i)-delta_x(i))
			    assert(_cellIdClusterMap[_cell->dbId()] == 0);   //assert not exist
			    _cellIdClusterMap[_cell->dbId()] = _cluster;
			    set_x_to_site(_cluster);
		    }		    
        }       
    }
    
    if(_forcePlace) { return true; }

    sort(_position_id_order.begin(), _position_id_order.end(), pair_compare_insure_dt);

    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        if(i % 10000 == 0){ cout<<"Placing Module number : "<<i<<endl;}
        if(_position_id_order[i].second == -1) continue;
        if(!_cir->module(_position_id_order[i].second).isStdCell()) continue;
        if(_fence_id == -1 && _cir->cellRegion(_position_id_order[i].second) != 0) continue;
        if(_fence_id != -1 && (_cir->cellRegion(_position_id_order[i].second) == 0 || (int)_cir->cellRegion(_position_id_order[i].second)->id() != _fence_id)) continue;
        //if(_cir->cellRegion(cell_order[i]) != 0) continue;

        Module* _cell = &_cir->module(_position_id_order[i].second);
        int rowHeight = (int)(_cell->height()/_cir->rowHeight()); 

        //cout<<"_cellRow[_position_id_order[i].second] = "<<_cellRow[_position_id_order[i].second]<<endl;
        Multi_PlaceRow(_cell,rowHeight,_cellRow[_position_id_order[i].second]);
    }

    return true;
}

double Placer::Multi_PlaceRow_trial_slight_dumb(Module* _cell, int rowNum, const vector<vector<double> >& _rightBound)
{
	//cout<<"in; ";cout.flush();
    double _alpha = 10;
    assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    double _position = get_valid_pos(_cell, rowNum);
    if(_position == DBL_MAX) { return DBL_MAX; }
    //_position = _cir->g_x_on_site(_position,0,Circuit::ALIGN_RIGHT);
    if(rowNum < 0 || (rowNum+rowHeight) > (int)(_cir->numRows())) { return DBL_MAX; }

    //find _interval index in each row
    vector<int> _interId;      // to store the interval index for each row
    _interId.resize(_cir->numRows(), -1);
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
    	if(_intervals[i].empty()) { return DBL_MAX; }
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            if(_intervals[i][j].first <= _position && _intervals[i][j].second >= _position + _cell->width())
            {
                _interId[i] = j;
                break;
            }
        }
        if(_interId[i] == -1) { return DBL_MAX; }
    }
    //cout<<"1; ";cout.flush();
    //determine cost (roughly)
    double _cost = 0;
    //cost of y
    _cost += abs(_cir->row_id_2_y(rowNum)-_modPLPos[0][_cell->dbId()].y());
    //cost of x
    double _rightmost = DBL_MIN;
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        if(_rightmost < _rightBound[i][_interId[i]])
        {
            _rightmost = _rightBound[i][_interId[i]];
        }
    }
    //cout<<"2; ";cout.flush();
    //check if out of bound
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        //int sites_num = floor(_intervals[i][_interId[i]].second/_cir->siteStep())-ceil(_intervals[i][_interId[i]].first/_cir->siteStep());
        //int now_site_num = floor((_rightmost + _cell->width()-_intervals[i][_interId[i]].first)/_cir->siteStep());
        if(_rightmost + _cell->width() > _intervals[i][_interId[i]].second) { return DBL_MAX; }
    }
    _cost += abs(_rightmost - _modPLPos[0][_cell->dbId()].x());

    //cost of deadspace
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        _cost += _alpha*(_rightmost-_rightBound[i][_interId[i]])*(_cir->rowHeight());
    }
    //cout<<"out\n";
    return _cost;
}

//try putting _cell in rowNum from left to right (intervals)
//if success, put it in and return true
//else return false
bool Placer::Multi_PlaceRow_very_dumb(Module* _cell, int rowNum, vector<vector<double> >& _rightBound)
{
	vector<pair<double,double> > _placeables;
	if(!find_placeable(_cell,rowNum,_rightBound,_placeables)) { return false; }

	assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    if(rowNum < 0 || (rowNum+rowHeight) > (int)(_cir->numRows())) { return false; }

    //cout<<"RowNum = "<<rowNum<<endl;
    double _toPlace = DBL_MIN;
    for(unsigned i = 0 ; i < _placeables.size() ; i++)
    {
        //cout<<">> placeable interval : [ "<<_placeables[i].first<<", "<<_placeables[i].second<<"]"<<endl;
        if((_placeables[i].second-_placeables[i].first) >= _cell->width() )
        {
            _toPlace = _placeables[i].first;
            assert(_toPlace == _cir->g_x_on_site(_toPlace,0,Circuit::ALIGN_HERE));
            break;
        }
    }
    //cin.get();
    if(_toPlace == DBL_MIN) { return false; }

    vector<int> _interId;
    _interId.resize(_cir->numRows(),-1);    
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            if(_intervals[i][j].first <= _toPlace && _intervals[i][j].second >= _toPlace + _cell->width())
            {
                _interId[i] = j;
                break;
            }
        }
        assert(_interId[i] != -1);
        // check Right Bound
        if(_rightBound[i][_interId[i]] != _toPlace){ return false; }
    }

    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        _rightBound[i][_interId[i]] = _toPlace+ _cell->width();
    }

    //actually putting the cell

    Cluster* _cluster = new Cluster(_fence_id);
    _clusters[_cluster->id] = _cluster;
    Node* _newNode = new Node(_cell, rowHeight, rowNum);
    _newNode->set_x_pos(_toPlace);    
    _cluster->_e += _cell->weight();   //numPins()
    //add module to _modules, _cellIdModuleMap
    _cluster->_modules.push_back(_newNode);
    _cluster->_cellIdModuleMap[_cell->dbId()] = _cluster->_modules.size()-1;
    _cluster->_ref_module = _newNode;
    _cluster->_delta_x.push_back(0);      // delta_x == 0 if module == ref module
    _cluster->_q += (_cell->weight())*(_newNode->_x_pos);    //q <- q + e*(x'(i)-delta_x(i))
    assert(_cellIdClusterMap[_cell->dbId()] == 0);   //assert not exist
    _cellIdClusterMap[_cell->dbId()] = _cluster;
    set_x_to_site(_cluster);

    //cin.get();
    return true;
}

bool Placer::find_placeable(Module* _cell, int rowNum, const vector<vector<double> >& _rightBound, vector<pair<double,double> >& _placeables, bool debugMode)
{
    assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    _placeables.clear();
    if(rowNum < 0 || (rowNum+rowHeight) > (int)(_cir->numRows())) { return false; }

    vector<pair<double, double> > _blocks;
    double _rightmost = DBL_MIN, _leftmost = DBL_MAX;
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
    	if(_intervals[i].empty()) { return false; }
        double interval_right = _cir->g_x_on_site(_intervals[i].back().second,0,Circuit::ALIGN_LEFT);
        double interval_left = _cir->g_x_on_site(_intervals[i][0].first,0,Circuit::ALIGN_RIGHT);
        if(_rightmost < interval_right) { _rightmost = interval_right; } 
        if(_leftmost > interval_left) { _leftmost = interval_left; }
    }
    if(_rightmost < _leftmost) { return false; }

    if(debugMode)
    {
    	cout<<">> leftmost = "<<_leftmost<<" ; rightmost = "<<_rightmost<<endl;
    }

    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        vector<pair<double, double> > _empty_slots;
        if(debugMode){cout<<"\nRow = rowNum+"<<(i-rowNum)<<endl;}
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            double interval_right = _cir->g_x_on_site(_intervals[i][j].second,0,Circuit::ALIGN_LEFT);
            if(debugMode)
            {
            	cout<<">> rightBound["<<i<<"]["<<j<<"] = "<<_rightBound[i][j];
            	cout<<" ; interval_right = "<<interval_right<<endl;
            }
            if(_rightBound[i][j] < interval_right)
            {
                _empty_slots.push_back(make_pair(_rightBound[i][j], interval_right));
            }            
        }
        if(debugMode)
        {
        	for(unsigned j = 1 ; j < _empty_slots.size() ; j++)
	        {
	            cout<<">> empty_slots["<<j<<"] = ("<<_empty_slots[j].first<<", "<<_empty_slots[j].second<<")\n";
	        }
        }
        if(_empty_slots.empty()) { _blocks.push_back(make_pair(_leftmost,_rightmost)); }
        else
        {
            if(_leftmost != _empty_slots[0].first) { _blocks.push_back(make_pair(_leftmost,_empty_slots[0].first));}
            if(_rightmost != _empty_slots.back().second) { _blocks.push_back(make_pair(_empty_slots.back().second, _rightmost));}
        }
        for(unsigned j = 1 ; j < _empty_slots.size() ; j++)
        {
            assert(_empty_slots[j-1].second < _empty_slots[j].first);
            _blocks.push_back(make_pair(_empty_slots[j-1].second, _empty_slots[j].first));
        }
    }
    sort(_blocks.begin(), _blocks.end(), pair_compare_insure);
    if(debugMode)
    {
    	for(unsigned i = 0 ; i < _blocks.size() ; i++)
    	{
    		cout<<">> _blocks["<<i<<"] = ("<<_blocks[i].first<<", "<<_blocks[i].second<<")\n";
    	}
    }
    unsigned _index = 0;
    for(unsigned i = 1 ; i < _blocks.size() ; i++)
    {
        if(_blocks[_index].second >= _blocks[i].first)
        {
            _blocks[_index].second = (_blocks[_index].second>_blocks[i].second)?_blocks[_index].second:_blocks[i].second;
            _blocks.erase( vector<pair<double,double> >::iterator( &(_blocks[i]) ) );
            --i;
        }
        else
        {
            _index = i;
        }
    }
    if(debugMode)
    {
    	cout<<"////////////AFTER MERGING/////////"<<endl;
    	cout<<"_blocks.size() =="<<_blocks.size()<<endl;
    	for(unsigned i = 0 ; i < _blocks.size() ; i++)
    	{
    		cout<<">> _blocks["<<i<<"] = ("<<_blocks[i].first<<", "<<_blocks[i].second<<")\n";
    	}
    }
    if(_blocks.empty()) { _placeables.push_back(make_pair(_leftmost,_rightmost)); }
    else
    {
        if(_leftmost != _blocks[0].first) { _placeables.push_back(make_pair(_leftmost, _blocks[0].first)); }
        if(_rightmost != _blocks.back().second) { _placeables.push_back(make_pair(_blocks.back().second,_rightmost)); }
    }
    for(unsigned j = 1 ; j < _blocks.size() ; j++)
    {
        assert(_blocks[j-1].second < _blocks[j].first);
        _placeables.push_back(make_pair(_blocks[j-1].second, _blocks[j].first));        
    }

    return true;
}

//////////////////////////////////////////////////////////
//	Belows are fail dummy legalization methods
//	Keep them just for reference
//////////////////////////////////////////////////////////

void Placer::legalize_dumb()
{
    //double _alpha = (_utilization > 0.8)?0.005:0.000; // a function of the "density" of the design (subject to change)
    vector<int> _cellRow;               //record the row the cell will place in
    vector<vector<double> > _rightBound;//right bound of each interval
    vector<double> _positions;

    _cellRow.resize(_cir->numModules(), -1);
    _rightBound.resize(_cir->numRows());
    _positions.resize(_cir->numModules(), -1);

    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        _rightBound[i].resize(_intervals[i].size());
    }
    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            _rightBound[i][j] = _cir->g_x_on_site(_intervals[i][j].first,0,Circuit::ALIGN_RIGHT);
        }
    }
    //decide best row for each cell
    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        if(!_cir->module(cell_order[i]).isStdCell()) continue;
        if(_fence_id == -1 && _cir->cellRegion(cell_order[i]) != 0) continue;
        if(_fence_id != -1 && (_cir->cellRegion(cell_order[i]) == 0 || (int)_cir->cellRegion(cell_order[i])->id() != _fence_id)) continue;
        //if(_cir->cellRegion(cell_order[i]) != 0) continue;

        Module* _cell = &_cir->module(cell_order[i]);
        int rowHeight = (int)(_cell->height()/_cir->rowHeight());
        int rowNum = _cir->y_2_row_id(_cell->y());
        double cost_best = DBL_MAX;
        int row_best = rowNum;

        //upward
        for(int counter = 0 ; counter <= (int)(_cir->numRows()-rowNum-rowHeight-1 ) ; counter++)
        {
            assert(rowNum+counter <= (int)(_cir->numRows()-rowHeight));
            //cout<<"RowNum+counter = "<<rowNum+counter<<endl;
            if(abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(rowNum+counter)) > cost_best) break;
            //if(counter > 15) break;
            if(rowHeight % 2 == 0 && counter % 2 == 1) continue;
            double cost;

            cost = Multi_PlaceRow_trial_dumb(_cell,rowNum+counter, _rightBound);
            //cout<<"upward\n";
            if(cost < cost_best){
                cost_best = cost;
                row_best = rowNum+counter;
            }
        }
        //downward
        for(int counter = 1 ; counter <= rowNum ; counter++)
        {
            assert(rowNum-counter >= 0);
            //cout<<"RowNum-counter = "<<rowNum-counter<<endl;
            if(abs(_modPLPos[0][_cell->dbId()].y()-_cir->row_id_2_y(rowNum-counter)) > cost_best) break;
            //if(counter > 15) break;
            if(rowHeight % 2 == 0 && counter % 2 == 1) continue;
            double cost;

            cost = Multi_PlaceRow_trial_dumb(_cell,rowNum-counter, _rightBound);
            //cout<<"downward\n";
            if(cost < cost_best){
                cost_best = cost;
                row_best = rowNum-counter;
            }
        }
        if(cost_best == DBL_MAX)
        {
            cout<<"QQ\n";
            //cin.get();
        }
        //renew _cellRow and _rightBound
        _cellRow[_cell->dbId()] = row_best;
        vector<int> _interId;      // to store the interval index for each row
        _interId.resize(_cir->numRows(), -1);
        double _position = get_valid_pos(_cell,row_best);
        for(int i = row_best ; i < row_best+rowHeight ; i++)
        {
            for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
            {
                if(_intervals[i][j].first <= _position && _intervals[i][j].second >= _position + _cell->width())
                {
                    _interId[i] = j;
                    break;
                }
            }
        }
        double _rightmost = 0;
        for(int i = row_best ; i < row_best+rowHeight ; i++)
        {
            if(_interId[i] == -1) continue;
            if(_rightmost < _rightBound[i][_interId[i]])
            {
                _rightmost = _rightBound[i][_interId[i]];
            }
        }
        for(int i = row_best ; i < row_best+rowHeight ; i++)
        {
            if(_interId[i] == -1) continue;
            _rightBound[i][_interId[i]] = _rightmost + _cell->width();
        }
        /*
        cout<<"Cell #"<<i;
        cout<<" ; Height = "<<_cell->height()<<" ; Width = "<<_cell->width()<<endl;
        cout<<"Best Row = "<<row_best<<endl;
        cout<<"Best Cost = "<<cost_best<<endl;
        cout<<"Rightmost = "<<_rightmost<<endl;//cin.get();*/
        assert(_rightmost == _cir->g_x_on_site(_rightmost,0,Circuit::ALIGN_HERE));
        _positions[_cell->dbId()] = _rightmost;
    }
    
    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        if(i % 10000 == 0){ cout<<"Placing Module number : "<<i<<endl;}
        if(!_cir->module(cell_order[i]).isStdCell()) continue;
        if(_fence_id == -1 && _cir->cellRegion(cell_order[i]) != 0) continue;
        if(_fence_id != -1 && (_cir->cellRegion(cell_order[i]) == 0 || (int)_cir->cellRegion(cell_order[i])->id() != _fence_id)) continue;
        //if(_cir->cellRegion(cell_order[i]) != 0) continue;

        Module* _cell = &_cir->module(cell_order[i]);
        int rowHeight = (int)(_cell->height()/_cir->rowHeight());        
        int row_best = _cellRow[cell_order[i]];
        assert(row_best >= 0 && row_best <= (int)_cir->numRows()-1);

        Multi_PlaceRow(_cell,rowHeight,row_best);
        /*
        Cluster* _cluster = new Cluster(_fence_id);
        _clusters[_cluster->id] = _cluster;
        Node* _newNode = new Node(_cell, rowHeight, row_best);
        _newNode->set_x_pos(_positions[_cell->dbId()]);    
        _cluster->_e += _cell->weight();   //numPins()
        //add module to _modules, _cellIdModuleMap
        _cluster->_modules.push_back(_newNode);
        _cluster->_cellIdModuleMap[_cell->dbId()] = _cluster->_modules.size()-1;
        _cluster->_ref_module = _newNode;
        _cluster->_delta_x.push_back(0);      // delta_x == 0 if module == ref module
        _cluster->_q += (_cell->weight())*(_newNode->_x_pos);    //q <- q + e*(x'(i)-delta_x(i))
        assert(_cellIdClusterMap[_cell->dbId()] == 0);   //assert not exist
        _cellIdClusterMap[_cell->dbId()] = _cluster;
        set_x_to_site(_cluster);*/
    }
}

// find the cost of placing _cell in rowNum
// cost = displacment + alpha* induced deadspace
double Placer::Multi_PlaceRow_trial_dumb(Module* _cell, int rowNum, const vector<vector<double> >& _rightBound)
{
    double _alpha = 0.005;
    assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    double _position = get_valid_pos(_cell, rowNum);
    if(_position == DBL_MAX) { return DBL_MAX; }
    //_position = _cir->g_x_on_site(_position,0,Circuit::ALIGN_RIGHT);
    if(rowNum < 0 || (rowNum+rowHeight) > (int)(_cir->numRows())) { return DBL_MAX; }

    //find _interval index in each row
    vector<int> _interId;      // to store the interval index for each row
    _interId.resize(_cir->numRows(), -1);
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        for(unsigned j = 0 ; j < _intervals[i].size() ; j++)
        {
            if(_intervals[i][j].first <= _position && _intervals[i][j].second >= _position + _cell->width())
            {
                _interId[i] = j;
                break;
            }
        }
        if(_interId[i] == -1) { return DBL_MAX; }
    }

    //determine cost (roughly)
    double _cost = 0;
    //cost of y
    _cost += abs(_cir->row_id_2_y(rowNum)-_modPLPos[0][_cell->dbId()].y());
    //cost of x
    double _rightmost = DBL_MIN;
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        if(_rightmost < _rightBound[i][_interId[i]])
        {
            _rightmost = _rightBound[i][_interId[i]];
        }
    }
    //check if out of bound
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        //int sites_num = floor(_intervals[i][_interId[i]].second/_cir->siteStep())-ceil(_intervals[i][_interId[i]].first/_cir->siteStep());
        //int now_site_num = floor((_rightmost + _cell->width()-_intervals[i][_interId[i]].first)/_cir->siteStep());
        if(_rightmost + _cell->width() > _intervals[i][_interId[i]].second) { return DBL_MAX; }
    }
    _cost += abs(_rightmost - _modPLPos[0][_cell->dbId()].x());
    //cost of deadspace
    for(int i = rowNum ; i < rowNum+rowHeight ; i++)
    {
        _cost += _alpha*(_rightmost-_rightBound[i][_interId[i]])*(_cir->rowHeight());
    }
    //cost of min max row
    double _minX = DBL_MAX;
    double _maxX = DBL_MIN;
    _minX = (_minX > _rightmost+_cell->width())?_rightmost+_cell->width():_minX;
    _maxX = (_maxX < _rightmost+_cell->width())?_rightmost+_cell->width():_maxX;
    for(int i = 0 ; i < (int)_cir->numRows() ; i++)
    {
        if(i >=rowNum && i < rowNum+rowHeight){ continue; }
        for(unsigned j = 0; j < _intervals[i].size() ; j++)
        {
            _minX = (_minX > _rightBound[i][j])?_rightBound[i][j]:_minX;
            _maxX = (_maxX < _rightBound[i][j])?_rightBound[i][j]:_maxX;
        }
    }
    _cost += _alpha*10*(_maxX-_minX)*(_cir->rowHeight());

    return _cost;
}