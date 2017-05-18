//*************************************************************************
// Copyright 2012, Cadence Design Systems
//
// This file is part of the Cadence LEF/DEF Open Source Distribution,
// Product Version 5.8. 
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
// implied.
//
// See the License for the specific language governing permissions and
// limitations under the License.
// 
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
//
//************************************************************************
/*
 *  Error message number:
 *  100 -  lef reader, lefrReader.c
 *  1000 - lef parser, error, lex.cpph, lef.y (CALLBACK & CHKERR)
 *  1300 - from lefiError, lefiLayer.cpp
 *  1350 - from lefiError, lefiMacro.cpp
 *  1360 - from lefiError, lefiMisc.cpp
 *  1400 - from lefiError, lefiNonDefault.cpp
 *  1420 - from lefiError, lefiVia.cpp
 *  1430 - from lefiError, lefiViaRule.cpp
 *  1500 - lef parser, error, lef.y
 *  2000 - lef parser, warning, lex.cpph
 *  2500 - lef parser, warning, lef.y
 *  3000 - lef parser, info, lex.cpph
 *  4000 - lef writer, error, lefwWrtier.cpp & lefwWriterCalls.cpp
 *  4500 - lef writer, warning, lefwWrtier.cpp & lefwWriterCalls.cpp
 *  4700 - lef writer, info, lefwWrtier.cpp & lefwWriterCalls.cpp
 *
 *  Highest message number = 4700
 */

%{
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "FEF_stringhash.h"
#include "lex.h"
#include "lefiDefs.hpp"
#include "lefiUser.hpp"
#include "lefiUtil.hpp"
#include "lefrCallBacks.hpp"
#ifdef WIN32
#include <malloc.h>
#endif

#define LYPROP_ECAP "EDGE_CAPACITANCE"

#define YYINITDEPTH 10000  /* pcr 640902 - initialize the yystacksize to 300 */
                           /* this may need to increase in a design gets */
                           /* larger and a polygon has around 300 sizes */
                           /* 11/21/2003 - incrreased to 500, design from */
                           /* Artisan is greater than 300, need to find a */
                           /* way to dynamically increase the size */
                           /* 2/10/2004 - increased to 1000 for pcr 686073 */
                           /* 3/22/2004 - increased to 2000 for pcr 695879 */
                           /* 9/29/2004 - double the size for pcr 746865 */
                           /* tried to overwrite the yyoverflow definition */
                           /* it is impossible due to the union structure */
                           /* 10/03/2006 - increased to 10000 for pcr 913695 */

#define YYMAXDEPTH 300000  /* 1/24/2008 - increased from 150000 */
                           /* This value has to be greater than YYINITDEPTH */
int  lefRetVal;
char *outMsg;

static char lefPropDefType;    /* save the current type of the property */

/* Macro to describe how we handle a callback.
 * If the function was set then call it.
 * If the function returns non zero then there was an error
 * so call the error routine and exit.
 */
#define CALLBACK(func, typ, data) \
    if (!lef_errors) { \
      if (func) { \
        if ((lefRetVal = (*func)(typ, data, lefrUserData)) == 0) { \
        } else { \
	  return lefRetVal; \
        } \
      } \
    }

#define CHKERR() \
    if (lef_errors > 20) { \
      lefError(1020, "Too many syntax errors."); \
      lef_errors = 0; \
      return 1; \
    }

extern int lefNlines, lefNtokens;
extern FILE* lefFile;

/**********************************************************************/
/**********************************************************************/

#define C_EQ 0
#define C_NE 1
#define C_LT 2
#define C_LE 3
#define C_GT 4
#define C_GE 5

extern int lefDumbMode;   // for communicating with parser
extern int lefNoNum;      // likewise, says no numbers for the next N tokens
extern int lefRealNum;    // Next token will be a real number
extern int lefNlToken;    // likewise
extern int lefDefIf;      // likewise
extern int lefNdRule;     // for communicating with parser
extern int lefInPropDef;  // for tracking if inside propertydefinitions
extern int lefInProp;     // for tracking if inside property

extern int lef_errors;    // from lex.cpph
extern char* Hist_text;   // for BEGINEXT - extension

extern int doneLib;       // keep track if the file is done parsing
extern int ge56almostDone;// lef file has Extension section
extern int ge56done;      // done reading 5.6 and beyond library

extern char* lefrFileName;

static int ignoreVersion = 0; // ignore checking version number

int lefNamesCaseSensitive = TRUE;  // always true in 5.6
int lefReaderCaseSensitive = FALSE;  // default to 0

int lefFixedMask = 0;     //All the LEF MACRO PIN MASK assignments can be 
                          //shifted to different mask.
                          
int lefrShiftCase = 0;     // if user wants to shift to all uppercase

int lefrRelaxMode = FALSE;    // relax mode?

/* XXXXXXXXXXXXX check out these variables */
double lef_save_x, lef_save_y;  // for interpreting (*) notation of LEF/DEF

/* #define STRING_LIST_SIZE 1024 */
/* char string_list[STRING_LIST_SIZE]; */

static char temp_name[258];
static char* layerName;
static char* viaName;
static char* viaRuleName;
static char* nonDefaultRuleName;
static char* siteName;
static char* arrayName;
static char* macroName;
static char* pinName;

static int siteDef, symDef, sizeDef, pinDef, obsDef, origDef;
static int useLenThr;
static int layerCut, layerMastOver, layerRout, layerDir;
static lefiAntennaEnum antennaType = lefiAntennaAR;  /* 5.4 - antenna type */
static int arrayCutsVal;       /* keep track the arraycuts value */
static int arrayCutsWar;       /* keep track if warning has already printed */
static int viaRuleLayer;       /* keep track number of layer in a viarule */
static int viaLayer;           /* keep track number of layer in a via */
static int ndLayer;            /* keep track number of layer in a nondefault */
static int numVia;             /* keep track number of via */
static int viaRuleHasDir;      /* viarule layer has direction construct */
static int viaRuleHasEnc;      /* viarule layer has enclosure construct */
static int ndRule = 0;         /* keep track if inside nondefaultrule */
static int ndLayerWidth;       /* keep track if width is set at ndLayer */
static int ndLayerSpace;       /* keep track if spacing is set at ndLayer */
static int isGenerate;         /* keep track if viarule has generate */
static int hasType;            /* keep track of type in layer */
static int hasMask;            /* keep track of numMasks in layer */
static int hasPitch;           /* keep track of pitch in layer */
static int hasWidth;           /* keep track of width in layer */
static int hasDirection;       /* keep track of direction in layer */
static int hasParallel;        /* keep track of parallelrunlength */
static int hasInfluence;       /* keep track of influence */
static int hasTwoWidths;       /* keep track of twoWidths */
static int hasLayerMincut;     /* keep track of layer minimumcut */
static int hasManufactur = 0;  /* keep track of manufacture is after unit */
static int hasMinfeature = 0;  /* keep track of minfeature is after unit */
static int hasPRP;             /* keep track if path, rect or poly is def */
static int needGeometry;       /* keep track if path/rect/poly is defined */
static int hasViaRule_layer = 0; /* keep track at least viarule or layer */
static int hasSamenet;         /* keep track if samenet is defined in spacing */
static int hasSite = 0;        /* keep track if SITE has defined for pre 5.6 */
static int hasSiteClass = 0;   /* keep track if SITE has CLASS */
static int hasSiteSize = 0;    /* keep track if SITE has SIZE */
static int hasSpCenter = 0;    /* keep track if LAYER SPACING has CENTER */
static int hasSpSamenet = 0;   /* keep track if LAYER SPACING has SAMENET */
static int hasSpParallel = 0;  /* keep track if LAYER SPACING has PARALLEL */
static int hasSpLayer = 0;     /* keep track if LAYER SPACING has LAYER */
static int hasGeoLayer = 0;    /* keep track if Geometries has LAYER */
static int hasFixedMask = 0;


// the following variables to keep track the number of warnings printed.
static int antennaInoutWarnings = 0;
static int antennaInputWarnings = 0;
static int antennaOutputWarnings = 0;
static int arrayWarnings = 0;
static int caseSensitiveWarnings = 0;
static int correctionTableWarnings = 0;
static int dielectricWarnings = 0;
static int edgeRateThreshold1Warnings = 0;
static int edgeRateThreshold2Warnings = 0;
static int edgeRateScaleFactorWarnings = 0;
static int inoutAntennaWarnings = 0;
static int inputAntennaWarnings = 0;
static int iRDropWarnings = 0;
static int layerWarnings = 0;
static int macroWarnings = 0;
static int maxStackViaWarnings = 0;
static int minFeatureWarnings = 0;
static int noiseMarginWarnings = 0;
static int noiseTableWarnings = 0;
static int nonDefaultWarnings = 0;
static int noWireExtensionWarnings = 0;
static int outputAntennaWarnings = 0;
static int pinWarnings = 0;
static int siteWarnings = 0;
static int spacingWarnings = 0;
static int timingWarnings = 0;
static int unitsWarnings = 0;
static int useMinSpacingWarnings = 0;
static int viaRuleWarnings = 0;
static int viaWarnings = 0;
static double layerCutSpacing = 0;

int spParallelLength;          /* the number of layer parallelrunlength */

lefiNum macroNum;              /* for origin & size callback */

int comp_str(char *s1, int op, char *s2)
{
    int k = strcmp(s1, s2);
    switch (op) {
	case C_EQ: return k == 0;
	case C_NE: return k != 0;
	case C_GT: return k >  0;
	case C_GE: return k >= 0;
	case C_LT: return k <  0;
	case C_LE: return k <= 0;
	}
    return 0;
}
int comp_num(double s1, int op, double s2)
{
    double k = s1 - s2;
    switch (op) {
	case C_EQ: return k == 0;
	case C_NE: return k != 0;
	case C_GT: return k >  0;
	case C_GE: return k >= 0;
	case C_LT: return k <  0;
	case C_LE: return k <= 0;
	}
    return 0;
}
/*
double convert_name2num(char *versionName)
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *p1;
    char *versionNm = strdup(versionName);
  
    double major = 0, minor = 0, subMinor = 0;
    double version;

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    if (p1 = strchr(minorNm, '.')) {
       subMinorNm = p1+1; 
       *p1 = '\0';
    }
    major = atof(majorNm);
    minor = atof(minorNm);
    if (subMinorNm)
       subMinor = atof(subMinorNm);

    version = major;
    
    if (minor > 0)
       version = major + minor/10;

    if (subMinor > 0)
       version = version + subMinor/1000;

    lefFree(versionNm);
    return version;
}
*/

int validNum(int values) {
    switch (values) {
        case 100:
        case 200:
        case 1000:
        case 2000:
             return 1;
        case 400:
        case 800:
        case 4000:
        case 8000:
        case 10000:
        case 20000:
             if (versionNum < 5.6) {
                if (lefrUnitsCbk) {
                  if (unitsWarnings++ < lefrUnitsWarnings) {
                    outMsg = (char*)lefMalloc(10000);
                    sprintf (outMsg,
                       "Error found when processing LEF file '%s'\nUnit %d is a version 5.6 or later syntax\nYour lef file is defined with version %g.",
                    lefrFileName, values, versionNum);
                    lefError(1501, outMsg);
                    lefFree(outMsg);
                  }
                }
                return 0;
             } else {
                return 1;
             }        
    }
    if (unitsWarnings++ < lefrUnitsWarnings) {
       outMsg = (char*)lefMalloc(10000);
       sprintf (outMsg,
          "The value %d defined for LEF UNITS DATABASE MICRONS is invalid\n. Correct value is 100, 200, 400, 800, 1000, 2000, 4000, 8000, 10000, or 20000", values);
       lefError(1502, outMsg);
       lefFree(outMsg);
    }
    CHKERR();
    return 0;
}

int zeroOrGt(double values) {
    if (values < 0)
      return 0;
    return 1;
}

void resetVars() {
     hasVer = 0;
     hasNameCase = 0;
     hasBusBit = 0;
     hasDivChar = 0;
     hasManufactur = 0;
     hasMinfeature = 0;
     hasFixedMask = 0;
     hasSite = 0;
     antennaInoutWarnings = 0;
     antennaInputWarnings = 0;
     antennaOutputWarnings = 0;
     arrayWarnings = 0;
     caseSensitiveWarnings = 0;
     correctionTableWarnings = 0;
     dielectricWarnings = 0;
     edgeRateThreshold1Warnings = 0;
     edgeRateThreshold2Warnings = 0;
     edgeRateScaleFactorWarnings = 0;
     inoutAntennaWarnings = 0;
     inputAntennaWarnings = 0;
     iRDropWarnings = 0;
     layerWarnings = 0;
     macroWarnings = 0;
     maxStackViaWarnings = 0;
     minFeatureWarnings = 0;
     noiseMarginWarnings = 0;
     noiseTableWarnings = 0;
     nonDefaultWarnings = 0;
     noWireExtensionWarnings = 0;
     outputAntennaWarnings = 0;
     siteWarnings = 0;
     spacingWarnings = 0;
     timingWarnings = 0;
     unitsWarnings = 0;
     useMinSpacingWarnings = 0;
     viaRuleWarnings = 0;
     viaWarnings = 0;
     return;
}

%}

%union {
	double    dval ;
	int       integer ;
	char *    string ;
	lefPOINT  pt;
}

%token <string> K_HISTORY
%token K_ABUT K_ABUTMENT K_ACTIVE K_ANALOG K_ARRAY K_AREA
%token K_BLOCK K_BOTTOMLEFT K_BOTTOMRIGHT
%token K_BY K_CAPACITANCE K_CAPMULTIPLIER K_CLASS K_CLOCK K_CLOCKTYPE
%token K_COLUMNMAJOR K_DESIGNRULEWIDTH K_INFLUENCE
%token K_CORE K_CORNER K_COVER K_CPERSQDIST K_CURRENT 
%token K_CURRENTSOURCE K_CUT K_DEFAULT K_DATABASE K_DATA
%token K_DIELECTRIC K_DIRECTION K_DO K_EDGECAPACITANCE
%token K_EEQ K_END K_ENDCAP K_FALL K_FALLCS K_FALLT0 K_FALLSATT1
%token K_FALLRS K_FALLSATCUR K_FALLTHRESH K_FEEDTHRU K_FIXED K_FOREIGN K_FROMPIN
%token K_GENERATE K_GENERATOR K_GROUND K_HEIGHT K_HORIZONTAL K_INOUT K_INPUT
%token K_INPUTNOISEMARGIN K_COMPONENTPIN
%token K_INTRINSIC K_INVERT K_IRDROP K_ITERATE K_IV_TABLES K_LAYER K_LEAKAGE
%token K_LEQ K_LIBRARY K_MACRO K_MATCH K_MAXDELAY K_MAXLOAD K_METALOVERHANG K_MILLIAMPS
%token K_MILLIWATTS K_MINFEATURE K_MUSTJOIN K_NAMESCASESENSITIVE K_NANOSECONDS
%token K_NETS K_NEW K_NONDEFAULTRULE
%token K_NONINVERT K_NONUNATE K_OBS K_OHMS K_OFFSET K_ORIENTATION K_ORIGIN K_OUTPUT
%token K_OUTPUTNOISEMARGIN
%token K_OVERHANG K_OVERLAP K_OFF K_ON K_OVERLAPS K_PAD K_PATH K_PATTERN K_PICOFARADS
%token K_PIN K_PITCH
%token K_PLACED K_POLYGON K_PORT K_POST K_POWER K_PRE K_PULLDOWNRES K_RECT
%token K_RESISTANCE K_RESISTIVE K_RING K_RISE K_RISECS K_RISERS K_RISESATCUR K_RISETHRESH
%token K_RISESATT1 K_RISET0 K_RISEVOLTAGETHRESHOLD K_FALLVOLTAGETHRESHOLD
%token K_ROUTING K_ROWMAJOR K_RPERSQ K_SAMENET K_SCANUSE K_SHAPE K_SHRINKAGE
%token K_SIGNAL K_SITE K_SIZE K_SOURCE K_SPACER K_SPACING K_SPECIALNETS K_STACK
%token K_START K_STEP K_STOP K_STRUCTURE K_SYMMETRY K_TABLE K_THICKNESS K_TIEHIGH
%token K_TIELOW K_TIEOFFR K_TIME K_TIMING K_TO K_TOPIN K_TOPLEFT K_TOPRIGHT
%token K_TOPOFSTACKONLY
%token K_TRISTATE K_TYPE K_UNATENESS K_UNITS K_USE K_VARIABLE K_VERTICAL K_VHI
%token K_VIA K_VIARULE K_VLO K_VOLTAGE K_VOLTS K_WIDTH K_X K_Y
%token <string> T_STRING QSTRING
%token <dval>   NUMBER
%token K_N K_S K_E K_W K_FN K_FS K_FE K_FW
%token K_R0 K_R90 K_R180 K_R270 K_MX K_MY K_MXR90 K_MYR90
%token K_USER K_MASTERSLICE
%token K_ENDMACRO K_ENDMACROPIN K_ENDVIARULE K_ENDVIA K_ENDLAYER K_ENDSITE
%token K_CANPLACE K_CANNOTOCCUPY K_TRACKS K_FLOORPLAN K_GCELLGRID K_DEFAULTCAP
%token K_MINPINS K_WIRECAP
%token K_STABLE K_SETUP K_HOLD
%token K_DEFINE K_DEFINES K_DEFINEB K_IF K_THEN K_ELSE K_FALSE K_TRUE 
%token K_EQ K_NE K_LE K_LT K_GE K_GT K_OR K_AND K_NOT
%token K_DELAY K_TABLEDIMENSION K_TABLEAXIS K_TABLEENTRIES K_TRANSITIONTIME
%token K_EXTENSION
%token K_PROPDEF K_STRING K_INTEGER K_REAL K_RANGE K_PROPERTY
%token K_VIRTUAL K_BUSBITCHARS K_VERSION
%token K_BEGINEXT K_ENDEXT
%token K_UNIVERSALNOISEMARGIN K_EDGERATETHRESHOLD1 K_CORRECTIONTABLE
%token K_EDGERATESCALEFACTOR K_EDGERATETHRESHOLD2 K_VICTIMNOISE
%token K_NOISETABLE K_EDGERATE K_OUTPUTRESISTANCE K_VICTIMLENGTH
%token K_CORRECTIONFACTOR K_OUTPUTPINANTENNASIZE
%token K_INPUTPINANTENNASIZE K_INOUTPINANTENNASIZE
%token K_CURRENTDEN K_PWL K_ANTENNALENGTHFACTOR K_TAPERRULE
%token K_DIVIDERCHAR K_ANTENNASIZE K_ANTENNAMETALLENGTH K_ANTENNAMETALAREA
%token K_RISESLEWLIMIT K_FALLSLEWLIMIT K_FUNCTION K_BUFFER K_INVERTER
%token K_NAMEMAPSTRING K_NOWIREEXTENSIONATPIN K_WIREEXTENSION
%token K_MESSAGE K_CREATEFILE K_OPENFILE K_CLOSEFILE K_WARNING
%token K_ERROR K_FATALERROR
%token K_RECOVERY K_SKEW K_ANYEDGE K_POSEDGE K_NEGEDGE
%token K_SDFCONDSTART K_SDFCONDEND K_SDFCOND
%token K_MPWH K_MPWL K_PERIOD
%token K_ACCURRENTDENSITY K_DCCURRENTDENSITY K_AVERAGE K_PEAK K_RMS K_FREQUENCY
%token K_CUTAREA K_MEGAHERTZ K_USELENGTHTHRESHOLD K_LENGTHTHRESHOLD
%token K_ANTENNAINPUTGATEAREA K_ANTENNAINOUTDIFFAREA K_ANTENNAOUTPUTDIFFAREA
%token K_ANTENNAAREARATIO K_ANTENNADIFFAREARATIO K_ANTENNACUMAREARATIO
%token K_ANTENNACUMDIFFAREARATIO K_ANTENNAAREAFACTOR K_ANTENNASIDEAREARATIO
%token K_ANTENNADIFFSIDEAREARATIO K_ANTENNACUMSIDEAREARATIO
%token K_ANTENNACUMDIFFSIDEAREARATIO K_ANTENNASIDEAREAFACTOR
%token K_DIFFUSEONLY K_MANUFACTURINGGRID K_FIXEDMASK
%token K_ANTENNACELL K_CLEARANCEMEASURE K_EUCLIDEAN K_MAXXY
%token K_USEMINSPACING K_ROWMINSPACING K_ROWABUTSPACING K_FLIP K_NONE
%token K_ANTENNAPARTIALMETALAREA K_ANTENNAPARTIALMETALSIDEAREA
%token K_ANTENNAGATEAREA K_ANTENNADIFFAREA K_ANTENNAMAXAREACAR
%token K_ANTENNAMAXSIDEAREACAR K_ANTENNAPARTIALCUTAREA K_ANTENNAMAXCUTCAR
%token K_SLOTWIREWIDTH K_SLOTWIRELENGTH K_SLOTWIDTH K_SLOTLENGTH
%token K_MAXADJACENTSLOTSPACING K_MAXCOAXIALSLOTSPACING K_MAXEDGESLOTSPACING
%token K_SPLITWIREWIDTH K_MINIMUMDENSITY K_MAXIMUMDENSITY K_DENSITYCHECKWINDOW
%token K_DENSITYCHECKSTEP K_FILLACTIVESPACING K_MINIMUMCUT K_ADJACENTCUTS
%token K_ANTENNAMODEL K_BUMP K_ENCLOSURE K_FROMABOVE K_FROMBELOW
%token K_IMPLANT K_LENGTH K_MAXVIASTACK K_AREAIO K_BLACKBOX
%token K_MAXWIDTH K_MINENCLOSEDAREA K_MINSTEP K_ORIENT K_OXIDE1 K_OXIDE2
%token K_OXIDE3 K_OXIDE4 K_PARALLELRUNLENGTH K_MINWIDTH
%token K_PROTRUSIONWIDTH K_SPACINGTABLE K_WITHIN
%token K_ABOVE K_BELOW K_CENTERTOCENTER K_CUTSIZE K_CUTSPACING K_DENSITY
%token K_DIAG45 K_DIAG135 K_MASK
%token K_DIAGMINEDGELENGTH K_DIAGSPACING K_DIAGPITCH K_DIAGWIDTH
%token K_GENERATED K_GROUNDSENSITIVITY K_HARDSPACING K_INSIDECORNER
%token K_LAYERS K_LENGTHSUM K_MICRONS K_MINCUTS
%token K_MINSIZE K_NETEXPR K_OUTSIDECORNER
%token K_PREFERENCLOSURE K_ROWCOL K_ROWPATTERN K_SOFT
%token K_SUPPLYSENSITIVITY K_USEVIA
%token K_USEVIARULE K_WELLTAP
%token K_ARRAYCUTS K_ARRAYSPACING K_ANTENNAAREADIFFREDUCEPWL
%token K_ANTENNAAREAMINUSDIFF
%token K_ANTENNACUMROUTINGPLUSCUT K_ANTENNAGATEPLUSDIFF
%token K_ENDOFLINE K_ENDOFNOTCHWIDTH K_EXCEPTEXTRACUT K_EXCEPTSAMEPGNET
%token K_EXCEPTPGNET
%token K_LONGARRAY K_MAXEDGES K_NOTCHLENGTH K_NOTCHSPACING K_ORTHOGONAL
%token K_PARALLELEDGE K_PARALLELOVERLAP K_PGONLY K_PRL K_TWOEDGES K_TWOWIDTHS

%type <string> start_macro end_macro
%type <string> start_layer
%type <string> macro_pin_use
%type <string> macro_scan_use
%type <string> pin_shape
%type <string> pad_type core_type endcap_type class_type site_class
%type <string> start_foreign spacing_type clearance_type
%type <pt> pt 
%type <pt> macro_origin
%type <string> layer_option layer_options layer_type layer_direction
%type <string> electrical_direction
%type <integer> orientation maskColor
%type <dval> expression
%type <integer> b_expr
%type <string>  s_expr
%type <integer> relop spacing_value
%type <string> opt_layer_name risefall unateness delay_or_transition
%type <string> two_pin_trigger from_pin_trigger to_pin_trigger
%type <string> one_pin_trigger req_layer_name 
%type <string> layer_table_type layer_enclosure_type_opt layer_minstep_type
%type <dval>   layer_sp_TwoWidthsPRL

%nonassoc IF
%left K_AND
%left K_OR
%left K_LE K_EQ K_LT K_NE K_GE K_GT
%nonassoc LNOT
%left '-' '+'
%left '*' '/'
%nonassoc UMINUS

