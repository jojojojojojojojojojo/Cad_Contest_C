#ifndef UTIL_H
#define UTIL_H

#include <cstdlib> // atof
#include <cassert> // assert
#include <cmath> // fabs
#include <iostream> // cout, ostream
#include <fstream> // ifstream
#include <vector>
#include <sstream> // stringstream
#include <string>
#include <sys/resource.h>
#include <sys/time.h>
#include <limits> // numeric_limits
#include <string.h> // memset

using namespace std;

namespace util
{
    #define MAX_UNSIGNED    numeric_limits<unsigned>::max()
    #define MAX_INT         numeric_limits<int>::max()
    #define MAX_SIZE_T      numeric_limits<size_t>::max()
    #define MAX_DOUBLE      numeric_limits<double>::max()
    #define MIN_DOUBLE      0.0001

    // =====================================================
    // show system info. -----------------------------------
    // =====================================================
    inline void showSysInfoComdLine( int argc, char *argv[] ){
        cout<<"Command line: ";for(int i=0; i<argc; ++i){ cout<<argv[i]<<" "; } cout<<endl;
        int systemret=0;
        cout<<"=================== SYSTEM INFORMATION =================="<<endl;
        systemret*=system( "echo 'User:       '`whoami`@`hostname`" );
        systemret*=system( "echo 'Date:       '`date`" );
        systemret*=system( "echo 'System:     '`uname -a`" );
        cout<<"========================================================="<<endl;
        assert( !systemret );
    }

    // =====================================================
    // filename --------------------------------------------
    // =====================================================
    inline string getFileDirname(const string &filePathName){
        string retStr = filePathName;
        string::size_type pos = retStr.rfind( "/" );
        if( pos != string::npos )
            retStr = retStr.substr( 0, pos );
        return retStr;
    }
    inline string getFileName(const string &filePathName){
        string retStr = filePathName;
        string::size_type pos = retStr.rfind( "/" );
        if( pos != string::npos )
            retStr = retStr.substr( pos + 1 );
        return retStr;
    }

    bool read_line_2_tokens( istream &inFile, vector<string> &tokens );
    bool is_char_special(char c);

    // =====================================================
    // CPU time & memory usage -----------------------------
    // =====================================================
    #define TIME_SCALE 1000000.0
    #define MEMORY_SCALE 1024.0
    class TimeUsage{
      public:
        TimeUsage() { start( FULL ); start( PARTIAL ); }
        struct TimeState{
            TimeState( long r=0, long u=0, long s=0 ): rTime(r), uTime(u), sTime(s) {}
            long rTime, uTime, sTime; //real, user, system
        };

        enum TimeType { FULL, PARTIAL };
        void start( TimeType type ){ ( type==FULL ) ? checkUsage( tStart_ ) : checkUsage( pStart_ ); }

        void showUsage( const string comment, TimeType type ){
            TimeState curSt; checkUsage( curSt );
            TimeState dur = ( type==FULL ) ? diff( tStart_, curSt ) : diff( pStart_, curSt );
            if( type==FULL )
                cout << "---------- " << comment << " total time usage -----------" << endl;
            else
                cout << "---------- " << comment << " period time usage -----------" << endl;
            cout << " Real:" << dur.rTime << "s;";
            cout << " User:" << dur.uTime << "s;";
            cout << " System:" << dur.sTime << "s." << endl << endl;
        }

      private:
        TimeState diff( TimeState &start, TimeState &end ){
            return TimeState( end.rTime-start.rTime, end.uTime-start.uTime, end.sTime-start.sTime );
        }

        void checkUsage( TimeState &st ) const {
            rusage tUsg;    getrusage( RUSAGE_SELF, &tUsg );
            timeval tReal;  gettimeofday( &tReal, NULL );
            st.uTime = tUsg.ru_utime.tv_sec + tUsg.ru_utime.tv_usec / TIME_SCALE;
            st.sTime = tUsg.ru_stime.tv_sec + tUsg.ru_stime.tv_usec / TIME_SCALE;
            st.rTime = tReal.tv_sec + tReal.tv_usec / TIME_SCALE;
        }
        TimeState tStart_, pStart_; //total, period
    };
    // memory
    inline double getPeakMemoryUsage(){
    #if defined(linux)
        char buf[1000];
        ifstream ifs( "/proc/self/stat" );
        for( int i=0; i!=23; ++i )
            ifs >> buf;
        return ( 1.0/(MEMORY_SCALE*MEMORY_SCALE) * atof(buf) ); // GB
    #else
        return -1;
    #endif
    }

