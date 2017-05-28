#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>
#include <string>
#include <map>
using namespace std;

#include "layer.h"
#include "module.h"
#include "net.h"
#include "pin.h"
#include "row.h"
#include "fregion.h"
#include "../common/paramhandler.h"
#include "../common/util.h"
#include "../common/GnuplotPlotter.h"

class Circuit
{
  public:
    Circuit();

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    string name() {return _name;}
    const Rect &chipRect() {return _rectangleChip;}  //Chip rectangle

    /////////////////////////////////////////////
    // get design objects/properties
    /////////////////////////////////////////////
    Rect &blockage(unsigned blockageId) {return _blockages[blockageId];}
    Fregion &fregion(unsigned fregionId) {return _fregions[fregionId];}
    Layer& layer(unsigned layerId) {return _layers[layerId]; }
    Module& module(unsigned moduleId) {return _modules[moduleId];}
    Module& comp(unsigned compId) {return _modules[compId];}
    Module& modMaster( Module &mod ) {return _masters[ mod.masterId() ];}
    Module& pinMod( Pin &pin ) {return _modules[ pin.moduleId() ];}
    Net& net(unsigned netId) {return _nets[netId];}
    Pin& pin(unsigned pinId) {return _pins[pinId];}
    Row& row(unsigned rowId) {return _rows[rowId];}
    Module& master(unsigned masterId) {return _masters[masterId];}
    Pin& masterPin(unsigned masterPinId) {return _masterPins[masterPinId];}
    double siteStep() {return Row::site.step();}
    Module& DFF(unsigned DFFId) {return *_DFFs[DFFId];}

    unsigned numBlockages() {return _blockages.size();}
    unsigned numFregions() {return _fregions.size();}
    unsigned numLayers() {return _layers.size(); }
    unsigned numModules() {return _modules.size();}
    unsigned numComponents() {return _numComponents;}
    unsigned numNets() {return _nets.size();}
    unsigned numPins() {return _pins.size();}
    unsigned numRows() {return _rows.size();}
    unsigned numMasters() {return _masters.size();}
    unsigned numMasterPins() {return _masterPins.size();}
    unsigned numPIs() {return _PIs.size();}
    unsigned numPOs() {return _POs.size();}
    unsigned numDFFs() {return _DFFs.size();}

    double rowHeight() {return Row::site.height();}

    /////////////////////////////////////////////
    // methods for design construction
    /////////////////////////////////////////////
    void move_placement(const Point &shift, const double &scale , bool isBack = false);
    void addBlockage(const Rect &blockage)  {_blockages.push_back(blockage);}
    void addFregion(const Fregion &fregion) {_fregions.push_back(fregion);}
    bool addLayer(const unsigned &layerId);
    void addModule(const Module &module) {_modules.push_back(module); _modules.back().setDbId(_modules.size()-1);}
    void addNet(const Net &net) {_nets.push_back(net);}
    void addPin(const Pin &pin) {_pins.push_back(pin),_pins.back().setPinId(_pins.size());}
    void addRow(const Row &row) {_rows.push_back(row);}
    void addMaster(const Module &master) {_masters.push_back(master);}
    void addMasterPin(const Pin &masterPin) {_masterPins.push_back(masterPin),_masterPins.back().setPinId(_masterPins.size());}

    void setNumComponents(unsigned size) {_numComponents = size;}

    void clearDesign();
    void clearBlockages() {_blockages.clear();}
    void clearFregions() {_fregions.clear();}
    void clearLayers() {_layers.clear();}
    void clearModules() {_modules.clear(); _DFFs.clear(); _PIs.clear(); _POs.clear();}
    void clearNets() {_nets.clear();}
    void clearPins() {_pins.clear();}
    void clearRows() {_rows.clear();}
    void clearMasters() {_masters.clear();}
    void clearMasterPins() {_masterPins.clear();}
    void clearSeqElements() {_PIs.clear(); _POs.clear(); _DFFs.clear(); }

    // initialize pins for modules and nets (construct hypergraph)
    //void connectPinsWithModulesAndNets();
    void createMasterNameMap();
    void createModuleNameMap();
    void createPinNameMap();
    void createNetNameMap();
    unsigned findMasterIdByName(const string &name);
    unsigned findModuleIdByName(const string &name);
    unsigned findPinIdByName(const string &name);
    unsigned findNetIdByName(const string &name);
    void addPins4EachMaster();
    void addPins4EachModule();
    void identify_FFs_PIOs(const string &clkPortName);
    void sortRows();
    void remove_sites();
    void remove_sites( vector<Row> &rows );
    void remove_sites_fixed_block( vector<Row> &rows ); // to-do (expand blocks)
    void remove_sites_blockage( vector<Row> &rows );
    void remove_sites_fence_region( vector<Row> &rows );
    void remove_sites_rect( vector<Row> &rows, const Rect &rect );
    void fix_sites_by_step( vector<Row> &rows );
    void setName(const string &name){ _name = name; }
    void setRectChip( Rect rectChip ) { _rectangleChip = rectChip; }

    // common utilities for ntuplace
    inline bool isInMaxDisp(const Point &posOri, const Point &pos);
    inline void rect2RowIds(const Rect &rect, int &idBottom, int &idTop);
    inline int y_2_row_id(const double &y);
    inline int y_top_2_row_id(const double &y);
    inline double row_id_2_y(const int &rowId);
    inline int x_diff_2_numSites(const double &x1, const double &x2, const int &rowId);
    inline void set_x_y_on_site(Point &pos);

    enum AlignDir { ALIGN_HERE, ALIGN_RIGHT, ALIGN_LEFT };
    inline double g_x_on_site(const double &x, const int &rowId, AlignDir aDir);

