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
    Cluster(){}
    //int round_x_to_site(double x_in, Row* _row);    //since q/e is usually not on site, this function round x into site
private:

    Module* _ref_module;        
    int _x_ref;                 //position of _ref_module (_q/_e)
    double _e;                  //cluster weight
    double _q;                  //cluster q
    vector<int> _delta_x;       //delta position of _Modules[i] to ref (same index as in _Modules)
    double _cost;               //stored cost
    map<int,Node*> _lastNode;   //last node in each row
    vector<Node*> _modules;     //all Modules 
};

class Placer
{
public:
    Placer(Circuit &inCir): _cir(&inCir), _modPLPos(0) {
        _modPLPos.resize( 3, vector<Point>( _cir->numModules() ) );
        save_modules_2_pos(PL_INIT);
        save_modules_2_pos(PL_BEST);
        save_modules_2_pos(PL_LAST);
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
    
    /////////////////////////////////////////////////
    //             Operating Functions             //
    /////////////////////////////////////////////////

    //change return type and input variables if neccessary
    void AddCell(Module* _cell, int _rowNum, Row* _row);
    void AddCluster();
    void Decluster();
    void RenewPosition(Cluster &c1);
    double RenewCost(Cluster &c1);         //return new cost
    Cluster* Collapse();
    vector<int> CheckOverlap(); //return vector of index (_modules[index]) overlapping with other cells 


    Circuit &cir() {return *_cir;}

private:
    Placer(): _cir(NULL){}

    /////////////////////////////////////////////
    // design data
    /////////////////////////////////////////////
    Circuit *_cir;
    vector< vector<Point> > _modPLPos;      // init, last, best (save poitions of modules)
    //vector<Point> _modInitPos, _modLastPos, _modBestPos;

    map<int,Cluster*> _cellIdClusterMap;    // use to store cell cluster mapping (also store clusters)
    vector< map<int,int> > nearby_cells;    // use this to detect nearby cells (clusters)
};

#endif // PLACEMENT_H
