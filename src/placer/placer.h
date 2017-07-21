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
    Cluster():id(global_id), _e(0), _q(0), _cost(0), dead(false){ global_id++; }
    Cluster(const Cluster& n){
        id = n.id;
        _ref_module = n._ref_module;
        _x_ref = n._x_ref;
        _e = n._e;
        _q = n._q;
        _delta_x = n._delta_x;
        _cost = n._cost;
        _lastNode = n._lastNode;
        _cellIdModuleMap = n._cellIdModuleMap;
        _modules = n._modules;
    }
    ~Cluster() {}
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
    bool dead;

    map<int,int> _lastNode;     //last node in each row (first: rowId, second: index in _modules)
    map<int,int> _cellIdModuleMap;// mapping between cell id and _modules (first: cellId, second: index in _modules)
    vector<Node*> _modules;     //all Modules 
};

class Placer
{
public:
    Placer(Circuit &inCir): _cir(&inCir), _modPLPos(0) {
        _modPLPos.resize( 3, vector<Point>( _cir->numModules() ) );
        _rowIdClusterMap.resize(_cir->numRows(),0);
        _cellIdClusterMap.resize(_cir->numModules(),0);
        prev_cells.resize(_cir->numRows());
        next_cells.resize(_cir->numRows());
        for(unsigned i = 0 ; i < _cir->numRows() ; i++)
        {
            prev_cells[i].resize(_cir->numModules(),-1);
            next_cells[i].resize(_cir->numModules(),-1);
        }
        save_modules_2_pos(PL_INIT);
        save_modules_2_pos(PL_BEST);
        save_modules_2_pos(PL_LAST);
    }
    ~Placer()
    {
        for(unsigned i = 0 ; i < _cir->numRows() ; i++)
        {
            prev_cells[i].clear();
            next_cells[i].clear();
        }
        prev_cells.clear();
        next_cells.clear();
    }

    //void place();

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    double compute_hpwl();
    enum PL_TYPE { PL_INIT=0, PL_LAST, PL_BEST };
    double compute_displacement(const PL_TYPE &pt);
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
    bool check_cluster_internal_overlap(Cluster* _clus);
    bool check_all(int to_index) const;

    void try_area();
    void try_area2();

    /////////////////////////////////////////////
    // test funcitons for fregions
    /////////////////////////////////////////////
    bool Is_Cluster_Block_Overlap_fence(Cluster* _clus);
    double Multi_PlaceRow_trial_fence(Module* _cell, int rowHeight, int rowNum);
    void fence_place(Fregion& _fregion);
    void set_intervals_for_fregion(Fregion& _fregion, double& top, double& bot);
    void fence();
    void sort_cells_fence(Fregion& _fregion);
    int leftbound(int x, int rowNum);
    int rightbound(int x, int rowNum);
    double reduce_DeadSpace_trial_fence(Module* _cell, int _rowNum, double _alpha);
    void set_x_to_site_fence(Cluster* _clus);
    
    /////////////////////////////////////////////////
    //             Operating Functions             //
    /////////////////////////////////////////////////

    //change return type and input variables if neccessary
    void AddCell(Cluster* &_clus, Module* _cell, int _rowNum, bool _firstCell);
    Cluster* AddCluster(Module* _prevCell, Module* _cell, bool _clus2prevClus = false);
    Cluster* Decluster(Cluster* _clus, const vector<int>& _ori_delta_x);

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

    void AddCell_trial(Cluster* _clus, Module* _cell, int _rowNum);
    Cluster* AddCluster_trial(Module* _prevCell, Module* _cell, Cluster* _clus);
    Cluster* AddCluster_trial_right(Module* _prevCell, Module* _cell, Cluster* _clus);

    double reduce_DeadSpace_trial(Module* _cell, int _rowNum, double _alpha);

    Cluster* Collapse_trial(Cluster* _clus);
    Cluster* Collapse_trial_right(Cluster* _clus);
    pair<int,int> CheckOverlap_trial(Cluster* _clus);
    pair<int,int> CheckOverlap_trial_right(Cluster* _clus);

    bool Is_Cluster_Block_Overlap(Cluster* _clus,bool output = false);
    bool Is_Interval_Block_Overlap(pair<int,int> _interval, int _rowNum);
    double find_utilization();

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
    vector<Cluster*> _rowIdClusterMap;      // store the last cluster in every row
    vector<Cluster*> _cellIdClusterMap;     // use to store cell cluster mapping (index = dbId())
    //vector< map<int,int> > prev_cells;    // use this to detect nearby previous cells (this cell id to prev cell id)
    //vector< map<int,int> > next_cells;    // use this to detect nearby previous cells (this cell id to prev cell id)
    vector< vector<int> > prev_cells;       // try to store it in static array to reduce time
    vector< vector<int> > next_cells;       // empty if value == -1

    vector<Row> _row;                       // for fregions
    

    map<int, Cluster*> _clusters;           // store all clusters
};

#endif // PLACEMENT_H