    // =====================================================
    // math-related auxiliary ------------------------------
    // =====================================================
    inline bool is_less ( double a, double b, double accuracy = 0.000001 ){
        return a-b < accuracy;
    }
    inline bool is_equal( double a, double b, double accuracy = 0.000001 ){
        return fabs(a-b) < accuracy;
    }
    inline double interpolate( const double &a, const double &b, const double &ratio ){
        return a + ((b-a)*ratio);
    }
    inline void round_me( double &a, double accuracy = 0.0001 ){
        if( is_equal( a, round(a), accuracy ) ) { a = round(a); }
    }
    
    // =====================================================
    // string-related conversion----------------------------
    // =====================================================
    enum Orient {OR_N, OR_W, OR_S, OR_E, OR_FN, OR_FW, OR_FS, OR_FE, OR_OTHER};
    inline string orientStr( Orient orient ) {
        const char *orientString[] = {"N", "W", "S", "E", "FN", "FW", "FS", "FE"};
        return orientString[orient];
    }
    enum PinDir { UNKNOWN, INPUT, OUTPUT, INOUT };
    inline string dir2Str( PinDir pinDir ){
        const char *dirString[] = {"unknown", "IN", "OUT"};
        return dirString[ pinDir ];
    }
    template <class T> inline string num2str(const T &p ){
        string num;
        std::stringstream out;
        out << p;
        num = out.str();
        return num;
    }
    inline string bool2Str( bool isTrue ){
        if( isTrue )    return "TRUE";
        else            return "FALSE";
    }

    // =====================================================
    // Point -----------------------------------------------
    // =====================================================
    class Point
    {
      public:
        Point(double x=MAX_DOUBLE, double y=MAX_DOUBLE):_x(x), _y(y) {}
        double x() const {return _x;}
        double y() const {return _y;}

        void set_x_y(const double &x, const double &y){
            _x = x;
            _y = y;
        }
        void shift(const double &x, const double &y){
            _x += x;
            _y += y;
        }
        void scale( const double &s ){
            _x *= s;
            _y *= s;
        }
        void scale(const double &sx, const double &sy ){
            _x *= sx;
            _y *= sy;
        }
        void shift_scale( const Point &sh, const double& sc, bool isBack ){
            if( isBack==false ){
                _x += sh.x(); _x *= sc;
                _y += sh.y(); _y *= sc;
            }else{
                _x *= sc; _x += sh.x();
                _y *= sc; _y += sh.y();
                round_me(_x); round_me(_y);
            }
        }

        static double length(const Point &p){
            return dist( p, Point(0.0,0.0) );
        }
        static double dist(const Point &p1, const Point &p2){
            return fabs( p1.x()-p2.x() ) + fabs( p1.y()-p2.y() );
        }
        friend ostream &operator << (ostream&, const Point&);
        void operator = (const Point &p){
            set_x_y( p.x(), p.y() );
        }
        Point operator + (const Point &p){
            return Point( _x+p.x(), _y+p.y() );
        }
        Point operator - (const Point &p){
            return Point( _x-p.x(), _y-p.y() );
        }
        bool operator == (const Point &p){
            return ( is_equal(_x, p.x()) && is_equal(_y, p.y()) );
        }
        bool operator != (const Point &p){
            return (!is_equal(_x, p.x())) || (!is_equal(_y, p.y()));
        }

      private:
        double _x, _y;
    };
    inline ostream &operator << (ostream& out, const Point& p){
        out<<"("<<p.x()<<","<<p.y()<<")";
        return out;
    }

    // =====================================================
    // rectangle -------------------------------------------
    // =====================================================
    class Rect
    {
    public:

        Rect(double left = 0, double bottom = 0, double right = 0, double top = 0)
            : _left(left), _bottom(bottom), _right(right), _top(top)
        {}
        Rect( const Rect &r ): _left(r.left()), _bottom(r.bottom()), _right(r.right()), _top(r.top()) {}

