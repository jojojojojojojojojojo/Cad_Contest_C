#ifndef ROW_H
#define ROW_H

#include <vector>
using namespace std;

#include "../common/util.h"
using namespace util;

class Site
{
  public:
    Site():_type(""), _width(0.0), _height(0.0), _step(0.0){}

    string type()                   {return _type;}
    double width()                  {return _width;}
    double height()                 {return _height;}
    double step()                   {return _step;}

    void setType(string type)       {_type = type;}
    void setWidth(double width)     {_width = width;}
    void setHeight(double height)   {_height = height;}
    void setStep(double step)       {_step = step;}

    void showInfo(){
        cout << "|============== Site  |" << endl;
        cout << " type                 : " << _type << endl;
        cout << " width                : " << _width << " micron" << endl;
        cout << " height               : " << _height << " micron" << endl;
        cout << " step                 : " << _step << " dbu" << endl;
        cout << endl;
    }

  private:
    string _type;
    double _width;  // um
    double _height; // um
    double _step;
};

class Row
{
public:

    Row(double x = -1, double y = -1, unsigned numSites = -1,
        Orient orient = OR_N, bool isSymmetric = true)
        : _name(""), _x(x), _y(y),
          _numSites(numSites), _orient(orient), _isSymmetric(isSymmetric)
    {}

    ////////////////////////////////////////////
    static bool lesser( const Row& r1, const Row& r2  ) { return ( r1._y < r2._y ); }
    static bool greater( const Row& r1, const Row& r2  ){ return ( r1._y > r2._y ); }


    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    string name() {return _name;}
    double x() {return _x;}
    double y() {return _y;}
    double height() {return site.height();}
    double width() {return double(_numSites)*site.step();}
    double numSites() {return _numSites;}
    Orient orient() {return _orient;}
    double left() {return x();}
    double right() {return x()+width(); }
    double numInterval() {return _intervals.size();}
    pair<double, double> &interval(unsigned i ) {return _intervals[i];}
    Rect rectInterval(const unsigned &i) {return Rect( _intervals[i].first, _y, _intervals[i].second, _y+site.height() );}

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////

    inline void shift_and_scale(const Point &p, const double &s , bool isBack=false);

    void setName(string name){
        _name = name;
    }
    void setPosition(double x, double y) {
        _x = x;
        _y = y;
    }
    void setNumSites(unsigned numSites) {
        _numSites = numSites;
    }
    void setOrient(Orient orient) {
        _orient = orient;
    }
    void setIsSymmetric(bool isSymmetric) {
        _isSymmetric = isSymmetric;
    }
    void setInterval( double x1, double x2 ){
        _intervals.clear();
        _intervals.push_back( make_pair( x1, x2 ) );
    }
    void addInterval( double x1, double x2 ){               //newly add
        _intervals.push_back( make_pair( x1, x2 ) );
    }
    void eraseInterval( int id ){
        _intervals.erase( vector<pair<double,double> >::iterator( &(_intervals[id]) ) );
    }
    inline void addBlockedInterval(double lBlk, double rBlk );

    static Site site;

    void showInfo(){
        cout << "|============== ROW   |" << endl;
        cout << " name                 : " << _name << endl;
        cout << " (x,y)                : " << _x  << ", " << _y << endl;
        cout << " height               : " << Row::site.height() << endl;
        cout << " siteStep             : " << Row::site.step() << endl;
        cout << " width                : " << width() << endl;
        cout << " numSites             : " << _numSites << endl;
        cout << " orient               : " << orientStr(_orient) << endl;
        cout << " isSymmetric          : " << _isSymmetric << endl;
        cout << " intervals            : " << _intervals.size() << endl;
        showInterval();
        cout << endl;
    }

    void showInterval(){
        for(unsigned i = 0 ; i < _intervals.size() ; i++)
        {
            cout<<">> interval["<<i<<"] : ("<<_intervals[i].first<<", "<<_intervals[i].second<<")"<<endl;
        }
    }

private:
    // variables from benchmark input
    string _name;
    double _x, _y;      // low x and low y
    unsigned _numSites; // number of sites
    Orient _orient;     // orient
    bool _isSymmetric;  // symmetry
    vector< pair<double, double> > _intervals; // a row is formed by placeable intervals
};


void Row::shift_and_scale(const Point &p, const double &s, bool isBack)
{
    if( isBack==false ){
        _x += p.x(); _x *= s;
        _y += p.y(); _y *= s;
        for( size_t i=0; i<numInterval(); ++i ){
            interval(i).first += p.x(); interval(i).second += p.x();
            interval(i).first *= s;     interval(i).second *= s;
        }
    }else{
        _x *= s; _x += p.x();
        _y *= s; _y += p.y();
        for( size_t i=0; i<numInterval(); ++i ){
            interval(i).first *= s;     interval(i).second *= s;
            interval(i).first += p.x(); interval(i).second += p.x();
        }
        round_me(_x); round_me(_y);
        for( size_t i=0; i<numInterval(); ++i ){
            round_me( interval(i).first );
            round_me( interval(i).second );
        }
    }

}

void Row::addBlockedInterval(double lBlk, double rBlk)
{
    for( unsigned i=0; i<_intervals.size(); ++i ){
        double &lInt = _intervals[i].first, &rInt = _intervals[i].second;
        if( lInt >= rBlk || rInt <= lBlk ) { continue; }

        if( lInt >= lBlk && rInt <= rBlk ){
            //  ---
            // BBBBB
            _intervals.erase( vector<pair<double,double> >::iterator( &(_intervals[i]) ) );
            --i;
        }else if( lInt >= lBlk && rInt > rBlk ){
            // -----
            // BBB
            _intervals[i].first = rBlk;
        }else if( lInt < lBlk && rInt <= rBlk ){
            // -----
            //   BBB
            _intervals[i].second = lBlk;
        }else if( lInt < lBlk && rInt > rBlk ){
            // -----
            //  BBB
            _intervals.insert( vector<pair<double,double> >::iterator( &(_intervals[i+1]) ), make_pair( rBlk, rInt ) );
            _intervals[i].second = lBlk;
            ++i;
        }else{
            cout << "[ERROR] Row::addBlockedInterval:: site under blockage is not removed..." << endl;
        }

    }
}

#endif // ROW_H
