#include "placer.h"
#include <cfloat>
#include <climits>
#include <algorithm>

unsigned Cluster::global_id = 0;

/*
void Placer::place()
{
    plot("init");

    // ------------------------ global placement
    random_place();
    plot("rand");

    if( !gArg.checkExist("nonlp") )
    {
        nlp::NLPlacer gp(*this);
        gp.solve();
        plot("nlp");
    }

    // ------------------------ legalization
    save_modules_2_pos(PL_LAST);
    lga::TetrisT lg(*this);
    lg.legal();
    //lg.legal_original();
  // lg.legal_improved();

    plot("lg");

    rowpl::RowPlacer rp( _cir );
    if( rp.initRowStructures( false ) ){
        cout << "==================================================" << endl;
        cout << "[Congratulation!!] All modules are non-overlapped!" << endl;
        cout << "==================================================" << endl;
    }else{
        cout << "==================================================" << endl;
        cout << "[Legalization Fail] Some modules overlap!!" << endl;
        cout << "(You may see the " << Plotter::convert2pltName( "lg" ) << " for reference.)" << endl;
        cout << "==================================================" << endl;
        getchar();
    }

    // ------------------------ detailed placement
//    dpT::Test t;
//    t.solve();
}
*/

//used in sorting cells
bool pair_compare(pair<int,double>& _p1, pair<int,double>& _p2)
{
    return (_p1.second<_p2.second);
}

double Placer::compute_hpwl()
{
    double totalHPWL = 0;
    for( size_t i=0; i<_cir->numNets(); ++i ){
        Net &tNet = _cir->net(i);
        double lx=DBL_MAX, ly=DBL_MAX, ux=-DBL_MAX, uy=-DBL_MAX;
        for( size_t j=0; j<tNet.numPins(); ++j ){
            Pin &tPin = tNet.pin(j);
            lx=min( tPin.x(), lx );
            ly=min( tPin.y(), ly );
            ux=max( tPin.x(), ux );
            uy=max( tPin.y(), uy );
        }
        totalHPWL += (ux-lx) + (uy-ly);
    }
    return totalHPWL;
}

double Placer::compute_displacement(const Placer::PL_TYPE &pt)
{
    double totalDisp = 0;
    for( size_t i=0; i<_cir->numComponents(); ++i ){
        Module &mod = _cir->module(i);
        totalDisp += Point::dist( mod.pos(), mod_2_pos(mod, pt) );
    }
    return totalDisp;
}

bool Placer::move_module_2_pos(Module &mod, const Point &pos, MOVE_TYPE mt)
{
    if( mod.isFixed() ) return false;

    Point targetPos( pos.x(), pos.y() );

    if( is_equal( pos.x(), mod.x() ) && is_equal( pos.y(), mod.y() ) ) { return true; }

    if( mt == MOVE_ONSITE ){
        _cir->set_x_y_on_site( targetPos );
        Row &row = _cir->row( _cir->y_2_row_id( targetPos.y() ) );
        mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect(), row.orient() );
    }else{
        mod.setIsBottomVss();
        if(mod.isBottomVss() == _cir->isRowBottomVss( _cir->y_2_row_id( targetPos.y() )))
            mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() );
        else{
            switch(mod.orient()){
                case 0:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_S);
                     break;
                case 1:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_E);
                     break;
                case 2:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_N);
                     break;
                case 3:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_W);
                     break;
                case 4:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_FS);
                     break;
                case 5:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_FE);
                     break;
                case 6:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_FN);  
                     break;
                case 7:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , OR_FW);
                     break;
                default:
                     mod.setPosition( targetPos.x(), targetPos.y(), _cir->chipRect() , );
                     break;
            }
           // still thinking a better way to deal with rotation
           // FN -> FS & FN -> S , which one is better ?
           // Is it possible that VSS is on the EAST or WEST side ?
        }
    }

    return true;
}

bool Placer::move_module_center_2_pos(Module &mod, const Point &pos, MOVE_TYPE mt)
{
    return move_module_2_pos( mod, Point( pos.x()-mod.width()/2, pos.y()-mod.height()/2 ), mt );
}