%%

lef_file: rules extension_opt  end_library
      {
        // 11/16/2001 - Wanda da Rosa - pcr 408334
        // Return 1 if there are errors
        if (lef_errors)
           return 1;
        if (!hasVer && versionNum < 5.6)
           lefWarning(2001, "VERSION is a required statement on LEF file with version 5.5 and earlier.\nWithout VERSION defined, the LEF file is technically incorrect.\nRefer the LEF/DEF 5.5 or earlier Language Reference manual on how to defined this statement.");
        //only pre 5.6, 5.6 it is obsolete
        if (!hasNameCase && versionNum < 5.6)
           lefWarning(2002, "NAMESCASESENSITIVE is a required statement on LEF file with version 5.5 and earlier.\nWithout NAMESCASESENSITIVE defined, the LEF file is technically incorrect.\nRefer the LEF/DEF 5.5 or earlier Language Referece manual on how to define this statement.");
        if (!hasBusBit && versionNum < 5.6)
           lefWarning(2003, "BUSBITCHARS is a required statement on LEF file with version 5.5 and earlier.\nWithout BUSBITCHARS defined, the LEF file is technically incorrect.\nRefer the LEF/DEF 5.5 or earlier Language Referece manual on how to define this statement.");
        if (!hasDivChar && versionNum < 5.6)
           lefWarning(2004, "DIVIDERCHAR is a required statementon LEF file with version 5.5 and earlier.\nWithout DIVIDECHAR defined, the LEF file is technically incorrect.\nRefer the LEF/DEF 5.5 or earlier Language Referece manual on how to define this statement.");

       (void)resetVars();
/*
        hasVer = 0;
        hasNameCase = 0;
        hasBusBit = 0;
        hasDivChar = 0;
        hasManufactur = 0;
        hasMinfeature = 0;
        hasFixedMask = 0;
        hasSite = 0;
        antennaInoutWarnings = 0;
        antennaInputWarnings = 0;
        antennaOutputWarnings = 0;
        arrayWarnings = 0;
        caseSensitiveWarnings = 0;
        correctionTableWarnings = 0;
        dielectricWarnings = 0;
        edgeRateThreshold1Warnings = 0;
        edgeRateThreshold2Warnings = 0;
        edgeRateScaleFactorWarnings = 0;
        inoutAntennaWarnings = 0;
        inputAntennaWarnings = 0;
        iRDropWarnings = 0;
        layerWarnings = 0;
        macroWarnings = 0;
        maxStackViaWarnings = 0;
        minFeatureWarnings = 0;
        noiseMarginWarnings = 0;
        noiseTableWarnings = 0;
        nonDefaultWarnings = 0;
        noWireExtensionWarnings = 0;
        outputAntennaWarnings = 0;
        siteWarnings = 0;
        spacingWarnings = 0;
        timingWarnings = 0;
        unitsWarnings = 0;
        useMinSpacingWarnings = 0;
        viaRuleWarnings = 0;
        viaWarnings = 0;
*/
      }

version: K_VERSION { lefDumbMode = 1; lefNoNum = 1;} T_STRING ';'
      { 
         versionNum = convert_name2num($3);
         if (versionNum > 5.8) {
            char temp[120];
            sprintf(temp,
               "Lef parser 5.8 does not support lef file with version %s. Parser will stop processing.", $3);
            lefError(1503, temp);
            return 1;
         }
/*
         versionNum = $3;         Save the version number for future use */
         if (lefrVersionStrCbk) {
            CALLBACK(lefrVersionStrCbk, lefrVersionStrCbkType, $3);
         } else {
            if (lefrVersionCbk)
               CALLBACK(lefrVersionCbk, lefrVersionCbkType, versionNum);
         }
         if (versionNum > 5.3 && versionNum < 5.4) {
            ignoreVersion = 1;
         }
         use5_3 = use5_4 = 0;
         lef_errors = 0;
         if (hasVer)     /* More than 1 lef file within the open file */
            (void)resetVars();
         hasVer = 1;
         if (versionNum < 5.6) {
            doneLib = 0;
            lefNamesCaseSensitive = lefReaderCaseSensitive;
         } else {
            doneLib = 1;
            lefNamesCaseSensitive = 1;
         }
      }


dividerchar: K_DIVIDERCHAR QSTRING ';'
      {
        if (lefrDividerCharCbk) {
          if (strcmp($2, "") != 0) {
             CALLBACK(lefrDividerCharCbk, lefrDividerCharCbkType, $2);
          } else {
             CALLBACK(lefrDividerCharCbk, lefrDividerCharCbkType, "/");
             lefWarning(2005, "DIVIDERCHAR has an invalid null value. Value is set to default /");
          }
        }
        hasDivChar = 1;
      }

busbitchars: K_BUSBITCHARS QSTRING ';'
      {
        if (lefrBusBitCharsCbk) {
          if (strcmp($2, "") != 0) {
             CALLBACK(lefrBusBitCharsCbk, lefrBusBitCharsCbkType, $2); 
          } else {
             CALLBACK(lefrBusBitCharsCbk, lefrBusBitCharsCbkType, "[]"); 
             lefWarning(2006, "BUSBITCHAR has an invalid null value. Value is set to default []");
          }
        }
        hasBusBit = 1;
      }

rules: /* empty */
	| rules rule
	| error 
	    { }

end_library: /* empty */
      {
        if (versionNum >= 5.6) {
           doneLib = 1;
           ge56done = 1;
        }
      }
      | K_END K_LIBRARY
      {
        doneLib = 1;
        ge56done = 1;
        if (lefrLibraryEndCbk)
          CALLBACK(lefrLibraryEndCbk, lefrLibraryEndCbkType, 0);
        // 11/16/2001 - Wanda da Rosa - pcr 408334
        // Return 1 if there are errors
/*
        if (lef_errors)
           return 1;
        if (!hasVer)
           lefWarning(2001, "VERSION is a required statement.");
        if (!hasNameCase)
           lefWarning(2002, "NAMESCASESENSITIVE is a required statement.");
        if (!hasBusBit && versionNum < 5.6)
           lefWarning(2003, "BUSBITCHARS is a required statement.");
        if (!hasDivChar && versionNum < 5.6)
           lefWarning(2004, "DIVIDERCHAR is a required statement.");
        hasVer = 0;
        hasNameCase = 0;
        hasBusBit = 0;
        hasDivChar = 0;
        hasManufactur = 0;
        hasMinfeature = 0;
        hasSite = 0;
*/
      }

rule:  version | busbitchars | case_sensitivity | units_section
    | layer_rule | via | viarule | viarule_generate | dividerchar
    | wireextension | msg_statement
    | spacing_rule | dielectric | minfeature | irdrop | site | macro | array
    | def_statement | nondefault_rule | prop_def_section
    | universalnoisemargin | edgeratethreshold1
    | edgeratescalefactor | edgeratethreshold2
    | noisetable | correctiontable | input_antenna
    | output_antenna | inout_antenna
    | antenna_input | antenna_inout | antenna_output | manufacturing  | fixedmask 
    | useminspacing | clearancemeasure | maxstack_via
    | create_file_statement
    	;

case_sensitivity: K_NAMESCASESENSITIVE K_ON ';'
	  {
            if (versionNum < 5.6) {
              lefNamesCaseSensitive = TRUE;
              if (lefrCaseSensitiveCbk)
                CALLBACK(lefrCaseSensitiveCbk, lefrCaseSensitiveCbkType,
			        lefNamesCaseSensitive);
              hasNameCase = 1;
            } else
              if (lefrCaseSensitiveCbk) /* write warning only if cbk is set */
                 if (caseSensitiveWarnings++ < lefrCaseSensitiveWarnings)
                   lefWarning(2007, "NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
	  }
      | K_NAMESCASESENSITIVE K_OFF ';'
	  {
            if (versionNum < 5.6) {
	      lefNamesCaseSensitive = FALSE;
              if (lefrCaseSensitiveCbk)
	        CALLBACK(lefrCaseSensitiveCbk, lefrCaseSensitiveCbkType,
			       lefNamesCaseSensitive);
              hasNameCase = 1;
            } else {
              if (lefrCaseSensitiveCbk) { /* write error only if cbk is set */
                if (caseSensitiveWarnings++ < lefrCaseSensitiveWarnings) {
                  lefError(1504, "NAMESCASESENSITIVE statement is set with OFF.\nStarting version 5.6, NAMESCASENSITIVE is obsolete,\nif it is defined, it has to have the ON value.\nParser will stop processing.");
                  CHKERR();
                }
              }
            }
	  }

wireextension: K_NOWIREEXTENSIONATPIN K_ON ';'
    {
      if (versionNum < 5.6) {
        if (lefrNoWireExtensionCbk)
          CALLBACK(lefrNoWireExtensionCbk, lefrNoWireExtensionCbkType, "ON");
      } else
        if (lefrNoWireExtensionCbk) /* write warning only if cbk is set */
           if (noWireExtensionWarnings++ < lefrNoWireExtensionWarnings)
             lefWarning(2008, "NOWIREEXTENSIONATPIN statement is obsolete in version 5.6 or later.\nThe NOWIREEXTENSIONATPIN statement will be ignored.");
    }
  | K_NOWIREEXTENSIONATPIN K_OFF ';'
    {
      if (versionNum < 5.6) {
        if (lefrNoWireExtensionCbk)
          CALLBACK(lefrNoWireExtensionCbk, lefrNoWireExtensionCbkType, "OFF");
      } else
        if (lefrNoWireExtensionCbk) /* write warning only if cbk is set */
           if (noWireExtensionWarnings++ < lefrNoWireExtensionWarnings)
             lefWarning(2008, "NOWIREEXTENSIONATPIN statement is obsolete in version 5.6 or later.\nThe NOWIREEXTENSIONATPIN statement will be ignored.");
    }

fixedmask: K_FIXEDMASK ';'
    { 
       if (versionNum >= 5.8) {
       
          if (lefrFixedMaskCbk) {
            lefFixedMask = 1;
            CALLBACK(lefrFixedMaskCbk, lefrFixedMaskCbkType, lefFixedMask);
          }
          
          hasFixedMask = 1;
       }
    }
    
manufacturing: K_MANUFACTURINGGRID NUMBER ';'
    {
      if (lefrManufacturingCbk)
        CALLBACK(lefrManufacturingCbk, lefrManufacturingCbkType, $2);
      hasManufactur = 1;
    }

useminspacing: K_USEMINSPACING spacing_type spacing_value ';'
  {
    if ((strcmp($2, "PIN") == 0) && (versionNum >= 5.6)) {
      if (lefrUseMinSpacingCbk) /* write warning only if cbk is set */
         if (useMinSpacingWarnings++ < lefrUseMinSpacingWarnings)
            lefWarning(2009, "USEMINSPACING PIN statement is obsolete in version 5.6 or later.\n The USEMINSPACING PIN statement will be ignored.");
    } else {
        if (lefrUseMinSpacingCbk) {
          lefrUseMinSpacing.lefiUseMinSpacing::set($2, $3);
          CALLBACK(lefrUseMinSpacingCbk, lefrUseMinSpacingCbkType,
                   &lefrUseMinSpacing);
      }
    }
  }

clearancemeasure: K_CLEARANCEMEASURE clearance_type ';'
    { CALLBACK(lefrClearanceMeasureCbk, lefrClearanceMeasureCbkType, $2); }

clearance_type:
  K_MAXXY   {$$ = (char*)"MAXXY";}
  | K_EUCLIDEAN   {$$ = (char*)"EUCLIDEAN";}

spacing_type:
  K_OBS     {$$ = (char*)"OBS";}
  | K_PIN   {$$ = (char*)"PIN";}

spacing_value:
  K_ON      {$$ = 1;}
  | K_OFF   {$$ = 0;}

units_section: start_units units_rules K_END K_UNITS
    { 
      if (lefrUnitsCbk)
        CALLBACK(lefrUnitsCbk, lefrUnitsCbkType, &lefrUnits);
    }

start_units: K_UNITS
    {
      lefrUnits.lefiUnits::clear();
      if (hasManufactur) {
        if (unitsWarnings++ < lefrUnitsWarnings) {
          lefError(1505, "MANUFACTURINGGRID statement was defined before UNITS.\nRefer the LEF Language Reference manual for the order of LEF statements.");
          CHKERR();
        }
      }
      if (hasMinfeature) {
        if (unitsWarnings++ < lefrUnitsWarnings) {
          lefError(1712, "MINFEATURE statement was defined before UNITS.\nRefer the LEF Language Reference manual for the order of LEF statements.");
          CHKERR();
        }
      }
      if (versionNum < 5.6) {
        if (hasSite) {/*SITE is defined before UNIT and is illegal in pre 5.6*/
          lefError(1713, "SITE statement was defined before UNITS.\nRefer the LEF Language Reference manual for the order of LEF statements.");
          CHKERR();
        }
      }
    }

units_rules: /* empty */
  | units_rules units_rule
  ;

units_rule: K_TIME K_NANOSECONDS NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setTime($3); }
  | K_CAPACITANCE K_PICOFARADS NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setCapacitance($3); }
  | K_RESISTANCE K_OHMS NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setResistance($3); }
  | K_POWER K_MILLIWATTS NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setPower($3); }
  | K_CURRENT K_MILLIAMPS NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setCurrent($3); }
  | K_VOLTAGE K_VOLTS NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setVoltage($3); }
  | K_DATABASE K_MICRONS NUMBER ';'
    { 
      if(validNum((int)$3)) {
         if (lefrUnitsCbk)
            lefrUnits.lefiUnits::setDatabase("MICRONS", $3);
      }
    }
  | K_FREQUENCY K_MEGAHERTZ NUMBER ';'
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setFrequency($3); }

layer_rule: start_layer 
    layer_options end_layer
    { 
      if (lefrLayerCbk)
        CALLBACK(lefrLayerCbk, lefrLayerCbkType, &lefrLayer);
    }

start_layer: K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING
    { 
      if (lefrHasMaxVS) {   /* 5.5 */
        if (lefrLayerCbk) { /* write error only if cbk is set */
          if (layerWarnings++ < lefrLayerWarnings) {
            lefError(1506, "A MAXVIASTACK statement is defined before the LAYER statement.\nRefer to the LEF Language Reference manual for the order of LEF statements.");
            CHKERR();
          }
        }
      }
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setName($3);
      useLenThr = 0;
      layerCut = 0;
      layerMastOver = 0;
      layerRout = 0;
      layerDir = 0;
      lefrHasLayer = 1;
      //strcpy(layerName, $3);
      layerName = strdup($3);
      hasType = 0;
      hasMask = 0;
      hasPitch = 0;
      hasWidth = 0;
      hasDirection = 0;
      hasParallel = 0;
      hasInfluence = 0;
      hasTwoWidths = 0;
      lefrHasSpacingTbl = 0;
      lefrHasSpacing = 0;
    }

end_layer: K_END {lefDumbMode = 1; lefNoNum = 1; } T_STRING
    { 
      if (strcmp(layerName, $3) != 0) {
        if (lefrLayerCbk) { /* write error only if cbk is set */
          if (layerWarnings++ < lefrLayerWarnings) {
             outMsg = (char*)lefMalloc(10000);
             sprintf (outMsg,
                "END LAYER name %s is different from the LAYER name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, layerName);
             lefError(1507, outMsg);
             lefFree(outMsg);
             lefFree(layerName);
             CHKERR(); 
          } else
             lefFree(layerName);
        } else
          lefFree(layerName);
      } else
        lefFree(layerName);
      if (!lefrRelaxMode) {
        if (hasType == 0) {
          if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1508, "TYPE statement is a required statement in a LAYER and it is not defined.");
               CHKERR(); 
            }
          }
        }
        if ((layerRout == 1) && (hasPitch == 0)) {
          if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1509, "PITCH statement is a required statement in a LAYER with type ROUTING and it is not defined.");
              CHKERR(); 
            }
          }
        }
        if ((layerRout == 1) && (hasWidth == 0)) {
          if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1510, "WIDTH statement is a required statement in a LAYER with type ROUTING and it is not defined.");
              CHKERR(); 
            }
          }
        }
        if ((layerRout == 1) && (hasDirection == 0)) {
          if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg, "The DIRECTION statement which is required in a LAYER with TYPE ROUTING is not defined in LAYER %s.\nUpdate your lef file and add the DIRECTION statement for layer %s.", $3, $3);
              lefError(1511, outMsg);
              lefFree(outMsg);
              CHKERR(); 
            }
          }
        }
      }
    }

layer_options:
  /* empty */
    { }
  | layer_options layer_option    /* Use left recursions */
/*  WANDA
  | layer_option layer_options
*/
    { }

layer_option:
  K_ARRAYSPACING                   /* 5.7 */
    {
       /* let setArraySpacingCutSpacing to set the data */
    }
    layer_arraySpacing_long
    layer_arraySpacing_width
    K_CUTSPACING NUMBER K_ARRAYCUTS NUMBER K_SPACING NUMBER
    {
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::setArraySpacingCut($6);
         lefrLayer.lefiLayer::addArraySpacingArray((int)$8, $10);
         arrayCutsVal = (int)$8;  /* set the value */
         arrayCutsWar = 0;
      }
    }
    layer_arraySpacing_arraycuts ';'
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "ARRAYSPACING is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1685, outMsg);
         lefFree(outMsg);
         CHKERR();
      }
    }
  | K_TYPE layer_type ';'
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::setType($2);
      hasType = 1;
    }
  | K_MASK NUMBER ';'
    {
      if (versionNum < 5.8) {
          if (layerWarnings++ < lefrViaWarnings) {
              lefError(2081, "MASK information can only be defined with version 5.8");
              CHKERR(); 
          }           
      } else {
          if (lefrLayerCbk) {
            lefrLayer.lefiLayer::setMask((int)$2);
          }
          
          hasMask = 1;
      }
    }
  | K_PITCH NUMBER ';'
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setPitch($2);
      hasPitch = 1;  
    }
  | K_PITCH NUMBER NUMBER ';'
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setPitchXY($2, $3);
      hasPitch = 1;  
    }
  | K_DIAGPITCH NUMBER ';'
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagPitch($2);
    }
  | K_DIAGPITCH NUMBER NUMBER ';'
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagPitchXY($2, $3);
    }
  | K_OFFSET NUMBER ';'
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setOffset($2);
    }
  | K_OFFSET NUMBER NUMBER ';'
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setOffsetXY($2, $3);
    }
  | K_DIAGWIDTH NUMBER ';'
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagWidth($2);
    }
  | K_DIAGSPACING NUMBER ';'
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagSpacing($2);
    }
  | K_WIDTH NUMBER ';'    // CUT & ROUTING
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setWidth($2);
      hasWidth = 1;  
    }
  | K_AREA NUMBER ';'
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setArea($2);
    }
  | K_SPACING NUMBER
    {
      hasSpCenter = 0;       /* reset to 0, only once per spacing is allowed */
      hasSpSamenet = 0;
      hasSpParallel = 0;
      hasSpLayer = 0;
      layerCutSpacing = $2;  /* for error message purpose */
      // 11/22/99 - Wanda da Rosa, PCR 283762
      //            Issue an error is this is defined in masterslice
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1512, "It is incorrect to define a SPACING statement in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      // 5.5 either SPACING or SPACINGTABLE, not both for routing layer only
      if (layerRout) {
        if (lefrHasSpacingTbl && versionNum < 5.7) {
           if (lefrLayerCbk) { /* write error only if cbk is set */
              if (layerWarnings++ < lefrLayerWarnings) {
                lefWarning(2010, "It is incorrect to have both SPACING rules & SPACINGTABLE rules within a ROUTING layer");
              }
           }
        }
        if (lefrLayerCbk)
           lefrLayer.lefiLayer::setSpacingMin($2);
        lefrHasSpacing = 1;
      } else { 
        if (lefrLayerCbk)
           lefrLayer.lefiLayer::setSpacingMin($2);
      }
    }
    layer_spacing_opts
    layer_spacing_cut_routing ';'
  | K_SPACINGTABLE K_ORTHOGONAL K_WITHIN NUMBER K_SPACING NUMBER   /* 5.7 */
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::setSpacingTableOrtho();
      if (lefrLayerCbk) /* due to converting to C, else, convertor produce */
         lefrLayer.lefiLayer::addSpacingTableOrthoWithin($4, $6);/*bad code*/
    }
    layer_spacingtable_opts ';'
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "SPACINGTABLE ORTHOGONAL is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1694, outMsg);
         lefFree(outMsg);
         CHKERR();
      }
    }
  | K_DIRECTION layer_direction ';'
    {
      layerDir = 1;
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1513, "DIRECTION statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDirection($2);
      hasDirection = 1;  
    }
  | K_RESISTANCE K_RPERSQ NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1514, "RESISTANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setResistance($3);
    }
  | K_RESISTANCE K_RPERSQ K_PWL '(' res_points ')' ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1515, "RESISTANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
    }
  | K_CAPACITANCE K_CPERSQDIST NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1516, "CAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setCapacitance($3);
    }
  | K_CAPACITANCE K_CPERSQDIST K_PWL '(' cap_points ')' ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1517, "CAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
    }
  | K_HEIGHT NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1518, "HEIGHT statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setHeight($2);
    }
  | K_WIREEXTENSION NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1519, "WIREEXTENSION statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setWireExtension($2);
    }
  | K_THICKNESS NUMBER ';'
    {
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1520, "THICKNESS statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setThickness($2);
    }
  | K_SHRINKAGE NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1521, "SHRINKAGE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setShrinkage($2);
    }
  | K_CAPMULTIPLIER NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1522, "CAPMULTIPLIER statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setCapMultiplier($2);
    }
  | K_EDGECAPACITANCE NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1523, "EDGECAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setEdgeCap($2);
    }
/* 3/24/2000 - Wanda da Rosa.  Same call for 5.4 but more options.
              Syntax is handle 1 page down.
  | K_ANTENNAAREAFACTOR NUMBER ';'
    {
      if (!layerRout && (layerCut || layerMastOver)) {
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1524, "ANTENNAAREAFACTOR statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaArea($2);
    }
*/
  | K_ANTENNALENGTHFACTOR NUMBER ';'
    { /* 5.3 syntax */
      use5_3 = 1;
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1525, "ANTENNALENGTHFACTOR statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      } else if (versionNum >= 5.4) {
         if (use5_4) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                  outMsg = (char*)lefMalloc(10000);
                  sprintf (outMsg,
                    "ANTENNALENGTHFACTOR statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNALENGTHFACTOR syntax, which is incorrect.", versionNum);
                  lefError(1526, outMsg);
                  lefFree(outMsg);
                  CHKERR();
               }
            }
         }
      }

      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaLength($2);
    }
  | K_CURRENTDEN NUMBER ';'
    {
      if (versionNum < 5.2) {
         if (!layerRout) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                 lefError(1702, "CURRENTDEN statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
                 CHKERR();
               }
            }
         }
         if (lefrLayerCbk) lefrLayer.lefiLayer::setCurrentDensity($2);
      } else {
         if (layerWarnings++ < lefrLayerWarnings) {
            lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            CHKERR();
         }
      }
    }
  | K_CURRENTDEN K_PWL '(' current_density_pwl_list ')' ';'
    { 
      if (versionNum < 5.2) {
         if (!layerRout) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                 lefError(1702, "CURRENTDEN statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
                 CHKERR();
               }
            }
         }
      } else {
         if (layerWarnings++ < lefrLayerWarnings) {
            lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            CHKERR();
         }
      }
    }
  | K_CURRENTDEN '(' NUMBER NUMBER ')' ';'
    {
      if (versionNum < 5.2) {
         if (!layerRout) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                 lefError(1702, "CURRENTDEN statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
                 CHKERR();
               }
            }
         }
         if (lefrLayerCbk) lefrLayer.lefiLayer::setCurrentPoint($3, $4);
      } else {
         if (layerWarnings++ < lefrLayerWarnings) {
            lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            CHKERR();
         }
      }
    }
  | K_PROPERTY { lefDumbMode = 10000000; lefRealNum = 1; lefInProp = 1; } layer_prop_list ';'
    {
      lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    }
  | K_ACCURRENTDENSITY layer_table_type
    {
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1527, "ACCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addAccurrentDensity($2);
    }
    layer_frequency
  | K_ACCURRENTDENSITY layer_table_type NUMBER ';'
    {
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1527, "ACCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) {
           lefrLayer.lefiLayer::addAccurrentDensity($2);
           lefrLayer.lefiLayer::setAcOneEntry($3);
      }
    }
  | K_DCCURRENTDENSITY K_AVERAGE NUMBER ';'
    {
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1528, "DCCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addDccurrentDensity("AVERAGE");
         lefrLayer.lefiLayer::setDcOneEntry($3);
      }
    }
  | K_DCCURRENTDENSITY K_AVERAGE K_CUTAREA NUMBER
    {
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1528, "DCCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (!layerCut) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1529, "CUTAREA statement can only be defined in LAYER with type CUT. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addDccurrentDensity("AVERAGE");
         lefrLayer.lefiLayer::addNumber($4);
      }
    }
    number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addDcCutarea(); }
    dc_layer_table
  | K_DCCURRENTDENSITY K_AVERAGE K_WIDTH NUMBER
    {
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1528, "DCCURRENTDENSITY can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1530, "WIDTH statement can only be defined in LAYER with type ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addDccurrentDensity("AVERAGE");
         lefrLayer.lefiLayer::addNumber($4);
      }
    }
    number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addDcWidth(); }
    dc_layer_table

