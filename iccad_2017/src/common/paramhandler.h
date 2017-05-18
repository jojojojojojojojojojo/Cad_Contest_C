#ifndef PARAMHANDLER_H
#define PARAMHANDLER_H

#include "arghandler.h"

struct ParamHandler
{
    ParamHandler(){}
    ~ParamHandler(){}

    void init();

    enum BenchMarkType { BMT_DEFAULT, BMT_ICCAD17 };
    BenchMarkType bmt;

    /////////////////////////////////////////////
    // Placement Contest Inputs
    string techLefFile;
    string cellLefFile;
    string defFile;
    //string verilogFile;
    string outDefFile;
    int numCPUs;
    double maxUtil;
    string prefix;

    /////////////////////////////////////////////
    // ICCAD 17
    string plConstranitFile;
    double maxDisp;

    /*
    /////////////////////////////////////////////
    // Algorithm Flow
    bool isRunGP;
    bool isRunLG;
    bool isRunDP;

    /////////////////////////////////////////////
    // Global Placement -- Non-linear Programming
    bool isDM2;

    /////////////////////////////////////////////
    // ISPD 14 & 15 (Detailed-routing-driven Placement) (and ICCAD17)
    string plConstranitFile;

    /////////////////////////////////////////////
    // ICCAD 15 (Timing-driven Detailed Placment)
    string settingsParamFile;
    string inputICCAD15File;
    string sdcFile;
    string libEarlyFile, libLateFile;
    double LOCAL_WIRE_CAP_PER_MICRON;
    double LOCAL_WIRE_RES_PER_MICRON;
    int MAX_WIRE_SEGMENT_IN_MICRON;
    int MAX_LCB_FANOUTS;
    double maxDisp;

    /////////////////////////////////////////////
    // Plotting
    bool isPlotGIF;
    int curPlotCount;
    void setCurPlotCountGIF(int c, bool isGIF=false);
    void resetCurPlotCountGIF() {curPlotCount=-1; isPlotGIF=false;}

    */
    /////////////////////////////////////////////
    // Default LEF/DEF
    string inputsFile;
    string loadDefFile;

    /////////////////////////////////////////////
    // Auxiliary Variables
    string tmpStr;
    bool tmpBool;
    const char *UNKNOWN = "unknown";
};

extern ParamHandler param; // global variable

#endif
