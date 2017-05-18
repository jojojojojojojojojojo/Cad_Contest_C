#include <cstring> // strcmp
#include <cstdlib> // atoi
#include <iostream> // cout

#include "paramhandler.h"
#include "arghandler.h"

ArgHandler gArg;

void ArgHandler::init(const int argc, char *argv[])
{
    argc_ = argc; argv_ = argv;
    param.init();
}

bool ArgHandler::getInt(const string caption, int *var)
{
    ConstOverIte ite = override_.find( caption );
    if( ite != override_.end() ){
        *var = atoi( ite->second.c_str() );
        return true;
    }

    int capIndex = findCaptionIndex( caption );
    if( capIndex >= 0 && capIndex+1 < argc_ )
        *var = atoi( argv_[capIndex+1] );

    return false;
}

bool ArgHandler::getDouble(const string caption, double *var)
{
    ConstOverIte ite = override_.find( caption );
    if( ite != override_.end() ){
        *var = atof( ite->second.c_str() );
        return true;
    }

    int capIndex = findCaptionIndex( caption );
    if( capIndex >= 0 && capIndex+1 < argc_ )
        *var = atof( argv_[capIndex+1] );

    return false;
}

bool ArgHandler::getString(const string caption, string *var)
{
    ConstOverIte ite = override_.find( caption );
    if( ite != override_.end() ){
        *var = ite->second;
        return true;
    }

    int capIndex = findCaptionIndex( caption );
    if( capIndex >= 0 && capIndex+1 < argc_ )
        *var = argv_[capIndex+1];

    return false;
}

bool ArgHandler::checkExist(string caption)
{
    if( override_.find( caption ) != override_.end() )
        return true;

    if( findCaptionIndex( caption ) > 0 )
        return true;

    return false;
}

void ArgHandler::override(const string caption, const string value)
{
    override_[ caption ] = value;
}

bool ArgHandler::removeOverride(const string caption)
{
    OverIte ite = override_.find( caption );
    if( ite == override_.end() )
        return false;

    override_.erase( ite );
    return true;
}

int ArgHandler::findCaptionIndex(const string caption)
{
    for( int i = 0; i < argc_; ++i )
    {
        if( strcmp( argv_[i]+1, caption.c_str() ) == 0 )
            return i;
    }
    return -1;
}