void Placer::save_modules_2_pos(const Placer::PL_TYPE &pl)
{
    for( size_t i=0; i<_cir->numModules(); ++i ){
        _modPLPos[pl][i].set_x_y( _cir->module(i).x(), _cir->module(i).y() );
    }
}

void Placer::bound_pos_in_max_disp(Module &mod, Point &pos, bool isOnSite, const double &dispRatio)
{
    if( param.maxDisp <= 0 ) return;

    if( isOnSite ) _cir->set_x_y_on_site(pos);

    double initX = mod_2_pos( mod, PL_INIT ).x(),
           initY = mod_2_pos( mod, PL_INIT ).y();
    double xDiff = pos.x()-initX,
           yDiff = pos.y()-initY;
    double displacement = fabs( xDiff ) + fabs( yDiff );

    double marginRatio = 0.9;
    bool isFirst = true;
    while( !is_pos_in_max_disp( mod, pos, dispRatio ) )
    {
        if( isFirst )
        {
            xDiff *= param.maxDisp*dispRatio*marginRatio / displacement;
            yDiff *= param.maxDisp*dispRatio*marginRatio / displacement;
            isFirst = false;
        }
        else
        {
            xDiff *= marginRatio;
            yDiff *= marginRatio;
        }

        displacement = fabs( xDiff ) + fabs( yDiff );
        pos.set_x_y( initX+xDiff, initY+yDiff );

        if( isOnSite ) _cir->set_x_y_on_site(pos);

        //cout << "exceed maxDisp" << endl;
    }

    assert( is_pos_in_max_disp( mod, pos ) );
}

bool Placer::is_pos_in_max_disp(Module &mod, const Point &pos, const double &dispRatio)
{
    if( param.maxDisp <= 0 ) return true; // unassigned
    return Point::dist( pos, mod_2_pos(mod, PL_INIT) ) <= param.maxDisp*dispRatio;
}

void Placer::move_pl_pos(const Point &shift, const double &scale, bool isBack)
{
    for( size_t pl=0; pl<3; ++pl ){
        for( size_t i=0; i<_cir->numModules(); ++i ){
            Point &p = _modPLPos[pl][i];
            p.shift_scale( shift, scale, isBack );
        }
    }
    _cir->move_placement( shift, scale, isBack );
}


///////////////////////////////////////////////////////////////
//                Newly Added Functions (ICCAD'17)           //
///////////////////////////////////////////////////////////////

//only for even row height cells
int Placer::find_valid_row(Module &mod)
{
    bool BottomVss = mod.isBottomVss();
    int rowId = _cir->y_2_row_id( mod.y() );
    if(_cir->isRowBottomVss(rowId)==BottomVss) { return rowId; }
    if( fabs(_cir->row_id_2_y(rowId-1)-mod.y()) < fabs(_cir->row_id_2_y(rowId+1)-mod.y()))
    {
        return max(0, (rowId-1));
    }
    return min((rowId+1), (int)(_cir->numRows()-1));
}

//only for std cell
void Placer::place_valid_site(Module &mod)
{
    assert(mod.isStdCell());
    int rowId;
    Orient orient = mod.orient();
    bool before = mod.isBottomVss(),change = false;
    if((int)(mod.height()/_cir->rowHeight())%2 == 0)
    {
        rowId =find_valid_row(mod);
    }
    else
    {
        rowId = _cir->y_2_row_id( mod.y() );
        if(_cir->isRowBottomVss(rowId) != mod.isBottomVss()){ orient = ((mod.orient()==OR_N)?OR_S:OR_N); change = true;}
    }
    double xPos = _cir->g_x_on_site(mod.x(), rowId, Circuit::ALIGN_HERE);
    double yPos = _cir->row_id_2_y(rowId);
    mod.setPosition( xPos, yPos, _cir->chipRect(), orient );
    mod.setIsBottomVss();
    if(change) { assert(before != mod.isBottomVss()); }
}

void Placer::place_all_mods_to_site()
{
    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        if(_cir->module(i).isStdCell()) { place_valid_site(_cir->module(i)); }
    }
}

