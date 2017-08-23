#ifndef PLACER_H
#define PLACER_H

#include <vector>
#include <string>
#include <cassert>
#include <map>
using namespace std;

#include "../circuit/circuit.h"
#include "../common/paramhandler.h"
#include "../common/util.h"
#include "../parser/parser.h"
#include "node.h"

using namespace util;

class Cluster
{
    friend class Placer;
public:
    Cluster(int fence_id = -1):id(global_id), _e(0), _q(0), _cost(0), _fence_id(fence_id) { global_id++; }
    Cluster(const Cluster& n){
        id = n.id;
        _ref_module = n._ref_module;
        _x_ref = n._x_ref;
        _e = n._e;
        _q = n._q;
        _delta_x = n._delta_x;
        _cost = n._cost;
        _fence_id = n._fence_id;
        //_cellIdModuleMap = n._cellIdModuleMap;
        _modules = n._modules;
    }
    ~Cluster() {}
    void sync_all_id()
    {
        for(unsigned i = 0 ; i < _modules.size() ; i++) { _modules[i]->_module->sync_id();  }
    }
    //int round_x_to_site(double x_in, Row* _row);    //since q/e is usually not on site, this function round x into site
private:

    unsigned id;                //a unique id for all clusters
    static unsigned global_id;  //use to assign id for clusters
    Node* _ref_module;        
    double _x_ref;              //position of _ref_module (_q/_e)
    double _e;                  //cluster weight
    double _q;                  //cluster q
    vector<int> _delta_x;       //delta position of _modules[i] to ref (same index as in _modules)
    double _cost;               //stored cost
    int _fence_id;               //fence region

    vector<Node*> _modules;     //all Modules 
};

class Placer
{
public:
    Placer(Circuit &inCir): _cir(&inCir), _modPLPos(0), _fence_id(-1) {
        _modPLPos.resize( 3, vector<Point>( _cir->numModules() ) );
        _rowIdModuleMap.resize(_cir->numRows(),0);
        _cellIdClusterMap.resize(_cir->numModules(),0);
        prev_cells.resize(_cir->numRows());
        next_cells.resize(_cir->numRows());
        _intervals.resize(_cir->numRows());
        for(unsigned i = 0 ; i < _cir->numRows() ; i++)
        {
            prev_cells[i].resize(_cir->numModules(),-1);
            next_cells[i].resize(_cir->numModules(),-1);
        }
        save_modules_2_pos(PL_INIT);
        save_modules_2_pos(PL_BEST);
        save_modules_2_pos(PL_LAST);
    }

    //void place();
    /////////////////////////////////////////////
    // Clear and Initialize (in placer 2)
    /////////////////////////////////////////////
    void clear();
    void clear_fail();
    void init_fence(int fence_id);

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    double compute_hpwl();
    enum PL_TYPE { PL_INIT=0, PL_LAST, PL_BEST };
    double compute_displacement(const PL_TYPE &pt);
    double max_displacement(const PL_TYPE &pt);
    double raw_score(double global_hpwl);
    const Point &mod_2_pos( Module &mod, const PL_TYPE &pt ){ return _modPLPos[pt][ mod.dbId() ]; }

    /////////////////////////////////////////////
    // operation
    /////////////////////////////////////////////
    enum MOVE_TYPE { MOVE_ONSITE, MOVE_FREE };
    bool move_module_2_pos(Module &mod, const Point &pos, MOVE_TYPE mt = MOVE_FREE);
    bool move_module_center_2_pos(Module &mod, const Point &pos, MOVE_TYPE mt = MOVE_FREE);
    void save_modules_2_pos(const PL_TYPE &pl);
    void bound_pos_in_max_disp(Module &mod, Point &pos, bool isOnSite=true, const double &dispRatio=1.0);
    bool is_pos_in_max_disp(Module &mod, const Point &pos, const double &dispRatio=1.0);
    void move_pl_pos(const Point &shift, const double &scale , bool isBack=false);

    /////////////////////////////////////////////
    // newly added operation (iccad'17)
    /////////////////////////////////////////////
    int find_valid_row(Module &mod);    //nearest row without vialation of P/G alignment
    void place_valid_site(Module &mod); //nearest site without vialation of P/G alignment 
    void place_all_mods_to_site();
    void sort_cells();

    /////////////////////////////////////////////
    // Debug Use Functions
    /////////////////////////////////////////////
    void print_cell_order() const;
    //void print_fanins_fanouts(Cluster* _clus) const;
    void print_delta_x(Cluster* _clus) const;
    void print_last_module_name() const;
    void print_intervals() const;
    bool check_cluster_internal_overlap(Cluster* _clus);
    bool check_all(int to_index) const;
    bool check_PG() const;

    bool legalize();
    void legalize_all();
    //void try_area2();

    /////////////////////////////////////////////////
    //             Operating Functions             //
    /////////////////////////////////////////////////

    //change return type and input variables if neccessary
    void AddCell(Cluster* &_clus, Module* _cell, int _rowNum, int maxX);
    Cluster* AddCluster(Module* _prevCell, Module* _cell, bool _clus2prevClus = false);
    void Decluster(Cluster* _clus);
    bool Decluster_Cell(Cluster* _clus, int index, double xPos);

    bool reduce_DeadSpace(Module* _cell, int _rowNum);

