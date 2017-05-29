#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <limits>
using namespace std;

#include "pin.h"
#include "row.h"
#include "../common/util.h"
using namespace util;

class Module
{
public:

    Module(string name = "" , double width = -1, double height = -1, bool isFixed = false)
        : _name(name), _x(-1), _y(-1), _width(width), _height(height),
          _isFixed(isFixed), _isDFF(false), _orient(OR_N), _isBottomVss(false),
          _classType(""), _edgeTypeLeft(0), _edgeTypeRight(0),
          _masterId(-1), _rects(0), _dbId(-1)
    {}

    static bool LesserY( const Module* m1, const Module* m2 ){
        return ( m1->y() < m2->y() );
    }
    static bool LesserX( const Module* m1, const Module* m2 ){
        return ( m1->x() < m2->x() );
    }

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    const string name() const {return _name;}
    double x() const    {return _x;} // x coordinates
    double y() const    {return _y;} // y coordinates //(x,y): lower-left point of the block
    Point pos() const   {return Point(_x, _y);}
    double centerX()    {return _x + _width/2;}
    double centerY()    {return _y + _height/2;}
    double width()      {return _width;}
    double height()     {return _height;}
    double area()       {return _width * _height;}
    bool isFixed()      {return _isFixed;} // if fixed module, return true
    bool isStdCell()    {return !_isFixed && _masterId!=-1;}
    bool isDFF()        {return _isDFF;}
    bool isIOPin()      {return _masterId==-1;}
    //newly add
    bool isBottomVss()  {return _isBottomVss;}
    size_t weight()     {return _pPins.size()};

    Orient orient()                     {return _orient;}
    string classType()                  {return _classType;}
    unsigned edgeTypeLeft()             {return _edgeTypeLeft;}
    unsigned edgeTypeRight()            {return _edgeTypeRight;}
    int masterId()                      {return _masterId;}
    unsigned numRects()                 {return _rects.size();}
    Rect rectangle()                    {return rect(0);}
    Rect rect(unsigned i)               {return _rects[i].shift( _x, _y );}
    Rect oriRect(unsigned i)            {return _rects[i];}
    void scaleRect(unsigned i, double s){ _rects[i].scale_me(s); }
    unsigned dbId()                     {return _dbId;}

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    inline void shift_and_scale(const Point &p, const double &s , bool isBack);
    inline void inflate( const double &s );
    void setName(const string &name) {
        _name = name;
    }
    void setPosition(double x, double y, Rect rect = Rect(), Orient orient = OR_OTHER) {
        if( rect.width() * rect.height() > 0 ) { bound_pos_in_rect( x, y, rect ); }

        _x = x; _y = y;

        if( orient != OR_OTHER ) { setOrient( orient ); }

        updatePinPositions();
    }
    void setCenterPosition(double x, double y, Rect rect = Rect()) {
        setPosition( x - _width/2, y - _height/2, rect );
    }
    void setWidth(double width)     { _width = width; }
    void setHeight(double height)   { _height = height; }
    void setIsFixed(bool isFixed)   { _isFixed = isFixed; }
    void setIsDFF(bool isDFF)       { _isDFF = isDFF; }
    void setOrient(Orient orient) {
        // swap width and height
        if ((_orient % 2) != (orient % 2))
            swap(_width, _height);

        // flip back
        if (_orient >= 4)
            for (unsigned i = 0; i < numPins(); i++) {
                Pin &pin = this->pin(i);
                pin.setOffset(-pin.xOffset(), pin.yOffset());
            }

        // rotate 90 degree (counter-clockwise)
        int rotateDistance = (8 + orient - _orient) % 4;
        for (unsigned i = 0; i < numPins(); i++) {
            Pin &pin = this->pin(i);
            for (int k = 0; k < rotateDistance; k++) {
                double xOffset = pin.xOffset();
                double yOffset = pin.yOffset();
                pin.setOffset(-yOffset, xOffset);
            }
        }

        // flip if needed
        if (orient >= 4){
            for (unsigned i = 0; i < numPins(); i++) {
                Pin &pin = this->pin(i);
                pin.setOffset(-pin.xOffset(), pin.yOffset());
            }
        }

        // change orient
        _orient = orient;
    }
    void setClassType(const string &classType)      { _classType = classType; }
    void setEdgeTypes(unsigned id1, unsigned id2)   { _edgeTypeLeft = id1; _edgeTypeRight = id2; }
    void setMasterId( int masterId )                { _masterId = masterId; }
    void setDbId( unsigned id )                     { _dbId = id; }

    void addRect(const Rect &rect){
        if( _rects.size()!=0 ){
            Rect &last = _rects.back();
            if( rect.left()==last.left() && rect.right()==last.right() ){
                if      ( rect.bottom()==last.top() ) { last.setTop( rect.top() ); }
                else if ( rect.top()==last.bottom() ) { last.setBottom( rect.bottom() ); }
                return;
            }
            else if( rect.bottom()==last.bottom() && rect.top()==last.bottom() ){
                if      ( rect.left()==last.right() ) { last.setRight( rect.right() ); }
                else if ( rect.right()==last.left() ) { last.setLeft( rect.left() ); }
                return;
            }
            else if( rect == last ){ return; }
        }
        _rects.push_back( rect );
    }