    bool check_all_std_cells_on_row_site();
    
    void showInfo();
    inline void print_masters();  
    inline void print_rows(); 
    inline void print_layers();
    inline void print_nets();
    inline void print_modules();
    inline void print_fregions();

    ///////////////////////////////////////////////////////////////
    //                Newly Added Functions (ICCAD'17)           //
    ///////////////////////////////////////////////////////////////
    void createSNetIndexVec();  //mapping from row index to "vdd" or "vss"
    bool isRowBottomVss(const unsigned& _rowId);
    void outputGnuplotFigure(string filePathName);

private:
    /////////////////////////////////////////////
    // design statistics
    /////////////////////////////////////////////
    string _name;
    Rect _rectangleChip;

    /////////////////////////////////////////////
    // design data
    /////////////////////////////////////////////
    vector<Rect> _blockages;
    vector<Fregion> _fregions;
    vector<Layer> _layers;
    vector<Module> _modules;
    vector<Net> _nets;
    vector<Pin> _pins;
    vector<Row> _rows;
    unsigned _numComponents; // not including fixed pins

      // from .lef
    vector<Module> _masters;
    vector<Pin> _masterPins;
    map<string,unsigned> _mastersNameMap;
    map<string,unsigned> _modulesNameMap;
    map<string,unsigned> _pinsNameMap;

      // from .v or .def
    map<string,unsigned> _netsNameMap;
    vector<string> _SNetIndexVec;

      // for timing
    vector<Module*> _DFFs;
    vector<Module*> _POs;
    vector<Module*> _PIs;
};

void Circuit::print_fregions()
{
    cout<<"Print Fence Regions...\n";
    for(unsigned i = 0 ; i < _fregions.size() ; i++)
    {
        _fregions[i].showInfo();
    }
}

void Circuit::print_modules()
{
    cout<<"Print Modules...\n";
    for(unsigned i = 0 ; i < _modules.size() ; i++)
    {
         _modules[i].showInfo();
         //_modules[i].showPins();
        //cout<<"Names = "<<_modules[i].name()<<endl;
    }
}

void Circuit::print_nets()
{
    cout<<"Print nets...\n";
    for(unsigned i = 0 ; i < _nets.size() ; i++)
    {
        _nets[i].showInfo();
    }
}

void Circuit::print_layers()
{
    cout<<"Print Layers...\n";
    for(unsigned i = 0 ; i < _layers.size() ; i++)
    {
        _layers[i].showInfo();
    }
}

void Circuit::print_rows()
{
    cout<<"Print Rows...\n";
    for(unsigned i = 0 ; i < _rows.size() ; i++)
    {
        _rows[i].showInfo();
    }
}

void Circuit::print_masters() 
{
    cout<<"Print Modules...\n";
    for(unsigned i = 0 ; i < _masters.size() ; i++)
    {
        _masters[i].showInfo(); 
        _masters[i].showPins();
    }
}

bool Circuit::isInMaxDisp(const Point &posOri, const Point &pos)
{
    double xDiff = pos.x()-posOri.x(), yDiff = pos.y()-posOri.y();
    double displacement = fabs( xDiff ) + fabs( yDiff );
    double norm_movement = displacement/rowHeight();
    //double displacement = fabs((pos.y()-posOri.y())/rowHeight());

    return norm_movement <= param.maxDisp;
}

void Circuit::rect2RowIds(const Rect &rect, int &idBottom, int &idTop)
{
    idBottom = y_2_row_id( rect.bottom() );
    idTop = y_top_2_row_id( rect.top() );

    assert( idBottom < int(_rows.size()) );
    assert( idTop < int(_rows.size()) );
}

int Circuit::y_2_row_id(const double &y)
{
    double yDiff = y-row(0).y();

    int ySiteId = floor( yDiff / rowHeight() );

    ySiteId = min( max( 0, ySiteId ), int( numRows() ) );

    return ySiteId;
}

int Circuit::y_top_2_row_id(const double &y)
{
    int ySiteId = y_2_row_id(y);

    if( is_equal( row(0).y() + ySiteId*rowHeight(), y ) )
        --ySiteId;

    return ySiteId;
}

double Circuit::row_id_2_y(const int &rowId)
{
    return row(0).y() + rowId*rowHeight();
}

int Circuit::x_diff_2_numSites(const double &x1, const double &x2, const int &rowId)
{
    Row &cRow = row(rowId);

    int retNum = floor( ( x2 - cRow.x() ) / siteStep() ) - ceil( ( x1 - cRow.x() ) / siteStep() );

    assert( retNum>=0 );

    return retNum;
}

void Circuit::set_x_y_on_site(Point &pos)
{
    size_t rowId = y_2_row_id( pos.y() );
    double legalX = g_x_on_site( pos.x(), rowId, ALIGN_HERE );
    double legalY = row_id_2_y( rowId );
    pos.set_x_y( legalX, legalY );
}

double Circuit::g_x_on_site(const double &x, const int &rowId, AlignDir aDir)
{
    Row &cRow = row(rowId);
    double rowRight = cRow.right();

    double curX = x;
    curX = max( cRow.x(), min( rowRight, curX ) );

    double numStep = -1;
    if( aDir == ALIGN_HERE )
        numStep = round( (curX - cRow.x()) / siteStep() );
    else if( aDir == ALIGN_LEFT )
        numStep = floor( (curX - cRow.x()) / siteStep() );
    else if( aDir == ALIGN_RIGHT )
        numStep = ceil( (curX - cRow.x()) / siteStep() );

    return numStep * siteStep() + cRow.x();
}

#endif