/* 3/23/2000 - 5.4 syntax.  Wanda da Rosa */
  | K_ANTENNAAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNAAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1531, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNADIFFAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNAAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1704, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (!layerRout && !layerCut && layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1533, "ANTENNAAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaAreaRatio($2);
    }
  | K_ANTENNADIFFAREARATIO
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNADIFFAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1532, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNADIFFAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNADIFFAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1704, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (!layerRout && !layerCut && layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1534, "ANTENNADIFFAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      antennaType = lefiAntennaDAR; 
    }
    layer_antenna_pwl ';'
  | K_ANTENNACUMAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNACUMAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1535, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNACUMAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1536, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (!layerRout && !layerCut && layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1537, "ANTENNACUMAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaCumAreaRatio($2);
    }
  | K_ANTENNACUMDIFFAREARATIO
    {  /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNACUMDIFFAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1538, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNACUMDIFFAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMDIFFAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1539, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (!layerRout && !layerCut && layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1540, "ANTENNACUMDIFFAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      antennaType = lefiAntennaCDAR;
    }
    layer_antenna_pwl ';'
  | K_ANTENNAAREAFACTOR NUMBER
    { /* both 5.3  & 5.4 syntax */
      if (!layerRout && !layerCut && layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1541, "ANTENNAAREAFACTOR can only be defined in LAYER with TYPE ROUTING or CUT. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      /* this does not need to check, since syntax is in both 5.3 & 5.4 */
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaAreaFactor($2);
      antennaType = lefiAntennaAF;
    }
    layer_antenna_duo ';'
  | K_ANTENNASIDEAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1542, "ANTENNASIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNASIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1543, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNASIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNASIDEAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1544, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaSideAreaRatio($2);
    }
  | K_ANTENNADIFFSIDEAREARATIO
    {  /* 5.4 syntax */
      use5_4 = 1;
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1545, "ANTENNADIFFSIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNADIFFSIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1546, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNADIFFSIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNADIFFSIDEAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1547, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      antennaType = lefiAntennaDSAR;
    }
    layer_antenna_pwl ';'
  | K_ANTENNACUMSIDEAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1548, "ANTENNACUMSIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNACUMSIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1549, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNACUMSIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMSIDEAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1550, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaCumSideAreaRatio($2);
    }
  | K_ANTENNACUMDIFFSIDEAREARATIO
    {  /* 5.4 syntax */
      use5_4 = 1;
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1551, "ANTENNACUMDIFFSIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNACUMDIFFSIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1552, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNACUMDIFFSIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMDIFFSIDEAREARATIO syntax, which is incorrect.", versionNum);
               lefError(1553, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      antennaType = lefiAntennaCDSAR;
    }
    layer_antenna_pwl ';'
  | K_ANTENNASIDEAREAFACTOR NUMBER
    { /* 5.4 syntax */
      use5_4 = 1;
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1554, "ANTENNASIDEAREAFACTOR can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNASIDEAREAFACTOR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1555, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNASIDEAREAFACTOR statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNASIDEAREAFACTOR syntax, which is incorrect.", versionNum);
               lefError(1556, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaSideAreaFactor($2);
      antennaType = lefiAntennaSAF;
    }
    layer_antenna_duo ';'
  | K_ANTENNAMODEL /* 5.5 */
    { /* 5.4 syntax */
      use5_4 = 1;
      if (!layerRout && !layerCut && layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1557, "ANTENNAMODEL can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNAMODEL statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1558, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (use5_3) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "ANTENNAMODEL statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNAMODEL syntax, which is incorrect.", versionNum);
               lefError(1559, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      antennaType = lefiAntennaO;
    }
    layer_oxide ';'
  | K_ANTENNACUMROUTINGPLUSCUT ';'        /* 5.7 */
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "ANTENNACUMROUTINGPLUSCUT is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1686, outMsg);
         lefFree(outMsg);
         CHKERR();
      } else {
         if (!layerRout && !layerCut) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                  lefError(1560, "ANTENNACUMROUTINGPLUSCUT can only be defined in LAYER with type ROUTING or CUT. Parser will stop processing.");
                  CHKERR();
               }
            }
         }
         if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaCumRoutingPlusCut();
      }
    }
  | K_ANTENNAGATEPLUSDIFF NUMBER ';'      /* 5.7 */
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "ANTENNAGATEPLUSDIFF is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1687, outMsg);
         lefFree(outMsg);
         CHKERR();
      } else {
         if (!layerRout && !layerCut) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                  lefError(1561, "ANTENNAGATEPLUSDIFF can only be defined in LAYER with type ROUTING or CUT. Parser will stop processing.");
                  CHKERR();
               }
            }
         }
         if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaGatePlusDiff($2);
      }
    }
  | K_ANTENNAAREAMINUSDIFF NUMBER ';'     /* 5.7 */
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "ANTENNAAREAMINUSDIFF is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1688, outMsg);
         lefFree(outMsg);
         CHKERR();
      } else {
         if (!layerRout && !layerCut) {
            if (lefrLayerCbk) { /* write error only if cbk is set */
               if (layerWarnings++ < lefrLayerWarnings) {
                  lefError(1562, "ANTENNAAREAMINUSDIFF can only be defined in LAYER with type ROUTING or CUT. Parser will stop processing.");
                  CHKERR();
               }
            }
         }
         if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaAreaMinusDiff($2);
      }
    }
  | K_ANTENNAAREADIFFREDUCEPWL '(' pt pt            /* 5.7 */
    {
      if (!layerRout && !layerCut) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1563, "ANTENNAAREADIFFREDUCEPWL can only be defined in LAYER with type ROUTING or CUT. Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) { /* require min 2 points, set the 1st 2 */
         lefrAntennaPWLPtr=(lefiAntennaPWL*)lefMalloc(sizeof(lefiAntennaPWL));
         lefrAntennaPWLPtr->lefiAntennaPWL::Init();
         lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($3.x, $3.y);
         lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($4.x, $4.y);
      }
    } 
    layer_diffusion_ratios ')' ';'
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setAntennaPWL(lefiAntennaADR, lefrAntennaPWLPtr);
    }
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "ANTENNAAREADIFFREDUCEPWL is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1689, outMsg);
        lefFree(outMsg);
        CHKERR();
      }
    }
  | K_SLOTWIREWIDTH NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireWidth($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2011, "SLOTWIREWIDTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "SLOTWIREWIDTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1564, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireWidth($2);
    }
  | K_SLOTWIRELENGTH NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireLength($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2012, "SLOTWIRELENGTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "SLOTWIRELENGTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1565, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireLength($2);
    }
  | K_SLOTWIDTH NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWidth($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2013, "SLOTWIDTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "SLOTWIDTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1566, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWidth($2);
    }
  | K_SLOTLENGTH NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotLength($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2014, "SLOTLENGTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "SLOTLENGTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1567, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotLength($2);
    }
  | K_MAXADJACENTSLOTSPACING NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxAdjacentSlotSpacing($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2015, "MAXADJACENTSLOTSPACING statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MAXADJACENTSLOTSPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1568, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxAdjacentSlotSpacing($2);
    }
  | K_MAXCOAXIALSLOTSPACING NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxCoaxialSlotSpacing($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
                lefWarning(2016, "MAXCOAXIALSLOTSPACING statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MAXCOAXIALSLOTSPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1569, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxCoaxialSlotSpacing($2);
    }
  | K_MAXEDGESLOTSPACING NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxEdgeSlotSpacing($2);
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2017, "MAXEDGESLOTSPACING statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MAXEDGESLOTSPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1570, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxEdgeSlotSpacing($2);
    }
  | K_SPLITWIREWIDTH NUMBER ';'
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum >= 5.7) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings)
               lefWarning(2018, "SPLITWIREWIDTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "SPLITWIREWIDTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1571, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setSplitWireWidth($2);
    }
  | K_MINIMUMDENSITY NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MINIMUMDENSITY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1572, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMinimumDensity($2);
    }
  | K_MAXIMUMDENSITY NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MAXIMUMDENSITY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1573, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMaximumDensity($2);
    }
  | K_DENSITYCHECKWINDOW NUMBER NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "DENSITYCHECKWINDOW statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1574, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDensityCheckWindow($2, $3);
    }
  | K_DENSITYCHECKSTEP NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "DENSITYCHECKSTEP statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1575, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDensityCheckStep($2);
    }
  | K_FILLACTIVESPACING NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (ignoreVersion) {
         /* do nothing */
      } else if (versionNum < 5.4) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "FILLACTIVESPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1576, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setFillActiveSpacing($2);
    }
  | K_MAXWIDTH NUMBER ';'              /* 5.5 */
    {
      // 5.5 MAXWIDTH, is for routing layer only
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               lefError(1577, "MAXWIDTH statement can only be defined in LAYER with TYPE ROUTING.  Parser will stop processing.");
               CHKERR();
            }
         }
      }
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MAXWIDTH statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1578, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxwidth($2);
    }
  | K_MINWIDTH NUMBER ';'              /* 5.5 */
    {
      // 5.5 MINWIDTH, is for routing layer only
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1579, "MINWIDTH statement can only be defined in LAYER with TYPE ROUTING.  Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MINWIDTH statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1580, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMinwidth($2);
    }
  | K_MINENCLOSEDAREA NUMBER           /* 5.5 */
    {
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "MINENCLOSEDAREA statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1581, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addMinenclosedarea($2);
    }
    layer_minen_width ';'
  | K_MINIMUMCUT NUMBER K_WIDTH NUMBER /* 5.5 */
    { /* pcr 409334 */
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addMinimumcut((int)$2, $4); 
      hasLayerMincut = 0;
    }
    layer_minimumcut_within
    layer_minimumcut_from
    layer_minimumcut_length ';'
    {
      if (!hasLayerMincut) {   /* FROMABOVE nor FROMBELOW is set */
         if (lefrLayerCbk)
             lefrLayer.lefiLayer::addMinimumcutConnect((char*)"");
      }
    }
  | K_MINSTEP NUMBER               /* 5.5 */
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstep($2);
    }
    layer_minstep_options ';'      /* 5.6 */
    {
    }
  | K_PROTRUSIONWIDTH NUMBER K_LENGTH NUMBER K_WIDTH NUMBER ';'  /* 5.5 */
    {
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "PROTRUSION RULE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1582, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setProtrusion($2, $4, $6);
    }
  | K_SPACINGTABLE                    /* 5.5 */
    {
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "SPACINGTABLE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1583, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      // 5.5 either SPACING or SPACINGTABLE in a layer, not both
      if (lefrHasSpacing && layerRout && versionNum < 5.7) {
         if (lefrLayerCbk)  /* write warning only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefWarning(2010, "It is incorrect to have both SPACING rules & SPACINGTABLE rules within a ROUTING layer");
            }
      } 
      if (lefrLayerCbk) lefrLayer.lefiLayer::addSpacingTable();
      lefrHasSpacingTbl = 1;
    }
    sp_options ';'
  /* 10/12/2003 - 5.6 syntax */
  | K_ENCLOSURE layer_enclosure_type_opt NUMBER NUMBER
    {
      if (versionNum < 5.6) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ENCLOSURE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1584, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addEnclosure($2, $3, $4);
    }
    layer_enclosure_width_opt ';'
  /* 12/30/2003 - 5.6 syntax */
  | K_PREFERENCLOSURE layer_enclosure_type_opt NUMBER NUMBER
    {
      if (versionNum < 5.6) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "PREFERENCLOSURE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1585, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addPreferEnclosure($2, $3, $4);
    }
    layer_preferenclosure_width_opt ';'
  | K_RESISTANCE NUMBER ';'
    {
      if (versionNum < 5.6) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "RESISTANCE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1586, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else {
         if (lefrLayerCbk)
            lefrLayer.lefiLayer::setResPerCut($2);
      }
    }
  | K_DIAGMINEDGELENGTH NUMBER ';'
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1587, "DIAGMINEDGELENGTH can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      } else if (versionNum < 5.6) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "DIAGMINEDGELENGTH statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1588, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else {
         if (lefrLayerCbk)
            lefrLayer.lefiLayer::setDiagMinEdgeLength($2);
      }
    }
  | K_MINSIZE
    {
      // Use the polygon code to retrieve the points for MINSIZE
      lefrGeometriesPtr = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
      lefrGeometriesPtr->lefiGeometries::Init();
      lefrDoGeometries = 1;
    }
    firstPt otherPts ';' 
    {
      if (lefrLayerCbk) {
         lefrGeometriesPtr->lefiGeometries::addPolygon();
         lefrLayer.lefiLayer::setMinSize(lefrGeometriesPtr);
      }
     lefrDoGeometries = 0;
      lefrGeometriesPtr->lefiGeometries::Destroy();
                                       // Don't need the object anymore
      lefFree(lefrGeometriesPtr);
    }

layer_arraySpacing_long:            // 5.7
  /* empty */
  | K_LONGARRAY
    {
        if (lefrLayerCbk)
           lefrLayer.lefiLayer::setArraySpacingLongArray();
    }

layer_arraySpacing_width:           // 5.7
  /* empty */
  | K_WIDTH NUMBER
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::setArraySpacingWidth($2);
    }

layer_arraySpacing_arraycuts:       // 5.7
  /* empty */
  | layer_arraySpacing_arraycut layer_arraySpacing_arraycuts

layer_arraySpacing_arraycut:
  K_ARRAYCUTS NUMBER K_SPACING NUMBER
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addArraySpacingArray((int)$2, $4);
         if (arrayCutsVal > (int)$2) {
            /* Mulitiple ARRAYCUTS value needs to me in ascending order */
            if (!arrayCutsWar) {
               if (layerWarnings++ < lefrLayerWarnings)
                  lefWarning(2080, "The number of cut values in multiple ARRAYSPACING ARRAYCUTS are not in increasing order.\nTo be consistent with the documentation, update the cut values to increasing order.");
               arrayCutsWar = 1;
            }
         }
         arrayCutsVal = (int)$2;
    }

sp_options:
  K_PARALLELRUNLENGTH NUMBER
    { 
      if (hasInfluence) {  // 5.5 - INFLUENCE table must follow a PARALLEL
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1589, "An INFLUENCE table statement was defined before the PARALLELRUNLENGTH table statement.\nINFLUENCE table statement should be defined following the PARALLELRUNLENGTH.\nChange the LEF file and rerun the parser.");
              CHKERR();
            }
         }
      }
      if (hasParallel) { // 5.5 - Only one PARALLEL table is allowed per layer
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1590, "There is multiple PARALLELRUNLENGTH table statements are defined within a layer.\nAccording to the LEF Reference Manual, only one PARALLELRUNLENGTH table statement is allowed per layer.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($2);
      hasParallel = 1;
    }
    number_list
    {
      spParallelLength = lefrLayer.lefiLayer::getNumber();
      if (lefrLayerCbk) lefrLayer.lefiLayer::addSpParallelLength();
    }
    K_WIDTH NUMBER
    { 
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addSpParallelWidth($7);
      }
    }
    number_list
    { 
      if (lefrLayer.lefiLayer::getNumber() != spParallelLength) {
         if (lefrLayerCbk) {
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1591, "The number of length in the PARALLELRUNLENGTH statement is not equal to\nthe total number of spacings defined in the WIDTH statement in the SPACINGTABLE.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addSpParallelWidthSpacing();
    }
    layer_sp_parallel_widths

  | K_TWOWIDTHS K_WIDTH NUMBER layer_sp_TwoWidthsPRL NUMBER
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($5);
    }
    number_list
    {
      if (hasParallel) { // 5.7 - Either PARALLEL OR TWOWIDTHS per layer
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1592, "A PARALLELRUNLENGTH statement was already defined in the layer.\nIt is PARALLELRUNLENGTH or TWOWIDTHS is allowed per layer.");
              CHKERR();
            }
         }
      }
      if (hasTwoWidths) { // 5.7 - only 1 TWOWIDTHS per layer
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1593, "A TWOWIDTHS table statement was already defined in the layer.\nOnly one TWOWIDTHS statement is allowed per layer.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addSpTwoWidths($3, $4);
      hasTwoWidths = 1;
    }
    layer_sp_TwoWidths
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "TWOWIDTHS is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1697, outMsg);
        lefFree(outMsg);
        CHKERR();
      } 
    }
  | K_INFLUENCE K_WIDTH NUMBER K_WITHIN NUMBER K_SPACING NUMBER
    {
      if (hasInfluence) {  // 5.5 - INFLUENCE table must follow a PARALLEL
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1594, "A INFLUENCE table statement was already defined in the layer.\nOnly one INFLUENCE statement is allowed per layer.");
              CHKERR();
            }
         }
      }
      if (!hasParallel) {  // 5.5 - INFLUENCE must follow a PARALLEL
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1595, "An INFLUENCE table statement was already defined before the layer.\nINFLUENCE statement has to be defined after the PARALLELRUNLENGTH table statement in the layer.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::setInfluence();
         lefrLayer.lefiLayer::addSpInfluence($3, $5, $7);
      }
    }
    layer_sp_influence_widths

layer_spacingtable_opts:      /* 5.7 */
  /* empty */
  | layer_spacingtable_opt layer_spacingtable_opts

layer_spacingtable_opt:
  K_WITHIN NUMBER K_SPACING NUMBER
  {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addSpacingTableOrthoWithin($2, $4);
  }

layer_enclosure_type_opt: 
    {$$ = (char*)"NULL";}   /* empty */
  | K_ABOVE  {$$ = (char*)"ABOVE";}
  | K_BELOW  {$$ = (char*)"BELOW";}

layer_enclosure_width_opt:  /* empty */
  | K_WIDTH NUMBER
    {
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addEnclosureWidth($2);
      }
    }
  layer_enclosure_width_except_opt
  | K_LENGTH NUMBER              /* 5.7 */
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "LENGTH is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1691, outMsg);
         lefFree(outMsg);
         CHKERR();
      } else {
         if (lefrLayerCbk) {
            lefrLayer.lefiLayer::addEnclosureLength($2);
         }
      }
    }
    
layer_enclosure_width_except_opt: /* empty */
  | K_EXCEPTEXTRACUT NUMBER       /* 5.7 */
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "EXCEPTEXTRACUT is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1690, outMsg);
         lefFree(outMsg);
         CHKERR();
      } else {
         if (lefrLayerCbk) {
            lefrLayer.lefiLayer::addEnclosureExceptEC($2);
         }
      }
    }

layer_preferenclosure_width_opt:  /* empty */
  | K_WIDTH NUMBER
    {
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addPreferEnclosureWidth($2);
      }
    }
    
layer_minimumcut_within: /* empty */
  | K_WITHIN NUMBER
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "MINIMUMCUT WITHIN is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1700, outMsg);
        lefFree(outMsg);
        CHKERR();
      } else {
         if (lefrLayerCbk) {
            lefrLayer.lefiLayer::addMinimumcutWithin($2);
         }
      }
    }

layer_minimumcut_from: /* empty */
  | K_FROMABOVE
    {
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "FROMABOVE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1596, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      hasLayerMincut = 1;
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addMinimumcutConnect((char*)"FROMABOVE");

    }
  | K_FROMBELOW
    {
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "FROMBELOW statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1597, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      }
      hasLayerMincut = 1;
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addMinimumcutConnect((char*)"FROMBELOW");
    }

layer_minimumcut_length: /* empty */
  | K_LENGTH NUMBER K_WITHIN NUMBER
    {   
      if (versionNum < 5.5) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "LENGTH WITHIN statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1598, outMsg);
               lefFree(outMsg);
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addMinimumcutLengDis($2, $4);
    }

layer_minstep_options: /* empty */
  | layer_minstep_options layer_minstep_option  /* Use left recursions */
/* WANDA
  | layer_minstep_option layer_minstep_options
*/

layer_minstep_option:
  layer_minstep_type
  {
    if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstepType($1);
  }
  | K_LENGTHSUM NUMBER
  {
    if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstepLengthsum($2);
  }
  | K_MAXEDGES NUMBER                  /* 5.7 */
  {
    if (versionNum < 5.7) {
      outMsg = (char*)lefMalloc(10000);
      sprintf(outMsg,
        "MAXEDGES is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
      lefError(1710, outMsg);
      lefFree(outMsg);
      CHKERR();
    } else
       if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstepMaxedges((int)$2);
  }

layer_minstep_type:
  K_INSIDECORNER {$$ = (char*)"INSIDECORNER";}
  | K_OUTSIDECORNER {$$ = (char*)"OUTSIDECORNER";}
  | K_STEP {$$ = (char*)"STEP";}

layer_antenna_pwl:
  NUMBER
      { if (lefrLayerCbk)
          lefrLayer.lefiLayer::setAntennaValue(antennaType, $1); }
  | K_PWL '(' pt pt
      { if (lefrLayerCbk) { /* require min 2 points, set the 1st 2 */
          lefrAntennaPWLPtr = (lefiAntennaPWL*)lefMalloc(sizeof(lefiAntennaPWL));
          lefrAntennaPWLPtr->lefiAntennaPWL::Init();
          lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($3.x, $3.y);
          lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($4.x, $4.y);
        }
      }
    layer_diffusion_ratios ')'
      { if (lefrLayerCbk)
          lefrLayer.lefiLayer::setAntennaPWL(antennaType, lefrAntennaPWLPtr);
      }

layer_diffusion_ratios: /* empty */
  | layer_diffusion_ratios layer_diffusion_ratio  /* Use left recursions */
/* WANDA
  | layer_diffusion_ratio layer_diffusion_ratios
*/
  ;

layer_diffusion_ratio:
  pt
  { if (lefrLayerCbk)
      lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($1.x, $1.y);
  }

layer_antenna_duo: /* empty */
  | K_DIFFUSEONLY
      { 
        use5_4 = 1;
        if (ignoreVersion) {
           /* do nothing */
        }
        else if ((antennaType == lefiAntennaAF) && (versionNum <= 5.3)) {
           if (lefrLayerCbk) { /* write error only if cbk is set */
              if (layerWarnings++ < lefrLayerWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                   "ANTENNAAREAFACTOR with DIFFUSEONLY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                 lefError(1599, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        } else if (use5_3) {
           if (lefrLayerCbk) { /* write error only if cbk is set */
              if (layerWarnings++ < lefrLayerWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                   "ANTENNAAREAFACTOR with DIFFUSEONLY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                 lefError(1599, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
        if (lefrLayerCbk)
          lefrLayer.lefiLayer::setAntennaDUO(antennaType);
      }

layer_table_type:
  K_PEAK       {$$ = (char*)"PEAK";}
  | K_AVERAGE  {$$ = (char*)"AVERAGE";}
  | K_RMS      {$$ = (char*)"RMS";}

layer_frequency:
  K_FREQUENCY NUMBER
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($2); }
  number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcFrequency(); }
  ac_layer_table_opt
  K_TABLEENTRIES NUMBER
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($9); }
    number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcTableEntry(); }

ac_layer_table_opt:  /* empty */
  | K_CUTAREA NUMBER
    {
      if (!layerCut) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1600, "CUTAREA statement can only be defined in LAYER with TYPE CUT.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($2);
    }
    number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcCutarea(); }
  | K_WIDTH NUMBER
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1601, "WIDTH can only be defined in LAYER with TYPE ROUTING.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($2);
    }
    number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcWidth(); }

dc_layer_table:
  K_TABLEENTRIES NUMBER
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($2); }
    number_list ';'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addDcTableEntry(); }

