#include <algorithm>

#include "circuit.h"

Circuit::Circuit()
{
    _name = "unkown";
    _rectangleChip.setBounds(0,0,0,0);
    _numComponents = 0;

    clearBlockages();
    clearFregions();
    clearLayers();
    clearModules();
    clearNets();
    clearPins();
    clearRows();
    clearMasters();
    clearMasterPins();
}

void Circuit::move_placement(const Point &shift, const double &scale, bool isBack)
{
    _rectangleChip.shift_and_scale_me( shift, scale, isBack );

    for( size_t i=0; i<numBlockages(); ++i ){
        blockage(i).shift_and_scale_me( shift, scale, isBack );
    }

    for( size_t i=0; i<numFregions(); ++i ){
        for( size_t j=0; j<fregion(i).numRects(); ++j ){
            fregion(i).rect(j).shift_and_scale_me( shift, scale, isBack );
        }
    }

    for( size_t i=0; i<numLayers(); ++i ){
        layer(i).shift_and_scale( shift, scale, isBack );
    }

    for( size_t i=0; i<numModules(); ++i ){
        module(i).shift_and_scale( shift, scale, isBack );
    }

    for( size_t i=0; i<numRows(); ++i ){
        row(i).shift_and_scale( shift, scale, isBack );
    }
    Row::site.setHeight( Row::site.height() * scale );
    Row::site.setStep( Row::site.step() * scale );
    Row::site.setWidth( Row::site.width() * scale );
}

bool Circuit::addLayer(const unsigned &layerId){
    if( layerId >= _layers.size() )     { _layers.resize( layerId+1 ); return true; }
    else return false;
}

void Circuit::clearDesign()
{
    clearBlockages();
    clearFregions();
    clearLayers();
    clearModules();
    clearNets();
    clearPins();
    clearRows();
    //clearMasters();
    //clearMasterPins();
    clearSeqElements();
}

void Circuit::createMasterNameMap()
{
    _mastersNameMap.clear();
    for( size_t i=0; i<_masters.size(); ++i )
        _mastersNameMap[ _masters[i].name() ] = i;
}

void Circuit::createModuleNameMap()
{
    _modulesNameMap.clear();
    for( size_t i=0; i<_modules.size(); ++i )
        _modulesNameMap[ _modules[i].name() ] = i;
}

void Circuit::createPinNameMap()
{
    _pinsNameMap.clear();
    for( size_t i=0; i<_pins.size(); ++i )
        _pinsNameMap[ _pins[i].name() ] = i;
}

void Circuit::createNetNameMap()
{
    _netsNameMap.clear();
    for( size_t i=0; i<_nets.size(); ++i )
        _netsNameMap[ _nets[i].name() ] = i;
}

unsigned Circuit::findMasterIdByName(const string &name)
{
    map<string, unsigned>::const_iterator ite = _mastersNameMap.find( name );
    if( ite==_mastersNameMap.end() ){
        cerr << "[ERROR] Cannot find " << name << " in .lef" << endl;
        exit(1);
    }
    return ite->second;
}

unsigned Circuit::findModuleIdByName(const string &name)
{
    map<string, unsigned>::const_iterator ite = _modulesNameMap.find( name );
    if( ite==_modulesNameMap.end() ){
        cerr << "[ERROR] Cannot find module " << name << " in .def" << endl;
        exit(1);
    }
    return ite->second;
}

unsigned Circuit::findPinIdByName(const string &name)
{
    map<string, unsigned>::const_iterator ite = _pinsNameMap.find( name );
    if( ite==_pinsNameMap.end() ){
        cerr << "[ERROR] Cannot find pin " << name << " in .def" << endl;
        exit(1);
    }
    return ite->second;
}

unsigned Circuit::findNetIdByName(const string &name)
{
    map<string, unsigned>::const_iterator ite = _netsNameMap.find( name );
    if( ite==_netsNameMap.end() ){
        cerr << "[ERROR] Cannot find net " << name << " in .def/.v" << endl;
        exit(1);
    }
    return ite->second;
}

void Circuit::addPins4EachMaster()
{
    for( size_t i=0; i<_masterPins.size(); ++i ){
        Pin &pin = _masterPins[i];
        Module &master = _masters[ pin.moduleId() ];
        master.addPin( &pin );
    }
}