void Placer::sort_cells()
{
    cell_order.clear();
    vector< pair<int,double> > index_x_pair;
    index_x_pair.reserve(_cir->numModules());
    for( unsigned  moduleId = 0 ; moduleId < _cir->numModules() ; moduleId++ )
    {
        index_x_pair.push_back(make_pair(moduleId, _modPLPos[0][moduleId].x() ));
    }

    sort(index_x_pair.begin(), index_x_pair.end(), pair_compare);

    cell_order.reserve(_cir->numModules());
    for( unsigned  moduleId = 0 ; moduleId < _cir->numModules() ; moduleId++ )
    {
        cell_order.push_back(index_x_pair[moduleId].first);
    }
}

void Placer::print_cell_order() const
{
    for( unsigned  moduleId = 0 ; moduleId < _cir->numModules() ; moduleId++ )
    {
        unsigned index = cell_order[moduleId];
        if(!_cir->module(index).isStdCell()){ continue; }
        cout<<"Cell X position = "<<_modPLPos[0][index];
        //cout<<"Cell Name = "<<_cir->module(index).name()<<"; X position = "<<_modPLPos[0][index];
        cout<<"; #Row = "<<(int)(_cir->module(index).height()/_cir->rowHeight())<<endl;
    }
}

void Placer::print_fanins_fanouts(Cluster* _clus) const
{
    cout<<"///// Print Fanins Fanouts of a cluster /////\n";
    for(unsigned i = 0 ; i < _clus->_modules.size() ; i++)
    {
        _clus->_modules[i]->printFI();
        _clus->_modules[i]->printFO();
    }
}

void Placer::print_last_module_name() const
{
    cout<<" ///// Print last module in each row ///// \n";
    for(unsigned i = 0 ; i < _cir->numRows() ; i++)
    {
        if(_rowIdClusterMap[i] != 0)
        {
            int _index = (_rowIdClusterMap[i]->_lastNode.find(i))->second;
            Node* _node = _rowIdClusterMap[i]->_modules[_index];
            cout<<"Row #"<<i<<" has last module named = "<<_node->_module->name()<<endl;
        }
    }
}

//this area is made just to try some functionalities of the code
void Placer::try_area()
{
    for(unsigned i = 0 ; i < _cir->numModules() ; i++)
    {
        if(!_cir->module(cell_order[i]).isStdCell()) continue;

        Module* _cell = &_cir->module(cell_order[i]);
        int rowHeight = (int)(_cell->height()/_cir->rowHeight());
        int rowNum = _cir->y_2_row_id(_cell->y());

        /*
        cout<<"\nCell Name = "<<_cell->name()<<";\nX position = "<<_modPLPos[0][cell_order[i]];
        cout<<"; #Row = "<<rowHeight<<"; width = "<<_cell->width()<<"; rowNum = "<<rowNum<<endl;
        */

        //find previous cluster (with max x position) if one exist
        Cluster* _cluster = 0;
        int maxX = INT_MIN;
        for(int j = 0 ; j < rowHeight ; j++ )
        {
            if(_rowIdClusterMap[rowNum+j] == 0) continue;
            Cluster* _lastClus = _rowIdClusterMap[rowNum+j];
            int index = (_lastClus->_lastNode.find(rowNum+i))->second;
            int rightmost_x = _lastClus->_x_ref + _lastClus->_delta_x[index] + _lastClus->_modules[index]->_module->width();
            /*
            cout<<"_x_ref of cluster = "<<_lastClus->_x_ref<<endl;
            cout<<"***Cell name = "<<_cell->name()<<"***\n";
            cout<<"rightmost_x = "<<rightmost_x<<endl;
            cout<<"_cell->x() = "<<_cell->x()<<endl;
            cin.get();
            */
            if(rightmost_x > maxX )//&& rightmost_x > (_cell->x()) ) 
            {
                _cluster = _lastClus;
            }
        }
        if(_cluster == 0) // new cluster
        {
            _cluster = new Cluster();
            _clusters[_cluster->id] = _cluster;
            AddCell(_cluster, _cell, rowNum, true);
            //cout<<"New Cluster! _x_ref = "<<_cluster->_x_ref<<endl;
        }
        else              // add to _cluster
        {
            AddCell(_cluster, _cell, rowNum, false);
        }
    }

    /*
    for(unsigned i = 0 ; i < _clusters.size() ; i ++)
    {
        print_fanins_fanouts(_clusters[i]);
    } */

    //print_last_module_name();

    for(unsigned i = 0 ; i < _clusters.size() ; i ++)
    {
        RenewPosition(*_clusters[i]);
    }

    cout<<"Cluster number = "<<_clusters.size()<<endl;
}