    void bound_pos_in_rect( double &x, double &y, const Rect &rect ) const {
        x = max( min( rect.right()-_width, x ), rect.left() );
        y = max( min( rect.top()-_height, y ), rect.bottom() );
    }
    void bound_pos_in_rect( Point &point, const Rect &rect ) const {
        double x=point.x(), y=point.y();
        bound_pos_in_rect( x, y, rect );
        point.set_x_y( x, y );
    }

    /////////////////////////////////////////////
    // get (for pins of this modules)
    /////////////////////////////////////////////
    unsigned numPins() {return _pPins.size();}
    Pin& pin(int index) {return *_pPins[index];}
    unsigned find_pin_id_by_name(const string &name){
        map<string, unsigned>::const_iterator ite = _pinsNameMap.find( name );
        if( ite==_pinsNameMap.end() ){
            cerr << "[ERROR] Cannot find " << name << " in module " << _name << endl;
            exit(1);
        }
        return ite->second;
    }
    void addPin(Pin *pPin) {_pPins.push_back(pPin);}
    void creatPinNameMap(){
        _pinsNameMap.clear();
        for( size_t i=0; i<_pPins.size(); ++i ){
            _pinsNameMap[ _pPins[i]->name() ] = i;
        }
    }
    void clearPins() {_pPins.clear();}

    //newly added
    void setIsBottomVss()
    {
        string pinName = _name + ":vss";
        // either find or not a core (-> a preplaced block)
        assert(_pinsNameMap.find(pinName) != _pinsNameMap.end() || _classType != "CORE");
        Pin* _vssPin = _pPins[_pinsNameMap.find(pinName)->second];
        double yPos = _vssPin->yOffset()+0.5*_height+0.5*(_vssPin->port().top()-_vssPin->port().bottom());
        _isBottomVss = ((int)(yPos/Row::site.height())%2 == 0)?true:false;
    }


    void showInfo(){
        cout.setf(ios::fixed);
        cout << "|============= MODULE |" << endl;
        cout << " name                 : " << _name << endl;
        cout << " (x,y)                : " << _x  << ", " << _y << endl;
        cout << " (width,height)       : " << _width << ", " << _height << endl;
        cout << " isFixed              : " << ((_isFixed)?"true":"false") << endl;
        cout << " orient               : " << orientStr(_orient) << endl;
        cout << " numPins              : " << _pPins.size() << endl;
        cout << " IsBottomVss          : " << ((_isBottomVss)?"true":"false") << endl;
        cout << " classType            : " << _classType << endl;
        cout << " (leftEdge,rightEdge) : " << _edgeTypeLeft << ", " << _edgeTypeRight << endl;
        cout << endl;
        cout.unsetf(ios::fixed);
    }

    //newly added for iccad'17

    void showPins()
    {
        for (unsigned i = 0; i < _pPins.size(); i++) { _pPins[i]->showInfo();  }
    }

  private:
    void updatePinPositions(){
        for (unsigned i = 0; i < _pPins.size(); i++) {
            Pin &pin = *_pPins[i];
            pin.setPosition(centerX()+pin.xOffset(), centerY()+pin.yOffset());
        }
    }

    // variables from benchmark input
    string                  _name;
    double                  _x, _y; // low x and low y
    double                  _width, _height;
    bool                    _isFixed;
    bool                    _isDFF;
    Orient                  _orient;
    vector<Pin*>            _pPins;
    map<string,unsigned>    _pinsNameMap;

    bool                    _isBottomVss;

    // from .lef
  public:
    static const unsigned   edgeTypeTableSize = 3;
    static vector< vector<double> > edgeTypeTable; // left, right

  private:
    string                  _classType;
    unsigned                _edgeTypeLeft, _edgeTypeRight;
    int                      _masterId; // fixed I/O pin: -1
    vector<Rect>            _rects;     // rectilinear shape
    int                     _dbId;      // index in circuit._modules
};

void Module::shift_and_scale(const Point &p, const double &s, bool isBack = false)
{
    Point modPos = pos();
    modPos.shift_scale( p, s, isBack );
    setPosition( modPos.x(), modPos.y() );
    _width *= s; _height *= s;
    for( size_t j=0; j<numRects(); ++j ){ scaleRect(j, s); }
    for( size_t j=0; j<numPins(); ++j ) { pin(j).shift_and_scale( p, s, isBack ); }
}

void Module::inflate(const double &s)
{
    _width *= s; _height *= s;
    for( size_t j=0; j<numRects(); ++j ){ scaleRect(j, s); }
}

#endif // MODULE_H