number_list:
  | number_list NUMBER
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($2); }

layer_prop_list:
  layer_prop
  | layer_prop_list layer_prop
  ;

layer_prop:
  T_STRING T_STRING
    {
      if (lefrLayerCbk) {
        char propTp;
        propTp = lefrLayerProp.lefiPropType::propType($1);
        lefrLayer.lefiLayer::addProp($1, $2, propTp);
      }
    }
  | T_STRING QSTRING
    {
      if (lefrLayerCbk) {
        char propTp;
        propTp = lefrLayerProp.lefiPropType::propType($1);
        lefrLayer.lefiLayer::addProp($1, $2, propTp);
      }
    }
  | T_STRING NUMBER
    {
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (lefrLayerCbk) {
        char propTp;
        propTp = lefrLayerProp.lefiPropType::propType($1);
        lefrLayer.lefiLayer::addNumProp($1, $2, temp, propTp);
      }
    }

current_density_pwl_list :
  current_density_pwl
    { }
  | current_density_pwl_list current_density_pwl
    { }

current_density_pwl: '(' NUMBER NUMBER ')'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::setCurrentPoint($2, $3); }

cap_points :
  cap_point
  | cap_points cap_point
  ;

cap_point: '(' NUMBER NUMBER ')'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::setCapacitancePoint($2, $3); }

res_points :
  res_point
  | res_points res_point
    { }

res_point: '(' NUMBER NUMBER ')'
    { if (lefrLayerCbk) lefrLayer.lefiLayer::setResistancePoint($2, $3); }

layer_type:
  K_ROUTING       {$$ = (char*)"ROUTING"; layerRout = 1;}
  | K_CUT         {$$ = (char*)"CUT"; layerCut = 1;}
  | K_OVERLAP     {$$ = (char*)"OVERLAP"; layerMastOver = 1;}
  | K_MASTERSLICE {$$ = (char*)"MASTERSLICE"; layerMastOver = 1;}
  | K_VIRTUAL     {$$ = (char*)"VIRTUAL";}
  | K_IMPLANT     {$$ = (char*)"IMPLANT";}

layer_direction:
  K_HORIZONTAL      {$$ = (char*)"HORIZONTAL";}
  |  K_VERTICAL     {$$ = (char*)"VERTICAL";}
  |  K_DIAG45       {$$ = (char*)"DIAG45";}
  |  K_DIAG135      {$$ = (char*)"DIAG135";}

layer_minen_width:
  | K_WIDTH NUMBER
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addMinenclosedareaWidth($2);
    }

layer_oxide:
  K_OXIDE1
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(1);
    }
  | K_OXIDE2
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(2);
    }
  | K_OXIDE3
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(3);
    }
  | K_OXIDE4
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(4);
    }

layer_sp_parallel_widths: /* empty */
    { }
  | layer_sp_parallel_widths layer_sp_parallel_width  /* Use left recursions */
/* WANDA
  | layer_sp_parallel_width layer_sp_parallel_widths
*/
    { }

layer_sp_parallel_width: K_WIDTH NUMBER
    { 
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addSpParallelWidth($2);
      }
    }
    number_list
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addSpParallelWidthSpacing(); }
 
layer_sp_TwoWidths: /* empty */              // 5.7
    { }
  | layer_sp_TwoWidth layer_sp_TwoWidths
    { }

/*
layer_sp_TwoWidth: K_WIDTH NUMBER layer_sp_TwoWidthsPRL number_list   5.7
    { 
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addSpTwoWidths($2, $3);
      }
    }
*/
/*
WANDA force at list one spacing value after layer_sp_TwoWidthsPRL
*/
layer_sp_TwoWidth: K_WIDTH NUMBER layer_sp_TwoWidthsPRL NUMBER
    {
       if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber($4);
    }
    number_list 
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addSpTwoWidths($2, $3);
    }

layer_sp_TwoWidthsPRL:                       // 5.7
    { $$ = -1; }           /* cannot use 0, since PRL number can be 0 */
  | K_PRL NUMBER
    { $$ = $2; }
 
layer_sp_influence_widths: /* empty */
    { }
  | layer_sp_influence_widths layer_sp_influence_width
    { }

layer_sp_influence_width: K_WIDTH NUMBER K_WITHIN NUMBER K_SPACING NUMBER
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addSpInfluence($2, $4, $6); }

maxstack_via: K_MAXVIASTACK NUMBER ';'
    {
      if (!lefrHasLayer) {  /* 5.5 */
        if (lefrMaxStackViaCbk) { /* write error only if cbk is set */
           if (maxStackViaWarnings++ < lefrMaxStackViaWarnings) {
             lefError(1602, "MAXVIASTACK statement has to be defined after the LAYER statement.");
             CHKERR();
           }
        }
      } else if (lefrHasMaxVS) {
        if (lefrMaxStackViaCbk) { /* write error only if cbk is set */
           if (maxStackViaWarnings++ < lefrMaxStackViaWarnings) {
             lefError(1603, "A MAXVIASTACK was already defined.\nOnly one MAXVIASTACK is allowed per lef file.");
             CHKERR();
           }
        }
      } else {
        if (lefrMaxStackViaCbk) {
           lefrMaxStackVia.lefiMaxStackVia::setMaxStackVia((int)$2);
           CALLBACK(lefrMaxStackViaCbk, lefrMaxStackViaCbkType, &lefrMaxStackVia);
        }
      }
      if (versionNum < 5.5) {
        if (lefrMaxStackViaCbk) { /* write error only if cbk is set */
           if (maxStackViaWarnings++ < lefrMaxStackViaWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "MAXVIASTACK statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1604, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      lefrHasMaxVS = 1;
    }
  | K_MAXVIASTACK NUMBER K_RANGE {lefDumbMode = 2; lefNoNum= 2;}
    T_STRING T_STRING ';'
    {
      if (!lefrHasLayer) {  /* 5.5 */
        if (lefrMaxStackViaCbk) { /* write error only if cbk is set */
           if (maxStackViaWarnings++ < lefrMaxStackViaWarnings) {
              lefError(1602, "MAXVIASTACK statement has to be defined after the LAYER statement.");
              CHKERR();
           }
        }
      } else if (lefrHasMaxVS) {
        if (lefrMaxStackViaCbk) { /* write error only if cbk is set */
           if (maxStackViaWarnings++ < lefrMaxStackViaWarnings) {
             lefError(1603, "A MAXVIASTACK was already defined.\nOnly one MAXVIASTACK is allowed per lef file.");
             CHKERR();
           }
        }
      } else {
        if (lefrMaxStackViaCbk) {
           lefrMaxStackVia.lefiMaxStackVia::setMaxStackVia((int)$2);
           lefrMaxStackVia.lefiMaxStackVia::setMaxStackViaRange($5, $6);
           CALLBACK(lefrMaxStackViaCbk, lefrMaxStackViaCbkType, &lefrMaxStackVia);
        }
      }
      lefrHasMaxVS = 1;
    }

via: start_via  { hasViaRule_layer = 0; } via_option end_via
    { 
      if (lefrViaCbk)
        CALLBACK(lefrViaCbk, lefrViaCbkType, &lefrVia);
    }

via_keyword : K_VIA                 /*needed to have a VIA named via*/
     { lefDumbMode = 1; lefNoNum = 1; }

start_via: via_keyword T_STRING 
    {
      /* 0 is nodefault */
      if (lefrViaCbk) lefrVia.lefiVia::setName($2, 0);
      viaLayer = 0;
      numVia++;
      //strcpy(viaName, $2);
      viaName = strdup($2);
    }
  | via_keyword T_STRING K_DEFAULT
    {
      /* 1 is default */
      if (lefrViaCbk) lefrVia.lefiVia::setName($2, 1);
      viaLayer = 0;
      //strcpy(viaName, $2);
      viaName = strdup($2);
    }
  | via_keyword T_STRING K_GENERATED
    {
      /* 2 is generated */
      if (lefrViaCbk) lefrVia.lefiVia::setName($2, 2);
      viaLayer = 0;
      //strcpy(viaName, $2);
      viaName = strdup($2);
    }

via_viarule: K_VIARULE {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
  K_CUTSIZE NUMBER NUMBER ';'
  K_LAYERS {lefDumbMode = 3; lefNoNum = 1; } T_STRING T_STRING T_STRING ';'
  K_CUTSPACING NUMBER NUMBER ';'
  K_ENCLOSURE NUMBER NUMBER NUMBER NUMBER ';'
    {
       if (versionNum < 5.6) {
         if (lefrViaCbk) { /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "VIARULE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1709, outMsg);
              lefFree(outMsg);
              CHKERR();
            }
         }
       }  else
          if (lefrViaCbk) lefrVia.lefiVia::setViaRule($3, $6, $7, $11, $12, $13,
                          $16, $17, $20, $21, $22, $23);
       viaLayer++;
       hasViaRule_layer = 1;
    }
  via_viarule_options
  ;

via_viarule_options: /* empty */
  | via_viarule_options via_viarule_option
  ;

via_viarule_option: K_ROWCOL NUMBER NUMBER ';'
    {
       if (lefrViaCbk) lefrVia.lefiVia::setRowCol((int)$2, (int)$3);
    }
  | K_ORIGIN NUMBER NUMBER ';'
    {
       if (lefrViaCbk) lefrVia.lefiVia::setOrigin($2, $3);
    }
  | K_OFFSET NUMBER NUMBER NUMBER NUMBER ';'
    {
       if (lefrViaCbk) lefrVia.lefiVia::setOffset($2, $3, $4, $5);
    }
  | K_PATTERN {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
       if (lefrViaCbk) lefrVia.lefiVia::setPattern($3);
    }
  ;

via_option: via_viarule
  | via_other_options

via_other_options: via_other_option
  via_more_options

via_more_options: /* empty */
  | via_more_options via_other_option
  ;

via_other_option:
  via_foreign 
    { }
  | via_layer_rule 
    { }
  | K_RESISTANCE NUMBER ';'
    { if (lefrViaCbk) lefrVia.lefiVia::setResistance($2); }
  | K_PROPERTY { lefDumbMode = 1000000; lefRealNum = 1; lefInProp = 1; } via_prop_list ';'
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    }
  | K_TOPOFSTACKONLY
    { 
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setTopOfStack();
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
              lefWarning(2019, "TOPOFSTACKONLY statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later");
    }

via_prop_list:
  via_name_value_pair
  | via_prop_list via_name_value_pair
  ;

via_name_value_pair:
  T_STRING NUMBER
    { 
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (lefrViaCbk) {
         char propTp;
         propTp = lefrViaProp.lefiPropType::propType($1);
         lefrVia.lefiVia::addNumProp($1, $2, temp, propTp);
      }
    }
  | T_STRING QSTRING
    {
      if (lefrViaCbk) {
         char propTp;
         propTp = lefrViaProp.lefiPropType::propType($1);
         lefrVia.lefiVia::addProp($1, $2, propTp);
      }
    }
  | T_STRING T_STRING
    {
      if (lefrViaCbk) {
         char propTp;
         propTp = lefrViaProp.lefiPropType::propType($1);
         lefrVia.lefiVia::addProp($1, $2, propTp);
      }
    }

via_foreign:
  start_foreign ';'
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign($1, 0, 0.0, 0.0, -1);
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign pt ';'
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign($1, 1, $2.x, $2.y, -1);
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign pt orientation ';'
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign($1, 1, $2.x, $2.y, $3);
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign orientation ';'
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign($1, 0, 0.0, 0.0, $2);
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

start_foreign:	K_FOREIGN {lefDumbMode = 1; lefNoNum= 1;} T_STRING
    { $$ = $3; }

orientation:
  K_N         {$$ = 0;}
  | K_W       {$$ = 1;}
  | K_S       {$$ = 2;}
  | K_E       {$$ = 3;}
  | K_FN      {$$ = 4;}
  | K_FW      {$$ = 5;}
  | K_FS      {$$ = 6;}
  | K_FE      {$$ = 7;}
  | K_R0      {$$ = 0;}
  | K_R90     {$$ = 1;}
  | K_R180    {$$ = 2;}
  | K_R270    {$$ = 3;}
  | K_MY      {$$ = 4;}
  | K_MYR90   {$$ = 5;}
  | K_MX      {$$ = 6;}
  | K_MXR90   {$$ = 7;}

via_layer_rule: via_layer via_geometries
    { }

via_layer: K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
      if (lefrViaCbk) lefrVia.lefiVia::addLayer($3);
      viaLayer++;
      hasViaRule_layer = 1;
    }

via_geometries:
  /* empty */
  | via_geometries via_geometry 
  ;

via_geometry:
  K_RECT maskColor pt pt ';'
    { 
      if (lefrViaCbk) {
        if (versionNum < 5.8 && (int)$2 > 0) {
          if (viaWarnings++ < lefrViaWarnings) {
              lefError(2081, "MASK information can only be defined with version 5.8");
              CHKERR(); 
            }           
        } else {
          lefrVia.lefiVia::addRectToLayer((int)$2, $3.x, $3.y, $4.x, $4.y);
        }
      }
    }
  | K_POLYGON maskColor                                              // 5.6
    {
      lefrGeometriesPtr = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
      lefrGeometriesPtr->lefiGeometries::Init();
      lefrDoGeometries = 1;
    }
    firstPt nextPt nextPt otherPts ';'
    { 
      if (lefrViaCbk) {
        if (versionNum < 5.8 && $2 > 0) {
          if (viaWarnings++ < lefrViaWarnings) {
              lefError(2083, "Color mask information can only be defined with version 5.8.");
              CHKERR(); 
            }           
        } else {
            lefrGeometriesPtr->lefiGeometries::addPolygon((int)$2);
            lefrVia.lefiVia::addPolyToLayer((int)$2, lefrGeometriesPtr);   // 5.6
        }
      }
      lefrGeometriesPtr->lefiGeometries::clearPolyItems(); // free items fields
      lefFree((char*)(lefrGeometriesPtr)); // Don't need anymore, poly data has
      lefrDoGeometries = 0;                // copied
    }

end_via: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING 
    { 
      // 10/17/2001 - Wanda da Rosa, PCR 404149
      //              Error if no layer in via
      if (!viaLayer) {
         if (lefrViaCbk) {  /* write error only if cbk is set */
            if (viaWarnings++ < lefrViaWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "A LAYER statement is missing in the VIA %s.\nAt least one LAYERis required per VIA statement.", $3);
              lefError(1606, outMsg);
              lefFree(outMsg);
              CHKERR();
            }
         }
      }
      if (strcmp(viaName, $3) != 0) {
         if (lefrViaCbk) { /* write error only if cbk is set */
            if (viaWarnings++ < lefrViaWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "END VIA name %s is different from the VIA name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, viaName);
              lefError(1607, outMsg);
              lefFree(outMsg);
              lefFree(viaName);
              CHKERR();
            } else
              lefFree(viaName);
         } else
            lefFree(viaName);
      } else
         lefFree(viaName);
    }

viarule_keyword : K_VIARULE { lefDumbMode = 1; lefNoNum = 1;} T_STRING
    { 
      if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setName($3);
      viaRuleLayer = 0;
      //strcpy(viaRuleName, $3);
      viaRuleName = strdup($3);
      isGenerate = 0;
    }

viarule:
  viarule_keyword viarule_layer_list via_names opt_viarule_props end_viarule
    {
      // 10/17/2001 - Wanda da Rosa, PCR 404163
      //              Error if layer number is not equal 2.
      // 11/14/2001 - Wanda da Rosa,
      //              Commented out for pcr 411781
      //if (viaRuleLayer != 2) {
         //lefError("VIARULE requires two layers");
         //CHKERR();
      //}
      if (viaRuleLayer == 0 || viaRuleLayer > 2) {
         if (lefrViaRuleCbk) {  /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              lefError(1608, "A VIARULE statement requires two layers.");
              CHKERR();
            }
         }
      }
      if (lefrViaRuleCbk)
        CALLBACK(lefrViaRuleCbk, lefrViaRuleCbkType, &lefrViaRule);
      // 2/19/2004 - reset the ENCLOSURE overhang values which may be
      // set by the old syntax OVERHANG -- Not necessary, but just incase
      if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::clearLayerOverhang();
    }

viarule_generate:
  viarule_keyword K_GENERATE viarule_generate_default
    {
      isGenerate = 1;
    }
  viarule_layer_list opt_viarule_props end_viarule
    {
      // 10/17/2001 - Wanda da Rosa, PCR 404181
      //              Error if layer number is not equal 3.
      // 11/14/2001 - Wanda da Rosa,
      //              Commented out for pcr 411781
      //if (viaRuleLayer != 3) {
         //lefError("VIARULE requires three layers");
         //CHKERR();
      //}
      if (viaRuleLayer == 0) {
         if (lefrViaRuleCbk) {  /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              lefError(1708, "A VIARULE GENERATE requires three layers.");
              CHKERR();
            }
         }
      } else if ((viaRuleLayer < 3) && (versionNum >= 5.6)) {
         if (lefrViaRuleCbk)  /* write warning only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings)
              lefWarning(2021, "turn-via is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else {
         if (lefrViaRuleCbk) {
            lefrViaRule.lefiViaRule::setGenerate();
            CALLBACK(lefrViaRuleCbk, lefrViaRuleCbkType, &lefrViaRule);
         }
      }
      // 2/19/2004 - reset the ENCLOSURE overhang values which may be
      // set by the old syntax OVERHANG
      if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::clearLayerOverhang();
    }

viarule_generate_default:  /* optional */
  | K_DEFAULT   // 5.6 syntax
    {
      if (versionNum < 5.6) {
         if (lefrViaRuleCbk) {  /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "DEFAULT statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1605, outMsg);
              lefFree(outMsg);
              CHKERR();
            }
         }
      } else
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setDefault();
    }
 
viarule_layer_list :
  viarule_layer
  | viarule_layer_list viarule_layer
  ;

opt_viarule_props:
  /* empty */
  | viarule_props
  ;

viarule_props:
  viarule_prop
  | viarule_props viarule_prop
  ;

viarule_prop: K_PROPERTY { lefDumbMode = 10000000; lefRealNum = 1; lefInProp = 1; } viarule_prop_list ';'
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    }

viarule_prop_list:
  viarule_prop
  | viarule_prop_list viarule_prop
  ;

viarule_prop:
  T_STRING T_STRING
    {
      if (lefrViaRuleCbk) {
         char propTp;
         propTp = lefrViaRuleProp.lefiPropType::propType($1);
         lefrViaRule.lefiViaRule::addProp($1, $2, propTp);
      }
    }
  | T_STRING QSTRING
    {
      if (lefrViaRuleCbk) {
         char propTp;
         propTp = lefrViaRuleProp.lefiPropType::propType($1);
         lefrViaRule.lefiViaRule::addProp($1, $2, propTp);
      }
    }
  | T_STRING NUMBER
    {
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (lefrViaRuleCbk) {
         char propTp;
         propTp = lefrViaRuleProp.lefiPropType::propType($1);
         lefrViaRule.lefiViaRule::addNumProp($1, $2, temp, propTp);
      }
    }

viarule_layer: viarule_layer_name viarule_layer_options
    {
      // 10/18/2001 - Wanda da Rosa PCR 404181
      //              Make sure the 1st 2 layers in viarule has direction
      // 04/28/2004 - PCR 704072 - DIRECTION in viarule generate is
      //              obsolete in 5.6
      if (versionNum >= 5.6) {
         if (viaRuleLayer < 2 && !viaRuleHasDir && !viaRuleHasEnc &&
             !isGenerate) {
            if (lefrViaRuleCbk) {  /* write error only if cbk is set */
               if (viaRuleWarnings++ < lefrViaRuleWarnings) {
                  lefError(1705, "VIARULE statement in a layer, requires a DIRECTION construct statement.");
                  CHKERR(); 
               }
            }
         }
      } else {
         if (viaRuleLayer < 2 && !viaRuleHasDir && !viaRuleHasEnc &&
             isGenerate) {
            if (lefrViaRuleCbk) {  /* write error only if cbk is set */
               if (viaRuleWarnings++ < lefrViaRuleWarnings) {
                  lefError(1705, "VIARULE statement in a layer, requires a DIRECTION construct statement.");
                  CHKERR(); 
               }
            }
         }
      }
      viaRuleLayer++;
    }
  ;

via_names:
  /* empty */
  | via_names via_name
  ;

via_name: via_keyword T_STRING ';'
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::addViaName($2); }

viarule_layer_name: K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setLayer($3);
      viaRuleHasDir = 0;
      viaRuleHasEnc = 0;
    }

viarule_layer_options:
  /* empty */
  | viarule_layer_options viarule_layer_option
  ;