/////////////////////////////////////////////////
//           Functions for Clusters            //
/////////////////////////////////////////////////

/////////////////TO-DO/////////////
//_firstCell is true if _cell is the first cell of _clus (default value: false)
//1. create node
//2. find previous node in cluster
//3. renew e, q, delta_x, x_ref, (ref_module)
//4. add module to _modules, _lastNode, _cellIdModuleMap
//5. renew _cellIdClusterMap and _rowIdClusterMap
//This Function only adds cells at the end, and shouldn't add cell in the middle or start of the cluster
void Placer::AddCell(Cluster* _clus, Module* _cell, int _rowNum, bool _firstCell)
{
    assert(_cell->isStdCell()); //assert is standard cell (module includes preplaced blocks, I/O pins)
    int rowHeight = (int)(_cell->height()/_cir->rowHeight());
    Node* _newNode = new Node(_cell, rowHeight, _rowNum);
    _clus->_e += _cell->weight();   //numPins()

    //add module to _modules, _cellIdModuleMap
    _clus->_modules.push_back(_newNode);
    _clus->_cellIdModuleMap[_cell->dbId()] = _clus->_modules.size()-1;

    map<int, int>::iterator _iter;

    if(_firstCell)
    {
        assert(_clus->_modules.size() == 1);
        _clus->_ref_module = _newNode;
        _clus->_delta_x.push_back(0);      // delta_x == 0 if module == ref module
        _clus->_q += (_cell->weight())*(_modPLPos[0][_cell->dbId()].x());    //q <- q + e*(x'(i)-delta_x(i))
        for(int i = 0; i < rowHeight ; i++)   
        {
            Cluster* _prevClus = _rowIdClusterMap[_rowNum+i];  //find previous cluster
            if(_prevClus != 0 )       //if do find a cluster
            {
                assert(_prevClus != _clus);         
                int nodeIndex = (_prevClus->_lastNode.find(_rowNum+i))->second;
                Node* _prevNode = _prevClus->_modules[nodeIndex];
                prev_cells[_rowNum+i][_cell->dbId()] = _prevNode->_module->dbId();
            }
        }
    }
    else
    {
        int delta_x = INT_MIN;
        // find previous node in cluster ( set "all"(not just adjacent ones) previous cells to FIs)->already modified 
        vector<double> FI_positions;
        FI_positions.reserve(rowHeight);
        // renew prev_cells at the same time
        for(int i = 0; i < rowHeight ; i++)   
        {
            _iter = _clus->_lastNode.find(_rowNum+i);
            if(_iter != _clus->_lastNode.end())    //find previous node
            {
                Node* _prevNode = _clus->_modules[_iter->second];
                int index = (_rowNum+i)-_prevNode->_rowId;
                assert(index >=0 && index < _prevNode->_degree && _prevNode->getFO(index) == 0);
                _prevNode->setFO(index,_newNode);
                _newNode->setFI(i,_prevNode);
                prev_cells[_rowNum+i][_cell->dbId()] = _prevNode->_module->dbId();
                if(_clus->_delta_x[_iter->second]+_prevNode->_module->width() > delta_x)
                {
                    delta_x = _clus->_delta_x[_iter->second]+_prevNode->_module->width();
                }
                FI_positions.push_back(_clus->_delta_x[_iter->second]+_prevNode->_module->width());
            }
            else    //find previous cell if _clus doesn't have one in rowId = _rowNum+i
            {
                Cluster* _prevClus = _rowIdClusterMap[_rowNum+i];  //find previous cluster
                if(_prevClus != 0 )       //if do find a cluster
                {
                    assert(_prevClus != _clus);        //cluster will not be _clus (or else will not go in here)
                    int nodeIndex = (_prevClus->_lastNode.find(_rowNum+i))->second;
                    Node* _prevNode = _prevClus->_modules[nodeIndex];
                    prev_cells[_rowNum+i][_cell->dbId()] = _prevNode->_module->dbId();
                }
            }
        }
        
        //eliminate non adjacent cells
        for(int i = 0 ; i < rowHeight ; i++)
        {
            if(_newNode->getFI(i) != 0 && delta_x-FI_positions[i] > 0.001)
            {
                assert(rowHeight > 1);
                int index = (_rowNum+i)-_newNode->getFI(i)->_rowId;
                _newNode->getFI(i)->setFO(index,0);
                _newNode->setFI(i,0);
            }
        }

        assert(delta_x != INT_MIN);
        // renew _clus->_delta_x
        _clus->_delta_x.push_back(delta_x);

        // renew q
        _clus->_q += (_cell->weight())*(_modPLPos[0][_cell->dbId()].x()-delta_x);
    }

    // renew _clus->_lastnode and _rowIdClusterMap
    for(int i = 0; i < rowHeight ; i++)   
    {
        // renew _clus->_lastnode
        _iter = _clus->_lastNode.find(_rowNum+i);
        if(_iter != _clus->_lastNode.end())
        {
            _iter->second = _clus->_modules.size()-1;
        }
        else
        {
            _clus->_lastNode[_rowNum+i] = (_clus->_modules.size()-1);
        }
        //renew _rowIdClusterMap
        _rowIdClusterMap[_rowNum+i] = _clus;
    }

    // renew _cellIdClusterMap
    assert(_cellIdClusterMap[_cell->dbId()] == 0);   //assert not exist
    _cellIdClusterMap[_cell->dbId()] = _clus;

    // renew _x_ref
    set_x_to_site(_clus);
}