    void RenewPosition(Cluster &c1);
    double RenewCost(Cluster &c1);         //return new cost
    Cluster* Collapse(Cluster* _clus, bool check = false);
    Cluster* Collapse_right(Cluster* _clus, bool check = false);
    pair<int,int> CheckOverlap(Cluster* _clus) ; //return pair of index (_modules[index]) overlapping with other cells 
    pair<int,int> CheckOverlap_right(Cluster* _clus);
    void set_x_to_site(Cluster* _clus);

    //below functions are in placer_2.cpp
    double Multi_PlaceRow(Module* _cell, int rowHeight, int rowNum);
    double Multi_PlaceRow_trial(Module* _cell, int rowHeight, int rowNum);

    //return false if unable to place
    bool AddCell_trial(Cluster* _clus, Module* _cell, int _rowNum, int maxX ,Node* &_todelete); 
    Cluster* AddCluster_trial(Module* _prevCell, Module* _cell, Cluster* _clus);
    Cluster* AddCluster_trial_right(Module* _prevCell, Module* _cell, Cluster* _clus);

    double reduce_DeadSpace_trial(Module* _cell, int _rowNum, double _alpha);

    Cluster* Collapse_trial(Cluster* _clus);
    Cluster* Collapse_trial_right(Cluster* _clus);
    pair<int,int> CheckOverlap_trial(Cluster* _clus);
    pair<int,int> CheckOverlap_trial_right(Cluster* _clus);

    bool Is_Cluster_Block_Overlap(Cluster* _clus,bool output = false) const;
    bool Is_Interval_Block_Overlap(pair<double,double> _interval, int _rowNum,bool output = false) const;
    double find_utilization();

    /////////////////////////////////////////////
    // test funcitons for fregions
    /////////////////////////////////////////////
    /*
    bool Is_Cluster_Block_Overlap_fence(Cluster* _clus);
    double Multi_PlaceRow_trial_fence(Module* _cell, int rowHeight, int rowNum);
    void fence_place(Fregion& _fregion);
    void set_intervals_for_fregion(Fregion& _fregion, double& top, double& bot);
    void fence(){}
    void sort_cells_fence(Fregion& _fregion);
    int leftbound(int x, int rowNum);
    int rightbound(int x, int rowNum);
    double reduce_DeadSpace_trial_fence(Module* _cell, int _rowNum, double _alpha);
    void set_x_to_site_fence(Cluster* _clus);
    */
    void set_intervals();
    void addBlockedInterval(double lBlk, double rBlk, unsigned rowNum);
    double get_valid_pos(Module* _module, int _rowId);

    void Renew_All_Position();

    //bool reduce_DeadSpace_double(Module* _cell, int _rowNum);
    bool check_interval_second_row(Module* _cell, int _rowNum, int _degree, int count, pair<int,int> inter);
    bool check_interval_second_row_trial(Module* _cell, int _rowNum, int _degree, int count , pair<int,int> inter, double& cost, double _alpha);

    double reduce_DeadSpace_Multi_trial(Module* _cell, int _rowNum, int _degree, double _alpha, double& cost);
    bool reduce_DeadSpace_Multi(Module* _cell, int _rowNum, int _degree, int count, double _x);


    /////////////////////////////////////////////
    // Legalization methods for insurance (in placer_insure.cpp)
    /////////////////////////////////////////////
    void legalize_dumb();
    double Multi_PlaceRow_trial_dumb(Module* _cell, int rowNum, const vector<vector<double> >& _rightBound);

    bool legalize_very_dumb();
    bool Multi_PlaceRow_very_dumb(Module* _cell, int rowNum, vector<vector<double> >& _rightBound);

    bool legalize_slight_dumb(bool _forcePlace = false);
    double Multi_PlaceRow_trial_slight_dumb(Module* _cell, int rowNum, const vector<vector<double> >& _rightBound);

    bool find_placeable(Module* _cell, int rowNum, const vector<vector<double> >& _rightBound, vector<pair<double,double> >& _placeables, bool debugMode =false);

    Circuit &cir() {return *_cir;}

private:
    Placer(): _cir(NULL){}

    /////////////////////////////////////////////
    // design data
    /////////////////////////////////////////////
    Circuit *_cir;
    vector< vector<Point> > _modPLPos;      // init, last, best (save poitions of modules)
    //vector<Point> _modInitPos, _modLastPos, _modBestPos;

    vector<int> cell_order;                 // used as legalization order ( _cir->module(cell_order[0]) : first cell )
    vector<Module*> _rowIdModuleMap;        // store the last module in every row
    vector<Cluster*> _cellIdClusterMap;     // use to store cell cluster mapping (index = dbId())
    //vector< map<int,int> > prev_cells;    // use this to detect nearby previous cells (this cell id to prev cell id)
    //vector< map<int,int> > next_cells;    // use this to detect nearby previous cells (this cell id to prev cell id)
    vector< vector<int> > prev_cells;       // try to store it in static array to reduce time
    vector< vector<int> > next_cells;       // empty if value == -1

    vector< vector< pair<double,double> > > _intervals;   // for fregions (first index : rows ; second index : intervals)
    int _fence_id;                          // fence region id the placer is currently placing
    double _utilization;                    // density

    map<int, Cluster*> _clusters;           // store all clusters
};

#endif // PLACEMENT_H