        /////////////////////////////////////////////
        // get
        /////////////////////////////////////////////
        double left() const {return _left;}
        double bottom() const {return _bottom;}
        double right() const {return _right;}
        double top() const {return _top;}
        double width() const {return _right - _left;}
        double height() const {return _top - _bottom;}
        double area() const {return width() * height(); }
        Point center() const {return Point(centerX(), centerY());}
        double centerX() const {return (_left + _right)/2;}
        double centerY() const {return (_bottom + _top)/2;}
        Rect scale(double s) const {return Rect( _left*s, _bottom*s, _right*s, _top*s );}
        void scale_me(double s) {_left*=s; _bottom*=s; _right*=s; _top*=s;}
        Rect shift(const Point &p) const {return Rect( _left+p.x(), _bottom+p.y(), _right+p.x(), _top+p.y() );}
        Rect shift(double x, double y) const {return Rect( _left+x, _bottom+y, _right+x, _top+y );}
        void shift_me(const Point &p) {_left+=p.x(); _bottom+=p.y(); _right+=p.x(); _top+=p.y();}
        void shift_and_scale_me( const Point &p, const double &s, bool isBack = false ){
            if( !isBack ){ shift_me(p); scale_me(s); }
            else         { scale_me(s); shift_me(p); round(); }
        }
        void round(){
            round_me( _left ); round_me( _right ); round_me( _top ); round_me( _bottom );
        }

        /////////////////////////////////////////////
        // set
        /////////////////////////////////////////////
        void setBounds(const double & left, const double & bottom, const double & right, const double & top) {
            _left = left;
            _bottom = bottom;
            _right = right;
            _top = top;
        }
        void setTop(const double &top){
            _top = top;
        }
        void setBottom(const double & bottom){
            _bottom = bottom;
        }
        void setRight(const double & right){
            _right = right;
        }
        void setLeft(const double & left){
            _left = left;
        }

        /////////////////////////////////////////////
        // overlap area of two rectangles
        /////////////////////////////////////////////
        static double overlapArea(const Rect &rect1, const Rect &rect2) {
            double overlapH = min(rect1.right(), rect2.right()) - max(rect1.left(), rect2.left());
            double overlapV = min(rect1.top(), rect2.top()) - max(rect1.bottom(), rect2.bottom());
            if (overlapH < 0) overlapH = 0;
            if (overlapV < 0) overlapV = 0;
            return overlapH * overlapV;
        }
        void boundPosition( double &x, double &y ){
            x = max( min( x, _right ), _left );
            y = max( min( x, _top ), _bottom );
        }

        friend ostream &operator << (ostream&, const Rect&);
        void operator = (const Rect &rect){
            setBounds( rect.left(), rect.bottom(), rect.right(), rect.top() );
        }
        friend bool operator == (const Rect&, const Rect&);

    private:
        double _left;
        double _bottom;
        double _right;
        double _top;
    };
    inline ostream& operator << (ostream& out, const Rect& rect){
        out<<"rect: ("<<rect.left()<<","<<rect.bottom()<<")-("<<rect.right()<<","<<rect.top()<<")";
        return out;
    }
    inline bool operator ==(const Rect &r1, const Rect &r2){
        return ( r1.left()==r2.left() ) && ( r1.bottom()==r2.bottom() ) &&
               ( r1.right()==r2.right() ) && ( r1.top()==r2.top() );
    }

    template <class T>
    class vector2D{
      public:
        vector2D():_xDim(0), _yDim(0), value(NULL) {}
        vector2D( size_t xDim, size_t yDim, T t ){ value = NULL; init( xDim, yDim, t ); }
        ~vector2D() { delete_values(); }

        void init( size_t xDim, size_t yDim, T t ){
            if( value != NULL ) {
                assert(_xDim==xDim && _yDim==yDim);
            }
            else{
                _xDim = xDim; _yDim = yDim;
                value = new T[ _xDim*_yDim ];
            }
            fill_n( value, xDim*yDim, t );
            //memset( value, t, sizeof(T)*xDim*yDim );
        }
        void delete_values() { delete [] value; }

        const vector2D& operator=( const vector2D& rVec){
            if( rVec.xDim()!=_xDim || rVec.yDim()!=_yDim ){
                delete_values();
                _xDim = rVec.xDim(); _yDim = rVec.yDim();
                value = new T[ _xDim*_yDim ];
            }
            for( size_t x=0; x<_xDim; ++x ){
                for( size_t y=0; y<_yDim; ++y ){
                    value[ y*_xDim + x ] = rVec( x,y );
                }
            }
        }

        int xDim() {return _xDim;}
        int yDim() {return _yDim;}

        T &operator () ( const size_t &xId, const size_t &yId ) const {
            return value[ yId*_xDim + xId ];
        }

      private:
        size_t _xDim, _yDim;
        T *value;
    };

    class GaussianSmooth
    {
        public:
        void   smooth( vector< vector<double> >& input );
        void   init_gaussian_2D( double theta, int size );
        double gaussian_discrete_2D( double theta, int x, int y );
        vector< vector< double> > m_kernel;
        const double PI = M_PI;
    };

    void smoothing_vec2D( vector2D<double> &vec2D );
}

#endif // UTIL_H
