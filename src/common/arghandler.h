#ifndef ARGHANDLER_H
#define ARGHANDLER_H

#include <string>
#include <map>

using namespace std;

class ArgHandler
{
  public:
    ArgHandler(): argv_(NULL), argc_(0){ override_.clear(); }
    ~ArgHandler(){}
    
    void init( const int argc, char* argv[] );

    bool checkExist( string caption );

    bool getInt( const string caption, int *var );
    bool getDouble( const string caption, double *var );
    bool getString( const string caption, string *var );

    void override(const string caption, const string value );
    bool removeOverride( const string caption );

  private:
    int findCaptionIndex( const string caption );

  private:
    char** argv_;
    int    argc_;

    typedef map<string,string>::const_iterator ConstOverIte;
    typedef map<string,string>::iterator OverIte;
    map< string, string > override_;
};

extern ArgHandler gArg;	// global variable

#endif