viarule_layer_option:
  K_DIRECTION K_HORIZONTAL ';'
    {
      if (viaRuleHasEnc) {
        if (lefrViaRuleCbk) {  /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              lefError(1706, "An ENCLOSRE statement was already defined in the layer.\nIt is DIRECTION or ENCLOSURE can be specified in a layer.");
              CHKERR();
           }
        }
      } else {
        if ((versionNum < 5.6) || (!isGenerate)) {
          if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setHorizontal();
        } else
          if (lefrViaRuleCbk)  /* write warning only if cbk is set */
             if (viaRuleWarnings++ < lefrViaRuleWarnings)
               lefWarning(2022, "DIRECTION statement in VIARULE is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      }
      viaRuleHasDir = 1;
    }
  | K_DIRECTION K_VERTICAL ';'
    { 
      if (viaRuleHasEnc) {
        if (lefrViaRuleCbk) { /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              lefError(1706, "An ENCLOSRE statement was already defined in the layer.\nIt is DIRECTION or ENCLOSURE can be specified in a layer.");
              CHKERR();
           }
        }
      } else {
        if ((versionNum < 5.6) || (!isGenerate)) {
          if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setVertical();
        } else
          if (lefrViaRuleCbk) /* write warning only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings)
              lefWarning(2022, "DIRECTION statement in VIARULE is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      }
      viaRuleHasDir = 1;
    }
  | K_ENCLOSURE NUMBER NUMBER ';'    /* 5.5 */
    {
      if (versionNum < 5.5) {
         if (lefrViaRuleCbk) { /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "ENCLOSURE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1707, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
         }
      }
      // 2/19/2004 - Enforced the rule that ENCLOSURE can only be defined
      // in VIARULE GENERATE
      if (!isGenerate) {
         if (lefrViaRuleCbk) { /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              lefError(1614, "An ENCLOSURE statement is defined in a VIARULE statement only.\nOVERHANG statement can only be defined in VIARULE GENERATE.");
              CHKERR();
           }
         }
      }
      if (viaRuleHasDir) {
         if (lefrViaRuleCbk) { /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              lefError(1609, "A DIRECTION statement was already defined in the layer.\nIt is DIRECTION or ENCLOSURE can be specified in a layer.");
              CHKERR();
           }
         }
      } else {
         if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setEnclosure($2, $3);
      }
      viaRuleHasEnc = 1;
    }
  | K_WIDTH NUMBER K_TO NUMBER ';'
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setWidth($2,$4); }
  | K_RECT pt pt ';'
    { if (lefrViaRuleCbk)
	lefrViaRule.lefiViaRule::setRect($2.x, $2.y, $3.x, $3.y); } 
  | K_SPACING NUMBER K_BY NUMBER ';'
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setSpacing($2,$4); }
  | K_RESISTANCE NUMBER ';'
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setResistance($2); }
  | K_OVERHANG NUMBER ';'
    {
      if (!viaRuleHasDir) {
         if (lefrViaRuleCbk) {  /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
               lefError(1610, "An OVERHANG statement is defined, but the required DIRECTION statement is not yet defined.\nUpdate the LEF file to define the DIRECTION statement before the OVERHANG.");
               CHKERR();
            }
         }
      }
      // 2/19/2004 - Enforced the rule that OVERHANG can only be defined
      // in VIARULE GENERATE after 5.3
      if ((versionNum > 5.3) && (!isGenerate)) {
         if (lefrViaRuleCbk) {  /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
               lefError(1611, "An OVERHANG statement is defined in a VIARULE statement only.\nOVERHANG statement can only be defined in VIARULE GENERATE.");
               CHKERR();
            }
         }
      }
      if (versionNum < 5.6) {
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setOverhang($2);
      } else {
        if (lefrViaRuleCbk)  /* write warning only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings)
              lefWarning(2023, "OVERHANG statement will be translated into similar ENCLOSURE rule");
        // In 5.6 & later, set it to either ENCLOSURE overhang1 or overhang2
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setOverhangToEnclosure($2);
      }
    }
  | K_METALOVERHANG NUMBER ';'
    {
      // 2/19/2004 - Enforced the rule that METALOVERHANG can only be defined
      // in VIARULE GENERATE
      if ((versionNum > 5.3) && (!isGenerate)) {
         if (lefrViaRuleCbk) {  /* write error only if cbk is set */
            if (viaRuleWarnings++ < lefrViaRuleWarnings) {
               lefError(1612, "An METALOVERHANG statement is defined in a VIARULE statement only.\nOVERHANG statement can only be defined in VIARULE GENERATE.");
               CHKERR();
            }
         }
      }
      if (versionNum < 5.6) {
        if (!viaRuleHasDir) {
           if (lefrViaRuleCbk) {  /* write error only if cbk is set */
             if (viaRuleWarnings++ < lefrViaRuleWarnings) {
                lefError(1613, "An METALOVERHANG statement is defined, but the required DIRECTION statement is not yet defined.\nUpdate the LEF file to define the DIRECTION statement before the OVERHANG.");
                CHKERR();
             } 
           }
        }
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setMetalOverhang($2);
      } else
        if (lefrViaRuleCbk)  /* write warning only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings)
             lefWarning(2024, "METALOVERHANG statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

end_viarule: K_END {lefDumbMode = 1; lefNoNum = 1;}  T_STRING 
    {
      if (strcmp(viaRuleName, $3) != 0) {
        if (lefrViaRuleCbk) {  /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END VIARULE name %s is different from the VIARULE name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, viaRuleName);
              lefError(1615, outMsg);
              lefFree(outMsg);
              lefFree(viaRuleName);
              CHKERR();
           } else
              lefFree(viaRuleName);
        } else
           lefFree(viaRuleName);
      } else
        lefFree(viaRuleName);
    }

spacing_rule: start_spacing spacings end_spacing 
    { }

start_spacing: K_SPACING
    {
      hasSamenet = 0;
      if ((versionNum < 5.6) || (!ndRule)) {
        /* if 5.6 and in nondefaultrule, it should not get in here, */
        /* it should go to the else statement to write out a warning */
        /* if 5.6, not in nondefaultrule, it will get in here */
        /* if 5.5 and earlier in nondefaultrule is ok to get in here */
        if (versionNum >= 5.7) { /* will get to this if statement if */ 
                           /* versionNum is 5.6 and higher but ndRule = 0 */
           if (spacingWarnings == 0) {  /* only print once */
              lefWarning(2077, "A SPACING SAMENET section is defined but it is not legal in a LEF 5.7 version file.\nIt will be ignored which will probably cause real DRC violations to be ignored, and may\ncause false DRC violations to occur.\n\nTo avoid this warning, and correctly handle these DRC rules, you should modify your\nLEF to use the appropriate SAMENET keywords as described in the LEF/DEF 5.7\nmanual under the SPACING statements in the LAYER (Routing) and LAYER (Cut)\nsections listed in the LEF Table of Contents.");
              spacingWarnings++;
           }
        } else if (lefrSpacingBeginCbk)
	  CALLBACK(lefrSpacingBeginCbk, lefrSpacingBeginCbkType, 0);
      } else
        if (lefrSpacingBeginCbk)  /* write warning only if cbk is set */
           if (spacingWarnings++ < lefrSpacingWarnings)
             lefWarning(2025, "SAMENET statement in NONDEFAULTRULE is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

end_spacing: K_END K_SPACING
    {
      if ((versionNum < 5.6) || (!ndRule)) {
        if ((versionNum <= 5.4) && (!hasSamenet)) {
           lefError(1616, "SAMENET statement is required inside SPACING for any lef file with version 5.4 and earlier, but is not defined in the parsed lef file.");
           CHKERR();
        } else if (versionNum < 5.7) { /* obsolete in 5.7 and later */
           if (lefrSpacingEndCbk)
             CALLBACK(lefrSpacingEndCbk, lefrSpacingEndCbkType, 0);
        }
      }
    }

spacings:
  /* empty */
  | spacings spacing
  ;

spacing:  samenet_keyword T_STRING T_STRING NUMBER ';'
    {
      if ((versionNum < 5.6) || (!ndRule)) {
        if (versionNum < 5.7) {
          if (lefrSpacingCbk) {
            lefrSpacing.lefiSpacing::set($2, $3, $4, 0);
            CALLBACK(lefrSpacingCbk, lefrSpacingCbkType, &lefrSpacing);
          }
        }
      }
    }
  | samenet_keyword T_STRING T_STRING NUMBER K_STACK ';'
    {
      if ((versionNum < 5.6) || (!ndRule)) {
        if (versionNum < 5.7) {
          if (lefrSpacingCbk) {
	    lefrSpacing.lefiSpacing::set($2, $3, $4, 1);
	    CALLBACK(lefrSpacingCbk, lefrSpacingCbkType, &lefrSpacing);
          }
        }
      }
    }

samenet_keyword: K_SAMENET
    /* must be followed by two names */
    { lefDumbMode = 2; lefNoNum = 2; hasSamenet = 1; }
     
maskColor:
    /* empty */
    { $$ = 0; }
    | K_MASK NUMBER
    { $$ = (int)$2; }
            
irdrop: start_irdrop ir_tables end_irdrop
    { }

start_irdrop: K_IRDROP
    {
      if (versionNum < 5.4) {
        if (lefrIRDropBeginCbk) 
	  CALLBACK(lefrIRDropBeginCbk, lefrIRDropBeginCbkType, 0);
      } else
        if (lefrIRDropBeginCbk) /* write warning only if cbk is set */
          if (iRDropWarnings++ < lefrIRDropWarnings)
            lefWarning(2026, "IRDROP statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

end_irdrop: K_END K_IRDROP
    {
      if (versionNum < 5.4) {
        if (lefrIRDropEndCbk)
	  CALLBACK(lefrIRDropEndCbk, lefrIRDropEndCbkType, 0);
      }
    }
      

ir_tables:
  /* empty */
  | ir_tables ir_table
  ;

ir_table: ir_tablename ir_table_values ';'
    { 
      if (versionNum < 5.4) {
        if (lefrIRDropCbk)
          CALLBACK(lefrIRDropCbk, lefrIRDropCbkType, &lefrIRDrop);
      }
    }

ir_table_values:
  /* empty */
  | ir_table_values ir_table_value 
  ;

ir_table_value: NUMBER NUMBER 
  { if (lefrIRDropCbk) lefrIRDrop.lefiIRDrop::setValues($1, $2); }

ir_tablename: K_TABLE T_STRING
  { if (lefrIRDropCbk) lefrIRDrop.lefiIRDrop::setTableName($2); }

minfeature: K_MINFEATURE NUMBER NUMBER ';'
  {
    hasMinfeature = 1;
    if (versionNum < 5.4) {
       if (lefrMinFeatureCbk) {
         lefrMinFeature.lefiMinFeature::set($2, $3);
         CALLBACK(lefrMinFeatureCbk, lefrMinFeatureCbkType, &lefrMinFeature);
       }
    } else
       if (lefrMinFeatureCbk) /* write warning only if cbk is set */
          if (minFeatureWarnings++ < lefrMinFeatureWarnings)
            lefWarning(2027, "MINFEATURE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }

dielectric: K_DIELECTRIC NUMBER ';'
  {
    if (versionNum < 5.4) {
       if (lefrDielectricCbk)
         CALLBACK(lefrDielectricCbk, lefrDielectricCbkType, $2);
    } else
       if (lefrDielectricCbk) /* write warning only if cbk is set */
         if (dielectricWarnings++ < lefrDielectricWarnings)
           lefWarning(2028, "DIELECTRIC statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }

nondefault_rule: K_NONDEFAULTRULE {lefDumbMode = 1; lefNoNum = 1;} T_STRING
  {
    (void)lefSetNonDefault($3);
    if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::setName($3);
    ndLayer = 0;
    ndRule = 1;
    numVia = 0;
    //strcpy(nonDefaultRuleName, $3);
    nonDefaultRuleName = strdup($3);
  }
  nd_hardspacing
  nd_rules {lefNdRule = 1;} end_nd_rule
  {
    // 10/18/2001 - Wanda da Rosa, PCR 404189
    //              At least 1 layer is required
    if ((!ndLayer) && (!lefrRelaxMode)) {
       if (lefrNonDefaultCbk) { /* write error only if cbk is set */
         if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
            lefError(1617, "NONDEFAULTRULE statement requires at least one LAYER statement.");
            CHKERR();
         }
       }
    }
    if ((!numVia) && (!lefrRelaxMode) && (versionNum < 5.6)) {
       // VIA is no longer a required statement in 5.6
       if (lefrNonDefaultCbk) { /* write error only if cbk is set */
         if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
            lefError(1618, "NONDEFAULTRULE statement requires at least one VIA statement.");
            CHKERR();
         }
       }
    }
    if (lefrNonDefaultCbk) {
      lefrNonDefault.lefiNonDefault::end();
      CALLBACK(lefrNonDefaultCbk, lefrNonDefaultCbkType, &lefrNonDefault);
    }
    ndRule = 0;
    lefDumbMode = 0;
    (void)lefUnsetNonDefault();
  }

end_nd_rule: K_END
    {
      if ((nonDefaultRuleName) && (*nonDefaultRuleName != '\0'))
        lefFree(nonDefaultRuleName);
    }
  | K_END T_STRING
    {
      if (strcmp(nonDefaultRuleName, $2) != 0) {
        if (lefrNonDefaultCbk) { /* write error only if cbk is set */
          if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
             outMsg = (char*)lefMalloc(10000);
             sprintf (outMsg,
                "END NONDEFAULTRULE name %s is different from the NONDEFAULTRULE name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $2, nonDefaultRuleName);
             lefError(1619, outMsg);
             lefFree(nonDefaultRuleName);
             lefFree(outMsg);
             CHKERR();
          } else
             lefFree(nonDefaultRuleName);
        } else
           lefFree(nonDefaultRuleName);
      } else
        lefFree(nonDefaultRuleName);
    }
  ;

nd_hardspacing:
  /* empty */
  | K_HARDSPACING ';'   /* HARDSPACING is optional in 5.6 */
    {
       if (versionNum < 5.6) {
          if (lefrNonDefaultCbk) { /* write error only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "HARDSPACING statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1620, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
          }
       } else 
          if (lefrNonDefaultCbk)
             lefrNonDefault.lefiNonDefault::setHardspacing();
    }
  ;

nd_rules: /* empty */
  | nd_rules nd_rule
  ;

nd_rule:
  nd_layer
  | via
  | spacing_rule
  | nd_prop
  | usevia
  | useviarule
  | mincuts
  ;

usevia: K_USEVIA T_STRING ';'
    {
       if (versionNum < 5.6) {
          if (lefrNonDefaultCbk) { /* write error only if cbk is set */
             outMsg = (char*)lefMalloc(10000);
             sprintf (outMsg,
               "USEVIA statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
             lefError(1621, outMsg);
             lefFree(outMsg);
             CHKERR();
          }
       } else {
          if (lefrNonDefaultCbk)
             lefrNonDefault.lefiNonDefault::addUseVia($2);
       }
    }

useviarule:  K_USEVIARULE T_STRING ';'
    {
       if (versionNum < 5.6) {
          if (lefrNonDefaultCbk) { /* write error only if cbk is set */
             if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
                outMsg = (char*)lefMalloc(10000);
                sprintf (outMsg,
                  "USEVIARULE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                lefError(1622, outMsg);
                lefFree(outMsg);
                CHKERR();
             }
          }
       } else {
          if (lefrNonDefaultCbk)
             lefrNonDefault.lefiNonDefault::addUseViaRule($2);
       }
    }

mincuts: K_MINCUTS T_STRING NUMBER ';'
    {
       if (versionNum < 5.6) {
          if (lefrNonDefaultCbk) { /* write error only if cbk is set */
             if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
                outMsg = (char*)lefMalloc(10000);
                sprintf (outMsg,
                  "MINCUTS statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                lefError(1623, outMsg);
                lefFree(outMsg);
                CHKERR();
             }
          }
       } else {
          if (lefrNonDefaultCbk)
             lefrNonDefault.lefiNonDefault::addMinCuts($2, (int)$3);
       }
    }

nd_prop: K_PROPERTY { lefDumbMode = 10000000; lefRealNum = 1; lefInProp = 1; } nd_prop_list ';'
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    }

nd_prop_list:
  nd_prop
  | nd_prop_list nd_prop
  ;

nd_prop:
  T_STRING T_STRING
    {
      if (lefrNonDefaultCbk) {
         char propTp;
         propTp = lefrNondefProp.lefiPropType::propType($1);
         lefrNonDefault.lefiNonDefault::addProp($1, $2, propTp);
      }
    }
  | T_STRING QSTRING
    {
      if (lefrNonDefaultCbk) {
         char propTp;
         propTp = lefrNondefProp.lefiPropType::propType($1);
         lefrNonDefault.lefiNonDefault::addProp($1, $2, propTp);
      }
    }
  | T_STRING NUMBER
    {
      if (lefrNonDefaultCbk) {
         char temp[32];
         char propTp;
         sprintf(temp, "%.11g", $2);
         propTp = lefrNondefProp.lefiPropType::propType($1);
         lefrNonDefault.lefiNonDefault::addNumProp($1, $2, temp, propTp);
      }
    }

nd_layer: K_LAYER {lefDumbMode = 1; lefNoNum = 1;} T_STRING
  {
    if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::addLayer($3);
    ndLayer++;
    //strcpy(layerName, $3);
    layerName = strdup($3);
    ndLayerWidth = 0;
    ndLayerSpace = 0;
  }
  K_WIDTH NUMBER ';'
  { 
    ndLayerWidth = 1;
    if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::addWidth($6);
  }
  nd_layer_stmts K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
  {
    if (strcmp(layerName, $12) != 0) {
      if (lefrNonDefaultCbk) { /* write error only if cbk is set */
         if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
            outMsg = (char*)lefMalloc(10000);
            sprintf (outMsg,
               "END LAYER name %s is different from the LAYER name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, layerName);
            lefError(1624, outMsg);
            lefFree(outMsg);
            lefFree(layerName);
            CHKERR();
         } else
            lefFree(layerName);
      } else
         lefFree(layerName);
    } else
      lefFree(layerName);
    if (!ndLayerWidth) {
      if (lefrNonDefaultCbk) { /* write error only if cbk is set */
         if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
            lefError(1625, "A WIDTH statement is required in the LAYER statement in NONDEFULTRULE.");
            CHKERR();
         }
      }
    }
    if (!ndLayerSpace && versionNum < 5.6) {   // 5.6, SPACING is optional
      if (lefrNonDefaultCbk) { /* write error only if cbk is set */
         if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
            outMsg = (char*)lefMalloc(10000);
            sprintf (outMsg,
               "A SPACING statement is required in the LAYER statement in NONDEFAULTRULE for lef file with version 5.5 and earlier.\nYour lef file is defined with version %g. Update your lef to add a LAYER statement and try again.",
                versionNum);
            lefError(1626, outMsg);
            lefFree(outMsg);
            CHKERR();
         }
      }
    }
  }
  ;

nd_layer_stmts:
  /* empty */
  | nd_layer_stmts nd_layer_stmt
  ;

nd_layer_stmt:
  K_SPACING NUMBER ';'
    {
      ndLayerSpace = 1;
      if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::addSpacing($2);
    }
  | K_WIREEXTENSION NUMBER ';'
    { if (lefrNonDefaultCbk)
         lefrNonDefault.lefiNonDefault::addWireExtension($2); }
  | K_RESISTANCE K_RPERSQ NUMBER ';'
    {
      if (ignoreVersion) {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addResistance($3);
      } else if (versionNum < 5.4) {
         if (lefrNonDefaultCbk) { /* write error only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "RESISTANCE RPERSQ statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1627, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (versionNum > 5.5) {  // obsolete in 5.6
         if (lefrNonDefaultCbk) /* write warning only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings)
              lefWarning(2029, "RESISTANCE RPERSQ statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else if (lefrNonDefaultCbk)
         lefrNonDefault.lefiNonDefault::addResistance($3);
    } 
 
  | K_CAPACITANCE K_CPERSQDIST NUMBER ';'
    {
      if (ignoreVersion) {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addCapacitance($3);
      } else if (versionNum < 5.4) {
         if (lefrNonDefaultCbk) { /* write error only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "CAPACITANCE CPERSQDIST statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1628, outMsg);
               lefFree(outMsg);
               CHKERR();
            }
         }
      } else if (versionNum > 5.5) { // obsolete in 5.6
         if (lefrNonDefaultCbk) /* write warning only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings)
              lefWarning(2030, "CAPACITANCE CPERSQDIST statement is obsolete in version 5.6. and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else if (lefrNonDefaultCbk)
         lefrNonDefault.lefiNonDefault::addCapacitance($3);
    }
  | K_EDGECAPACITANCE NUMBER ';'
    {
      if (ignoreVersion) {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addEdgeCap($2);
      } else if (versionNum < 5.4) {
         if (lefrNonDefaultCbk) { /* write error only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "EDGECAPACITANCE statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1629, outMsg);
               lefFree(outMsg);
              CHKERR();
            }
         }
      } else if (versionNum > 5.5) {  // obsolete in 5.6
         if (lefrNonDefaultCbk) /* write warning only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings)
              lefWarning(2031, "EDGECAPACITANCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else if (lefrNonDefaultCbk)
         lefrNonDefault.lefiNonDefault::addEdgeCap($2);
    }
  | K_DIAGWIDTH NUMBER ';'
    {
      if (versionNum < 5.6) {  // 5.6 syntax
         if (lefrNonDefaultCbk) { /* write error only if cbk is set */
            if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                 "DIAGWIDTH statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
               lefError(1630, outMsg);
               lefFree(outMsg);
               CHKERR(); 
            }
         }
      } else {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addDiagWidth($2);
      }
    }

site: start_site site_options end_site
    { 
      if (lefrSiteCbk)
        CALLBACK(lefrSiteCbk, lefrSiteCbkType, &lefrSite);
    }

start_site: K_SITE {lefDumbMode = 1; lefNoNum = 1;} T_STRING 
    { 
      if (lefrSiteCbk) lefrSite.lefiSite::setName($3);
      //strcpy(siteName, $3);
      siteName = strdup($3);
      hasSiteClass = 0;
      hasSiteSize = 0;
      hasSite = 1;
    }

end_site: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
    {
      if (strcmp(siteName, $3) != 0) {
        if (lefrSiteCbk) { /* write error only if cbk is set */
           if (siteWarnings++ < lefrSiteWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END SITE name %s is different from the SITE name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, siteName);
              lefError(1631, outMsg);
              lefFree(outMsg);
              lefFree(siteName);
              CHKERR();
           } else
              lefFree(siteName);
        } else
           lefFree(siteName);
      } else {
        lefFree(siteName);
        if (lefrSiteCbk) { /* write error only if cbk is set */
          if (hasSiteClass == 0) {
             lefError(1632, "A CLASS statement is required in the SITE statement.");
             CHKERR();
          }
          if (hasSiteSize == 0) {
             lefError(1633, "A SIZE  statement is required in the SITE statement.");
             CHKERR();
          }
        }
      }
    }

site_options:
  /* empty */
  | site_options site_option
  ;

site_option:
  K_SIZE NUMBER K_BY NUMBER ';' 
    {
/* Workaround for pcr 640902
*/
      if (lefrSiteCbk) lefrSite.lefiSite::setSize($2,$4);
      hasSiteSize = 1;
    }
  | site_symmetry_statement
    { }
  | site_class 
    { 
      if (lefrSiteCbk) lefrSite.lefiSite::setClass($1);
      hasSiteClass = 1;
    }
  | site_rowpattern_statement
    { }

site_class:
  K_CLASS K_PAD ';' {$$ = (char*)"PAD"; }
  | K_CLASS K_CORE ';'  {$$ = (char*)"CORE"; }
  | K_CLASS K_VIRTUAL ';'  {$$ = (char*)"VIRTUAL"; }

site_symmetry_statement: K_SYMMETRY site_symmetries ';'
    { }

site_symmetries:
  /* empty */
  | site_symmetries site_symmetry
  ;

site_symmetry:
  K_X 
    { if (lefrSiteCbk) lefrSite.lefiSite::setXSymmetry(); }
  | K_Y 
    { if (lefrSiteCbk) lefrSite.lefiSite::setYSymmetry(); }
  | K_R90
    { if (lefrSiteCbk) lefrSite.lefiSite::set90Symmetry(); }

site_rowpattern_statement: K_ROWPATTERN {lefDumbMode = 1; lefNoNum = 1;}
    site_rowpatterns ';'
    { }

site_rowpatterns:
  /* empty */
  | site_rowpatterns site_rowpattern
  ;

site_rowpattern: T_STRING orientation {lefDumbMode = 1; lefNoNum = 1;}
    { if (lefrSiteCbk) lefrSite.lefiSite::addRowPattern($1, $2); }

pt:
  NUMBER NUMBER
    { $$.x = $1; $$.y = $2; }
  | '(' NUMBER NUMBER ')'
    { $$.x = $2; $$.y = $3; }

macro: start_macro macro_options
    { 
      if (lefrMacroCbk)
        CALLBACK(lefrMacroCbk, lefrMacroCbkType, &lefrMacro);
      lefrDoSite = 0;
    }
    end_macro

start_macro: K_MACRO {lefDumbMode = 1; lefNoNum = 1;} T_STRING 
    {
      siteDef = 0;
      symDef = 0;
      sizeDef = 0; 
      pinDef = 0; 
      obsDef = 0; 
      origDef = 0;
      lefrMacro.lefiMacro::clear();      
      if (lefrMacroBeginCbk || lefrMacroCbk) {
        // some reader may not have MacroBeginCB, but has MacroCB set
        lefrMacro.lefiMacro::setName($3);
        CALLBACK(lefrMacroBeginCbk, lefrMacroBeginCbkType, $3);
      }
      //strcpy(macroName, $3);
      macroName = strdup($3);
    }

end_macro: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
    {
      if (strcmp(macroName, $3) != 0) {
        if (lefrMacroEndCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END MACRO name %s is different from the MACRO name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, macroName);
              lefError(1634, outMsg);
              lefFree(outMsg);
              lefFree(macroName);
              CHKERR();
           } else
              lefFree(macroName);
        } else
           lefFree(macroName);
      } else
        lefFree(macroName);
      if (lefrMacroEndCbk)
        CALLBACK(lefrMacroEndCbk, lefrMacroEndCbkType, $3);
    }

macro_options:
  /* empty */
  | macro_options macro_option   /* Use left recursions */
/* WANDA
  | macro_option macro_options 
*/
  ;

macro_option:
  macro_class 
  | macro_generator 
  | macro_generate 
  | macro_source
  | macro_symmetry_statement 
  | macro_fixedMask
      { }
  | macro_origin 
      { }
  | macro_power 
      { }
  | macro_foreign
      { }
  | macro_eeq 
  | macro_leq 
  | macro_size 
      { }
  | macro_site 
      { }
  | macro_pin 
      { }
  | K_FUNCTION K_BUFFER ';'
      { if (lefrMacroCbk) lefrMacro.lefiMacro::setBuffer(); }
  | K_FUNCTION K_INVERTER ';'
      { if (lefrMacroCbk) lefrMacro.lefiMacro::setInverter(); }
  | macro_obs 
      { }
  | macro_density 
      { }
  | macro_clocktype 
      { }
  | timing
      { }
  | K_PROPERTY {lefDumbMode = 1000000; lefRealNum = 1; lefInProp = 1; } macro_prop_list  ';'
      { lefDumbMode = 0;
        lefRealNum = 0;
        lefInProp = 0;
      }

macro_prop_list:
  macro_name_value_pair
  | macro_prop_list macro_name_value_pair
  ;

macro_symmetry_statement: K_SYMMETRY macro_symmetries ';'
    {
      if (siteDef) { /* SITE is defined before SYMMETRY */
          /* pcr 283846 suppress warning */
          if (lefrMacroCbk) /* write warning only if cbk is set */
             if (macroWarnings++ < lefrMacroWarnings)
               lefWarning(2032, "A SITE statement is defined before SYMMETRY statement.\nTo avoid this warning in the future, define SITE after SYMMETRY");
      }
      symDef = 1;
    }

macro_symmetries:
  /* empty */
  | macro_symmetries macro_symmetry
  ;

macro_symmetry:
  K_X 
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setXSymmetry(); }
  | K_Y 
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setYSymmetry(); }
  | K_R90
    { if (lefrMacroCbk) lefrMacro.lefiMacro::set90Symmetry(); }

