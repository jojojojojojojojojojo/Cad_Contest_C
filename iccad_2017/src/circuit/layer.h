#ifndef LAYER_H
#define LAYER_H

#include <iostream> // cout
#include <vector>
#include <string>

#include "../common/util.h"
using namespace util;

#define Rail pair<string,Rect>

class Layer
{
public:
    Layer()
        : _name(""), _type(""), _direction(""),
          _pitch(0.0), _offset(0.0), _width(0.0),
          _numTracks(0.0), _rails(0), _blockages(0)
    {}

    /////////////////////////////////////////////
    // get
    ////////////////////////////////////////////
    string name() {return _name;}
    string type() {return _type;}
    string direction() {return _direction;}
    const double &pitch() {return _pitch;}
    const double &offset() {return _offset;}
    const double &width() {return _width;}
    const unsigned &numTracks() {return _numTracks;}
    const Rail &rail(unsigned i) {return _rails[i];}
    unsigned numRails() {return _rails.size();}
    unsigned numBlockages() {return _blockages.size();}

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setName(const string name) {
        _name = name;
    }
    void setType(const string type) {
        _type = type;
    }
    void setDirection(const string direction) {
        _direction = direction;
    }
    void setPitch(double pitch) {
        _pitch = pitch;
    }
    void setOffset(double offset) {
        _offset = offset;
    }
    void setWidth(double width) {
        _width = width;
    }
    void setNumTracks(unsigned numTracks){
        _numTracks = numTracks;
    }
    void addRail(const Rect& rail, const string& name = ""){
        _rails.push_back(make_pair(name, rail));
    }
    void clearRails(){
        _rails.clear();
    }
    void addBlockage(const Rect& blockage){
        _blockages.push_back(blockage);
    }
    void clearBlockages(){
        _blockages.clear();
    }

    void shift_and_scale( const Point &p, const double &s, bool isBack = false ){
        if( !isBack ) { shiftAttributes(p); scaleAttributes(s); }
        else          { scaleAttributes(s); shiftAttributes(p); roundAttributes(); }
    }

    void shiftAttributes(const Point &p){
        for( size_t i=0; i<_rails.size(); ++i )
            _rails[i].second.shift_me( p );
        for( size_t i=0; i<_blockages.size(); ++i )
            _blockages[i].shift_me( p );
    }

    void scaleAttributes(const double &s){
        _pitch *= s;
        _offset *= s;
        _width *= s;
        for( size_t i=0; i<_rails.size(); ++i )
            _rails[i].second.scale_me( s );
        for( size_t i=0; i<_blockages.size(); ++i )
            _blockages[i].scale_me( s );
    }

    void roundAttributes(){
        for( size_t i=0; i<_rails.size(); ++i )
            _rails[i].second.round();
        for( size_t i=0; i<_blockages.size(); ++i )
            _blockages[i].round();
    }

    void showInfo(){
        cout << "|============== LAYER |" << endl;
        cout << " name                 : " << _name << endl;
        cout << " type                 : " << _type << endl;
        cout << " direction            : " << _direction << endl;
        cout << " pitch                : " << _pitch  << endl;
        cout << " offset               : " << _offset << endl;
        cout << " wireWidth            : " << _width << endl;
        cout << " numTracks            : " << _numTracks << endl;
        cout << " numPowerStripes      : " << _rails.size() << endl;
        cout << " numBlockages         : " << _blockages.size() << endl;
        for( unsigned i=0; i<_blockages.size(); ++i ){
            cout << "blk: " << _blockages[i] << endl;
        }
        for( unsigned i=0; i<_rails.size(); ++i ){
            cout << "power: ( "<<_rails[i].first<<" ) " << _rails[i].second << endl;
        }
        cout << endl;
    }

private:
    // variables from benchmark input
    string _name;
    string _type;
    string _direction; // HORIZONTAL or VERTICAL
    double _pitch;
    double _offset;
    double _width; // wire width in this layer
    unsigned _numTracks;
    vector<Rail> _rails;
    vector<Rect> _blockages;
};

#endif
