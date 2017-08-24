#ifndef PLACERROW_H
#define PLACERROW_H

#include <utility>
#include "../circuit/circuit.h"

namespace rowpl {

struct ModR {
    ModR( Module *mod = NULL ):_module(mod) { set_x_interval(0,0); }
    ModR( const double &x, const double &width, Module *inModule = NULL )
    {
        set_x_interval( x, width );
        _module = inModule;
    }
    void set_x_interval( const double &x, const double width )
    {
        interval.first = x;
        interval.second = x + width;
    }
    void setModCir(Module &inModule) {_module=&inModule;}
    Module &modCir() const {return *_module;}
    double x1() const {return interval.first;}
    double x2() const {return interval.second;}
    double width() {return interval.second-interval.first;}

    friend bool operator < ( ModR &n1, ModR &n2 );
    friend bool operator > ( ModR &n1, ModR &n2 );
    friend bool operator == ( ModR &n1, ModR &n2 );
    friend ostream &operator << (ostream&, const ModR&);

private:
    pair<double, double> interval;
    Module *_module;
};

struct CompareDeModInRow
{
    bool operator() ( const ModR *n1, const ModR *n2 ){ return is_less( n1->x2(), n2->x1() ); }
};


typedef map<ModR*, Module*, CompareDeModInRow> RowM;
typedef RowM::iterator RMMIte;

class RowPlacer
{
  public:
    RowPlacer( Circuit *cir );

    enum Dir { LEFT, RIGHT };

    bool is_pos_legal_for_mod(Module &mod, const Point &pos, ModR *obsModR=NULL);
    bool put_legal_pos_for_mod(Module &mod, Point &pos);
    bool find_legal_pos_for_mod(Module &mod, Point &pos, ModR *obsModR=NULL);
    bool find_legal_pos_for_mod_row(Module &mod, Point &pos, ModR *obsModR=NULL);
    bool ensure_legal_pos_for_mod(Module &mod, Point &pos, vector<pair<Module *, Point> > &moveMods);
    bool initRowStructures(bool isOnlyConsiderFixedMacros);

    RowM &g_rowModMap( int rowId ) {return _rowsModMap[ rowId ];}

    void showAllRowsModules();
    void showRowModules(int rowId);

  private:
    RowPlacer() {}

    bool checkStdCellsOnRowSite();
    bool delete_modInRow(Module &mod , bool isIgnoreNotFound=false);

    bool find_legal_x_on_row(const int &rowId, Module &mod, const Dir &dir, double &xLegal, ModR *obsModR=NULL);
    ModR &mod_2_modR( Module &mod ) {return *_modId2ModInRow[ mod.dbId() ];}
    void set_mod_rowM(Module &mod, const Point &pos);

    Circuit *_cir;
    vector<ModR> _modsInRow;        // one ModInRow node for one interval of each rectangle of each module
    vector<RowM> _rowsModMap;      // one map for a row
    vector<ModR*> _modId2ModInRow;  // moduleId -> DeModInRow
};





inline bool operator < (ModR &n1, ModR &n2){
    return is_less( n1.interval.second, n2.interval.first );
}
inline bool operator == (ModR &n1, ModR &n2){
    return !(n1 < n2) && !(n1 > n2);
}
inline ostream& operator << (ostream& out, const ModR& modTNode){
    out << modTNode.modCir().name() << " interval: (" << modTNode.x1() << "->" << modTNode.x2() << ")";
    return out;
}

}
#endif