macro_name_value_pair:
  T_STRING NUMBER
    {
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (lefrMacroCbk) {
         char propTp;
         propTp = lefrMacroProp.lefiPropType::propType($1);
         lefrMacro.lefiMacro::setNumProperty($1, $2, temp,  propTp);
      }
    }
  | T_STRING QSTRING
    {
      if (lefrMacroCbk) {
         char propTp;
         propTp = lefrMacroProp.lefiPropType::propType($1);
         lefrMacro.lefiMacro::setProperty($1, $2, propTp);
      }
    }
  | T_STRING T_STRING
    {
      if (lefrMacroCbk) {
         char propTp;
         propTp = lefrMacroProp.lefiPropType::propType($1);
         lefrMacro.lefiMacro::setProperty($1, $2, propTp);
      }
    }

macro_class: K_CLASS class_type ';'
    {
       if (lefrMacroCbk) lefrMacro.lefiMacro::setClass($2);
       if (lefrMacroClassTypeCbk)
          CALLBACK(lefrMacroClassTypeCbk, lefrMacroClassTypeCbkType, $2);
    }

class_type:
  K_COVER {$$ = (char*)"COVER"; }
  | K_COVER K_BUMP
    { $$ = (char*)"COVER BUMP";
      if (versionNum < 5.5) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              if (lefrRelaxMode)
                 lefWarning(2033, "The statement COVER BUMP is a LEF verion 5.5 syntax.\nYour LEF file is version 5.4 or earlier which is incorrect but will be allowed\nbecause this application does not enforce strict version checking.\nOther tools that enforce strict checking will have a syntax error when reading this file.\nYou can change the VERSION statement in this LEF file to 5.5 or higher to stop this warning.");
              else {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "COVER BUMP statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                 lefError(1635, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
      }
    }
  | K_RING    {$$ = (char*)"RING"; }
  | K_BLOCK   {$$ = (char*)"BLOCK"; }
  | K_BLOCK K_BLACKBOX 
    { $$ = (char*)"BLOCK BLACKBOX";
      if (versionNum < 5.5) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
             if (lefrRelaxMode)
                lefWarning(2034, "The statement BLOCK BLACKBOX is a LEF verion 5.5 syntax.\nYour LEF file is version 5.4 or earlier which is incorrect but will be allowed\nbecause this application does not enforce strict version checking.\nOther tools that enforce strict checking will have a syntax error when reading this file.\nYou can change the VERSION statement in this LEF file to 5.5 or higher to stop this warning.");
              else {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "BLOCK BLACKBOX statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                 lefError(1636, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
      }
    }
  | K_BLOCK K_SOFT
    {
      if (ignoreVersion) {
        $$ = (char*)"BLOCK SOFT";
      } else if (versionNum < 5.6) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "BLOCK SOFT statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1637, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      else
        $$ = (char*)"BLOCK SOFT";
    }
  | K_NONE    {$$ = (char*)"NONE"; }
  | K_BUMP                         /* 5.7 */
      {
        if (versionNum < 5.7) {
          outMsg = (char*)lefMalloc(10000);
          sprintf(outMsg,
            "BUMP is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
          lefError(1698, outMsg);
          lefFree(outMsg);
          CHKERR();
      } else
        $$ = (char*)"BUMP";
      }
  | K_PAD     {$$ = (char*)"PAD"; } 
  | K_VIRTUAL {$$ = (char*)"VIRTUAL"; }
  | K_PAD  pad_type 
      {  sprintf(temp_name, "PAD %s", $2);
        $$ = temp_name; 
        if (versionNum < 5.5) {
           if (strcmp("AREAIO", $2) != 0) {
             sprintf(temp_name, "PAD %s", $2);
             $$ = temp_name; 
           } else if (lefrMacroCbk) { 
             if (macroWarnings++ < lefrMacroWarnings) {
               if (lefrRelaxMode)
                  lefWarning(2035, "The statement PAD AREAIO is a LEF verion 5.5 syntax.\nYour LEF file is version 5.4 or earlier which is incorrect but will be allowed\nbecause this application does not enforce strict version checking.\nOther tools that enforce strict checking will have a syntax error when reading this file.\nYou can change the VERSION statement in this LEF file to 5.5 or higher to stop this warning.");
               else {
                  outMsg = (char*)lefMalloc(10000);
                  sprintf (outMsg,
                     "PAD AREAIO statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
                  lefError(1638, outMsg);
                  lefFree(outMsg);
                  CHKERR();
               }
            }
          }
        }
      }
  | K_CORE    {$$ = (char*)"CORE"; }
  | K_CORNER 
      {$$ = (char*)"CORNER";
      /* This token is NOT in the spec but has shown up in 
       * some lef files.  This exception came from LEFOUT
       * in 'frameworks'
       */
      }
  | K_CORE core_type
      {sprintf(temp_name, "CORE %s", $2);
      $$ = temp_name;} 
  | K_ENDCAP endcap_type
      {sprintf(temp_name, "ENDCAP %s", $2);
      $$ = temp_name;} 

pad_type: /*      {$$ = (char*)"";}
  | K_INPUT 	{$$ = (char*)"INPUT";} */
  K_INPUT 	{$$ = (char*)"INPUT";}
  | K_OUTPUT	{$$ = (char*)"OUTPUT";}
  | K_INOUT 	{$$ = (char*)"INOUT";}
  | K_POWER 	{$$ = (char*)"POWER";}
  | K_SPACER	{$$ = (char*)"SPACER";}
  | K_AREAIO    {$$ = (char*)"AREAIO";}

core_type:
  K_FEEDTHRU        {$$ = (char*)"FEEDTHRU";}
  | K_TIEHIGH       {$$ = (char*)"TIEHIGH";}
  | K_TIELOW        {$$ = (char*)"TIELOW";}
  | K_SPACER
    { 
      if (ignoreVersion) {
        $$ = (char*)"SPACER";
      } else if (versionNum < 5.4) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "SPACER statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1639, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      else
        $$ = (char*)"SPACER";
    }
  | K_ANTENNACELL
    { 
      if (ignoreVersion) {
        $$ = (char*)"ANTENNACELL";
      } else if (versionNum < 5.4) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNACELL statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1640, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      else
        $$ = (char*)"ANTENNACELL";
    }
  | K_WELLTAP
    { 
      if (ignoreVersion) {
        $$ = (char*)"WELLTAP";
      } else if (versionNum < 5.6) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "WELLTAP statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1641, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      else
        $$ = (char*)"WELLTAP";
    }

endcap_type:
  K_PRE 	{$$ = (char*)"PRE";}
  | K_POST 	{$$ = (char*)"POST";}
  | K_TOPLEFT 	{$$ = (char*)"TOPLEFT";}
  | K_TOPRIGHT 	{$$ = (char*)"TOPRIGHT";}
  | K_BOTTOMLEFT 	{$$ = (char*)"BOTTOMLEFT";}
  | K_BOTTOMRIGHT	{$$ = (char*)"BOTTOMRIGHT";}

macro_generator: K_GENERATOR T_STRING ';'
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setGenerator($2); }

macro_generate: K_GENERATE T_STRING T_STRING ';'
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setGenerate($2, $3); }

macro_source:
  K_SOURCE K_USER ';'
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setSource("USER");
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2036, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | K_SOURCE K_GENERATE ';'
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setSource("GENERATE");
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2037, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | K_SOURCE K_BLOCK ';'
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setSource("BLOCK");
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2037, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

macro_power: K_POWER NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setPower($2);
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2038, "MACRO POWER statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

macro_origin: K_ORIGIN pt ';'
    { 
       if (origDef) { /* Has multiple ORIGIN defined in a macro, stop parsing*/
          if (lefrMacroCbk) { /* write error only if cbk is set */
             if (macroWarnings++ < lefrMacroWarnings) {
                lefError(1642, "ORIGIN statement has defined more than once in a MACRO statement.\nOnly one ORIGIN statement can be defined in a Macro.\nParser will stop processing.");
               CHKERR();
             }
          }
       }
       origDef = 1;
       if (siteDef) { /* SITE is defined before ORIGIN */
          /* pcr 283846 suppress warning */
          if (lefrMacroCbk) /* write warning only if cbk is set */
             if (macroWarnings++ < lefrMacroWarnings)
               lefWarning(2039, "A SITE statement is defined before ORIGIN statement.\nTo avoid this warning in the future, define SITE after ORIGIN");
       }
       if (pinDef) { /* PIN is defined before ORIGIN */
          /* pcr 283846 suppress warning */
          if (lefrMacroCbk) /* write warning only if cbk is set */
             if (macroWarnings++ < lefrMacroWarnings)
               lefWarning(2040, "A PIN statement is defined before ORIGIN statement.\nTo avoid this warning in the future, define PIN after ORIGIN");
       }
       if (obsDef) { /* OBS is defined before ORIGIN */
          /* pcr 283846 suppress warning */
          if (lefrMacroCbk) /* write warning only if cbk is set */
             if (macroWarnings++ < lefrMacroWarnings)
               lefWarning(2041, "A OBS statement is defined before ORIGIN statement.\nTo avoid this warning in the future, define OBS after ORIGIN");
       }
       /* 11/22/99 - Wanda da Rosa. PCR 283846 
                     can be defined any order.
       if (symDef)  * SYMMETRY is defined before ORIGIN *
          lefWarning("SYMMETRY is defined before ORIGIN.");
       */
       /* Add back it back in per Nora request on PCR 283846 */
       /* 1/14/2000 - Wanda da Rosa, PCR 288770
       if (sizeDef)  * SIZE is defined before ORIGIN *
          lefWarning("SIZE is defined before ORIGIN.");
       */
      
       /* Workaround for pcr 640902 */
       if (lefrMacroCbk) lefrMacro.lefiMacro::setOrigin($2.x, $2.y);
       if (lefrMacroOriginCbk) {
          macroNum.x = $2.x; 
          macroNum.y = $2.y; 
          CALLBACK(lefrMacroOriginCbk, lefrMacroOriginCbkType, macroNum);
       }
    }

macro_foreign:
  start_foreign ';'
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign($1, 0, 0.0, 0.0, -1);
    }
  | start_foreign pt ';'
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign($1, 1, $2.x, $2.y, -1);
    }
  | start_foreign pt orientation ';'
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign($1, 1, $2.x, $2.y, $3);
    }
  | start_foreign orientation ';'
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign($1, 0, 0.0, 0.0, $2);
    }

macro_fixedMask:
   K_FIXEDMASK ';' 
   {   
       if (lefrMacroCbk && versionNum >= 5.8) {
          lefrMacro.lefiMacro::setFixedMask(1);
       }
       if (lefrMacroFixedMaskCbk) {
          CALLBACK(lefrMacroFixedMaskCbk, lefrMacroFixedMaskCbkType, 1);
       }        
    }
    
macro_eeq: K_EEQ { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setEEQ($3); }

macro_leq: K_LEQ { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setLEQ($3);
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2042, "LEQ statement in MACRO is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

macro_site:
  macro_site_word  T_STRING ';'
    {
      if (lefrMacroCbk) {
        lefrMacro.lefiMacro::setSiteName($2);
/* For later than 5.6 release
        lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
                              sizeof(lefiSitePattern));
        lefrSitePatternPtr->lefiSitePattern::Init();
        lefrSitePatternPtr->lefiSitePattern::setSiteName($2);
*/
      }
    }
  | macro_site_word sitePattern ';'
    {
      if (lefrMacroCbk) {
        /* also set site name in the variable siteName_ in lefiMacro */
        /* this, if user wants to use method siteName will get the name also */
        /* Does not work, it will only set with the last sitename, if multiple
           SITEs are defined.  Therefore, data will not be correct
        lefrMacro.lefiMacro::setSitePatternName(
            lefrSitePatternPtr->lefiSitePattern::name());
        */
	lefrMacro.lefiMacro::setSitePattern(lefrSitePatternPtr);
	lefrSitePatternPtr = 0;
      }
    }

macro_site_word: K_SITE
    { lefDumbMode = 1; lefNoNum = 1; siteDef = 1;
        if (lefrMacroCbk) lefrDoSite = 1; }

site_word: K_SITE
    { lefDumbMode = 1; lefNoNum = 1; }

macro_size: K_SIZE NUMBER K_BY NUMBER ';'
    { 
      if (siteDef) { /* SITE is defined before SIZE */
         /* pcr 283846 suppress warning
         if (siteWarnings++ < lefrSiteWarnings)
           lefWarning("SITE is defined before SIZE.");
         return 1; 
         */
      }
      sizeDef = 1;
      if (lefrMacroCbk) lefrMacro.lefiMacro::setSize($2, $4);
      if (lefrMacroSizeCbk) {
         macroNum.x = $2; 
         macroNum.y = $4; 
         CALLBACK(lefrMacroSizeCbk, lefrMacroSizeCbkType, macroNum);
      }
    }

/* This is confusing, since FEF and LEF have opposite definitions of
   ports and pins */

macro_pin: start_macro_pin macro_pin_options end_macro_pin
    { 
      if (lefrPinCbk)
        CALLBACK(lefrPinCbk, lefrPinCbkType, &lefrPin);
      lefrPin.lefiPin::clear();
    }

start_macro_pin: K_PIN {lefDumbMode = 1; lefNoNum = 1; pinDef = 1;} T_STRING 
    { if (lefrPinCbk) lefrPin.lefiPin::setName($3);
      //strcpy(pinName, $3);
      pinName = strdup($3);
    }

end_macro_pin: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
    {
      if (strcmp(pinName, $3) != 0) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END PIN name %s is different from the PIN name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, pinName);
              lefError(1643, outMsg);
              lefFree(outMsg);
              lefFree(pinName);
              CHKERR();
           } else
              lefFree(pinName);
        } else
           lefFree(pinName);
      } else
        lefFree(pinName);
    }

macro_pin_options:
  /* empty */
    { }
  | macro_pin_options macro_pin_option 
    { }

macro_pin_option:
  start_foreign ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign($1, 0, 0.0, 0.0, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign pt ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign($1, 1, $2.x, $2.y, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign pt orientation ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign($1, 1, $2.x, $2.y, $3);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign K_STRUCTURE ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign($1, 0, 0.0, 0.0, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign K_STRUCTURE pt ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign($1, 1, $3.x, $3.y, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | start_foreign K_STRUCTURE pt orientation ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign($1, 1, $3.x, $3.y, $4);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | K_LEQ { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::setLEQ($3);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2044, "LEQ statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
   }
  | K_POWER NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setPower($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2045, "MACRO POWER statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | electrical_direction
    { if (lefrPinCbk) lefrPin.lefiPin::setDirection($1); }
  | K_USE macro_pin_use ';'
    { if (lefrPinCbk) lefrPin.lefiPin::setUse($2); }
  | K_SCANUSE macro_scan_use ';'
    { }
  | K_LEAKAGE NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setLeakage($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2046, "MACRO LEAKAGE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, r emove this statement from the LEF file with version 5.4 or later.");
    }
  | K_RISETHRESH NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setRiseThresh($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2047, "MACRO RISETHRESH statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_FALLTHRESH NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setFallThresh($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2048, "MACRO FALLTHRESH statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_RISESATCUR NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setRiseSatcur($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2049, "MACRO RISESATCUR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_FALLSATCUR NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setFallSatcur($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2050, "MACRO FALLSATCUR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_VLO NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setVLO($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2051, "MACRO VLO statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_VHI NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setVHI($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2052, "MACRO VHI statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_TIEOFFR NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setTieoffr($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2053, "MACRO TIEOFFR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_SHAPE pin_shape ';'
    { if (lefrPinCbk) lefrPin.lefiPin::setShape($2); }
  | K_MUSTJOIN {lefDumbMode = 1; lefNoNum = 1;} T_STRING ';'
    { if (lefrPinCbk) lefrPin.lefiPin::setMustjoin($3); }
  | K_OUTPUTNOISEMARGIN {lefDumbMode = 1;} NUMBER NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setOutMargin($3, $4);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2054, "MACRO OUTPUTNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_OUTPUTRESISTANCE {lefDumbMode = 1;} NUMBER NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setOutResistance($3, $4);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2055, "MACRO OUTPUTRESISTANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_INPUTNOISEMARGIN {lefDumbMode = 1;} NUMBER NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setInMargin($3, $4);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2056, "MACRO INPUTNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_CAPACITANCE NUMBER ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setCapacitance($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2057, "MACRO CAPACITANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_MAXDELAY NUMBER ';' 
    { if (lefrPinCbk) lefrPin.lefiPin::setMaxdelay($2); }
  | K_MAXLOAD NUMBER ';' 
    { if (lefrPinCbk) lefrPin.lefiPin::setMaxload($2); }
  | K_RESISTANCE NUMBER ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setResistance($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2058, "MACRO RESISTANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_PULLDOWNRES NUMBER ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setPulldownres($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2059, "MACRO PULLDOWNRES statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_CURRENTSOURCE K_ACTIVE ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setCurrentSource("ACTIVE");
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2060, "MACRO CURRENTSOURCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_CURRENTSOURCE K_RESISTIVE ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setCurrentSource("RESISTIVE");
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2061, "MACRO CURRENTSOURCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_RISEVOLTAGETHRESHOLD NUMBER ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setRiseVoltage($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2062, "MACRO RISEVOLTAGETHRESHOLD statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_FALLVOLTAGETHRESHOLD NUMBER ';' 
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setFallVoltage($2);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2063, "MACRO FALLVOLTAGETHRESHOLD statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_IV_TABLES T_STRING T_STRING ';'
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setTables($2, $3);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2064, "MACRO IV_TABLES statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_TAPERRULE T_STRING ';'
    { if (lefrPinCbk) lefrPin.lefiPin::setTaperRule($2); }
  | K_PROPERTY {lefDumbMode = 1000000; lefRealNum = 1; lefInProp = 1; } pin_prop_list ';'
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    }
  | start_macro_port macro_port_class_option geometries K_END
    {
      lefDumbMode = 0;
      hasGeoLayer = 0;
      if (lefrPinCbk) {
	lefrPin.lefiPin::addPort(lefrGeometriesPtr);
	lefrGeometriesPtr = 0;
	lefrDoGeometries = 0;
      }
      if ((needGeometry) && (needGeometry != 2))  // if the last LAYER in PORT
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2065, "Either PATH, RECT or POLYGON statement is a required in MACRO/PIN/PORT.");
    }
  | start_macro_port K_END
    /* 06/07/2001 - Wanda da Rosa.  Allow empty PORT due to incorrect
     *              old lef files from customers
     */
    {
      // Since in start_macro_port it has call the Init method, here
      // we need to call the Destroy method.
      // Still add a null pointer to set the number of port
      if (lefrPinCbk) {
        lefrPin.lefiPin::addPort(lefrGeometriesPtr);
        lefrGeometriesPtr = 0;
        lefrDoGeometries = 0;
      }
      hasGeoLayer = 0;
    }
  | K_ANTENNASIZE NUMBER opt_layer_name ';'
    {  /* a pre 5.4 syntax */
      use5_3 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum >= 5.4) {
        if (use5_4) {
           if (lefrPinCbk) { /* write error only if cbk is set */
             if (pinWarnings++ < lefrPinWarnings) {
                outMsg = (char*)lefMalloc(10000);
                sprintf (outMsg,
                   "ANTENNASIZE statement is a version 5.3 and earlier syntax.\nYour lef file is defined with version %g.", versionNum);
                lefError(1644, outMsg);
                lefFree(outMsg);
                CHKERR();
             }
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaSize($2, $3);
    }
  | K_ANTENNAMETALAREA NUMBER opt_layer_name ';'
    {  /* a pre 5.4 syntax */
      use5_3 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum >= 5.4) {
        if (use5_4) {
           if (lefrPinCbk) { /* write error only if cbk is set */
              if (pinWarnings++ < lefrPinWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "ANTENNAMETALAREA statement is a version 5.3 and earlier syntax.\nYour lef file is defined with version %g.", versionNum);
                 lefError(1645, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMetalArea($2, $3);
    }
  | K_ANTENNAMETALLENGTH NUMBER opt_layer_name ';'
    { /* a pre 5.4 syntax */ 
      use5_3 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum >= 5.4) {
        if (use5_4) {
           if (lefrPinCbk) { /* write error only if cbk is set */
              if (pinWarnings++ < lefrPinWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "ANTENNAMETALLENGTH statement is a version 5.3 and earlier syntax.\nYour lef file is defined with version %g.", versionNum);
                 lefError(1646, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMetalLength($2, $3);
    }
  | K_RISESLEWLIMIT NUMBER ';'
    { if (lefrPinCbk) lefrPin.lefiPin::setRiseSlewLimit($2); }
  | K_FALLSLEWLIMIT NUMBER ';'
    { if (lefrPinCbk) lefrPin.lefiPin::setFallSlewLimit($2); }
  | K_ANTENNAPARTIALMETALAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAPARTIALMETALAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1647, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAPARTIALMETALAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1647, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaPartialMetalArea($2, $3);
    }
  | K_ANTENNAPARTIALMETALSIDEAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAPARTIALMETALSIDEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1648, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAPARTIALMETALSIDEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1648, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaPartialMetalSideArea($2, $3);
    }
  | K_ANTENNAPARTIALCUTAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAPARTIALCUTAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1649, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAPARTIALCUTAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1649, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaPartialCutArea($2, $3);
    }
  | K_ANTENNADIFFAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNADIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1650, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNADIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1650, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaDiffArea($2, $3);
    }
  | K_ANTENNAGATEAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAGATEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1651, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAGATEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1651, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaGateArea($2, $3);
    }
  | K_ANTENNAMAXAREACAR NUMBER req_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMAXAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1652, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMAXAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1652, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMaxAreaCar($2, $3);
    }
  | K_ANTENNAMAXSIDEAREACAR NUMBER req_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMAXSIDEAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1653, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMAXSIDEAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1653, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMaxSideAreaCar($2, $3);
    }
  | K_ANTENNAMAXCUTCAR NUMBER req_layer_name ';'
    { /* 5.4 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.4) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMAXCUTCAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1654, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMAXCUTCAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1654, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMaxCutCar($2, $3);
    }
  | K_ANTENNAMODEL
    { /* 5.5 syntax */
      use5_4 = 1;
      if (ignoreVersion) {
        /* do nothing */
      } else if (versionNum < 5.5) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMODEL statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1655, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else if (use5_3) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ANTENNAMODEL statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1655, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      }
    }
    pin_layer_oxide ';'
  | K_NETEXPR {lefDumbMode = 2; lefNoNum = 2; } QSTRING ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "NETEXPR statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1656, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else
        if (lefrPinCbk) lefrPin.lefiPin::setNetExpr($3);
    }
  | K_SUPPLYSENSITIVITY {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "SUPPLYSENSITIVITY statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1657, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else
        if (lefrPinCbk) lefrPin.lefiPin::setSupplySensitivity($3);
    }
  | K_GROUNDSENSITIVITY {lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) { /* write error only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "GROUNDSENSITIVITY statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1658, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } else
        if (lefrPinCbk) lefrPin.lefiPin::setGroundSensitivity($3);
    }

pin_layer_oxide:
  K_OXIDE1
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(1);
    }
  | K_OXIDE2
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(2);
    }
  | K_OXIDE3
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(3);
    }
  | K_OXIDE4
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(4);
    }

pin_prop_list:
  pin_name_value_pair
  | pin_prop_list pin_name_value_pair
  ;

pin_name_value_pair:
  T_STRING NUMBER
    { 
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (lefrPinCbk) {
         char propTp;
         propTp = lefrPinProp.lefiPropType::propType($1);
         lefrPin.lefiPin::setNumProperty($1, $2, temp, propTp);
      }
    }
  | T_STRING QSTRING
    {
      if (lefrPinCbk) {
         char propTp;
         propTp = lefrPinProp.lefiPropType::propType($1);
         lefrPin.lefiPin::setProperty($1, $2, propTp);
      }
    }
  | T_STRING T_STRING
    {
      if (lefrPinCbk) {
         char propTp;
         propTp = lefrPinProp.lefiPropType::propType($1);
         lefrPin.lefiPin::setProperty($1, $2, propTp);
      }
    }

electrical_direction:
  K_DIRECTION K_INPUT ';'         {$$ = (char*)"INPUT";}
  | K_DIRECTION K_OUTPUT ';'            {$$ = (char*)"OUTPUT";}
  | K_DIRECTION K_OUTPUT K_TRISTATE ';' {$$ = (char*)"OUTPUT TRISTATE";}
  | K_DIRECTION K_INOUT  ';'            {$$ = (char*)"INOUT";}
  | K_DIRECTION K_FEEDTHRU ';'          {$$ = (char*)"FEEDTHRU";}

start_macro_port: K_PORT
    {
      if (lefrPinCbk) {
	lefrDoGeometries = 1;
        hasPRP = 0;
	lefrGeometriesPtr = (lefiGeometries*)lefMalloc( sizeof(lefiGeometries));
	lefrGeometriesPtr->lefiGeometries::Init();
      }
      needGeometry = 0;  // don't need rect/path/poly define yet
      hasGeoLayer = 0;   // make sure LAYER is set before geometry
    }

macro_port_class_option: /* empty */
  | K_CLASS class_type ';'
    { if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::addClass($2); }

macro_pin_use:
  K_SIGNAL	{$$ = (char*)"SIGNAL";}
  | K_ANALOG 	{$$ = (char*)"ANALOG";}
  | K_POWER 	{$$ = (char*)"POWER";}
  | K_GROUND	{$$ = (char*)"GROUND";}
  | K_CLOCK	{$$ = (char*)"CLOCK";}
  | K_DATA	{$$ = (char*)"DATA";}

macro_scan_use:
  K_INPUT {$$ = (char*)"INPUT";}
  | K_OUTPUT	{$$ = (char*)"OUTPUT";}
  | K_START	{$$ = (char*)"START";}
  | K_STOP	{$$ = (char*)"STOP";}

pin_shape:
  /* empty */   {$$ = (char*)""; }      /* non-ring shape */
  | K_ABUTMENT	{$$ = (char*)"ABUTMENT";}
  | K_RING 	{$$ = (char*)"RING";}
  | K_FEEDTHRU	{$$ = (char*)"FEEDTHRU";}

geometries: geometry geometry_options

geometry:
  K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING
    {
      if ((needGeometry) && (needGeometry != 2)) // 1 LAYER follow after another
        if (layerWarnings++ < lefrLayerWarnings) {
          /* geometries is called by MACRO/OBS & MACRO/PIN/PORT */
          if (obsDef)
             lefWarning(2076, "Either PATH, RECT or POLYGON statement is a required in MACRO/OBS.");
          else
             lefWarning(2065, "Either PATH, RECT or POLYGON statement is a required in MACRO/PIN/PORT.");
        }
      if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::addLayer($3);
      needGeometry = 1;    // within LAYER it requires either path, rect, poly
      hasGeoLayer = 1;
    }
  layer_exceptpgnet
  layer_spacing ';'
  | K_WIDTH NUMBER ';'
    { 
      if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else
           lefrGeometriesPtr->lefiGeometries::addWidth($2); 
      } 
    }
  | K_PATH maskColor firstPt otherPts ';'
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
                lefrGeometriesPtr->lefiGeometries::addPath((int)$2);
           }
        }
      }
      hasPRP = 1;
      needGeometry = 2;
    }
  | K_PATH maskColor K_ITERATE firstPt otherPts stepPattern ';'
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addPathIter((int)$2);
            }
         }
      } 
      hasPRP = 1;
      needGeometry = 2;
    }
  | K_RECT maskColor pt pt';'
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addRect((int)$2, $3.x, $3.y, $4.x, $4.y);
           }
        }
      }
      needGeometry = 2;
    }
  | K_RECT maskColor K_ITERATE pt pt stepPattern ';'
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addRectIter((int)$2, $4.x, $4.y, $5.x, $5.y);
           }
        }
      }
      needGeometry = 2;
    }
  | K_POLYGON maskColor firstPt nextPt nextPt otherPts ';'
    {
      if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addPolygon((int)$2);
            }
           }
      }
      hasPRP = 1;
      needGeometry = 2;
    }
  | K_POLYGON maskColor K_ITERATE firstPt nextPt nextPt otherPts stepPattern ';'
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addPolygonIter((int)$2);
           }
         }
      }
      hasPRP = 1;
      needGeometry = 2;
    }
  | via_placement
    { }

