#include <cstdlib>

#include "paramhandler.h"

ParamHandler param;
void ParamHandler::init()
{
    bmt = BMT_DEFAULT;
    techLefFile = cellLefFile = defFile = outDefFile = loadDefFile = plConstranitFile = UNKNOWN;
    numCPUs = maxUtil = 1;  maxDisp = -1;
    //techLefFile = cellLefFile = loadDefFile = defFile = verilogFile = outDefFile = plConstranitFile = UNKNOWN;
    //settingsParamFile = inputICCAD15File = libEarlyFile = libLateFile = sdcFile = UNKNOWN;
    //numCPUs = maxUtil = 1; maxDisp = curPlotCount = -1;
    //isPlotGIF = gArg.checkExist( "gif" );

    prefix = "ICCAD17";
    gArg.getString( "prefix", &prefix );
    gArg.getString( "out", &prefix );

    tmpStr = UNKNOWN;
    /*
    tmpBool = false;

    isRunGP = isRunLG = isRunDP = true;
    if( gArg.checkExist( "noGP" ) ) { isRunGP = false; }
    if( gArg.checkExist( "noLG" ) ) { isRunLG = false; }
    if( gArg.checkExist( "noDP" ) ) { isRunDP = false; }
    isDM2 = false;
    if( gArg.checkExist( "DM2" ) )  { isDM2 = true; }
    */
}

/*
void ParamHandler::setCurPlotCountGIF(int c, bool isGIF)
{
    curPlotCount=c;
    isPlotGIF=isGIF;
}
*/