void Circuit::addPins4EachModule()
{
    for( size_t i=0; i<_pins.size(); ++i ){
        Pin &pin = _pins[i];
        Module &mod = _modules[ pin.moduleId() ];
        mod.addPin( &pin );
    }

    for( size_t i=0; i<_modules.size(); ++i ){
        Module &mod = _modules[i];
        mod.creatPinNameMap();
        mod.setPosition( mod.x(), mod.y() );
        mod.setIsBottomVss();
    }
}

void Circuit::identify_FFs_PIOs(const string &clkPortName )
{
    _PIs.clear(); _POs.clear(); _DFFs.clear();

    for( size_t i=0; i<_modules.size(); ++i ){
        Module &mod = _modules[i];
        if( mod.isIOPin() ){
            Pin &pin = mod.pin(0);
            if( pin.dir() == PinDir::INPUT )
                _PIs.push_back( &mod );
            else if( pin.dir() == PinDir::OUTPUT )
                _POs.push_back( &mod );
        }else{
            for( size_t j=0; j<mod.numPins(); ++j ){
                Pin &pin = mod.pin(j);
                string pinName = pin.name().substr( pin.name().find(":")+1 );
                if( pinName == clkPortName ){
                    _DFFs.push_back( &mod );
                    mod.setIsDFF(true);
                    break;
                }
            }
        }
    }
}

void Circuit::sortRows()
{
    sort( _rows.begin(), _rows.end(), Row::lesser );
}

void Circuit::remove_sites(){
    remove_sites( _rows );
}

void Circuit::remove_sites(vector<Row> &rows)
{
    remove_sites_fixed_block( rows );
    remove_sites_blockage( rows );
    fix_sites_by_step( rows );
}

void Circuit::remove_sites_fixed_block(vector<Row> &rows)
{
    for( size_t i=0; i<_modules.size(); ++i ){
        Module &mod = _modules[i];
        if( mod.width() == 0 || !mod.isFixed() ) { continue; }
        for( size_t j=0; j<mod.numRects(); ++j ){
            Rect rect = mod.rect(j);
            remove_sites_rect( rows, rect );
        }
    }
}

void Circuit::remove_sites_blockage(vector<Row> &rows)
{
    for( size_t i=0; i<_blockages.size(); ++i ){
        remove_sites_rect( rows, _blockages[i] );
    }
}

void Circuit::remove_sites_fence_region(vector<Row> &rows)
{
    for( size_t i=0; i<_fregions.size(); ++i ){
        Fregion &fr = _fregions[i];
        for( size_t j=0; j<fr.numRects(); ++j ){
            Rect rect = fr.rect(j);
            remove_sites_rect( rows, rect );
        }
    }
}

void Circuit::remove_sites_rect(vector<Row> &rows, const Rect &rect)
{
    size_t lowRowId = y_2_row_id( rect.bottom() );
    size_t topRowId = y_top_2_row_id( rect.top() );

    for( size_t i=lowRowId; i<=topRowId; ++i ){
        Row &row = rows[i];
        row.addBlockedInterval( rect.left(), rect.right() );
    }
}

void Circuit::fix_sites_by_step(vector<Row> &rows)
{
    double step = Row::site.step();
    for( size_t i=0; i<rows.size(); ++i ){
        Row &row = rows[i];
        for( size_t j=0; j<row.numInterval(); ++j ){
            double &lInt = row.interval(j).first, &rInt = row.interval(j).second;
            double oldWidth = rInt - lInt;
            if( oldWidth < step ){ row.eraseInterval(j); --j; continue; }

            double lIntNew = g_x_on_site( lInt, i, ALIGN_RIGHT ), rIntNew = g_x_on_site( rInt, i, ALIGN_LEFT );
            double newWidth = rIntNew - lIntNew;
            if( newWidth > 0 ){ lInt = lIntNew; rInt = rIntNew; }
            else{
                row.eraseInterval(j); --j;
                if( newWidth < 0 ){
                    cout << "[ERROR] Circuit::fix_sites_by_step :: fix interval to negative width..." << endl;
                    getchar();
                }
            }
        }
    }
}

bool Circuit::check_all_std_cells_on_row_site()
{
    bool isStdOnRowSite = true;
    for( size_t i=0; i<numComponents(); ++i ){
        Module &mod = comp(i);
        if( !mod.isStdCell() ) { continue; }

        int rowId = y_2_row_id( mod.y() );
        bool isModOnRow = is_equal( mod.y(), row_id_2_y( rowId ) );
        if( !isModOnRow ){
            cout << "[ERROR] RowPlacer::checkStdCellsOnRowSite : mod " << mod.name() << " not on the rowSite." << endl;
            mod.showInfo();
            isStdOnRowSite = false;
            break;
        }

        double xSiteStep = g_x_on_site( mod.x(), rowId, ALIGN_HERE );
        bool isModOnSiteStep = is_equal( mod.x(), xSiteStep );
        if( !isModOnSiteStep ){
            cout << "[ERROR] RowPlacer::checkStdCellsOnRowSite : mod " << mod.name() << " not on the siteStep." << endl;
            mod.showInfo();
            isStdOnRowSite = false;
            break;
        }
    }

    return isStdOnRowSite;
}