geometry_options: /* empty */
  | geometry_options geometry

layer_exceptpgnet: /* empty */
  | K_EXCEPTPGNET                   /* 5.7 */
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "EXCEPTPGNET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1699, outMsg);
        lefFree(outMsg);
        CHKERR();
      } else {
       if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::addLayerExceptPgNet();
      }
    }

layer_spacing: /* empty */
  | K_SPACING NUMBER
    { if (lefrDoGeometries) {
        if (zeroOrGt($2))
           lefrGeometriesPtr->lefiGeometries::addLayerMinSpacing($2);
        else {
           outMsg = (char*)lefMalloc(10000);
           sprintf (outMsg,
              "THE SPACING statement has the value %g in MACRO OBS.\nValue has to be 0 or greater.", $2);
           lefError(1659, outMsg);
           lefFree(outMsg);
           CHKERR();
        }
      }
    }
  | K_DESIGNRULEWIDTH NUMBER
    { if (lefrDoGeometries) {
        if (zeroOrGt($2))
           lefrGeometriesPtr->lefiGeometries::addLayerRuleWidth($2);
        else {
           outMsg = (char*)lefMalloc(10000);
           sprintf (outMsg,
              "THE DESIGNRULEWIDTH statement has the value %g in MACRO OBS.\nValue has to be 0 or greater.", $2);
           lefError(1660, outMsg);
           lefFree(outMsg);
           CHKERR();
        }
      }
    }

firstPt: pt  
    { if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::startList($1.x, $1.y); }

nextPt:  pt
    { if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::addToList($1.x, $1.y); }

otherPts:
  /* empty */
/* switch to avoid stack overflow with large polygon
  | nextPt otherPts
*/
  | otherPts nextPt
  ;

via_placement:
  K_VIA maskColor pt {lefDumbMode = 1;} T_STRING ';'
    { 
        if (lefrDoGeometries){
            if (versionNum < 5.8 && (int)$2 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
            } else {
                lefrGeometriesPtr->lefiGeometries::addVia((int)$2, $3.x, $3.y, $5);
            }
        }
    }
  | K_VIA K_ITERATE maskColor pt {lefDumbMode = 1; lefNoNum = 1;} T_STRING
    stepPattern ';'
    { 
        if (lefrDoGeometries) {
            if (versionNum < 5.8 && (int)$3 > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
            } else {
              lefrGeometriesPtr->lefiGeometries::addViaIter((int)$3, $4.x, $4.y, $6); 
            }
        }
    }
        
stepPattern: K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
     { if (lefrDoGeometries)
         lefrGeometriesPtr->lefiGeometries::addStepPattern($2, $4, $6, $7); }

sitePattern: T_STRING NUMBER NUMBER orientation
  K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
    {
      if (lefrDoSite) {
	lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
				   sizeof(lefiSitePattern));
	lefrSitePatternPtr->lefiSitePattern::Init();
	lefrSitePatternPtr->lefiSitePattern::set($1, $2, $3, $4, $6, $8,
	  $10, $11);
	}
    }
  | T_STRING NUMBER NUMBER orientation
    {
      if (lefrDoSite) {
	lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
				   sizeof(lefiSitePattern));
	lefrSitePatternPtr->lefiSitePattern::Init();
	lefrSitePatternPtr->lefiSitePattern::set($1, $2, $3, $4, -1, -1,
	  -1, -1);
	}
    }

trackPattern:
  K_X NUMBER K_DO NUMBER K_STEP NUMBER 
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
				sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("X", $2, (int)$4, $6);
      }    
    }
    K_LAYER {lefDumbMode = 1000000000;} trackLayers
    { lefDumbMode = 0;}
  | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("Y", $2, (int)$4, $6);
      }    
    }
    K_LAYER {lefDumbMode = 1000000000;} trackLayers
    { lefDumbMode = 0;}
  | K_X NUMBER K_DO NUMBER K_STEP NUMBER 
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("X", $2, (int)$4, $6);
      }    
    }
  | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("Y", $2, (int)$4, $6);
      }    
    }

trackLayers:
  /* empty */
  | trackLayers layer_name
  ;

layer_name: T_STRING
    { if (lefrDoTrack) lefrTrackPatternPtr->lefiTrackPattern::addLayer($1); }

gcellPattern: K_X NUMBER K_DO NUMBER K_STEP NUMBER
    {
      if (lefrDoGcell) {
	lefrGcellPatternPtr = (lefiGcellPattern*)lefMalloc(
                                    sizeof(lefiGcellPattern));
	lefrGcellPatternPtr->lefiGcellPattern::Init();
	lefrGcellPatternPtr->lefiGcellPattern::set("X", $2, (int)$4, $6);
      }    
    }
  | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
    {
      if (lefrDoGcell) {
	lefrGcellPatternPtr = (lefiGcellPattern*)lefMalloc(
                                    sizeof(lefiGcellPattern));
	lefrGcellPatternPtr->lefiGcellPattern::Init();
	lefrGcellPatternPtr->lefiGcellPattern::set("Y", $2, (int)$4, $6);
      }    
    }

macro_obs: start_macro_obs geometries K_END
    { 
      if (lefrObstructionCbk) {
	lefrObstruction.lefiObstruction::setGeometries(lefrGeometriesPtr);
	lefrGeometriesPtr = 0;
	lefrDoGeometries = 0;
        CALLBACK(lefrObstructionCbk, lefrObstructionCbkType, &lefrObstruction);
      }
      lefDumbMode = 0;
      hasGeoLayer = 0;       /* reset */
    }
  | start_macro_obs K_END
    /* 08/14/00 - Wanda da Rosa.  Allow empty OBS due to so many lef files
     * have empty OBS
    {
       lefError("OBS requires either a LAYER or a VIA.");
       CHKERR();
    }
    */
    {
       /* The pointer has malloced in start, need to free manually */
       if (lefrGeometriesPtr) {
          lefrGeometriesPtr->lefiGeometries::Destroy();
          lefFree(lefrGeometriesPtr);
          lefrGeometriesPtr = 0;
	  lefrDoGeometries = 0;
       }
       hasGeoLayer = 0;
    }

start_macro_obs: K_OBS
    {
      obsDef = 1;
      if (lefrObstructionCbk) {
	lefrDoGeometries = 1;
	lefrGeometriesPtr = (lefiGeometries*)lefMalloc(
	    sizeof(lefiGeometries));
	lefrGeometriesPtr->lefiGeometries::Init();
	}
      hasGeoLayer = 0;
    }

macro_density: K_DENSITY density_layer density_layers K_END
    { 
      if (versionNum < 5.6) {
        if (lefrDensityCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "DENSITY statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1661, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
      } 
      if (lefrDensityCbk) {
        CALLBACK(lefrDensityCbk, lefrDensityCbkType, &lefrDensity);
        lefrDensity.lefiDensity::clear();
      }
      lefDumbMode = 0;
    }

density_layers: /* empty */
    | density_layers density_layer
    ;

density_layer: K_LAYER { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    {
      if (lefrDensityCbk)
        lefrDensity.lefiDensity::addLayer($3);
    }
    density_layer_rect density_layer_rects

density_layer_rects: /* empty */
    | density_layer_rects density_layer_rect
    ;

density_layer_rect: K_RECT pt pt NUMBER ';'
    {
      if (lefrDensityCbk)
        lefrDensity.lefiDensity::addRect($2.x, $2.y, $3.x, $3.y, $4); 
    }

macro_clocktype: K_CLOCKTYPE { lefDumbMode = 1; lefNoNum = 1; } T_STRING ';'
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setClockType($3); }

timing: start_timing timing_options end_timing
    { }

start_timing: K_TIMING 
    { /* XXXXX for macros */ }

end_timing: K_END K_TIMING 
  {
    if (versionNum < 5.4) {
      if (lefrTimingCbk && lefrTiming.lefiTiming::hasData())
        CALLBACK(lefrTimingCbk, lefrTimingCbkType, &lefrTiming);
      lefrTiming.lefiTiming::clear();
    } else {
      if (lefrTimingCbk) /* write warning only if cbk is set */
        if (timingWarnings++ < lefrTimingWarnings)
          lefWarning(2066, "MACRO TIMING statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
      lefrTiming.lefiTiming::clear();
    }
  }

timing_options:
  /* empty */
  | timing_options timing_option
  ;

timing_option:
  K_FROMPIN 
    {
    if (versionNum < 5.4) {
      if (lefrTimingCbk && lefrTiming.lefiTiming::hasData())
        CALLBACK(lefrTimingCbk, lefrTimingCbkType, &lefrTiming);
    }
    lefDumbMode = 1000000000;
    lefrTiming.lefiTiming::clear();
    }
    list_of_from_strings ';'
    { lefDumbMode = 0;}
  | K_TOPIN {lefDumbMode = 1000000000;} list_of_to_strings ';'
    { lefDumbMode = 0;}
  | risefall K_INTRINSIC NUMBER NUMBER
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFall($1,$3,$4); }
    slew_spec K_VARIABLE NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallVariable($8,$9); }
  | risefall delay_or_transition K_UNATENESS unateness
    K_TABLEDIMENSION NUMBER NUMBER NUMBER ';' 
    { if (lefrTimingCbk) {
	if ($2[0] == 'D' || $2[0] == 'd') /* delay */
	  lefrTiming.lefiTiming::addDelay($1, $4, $6, $7, $8);
	else
	  lefrTiming.lefiTiming::addTransition($1, $4, $6, $7, $8);
      }
    }
  | K_TABLEAXIS list_of_table_axis_numbers ';'
    { }
  | K_TABLEENTRIES list_of_table_entries ';'
    { }
  | K_RISERS NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseRS($2,$3); }
  | K_FALLRS NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallRS($2,$3); }
  | K_RISECS NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseCS($2,$3); }
  | K_FALLCS NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallCS($2,$3); }
  | K_RISESATT1 NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseAtt1($2,$3); }
  | K_FALLSATT1 NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallAtt1($2,$3); }
  | K_RISET0 NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseTo($2,$3); }
  | K_FALLT0 NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallTo($2,$3); }
  | K_UNATENESS unateness ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addUnateness($2); }
  | K_STABLE K_SETUP NUMBER K_HOLD NUMBER risefall ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setStable($3,$5,$6); }
  | two_pin_trigger from_pin_trigger to_pin_trigger K_TABLEDIMENSION NUMBER NUMBER NUMBER ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addSDF2Pins($1,$2,$3,$5,$6,$7); }
  | one_pin_trigger K_TABLEDIMENSION NUMBER NUMBER NUMBER ';' 
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addSDF1Pin($1,$3,$4,$4); }
  | K_SDFCONDSTART QSTRING ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setSDFcondStart($2); }
  | K_SDFCONDEND QSTRING ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setSDFcondEnd($2); }
  | K_SDFCOND QSTRING ';'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setSDFcond($2); }
  | K_EXTENSION ';'
    { /* XXXXX */ }

one_pin_trigger:
  K_MPWH
    { $$ = (char*)"MPWH";}
  | K_MPWL
    { $$ = (char*)"MPWL";}
  | K_PERIOD
    { $$ = (char*)"PERIOD";}

two_pin_trigger :
  K_SETUP
    { $$ = (char*)"SETUP";}
  | K_HOLD
    { $$ = (char*)"HOLD";}
  | K_RECOVERY
    { $$ = (char*)"RECOVERY";}
  | K_SKEW
    { $$ = (char*)"SKEW";}

from_pin_trigger:
  K_ANYEDGE
    { $$ = (char*)"ANYEDGE";}
  | K_POSEDGE
    { $$ = (char*)"POSEDGE";}
  | K_NEGEDGE 
    { $$ = (char*)"NEGEDGE";}

to_pin_trigger:
  K_ANYEDGE
    { $$ = (char*)"ANYEDGE";}
  | K_POSEDGE
    { $$ = (char*)"POSEDGE";}
  | K_NEGEDGE 
    { $$ = (char*)"NEGEDGE";}

delay_or_transition :
  K_DELAY
    { $$ = (char*)"DELAY"; }
  | K_TRANSITIONTIME
    { $$ = (char*)"TRANSITION"; }

list_of_table_entries:
  table_entry
    { }
  | list_of_table_entries table_entry
    { }

table_entry: '(' NUMBER NUMBER NUMBER ')'
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addTableEntry($2,$3,$4); }

list_of_table_axis_numbers:
  NUMBER
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addTableAxisNumber($1); }
  | list_of_table_axis_numbers NUMBER
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addTableAxisNumber($2); }

slew_spec:
  /* empty */
    { }
  | NUMBER NUMBER NUMBER NUMBER 
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallSlew($1,$2,$3,$4); }
  |  NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER 
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallSlew($1,$2,$3,$4);
      if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallSlew2($5,$6,$7); }

risefall:
  K_RISE
    { $$ = (char*)"RISE"; }
  | K_FALL 
    { $$ = (char*)"FALL"; }

unateness:
  K_INVERT
    { $$ = (char*)"INVERT"; }
  | K_NONINVERT
    { $$ = (char*)"NONINVERT"; }
  | K_NONUNATE 
    { $$ = (char*)"NONUNATE"; }

list_of_from_strings:
  T_STRING
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addFromPin($1); }
  | list_of_from_strings T_STRING 
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addFromPin($2); }

list_of_to_strings:
  T_STRING
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addToPin($1); }
  | list_of_to_strings T_STRING 
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addToPin($2); }

array: start_array array_rules
    {
      if (lefrArrayCbk)
	CALLBACK(lefrArrayCbk, lefrArrayCbkType, &lefrArray);
      lefrArray.lefiArray::clear();
      lefrSitePatternPtr = 0;
      lefrDoSite = 0;
   }
    end_array

start_array: K_ARRAY {lefDumbMode = 1; lefNoNum = 1;} T_STRING
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::setName($3);
	CALLBACK(lefrArrayBeginCbk, lefrArrayBeginCbkType, $3);
      }
      //strcpy(arrayName, $3);
      arrayName = strdup($3);
    }

end_array: K_END {lefDumbMode = 1; lefNoNum = 1;} T_STRING
    {
      if (lefrArrayCbk && lefrArrayEndCbk)
	CALLBACK(lefrArrayEndCbk, lefrArrayEndCbkType, $3);
      if (strcmp(arrayName, $3) != 0) {
        if (lefrArrayCbk) { /* write error only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END ARRAY name %s is different from the ARRAY name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", $3, arrayName);
              lefError(1662, outMsg);
              lefFree(outMsg);
              lefFree(arrayName);
              CHKERR();
           } else
              lefFree(arrayName);
        } else
           lefFree(arrayName);
      } else
        lefFree(arrayName);
    }

array_rules:
  /* empty */
    { }
  | array_rules array_rule
    { }

array_rule:
  site_word { if (lefrArrayCbk) lefrDoSite = 1; lefDumbMode = 1; }
    sitePattern  ';'
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addSitePattern(lefrSitePatternPtr);
      }
    }
  | K_CANPLACE {lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; }
    sitePattern ';'
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addCanPlace(lefrSitePatternPtr);
      }
    }
  | K_CANNOTOCCUPY {lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; }
    sitePattern ';'
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addCannotOccupy(lefrSitePatternPtr);
      }
    }
  | K_TRACKS { if (lefrArrayCbk) lefrDoTrack = 1; } trackPattern ';'
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addTrack(lefrTrackPatternPtr);
      }
    }
  | floorplan_start floorplan_list K_END T_STRING 
    {
    }
  | K_GCELLGRID { if (lefrArrayCbk) lefrDoGcell = 1; } gcellPattern ';'
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addGcell(lefrGcellPatternPtr);
      }
    }
  | K_DEFAULTCAP NUMBER cap_list K_END K_DEFAULTCAP
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::setTableSize((int)$2);
      }
    }
  | def_statement
    { }

floorplan_start: K_FLOORPLAN T_STRING
    { if (lefrArrayCbk) lefrArray.lefiArray::addFloorPlan($2); }
	
floorplan_list:
  /* empty */
    { }
  | floorplan_list floorplan_element
    { }

floorplan_element:
  K_CANPLACE { lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; }
    sitePattern ';'
    {
      if (lefrArrayCbk)
	lefrArray.lefiArray::addSiteToFloorPlan("CANPLACE",
	lefrSitePatternPtr);
    }
  | K_CANNOTOCCUPY { if (lefrArrayCbk) lefrDoSite = 1; lefDumbMode = 1; }
    sitePattern ';'
    {
      if (lefrArrayCbk)
	lefrArray.lefiArray::addSiteToFloorPlan("CANNOTOCCUPY",
	lefrSitePatternPtr);
     }

cap_list:
  /* empty */
    { }
  | cap_list one_cap
    { }

one_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'
    { if (lefrArrayCbk) lefrArray.lefiArray::addDefaultCap((int)$2, $4); }

