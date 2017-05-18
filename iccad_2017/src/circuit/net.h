#ifndef NET_H
#define NET_H

#include <vector>
using namespace std;

#include "pin.h"

class Net {
public:
    Net(string name = "", unsigned id = MAX_UNSIGNED):_name(name), _dbId(id), _pPins(0), _source(NULL) {}

    /////////////////////////////////////////////
    // get (for pins of this net)
    /////////////////////////////////////////////
    string name()           {return _name;}
    unsigned dbId()         {return _dbId;}
    unsigned numPins()      {return _pPins.size();}
    Pin& pin(unsigned index){return *_pPins[index];} // index ranges from 0 to (numPins-1)
    Pin& source()           {assert( _source!=NULL ); return *_source;}

    /////////////////////////////////////////////
    // set (for pins of this net)
    /////////////////////////////////////////////
    void setName(const string &name) {_name = name; }
    void addPin(Pin &pPin) {_pPins.push_back(&pPin);}
    void clearPins() {_pPins.clear();}
    void setSourcePin(Pin &pSource) { _source = &pSource; }

    void showInfo(){
        cout << "|================ NET |" << endl;
        cout << " name                 : " << _name << endl;
        cout << " numPins              : " << _pPins.size() << endl;;
        cout << endl;
    }

private:
    string _name;
    unsigned _dbId;
    vector<Pin*> _pPins; // pins of the module
    Pin *_source;
};

#endif // NET_H