/////////////////TO-DO/////////////
// _prevCell is the cell in previous cluster which overlaps with _cell
// _prevCell and _cell must belongs to different cluster and should overlap
// will check and possibly modify _rowIdClusterMap and _cellIdClusterMap
//
void Placer::AddCluster(Module* _prevCell, Module* _cell)
{
    Cluster* _prevClus = _cellIdClusterMap[_prevCell->dbId()];
    Cluster* _clus = _cellIdClusterMap[_cell->dbId()];
    assert(_prevClus != 0 && _clus != 0 && _prevClus != _clus);
    //assert(_prevClus->)
}

void Placer::Decluster()
{

}

void Placer::RenewPosition(Cluster &c1) 
{
    for(size_t i = 0 ; i < c1._modules.size() ; i++){
        Point pos(c1._x_ref+c1._delta_x[i],_cir->row_id_2_y(c1._modules[i]->_rowId));
        if(c1._modules[i]->_degree%2)
            move_module_2_pos(*c1._modules[i]->_module,pos,MOVE_FREE);//MOVE_ONSITE);
        else
            move_module_2_pos(*c1._modules[i]->_module,pos,MOVE_FREE); // not finished, need to check
            //need to consider cell flipping when placing odd row height cells
            //Circuit::isRowBottomVss(rowId), Module::IsBottomVss(), Module::setIsBottomVss() may come in handy?
            //use setIsBottomVss() before IsBottomVss()
            //maybe we should just modify function move_module_2_pos?
    }
}

double Placer::RenewCost(Cluster &c1)   
{
    c1._cost = 0; // already had a data member "cost" in class cluster, we can just store cost in there
    for(size_t i = 0 ; i < c1._modules.size() ; i++){
        c1._cost += abs(c1._modules[i]->_module->x()-_modPLPos[0][c1._modules[i]->_module->dbId()].x()) + abs(c1._modules[i]->_module->y()-_modPLPos[0][c1._modules[i]->_module->dbId()].y());
        //since renewCost is used when placeRow(trial) (to determine cost of placing in different rows)
        //it's maybe better to use manhattan distance instead of quadratic distance to minimize displacement
        //(because displacment is measured in manhattan distance when evaluating)
        //(also should take displacement in y into account)
    }
    return c1._cost;
}

Cluster* Placer::Collapse()
{
    return 0;
}

vector<int> Placer::CheckOverlap()
{
    vector<int> _modulesIndex;
    return _modulesIndex;
}

void Placer::set_x_to_site(Cluster* _clus)
{
    double x = (_clus->_q/_clus->_e);
    _clus->_x_ref = _cir->g_x_on_site(x, 0, Circuit::ALIGN_HERE);
}