msg_statement:
  K_MESSAGE {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' s_expr dtrm
    { lefAddStringMessage($3, $5); }

create_file_statement:
  K_CREATEFILE {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' s_expr dtrm
    { }

def_statement:
  K_DEFINE {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' expression dtrm
    {
      if (versionNum < 5.6)
        lefAddNumDefine($3, $5);
      else
        if (lefrArrayCbk) /* write warning only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings)
             lefWarning(2067, "DEFINE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  |  K_DEFINES {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' s_expr dtrm
    {
      if (versionNum < 5.6)
        lefAddStringDefine($3, $5);
      else
        if (lefrArrayCbk) /* write warning only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings)
             lefWarning(2068, "DEFINES statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  |  K_DEFINEB {lefDumbMode=1;lefNlToken=TRUE;} T_STRING '=' b_expr dtrm
    {
      if (versionNum < 5.6)
        lefAddBooleanDefine($3, $5);
      else
        if (lefrArrayCbk) /* write warning only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings)
             lefWarning(2069, "DEFINEB statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

/* terminator for &defines.  Can be semicolon or newline */
dtrm:
  |  ';' {lefNlToken = FALSE;}
  |  '\n'	{lefNlToken = FALSE;}

then:
  K_THEN
  | '\n' K_THEN
  ;

else:
  K_ELSE
  | '\n' K_ELSE
  ;

expression:
  expression '+' expression   {$$ = $1 + $3; }
  | expression '-' expression   {$$ = $1 - $3; }
  | expression '*' expression   {$$ = $1 * $3; }
  | expression '/' expression   {$$ = $1 / $3; }
  | '-' expression %prec UMINUS {$$ = -$2;}
  | '(' expression ')'		{$$ = $2;}
  | K_IF b_expr then expression else expression %prec IF
		{$$ = ($2 != 0) ? $4 : $6;}
  | NUMBER			{$$ = $1;}

b_expr:
  expression relop expression {$$ = comp_num($1,$2,$3);}
  | expression K_AND expression {$$ = $1 != 0 && $3 != 0;}
  | expression K_OR  expression {$$ = $1 != 0 || $3 != 0;}
  | s_expr relop s_expr	      {$$ = comp_str($1,$2,$3);}
  | s_expr K_AND s_expr	      {$$ = $1[0] != 0 && $3[0] != 0;}
  | s_expr K_OR  s_expr	      {$$ = $1[0] != 0 || $3[0] != 0;}
  | b_expr K_EQ b_expr	      {$$ = $1 == $3;}
  | b_expr K_NE b_expr	      {$$ = $1 != $3;}
  | b_expr K_AND b_expr	      {$$ = $1 && $3;}
  | b_expr K_OR  b_expr	      {$$ = $1 || $3;}
  | K_NOT b_expr		      %prec LNOT {$$ = !$$;}
  | '(' b_expr ')'	      {$$ = $2;}
  | K_IF b_expr then b_expr else b_expr %prec IF
	  {$$ = ($2 != 0) ? $4 : $6;}
  | K_TRUE		      {$$ = 1;}
  | K_FALSE		      {$$ = 0;}

s_expr:
  s_expr '+' s_expr
    {
      $$ = (char*)lefMalloc(strlen($1)+strlen($3)+1);
      strcpy($$,$1);
      strcat($$,$3);
    }
  | '(' s_expr ')'
    { $$ = $2; }
  | K_IF b_expr then s_expr else s_expr %prec IF
    {
      lefDefIf = TRUE;
      if ($2 != 0) {
	$$ = $4;	
      } else {
	$$ = $6;
      }
    }
  | QSTRING
    { $$ = $1; }

relop:
  K_LE {$$ = C_LE;}
  | K_LT {$$ = C_LT;}
  | K_GE {$$ = C_GE;}
  | K_GT {$$ = C_GT;}
  | K_EQ {$$ = C_EQ;}
  | K_NE {$$ = C_NE;}
  | '='  {$$ = C_EQ;}
  | '<'  {$$ = C_LT;}
  | '>'  {$$ = C_GT;}


prop_def_section: K_PROPDEF
    { 
      if (lefrPropBeginCbk)
        CALLBACK(lefrPropBeginCbk, lefrPropBeginCbkType, 0);
      lefInPropDef = 1;  /* set flag as inside propertydefinitions */
    }
    prop_stmts K_END K_PROPDEF
    { 
      if (lefrPropEndCbk)
        CALLBACK(lefrPropEndCbk, lefrPropEndCbkType, 0);
      lefRealNum = 0;     /* just want to make sure it is reset */
      lefInPropDef = 0;   /* reset flag */
    }

prop_stmts:
  /* empty */
    { }
  | prop_stmts prop_stmt
    { }

prop_stmt:
  K_LIBRARY {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("library", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrLibProp.lefiPropType::setPropType($3, lefPropDefType);
    }
  | K_COMPONENTPIN {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("componentpin", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrCompProp.lefiPropType::setPropType($3, lefPropDefType);
    }
  | K_PIN {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("pin", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrPinProp.lefiPropType::setPropType($3, lefPropDefType);
      
    }
  | K_MACRO {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("macro", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrMacroProp.lefiPropType::setPropType($3, lefPropDefType);
    }
  | K_VIA {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("via", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrViaProp.lefiPropType::setPropType($3, lefPropDefType);
    }
  | K_VIARULE {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("viarule", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrViaRuleProp.lefiPropType::setPropType($3, lefPropDefType);
    }
  | K_LAYER {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("layer", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrLayerProp.lefiPropType::setPropType($3, lefPropDefType);
    }
  | K_NONDEFAULTRULE {lefDumbMode = 1; lefrProp.lefiProp::clear(); }
    T_STRING prop_define ';'
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("nondefaultrule", $3);
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrNondefProp.lefiPropType::setPropType($3, lefPropDefType);
    }
    
prop_define:
  K_INTEGER { lefRealNum = 0 } opt_def_range opt_def_value 
    { 
      if (lefrPropCbk) lefrProp.lefiProp::setPropInteger();
      lefPropDefType = 'I'
    }
  | K_REAL { lefRealNum = 1 } opt_def_range opt_def_value
    { 
      if (lefrPropCbk) lefrProp.lefiProp::setPropReal();
      lefPropDefType = 'R';
      lefRealNum = 0;
    }
  | K_STRING
    {
      if (lefrPropCbk) lefrProp.lefiProp::setPropString();
      lefPropDefType = 'S'
    }
  | K_STRING QSTRING
    {
      if (lefrPropCbk) lefrProp.lefiProp::setPropQString($2);
      lefPropDefType = 'Q'
    }
  | K_NAMEMAPSTRING T_STRING
    {
      if (lefrPropCbk) lefrProp.lefiProp::setPropNameMapString($2);
      lefPropDefType = 'S'
    }

opt_range_second:
  /* nothing */
    { }
  | K_USELENGTHTHRESHOLD
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingRangeUseLength();
    }
  | K_INFLUENCE NUMBER
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingRangeInfluence($2);
        lefrLayer.lefiLayer::setSpacingRangeInfluenceRange(-1, -1);
      }
    }
  | K_INFLUENCE NUMBER K_RANGE NUMBER NUMBER
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingRangeInfluence($2);
        lefrLayer.lefiLayer::setSpacingRangeInfluenceRange($4, $5);
      }
    }
  | K_RANGE NUMBER NUMBER
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingRangeRange($2, $3);
    }

opt_endofline:                                      /* 5.7 */
  /* nothing */
    { }
  | K_PARALLELEDGE NUMBER K_WITHIN NUMBER
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingParSW($2, $4);
    }
    opt_endofline_twoedges

opt_endofline_twoedges:                             /* 5.7 */
  /* nothing */
    { }
  | K_TWOEDGES
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingParTwoEdges();
    }

opt_samenetPGonly:                                  /* 5.7 */
  /* nothing */
    { }
  | K_PGONLY
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingSamenetPGonly();
    }

opt_def_range:
  /* nothing */
    { }
  | K_RANGE NUMBER NUMBER
    {  if (lefrPropCbk) lefrProp.lefiProp::setRange($2, $3); }

opt_def_value:
  /* empty */
    { }
  | NUMBER
    { if (lefrPropCbk) lefrProp.lefiProp::setNumber($1); }

layer_spacing_opts:
  /* empty */
  | layer_spacing_opt layer_spacing_opts

layer_spacing_opt: K_CENTERTOCENTER      /* 5.7 */
    {
      if (lefrLayerCbk) {
         if (hasSpCenter) {
           if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1663, "A CENTERTOCENTER statement was already defined in SPACING\nCENTERTOCENTER can only be defined once per LAYER CUT SPACING.");
              CHKERR();
           }
        }
        hasSpCenter = 1;
        if (versionNum < 5.6) {
           if (layerWarnings++ < lefrLayerWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "CENTERTOCENTER statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1664, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
        if (lefrLayerCbk)
          lefrLayer.lefiLayer::setSpacingCenterToCenter();
      }
    }
  | K_SAMENET             /* 5.7 */
    {
      if (lefrLayerCbk) {
        if (hasSpSamenet) {
           if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1665, "A SAMENET statement was already defined in SPACING\nSAMENET can only be defined once per LAYER CUT SPACING.");
              CHKERR();
           }
        }
        hasSpSamenet = 1;
        if (lefrLayerCbk)
          lefrLayer.lefiLayer::setSpacingSamenet();
       }
    }
    opt_samenetPGonly
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "SAMENET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1684, outMsg);
        lefFree(outMsg);
        CHKERR();
      }
    }
  | K_PARALLELOVERLAP    /* 5.7 */
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "PARALLELOVERLAP is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1680, outMsg);
        lefFree(outMsg);
        CHKERR(); 
      } else {
        if (lefrLayerCbk) {
          if (hasSpParallel) {
             if (layerWarnings++ < lefrLayerWarnings) {
                lefError(1666, "A PARALLELOVERLAP statement was already defined in SPACING\nPARALLELOVERLAP can only be defined once per LAYER CUT SPACING.");
                CHKERR();
             }
          }
          hasSpParallel = 1;
          if (lefrLayerCbk)
            lefrLayer.lefiLayer::setSpacingParallelOverlap();
        }
      }
    }

layer_spacing_cut_routing:
  /* empty */
  | K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING
    {
      if (lefrLayerCbk)
{
        if (versionNum < 5.7) {
           if (hasSpSamenet) {    /* 5.6 and earlier does not allow */
              if (layerWarnings++ < lefrLayerWarnings) {
                 lefError(1667, "A SAMENET statement was already defined in SPACING\nEither SAMENET or LAYER can be defined, but not both.");
                 CHKERR();
              }
           }
        }
        lefrLayer.lefiLayer::setSpacingName($3);
      }
    }
    spacing_cut_layer_opt
  | K_ADJACENTCUTS NUMBER K_WITHIN NUMBER
    {
      if (lefrLayerCbk) {
        if (versionNum < 5.5) {
           if (layerWarnings++ < lefrLayerWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "ADJACENTCUTS statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", versionNum);
              lefError(1668, outMsg);
              lefFree(outMsg);
              CHKERR();
           }
        }
        if (versionNum < 5.7) {
           if (hasSpSamenet) {    /* 5.6 and earlier does not allow */
              if (layerWarnings++ < lefrLayerWarnings) {
                 lefError(1669, "A SAMENET statement was already defined in SPACING\nEither SAMENET or ADJACENTCUTS can be defined, but not both.");
                 CHKERR();
              }
           }
        }
        lefrLayer.lefiLayer::setSpacingAdjacent((int)$2, $4);
      }
    }
    opt_adjacentcuts_exceptsame
  | K_AREA NUMBER        /* 5.7 */
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "AREA is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1693, outMsg);
        lefFree(outMsg);
        CHKERR();
      } else {
        if (lefrLayerCbk) {
          if (versionNum < 5.7) {
             if (hasSpSamenet) {    /* 5.6 and earlier does not allow */
                if (layerWarnings++ < lefrLayerWarnings) {
                   lefError(1670, "A SAMENET statement was already defined in SPACING\nEither SAMENET or AREA can be defined, but not both.");
                   CHKERR();
                }
             }
          }
          lefrLayer.lefiLayer::setSpacingArea($2);
        }
      }
    }
  | K_RANGE NUMBER NUMBER
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingRange($2, $3);
    }
    opt_range_second
  | K_LENGTHTHRESHOLD NUMBER
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingLength($2);
      }
    }
  | K_LENGTHTHRESHOLD NUMBER K_RANGE NUMBER NUMBER
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingLength($2);
        lefrLayer.lefiLayer::setSpacingLengthRange($4, $5);
      }
    }
  | K_ENDOFLINE NUMBER K_WITHIN NUMBER    /* 5.7 */
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingEol($2, $4);
    }
    opt_endofline
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "ENDOFLINE is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1681, outMsg);
        lefFree(outMsg);
        CHKERR();
      }
    }
  | K_NOTCHLENGTH NUMBER      /* 5.7 */
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "NOTCHLENGTH is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1682, outMsg);
        lefFree(outMsg);
        CHKERR();
      } else {
        if (lefrLayerCbk)
          lefrLayer.lefiLayer::setSpacingNotchLength($2);
      }
    }
  | K_ENDOFNOTCHWIDTH NUMBER K_NOTCHSPACING NUMBER K_NOTCHLENGTH NUMBER /*5.7*/
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "ENDOFNOTCHWIDTH is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1696, outMsg);
        lefFree(outMsg);
        CHKERR();
      } else {
        if (lefrLayerCbk)
          lefrLayer.lefiLayer::setSpacingEndOfNotchWidth($2, $4, $6);
      }
    }

spacing_cut_layer_opt:                      /* 5.7 */
  /* empty */
    {}
  | K_STACK
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingLayerStack();
    }

opt_adjacentcuts_exceptsame:                /* 5.7 */
  /* empty */
    {}
  | K_EXCEPTSAMEPGNET
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "EXCEPTSAMEPGNET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1683, outMsg);
        lefFree(outMsg);
        CHKERR();
      } else {
        if (lefrLayerCbk)
          lefrLayer.lefiLayer::setSpacingAdjacentExcept();
      }
    }

opt_layer_name:
  /* empty */
    { $$ = 0; }
  | K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING
    { $$ = $3; }

req_layer_name:
  /* pcr 355313 */
   K_LAYER {lefDumbMode = 1; lefNoNum = 1; } T_STRING
    { $$ = $3; }

/* 9/11/2001 - Wanda da Rosa.  The following are obsolete in 5.4 */
universalnoisemargin: K_UNIVERSALNOISEMARGIN NUMBER NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrNoiseMarginCbk) {
          lefrNoiseMargin.low = $2;
          lefrNoiseMargin.high = $3;
          CALLBACK(lefrNoiseMarginCbk, lefrNoiseMarginCbkType, &lefrNoiseMargin);
        }
      } else
        if (lefrNoiseMarginCbk) /* write warning only if cbk is set */
          if (noiseMarginWarnings++ < lefrNoiseMarginWarnings)
            lefWarning(2070, "UNIVERSALNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

edgeratethreshold1: K_EDGERATETHRESHOLD1 NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrEdgeRateThreshold1Cbk) {
          CALLBACK(lefrEdgeRateThreshold1Cbk,
          lefrEdgeRateThreshold1CbkType, $2);
        }
      } else
        if (lefrEdgeRateThreshold1Cbk) /* write warning only if cbk is set */
          if (edgeRateThreshold1Warnings++ < lefrEdgeRateThreshold1Warnings)
            lefWarning(2071, "EDGERATETHRESHOLD1 statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

edgeratethreshold2: K_EDGERATETHRESHOLD2 NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrEdgeRateThreshold2Cbk) {
	  CALLBACK(lefrEdgeRateThreshold2Cbk,
	  lefrEdgeRateThreshold2CbkType, $2);
        }
      } else
        if (lefrEdgeRateThreshold2Cbk) /* write warning only if cbk is set */
          if (edgeRateThreshold2Warnings++ < lefrEdgeRateThreshold2Warnings)
            lefWarning(2072, "EDGERATETHRESHOLD2 statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

edgeratescalefactor: K_EDGERATESCALEFACTOR NUMBER ';'
    {
      if (versionNum < 5.4) {
        if (lefrEdgeRateScaleFactorCbk) {
	  CALLBACK(lefrEdgeRateScaleFactorCbk,
	  lefrEdgeRateScaleFactorCbkType, $2);
        }
      } else
        if (lefrEdgeRateScaleFactorCbk) /* write warning only if cbk is set */
          if (edgeRateScaleFactorWarnings++ < lefrEdgeRateScaleFactorWarnings)
            lefWarning(2073, "EDGERATESCALEFACTOR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

noisetable: K_NOISETABLE NUMBER
    { if (lefrNoiseTableCbk) lefrNoiseTable.lefiNoiseTable::setup((int)$2); }
    ';' noise_table_list end_noisetable dtrm
    { }

end_noisetable:
  K_END K_NOISETABLE
  {
    if (versionNum < 5.4) {
      if (lefrNoiseTableCbk)
        CALLBACK(lefrNoiseTableCbk, lefrNoiseTableCbkType, &lefrNoiseTable);
    } else
      if (lefrNoiseTableCbk) /* write warning only if cbk is set */
        if (noiseTableWarnings++ < lefrNoiseTableWarnings)
          lefWarning(2074, "NOISETABLE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }


noise_table_list :
  noise_table_entry
  | noise_table_list noise_table_entry
  ;

noise_table_entry:
  K_EDGERATE NUMBER ';'
    { if (lefrNoiseTableCbk)
         {
            lefrNoiseTable.lefiNoiseTable::newEdge();
            lefrNoiseTable.lefiNoiseTable::addEdge($2);
         }
    }
  | output_resistance_entry
    { }

output_resistance_entry: K_OUTPUTRESISTANCE
    { if (lefrNoiseTableCbk) lefrNoiseTable.lefiNoiseTable::addResistance(); }
    num_list ';' victim_list
    ;

num_list:
  NUMBER
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addResistanceNumber($1); }
   | num_list NUMBER
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addResistanceNumber($2); }

victim_list:
  victim
  | victim_list victim
  ;

victim: K_VICTIMLENGTH NUMBER ';'
	{ if (lefrNoiseTableCbk)
	lefrNoiseTable.lefiNoiseTable::addVictimLength($2); }
      K_VICTIMNOISE vnoiselist ';'
	{ }

vnoiselist:
  NUMBER
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addVictimNoise($1); }
  | vnoiselist NUMBER
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addVictimNoise($2); }

correctiontable: K_CORRECTIONTABLE NUMBER ';'
    { if (lefrCorrectionTableCbk)
    lefrCorrectionTable.lefiCorrectionTable::setup((int)$2); }
    correction_table_list end_correctiontable dtrm
    { }

end_correctiontable:
  K_END K_CORRECTIONTABLE
  {
    if (versionNum < 5.4) {
      if (lefrCorrectionTableCbk)
        CALLBACK(lefrCorrectionTableCbk, lefrCorrectionTableCbkType,
               &lefrCorrectionTable);
    } else
      if (lefrCorrectionTableCbk) /* write warning only if cbk is set */
        if (correctionTableWarnings++ < lefrCorrectionTableWarnings)
          lefWarning(2075, "CORRECTIONTABLE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }

correction_table_list:
  correction_table_item
  | correction_table_list correction_table_item
  ;

correction_table_item:
  K_EDGERATE NUMBER ';'
    { if (lefrCorrectionTableCbk)
         {
            lefrCorrectionTable.lefiCorrectionTable::newEdge();
            lefrCorrectionTable.lefiCorrectionTable::addEdge($2);
         }
    }
  | output_list
    { }

output_list: K_OUTPUTRESISTANCE
  { if (lefrCorrectionTableCbk)
  lefrCorrectionTable.lefiCorrectionTable::addResistance(); }
  numo_list ';' corr_victim_list
  { }

numo_list:
  NUMBER
    { if (lefrCorrectionTableCbk)
    lefrCorrectionTable.lefiCorrectionTable::addResistanceNumber($1); }
  | numo_list NUMBER
    { if (lefrCorrectionTableCbk)
    lefrCorrectionTable.lefiCorrectionTable::addResistanceNumber($2); }

corr_victim_list:
   corr_victim
   | corr_victim_list corr_victim
   ;

corr_victim:
  K_VICTIMLENGTH NUMBER ';'
     { if (lefrCorrectionTableCbk)
     lefrCorrectionTable.lefiCorrectionTable::addVictimLength($2); }
  K_CORRECTIONFACTOR corr_list ';'
     { }

corr_list:
  NUMBER
    { if (lefrCorrectionTableCbk)
	lefrCorrectionTable.lefiCorrectionTable::addVictimCorrection($1); }
  | corr_list NUMBER
    { if (lefrCorrectionTableCbk)
	lefrCorrectionTable.lefiCorrectionTable::addVictimCorrection($2); }

/* end of 5.4 obsolete syntax */

input_antenna: K_INPUTPINANTENNASIZE NUMBER ';'
    { /* 5.3 syntax */
        use5_3 = 1;
        if (ignoreVersion) {
           /* do nothing */
        } else if (versionNum > 5.3) {
           /* A 5.3 syntax in 5.4 */
           if (use5_4) {
              if (lefrInputAntennaCbk) { /* write warning only if cbk is set */
                if (inputAntennaWarnings++ < lefrInputAntennaWarnings) {
                   outMsg = (char*)lefMalloc(10000);
                   sprintf (outMsg,
                      "INPUTPINANTENNASIZE statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new INPUTPINANTENNASIZE syntax, which is incorrect.", versionNum);
                   lefError(1671, outMsg);
                   lefFree(outMsg);
                   CHKERR();
                }
              }
           }
        }
        if (lefrInputAntennaCbk)
          CALLBACK(lefrInputAntennaCbk, lefrInputAntennaCbkType, $2);
    }

output_antenna: K_OUTPUTPINANTENNASIZE NUMBER ';'
    { /* 5.3 syntax */
        use5_3 = 1;
        if (ignoreVersion) {
           /* do nothing */
        } else if (versionNum > 5.3) {
           /* A 5.3 syntax in 5.4 */
           if (use5_4) {
              if (lefrOutputAntennaCbk) { /* write warning only if cbk is set */
                if (outputAntennaWarnings++ < lefrOutputAntennaWarnings) {
                   outMsg = (char*)lefMalloc(10000);
                   sprintf (outMsg,
                      "OUTPUTPINANTENNASIZE statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new OUTPUTPINANTENNASIZE syntax, which is incorrect.", versionNum);
                   lefError(1672, outMsg);
                   lefFree(outMsg);
                   CHKERR();
                }
              }
           }
        }
        if (lefrOutputAntennaCbk)
          CALLBACK(lefrOutputAntennaCbk, lefrOutputAntennaCbkType, $2);
    }

inout_antenna: K_INOUTPINANTENNASIZE NUMBER ';'
    { /* 5.3 syntax */
        use5_3 = 1;
        if (ignoreVersion) {
           /* do nothing */
        } else if (versionNum > 5.3) {
           /* A 5.3 syntax in 5.4 */
           if (use5_4) {
              if (lefrInoutAntennaCbk) { /* write warning only if cbk is set */
                if (inoutAntennaWarnings++ < lefrInoutAntennaWarnings) {
                   outMsg = (char*)lefMalloc(10000);
                   sprintf (outMsg,
                      "INOUTPINANTENNASIZE statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new INOUTPINANTENNASIZE syntax, which is incorrect.", versionNum);
                   lefError(1673, outMsg);
                   lefFree(outMsg);
                   CHKERR();
                }
              }
           }
        }
        if (lefrInoutAntennaCbk)
          CALLBACK(lefrInoutAntennaCbk, lefrInoutAntennaCbkType, $2);
    }

antenna_input: K_ANTENNAINPUTGATEAREA NUMBER ';'
    { /* 5.4 syntax */
        /* 11/12/2002 - this is obsolete in 5.5, suppose should be ingored */
        /* 12/16/2002 - talked to Dave Noice, leave them in here for debugging*/
        use5_4 = 1;
        if (ignoreVersion) {
           /* do nothing */
        } else if (versionNum < 5.4) {
           if (lefrAntennaInputCbk) { /* write warning only if cbk is set */
             if (antennaInputWarnings++ < lefrAntennaInputWarnings) {
               outMsg = (char*)lefMalloc(10000);
               sprintf (outMsg,
                  "ANTENNAINPUTGATEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.\nEither update your VERSION number or use the 5.3 syntax.", versionNum);
               lefError(1674, outMsg);
               lefFree(outMsg);
               CHKERR();
             }
           }
        } else if (use5_3) {
           if (lefrAntennaInputCbk) { /* write warning only if cbk is set */
             if (antennaInputWarnings++ < lefrAntennaInputWarnings) {
                outMsg = (char*)lefMalloc(10000);
                sprintf (outMsg,
                   "ANTENNAINPUTGATEAREA statement is a version 5.4 or later syntax.\nYour lef file with version %g, has both old and new ANTENNAINPUTGATEAREA syntax, which is incorrect.", versionNum);
                lefError(1675, outMsg);
                lefFree(outMsg);
               CHKERR();
             }
           }
        }
        if (lefrAntennaInputCbk)
          CALLBACK(lefrAntennaInputCbk, lefrAntennaInputCbkType, $2);
    }

antenna_inout: K_ANTENNAINOUTDIFFAREA NUMBER ';'
    { /* 5.4 syntax */
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        /* 12/16/2002 - talked to Dave Noice, leave them in here for debugging*/
        use5_4 = 1;
        if (ignoreVersion) {
           /* do nothing */
        } else if (versionNum < 5.4) {
           if (lefrAntennaInoutCbk) { /* write warning only if cbk is set */
              if (antennaInoutWarnings++ < lefrAntennaInoutWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "ANTENNAINOUTDIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.\nEither update your VERSION number or use the 5.3 syntax.", versionNum);
                 lefError(1676, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        } else if (use5_3) {
           if (lefrAntennaInoutCbk) { /* write warning only if cbk is set */
              if (antennaInoutWarnings++ < lefrAntennaInoutWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "ANTENNAINOUTDIFFAREA statement is a version 5.4 or later syntax.\nYour lef file with version %g, has both old and new ANTENNAINOUTDIFFAREA syntax, which is incorrect.", versionNum);
                 lefError(1677, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
        if (lefrAntennaInoutCbk)
          CALLBACK(lefrAntennaInoutCbk, lefrAntennaInoutCbkType, $2);
    }

antenna_output: K_ANTENNAOUTPUTDIFFAREA NUMBER ';'
    { /* 5.4 syntax */
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        /* 12/16/2002 - talked to Dave Noice, leave them in here for debugging*/
        use5_4 = 1;
        if (ignoreVersion) {
           /* do nothing */
        } else if (versionNum < 5.4) {
           if (lefrAntennaOutputCbk) { /* write warning only if cbk is set */
              if (antennaOutputWarnings++ < lefrAntennaOutputWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "ANTENNAOUTPUTDIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.\nEither update your VERSION number or use the 5.3 syntax.", versionNum);
                 lefError(1678, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        } else if (use5_3) {
           if (lefrAntennaOutputCbk) { /* write warning only if cbk is set */
              if (antennaOutputWarnings++ < lefrAntennaOutputWarnings) {
                 outMsg = (char*)lefMalloc(10000);
                 sprintf (outMsg,
                    "ANTENNAOUTPUTDIFFAREA statement is a version 5.4 or later syntax.\nYour lef file with version %g, has both old and new ANTENNAOUTPUTDIFFAREA syntax, which is incorrect.", versionNum);
                 lefError(1679, outMsg);
                 lefFree(outMsg);
                 CHKERR();
              }
           }
        }
        if (lefrAntennaOutputCbk)
          CALLBACK(lefrAntennaOutputCbk, lefrAntennaOutputCbkType, $2);
    }

extension_opt:  /* empty */
    | extension

extension: K_BEGINEXT
    { 
        if (lefrExtensionCbk)
          CALLBACK(lefrExtensionCbk, lefrExtensionCbkType, Hist_text);
        if (versionNum >= 5.6)
           ge56almostDone = 1;
    }

%%