void Circuit::showInfo()
{
    // netlist analysis
    size_t pin2 = 0, pin3 = 0, pin10 = 0, pin100 = 0;
    for( size_t i=0; i<numNets(); ++i ){
        if( net(i).numPins() == 2 )         { ++pin2; }
        else if( net(i).numPins() < 10 )    { ++pin3; }
        else if( net(i).numPins() < 100 )   { ++pin10; }
        else                                { ++pin100; }
    }

    // floating pins
    size_t numFloatingPins = 0;
    for( size_t i=0; i<numPins(); ++i ){
        Pin &curPin = pin(i);
        if( curPin.netId()==MAX_UNSIGNED ){ ++numFloatingPins; }
        curPin.setIsInNetlist();
    }

    cout << endl;
    cout << "====================  CIRCUIT INFORMATION =====================\n";
    cout << " Core Region    : " << chipRect() << endl;
    cout << " Row Height     : " << rowHeight() << endl;
    cout << " Row Step       : " << Row::site.step() << endl;
    if( param.maxDisp>0 )
    cout << " Max Distance   : " << param.maxDisp << endl;
    cout << " # Layers       : " << numLayers() << endl;
    cout << " # Rows         : " << numRows() << endl;
    cout << " # Fence Regions: " << numFregions() << endl;
    cout << " # Masters      : " << numMasters() << endl;
    cout << " # Components   : " << numComponents() << endl;
    cout << "   FF counts    : " << numDFFs() << endl;
    cout << "   PI counts    : " << numPIs() << endl;
    cout << "   PO counts    : " << numPOs() << endl;
    cout << " # Pins         : " << numPins() << " = " << numPins()-numFloatingPins << " + " << numFloatingPins << "(floating)" << endl;
    cout << " # Nets         : " << numNets() << endl;
    cout << "   Pin counts   : 2 ("<<pin2<<") 3-10 ("<<pin3<<") 11-100 ("<<pin10<<") 100- ("<<pin100<<")" <<endl;
    cout << "===============================================================\n";
    cout << endl;
}


///////////////////////////////////////////////////////////////
//                Newly Added Functions (ICCAD'17)           //
///////////////////////////////////////////////////////////////

void Circuit::createSNetIndexVec()
{
    _SNetIndexVec.clear();
    Layer& _metal1 = _layers[0];
    for(unsigned i = 1 ; i < _layers.size() ; i++) { if(_layers[i].name()=="metal1") { _metal1 = _layers[i]; } }

    assert(_metal1.numRails() == numRows()-1);
    _SNetIndexVec.resize(_metal1.numRails(),"");
    for(unsigned i = 0 ; i < _metal1.numRails() ; ++i )
    {
        unsigned index = (unsigned)(0.5*(_metal1.rail(i).second.top()+_metal1.rail(i).second.bottom()))/rowHeight()-1;
        assert(index < (numRows()-1));
        _SNetIndexVec[index] = _metal1.rail(i).first;
    }
}

bool Circuit::isRowBottomVss(const unsigned& _rowId)
{
    if(_rowId == 0)
    {
        if(_SNetIndexVec[_rowId]=="vss") return false;
        else if(_SNetIndexVec[_rowId]=="vdd")return true;
    }
    else
    {
        if(_SNetIndexVec[_rowId-1]=="vss") return true;
        else if(_SNetIndexVec[_rowId-1]=="vdd") return false;
        assert(false);
    }
    return false;
}

void Circuit::outputGnuplotFigure(string filePathName)
{
    GnuplotPlotter plotter;

    plotter.setTitle("placement figure");

    // add rectangles of moudles
    for (unsigned i = 0; i < _modules.size(); i++) {
        Module &module = _modules[i];
        plotter.addRectangle(Rect(module.x(),module.y(),module.x()+module.width(),module.y()+module.height()));
    }

    // add rectangle of placement core reigon
    plotter.addRectangle(_rectangleChip);

    plotter.outputPlotFile(filePathName);
}