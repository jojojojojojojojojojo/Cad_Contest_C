#ifndef PIN_H
#define PIN_H

#include <string>

#include "../common/util.h"
using namespace util;

class Pin
{
public:
    Pin(unsigned moduleId = MAX_UNSIGNED, unsigned netId = MAX_UNSIGNED, double xOffset = -1, double yOffset = -1)
        : _x(-1), _y(-1), _xOffset(xOffset), _yOffset(yOffset), _moduleId(moduleId), _netId(netId),
          _isIOPin(false), _dir(UNKNOWN), _isMultiPorts(false), _taperRuleName(""), _isInNetlist(false),
          _earlySlk(0), _lateSlk(0), _earlyAt(0), _lateAt(0), _earlyRat(0), _lateRat(0), _cap(0)
    {}

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    Point pos() {return Point(_x, _y);}
    double x() {return _x;}
    double y() {return _y;}
    double xOffset() {return _xOffset;}
    double yOffset() {return _yOffset;}
    unsigned moduleId() {return _moduleId;}
    unsigned netId() {return _netId;}
    unsigned pinId() {return _pinId;}
    string name() {return _name;} // _name = moduleName:pinName;
    PinDir dir() {return _dir;}
    bool isIOPin() {return _isIOPin;}
    bool isPI() {return _isIOPin && _dir==PinDir::INPUT;}
    bool isPO() {return _isIOPin && _dir==PinDir::OUTPUT;}
    Rect port(bool isShift=false) {return (isShift) ? _port.shift(_x, _y) : _port;}
    string layerName() {return _layerName;}
    bool isMetal1() {return (_layerName=="metal1") || (_layerName=="METAL1");}
    bool isMultiPorts() {return _isMultiPorts;}
    string taperRuleName() {return _taperRuleName;}
    bool isInNetlist() {return _isInNetlist;}
    double earlySlk() {return _earlySlk;}
    double lateSlk() {return _lateSlk;}
    double earlyAt() {return _earlyAt;}
    double lateAt() {return _lateAt;}
    double earlyRat() {return _earlyRat;}
    double lateRat() {return _lateRat;}
    double cap() {return _cap;}

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    inline void shift_and_scale(const Point &p, const double &s, bool isBack);
    void setPosition(double x, double y) {
        _x = x;
        _y = y;
    }
    void setOffset(double xOffset, double yOffset) {
        _xOffset = xOffset;
        _yOffset = yOffset;
    }
    void setModuleId(unsigned moduleId) {
        _moduleId = moduleId;
    }
    void setNetId(unsigned netId) {
        _netId = netId;
    }
    void setPinId(unsigned pinId) {
        _pinId = pinId;
    }
    void setIsIOPin(){
        _isIOPin = true;
    }
    void setName(string name){
        _name = name;
    }
    void setDirStr(const string &dir){
        if( dir=="INPUT" ) _dir = INPUT;
        else if( dir=="OUTPUT" ) _dir = OUTPUT;
        else if( dir=="INOUT" ) _dir = INOUT;
        else cerr << "[ERROR] unknown pin direction: " << dir << endl;
    }
    void setDir(PinDir dir){
        _dir = dir;
    }
    void setPort(Rect port){
        _port = port;
    }
    void setLayerName(string layerName){
        _layerName = layerName;
    }
    void setIsMultiPorts(bool isMultiPorts){
        _isMultiPorts = isMultiPorts;
    }
    void setTaperRuleName(string taperRuleName){
        _taperRuleName = taperRuleName;
    }
    void setIsInNetlist(){
        _isInNetlist = true;
    }
    void setEarlySlk(double earlySlk){
        _earlySlk = earlySlk;
    }
    void setLateSlk(double lateSlk){
        _lateSlk = lateSlk;
    }
    void setEarlyAt(double earlyAt){
        _earlyAt = earlyAt;
    }
    void setLateAt(double lateAt){
        _lateAt = lateAt;
    }
    void setEarlyRat(double earlyRat){
        _earlyRat = earlyRat;
    }
    void setLateRat(double lateRat){
        _lateRat = lateRat;
    }
    void setCap(double cap){
        _cap = cap;
    }

    void showInfo(){
        cout << "|================ PIN |" << endl;
        cout << " name                 : " << _name << endl;
        cout << " id                   : " << _pinId << endl;
        cout << " netId                : " << _netId << endl;
        cout << " moduleId             : " << _moduleId << endl;
        cout << " (x_coord,y_coord)    : " << _x  << ", " << _y  << endl;
        cout << " (x_offset,y_offset)  : " << _xOffset << ", " << _yOffset << endl;
        string dir = ( _dir==UNKNOWN ) ? "unknown" : (_dir==INPUT) ? "input" : "output";
        cout << " dir                  : " << dir << endl;
        cout << " port                 : " << _port << endl;
        cout << " isMultiPorts         : " << _isMultiPorts << endl;
        cout << " rule name            : " << _taperRuleName << endl;
        cout << endl;
    }
    void showTimingInfo(){
        if( !_isInNetlist ) return ;

        cout << "|================ PIN |" << endl;
        cout << " name                 : " << _name << endl;
        cout << " slk                  : " << _earlySlk << " / " << _lateSlk << endl;
        cout << " at                   : " << _earlyAt << " / " << _lateAt << endl;
        cout << " rat                  : " << _earlyRat << " / " << _lateRat << endl;
        cout << endl;
    }

private:

    double _x, _y;              // absolute x and y
    double _xOffset, _yOffset;  // offsets from the center of the module
    unsigned _moduleId;         // id of the associated module
    unsigned _netId;            // id of the associated net
    unsigned _pinId;            // Pin id (index in circuit._pins)
    bool _isIOPin;

      // routability
    string  _name;              // module name + "_" + port_name
    PinDir  _dir;               // pin direction
    Rect    _port;              // only record the first port for each pin (from the left-bottom corner of the module)
    string  _layerName;         // layer of _port
    bool    _isMultiPorts;      // rectanlinear (e.g., L-shape) pin
    string  _taperRuleName;     // non-default-rule for detailed routing

      // timing
    bool _isInNetlist;
    double _earlySlk, _lateSlk;
    double _earlyAt, _lateAt;
    double _earlyRat, _lateRat;
    double _cap;
};

void Pin::shift_and_scale(const Point &p, const double &s, bool isBack = false)
{
    if( !isBack ){ _xOffset += p.x(); _xOffset *= s; _yOffset += p.y(); _yOffset *= s; }
    else         { _xOffset *= s; _xOffset += p.x(); _yOffset *= s; _yOffset += p.y(); }
    _port.scale_me( s );
}

#endif // PIN_H



