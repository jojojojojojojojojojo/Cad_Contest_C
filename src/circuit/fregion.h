#ifndef FREGION_H
#define FREGION_H

/* Fence region
 *
 *
/*/

#include <vector>
#include <string>
#include <iostream> // cout
using namespace std;

#include "module.h"
#include "../common/util.h"
using namespace util;

class Fregion
{
  public:
    Fregion(): _id(global_id), _name(""), _rects(0), _modules(0) { global_id++; }


    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    unsigned id() {return _id;}
    string name() {return _name;}
    Rect rect(unsigned i) {return _rects[i];}
    Module &mod(unsigned i) {return *_modules[i];}
    unsigned numRects() {return _rects.size();}
    unsigned numModules() {return _modules.size();}

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setName(string name){
        _name = name;
    }
    void add_rect(Rect &rect){
        _rects.push_back( rect );
    }
    void check_and_add_module(Module &mod){
        string name = mod.name();
        for( unsigned i=0; i<_prefixes.size(); ++i )
            if( name.find( _prefixes[i] )!=string::npos )
                _modules.push_back( &mod );
    }
    void add_prefix(const string &name){
        size_t pos = name.rfind( "/" );
        _prefixes.push_back( name.substr( 0, pos ) );
    }

    void showInfo(){
        cout << "|============ FREGION |" << endl;
        cout << " Id                   : " << _id << endl;
        cout << " name                 : " << _name << endl;
        cout << " numRects             : " << _rects.size() << endl;
        cout << " numModules           : " << _modules.size() << endl;
        cout << " numTypes             : " << _prefixes.size() << endl;
        cout << endl;
    }

  private:
    unsigned _id;
    static unsigned global_id;  //use to assign id 
    string _name;
    vector<Rect> _rects;
    vector<Module*> _modules;
    vector<string> _prefixes;
};

//unsigned Fregion::global_id = 0;

#endif
