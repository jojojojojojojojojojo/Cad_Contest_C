/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         lefyyparse
#define yylex           lefyylex
#define yyerror         lefyyerror
#define yylval          lefyylval
#define yychar          lefyychar
#define yydebug         lefyydebug
#define yynerrs         lefyynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 46 "lef.y"

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



/* Line 189 of yacc.c  */
#line 427 "lef.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     K_HISTORY = 258,
     K_ABUT = 259,
     K_ABUTMENT = 260,
     K_ACTIVE = 261,
     K_ANALOG = 262,
     K_ARRAY = 263,
     K_AREA = 264,
     K_BLOCK = 265,
     K_BOTTOMLEFT = 266,
     K_BOTTOMRIGHT = 267,
     K_BY = 268,
     K_CAPACITANCE = 269,
     K_CAPMULTIPLIER = 270,
     K_CLASS = 271,
     K_CLOCK = 272,
     K_CLOCKTYPE = 273,
     K_COLUMNMAJOR = 274,
     K_DESIGNRULEWIDTH = 275,
     K_INFLUENCE = 276,
     K_CORE = 277,
     K_CORNER = 278,
     K_COVER = 279,
     K_CPERSQDIST = 280,
     K_CURRENT = 281,
     K_CURRENTSOURCE = 282,
     K_CUT = 283,
     K_DEFAULT = 284,
     K_DATABASE = 285,
     K_DATA = 286,
     K_DIELECTRIC = 287,
     K_DIRECTION = 288,
     K_DO = 289,
     K_EDGECAPACITANCE = 290,
     K_EEQ = 291,
     K_END = 292,
     K_ENDCAP = 293,
     K_FALL = 294,
     K_FALLCS = 295,
     K_FALLT0 = 296,
     K_FALLSATT1 = 297,
     K_FALLRS = 298,
     K_FALLSATCUR = 299,
     K_FALLTHRESH = 300,
     K_FEEDTHRU = 301,
     K_FIXED = 302,
     K_FOREIGN = 303,
     K_FROMPIN = 304,
     K_GENERATE = 305,
     K_GENERATOR = 306,
     K_GROUND = 307,
     K_HEIGHT = 308,
     K_HORIZONTAL = 309,
     K_INOUT = 310,
     K_INPUT = 311,
     K_INPUTNOISEMARGIN = 312,
     K_COMPONENTPIN = 313,
     K_INTRINSIC = 314,
     K_INVERT = 315,
     K_IRDROP = 316,
     K_ITERATE = 317,
     K_IV_TABLES = 318,
     K_LAYER = 319,
     K_LEAKAGE = 320,
     K_LEQ = 321,
     K_LIBRARY = 322,
     K_MACRO = 323,
     K_MATCH = 324,
     K_MAXDELAY = 325,
     K_MAXLOAD = 326,
     K_METALOVERHANG = 327,
     K_MILLIAMPS = 328,
     K_MILLIWATTS = 329,
     K_MINFEATURE = 330,
     K_MUSTJOIN = 331,
     K_NAMESCASESENSITIVE = 332,
     K_NANOSECONDS = 333,
     K_NETS = 334,
     K_NEW = 335,
     K_NONDEFAULTRULE = 336,
     K_NONINVERT = 337,
     K_NONUNATE = 338,
     K_OBS = 339,
     K_OHMS = 340,
     K_OFFSET = 341,
     K_ORIENTATION = 342,
     K_ORIGIN = 343,
     K_OUTPUT = 344,
     K_OUTPUTNOISEMARGIN = 345,
     K_OVERHANG = 346,
     K_OVERLAP = 347,
     K_OFF = 348,
     K_ON = 349,
     K_OVERLAPS = 350,
     K_PAD = 351,
     K_PATH = 352,
     K_PATTERN = 353,
     K_PICOFARADS = 354,
     K_PIN = 355,
     K_PITCH = 356,
     K_PLACED = 357,
     K_POLYGON = 358,
     K_PORT = 359,
     K_POST = 360,
     K_POWER = 361,
     K_PRE = 362,
     K_PULLDOWNRES = 363,
     K_RECT = 364,
     K_RESISTANCE = 365,
     K_RESISTIVE = 366,
     K_RING = 367,
     K_RISE = 368,
     K_RISECS = 369,
     K_RISERS = 370,
     K_RISESATCUR = 371,
     K_RISETHRESH = 372,
     K_RISESATT1 = 373,
     K_RISET0 = 374,
     K_RISEVOLTAGETHRESHOLD = 375,
     K_FALLVOLTAGETHRESHOLD = 376,
     K_ROUTING = 377,
     K_ROWMAJOR = 378,
     K_RPERSQ = 379,
     K_SAMENET = 380,
     K_SCANUSE = 381,
     K_SHAPE = 382,
     K_SHRINKAGE = 383,
     K_SIGNAL = 384,
     K_SITE = 385,
     K_SIZE = 386,
     K_SOURCE = 387,
     K_SPACER = 388,
     K_SPACING = 389,
     K_SPECIALNETS = 390,
     K_STACK = 391,
     K_START = 392,
     K_STEP = 393,
     K_STOP = 394,
     K_STRUCTURE = 395,
     K_SYMMETRY = 396,
     K_TABLE = 397,
     K_THICKNESS = 398,
     K_TIEHIGH = 399,
     K_TIELOW = 400,
     K_TIEOFFR = 401,
     K_TIME = 402,
     K_TIMING = 403,
     K_TO = 404,
     K_TOPIN = 405,
     K_TOPLEFT = 406,
     K_TOPRIGHT = 407,
     K_TOPOFSTACKONLY = 408,
     K_TRISTATE = 409,
     K_TYPE = 410,
     K_UNATENESS = 411,
     K_UNITS = 412,
     K_USE = 413,
     K_VARIABLE = 414,
     K_VERTICAL = 415,
     K_VHI = 416,
     K_VIA = 417,
     K_VIARULE = 418,
     K_VLO = 419,
     K_VOLTAGE = 420,
     K_VOLTS = 421,
     K_WIDTH = 422,
     K_X = 423,
     K_Y = 424,
     T_STRING = 425,
     QSTRING = 426,
     NUMBER = 427,
     K_N = 428,
     K_S = 429,
     K_E = 430,
     K_W = 431,
     K_FN = 432,
     K_FS = 433,
     K_FE = 434,
     K_FW = 435,
     K_R0 = 436,
     K_R90 = 437,
     K_R180 = 438,
     K_R270 = 439,
     K_MX = 440,
     K_MY = 441,
     K_MXR90 = 442,
     K_MYR90 = 443,
     K_USER = 444,
     K_MASTERSLICE = 445,
     K_ENDMACRO = 446,
     K_ENDMACROPIN = 447,
     K_ENDVIARULE = 448,
     K_ENDVIA = 449,
     K_ENDLAYER = 450,
     K_ENDSITE = 451,
     K_CANPLACE = 452,
     K_CANNOTOCCUPY = 453,
     K_TRACKS = 454,
     K_FLOORPLAN = 455,
     K_GCELLGRID = 456,
     K_DEFAULTCAP = 457,
     K_MINPINS = 458,
     K_WIRECAP = 459,
     K_STABLE = 460,
     K_SETUP = 461,
     K_HOLD = 462,
     K_DEFINE = 463,
     K_DEFINES = 464,
     K_DEFINEB = 465,
     K_IF = 466,
     K_THEN = 467,
     K_ELSE = 468,
     K_FALSE = 469,
     K_TRUE = 470,
     K_EQ = 471,
     K_NE = 472,
     K_LE = 473,
     K_LT = 474,
     K_GE = 475,
     K_GT = 476,
     K_OR = 477,
     K_AND = 478,
     K_NOT = 479,
     K_DELAY = 480,
     K_TABLEDIMENSION = 481,
     K_TABLEAXIS = 482,
     K_TABLEENTRIES = 483,
     K_TRANSITIONTIME = 484,
     K_EXTENSION = 485,
     K_PROPDEF = 486,
     K_STRING = 487,
     K_INTEGER = 488,
     K_REAL = 489,
     K_RANGE = 490,
     K_PROPERTY = 491,
     K_VIRTUAL = 492,
     K_BUSBITCHARS = 493,
     K_VERSION = 494,
     K_BEGINEXT = 495,
     K_ENDEXT = 496,
     K_UNIVERSALNOISEMARGIN = 497,
     K_EDGERATETHRESHOLD1 = 498,
     K_CORRECTIONTABLE = 499,
     K_EDGERATESCALEFACTOR = 500,
     K_EDGERATETHRESHOLD2 = 501,
     K_VICTIMNOISE = 502,
     K_NOISETABLE = 503,
     K_EDGERATE = 504,
     K_OUTPUTRESISTANCE = 505,
     K_VICTIMLENGTH = 506,
     K_CORRECTIONFACTOR = 507,
     K_OUTPUTPINANTENNASIZE = 508,
     K_INPUTPINANTENNASIZE = 509,
     K_INOUTPINANTENNASIZE = 510,
     K_CURRENTDEN = 511,
     K_PWL = 512,
     K_ANTENNALENGTHFACTOR = 513,
     K_TAPERRULE = 514,
     K_DIVIDERCHAR = 515,
     K_ANTENNASIZE = 516,
     K_ANTENNAMETALLENGTH = 517,
     K_ANTENNAMETALAREA = 518,
     K_RISESLEWLIMIT = 519,
     K_FALLSLEWLIMIT = 520,
     K_FUNCTION = 521,
     K_BUFFER = 522,
     K_INVERTER = 523,
     K_NAMEMAPSTRING = 524,
     K_NOWIREEXTENSIONATPIN = 525,
     K_WIREEXTENSION = 526,
     K_MESSAGE = 527,
     K_CREATEFILE = 528,
     K_OPENFILE = 529,
     K_CLOSEFILE = 530,
     K_WARNING = 531,
     K_ERROR = 532,
     K_FATALERROR = 533,
     K_RECOVERY = 534,
     K_SKEW = 535,
     K_ANYEDGE = 536,
     K_POSEDGE = 537,
     K_NEGEDGE = 538,
     K_SDFCONDSTART = 539,
     K_SDFCONDEND = 540,
     K_SDFCOND = 541,
     K_MPWH = 542,
     K_MPWL = 543,
     K_PERIOD = 544,
     K_ACCURRENTDENSITY = 545,
     K_DCCURRENTDENSITY = 546,
     K_AVERAGE = 547,
     K_PEAK = 548,
     K_RMS = 549,
     K_FREQUENCY = 550,
     K_CUTAREA = 551,
     K_MEGAHERTZ = 552,
     K_USELENGTHTHRESHOLD = 553,
     K_LENGTHTHRESHOLD = 554,
     K_ANTENNAINPUTGATEAREA = 555,
     K_ANTENNAINOUTDIFFAREA = 556,
     K_ANTENNAOUTPUTDIFFAREA = 557,
     K_ANTENNAAREARATIO = 558,
     K_ANTENNADIFFAREARATIO = 559,
     K_ANTENNACUMAREARATIO = 560,
     K_ANTENNACUMDIFFAREARATIO = 561,
     K_ANTENNAAREAFACTOR = 562,
     K_ANTENNASIDEAREARATIO = 563,
     K_ANTENNADIFFSIDEAREARATIO = 564,
     K_ANTENNACUMSIDEAREARATIO = 565,
     K_ANTENNACUMDIFFSIDEAREARATIO = 566,
     K_ANTENNASIDEAREAFACTOR = 567,
     K_DIFFUSEONLY = 568,
     K_MANUFACTURINGGRID = 569,
     K_FIXEDMASK = 570,
     K_ANTENNACELL = 571,
     K_CLEARANCEMEASURE = 572,
     K_EUCLIDEAN = 573,
     K_MAXXY = 574,
     K_USEMINSPACING = 575,
     K_ROWMINSPACING = 576,
     K_ROWABUTSPACING = 577,
     K_FLIP = 578,
     K_NONE = 579,
     K_ANTENNAPARTIALMETALAREA = 580,
     K_ANTENNAPARTIALMETALSIDEAREA = 581,
     K_ANTENNAGATEAREA = 582,
     K_ANTENNADIFFAREA = 583,
     K_ANTENNAMAXAREACAR = 584,
     K_ANTENNAMAXSIDEAREACAR = 585,
     K_ANTENNAPARTIALCUTAREA = 586,
     K_ANTENNAMAXCUTCAR = 587,
     K_SLOTWIREWIDTH = 588,
     K_SLOTWIRELENGTH = 589,
     K_SLOTWIDTH = 590,
     K_SLOTLENGTH = 591,
     K_MAXADJACENTSLOTSPACING = 592,
     K_MAXCOAXIALSLOTSPACING = 593,
     K_MAXEDGESLOTSPACING = 594,
     K_SPLITWIREWIDTH = 595,
     K_MINIMUMDENSITY = 596,
     K_MAXIMUMDENSITY = 597,
     K_DENSITYCHECKWINDOW = 598,
     K_DENSITYCHECKSTEP = 599,
     K_FILLACTIVESPACING = 600,
     K_MINIMUMCUT = 601,
     K_ADJACENTCUTS = 602,
     K_ANTENNAMODEL = 603,
     K_BUMP = 604,
     K_ENCLOSURE = 605,
     K_FROMABOVE = 606,
     K_FROMBELOW = 607,
     K_IMPLANT = 608,
     K_LENGTH = 609,
     K_MAXVIASTACK = 610,
     K_AREAIO = 611,
     K_BLACKBOX = 612,
     K_MAXWIDTH = 613,
     K_MINENCLOSEDAREA = 614,
     K_MINSTEP = 615,
     K_ORIENT = 616,
     K_OXIDE1 = 617,
     K_OXIDE2 = 618,
     K_OXIDE3 = 619,
     K_OXIDE4 = 620,
     K_PARALLELRUNLENGTH = 621,
     K_MINWIDTH = 622,
     K_PROTRUSIONWIDTH = 623,
     K_SPACINGTABLE = 624,
     K_WITHIN = 625,
     K_ABOVE = 626,
     K_BELOW = 627,
     K_CENTERTOCENTER = 628,
     K_CUTSIZE = 629,
     K_CUTSPACING = 630,
     K_DENSITY = 631,
     K_DIAG45 = 632,
     K_DIAG135 = 633,
     K_MASK = 634,
     K_DIAGMINEDGELENGTH = 635,
     K_DIAGSPACING = 636,
     K_DIAGPITCH = 637,
     K_DIAGWIDTH = 638,
     K_GENERATED = 639,
     K_GROUNDSENSITIVITY = 640,
     K_HARDSPACING = 641,
     K_INSIDECORNER = 642,
     K_LAYERS = 643,
     K_LENGTHSUM = 644,
     K_MICRONS = 645,
     K_MINCUTS = 646,
     K_MINSIZE = 647,
     K_NETEXPR = 648,
     K_OUTSIDECORNER = 649,
     K_PREFERENCLOSURE = 650,
     K_ROWCOL = 651,
     K_ROWPATTERN = 652,
     K_SOFT = 653,
     K_SUPPLYSENSITIVITY = 654,
     K_USEVIA = 655,
     K_USEVIARULE = 656,
     K_WELLTAP = 657,
     K_ARRAYCUTS = 658,
     K_ARRAYSPACING = 659,
     K_ANTENNAAREADIFFREDUCEPWL = 660,
     K_ANTENNAAREAMINUSDIFF = 661,
     K_ANTENNACUMROUTINGPLUSCUT = 662,
     K_ANTENNAGATEPLUSDIFF = 663,
     K_ENDOFLINE = 664,
     K_ENDOFNOTCHWIDTH = 665,
     K_EXCEPTEXTRACUT = 666,
     K_EXCEPTSAMEPGNET = 667,
     K_EXCEPTPGNET = 668,
     K_LONGARRAY = 669,
     K_MAXEDGES = 670,
     K_NOTCHLENGTH = 671,
     K_NOTCHSPACING = 672,
     K_ORTHOGONAL = 673,
     K_PARALLELEDGE = 674,
     K_PARALLELOVERLAP = 675,
     K_PGONLY = 676,
     K_PRL = 677,
     K_TWOEDGES = 678,
     K_TWOWIDTHS = 679,
     IF = 680,
     LNOT = 681,
     UMINUS = 682
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 392 "lef.y"

	double    dval ;
	int       integer ;
	char *    string ;
	lefPOINT  pt;



/* Line 214 of yacc.c  */
#line 899 "lef.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 911 "lef.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2087

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  439
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  448
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1017
/* YYNRULES -- Number of states.  */
#define YYNSTATES  2047

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   682

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     436,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     433,   434,   429,   428,     2,   427,     2,   430,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   432,
     437,   435,   438,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   431
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,     8,    13,    17,    21,    22,    25,
      27,    28,    31,    33,    35,    37,    39,    41,    43,    45,
      47,    49,    51,    53,    55,    57,    59,    61,    63,    65,
      67,    69,    71,    73,    75,    77,    79,    81,    83,    85,
      87,    89,    91,    93,    95,    97,    99,   101,   103,   105,
     107,   109,   113,   117,   121,   125,   128,   132,   137,   141,
     143,   145,   147,   149,   151,   153,   158,   160,   161,   164,
     169,   174,   179,   184,   189,   194,   199,   204,   208,   209,
     213,   214,   218,   219,   222,   223,   224,   238,   242,   246,
     250,   255,   259,   264,   268,   273,   277,   281,   285,   289,
     290,   297,   298,   308,   312,   317,   325,   330,   338,   342,
     346,   350,   354,   358,   362,   366,   370,   377,   384,   385,
     390,   391,   396,   401,   406,   407,   408,   418,   419,   420,
     430,   434,   435,   440,   444,   445,   450,   451,   457,   461,
     462,   467,   471,   472,   477,   478,   484,   485,   490,   493,
     497,   501,   502,   503,   513,   517,   521,   525,   529,   533,
     537,   541,   545,   549,   553,   558,   562,   566,   570,   574,
     575,   581,   582,   592,   593,   599,   607,   608,   613,   614,
     622,   623,   631,   635,   639,   640,   646,   647,   649,   650,
     653,   654,   657,   662,   663,   664,   665,   666,   678,   679,
     680,   690,   691,   701,   702,   705,   710,   711,   713,   715,
     716,   717,   722,   725,   726,   729,   730,   733,   734,   737,
     738,   740,   742,   743,   748,   749,   752,   754,   757,   760,
     762,   764,   766,   768,   769,   777,   778,   781,   783,   784,
     786,   788,   790,   792,   793,   794,   795,   808,   809,   810,
     816,   817,   823,   824,   830,   831,   834,   836,   839,   842,
     845,   848,   850,   853,   858,   860,   863,   868,   870,   873,
     878,   880,   882,   884,   886,   888,   890,   892,   894,   896,
     898,   899,   902,   904,   906,   908,   910,   911,   914,   915,
     920,   921,   924,   925,   932,   933,   936,   937,   940,   947,
     951,   952,   960,   961,   966,   968,   971,   975,   979,   980,
     981,   982,  1009,  1010,  1013,  1018,  1023,  1030,  1031,  1036,
    1038,  1040,  1043,  1044,  1047,  1049,  1051,  1055,  1056,  1061,
    1063,  1065,  1068,  1071,  1074,  1077,  1080,  1084,  1089,  1093,
    1094,  1098,  1100,  1102,  1104,  1106,  1108,  1110,  1112,  1114,
    1116,  1118,  1120,  1122,  1124,  1126,  1128,  1130,  1133,  1134,
    1139,  1140,  1143,  1149,  1150,  1159,  1160,  1164,  1165,  1169,
    1175,  1176,  1184,  1185,  1187,  1189,  1192,  1193,  1195,  1197,
    1200,  1201,  1206,  1208,  1211,  1214,  1217,  1220,  1223,  1224,
    1227,  1231,  1232,  1237,  1238,  1241,  1245,  1249,  1254,  1260,
    1265,  1271,  1275,  1279,  1283,  1284,  1288,  1292,  1294,  1297,
    1298,  1301,  1307,  1314,  1316,  1317,  1320,  1324,  1326,  1329,
    1330,  1333,  1337,  1338,  1341,  1344,  1347,  1352,  1356,  1357,
    1358,  1359,  1368,  1370,  1373,  1374,  1377,  1378,  1381,  1383,
    1385,  1387,  1389,  1391,  1393,  1395,  1399,  1403,  1408,  1409,
    1414,  1416,  1419,  1422,  1425,  1428,  1429,  1430,  1431,  1432,
    1445,  1446,  1449,  1453,  1457,  1462,  1467,  1471,  1475,  1479,
    1480,  1484,  1485,  1489,  1490,  1493,  1499,  1501,  1503,  1505,
    1509,  1513,  1517,  1521,  1522,  1525,  1527,  1529,  1531,  1532,
    1537,  1538,  1541,  1542,  1546,  1549,  1554,  1555,  1560,  1561,
    1565,  1566,  1570,  1571,  1574,  1576,  1578,  1580,  1582,  1584,
    1586,  1588,  1590,  1592,  1594,  1596,  1598,  1600,  1602,  1606,
    1610,  1612,  1614,  1616,  1618,  1619,  1624,  1626,  1629,  1633,
    1634,  1637,  1639,  1641,  1643,  1646,  1649,  1652,  1656,  1658,
    1661,  1663,  1665,  1668,  1671,  1673,  1675,  1677,  1679,  1682,
    1684,  1686,  1689,  1692,  1694,  1696,  1698,  1700,  1702,  1704,
    1706,  1708,  1710,  1712,  1714,  1716,  1718,  1720,  1722,  1724,
    1726,  1728,  1732,  1737,  1741,  1745,  1749,  1753,  1757,  1760,
    1764,  1769,  1773,  1776,  1777,  1782,  1783,  1788,  1792,  1796,
    1798,  1800,  1806,  1810,  1811,  1815,  1816,  1820,  1821,  1824,
    1827,  1831,  1836,  1840,  1845,  1851,  1852,  1857,  1861,  1863,
    1867,  1871,  1875,  1879,  1883,  1887,  1891,  1895,  1899,  1903,
    1907,  1908,  1913,  1914,  1920,  1921,  1927,  1928,  1934,  1938,
    1942,  1946,  1950,  1954,  1958,  1962,  1966,  1970,  1975,  1979,
    1980,  1985,  1990,  1993,  1998,  2003,  2008,  2012,  2016,  2021,
    2026,  2031,  2036,  2041,  2046,  2051,  2056,  2057,  2062,  2063,
    2068,  2069,  2074,  2075,  2080,  2082,  2084,  2086,  2088,  2090,
    2093,  2096,  2099,  2102,  2106,  2110,  2115,  2119,  2123,  2125,
    2126,  2130,  2132,  2134,  2136,  2138,  2140,  2142,  2144,  2146,
    2148,  2150,  2151,  2153,  2155,  2157,  2160,  2161,  2162,  2170,
    2174,  2180,  2188,  2194,  2202,  2210,  2220,  2222,  2223,  2226,
    2227,  2229,  2230,  2233,  2236,  2238,  2240,  2241,  2244,  2245,
    2252,  2253,  2262,  2270,  2282,  2287,  2288,  2289,  2300,  2301,
    2302,  2313,  2320,  2327,  2328,  2331,  2333,  2340,  2347,  2351,
    2354,  2356,  2361,  2362,  2365,  2366,  2367,  2375,  2376,  2379,
    2385,  2386,  2391,  2395,  2397,  2400,  2401,  2404,  2405,  2410,
    2411,  2416,  2417,  2428,  2438,  2442,  2446,  2451,  2456,  2461,
    2466,  2471,  2476,  2481,  2486,  2490,  2498,  2507,  2514,  2518,
    2522,  2526,  2529,  2531,  2533,  2535,  2537,  2539,  2541,  2543,
    2545,  2547,  2549,  2551,  2553,  2555,  2557,  2559,  2561,  2564,
    2570,  2572,  2575,  2576,  2581,  2589,  2591,  2593,  2595,  2597,
    2599,  2601,  2604,  2606,  2609,  2610,  2615,  2616,  2620,  2621,
    2625,  2626,  2629,  2630,  2635,  2636,  2641,  2642,  2647,  2648,
    2653,  2658,  2659,  2664,  2670,  2672,  2675,  2676,  2679,  2680,
    2685,  2686,  2691,  2692,  2695,  2701,  2702,  2709,  2710,  2717,
    2718,  2725,  2726,  2733,  2734,  2741,  2742,  2744,  2746,  2748,
    2751,  2753,  2756,  2760,  2764,  2768,  2772,  2775,  2779,  2786,
    2788,  2792,  2796,  2800,  2804,  2808,  2812,  2816,  2820,  2824,
    2828,  2831,  2835,  2842,  2844,  2846,  2850,  2854,  2861,  2863,
    2865,  2867,  2869,  2871,  2873,  2875,  2877,  2879,  2881,  2882,
    2888,  2889,  2892,  2893,  2899,  2900,  2906,  2907,  2913,  2914,
    2920,  2921,  2927,  2928,  2934,  2935,  2941,  2942,  2948,  2949,
    2954,  2955,  2960,  2962,  2965,  2968,  2969,  2971,  2974,  2980,
    2984,  2985,  2986,  2993,  2994,  2996,  2997,  2999,  3000,  3004,
    3005,  3007,  3008,  3011,  3013,  3014,  3018,  3020,  3021,  3022,
    3023,  3029,  3030,  3037,  3040,  3041,  3047,  3050,  3056,  3057,
    3064,  3067,  3074,  3075,  3077,  3078,  3080,  3081,  3082,  3086,
    3087,  3091,  3096,  3100,  3104,  3108,  3109,  3117,  3120,  3122,
    3125,  3129,  3131,  3132,  3138,  3140,  3143,  3145,  3148,  3149,
    3157,  3159,  3162,  3163,  3171,  3174,  3176,  3179,  3183,  3185,
    3186,  3192,  3194,  3197,  3199,  3202,  3203,  3211,  3213,  3216,
    3220,  3224,  3228,  3232,  3236,  3240,  3241,  3243
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     440,     0,    -1,   445,   885,   446,    -1,    -1,   239,   442,
     170,   432,    -1,   260,   171,   432,    -1,   238,   171,   432,
      -1,    -1,   445,   447,    -1,     1,    -1,    -1,    37,    67,
      -1,   441,    -1,   444,    -1,   448,    -1,   457,    -1,   461,
      -1,   558,    -1,   590,    -1,   591,    -1,   443,    -1,   449,
      -1,   799,    -1,   609,    -1,   625,    -1,   624,    -1,   616,
      -1,   647,    -1,   664,    -1,   779,    -1,   803,    -1,   626,
      -1,   814,    -1,   851,    -1,   852,    -1,   854,    -1,   853,
      -1,   855,    -1,   867,    -1,   879,    -1,   880,    -1,   881,
      -1,   882,    -1,   883,    -1,   884,    -1,   451,    -1,   450,
      -1,   452,    -1,   453,    -1,   556,    -1,   801,    -1,    77,
      94,   432,    -1,    77,    93,   432,    -1,   270,    94,   432,
      -1,   270,    93,   432,    -1,   315,   432,    -1,   314,   172,
     432,    -1,   320,   455,   456,   432,    -1,   317,   454,   432,
      -1,   319,    -1,   318,    -1,    84,    -1,   100,    -1,    94,
      -1,    93,    -1,   458,   459,    37,   157,    -1,   157,    -1,
      -1,   459,   460,    -1,   147,    78,   172,   432,    -1,    14,
      99,   172,   432,    -1,   110,    85,   172,   432,    -1,   106,
      74,   172,   432,    -1,    26,    73,   172,   432,    -1,   165,
     166,   172,   432,    -1,    30,   390,   172,   432,    -1,   295,
     297,   172,   432,    -1,   462,   466,   464,    -1,    -1,    64,
     463,   170,    -1,    -1,    37,   465,   170,    -1,    -1,   466,
     467,    -1,    -1,    -1,   404,   468,   494,   495,   375,   172,
     403,   172,   134,   172,   469,   496,   432,    -1,   155,   543,
     432,    -1,   379,   172,   432,    -1,   101,   172,   432,    -1,
     101,   172,   172,   432,    -1,   382,   172,   432,    -1,   382,
     172,   172,   432,    -1,    86,   172,   432,    -1,    86,   172,
     172,   432,    -1,   383,   172,   432,    -1,   381,   172,   432,
      -1,   167,   172,   432,    -1,     9,   172,   432,    -1,    -1,
     134,   172,   470,   836,   839,   432,    -1,    -1,   369,   418,
     370,   172,   134,   172,   471,   506,   432,    -1,    33,   544,
     432,    -1,   110,   124,   172,   432,    -1,   110,   124,   257,
     433,   541,   434,   432,    -1,    14,    25,   172,   432,    -1,
      14,    25,   257,   433,   539,   434,   432,    -1,    53,   172,
     432,    -1,   271,   172,   432,    -1,   143,   172,   432,    -1,
     128,   172,   432,    -1,    15,   172,   432,    -1,    35,   172,
     432,    -1,   258,   172,   432,    -1,   256,   172,   432,    -1,
     256,   257,   433,   537,   434,   432,    -1,   256,   433,   172,
     172,   434,   432,    -1,    -1,   236,   472,   535,   432,    -1,
      -1,   290,   524,   473,   525,    -1,   290,   524,   172,   432,
      -1,   291,   292,   172,   432,    -1,    -1,    -1,   291,   292,
     296,   172,   474,   534,   432,   475,   532,    -1,    -1,    -1,
     291,   292,   167,   172,   476,   534,   432,   477,   532,    -1,
     303,   172,   432,    -1,    -1,   304,   478,   519,   432,    -1,
     305,   172,   432,    -1,    -1,   306,   479,   519,   432,    -1,
      -1,   307,   172,   480,   523,   432,    -1,   308,   172,   432,
      -1,    -1,   309,   481,   519,   432,    -1,   310,   172,   432,
      -1,    -1,   311,   482,   519,   432,    -1,    -1,   312,   172,
     483,   523,   432,    -1,    -1,   348,   484,   546,   432,    -1,
     407,   432,    -1,   408,   172,   432,    -1,   406,   172,   432,
      -1,    -1,    -1,   405,   433,   663,   663,   485,   521,   434,
     432,   486,    -1,   333,   172,   432,    -1,   334,   172,   432,
      -1,   335,   172,   432,    -1,   336,   172,   432,    -1,   337,
     172,   432,    -1,   338,   172,   432,    -1,   339,   172,   432,
      -1,   340,   172,   432,    -1,   341,   172,   432,    -1,   342,
     172,   432,    -1,   343,   172,   172,   432,    -1,   344,   172,
     432,    -1,   345,   172,   432,    -1,   358,   172,   432,    -1,
     367,   172,   432,    -1,    -1,   359,   172,   487,   545,   432,
      -1,    -1,   346,   172,   167,   172,   488,   513,   514,   515,
     432,    -1,    -1,   360,   172,   489,   516,   432,    -1,   368,
     172,   354,   172,   167,   172,   432,    -1,    -1,   369,   490,
     498,   432,    -1,    -1,   350,   508,   172,   172,   491,   509,
     432,    -1,    -1,   395,   508,   172,   172,   492,   512,   432,
      -1,   110,   172,   432,    -1,   380,   172,   432,    -1,    -1,
     392,   493,   731,   733,   432,    -1,    -1,   414,    -1,    -1,
     167,   172,    -1,    -1,   497,   496,    -1,   403,   172,   134,
     172,    -1,    -1,    -1,    -1,    -1,   366,   172,   499,   534,
     500,   167,   172,   501,   534,   502,   547,    -1,    -1,    -1,
     424,   167,   172,   553,   172,   503,   534,   504,   550,    -1,
      -1,    21,   167,   172,   370,   172,   134,   172,   505,   554,
      -1,    -1,   507,   506,    -1,   370,   172,   134,   172,    -1,
      -1,   371,    -1,   372,    -1,    -1,    -1,   167,   172,   510,
     511,    -1,   354,   172,    -1,    -1,   411,   172,    -1,    -1,
     167,   172,    -1,    -1,   370,   172,    -1,    -1,   351,    -1,
     352,    -1,    -1,   354,   172,   370,   172,    -1,    -1,   516,
     517,    -1,   518,    -1,   389,   172,    -1,   415,   172,    -1,
     387,    -1,   394,    -1,   138,    -1,   172,    -1,    -1,   257,
     433,   663,   663,   520,   521,   434,    -1,    -1,   521,   522,
      -1,   663,    -1,    -1,   313,    -1,   293,    -1,   292,    -1,
     294,    -1,    -1,    -1,    -1,   295,   172,   526,   534,   432,
     527,   529,   228,   172,   528,   534,   432,    -1,    -1,    -1,
     296,   172,   530,   534,   432,    -1,    -1,   167,   172,   531,
     534,   432,    -1,    -1,   228,   172,   533,   534,   432,    -1,
      -1,   534,   172,    -1,   536,    -1,   535,   536,    -1,   170,
     170,    -1,   170,   171,    -1,   170,   172,    -1,   538,    -1,
     537,   538,    -1,   433,   172,   172,   434,    -1,   540,    -1,
     539,   540,    -1,   433,   172,   172,   434,    -1,   542,    -1,
     541,   542,    -1,   433,   172,   172,   434,    -1,   122,    -1,
      28,    -1,    92,    -1,   190,    -1,   237,    -1,   353,    -1,
      54,    -1,   160,    -1,   377,    -1,   378,    -1,    -1,   167,
     172,    -1,   362,    -1,   363,    -1,   364,    -1,   365,    -1,
      -1,   547,   548,    -1,    -1,   167,   172,   549,   534,    -1,
      -1,   551,   550,    -1,    -1,   167,   172,   553,   172,   552,
     534,    -1,    -1,   422,   172,    -1,    -1,   554,   555,    -1,
     167,   172,   370,   172,   134,   172,    -1,   355,   172,   432,
      -1,    -1,   355,   172,   235,   557,   170,   170,   432,    -1,
      -1,   561,   559,   569,   586,    -1,   162,    -1,   560,   170,
      -1,   560,   170,    29,    -1,   560,   170,   384,    -1,    -1,
      -1,    -1,   163,   563,   170,   432,   374,   172,   172,   432,
     388,   564,   170,   170,   170,   432,   375,   172,   172,   432,
     350,   172,   172,   172,   172,   432,   565,   566,    -1,    -1,
     566,   567,    -1,   396,   172,   172,   432,    -1,    88,   172,
     172,   432,    -1,    86,   172,   172,   172,   172,   432,    -1,
      -1,    98,   568,   170,   432,    -1,   562,    -1,   570,    -1,
     572,   571,    -1,    -1,   571,   572,    -1,   576,    -1,   580,
      -1,   110,   172,   432,    -1,    -1,   236,   573,   574,   432,
      -1,   153,    -1,   575,    -1,   574,   575,    -1,   170,   172,
      -1,   170,   171,    -1,   170,   170,    -1,   577,   432,    -1,
     577,   663,   432,    -1,   577,   663,   579,   432,    -1,   577,
     579,   432,    -1,    -1,    48,   578,   170,    -1,   173,    -1,
     176,    -1,   174,    -1,   175,    -1,   177,    -1,   180,    -1,
     178,    -1,   179,    -1,   181,    -1,   182,    -1,   183,    -1,
     184,    -1,   186,    -1,   188,    -1,   185,    -1,   187,    -1,
     581,   583,    -1,    -1,    64,   582,   170,   432,    -1,    -1,
     583,   584,    -1,   109,   615,   663,   663,   432,    -1,    -1,
     103,   615,   585,   731,   732,   732,   733,   432,    -1,    -1,
      37,   587,   170,    -1,    -1,   163,   589,   170,    -1,   588,
     594,   601,   595,   607,    -1,    -1,   588,    50,   593,   592,
     594,   595,   607,    -1,    -1,    29,    -1,   600,    -1,   594,
     600,    -1,    -1,   596,    -1,   597,    -1,   596,   597,    -1,
      -1,   236,   598,   599,   432,    -1,   597,    -1,   599,   597,
      -1,   170,   170,    -1,   170,   171,    -1,   170,   172,    -1,
     603,   605,    -1,    -1,   601,   602,    -1,   560,   170,   432,
      -1,    -1,    64,   604,   170,   432,    -1,    -1,   605,   606,
      -1,    33,    54,   432,    -1,    33,   160,   432,    -1,   350,
     172,   172,   432,    -1,   167,   172,   149,   172,   432,    -1,
     109,   663,   663,   432,    -1,   134,   172,    13,   172,   432,
      -1,   110,   172,   432,    -1,    91,   172,   432,    -1,    72,
     172,   432,    -1,    -1,    37,   608,   170,    -1,   610,   612,
     611,    -1,   134,    -1,    37,   134,    -1,    -1,   612,   613,
      -1,   614,   170,   170,   172,   432,    -1,   614,   170,   170,
     172,   136,   432,    -1,   125,    -1,    -1,   379,   172,    -1,
     617,   619,   618,    -1,    61,    -1,    37,    61,    -1,    -1,
     619,   620,    -1,   623,   621,   432,    -1,    -1,   621,   622,
      -1,   172,   172,    -1,   142,   170,    -1,    75,   172,   172,
     432,    -1,    32,   172,   432,    -1,    -1,    -1,    -1,    81,
     627,   170,   628,   631,   632,   629,   630,    -1,    37,    -1,
      37,   170,    -1,    -1,   386,   432,    -1,    -1,   632,   633,
      -1,   640,    -1,   558,    -1,   609,    -1,   637,    -1,   634,
      -1,   635,    -1,   636,    -1,   400,   170,   432,    -1,   401,
     170,   432,    -1,   391,   170,   172,   432,    -1,    -1,   236,
     638,   639,   432,    -1,   637,    -1,   639,   637,    -1,   170,
     170,    -1,   170,   171,    -1,   170,   172,    -1,    -1,    -1,
      -1,    -1,    64,   641,   170,   642,   167,   172,   432,   643,
     645,    37,   644,   170,    -1,    -1,   645,   646,    -1,   134,
     172,   432,    -1,   271,   172,   432,    -1,   110,   124,   172,
     432,    -1,    14,    25,   172,   432,    -1,    35,   172,   432,
      -1,   383,   172,   432,    -1,   648,   652,   650,    -1,    -1,
     130,   649,   170,    -1,    -1,    37,   651,   170,    -1,    -1,
     652,   653,    -1,   131,   172,    13,   172,   432,    -1,   655,
      -1,   654,    -1,   658,    -1,    16,    96,   432,    -1,    16,
      22,   432,    -1,    16,   237,   432,    -1,   141,   656,   432,
      -1,    -1,   656,   657,    -1,   168,    -1,   169,    -1,   182,
      -1,    -1,   397,   659,   660,   432,    -1,    -1,   660,   661,
      -1,    -1,   170,   579,   662,    -1,   172,   172,    -1,   433,
     172,   172,   434,    -1,    -1,   666,   670,   665,   668,    -1,
      -1,    68,   667,   170,    -1,    -1,    37,   669,   170,    -1,
      -1,   670,   671,    -1,   678,    -1,   683,    -1,   684,    -1,
     685,    -1,   674,    -1,   689,    -1,   687,    -1,   686,    -1,
     688,    -1,   690,    -1,   692,    -1,   697,    -1,   694,    -1,
     698,    -1,   266,   267,   432,    -1,   266,   268,   432,    -1,
     747,    -1,   749,    -1,   756,    -1,   758,    -1,    -1,   236,
     672,   673,   432,    -1,   677,    -1,   673,   677,    -1,   141,
     675,   432,    -1,    -1,   675,   676,    -1,   168,    -1,   169,
      -1,   182,    -1,   170,   172,    -1,   170,   171,    -1,   170,
     170,    -1,    16,   679,   432,    -1,    24,    -1,    24,   349,
      -1,   112,    -1,    10,    -1,    10,   357,    -1,    10,   398,
      -1,   324,    -1,   349,    -1,    96,    -1,   237,    -1,    96,
     680,    -1,    22,    -1,    23,    -1,    22,   681,    -1,    38,
     682,    -1,    56,    -1,    89,    -1,    55,    -1,   106,    -1,
     133,    -1,   356,    -1,    46,    -1,   144,    -1,   145,    -1,
     133,    -1,   316,    -1,   402,    -1,   107,    -1,   105,    -1,
     151,    -1,   152,    -1,    11,    -1,    12,    -1,    51,   170,
     432,    -1,    50,   170,   170,   432,    -1,   132,   189,   432,
      -1,   132,    50,   432,    -1,   132,    10,   432,    -1,   106,
     172,   432,    -1,    88,   663,   432,    -1,   577,   432,    -1,
     577,   663,   432,    -1,   577,   663,   579,   432,    -1,   577,
     579,   432,    -1,   315,   432,    -1,    -1,    36,   691,   170,
     432,    -1,    -1,    66,   693,   170,   432,    -1,   695,   170,
     432,    -1,   695,   738,   432,    -1,   130,    -1,   130,    -1,
     131,   172,    13,   172,   432,    -1,   699,   703,   701,    -1,
      -1,   100,   700,   170,    -1,    -1,    37,   702,   170,    -1,
      -1,   703,   704,    -1,   577,   432,    -1,   577,   663,   432,
      -1,   577,   663,   579,   432,    -1,   577,   140,   432,    -1,
     577,   140,   663,   432,    -1,   577,   140,   663,   579,   432,
      -1,    -1,    66,   705,   170,   432,    -1,   106,   172,   432,
      -1,   718,    -1,   158,   721,   432,    -1,   126,   722,   432,
      -1,    65,   172,   432,    -1,   117,   172,   432,    -1,    45,
     172,   432,    -1,   116,   172,   432,    -1,    44,   172,   432,
      -1,   164,   172,   432,    -1,   161,   172,   432,    -1,   146,
     172,   432,    -1,   127,   723,   432,    -1,    -1,    76,   706,
     170,   432,    -1,    -1,    90,   707,   172,   172,   432,    -1,
      -1,   250,   708,   172,   172,   432,    -1,    -1,    57,   709,
     172,   172,   432,    -1,    14,   172,   432,    -1,    70,   172,
     432,    -1,    71,   172,   432,    -1,   110,   172,   432,    -1,
     108,   172,   432,    -1,    27,     6,   432,    -1,    27,   111,
     432,    -1,   120,   172,   432,    -1,   121,   172,   432,    -1,
      63,   170,   170,   432,    -1,   259,   170,   432,    -1,    -1,
     236,   710,   716,   432,    -1,   719,   720,   724,    37,    -1,
     719,    37,    -1,   261,   172,   847,   432,    -1,   263,   172,
     847,   432,    -1,   262,   172,   847,   432,    -1,   264,   172,
     432,    -1,   265,   172,   432,    -1,   325,   172,   847,   432,
      -1,   326,   172,   847,   432,    -1,   331,   172,   847,   432,
      -1,   328,   172,   847,   432,    -1,   327,   172,   847,   432,
      -1,   329,   172,   849,   432,    -1,   330,   172,   849,   432,
      -1,   332,   172,   849,   432,    -1,    -1,   348,   711,   715,
     432,    -1,    -1,   393,   712,   171,   432,    -1,    -1,   399,
     713,   170,   432,    -1,    -1,   385,   714,   170,   432,    -1,
     362,    -1,   363,    -1,   364,    -1,   365,    -1,   717,    -1,
     716,   717,    -1,   170,   172,    -1,   170,   171,    -1,   170,
     170,    -1,    33,    56,   432,    -1,    33,    89,   432,    -1,
      33,    89,   154,   432,    -1,    33,    55,   432,    -1,    33,
      46,   432,    -1,   104,    -1,    -1,    16,   679,   432,    -1,
     129,    -1,     7,    -1,   106,    -1,    52,    -1,    17,    -1,
      31,    -1,    56,    -1,    89,    -1,   137,    -1,   139,    -1,
      -1,     5,    -1,   112,    -1,    46,    -1,   725,   728,    -1,
      -1,    -1,    64,   726,   170,   727,   729,   730,   432,    -1,
     167,   172,   432,    -1,    97,   615,   731,   733,   432,    -1,
      97,   615,    62,   731,   733,   737,   432,    -1,   109,   615,
     663,   663,   432,    -1,   109,   615,    62,   663,   663,   737,
     432,    -1,   103,   615,   731,   732,   732,   733,   432,    -1,
     103,   615,    62,   731,   732,   732,   733,   737,   432,    -1,
     734,    -1,    -1,   728,   725,    -1,    -1,   413,    -1,    -1,
     134,   172,    -1,    20,   172,    -1,   663,    -1,   663,    -1,
      -1,   733,   732,    -1,    -1,   162,   615,   663,   735,   170,
     432,    -1,    -1,   162,    62,   615,   663,   736,   170,   737,
     432,    -1,    34,   172,    13,   172,   138,   172,   172,    -1,
     170,   172,   172,   579,    34,   172,    13,   172,   138,   172,
     172,    -1,   170,   172,   172,   579,    -1,    -1,    -1,   168,
     172,    34,   172,   138,   172,   740,    64,   741,   744,    -1,
      -1,    -1,   169,   172,    34,   172,   138,   172,   742,    64,
     743,   744,    -1,   168,   172,    34,   172,   138,   172,    -1,
     169,   172,    34,   172,   138,   172,    -1,    -1,   744,   745,
      -1,   170,    -1,   168,   172,    34,   172,   138,   172,    -1,
     169,   172,    34,   172,   138,   172,    -1,   748,   724,    37,
      -1,   748,    37,    -1,    84,    -1,   376,   751,   750,    37,
      -1,    -1,   750,   751,    -1,    -1,    -1,    64,   752,   170,
     432,   753,   755,   754,    -1,    -1,   754,   755,    -1,   109,
     663,   663,   172,   432,    -1,    -1,    18,   757,   170,   432,
      -1,   759,   761,   760,    -1,   148,    -1,    37,   148,    -1,
      -1,   761,   762,    -1,    -1,    49,   763,   777,   432,    -1,
      -1,   150,   764,   778,   432,    -1,    -1,   775,    59,   172,
     172,   765,   774,   159,   172,   172,   432,    -1,   775,   770,
     156,   776,   226,   172,   172,   172,   432,    -1,   227,   773,
     432,    -1,   228,   771,   432,    -1,   115,   172,   172,   432,
      -1,    43,   172,   172,   432,    -1,   114,   172,   172,   432,
      -1,    40,   172,   172,   432,    -1,   118,   172,   172,   432,
      -1,    42,   172,   172,   432,    -1,   119,   172,   172,   432,
      -1,    41,   172,   172,   432,    -1,   156,   776,   432,    -1,
     205,   206,   172,   207,   172,   775,   432,    -1,   767,   768,
     769,   226,   172,   172,   172,   432,    -1,   766,   226,   172,
     172,   172,   432,    -1,   284,   171,   432,    -1,   285,   171,
     432,    -1,   286,   171,   432,    -1,   230,   432,    -1,   287,
      -1,   288,    -1,   289,    -1,   206,    -1,   207,    -1,   279,
      -1,   280,    -1,   281,    -1,   282,    -1,   283,    -1,   281,
      -1,   282,    -1,   283,    -1,   225,    -1,   229,    -1,   772,
      -1,   771,   772,    -1,   433,   172,   172,   172,   434,    -1,
     172,    -1,   773,   172,    -1,    -1,   172,   172,   172,   172,
      -1,   172,   172,   172,   172,   172,   172,   172,    -1,   113,
      -1,    39,    -1,    60,    -1,    82,    -1,    83,    -1,   170,
      -1,   777,   170,    -1,   170,    -1,   778,   170,    -1,    -1,
     781,   785,   780,   783,    -1,    -1,     8,   782,   170,    -1,
      -1,    37,   784,   170,    -1,    -1,   785,   786,    -1,    -1,
     696,   787,   738,   432,    -1,    -1,   197,   788,   738,   432,
      -1,    -1,   198,   789,   738,   432,    -1,    -1,   199,   790,
     739,   432,    -1,   792,   793,    37,   170,    -1,    -1,   201,
     791,   746,   432,    -1,   202,   172,   797,    37,   202,    -1,
     803,    -1,   200,   170,    -1,    -1,   793,   794,    -1,    -1,
     197,   795,   738,   432,    -1,    -1,   198,   796,   738,   432,
      -1,    -1,   797,   798,    -1,   203,   172,   204,   172,   432,
      -1,    -1,   272,   800,   170,   435,   812,   807,    -1,    -1,
     273,   802,   170,   435,   812,   807,    -1,    -1,   208,   804,
     170,   435,   810,   807,    -1,    -1,   209,   805,   170,   435,
     812,   807,    -1,    -1,   210,   806,   170,   435,   811,   807,
      -1,    -1,   432,    -1,   436,    -1,   212,    -1,   436,   212,
      -1,   213,    -1,   436,   213,    -1,   810,   428,   810,    -1,
     810,   427,   810,    -1,   810,   429,   810,    -1,   810,   430,
     810,    -1,   427,   810,    -1,   433,   810,   434,    -1,   211,
     811,   808,   810,   809,   810,    -1,   172,    -1,   810,   813,
     810,    -1,   810,   223,   810,    -1,   810,   222,   810,    -1,
     812,   813,   812,    -1,   812,   223,   812,    -1,   812,   222,
     812,    -1,   811,   216,   811,    -1,   811,   217,   811,    -1,
     811,   223,   811,    -1,   811,   222,   811,    -1,   224,   811,
      -1,   433,   811,   434,    -1,   211,   811,   808,   811,   809,
     811,    -1,   215,    -1,   214,    -1,   812,   428,   812,    -1,
     433,   812,   434,    -1,   211,   811,   808,   812,   809,   812,
      -1,   171,    -1,   218,    -1,   219,    -1,   220,    -1,   221,
      -1,   216,    -1,   217,    -1,   435,    -1,   437,    -1,   438,
      -1,    -1,   231,   815,   816,    37,   231,    -1,    -1,   816,
     817,    -1,    -1,    67,   818,   170,   826,   432,    -1,    -1,
      58,   819,   170,   826,   432,    -1,    -1,   100,   820,   170,
     826,   432,    -1,    -1,    68,   821,   170,   826,   432,    -1,
      -1,   162,   822,   170,   826,   432,    -1,    -1,   163,   823,
     170,   826,   432,    -1,    -1,    64,   824,   170,   826,   432,
      -1,    -1,    81,   825,   170,   826,   432,    -1,    -1,   233,
     827,   834,   835,    -1,    -1,   234,   828,   834,   835,    -1,
     232,    -1,   232,   171,    -1,   269,   170,    -1,    -1,   298,
      -1,    21,   172,    -1,    21,   172,   235,   172,   172,    -1,
     235,   172,   172,    -1,    -1,    -1,   419,   172,   370,   172,
     831,   832,    -1,    -1,   423,    -1,    -1,   421,    -1,    -1,
     235,   172,   172,    -1,    -1,   172,    -1,    -1,   837,   836,
      -1,   373,    -1,    -1,   125,   838,   833,    -1,   420,    -1,
      -1,    -1,    -1,    64,   840,   170,   841,   845,    -1,    -1,
     347,   172,   370,   172,   842,   846,    -1,     9,   172,    -1,
      -1,   235,   172,   172,   843,   829,    -1,   299,   172,    -1,
     299,   172,   235,   172,   172,    -1,    -1,   409,   172,   370,
     172,   844,   830,    -1,   416,   172,    -1,   410,   172,   417,
     172,   416,   172,    -1,    -1,   136,    -1,    -1,   412,    -1,
      -1,    -1,    64,   848,   170,    -1,    -1,    64,   850,   170,
      -1,   242,   172,   172,   432,    -1,   243,   172,   432,    -1,
     246,   172,   432,    -1,   245,   172,   432,    -1,    -1,   248,
     172,   856,   432,   858,   857,   807,    -1,    37,   248,    -1,
     859,    -1,   858,   859,    -1,   249,   172,   432,    -1,   860,
      -1,    -1,   250,   861,   862,   432,   863,    -1,   172,    -1,
     862,   172,    -1,   864,    -1,   863,   864,    -1,    -1,   251,
     172,   432,   865,   247,   866,   432,    -1,   172,    -1,   866,
     172,    -1,    -1,   244,   172,   432,   868,   870,   869,   807,
      -1,    37,   244,    -1,   871,    -1,   870,   871,    -1,   249,
     172,   432,    -1,   872,    -1,    -1,   250,   873,   874,   432,
     875,    -1,   172,    -1,   874,   172,    -1,   876,    -1,   875,
     876,    -1,    -1,   251,   172,   432,   877,   252,   878,   432,
      -1,   172,    -1,   878,   172,    -1,   254,   172,   432,    -1,
     253,   172,   432,    -1,   255,   172,   432,    -1,   300,   172,
     432,    -1,   301,   172,   432,    -1,   302,   172,   432,    -1,
      -1,   886,    -1,   240,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   531,   531,   589,   589,   625,   638,   651,   652,   653,
     657,   663,   692,   692,   692,   692,   693,   693,   693,   693,
     693,   694,   694,   695,   695,   695,   695,   695,   695,   695,
     696,   696,   696,   697,   697,   698,   698,   699,   699,   699,
     700,   700,   701,   701,   701,   701,   701,   702,   702,   702,
     703,   706,   719,   737,   747,   758,   771,   778,   793,   797,
     798,   801,   802,   805,   806,   808,   814,   837,   838,   841,
     843,   845,   847,   849,   851,   853,   860,   863,   870,   870,
     902,   902,   961,   962,   970,   976,   969,   995,  1001,  1016,
    1021,  1026,  1030,  1034,  1038,  1042,  1046,  1050,  1055,  1060,
    1059,  1096,  1095,  1113,  1127,  1139,  1150,  1162,  1173,  1185,
    1197,  1209,  1221,  1233,  1258,  1285,  1304,  1322,  1341,  1341,
    1348,  1347,  1360,  1375,  1391,  1414,  1390,  1417,  1440,  1416,
    1444,  1483,  1482,  1521,  1560,  1559,  1599,  1598,  1613,  1652,
    1651,  1690,  1729,  1728,  1768,  1767,  1808,  1807,  1846,  1867,
    1888,  1910,  1927,  1909,  1941,  1965,  1989,  2013,  2037,  2061,
    2085,  2109,  2132,  2150,  2168,  2186,  2204,  2222,  2247,  2273,
    2272,  2290,  2289,  2305,  2304,  2311,  2328,  2327,  2354,  2353,
    2373,  2372,  2390,  2408,  2434,  2433,  2452,  2454,  2460,  2462,
    2468,  2470,  2473,  2490,  2511,  2516,  2522,  2489,  2536,  2540,
    2535,  2572,  2571,  2596,  2598,  2601,  2608,  2609,  2610,  2612,
    2614,  2613,  2620,  2636,  2637,  2653,  2654,  2661,  2662,  2678,
    2679,  2698,  2717,  2718,  2736,  2737,  2743,  2747,  2751,  2765,
    2766,  2767,  2770,  2774,  2773,  2786,  2787,  2794,  2799,  2800,
    2834,  2835,  2836,  2840,  2842,  2845,  2839,  2849,  2851,  2850,
    2865,  2864,  2881,  2880,  2885,  2886,  2890,  2891,  2895,  2903,
    2911,  2923,  2925,  2928,  2932,  2933,  2936,  2940,  2941,  2944,
    2948,  2949,  2950,  2951,  2952,  2953,  2956,  2957,  2958,  2959,
    2961,  2962,  2969,  2974,  2979,  2984,  2991,  2992,  2999,  2998,
    3008,  3009,  3024,  3023,  3034,  3035,  3039,  3040,  3043,  3046,
    3082,  3082,  3109,  3109,  3115,  3118,  3127,  3135,  3144,  3146,
    3149,  3144,  3170,  3171,  3174,  3178,  3182,  3186,  3186,  3192,
    3193,  3195,  3198,  3199,  3203,  3205,  3207,  3209,  3209,  3214,
    3225,  3226,  3230,  3240,  3248,  3258,  3267,  3276,  3285,  3295,
    3295,  3299,  3300,  3301,  3302,  3303,  3304,  3305,  3306,  3307,
    3308,  3309,  3310,  3311,  3312,  3313,  3314,  3316,  3319,  3319,
    3326,  3328,  3332,  3346,  3345,  3369,  3369,  3403,  3403,  3413,
    3440,  3439,  3475,  3476,  3494,  3495,  3498,  3500,  3504,  3505,
    3508,  3508,  3515,  3516,  3520,  3528,  3536,  3547,  3578,  3580,
    3583,  3586,  3586,  3592,  3594,  3598,  3617,  3636,  3672,  3674,
    3677,  3679,  3681,  3711,  3739,  3739,  3759,  3762,  3784,  3797,
    3799,  3802,  3813,  3825,  3831,  3832,  3835,  3838,  3849,  3858,
    3860,  3863,  3871,  3873,  3876,  3879,  3882,  3896,  3907,  3908,
    3918,  3907,  3948,  3953,  3974,  3976,  3995,  3996,  4000,  4001,
    4002,  4003,  4004,  4005,  4006,  4009,  4026,  4045,  4064,  4064,
    4071,  4072,  4076,  4084,  4092,  4103,  4104,  4113,  4117,  4103,
    4159,  4161,  4165,  4170,  4173,  4197,  4220,  4243,  4262,  4268,
    4268,  4278,  4278,  4309,  4311,  4315,  4322,  4324,  4329,  4333,
    4334,  4335,  4337,  4340,  4342,  4346,  4348,  4350,  4353,  4353,
    4357,  4359,  4362,  4362,  4366,  4368,  4372,  4371,  4379,  4379,
    4397,  4397,  4419,  4421,  4428,  4429,  4430,  4431,  4432,  4433,
    4435,  4437,  4439,  4441,  4442,  4443,  4445,  4447,  4449,  4451,
    4453,  4455,  4457,  4459,  4461,  4461,  4468,  4469,  4472,  4483,
    4485,  4489,  4491,  4493,  4497,  4507,  4515,  4524,  4532,  4533,
    4552,  4553,  4554,  4573,  4592,  4593,  4605,  4606,  4607,  4630,
    4631,  4638,  4641,  4647,  4648,  4649,  4650,  4651,  4652,  4655,
    4656,  4657,  4658,  4677,  4696,  4717,  4718,  4719,  4720,  4721,
    4722,  4724,  4727,  4731,  4740,  4749,  4759,  4769,  4819,  4823,
    4827,  4831,  4837,  4847,  4847,  4850,  4850,  4861,  4873,  4888,
    4892,  4895,  4916,  4923,  4923,  4929,  4929,  4951,  4952,  4956,
    4965,  4974,  4983,  4992,  5001,  5010,  5010,  5019,  5028,  5030,
    5032,  5034,  5043,  5052,  5061,  5070,  5079,  5088,  5097,  5106,
    5108,  5108,  5110,  5110,  5119,  5119,  5128,  5128,  5137,  5146,
    5148,  5150,  5159,  5168,  5177,  5186,  5195,  5204,  5213,  5215,
    5215,  5220,  5234,  5249,  5270,  5291,  5312,  5314,  5316,  5346,
    5376,  5406,  5436,  5466,  5496,  5526,  5557,  5556,  5586,  5586,
    5602,  5602,  5618,  5618,  5636,  5641,  5646,  5651,  5658,  5659,
    5663,  5673,  5681,  5691,  5692,  5693,  5694,  5695,  5697,  5709,
    5710,  5715,  5716,  5717,  5718,  5719,  5720,  5723,  5724,  5725,
    5726,  5729,  5730,  5731,  5732,  5734,  5737,  5738,  5737,  5754,
    5766,  5787,  5808,  5828,  5848,  5870,  5891,  5894,  5895,  5897,
    5898,  5913,  5914,  5928,  5943,  5947,  5951,  5956,  5960,  5960,
    5973,  5973,  5988,  5992,  6003,  6016,  6024,  6015,  6027,  6035,
    6026,  6037,  6046,  6056,  6058,  6061,  6064,  6073,  6083,  6094,
    6113,  6125,  6146,  6147,  6150,  6151,  6150,  6157,  6158,  6161,
    6167,  6167,  6170,  6173,  6176,  6190,  6192,  6197,  6196,  6207,
    6207,  6210,  6209,  6213,  6222,  6224,  6226,  6228,  6230,  6232,
    6234,  6236,  6238,  6240,  6242,  6244,  6246,  6248,  6250,  6252,
    6254,  6256,  6260,  6262,  6264,  6268,  6270,  6272,  6274,  6278,
    6280,  6282,  6286,  6288,  6290,  6294,  6296,  6300,  6302,  6305,
    6309,  6311,  6316,  6317,  6319,  6324,  6326,  6330,  6332,  6334,
    6338,  6340,  6344,  6346,  6350,  6349,  6359,  6359,  6369,  6369,
    6393,  6394,  6398,  6398,  6405,  6405,  6412,  6412,  6419,  6419,
    6425,  6428,  6428,  6434,  6440,  6443,  6448,  6449,  6453,  6453,
    6460,  6460,  6470,  6471,  6474,  6478,  6478,  6482,  6482,  6486,
    6486,  6495,  6495,  6504,  6504,  6515,  6516,  6517,  6520,  6521,
    6525,  6526,  6530,  6531,  6532,  6533,  6534,  6535,  6536,  6538,
    6541,  6542,  6543,  6544,  6545,  6546,  6547,  6548,  6549,  6550,
    6551,  6552,  6553,  6555,  6556,  6559,  6565,  6567,  6576,  6580,
    6581,  6582,  6583,  6584,  6585,  6586,  6587,  6588,  6592,  6591,
    6607,  6608,  6612,  6612,  6621,  6621,  6630,  6630,  6640,  6640,
    6649,  6649,  6658,  6658,  6667,  6667,  6676,  6676,  6687,  6687,
    6692,  6692,  6698,  6703,  6708,  6716,  6717,  6722,  6729,  6736,
    6744,  6746,  6745,  6754,  6755,  6763,  6764,  6772,  6773,  6778,
    6779,  6782,  6784,  6786,  6811,  6810,  6835,  6859,  6861,  6862,
    6861,  6878,  6877,  6902,  6926,  6925,  6931,  6937,  6945,  6944,
    6960,  6974,  6991,  6992,  7000,  7001,  7018,  7019,  7019,  7024,
    7024,  7028,  7042,  7055,  7068,  7082,  7081,  7087,  7100,  7101,
    7105,  7112,  7116,  7115,  7121,  7124,  7129,  7130,  7134,  7133,
    7140,  7143,  7148,  7147,  7154,  7167,  7168,  7172,  7179,  7183,
    7182,  7189,  7192,  7197,  7198,  7203,  7202,  7209,  7212,  7218,
    7242,  7266,  7290,  7324,  7358,  7392,  7393,  7395
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "K_HISTORY", "K_ABUT", "K_ABUTMENT",
  "K_ACTIVE", "K_ANALOG", "K_ARRAY", "K_AREA", "K_BLOCK", "K_BOTTOMLEFT",
  "K_BOTTOMRIGHT", "K_BY", "K_CAPACITANCE", "K_CAPMULTIPLIER", "K_CLASS",
  "K_CLOCK", "K_CLOCKTYPE", "K_COLUMNMAJOR", "K_DESIGNRULEWIDTH",
  "K_INFLUENCE", "K_CORE", "K_CORNER", "K_COVER", "K_CPERSQDIST",
  "K_CURRENT", "K_CURRENTSOURCE", "K_CUT", "K_DEFAULT", "K_DATABASE",
  "K_DATA", "K_DIELECTRIC", "K_DIRECTION", "K_DO", "K_EDGECAPACITANCE",
  "K_EEQ", "K_END", "K_ENDCAP", "K_FALL", "K_FALLCS", "K_FALLT0",
  "K_FALLSATT1", "K_FALLRS", "K_FALLSATCUR", "K_FALLTHRESH", "K_FEEDTHRU",
  "K_FIXED", "K_FOREIGN", "K_FROMPIN", "K_GENERATE", "K_GENERATOR",
  "K_GROUND", "K_HEIGHT", "K_HORIZONTAL", "K_INOUT", "K_INPUT",
  "K_INPUTNOISEMARGIN", "K_COMPONENTPIN", "K_INTRINSIC", "K_INVERT",
  "K_IRDROP", "K_ITERATE", "K_IV_TABLES", "K_LAYER", "K_LEAKAGE", "K_LEQ",
  "K_LIBRARY", "K_MACRO", "K_MATCH", "K_MAXDELAY", "K_MAXLOAD",
  "K_METALOVERHANG", "K_MILLIAMPS", "K_MILLIWATTS", "K_MINFEATURE",
  "K_MUSTJOIN", "K_NAMESCASESENSITIVE", "K_NANOSECONDS", "K_NETS", "K_NEW",
  "K_NONDEFAULTRULE", "K_NONINVERT", "K_NONUNATE", "K_OBS", "K_OHMS",
  "K_OFFSET", "K_ORIENTATION", "K_ORIGIN", "K_OUTPUT",
  "K_OUTPUTNOISEMARGIN", "K_OVERHANG", "K_OVERLAP", "K_OFF", "K_ON",
  "K_OVERLAPS", "K_PAD", "K_PATH", "K_PATTERN", "K_PICOFARADS", "K_PIN",
  "K_PITCH", "K_PLACED", "K_POLYGON", "K_PORT", "K_POST", "K_POWER",
  "K_PRE", "K_PULLDOWNRES", "K_RECT", "K_RESISTANCE", "K_RESISTIVE",
  "K_RING", "K_RISE", "K_RISECS", "K_RISERS", "K_RISESATCUR",
  "K_RISETHRESH", "K_RISESATT1", "K_RISET0", "K_RISEVOLTAGETHRESHOLD",
  "K_FALLVOLTAGETHRESHOLD", "K_ROUTING", "K_ROWMAJOR", "K_RPERSQ",
  "K_SAMENET", "K_SCANUSE", "K_SHAPE", "K_SHRINKAGE", "K_SIGNAL", "K_SITE",
  "K_SIZE", "K_SOURCE", "K_SPACER", "K_SPACING", "K_SPECIALNETS",
  "K_STACK", "K_START", "K_STEP", "K_STOP", "K_STRUCTURE", "K_SYMMETRY",
  "K_TABLE", "K_THICKNESS", "K_TIEHIGH", "K_TIELOW", "K_TIEOFFR", "K_TIME",
  "K_TIMING", "K_TO", "K_TOPIN", "K_TOPLEFT", "K_TOPRIGHT",
  "K_TOPOFSTACKONLY", "K_TRISTATE", "K_TYPE", "K_UNATENESS", "K_UNITS",
  "K_USE", "K_VARIABLE", "K_VERTICAL", "K_VHI", "K_VIA", "K_VIARULE",
  "K_VLO", "K_VOLTAGE", "K_VOLTS", "K_WIDTH", "K_X", "K_Y", "T_STRING",
  "QSTRING", "NUMBER", "K_N", "K_S", "K_E", "K_W", "K_FN", "K_FS", "K_FE",
  "K_FW", "K_R0", "K_R90", "K_R180", "K_R270", "K_MX", "K_MY", "K_MXR90",
  "K_MYR90", "K_USER", "K_MASTERSLICE", "K_ENDMACRO", "K_ENDMACROPIN",
  "K_ENDVIARULE", "K_ENDVIA", "K_ENDLAYER", "K_ENDSITE", "K_CANPLACE",
  "K_CANNOTOCCUPY", "K_TRACKS", "K_FLOORPLAN", "K_GCELLGRID",
  "K_DEFAULTCAP", "K_MINPINS", "K_WIRECAP", "K_STABLE", "K_SETUP",
  "K_HOLD", "K_DEFINE", "K_DEFINES", "K_DEFINEB", "K_IF", "K_THEN",
  "K_ELSE", "K_FALSE", "K_TRUE", "K_EQ", "K_NE", "K_LE", "K_LT", "K_GE",
  "K_GT", "K_OR", "K_AND", "K_NOT", "K_DELAY", "K_TABLEDIMENSION",
  "K_TABLEAXIS", "K_TABLEENTRIES", "K_TRANSITIONTIME", "K_EXTENSION",
  "K_PROPDEF", "K_STRING", "K_INTEGER", "K_REAL", "K_RANGE", "K_PROPERTY",
  "K_VIRTUAL", "K_BUSBITCHARS", "K_VERSION", "K_BEGINEXT", "K_ENDEXT",
  "K_UNIVERSALNOISEMARGIN", "K_EDGERATETHRESHOLD1", "K_CORRECTIONTABLE",
  "K_EDGERATESCALEFACTOR", "K_EDGERATETHRESHOLD2", "K_VICTIMNOISE",
  "K_NOISETABLE", "K_EDGERATE", "K_OUTPUTRESISTANCE", "K_VICTIMLENGTH",
  "K_CORRECTIONFACTOR", "K_OUTPUTPINANTENNASIZE", "K_INPUTPINANTENNASIZE",
  "K_INOUTPINANTENNASIZE", "K_CURRENTDEN", "K_PWL",
  "K_ANTENNALENGTHFACTOR", "K_TAPERRULE", "K_DIVIDERCHAR", "K_ANTENNASIZE",
  "K_ANTENNAMETALLENGTH", "K_ANTENNAMETALAREA", "K_RISESLEWLIMIT",
  "K_FALLSLEWLIMIT", "K_FUNCTION", "K_BUFFER", "K_INVERTER",
  "K_NAMEMAPSTRING", "K_NOWIREEXTENSIONATPIN", "K_WIREEXTENSION",
  "K_MESSAGE", "K_CREATEFILE", "K_OPENFILE", "K_CLOSEFILE", "K_WARNING",
  "K_ERROR", "K_FATALERROR", "K_RECOVERY", "K_SKEW", "K_ANYEDGE",
  "K_POSEDGE", "K_NEGEDGE", "K_SDFCONDSTART", "K_SDFCONDEND", "K_SDFCOND",
  "K_MPWH", "K_MPWL", "K_PERIOD", "K_ACCURRENTDENSITY",
  "K_DCCURRENTDENSITY", "K_AVERAGE", "K_PEAK", "K_RMS", "K_FREQUENCY",
  "K_CUTAREA", "K_MEGAHERTZ", "K_USELENGTHTHRESHOLD", "K_LENGTHTHRESHOLD",
  "K_ANTENNAINPUTGATEAREA", "K_ANTENNAINOUTDIFFAREA",
  "K_ANTENNAOUTPUTDIFFAREA", "K_ANTENNAAREARATIO",
  "K_ANTENNADIFFAREARATIO", "K_ANTENNACUMAREARATIO",
  "K_ANTENNACUMDIFFAREARATIO", "K_ANTENNAAREAFACTOR",
  "K_ANTENNASIDEAREARATIO", "K_ANTENNADIFFSIDEAREARATIO",
  "K_ANTENNACUMSIDEAREARATIO", "K_ANTENNACUMDIFFSIDEAREARATIO",
  "K_ANTENNASIDEAREAFACTOR", "K_DIFFUSEONLY", "K_MANUFACTURINGGRID",
  "K_FIXEDMASK", "K_ANTENNACELL", "K_CLEARANCEMEASURE", "K_EUCLIDEAN",
  "K_MAXXY", "K_USEMINSPACING", "K_ROWMINSPACING", "K_ROWABUTSPACING",
  "K_FLIP", "K_NONE", "K_ANTENNAPARTIALMETALAREA",
  "K_ANTENNAPARTIALMETALSIDEAREA", "K_ANTENNAGATEAREA",
  "K_ANTENNADIFFAREA", "K_ANTENNAMAXAREACAR", "K_ANTENNAMAXSIDEAREACAR",
  "K_ANTENNAPARTIALCUTAREA", "K_ANTENNAMAXCUTCAR", "K_SLOTWIREWIDTH",
  "K_SLOTWIRELENGTH", "K_SLOTWIDTH", "K_SLOTLENGTH",
  "K_MAXADJACENTSLOTSPACING", "K_MAXCOAXIALSLOTSPACING",
  "K_MAXEDGESLOTSPACING", "K_SPLITWIREWIDTH", "K_MINIMUMDENSITY",
  "K_MAXIMUMDENSITY", "K_DENSITYCHECKWINDOW", "K_DENSITYCHECKSTEP",
  "K_FILLACTIVESPACING", "K_MINIMUMCUT", "K_ADJACENTCUTS",
  "K_ANTENNAMODEL", "K_BUMP", "K_ENCLOSURE", "K_FROMABOVE", "K_FROMBELOW",
  "K_IMPLANT", "K_LENGTH", "K_MAXVIASTACK", "K_AREAIO", "K_BLACKBOX",
  "K_MAXWIDTH", "K_MINENCLOSEDAREA", "K_MINSTEP", "K_ORIENT", "K_OXIDE1",
  "K_OXIDE2", "K_OXIDE3", "K_OXIDE4", "K_PARALLELRUNLENGTH", "K_MINWIDTH",
  "K_PROTRUSIONWIDTH", "K_SPACINGTABLE", "K_WITHIN", "K_ABOVE", "K_BELOW",
  "K_CENTERTOCENTER", "K_CUTSIZE", "K_CUTSPACING", "K_DENSITY", "K_DIAG45",
  "K_DIAG135", "K_MASK", "K_DIAGMINEDGELENGTH", "K_DIAGSPACING",
  "K_DIAGPITCH", "K_DIAGWIDTH", "K_GENERATED", "K_GROUNDSENSITIVITY",
  "K_HARDSPACING", "K_INSIDECORNER", "K_LAYERS", "K_LENGTHSUM",
  "K_MICRONS", "K_MINCUTS", "K_MINSIZE", "K_NETEXPR", "K_OUTSIDECORNER",
  "K_PREFERENCLOSURE", "K_ROWCOL", "K_ROWPATTERN", "K_SOFT",
  "K_SUPPLYSENSITIVITY", "K_USEVIA", "K_USEVIARULE", "K_WELLTAP",
  "K_ARRAYCUTS", "K_ARRAYSPACING", "K_ANTENNAAREADIFFREDUCEPWL",
  "K_ANTENNAAREAMINUSDIFF", "K_ANTENNACUMROUTINGPLUSCUT",
  "K_ANTENNAGATEPLUSDIFF", "K_ENDOFLINE", "K_ENDOFNOTCHWIDTH",
  "K_EXCEPTEXTRACUT", "K_EXCEPTSAMEPGNET", "K_EXCEPTPGNET", "K_LONGARRAY",
  "K_MAXEDGES", "K_NOTCHLENGTH", "K_NOTCHSPACING", "K_ORTHOGONAL",
  "K_PARALLELEDGE", "K_PARALLELOVERLAP", "K_PGONLY", "K_PRL", "K_TWOEDGES",
  "K_TWOWIDTHS", "IF", "LNOT", "'-'", "'+'", "'*'", "'/'", "UMINUS", "';'",
  "'('", "')'", "'='", "'\\n'", "'<'", "'>'", "$accept", "lef_file",
  "version", "$@1", "dividerchar", "busbitchars", "rules", "end_library",
  "rule", "case_sensitivity", "wireextension", "fixedmask",
  "manufacturing", "useminspacing", "clearancemeasure", "clearance_type",
  "spacing_type", "spacing_value", "units_section", "start_units",
  "units_rules", "units_rule", "layer_rule", "start_layer", "$@2",
  "end_layer", "$@3", "layer_options", "layer_option", "$@4", "$@5", "$@6",
  "$@7", "$@8", "$@9", "$@10", "$@11", "$@12", "$@13", "$@14", "$@15",
  "$@16", "$@17", "$@18", "$@19", "$@20", "$@21", "$@22", "$@23", "$@24",
  "$@25", "$@26", "$@27", "$@28", "$@29", "layer_arraySpacing_long",
  "layer_arraySpacing_width", "layer_arraySpacing_arraycuts",
  "layer_arraySpacing_arraycut", "sp_options", "$@30", "$@31", "$@32",
  "$@33", "$@34", "$@35", "$@36", "layer_spacingtable_opts",
  "layer_spacingtable_opt", "layer_enclosure_type_opt",
  "layer_enclosure_width_opt", "$@37", "layer_enclosure_width_except_opt",
  "layer_preferenclosure_width_opt", "layer_minimumcut_within",
  "layer_minimumcut_from", "layer_minimumcut_length",
  "layer_minstep_options", "layer_minstep_option", "layer_minstep_type",
  "layer_antenna_pwl", "$@38", "layer_diffusion_ratios",
  "layer_diffusion_ratio", "layer_antenna_duo", "layer_table_type",
  "layer_frequency", "$@39", "$@40", "$@41", "ac_layer_table_opt", "$@42",
  "$@43", "dc_layer_table", "$@44", "number_list", "layer_prop_list",
  "layer_prop", "current_density_pwl_list", "current_density_pwl",
  "cap_points", "cap_point", "res_points", "res_point", "layer_type",
  "layer_direction", "layer_minen_width", "layer_oxide",
  "layer_sp_parallel_widths", "layer_sp_parallel_width", "$@45",
  "layer_sp_TwoWidths", "layer_sp_TwoWidth", "$@46",
  "layer_sp_TwoWidthsPRL", "layer_sp_influence_widths",
  "layer_sp_influence_width", "maxstack_via", "$@47", "via", "$@48",
  "via_keyword", "start_via", "via_viarule", "$@49", "$@50", "$@51",
  "via_viarule_options", "via_viarule_option", "$@52", "via_option",
  "via_other_options", "via_more_options", "via_other_option", "$@53",
  "via_prop_list", "via_name_value_pair", "via_foreign", "start_foreign",
  "$@54", "orientation", "via_layer_rule", "via_layer", "$@55",
  "via_geometries", "via_geometry", "$@56", "end_via", "$@57",
  "viarule_keyword", "$@58", "viarule", "viarule_generate", "$@59",
  "viarule_generate_default", "viarule_layer_list", "opt_viarule_props",
  "viarule_props", "viarule_prop", "$@60", "viarule_prop_list",
  "viarule_layer", "via_names", "via_name", "viarule_layer_name", "$@61",
  "viarule_layer_options", "viarule_layer_option", "end_viarule", "$@62",
  "spacing_rule", "start_spacing", "end_spacing", "spacings", "spacing",
  "samenet_keyword", "maskColor", "irdrop", "start_irdrop", "end_irdrop",
  "ir_tables", "ir_table", "ir_table_values", "ir_table_value",
  "ir_tablename", "minfeature", "dielectric", "nondefault_rule", "$@63",
  "$@64", "$@65", "end_nd_rule", "nd_hardspacing", "nd_rules", "nd_rule",
  "usevia", "useviarule", "mincuts", "nd_prop", "$@66", "nd_prop_list",
  "nd_layer", "$@67", "$@68", "$@69", "$@70", "nd_layer_stmts",
  "nd_layer_stmt", "site", "start_site", "$@71", "end_site", "$@72",
  "site_options", "site_option", "site_class", "site_symmetry_statement",
  "site_symmetries", "site_symmetry", "site_rowpattern_statement", "$@73",
  "site_rowpatterns", "site_rowpattern", "$@74", "pt", "macro", "$@75",
  "start_macro", "$@76", "end_macro", "$@77", "macro_options",
  "macro_option", "$@78", "macro_prop_list", "macro_symmetry_statement",
  "macro_symmetries", "macro_symmetry", "macro_name_value_pair",
  "macro_class", "class_type", "pad_type", "core_type", "endcap_type",
  "macro_generator", "macro_generate", "macro_source", "macro_power",
  "macro_origin", "macro_foreign", "macro_fixedMask", "macro_eeq", "$@79",
  "macro_leq", "$@80", "macro_site", "macro_site_word", "site_word",
  "macro_size", "macro_pin", "start_macro_pin", "$@81", "end_macro_pin",
  "$@82", "macro_pin_options", "macro_pin_option", "$@83", "$@84", "$@85",
  "$@86", "$@87", "$@88", "$@89", "$@90", "$@91", "$@92",
  "pin_layer_oxide", "pin_prop_list", "pin_name_value_pair",
  "electrical_direction", "start_macro_port", "macro_port_class_option",
  "macro_pin_use", "macro_scan_use", "pin_shape", "geometries", "geometry",
  "$@93", "$@94", "geometry_options", "layer_exceptpgnet", "layer_spacing",
  "firstPt", "nextPt", "otherPts", "via_placement", "$@95", "$@96",
  "stepPattern", "sitePattern", "trackPattern", "$@97", "$@98", "$@99",
  "$@100", "trackLayers", "layer_name", "gcellPattern", "macro_obs",
  "start_macro_obs", "macro_density", "density_layers", "density_layer",
  "$@101", "$@102", "density_layer_rects", "density_layer_rect",
  "macro_clocktype", "$@103", "timing", "start_timing", "end_timing",
  "timing_options", "timing_option", "$@104", "$@105", "$@106",
  "one_pin_trigger", "two_pin_trigger", "from_pin_trigger",
  "to_pin_trigger", "delay_or_transition", "list_of_table_entries",
  "table_entry", "list_of_table_axis_numbers", "slew_spec", "risefall",
  "unateness", "list_of_from_strings", "list_of_to_strings", "array",
  "$@107", "start_array", "$@108", "end_array", "$@109", "array_rules",
  "array_rule", "$@110", "$@111", "$@112", "$@113", "$@114",
  "floorplan_start", "floorplan_list", "floorplan_element", "$@115",
  "$@116", "cap_list", "one_cap", "msg_statement", "$@117",
  "create_file_statement", "$@118", "def_statement", "$@119", "$@120",
  "$@121", "dtrm", "then", "else", "expression", "b_expr", "s_expr",
  "relop", "prop_def_section", "$@122", "prop_stmts", "prop_stmt", "$@123",
  "$@124", "$@125", "$@126", "$@127", "$@128", "$@129", "$@130",
  "prop_define", "$@131", "$@132", "opt_range_second", "opt_endofline",
  "$@133", "opt_endofline_twoedges", "opt_samenetPGonly", "opt_def_range",
  "opt_def_value", "layer_spacing_opts", "layer_spacing_opt", "$@134",
  "layer_spacing_cut_routing", "$@135", "$@136", "$@137", "$@138", "$@139",
  "spacing_cut_layer_opt", "opt_adjacentcuts_exceptsame", "opt_layer_name",
  "$@140", "req_layer_name", "$@141", "universalnoisemargin",
  "edgeratethreshold1", "edgeratethreshold2", "edgeratescalefactor",
  "noisetable", "$@142", "end_noisetable", "noise_table_list",
  "noise_table_entry", "output_resistance_entry", "$@143", "num_list",
  "victim_list", "victim", "$@144", "vnoiselist", "correctiontable",
  "$@145", "end_correctiontable", "correction_table_list",
  "correction_table_item", "output_list", "$@146", "numo_list",
  "corr_victim_list", "corr_victim", "$@147", "corr_list", "input_antenna",
  "output_antenna", "inout_antenna", "antenna_input", "antenna_inout",
  "antenna_output", "extension_opt", "extension", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     615,   616,   617,   618,   619,   620,   621,   622,   623,   624,
     625,   626,   627,   628,   629,   630,   631,   632,   633,   634,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,   648,   649,   650,   651,   652,   653,   654,
     655,   656,   657,   658,   659,   660,   661,   662,   663,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,   677,   678,   679,   680,   681,    45,    43,    42,
      47,   682,    59,    40,    41,    61,    10,    60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   439,   440,   442,   441,   443,   444,   445,   445,   445,
     446,   446,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   448,   448,   449,   449,   450,   451,   452,   453,   454,
     454,   455,   455,   456,   456,   457,   458,   459,   459,   460,
     460,   460,   460,   460,   460,   460,   460,   461,   463,   462,
     465,   464,   466,   466,   468,   469,   467,   467,   467,   467,
     467,   467,   467,   467,   467,   467,   467,   467,   467,   470,
     467,   471,   467,   467,   467,   467,   467,   467,   467,   467,
     467,   467,   467,   467,   467,   467,   467,   467,   472,   467,
     473,   467,   467,   467,   474,   475,   467,   476,   477,   467,
     467,   478,   467,   467,   479,   467,   480,   467,   467,   481,
     467,   467,   482,   467,   483,   467,   484,   467,   467,   467,
     467,   485,   486,   467,   467,   467,   467,   467,   467,   467,
     467,   467,   467,   467,   467,   467,   467,   467,   467,   487,
     467,   488,   467,   489,   467,   467,   490,   467,   491,   467,
     492,   467,   467,   467,   493,   467,   494,   494,   495,   495,
     496,   496,   497,   499,   500,   501,   502,   498,   503,   504,
     498,   505,   498,   506,   506,   507,   508,   508,   508,   509,
     510,   509,   509,   511,   511,   512,   512,   513,   513,   514,
     514,   514,   515,   515,   516,   516,   517,   517,   517,   518,
     518,   518,   519,   520,   519,   521,   521,   522,   523,   523,
     524,   524,   524,   526,   527,   528,   525,   529,   530,   529,
     531,   529,   533,   532,   534,   534,   535,   535,   536,   536,
     536,   537,   537,   538,   539,   539,   540,   541,   541,   542,
     543,   543,   543,   543,   543,   543,   544,   544,   544,   544,
     545,   545,   546,   546,   546,   546,   547,   547,   549,   548,
     550,   550,   552,   551,   553,   553,   554,   554,   555,   556,
     557,   556,   559,   558,   560,   561,   561,   561,   563,   564,
     565,   562,   566,   566,   567,   567,   567,   568,   567,   569,
     569,   570,   571,   571,   572,   572,   572,   573,   572,   572,
     574,   574,   575,   575,   575,   576,   576,   576,   576,   578,
     577,   579,   579,   579,   579,   579,   579,   579,   579,   579,
     579,   579,   579,   579,   579,   579,   579,   580,   582,   581,
     583,   583,   584,   585,   584,   587,   586,   589,   588,   590,
     592,   591,   593,   593,   594,   594,   595,   595,   596,   596,
     598,   597,   599,   599,   597,   597,   597,   600,   601,   601,
     602,   604,   603,   605,   605,   606,   606,   606,   606,   606,
     606,   606,   606,   606,   608,   607,   609,   610,   611,   612,
     612,   613,   613,   614,   615,   615,   616,   617,   618,   619,
     619,   620,   621,   621,   622,   623,   624,   625,   627,   628,
     629,   626,   630,   630,   631,   631,   632,   632,   633,   633,
     633,   633,   633,   633,   633,   634,   635,   636,   638,   637,
     639,   639,   637,   637,   637,   641,   642,   643,   644,   640,
     645,   645,   646,   646,   646,   646,   646,   646,   647,   649,
     648,   651,   650,   652,   652,   653,   653,   653,   653,   654,
     654,   654,   655,   656,   656,   657,   657,   657,   659,   658,
     660,   660,   662,   661,   663,   663,   665,   664,   667,   666,
     669,   668,   670,   670,   671,   671,   671,   671,   671,   671,
     671,   671,   671,   671,   671,   671,   671,   671,   671,   671,
     671,   671,   671,   671,   672,   671,   673,   673,   674,   675,
     675,   676,   676,   676,   677,   677,   677,   678,   679,   679,
     679,   679,   679,   679,   679,   679,   679,   679,   679,   679,
     679,   679,   679,   680,   680,   680,   680,   680,   680,   681,
     681,   681,   681,   681,   681,   682,   682,   682,   682,   682,
     682,   683,   684,   685,   685,   685,   686,   687,   688,   688,
     688,   688,   689,   691,   690,   693,   692,   694,   694,   695,
     696,   697,   698,   700,   699,   702,   701,   703,   703,   704,
     704,   704,   704,   704,   704,   705,   704,   704,   704,   704,
     704,   704,   704,   704,   704,   704,   704,   704,   704,   704,
     706,   704,   707,   704,   708,   704,   709,   704,   704,   704,
     704,   704,   704,   704,   704,   704,   704,   704,   704,   710,
     704,   704,   704,   704,   704,   704,   704,   704,   704,   704,
     704,   704,   704,   704,   704,   704,   711,   704,   712,   704,
     713,   704,   714,   704,   715,   715,   715,   715,   716,   716,
     717,   717,   717,   718,   718,   718,   718,   718,   719,   720,
     720,   721,   721,   721,   721,   721,   721,   722,   722,   722,
     722,   723,   723,   723,   723,   724,   726,   727,   725,   725,
     725,   725,   725,   725,   725,   725,   725,   728,   728,   729,
     729,   730,   730,   730,   731,   732,   733,   733,   735,   734,
     736,   734,   737,   738,   738,   740,   741,   739,   742,   743,
     739,   739,   739,   744,   744,   745,   746,   746,   747,   747,
     748,   749,   750,   750,   752,   753,   751,   754,   754,   755,
     757,   756,   758,   759,   760,   761,   761,   763,   762,   764,
     762,   765,   762,   762,   762,   762,   762,   762,   762,   762,
     762,   762,   762,   762,   762,   762,   762,   762,   762,   762,
     762,   762,   766,   766,   766,   767,   767,   767,   767,   768,
     768,   768,   769,   769,   769,   770,   770,   771,   771,   772,
     773,   773,   774,   774,   774,   775,   775,   776,   776,   776,
     777,   777,   778,   778,   780,   779,   782,   781,   784,   783,
     785,   785,   787,   786,   788,   786,   789,   786,   790,   786,
     786,   791,   786,   786,   786,   792,   793,   793,   795,   794,
     796,   794,   797,   797,   798,   800,   799,   802,   801,   804,
     803,   805,   803,   806,   803,   807,   807,   807,   808,   808,
     809,   809,   810,   810,   810,   810,   810,   810,   810,   810,
     811,   811,   811,   811,   811,   811,   811,   811,   811,   811,
     811,   811,   811,   811,   811,   812,   812,   812,   812,   813,
     813,   813,   813,   813,   813,   813,   813,   813,   815,   814,
     816,   816,   818,   817,   819,   817,   820,   817,   821,   817,
     822,   817,   823,   817,   824,   817,   825,   817,   827,   826,
     828,   826,   826,   826,   826,   829,   829,   829,   829,   829,
     830,   831,   830,   832,   832,   833,   833,   834,   834,   835,
     835,   836,   836,   837,   838,   837,   837,   839,   840,   841,
     839,   842,   839,   839,   843,   839,   839,   839,   844,   839,
     839,   839,   845,   845,   846,   846,   847,   848,   847,   850,
     849,   851,   852,   853,   854,   856,   855,   857,   858,   858,
     859,   859,   861,   860,   862,   862,   863,   863,   865,   864,
     866,   866,   868,   867,   869,   870,   870,   871,   871,   873,
     872,   874,   874,   875,   875,   877,   876,   878,   878,   879,
     880,   881,   882,   883,   884,   885,   885,   886
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     0,     4,     3,     3,     0,     2,     1,
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     2,     3,     4,     3,     1,
       1,     1,     1,     1,     1,     4,     1,     0,     2,     4,
       4,     4,     4,     4,     4,     4,     4,     3,     0,     3,
       0,     3,     0,     2,     0,     0,    13,     3,     3,     3,
       4,     3,     4,     3,     4,     3,     3,     3,     3,     0,
       6,     0,     9,     3,     4,     7,     4,     7,     3,     3,
       3,     3,     3,     3,     3,     3,     6,     6,     0,     4,
       0,     4,     4,     4,     0,     0,     9,     0,     0,     9,
       3,     0,     4,     3,     0,     4,     0,     5,     3,     0,
       4,     3,     0,     4,     0,     5,     0,     4,     2,     3,
       3,     0,     0,     9,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     3,     3,     3,     3,     0,
       5,     0,     9,     0,     5,     7,     0,     4,     0,     7,
       0,     7,     3,     3,     0,     5,     0,     1,     0,     2,
       0,     2,     4,     0,     0,     0,     0,    11,     0,     0,
       9,     0,     9,     0,     2,     4,     0,     1,     1,     0,
       0,     4,     2,     0,     2,     0,     2,     0,     2,     0,
       1,     1,     0,     4,     0,     2,     1,     2,     2,     1,
       1,     1,     1,     0,     7,     0,     2,     1,     0,     1,
       1,     1,     1,     0,     0,     0,    12,     0,     0,     5,
       0,     5,     0,     5,     0,     2,     1,     2,     2,     2,
       2,     1,     2,     4,     1,     2,     4,     1,     2,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     2,     1,     1,     1,     1,     0,     2,     0,     4,
       0,     2,     0,     6,     0,     2,     0,     2,     6,     3,
       0,     7,     0,     4,     1,     2,     3,     3,     0,     0,
       0,    26,     0,     2,     4,     4,     6,     0,     4,     1,
       1,     2,     0,     2,     1,     1,     3,     0,     4,     1,
       1,     2,     2,     2,     2,     2,     3,     4,     3,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     0,     4,
       0,     2,     5,     0,     8,     0,     3,     0,     3,     5,
       0,     7,     0,     1,     1,     2,     0,     1,     1,     2,
       0,     4,     1,     2,     2,     2,     2,     2,     0,     2,
       3,     0,     4,     0,     2,     3,     3,     4,     5,     4,
       5,     3,     3,     3,     0,     3,     3,     1,     2,     0,
       2,     5,     6,     1,     0,     2,     3,     1,     2,     0,
       2,     3,     0,     2,     2,     2,     4,     3,     0,     0,
       0,     8,     1,     2,     0,     2,     0,     2,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     4,     0,     4,
       1,     2,     2,     2,     2,     0,     0,     0,     0,    12,
       0,     2,     3,     3,     4,     4,     3,     3,     3,     0,
       3,     0,     3,     0,     2,     5,     1,     1,     1,     3,
       3,     3,     3,     0,     2,     1,     1,     1,     0,     4,
       0,     2,     0,     3,     2,     4,     0,     4,     0,     3,
       0,     3,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     0,     4,     1,     2,     3,     0,
       2,     1,     1,     1,     2,     2,     2,     3,     1,     2,
       1,     1,     2,     2,     1,     1,     1,     1,     2,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     4,     3,     3,     3,     3,     3,     2,     3,
       4,     3,     2,     0,     4,     0,     4,     3,     3,     1,
       1,     5,     3,     0,     3,     0,     3,     0,     2,     2,
       3,     4,     3,     4,     5,     0,     4,     3,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       0,     4,     0,     5,     0,     5,     0,     5,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     4,     3,     0,
       4,     4,     2,     4,     4,     4,     3,     3,     4,     4,
       4,     4,     4,     4,     4,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     3,     3,     4,     3,     3,     1,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     1,     1,     2,     0,     0,     7,     3,
       5,     7,     5,     7,     7,     9,     1,     0,     2,     0,
       1,     0,     2,     2,     1,     1,     0,     2,     0,     6,
       0,     8,     7,    11,     4,     0,     0,    10,     0,     0,
      10,     6,     6,     0,     2,     1,     6,     6,     3,     2,
       1,     4,     0,     2,     0,     0,     7,     0,     2,     5,
       0,     4,     3,     1,     2,     0,     2,     0,     4,     0,
       4,     0,    10,     9,     3,     3,     4,     4,     4,     4,
       4,     4,     4,     4,     3,     7,     8,     6,     3,     3,
       3,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     5,
       1,     2,     0,     4,     7,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     0,     4,     0,     3,     0,     3,
       0,     2,     0,     4,     0,     4,     0,     4,     0,     4,
       4,     0,     4,     5,     1,     2,     0,     2,     0,     4,
       0,     4,     0,     2,     5,     0,     6,     0,     6,     0,
       6,     0,     6,     0,     6,     0,     1,     1,     1,     2,
       1,     2,     3,     3,     3,     3,     2,     3,     6,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     6,     1,     1,     3,     3,     6,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     5,
       0,     2,     0,     5,     0,     5,     0,     5,     0,     5,
       0,     5,     0,     5,     0,     5,     0,     5,     0,     4,
       0,     4,     1,     2,     2,     0,     1,     2,     5,     3,
       0,     0,     6,     0,     1,     0,     1,     0,     3,     0,
       1,     0,     2,     1,     0,     3,     1,     0,     0,     0,
       5,     0,     6,     2,     0,     5,     2,     5,     0,     6,
       2,     6,     0,     1,     0,     1,     0,     0,     3,     0,
       3,     4,     3,     3,     3,     0,     7,     2,     1,     2,
       3,     1,     0,     5,     1,     2,     1,     2,     0,     7,
       1,     2,     0,     7,     2,     1,     2,     3,     1,     0,
       5,     1,     2,     1,     2,     0,     7,     1,     2,     3,
       3,     3,     3,     3,     3,     0,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     9,     0,  1015,     1,   816,     0,   417,    78,   498,
       0,     0,   428,   469,   407,    66,   304,   367,   849,   851,
     853,   898,     0,     3,  1017,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   845,   847,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    20,    13,     8,
      14,    21,    46,    45,    47,    48,    15,    67,    16,    82,
      49,    17,     0,   302,     0,    18,    19,    23,   409,    26,
     419,    25,    24,    31,    27,   473,    28,   502,    29,   820,
      22,    50,    30,    32,    33,    34,    36,    35,    37,    38,
      39,    40,    41,    42,    43,    44,    10,  1016,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   900,     0,     0,     0,     0,     0,     0,     0,   975,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    60,    59,     0,    61,    62,     0,     0,
       0,     0,   305,     0,   372,   391,   388,   374,   393,     0,
       0,     0,   496,   814,     0,     2,   817,   427,    79,   499,
       0,    52,    51,   429,   470,   368,     0,     0,     0,     0,
       6,     0,     0,   972,   992,   974,   973,     0,  1010,  1009,
    1011,     5,    54,    53,     0,     0,  1012,  1013,  1014,    56,
      58,    64,    63,     0,   300,   299,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    68,     0,     0,     0,     0,
       0,    80,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   118,     0,     0,     0,     0,     0,     0,   131,     0,
     134,     0,     0,   139,     0,   142,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   146,   206,     0,     0,     0,     0,     0,   176,     0,
       0,     0,     0,     0,   184,   206,    84,     0,     0,     0,
       0,    77,    83,   306,   307,   339,   358,     0,   329,   308,
     327,   319,     0,   320,   322,   324,     0,   325,   360,   373,
     370,     0,   375,   376,   387,     0,   413,   406,   410,     0,
       0,     0,   416,   420,   422,     0,   471,     0,   483,   488,
     468,   474,   477,   476,   478,     0,   750,   583,     0,     0,
     585,   740,     0,   593,     0,   589,     0,     0,   529,   753,
     524,     0,     0,     0,     0,     0,   503,   508,   504,   505,
     506,   507,   511,   510,   512,   509,   513,   514,   516,     0,
     515,   517,   597,   520,     0,   521,   522,   523,   755,   590,
     824,   826,   828,     0,   831,     0,   822,     0,   821,   836,
     834,    11,   426,   434,     0,     0,     0,     0,   904,   914,
     902,   908,   916,   906,   910,   912,   901,     4,   971,     0,
       0,     0,     0,    57,     0,     0,     0,     0,    65,     0,
       0,     0,     0,     0,     0,     0,     0,   276,   277,   278,
     279,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      99,     0,   271,   272,   270,   273,   274,   275,     0,     0,
       0,     0,     0,     0,     0,     0,   241,   240,   242,   120,
       0,     0,     0,     0,     0,   136,     0,     0,     0,     0,
     144,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   207,   208,     0,     0,
     169,   173,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   186,     0,     0,   148,     0,     0,     0,
       0,     0,     0,   365,   303,   321,     0,   341,   343,   344,
     342,   345,   347,   348,   346,   349,   350,   351,   352,   355,
     353,   356,   354,   335,     0,     0,     0,   357,     0,     0,
       0,   380,     0,     0,   377,   378,   389,     0,     0,     0,
       0,     0,     0,     0,     0,   394,   408,     0,   418,   425,
       0,     0,     0,     0,     0,     0,     0,   490,   541,   549,
     550,   538,     0,   546,   540,   547,   544,   545,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   582,   744,   742,   578,     0,
       0,   500,   497,     0,     0,     0,   739,   696,   414,   414,
     414,   414,     0,     0,   707,   706,     0,     0,     0,     0,
     835,     0,   842,     0,   818,   815,     0,     0,   436,   869,
       0,     0,     0,   855,   888,     0,     0,   855,     0,   884,
     883,     0,     0,     0,   855,     0,   899,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   999,     0,   995,   998,
       0,   982,     0,   978,   981,   855,   855,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    98,     0,     0,   112,
     103,   113,    81,   108,     0,    93,     0,    89,     0,     0,
     182,   111,   941,   110,    87,    97,     0,     0,   256,   115,
       0,     0,   114,   109,     0,     0,     0,     0,     0,   130,
     232,     0,     0,   133,     0,   238,   138,     0,   141,     0,
     238,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,     0,   165,   166,     0,   282,   283,   284,   285,     0,
       0,   167,   280,   224,   168,     0,     0,     0,     0,     0,
       0,    88,   183,    96,     0,    91,    95,   714,   716,     0,
     187,   188,     0,   150,   149,   340,     0,   326,     0,     0,
       0,   330,     0,   323,   494,     0,   338,   336,     0,   414,
     414,   361,   376,   392,   384,   385,   386,     0,     0,   404,
     369,   379,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   421,   423,   480,   479,   481,   472,     0,
     485,   486,   487,   482,   484,     0,   542,   543,   559,   562,
     560,   561,   563,   564,   551,   539,   569,   570,   566,   565,
     567,   568,   552,   555,   553,   554,   556,   557,   558,   548,
     537,     0,     0,     0,   571,     0,   577,   594,   576,     0,
     575,   574,   573,   531,   532,   533,   528,   530,     0,     0,
     526,   518,   519,     0,     0,   581,   579,     0,     0,     0,
     587,   588,     0,     0,     0,   595,     0,     0,   626,     0,
       0,   605,     0,     0,   620,   622,   678,     0,     0,     0,
       0,     0,     0,     0,     0,   691,     0,     0,     0,     0,
     639,   624,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   656,   662,   658,   660,
       0,   592,   598,   608,   679,     0,     0,     0,     0,     0,
     414,     0,     0,   738,   695,     0,   806,     0,     0,     0,
       0,   757,   805,     0,     0,     0,     0,   759,     0,     0,
     785,   786,     0,     0,     0,   787,   788,     0,     0,     0,
     782,   783,   784,   752,   756,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   838,   840,   837,   435,   430,     0,   866,     0,     0,
       0,     0,     0,   856,   857,   850,     0,     0,     0,   852,
       0,   880,     0,     0,     0,   893,   894,   889,   890,   891,
     892,     0,     0,   895,   896,   897,     0,     0,     0,     0,
       0,   854,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   855,   996,     0,     0,
       0,   855,   979,   846,   848,     0,    70,    73,    75,    72,
      71,    69,    74,    76,   106,     0,    94,    90,   104,     0,
     944,   943,   946,   947,   941,   258,   259,   260,   119,   257,
       0,     0,   261,     0,   122,     0,   121,   127,   123,   124,
       0,   132,   135,   239,     0,   140,   143,     0,   164,   171,
     147,   178,     0,     0,     0,     0,     0,     0,   193,     0,
     177,    92,     0,   180,     0,     0,   151,   359,     0,   334,
     333,   332,   328,   331,   366,     0,   337,   363,     0,     0,
     382,     0,   390,     0,   395,   396,   403,   402,     0,   401,
       0,     0,     0,     0,   424,     0,     0,   489,   491,   751,
     584,   572,   586,     0,   536,   535,   534,   525,   527,     0,
     741,   743,   580,   501,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     687,   688,   689,   690,     0,   692,   694,   693,     0,     0,
     682,   685,   686,   684,   683,   681,     0,     0,     0,     0,
       0,     0,   966,   966,   966,     0,     0,   966,   966,   966,
     966,     0,     0,   966,     0,     0,     0,     0,     0,     0,
     599,     0,     0,   642,     0,   697,   415,     0,   716,     0,
       0,     0,     0,     0,   718,   699,   708,   754,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   807,   808,
     809,     0,     0,   800,     0,     0,     0,   797,   781,     0,
       0,     0,     0,   789,   790,   791,     0,     0,   795,   796,
       0,   825,   827,     0,     0,   829,     0,     0,   832,     0,
       0,   843,   823,   819,   830,     0,     0,   455,     0,   448,
       0,     0,     0,   439,   440,     0,   437,   442,   443,   444,
     441,   438,   858,     0,     0,   867,   863,   862,   864,   865,
       0,   886,   885,     0,   881,   872,   871,   870,   876,   877,
     879,   878,   875,   874,   873,   922,   918,   920,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   997,  1001,     0,
     994,   993,   980,   984,     0,   977,   976,   301,     0,     0,
     264,     0,     0,   267,   935,     0,   948,     0,     0,     0,
       0,     0,     0,     0,   942,     0,     0,   262,     0,   243,
     254,   254,     0,   137,   145,   217,   209,   281,   170,   231,
     229,     0,   230,     0,   174,   225,   226,     0,     0,     0,
     254,   294,   185,   715,   717,   215,   189,     0,   235,     0,
     495,     0,     0,   371,   381,   383,   405,   399,     0,     0,
     397,     0,   411,   475,   492,   591,   745,   724,   628,   633,
     634,   677,   676,   673,     0,   674,   596,   615,   613,     0,
       0,   611,     0,   629,   630,     0,     0,   607,   632,   631,
     614,   612,   635,   636,   610,   619,   618,   609,   617,   616,
       0,     0,   668,     0,   638,   967,     0,     0,     0,   646,
     647,     0,     0,     0,     0,   969,     0,     0,     0,     0,
     664,   665,   666,   667,     0,     0,     0,     0,   602,     0,
     600,     0,     0,     0,   709,   716,     0,     0,     0,     0,
       0,   720,     0,     0,     0,     0,     0,   810,     0,     0,
       0,     0,     0,   812,     0,   774,     0,   801,   764,     0,
     765,   798,   778,   779,   780,     0,   792,   793,   794,     0,
       0,     0,     0,     0,     0,     0,   833,     0,     0,     0,
       0,   452,   453,   454,     0,     0,     0,     0,   432,   431,
     859,     0,     0,     0,     0,     0,   923,   937,   937,   924,
     905,   915,   903,   909,   917,   907,   911,   913,  1002,     0,
     985,     0,     0,     0,   265,     0,     0,   268,   936,   945,
     953,     0,     0,   956,     0,     0,     0,   960,   100,     0,
     116,   117,   254,     0,     0,   233,     0,   219,     0,     0,
       0,   227,   228,     0,   101,     0,   194,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   400,   398,   412,   493,
       0,     0,   675,     0,   637,   606,   621,     0,   672,   671,
     670,   640,   669,     0,     0,   643,   645,   644,   648,   649,
     652,   651,     0,   653,   654,   650,   655,   657,   663,   659,
     661,   603,     0,   601,   680,   641,   710,   711,     0,   700,
       0,   716,     0,   702,     0,     0,   769,   773,   771,   767,
     811,   758,   768,   766,   770,   772,   813,   760,     0,     0,
       0,     0,   761,     0,     0,     0,     0,     0,     0,   839,
     841,   456,   450,     0,     0,   445,   446,   433,   860,     0,
       0,     0,     0,     0,   939,   939,     0,  1000,  1003,     0,
     983,   986,     0,   107,     0,   105,   949,   954,     0,     0,
       0,     0,   263,     0,   255,   128,   125,   235,   218,   220,
     221,   222,   210,   212,   179,   175,   203,     0,     0,   295,
     198,   216,   181,     0,     0,   236,   237,     0,     0,   362,
       0,   747,     0,   627,   623,   625,   968,   970,   604,     0,
       0,     0,     0,     0,   716,     0,     0,     0,   719,     0,
       0,     0,     0,   802,     0,     0,     0,     0,     0,     0,
       0,   449,   451,   447,   861,   868,   887,   882,     0,   940,
     919,   921,     0,  1004,     0,   987,   266,   269,   962,   925,
       0,   951,   958,     0,   244,     0,     0,     0,     0,     0,
     213,     0,     0,   203,     0,     0,   254,     0,   152,     0,
     716,     0,   746,     0,   713,   712,   698,     0,   701,     0,
     704,   703,     0,     0,   799,   777,     0,     0,     0,     0,
     725,   728,   736,   737,   844,     0,   938,  1005,   988,   963,
     950,     0,     0,   926,   955,   957,   964,   930,     0,   247,
       0,   129,   126,   234,     0,   172,     0,   211,     0,   102,
     204,   201,   195,   199,     0,   153,   309,     0,     0,   748,
       0,     0,     0,   721,   775,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   927,     0,   965,   952,     0,   959,
     961,     0,     0,     0,   252,     0,   214,     0,   296,   254,
     290,    85,     0,   364,     0,     0,     0,   705,   776,     0,
       0,     0,   726,   729,   457,     0,     0,     0,   929,     0,
     250,   248,     0,   254,   223,   205,   202,   196,     0,   200,
     290,   190,     0,   749,     0,     0,   803,     0,   763,   733,
     733,   460,  1007,     0,   990,     0,     0,     0,   254,   254,
     245,     0,     0,   297,   286,   294,   291,     0,     0,   190,
       0,   723,     0,     0,   762,   727,   730,     0,  1008,  1006,
     991,   989,   928,   931,     0,     0,   254,   253,     0,   197,
       0,     0,    86,   191,     0,   722,     0,   735,   734,     0,
       0,   458,     0,     0,     0,     0,   461,   933,   251,   249,
       0,     0,     0,   287,   292,     0,     0,   804,     0,     0,
       0,     0,     0,     0,     0,   934,   932,   246,     0,   288,
     254,   192,     0,     0,   466,   459,     0,   462,   463,   467,
       0,   254,   293,     0,   465,   464,   298,   289,     0,     0,
       0,     0,     0,     0,     0,   310,   312,   311,     0,     0,
     317,     0,   313,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   315,   318,   314,     0,   316
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    46,   113,    47,    48,     3,   155,    49,    50,
      51,    52,    53,    54,    55,   135,   138,   193,    56,    57,
     140,   205,    58,    59,   100,   271,   413,   141,   272,   483,
    1911,   672,  1706,   430,   685,  1351,  1776,  1350,  1775,   442,
     444,   695,   447,   449,   700,   465,  1378,  1845,   722,  1355,
     723,   475,  1356,  1375,   481,   741,  1085,  1938,  1939,   730,
    1370,  1708,  1879,  1934,  1786,  1880,  1878,  1782,  1783,   468,
    1570,  1780,  1837,  1580,  1567,  1701,  1779,  1074,  1365,  1366,
     692,  1697,  1582,  1715,  1064,   439,  1056,  1562,  1829,  1956,
    1873,  1929,  1928,  1831,  1903,  1563,   677,   678,  1051,  1052,
    1329,  1330,  1332,  1333,   428,   411,  1073,   719,  1959,  1983,
    2011,  1909,  1910,  2000,  1578,  1906,  1933,    60,   394,    61,
     143,    62,    63,   281,   491,  1882,  2026,  2027,  2032,  2035,
     282,   283,   495,   284,   492,   750,   751,   285,   286,   488,
     515,   287,   288,   489,   517,   761,  1381,   494,   752,    64,
     107,    65,    66,   518,   290,   146,   523,   524,   525,   767,
    1101,   147,   293,   526,   148,   291,   294,   535,   770,  1103,
      67,    68,   297,   149,   298,   299,   907,    69,    70,   302,
     150,   303,   540,   784,   304,    71,    72,    73,   105,   373,
    1275,  1519,   608,   965,  1276,  1277,  1278,  1279,  1280,  1514,
    1663,  1281,  1510,  1750,  1921,  1990,  1947,  1976,    74,    75,
     106,   310,   544,   151,   311,   312,   313,   546,   794,   314,
     547,   795,  1118,  1589,  1373,    76,   335,    77,   101,   582,
     848,   152,   336,   572,   839,   337,   571,   837,   840,   338,
     558,   819,   804,   812,   339,   340,   341,   342,   343,   344,
     345,   346,   560,   347,   563,   348,   349,   366,   350,   351,
     352,   565,   901,  1142,   585,   902,  1148,  1151,  1152,  1180,
    1145,  1179,  1195,  1197,  1198,  1196,  1454,  1431,  1432,   903,
     904,  1204,  1176,  1164,  1168,   593,   594,   905,  1464,   914,
    1627,  1731,   738,  1374,  1082,   595,  1472,  1634,  1733,   584,
     953,  1859,  1919,  1860,  1920,  1945,  1968,   956,   353,   354,
     355,   844,   577,   843,  1590,  1792,  1721,   356,   559,   357,
     358,   943,   596,   944,  1222,  1227,  1743,   945,   946,  1246,
    1499,  1250,  1236,  1237,  1234,  1808,   947,  1231,  1478,  1484,
      78,   367,    79,    98,   605,   959,   153,   368,   603,   597,
     598,   599,   601,   369,   606,   963,  1265,  1266,   957,  1261,
      80,   126,    81,   127,    82,   108,   109,   110,   975,  1284,
    1670,   623,   624,   625,   996,    83,   111,   169,   386,   629,
     627,   632,   630,   633,   634,   628,   631,  1309,  1527,  1528,
    1824,  1869,  1977,  1996,  1549,  1674,  1760,  1043,  1044,  1334,
    1343,  1551,  1768,  1826,  1769,  1827,  1820,  1867,  1436,  1604,
    1446,  1612,    84,    85,    86,    87,    88,   177,  1021,   642,
     643,   644,  1019,  1324,  1680,  1681,  1863,  1925,    89,   389,
    1016,   637,   638,   639,  1014,  1319,  1677,  1678,  1862,  1923,
      90,    91,    92,    93,    94,    95,    96,    97
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -1560
static const yytype_int16 yypact[] =
{
     755, -1560,    92,   958, -1560, -1560,   -95, -1560, -1560, -1560,
     -73,   453, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560,   -37, -1560, -1560,   113,   128,   225,   239,   244,
     267,   277,   282,   287,   126,   522, -1560, -1560,   290,   295,
     302,   312,  -220,   446,   419,   326, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560,   345, -1560,   212, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560,   480, -1560,   356,   100,
     383,   386,   371,   132,   136,   423,   425,   439,   449,   468,
     484, -1560,   157,   489,   452,   198,   204,   232,   237, -1560,
     240,   260,   314,   316,   322,   341,   613,   623,   381,   394,
     397,   403, -1560, -1560, -1560,   422, -1560, -1560,   692,  -164,
      60,   399,     0,   396,   802, -1560,   797, -1560, -1560,   125,
      75,     9,   242,   752,   827, -1560, -1560, -1560, -1560, -1560,
     432, -1560, -1560, -1560, -1560, -1560,   479,   481,   485,  1216,
   -1560,   483,   490, -1560, -1560, -1560, -1560,   492, -1560, -1560,
   -1560, -1560, -1560, -1560,   495,   496, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560,   523, -1560, -1560,   857,   884,   568,   810,
     885,   883,   891,   804,   674, -1560,   803,   949,   805,     8,
     806, -1560,   807,   808,   809,   150,   811,   812,   813,     6,
     815, -1560,  -130,   816,   817,   284,   684,   819, -1560,   820,
   -1560,   824,   830, -1560,   832, -1560,   833,   834,   835,   839,
     840,   841,   842,   844,   845,   846,   848,   849,   851,   852,
     858, -1560,   456,   859,   860,   862,   864,   865,   564,   866,
     868,   869,   870,   871, -1560,   456, -1560,   596,   872,   614,
     873, -1560, -1560, -1560, -1560, -1560, -1560,   875, -1560, -1560,
   -1560, -1560,  1011, -1560, -1560, -1560,   540, -1560, -1560, -1560,
   -1560,   879, -1560,   329,   119,   916, -1560, -1560, -1560,   881,
     991,   888, -1560, -1560, -1560,   170, -1560,   882, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560,    30, -1560, -1560,   889,   890,
   -1560, -1560,   -92, -1560,   892, -1560,   893,   122, -1560, -1560,
   -1560,   557,   621,   997,   665,  1025, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,   898,
   -1560, -1560, -1560, -1560,   373, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560,   901, -1560,   902, -1560,  1026, -1560, -1560,
   -1560, -1560, -1560,   687,    -5,   -89,    80,   847, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,   584,
     610,   -89,   -89, -1560,   906,   905,   907,   908, -1560,   909,
     910,   915,   917,   918,   667,    44,   668, -1560, -1560, -1560,
   -1560,   669,   671,   932,   672,  -125,   -87,   111,   673,   675,
   -1560,   676, -1560, -1560, -1560, -1560, -1560, -1560,   677,   679,
     936,   680,   681,   941,   685,   686, -1560, -1560, -1560,   947,
       2,   690,   268,   728,   268, -1560,   729,   268,   730,   268,
   -1560,   731,   732,   733,   737,   738,   739,   740,   742,   748,
     749,   952,   750,   754,  1017,   332, -1560, -1560,  1015,   756,
   -1560, -1560,   758,   837,   823,     3,   762,   763,   767,   -79,
     773,   -92,  1035,   794,   -92,   777, -1560,   778,  1044,  1045,
     785,  1057,  1059, -1560, -1560,   457,  1080, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560,  1084,   800,   760,    38,   797,   825,
     441, -1560,  1091,  1225,    39, -1560, -1560,   117,  1092,  1093,
     -92,  1094,  1095,  1096,  1097, -1560, -1560,  1100, -1560, -1560,
     -61,   831,   850,   853,  1101,  1263,  -103, -1560,  -196,   -20,
   -1560,   928,   477,    40, -1560, -1560, -1560, -1560,   855,  1109,
    1111,  1118,   861,  1119,   863,  1120,   867,  1278,   874,   876,
     877,   -93,  1124,   880,   903, -1560, -1560, -1560, -1560,   904,
     955, -1560, -1560,   -52,   912,    73, -1560, -1560,   919,   919,
     919,   -30,  1128,  1264, -1560, -1560,  1136,  1132,  1132,   699,
   -1560,   702, -1560,  1132, -1560, -1560,    24,   913, -1560, -1560,
      80,    -5,    -5,   143, -1560,    80,   -89,  -126,    80, -1560,
   -1560,    80,    80,   656,    65,   688, -1560,  1133,  1134,  1137,
    1141,  1145,  1163,  1164,  1167,  1142, -1560,   138, -1560, -1560,
    1166, -1560,   201, -1560, -1560,  -126,  -126,  1169,   914,   920,
     921,   922,   923,   924,   925,   926, -1560,   927,   929, -1560,
   -1560, -1560, -1560, -1560,   933, -1560,   935, -1560,   937,   938,
   -1560, -1560,   -74, -1560, -1560, -1560,   529,   -82, -1560, -1560,
     939,  1168, -1560, -1560,   942,  1052,  1176,   943,  1177, -1560,
   -1560,   940,   944, -1560,   945,  1037, -1560,   948, -1560,   950,
    1037, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560,   951, -1560, -1560,  1179, -1560, -1560, -1560, -1560,   953,
    1188, -1560,  1194, -1560, -1560,  1196,  1198,  1214,  1212,  1219,
     956, -1560, -1560, -1560,   957, -1560, -1560, -1560, -1560,  1218,
   -1560,  1224,   -92, -1560, -1560, -1560,   960, -1560,   961,   559,
     -65, -1560,  1226, -1560, -1560,  1222, -1560, -1560,   963,   919,
     919, -1560,    69, -1560, -1560, -1560, -1560,    39,   965, -1560,
   -1560, -1560,   966,   967,   968,   969,   -92,   970,  1391,  1256,
    1234,  1235,  1236, -1560, -1560, -1560, -1560, -1560, -1560,  1237,
   -1560, -1560, -1560, -1560, -1560,   -62, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560,   978,   979,   980, -1560,   981, -1560, -1560, -1560,  1242,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,   590,   -54,
   -1560, -1560, -1560,  1247,   475, -1560, -1560,   986,  1249,  1254,
   -1560, -1560,  1255,   109,   474, -1560,  1257,  1265, -1560,  1258,
    1270, -1560,  1271,  1272, -1560, -1560, -1560,  1273,  1274,  1275,
    1276,  1277,  1279,  1280,   455,   118,  1281,   531,  1282,  1283,
   -1560, -1560,  1266,  1284,  1285,  1286,  1287,  1288,  1289,  1290,
    1291,  1292,  1293,  1294,  1295,  1296, -1560, -1560, -1560, -1560,
     -81, -1560, -1560, -1560,   208,  1299,  1298,   -24,   -21,   -18,
     919,   -92,  1018, -1560,   407,  1323, -1560,  1300,  1301,  1302,
    1303, -1560, -1560,  1304,  1305,  1306,  1307, -1560,   454,  1229,
   -1560, -1560,  1308,  1048,  1050, -1560, -1560,  1312,  1313,  1314,
   -1560, -1560, -1560, -1560, -1560,  1260,   494,   299,  1315,  1056,
    1058,  1317,  1321,  1062,  1324,  1325,  1063,    77,  1066,  1329,
    1330, -1560, -1560, -1560, -1560,   -25,    47, -1560,   257,    -5,
      -5,    -5,    -5, -1560, -1560, -1560,    47,  -275,   -89, -1560,
      47, -1560,   428,   264,   380, -1560, -1560, -1560, -1560, -1560,
   -1560,    -5,    -5, -1560, -1560, -1560,    -5,    80,    80,    80,
      80, -1560,   -89,   -89,   -89,   318,   318,   318,   318,   318,
     318,   318,   318,  1069,  1331,  1262,    86, -1560,  1070,  1332,
    1259,    86, -1560, -1560, -1560,  1076, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560,  1077, -1560, -1560, -1560,  1078,
   -1560, -1560, -1560,    14,   -74, -1560, -1560, -1560, -1560, -1560,
    1337,   447, -1560,  1079, -1560,  1340, -1560, -1560, -1560, -1560,
     -92, -1560, -1560, -1560,  1082, -1560, -1560,  1083, -1560, -1560,
   -1560, -1560,  1344,  1085,   -75,  1351,  1385,  1348, -1560,  1349,
   -1560, -1560,  -117, -1560,  1350,  1148, -1560, -1560,  1150, -1560,
   -1560, -1560, -1560, -1560, -1560,  1098, -1560, -1560,   -92,  1225,
   -1560,  -101, -1560,  1355, -1560, -1560, -1560, -1560,  1099, -1560,
    1354,  1356,  1102,   -99, -1560,  1103,  1144, -1560, -1560, -1560,
   -1560, -1560, -1560,  1104, -1560, -1560, -1560, -1560, -1560,  1105,
   -1560, -1560, -1560, -1560,  1144,  1106,  1107,  1108,  1110,  1112,
    1113,  -111,  1357,  1114,  1115,  1358,  1359,  1116,  1363,  1117,
    1121,  1371,  1378,  1122,  1123,  1125,  1126,  1127,  1129,  1130,
   -1560, -1560, -1560, -1560,  1131, -1560, -1560, -1560,  1135,  1138,
   -1560, -1560, -1560, -1560, -1560, -1560,  1139,  1140,  1143,  1373,
    1379,  1146,  1488,  1488,  1488,  1147,  1149,  1488,  1488,  1488,
    1488,  1492,  1492,  1488,  1492,   388,  1390,  1393,  1395,  -108,
   -1560,   971,    30, -1560,   407, -1560, -1560,   -92, -1560,   -92,
     -92,   -92,   -92,   -92, -1560, -1560, -1560, -1560,  1394,  1396,
    1397,  1401,  1404,  1405,  1408,  1410,  1411,  1406, -1560, -1560,
   -1560,  1152,  1413, -1560,   -41,  1414,   451, -1560, -1560,  1155,
    1156,  1157,  1418, -1560, -1560, -1560,   507,  1419, -1560, -1560,
    1436, -1560, -1560,  1559,  1560, -1560,  1561,  1562, -1560,  1398,
    1425, -1560, -1560, -1560, -1560,  1132,  1132, -1560,   639, -1560,
    1428,  1429,  1431, -1560, -1560,  1565, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560,  1392,    -5, -1560,   461,   461, -1560, -1560,
     -89, -1560, -1560,    80, -1560,   342,   342,   342, -1560, -1560,
     670,   369,  1175,  1175,  1175,  1434, -1560, -1560,  1437,  1174,
    1178,  1180,  1181,  1182,  1183,  1184,  1185, -1560, -1560,   -14,
   -1560, -1560, -1560, -1560,    15, -1560, -1560, -1560,  1439,   459,
   -1560,  1446,   466, -1560,  1187,  1447, -1560,  1448,  1449,  1450,
    1451,  1452,  1453,  1195, -1560,  1454,  1197, -1560,  1199, -1560,
   -1560, -1560,   -92, -1560, -1560,  1239,   -83, -1560, -1560, -1560,
   -1560,  1456, -1560,  1458, -1560, -1560, -1560,  1460,  1461,  1267,
   -1560,  1213, -1560, -1560, -1560,  1467, -1560,  1464, -1560,  1466,
   -1560,   -92,   -92, -1560, -1560, -1560, -1560, -1560,  1207,  1208,
   -1560,  1209, -1560, -1560, -1560, -1560, -1560,  1608, -1560, -1560,
   -1560, -1560, -1560, -1560,  1211, -1560, -1560, -1560, -1560,  1472,
    1215, -1560,  1217, -1560, -1560,  1220,  1473, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
     650,   -51, -1560,  1474, -1560, -1560,  1221,  1223,  1227, -1560,
   -1560,  1228,  1230,  1231,  1232, -1560,  1233,  1238,  1240,  1241,
   -1560, -1560, -1560, -1560,  1243,  1244,  1245,  1246, -1560,  1060,
   -1560,  1248,  1250,  1611,  1253, -1560,  -105,   -92,   -92,   -92,
    1251, -1560,  1480,  1252,  1261,  1268,  1269, -1560,   -15,  1297,
    1309,  1310,  1311, -1560,   -13, -1560,  1444, -1560, -1560,  1482,
   -1560, -1560, -1560, -1560, -1560,  1484, -1560, -1560, -1560,  1432,
    1485,   454,  1489,  1495,  1496,  1497, -1560,  1470,  1316,  1318,
    1501, -1560, -1560, -1560,    53,  1507,  1319,  1320,  1511, -1560,
   -1560,    66,     5,  1003,    19,   460, -1560,  1455,  1455, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,  1435,
   -1560,  1438,  1513,  1322, -1560,  1515,  1326, -1560, -1560, -1560,
   -1560,  1518,  1519,  1457,  1327,  1328,  1336, -1560, -1560,  1333,
   -1560, -1560, -1560,    16,    25, -1560,  1522,   551,  1523,  1524,
    1334, -1560, -1560,  1338, -1560,  1527,  1530,  1531,  1532,  1533,
    1339,  1335,   -71,  1534,   -92,  1341, -1560, -1560, -1560, -1560,
    1598,  1536, -1560,  1342, -1560, -1560, -1560,  1343, -1560, -1560,
   -1560, -1560, -1560,  1345,  1539, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560,  1540, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560,  1346, -1560, -1560, -1560, -1560,    52,    -7, -1560,
     -92, -1560,  1677, -1560,  1542,  1347, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,  1541,  1543,
    1544,  1545, -1560,  1493,  1576,  1580,  1582,  1583,  1550, -1560,
   -1560, -1560, -1560,   -76,  1352, -1560, -1560, -1560, -1560,  1510,
      -5,   -89,    80,  1552,  1553,  1553,  1554,  1435, -1560,  1555,
    1438, -1560,  1353, -1560,  1360, -1560, -1560, -1560,  1556,  1558,
    1563,  1564, -1560,    29, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560,  1377, -1560, -1560, -1560, -1560,  1362,  1599,  1567, -1560,
   -1560, -1560, -1560,  1568,  1361, -1560, -1560,  1364,   -92, -1560,
     -92, -1560,  1724, -1560, -1560, -1560, -1560, -1560, -1560,  1572,
    1573,  1365,  1574,  1366, -1560,    10,  1367,  1677, -1560,    63,
    1368,  1369,  1575,  1577,  1584,  1585,  1587,  1588,  1589,  1372,
    1595, -1560, -1560, -1560, -1560,   342,  1175,   417,  1591, -1560,
   -1560, -1560,  1374, -1560,  1375, -1560, -1560, -1560,  1603,    49,
    1592, -1560, -1560,  1370, -1560,  1537,  1537,   -69,  1596,  1376,
    1380,  1597,  1381,  1362,  1600,  1604, -1560,  1621, -1560,  1400,
   -1560,   -92,  1598,  1609, -1560, -1560, -1560,  1767, -1560,    -7,
   -1560, -1560,  1382,  1383, -1560, -1560,  1610,  1613,  1624,  1617,
    1384,  1386, -1560, -1560, -1560,  1618, -1560, -1560, -1560, -1560,
   -1560,  1620,  1623, -1560, -1560, -1560,  1388,  1402,  1631,   -63,
    1633, -1560, -1560, -1560,  1440, -1560,  1637, -1560,  1678, -1560,
   -1560, -1560, -1560,  1530,  1639, -1560, -1560,    13,  1645, -1560,
    1681,  1648,  1399, -1560, -1560,  1403,  1650,  1651,  1652,  1761,
    1762,  1407,  1578,  1581,  1594,  1655, -1560, -1560,  1660, -1560,
   -1560,  1661,  1662,  1612, -1560,  1664, -1560,  1665, -1560, -1560,
    1671, -1560,  1672, -1560,  1409,  1673,  1705, -1560, -1560,  1674,
    1675,  1412, -1560, -1560, -1560,  1676,  1679,  1680, -1560,  1479,
   -1560, -1560,  1682, -1560, -1560, -1560,  1683,  1530,  1684, -1560,
    1671,  1459,  1685, -1560,  1686,  1687,  1688,  1421, -1560, -1560,
   -1560, -1560, -1560,    31, -1560,    32,  1689,  1691, -1560, -1560,
   -1560,    33,  1692, -1560, -1560,  1213, -1560,  1693,  1441,  1459,
    1696, -1560,  1695,  1697, -1560,  1698,  1698,    46, -1560, -1560,
   -1560, -1560, -1560, -1560,    36,    41, -1560, -1560,  1487,  1703,
    1699,  1738, -1560, -1560,  1442, -1560,  1704, -1560, -1560,  1850,
    1706, -1560,  1753,  1707,  1708,  1709, -1560,  1462, -1560, -1560,
      43,  1710,  1711, -1560, -1560,  1712,  1512, -1560,  1714,  1463,
    1718,  1717,  1465,  1468,  1469, -1560, -1560, -1560,  1756, -1560,
   -1560, -1560,  1719,  1471, -1560, -1560,  1475, -1560, -1560, -1560,
    1720, -1560,  1530,  1721, -1560, -1560, -1560,  1530,  1476,  1546,
    1722,  1726,  1727,  1730,  1477, -1560, -1560,   -53,  1732,  1733,
   -1560,  1734, -1560,  1739,  1740,  1743,  1742,  1744,  1478,  1483,
    1486,  1745, -1560, -1560, -1560,  1490, -1560
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560,   -19, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560,   140, -1560,  1654,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
     209, -1560,   224, -1560,  1387, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560,   148, -1560, -1348, -1560,  1389, -1560,   878,
   -1560,   597, -1560,   593, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560,    17, -1560, -1560,    -4, -1560, -1560, -1560, -1560,   972,
   -1560,  1635, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560,  1443, -1560, -1560,  1186, -1560,  -132, -1560,
    -332, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560,  1415,  1170, -1560,  -513, -1560,
   -1560,  -142, -1560, -1560, -1560, -1560, -1560, -1560,   836, -1560,
     974, -1560, -1560, -1560, -1560, -1560,  -533, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1407, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560,  -286, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,  1151, -1560,
     741, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,   499, -1560,
   -1560, -1560, -1560, -1560, -1560,   736,  1020, -1560, -1560, -1560,
   -1560, -1560,  -889, -1195, -1200, -1560, -1560, -1560, -1559,  -591,
   -1560, -1560, -1560, -1560, -1560,    21, -1560, -1560, -1560, -1560,
   -1560, -1560,  1153, -1560, -1560, -1560,   152, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560,   706, -1560, -1560,   206,   445, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560,  1794, -1560, -1560, -1560,  -596,  -435,
    -964,  -365,  -605,  -361,  -624, -1560, -1560, -1560, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560,  -211, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560,   420,   274,   911, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,  -522, -1560,
    -715, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560,
    1416, -1560, -1560, -1560, -1560,   270, -1560, -1560, -1560, -1560,
   -1560, -1560,  1417, -1560, -1560, -1560, -1560,   275, -1560, -1560,
   -1560, -1560, -1560, -1560, -1560, -1560, -1560, -1560
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -733
static const yytype_int16 yytable[] =
{
     516,  1004,   579,  1564,   292,   966,   949,   950,  1466,   613,
     976,   771,   958,   980,   617,  1468,   981,   983,  1208,  1210,
     334,   979,  1576,  1335,   727,   305,   798,  1732,  1001,   273,
     645,   646,   910,  2028,   422,  2029,   564,  1391,  1207,  1267,
     548,  1209,   431,  1404,  1211,  2030,   306,   664,   580,  1023,
    1024,  1040,   549,   550,   551,   496,   908,   909,   911,  1199,
    1969,   960,   407,  1359,   496,   790,   791,   496,   552,   520,
    1821,   194,  1729,  1736,   196,   833,   834,    99,  1336,   792,
     496,  1970,   614,  1971,  1568,   666,   197,   852,   676,   835,
     198,   496,     4,   734,  1268,   813,   814,   199,   423,   102,
     853,   496,   916,   496,  1871,   749,   854,  1662,  1116,    14,
     855,   782,   300,   799,  1259,  1136,   838,   856,   857,  1430,
     849,   275,   615,  1165,   800,   801,   553,   432,   424,   815,
     858,  1487,   568,   145,   112,   521,   859,    16,   860,   861,
     307,   759,   554,   862,   863,  1268,   816,   760,   496,   864,
     308,   496,   527,   978,   496,  1640,  1972,  1646,  1538,  1291,
    1269,   796,   295,   865,  1166,   496,   200,   609,   408,   686,
     201,   772,   569,   817,   687,  1015,   922,   866,  1802,   867,
    1973,   868,   496,   869,   758,   496,  1730,  1540,  1694,   870,
     871,   528,   541,   872,   873,   737,   425,  1694,   742,   874,
     875,  1694,   797,  1948,  1950,  1694,   610,   202,  1694,   520,
     529,  1269,   132,  1694,  1693,  1694,   657,   301,  1668,   876,
    1137,   961,   962,  1268,  1202,   203,  1097,  1098,   530,   531,
    1167,   877,  1668,  1872,   878,   997,   998,   879,  1020,   520,
    1852,   999,  1000,   426,   776,  1203,   967,   968,   847,  1337,
     296,   614,   609,   532,  1100,   977,  1752,   982,   315,  1282,
     316,   984,   144,   997,   998,  1628,   542,   555,   195,   999,
    1000,  1569,  1630,  1631,   417,   521,   145,   773,   317,  1668,
    1260,   997,   998,   668,  1822,   114,   533,   999,  1000,  1269,
     275,   618,   318,   319,   619,   620,   802,   123,   688,  1041,
     115,   658,   978,   433,   621,   521,   973,   665,   320,   880,
     974,   570,  1360,  1338,  1361,  1372,   514,  1974,  1465,  1362,
    1467,  1405,   418,   881,  1458,   514,   321,  1629,   514,   793,
     322,  1384,   882,  1392,   883,   884,   885,   886,   887,   836,
    1363,   514,   323,  2031,   616,   667,  1042,  1823,   324,   906,
    1048,  1200,   514,   735,   556,   204,  1751,  1364,  1247,   427,
    1004,  1339,   514,  1714,   514,  1833,  1270,  1092,   669,   728,
    1117,   783,   325,   326,   327,  1271,  1272,  1213,  1127,   557,
     850,  1601,   803,   328,   274,   409,   410,   635,   636,  1718,
     329,  1488,  1298,  1299,  1300,  1301,   818,   116,   888,   889,
     890,   891,   892,   893,   894,   895,   309,   543,   206,   514,
     586,   117,   514,   207,   208,   514,   118,  1641,  1539,  1647,
    1321,   896,   611,  1340,  1341,  1326,   514,   729,   612,  1975,
    1342,  1735,   209,   978,   210,  1734,   211,   587,  1843,   119,
     690,  1669,  1800,   514,   275,  1883,   514,  1541,  1695,   120,
     640,   641,   212,   900,   121,  1669,  1086,  1696,   897,   122,
     276,  1774,   128,  1949,  1951,  1957,   898,   129,  1978,   534,
     588,   587,   899,  1979,   130,  1997,   589,  1447,   330,  1449,
     997,   998,   590,  1283,   131,   213,   999,  1000,   806,   807,
    1108,    16,  1584,   969,   970,   971,   972,   973,   139,   520,
     214,   974,  1669,   136,   588,   275,   277,   611,   331,   215,
     589,  1160,  1130,   622,  1228,   142,   590,   154,   973,   137,
    1138,   276,   974,  1790,  1248,   691,   156,   216,  1249,  1139,
    1140,  1907,   157,   217,  1799,   591,  1229,  1230,  1170,   576,
     592,  1290,   218,   160,  1161,  1293,   103,   104,  1171,   278,
    1305,  1306,  1307,   158,   219,  1931,   159,   332,  1671,   279,
    1672,  1671,  1172,  1141,   161,   521,   220,   277,   162,   591,
     969,   970,   971,   972,   592,   973,   436,   437,   438,   974,
    1954,  1955,   808,  1173,   809,   997,   998,  1308,  1385,   170,
    1847,   999,  1162,   163,  1163,   164,   985,   986,   987,   988,
     989,   990,  1002,  1003,  1286,  1287,  1288,  1289,  1980,   165,
     278,   764,   765,   766,  1201,   124,   125,  1292,   333,   166,
     292,   737,   737,  1212,   172,  1214,  1295,  1296,   810,   811,
     173,  1297,   280,   997,   998,   221,   174,  1174,   167,   999,
    1000,  1302,  1303,  1304,   985,   986,   987,   988,   989,   990,
     991,   992,  2012,   694,   168,   222,   697,   223,   699,   171,
    1175,  1437,  1438,  2017,   175,  1441,  1442,  1443,  1444,   176,
     224,  1448,   178,  1668,  1508,  1509,   985,   986,   987,   988,
     989,   990,  1002,  1003,   969,   970,   971,   972,  1524,   225,
     226,  1285,   179,   280,   715,   716,   717,   718,  1294,  1045,
    1046,  1047,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   496,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,  1089,
    1090,  1091,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   180,   251,   181,   252,
    1450,  1451,  1452,  1453,   182,    -7,     1,   253,   254,   255,
    1124,  1125,  1126,    -7,   133,   134,   256,   257,   258,   969,
     970,   971,   972,   183,  1352,  1243,  1244,  1245,   259,   260,
     261,   262,   263,   184,  1394,   191,   192,    -7,  1496,  1497,
    1498,   264,    -7,   185,   265,  1310,  1311,  1312,  1313,  1314,
    1315,  1316,  1397,   266,   267,   268,   269,   270,   978,  1511,
    1512,  1513,  1382,   186,  1291,   993,    -7,   994,   995,    -7,
    1598,  1599,  1600,    -7,   573,   574,   187,   466,   467,   188,
      -7,   289,    -7,   635,   636,   189,    -7,   496,   497,   498,
     499,   500,   501,   502,   503,   504,   505,   506,   507,   508,
     509,   510,   511,   512,   190,   969,   970,   971,   972,   640,
     641,   145,  1285,   993,   372,   994,   995,   951,   952,  1461,
     954,   955,   985,   986,   987,   988,   989,   990,   991,   992,
    1050,  1346,   359,  1490,  1235,    -7,   997,   998,   978,    -7,
     971,   972,  1328,  1543,   371,   993,  1669,   994,   995,  1331,
    1546,  1004,  1699,  1700,   985,   986,   987,   988,   989,   990,
    1002,  1003,    -7,  1459,   374,   387,   375,    -7,    -7,  1521,
     376,   737,   388,   737,   390,  1469,  1470,  1471,  1523,  1522,
     391,   392,  1525,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,   360,
     361,   362,   363,   364,   365,   393,   395,   396,   397,   399,
      18,    19,    20,    -7,    -7,    -7,     5,   398,   400,   401,
     402,   403,   513,   514,   405,   404,   440,   406,   412,   414,
     415,   416,   474,   419,   420,   421,    -7,   429,   434,   435,
       6,   441,   443,    -7,    -7,    -7,   445,    -7,    -7,    -7,
      -7,    -7,   446,    -7,   448,   450,   451,   452,    -7,    -7,
      -7,   453,   454,   455,   456,    -7,   457,   458,   459,     7,
     460,   461,     8,   462,   463,    -7,     9,    -7,    -7,   484,
     464,   469,   470,    10,   471,    11,   472,   473,   476,    12,
     477,   478,   479,   480,   485,   487,   486,   490,   493,   519,
     536,   537,   538,   575,   545,    -7,    -7,    -7,   539,   561,
     562,   576,   581,   604,   566,   567,  1565,  1757,   583,    -7,
      -7,   600,    -7,   607,   602,    -7,   647,   648,   626,   649,
     650,   651,   652,   969,   970,   971,   972,   653,    13,   654,
     655,   993,    14,   994,   995,   737,  1585,   578,   514,   656,
     659,   660,   662,   661,   663,   670,   676,   671,   673,   674,
      -7,   675,   679,   681,   680,    15,   978,   682,   683,   684,
      16,    17,   689,   993,   711,   994,   995,  1622,   497,   498,
     499,   500,   501,   502,   503,   504,   505,   506,   507,   508,
     509,   510,   511,   512,   497,   498,   499,   500,   501,   502,
     503,   504,   505,   506,   507,   508,   509,   510,   511,   512,
     693,   696,   698,   701,   702,   703,    18,    19,    20,   704,
     705,   706,   707,   915,   708,   916,   917,   918,   919,   920,
     709,   710,   712,  1632,   714,   921,   713,   720,   721,    21,
     724,   725,   757,   726,   731,   732,    22,    23,    24,   733,
      25,    26,    27,    28,    29,   736,    30,   739,   740,   743,
     744,    31,    32,    33,   745,   746,  1668,   747,    34,   985,
     986,   987,   988,   989,   990,   991,   992,   748,    35,   749,
      36,    37,   756,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,   922,
     923,   924,   754,   377,   925,   926,   755,   763,    38,    39,
      40,   768,   769,   785,   774,   775,   777,   778,   779,   780,
     781,   788,    41,    42,   378,    43,   789,   805,    44,   821,
     379,   822,   786,   380,   381,   787,   927,   820,   823,   825,
     827,   829,   928,   824,   838,   826,  1716,   382,   906,   828,
     912,   913,   948,  1005,  1006,  1755,   830,  1007,   831,   832,
    1756,  1008,   841,    45,  1013,  1009,   383,   497,   498,   499,
     500,   501,   502,   503,   504,   505,   506,   507,   508,   509,
     510,   511,   512,  1010,  1011,   842,   845,  1012,  1018,  1025,
    1053,   929,   930,   931,   851,   964,  1026,  1055,  1057,  1059,
    1063,  1069,  1027,  1028,  1029,  1030,  1031,  1032,  1033,  1034,
    1071,  1072,  1035,   932,   933,  1036,   934,  1037,  1075,  1038,
    1076,  1039,  1050,  1060,  1054,  1058,  1061,  1062,   384,   385,
    1065,  1077,  1066,  1068,  1078,  1070,  1079,   846,  1080,  1081,
    1083,  1084,  1087,  1088,  1095,  1096,  1094,  1102,  1104,  1105,
    1106,  1107,  1109,  1460,  1110,  1111,  1112,  1113,  1114,  1115,
    1119,  1120,  1121,  1122,  1123,   935,   936,  1129,  1132,  1133,
     937,   938,   939,   940,   941,   942,  1134,  1135,  1146,  1143,
     969,   970,   971,   972,  1791,  1232,  1181,  1144,   993,  1669,
     994,   995,  1147,  1149,  1150,  1153,  1154,  1155,  1156,  1157,
    1215,  1158,  1159,  1169,  1177,  1178,  1182,  1183,  1184,  1185,
    1186,  1187,  1188,  1189,  1190,  1191,  1192,  1193,  1194,  1205,
    1206,  1217,  1218,  1219,  1220,  1221,  1223,  1224,  1225,  1226,
    1233,  1235,  1238,  1239,  1240,  1241,  1242,   849,  1251,  1253,
    1252,  1716,  1621,  1254,  1255,  1258,  1256,  1257,  1262,  1263,
    1264,  1317,  1322,  1318,  1323,  1848,  1320,  1325,  1327,  1345,
    1328,  1331,  1349,  1348,  1353,  1354,  1357,  1358,  1367,  1368,
    1369,  1371,  1376,  1377,  1379,  1386,  1388,  1406,  1389,  1410,
    1409,  1387,  1380,  1412,  1390,  1393,  1395,  1396,  1398,  1399,
    1400,  1415,  1401,  1430,  1402,  1403,  1407,  1408,  1411,  1413,
    1416,  1433,  1435,  1414,  1417,  1418,  1445,  1419,  1420,  1421,
    1455,  1422,  1423,  1424,  1456,  1457,  1473,  1425,  1474,  1475,
    1426,  1427,  1428,  1476,  1477,  1429,  1483,  1479,  1434,  1439,
    1480,  1440,  1481,  1482,  1485,  1486,  1489,  1492,  1493,  1494,
    1495,  1500,  1501,  1502,  1503,  1504,  1505,  1507,  1515,  1516,
    1506,  1517,  1518,   978,  1520,  1526,  1530,  1529,  1548,  1566,
    1531,  1542,  1532,  1533,  1534,  1535,  1536,  1537,  1545,  1550,
    1552,  1553,  1554,  1555,  1556,  1557,  1559,  1558,  1571,  1560,
    1572,  1561,  1573,  1574,  1579,  1577,  1581,  1575,  1583,  1586,
    1587,  1588,  1591,  1592,  1593,  1597,  1603,  1594,  1625,  1595,
    1635,  1648,  1596,  1605,  1649,  1606,  1650,  1652,  1651,  1607,
    1608,  1654,  1609,  1610,  1611,  1613,  1626,  1655,  1656,  1657,
    1614,  1661,  1615,  1616,  1658,  1617,  1618,  1619,  1620,  1664,
    1623,  1667,  1624,  1633,  1636,  1682,  1676,  1684,  1686,  1679,
    1673,  1687,  1688,  1637,  1698,  1702,  1703,  1689,  1690,  1707,
    1638,  1639,  1694,  1709,  1710,  1711,  1717,  1720,  1722,  1726,
    1727,  1732,  1737,  1739,  1745,  1740,  1741,  1742,  1746,  1744,
    1747,  1748,  1749,  1754,  1758,  1759,  1762,  1764,  1770,  1642,
    1771,  1778,  1781,  1784,  1785,  1772,  1773,  1793,  1713,  1819,
    1787,  1643,  1644,  1645,  1794,  1795,  1797,  1806,  1659,  1807,
    1660,  1665,  1666,  1691,  1683,  1844,  1809,  1810,  1685,  1811,
    1812,  1813,  1815,  1816,  1825,  1830,  1704,  1692,  1834,  1838,
    1705,  1712,  1841,  1719,  1723,  1724,  1842,  1725,  1728,  1738,
    1851,  1850,  1855,  1857,  1753,  1856,  1828,  1766,  1846,  1858,
    1861,  1836,  1864,  1788,  1767,  1865,  1789,  1796,  1798,  1801,
    1866,  1805,  1804,  1870,  1814,  1874,  1817,  1818,  1835,  1876,
    1875,  1881,  1877,  1839,  1853,  1854,  -731,  1884,  -732,  1885,
    1886,  1868,  1889,  1890,  1891,  1892,  1893,  1898,  1896,  1897,
    1895,  1887,  1899,  1900,  1901,  1888,  1904,  1905,  1908,  1894,
    1902,  1913,  1912,  1915,  1918,  1914,  1916,  1917,  1922,  1927,
    1932,  1924,  1926,  1944,  1930,  1940,  1935,  1981,  1941,  1942,
    1943,  1952,  1937,  1953,  1958,  1961,  1964,  1965,  1967,  1966,
    1982,  1984,  1985,  1962,  1986,  1988,  1987,  1991,  1989,  1992,
    1993,  1994,  1998,  1999,  2001,  1995,  2003,  2002,  2005,  2006,
    2010,  2013,  2016,  2018,  2021,  2004,  2020,  2007,  2022,  2023,
    2008,  2009,  2024,  2014,  2033,  2034,  2036,  2015,  2019,  2025,
    2042,  2037,  2038,  2039,  2040,  2043,  2041,  2045,  2044,   482,
    1963,  1777,  2046,  1840,  1832,  1547,  1544,  1936,   522,  1347,
    1602,  1960,  1099,   762,  1216,  1383,  1093,  1273,   753,  1274,
    1463,  1946,  1491,  1462,  1849,  1803,  1653,   370,  1675,  1761,
    1765,     0,  1763,     0,     0,  1344,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1128,     0,     0,     0,     0,     0,     0,  1131,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1017,     0,     0,     0,  1022,     0,
       0,     0,     0,     0,     0,     0,  1049,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1067
};

static const yytype_int16 yycheck[] =
{
     286,   625,   334,  1351,   146,   610,   597,   598,  1208,   374,
     615,   524,   603,   618,   375,  1210,   621,   622,   907,   908,
     152,   617,  1370,     9,    21,    16,    46,    34,   624,    29,
     391,   392,    62,    86,    28,    88,   322,   136,    62,    64,
      10,    62,   172,   154,    62,    98,    37,   172,   334,   645,
     646,   125,    22,    23,    24,   172,   589,   590,   591,   140,
      14,    37,    54,   138,   172,   168,   169,   172,    38,   170,
      21,   235,    20,  1632,    14,   168,   169,   172,    64,   182,
     172,    35,   171,    37,   167,   172,    26,    14,   170,   182,
      30,   172,     0,   172,   170,    55,    56,    37,    92,   172,
      27,   172,    39,   172,   167,   170,    33,  1514,   170,   134,
      37,   172,    37,   133,    37,     6,   170,    44,    45,   170,
     172,    48,   211,     5,   144,   145,    96,   257,   122,    89,
      57,   172,    10,    64,   171,   236,    63,   162,    65,    66,
     131,   103,   112,    70,    71,   170,   106,   109,   172,    76,
     141,   172,    33,   428,   172,   170,   110,   170,   172,   434,
     236,   357,    37,    90,    46,   172,   106,   172,   160,   167,
     110,    54,    50,   133,   172,    37,   113,   104,  1737,   106,
     134,   108,   172,   110,   516,   172,   134,   172,   172,   116,
     117,    72,    22,   120,   121,   481,   190,   172,   484,   126,
     127,   172,   398,   172,   172,   172,   211,   147,   172,   170,
      91,   236,   432,   172,  1562,   172,   172,   142,   213,   146,
     111,   197,   198,   170,    16,   165,   759,   760,   109,   110,
     112,   158,   213,   296,   161,   216,   217,   164,    37,   170,
    1799,   222,   223,   237,   530,    37,   611,   612,   580,   235,
     125,   171,   172,   134,   767,   616,  1663,   622,    16,   212,
      18,   622,    50,   216,   217,  1465,    96,   237,   432,   222,
     223,   354,  1467,  1468,   124,   236,    64,   160,    36,   213,
     203,   216,   217,   172,   235,   172,   167,   222,   223,   236,
      48,   211,    50,    51,   214,   215,   316,   171,   296,   373,
     172,   257,   428,   433,   224,   236,   432,   432,    66,   236,
     436,   189,   387,   299,   389,   432,   433,   271,  1207,   394,
    1209,   432,   172,   250,   432,   433,    84,   432,   433,   432,
      88,   432,   259,   432,   261,   262,   263,   264,   265,   432,
     415,   433,   100,   396,   433,   432,   420,   298,   106,   379,
     432,   432,   433,   432,   324,   295,   432,   432,    59,   353,
     984,   347,   433,   434,   433,   434,   391,   432,   257,   366,
     432,   432,   130,   131,   132,   400,   401,   910,   432,   349,
     432,   432,   402,   141,   384,   377,   378,   249,   250,  1584,
     148,   432,   997,   998,   999,  1000,   356,   172,   325,   326,
     327,   328,   329,   330,   331,   332,   397,   237,     9,   433,
      37,   172,   433,    14,    15,   433,   172,   432,   432,   432,
    1016,   348,   427,   409,   410,  1021,   433,   424,   433,   383,
     416,  1631,    33,   428,    35,  1630,    37,    64,  1786,   172,
     172,   436,   432,   433,    48,   432,   433,   432,   432,   172,
     249,   250,    53,   585,   172,   436,   742,   432,   385,   172,
      64,   432,   172,   432,   432,   432,   393,   172,   432,   350,
      97,    64,   399,   432,   172,   432,   103,  1192,   236,  1194,
     216,   217,   109,   436,   172,    86,   222,   223,    11,    12,
     776,   162,  1381,   427,   428,   429,   430,   432,   172,   170,
     101,   436,   436,    84,    97,    48,   110,   427,   266,   110,
     103,    56,    37,   433,    60,   170,   109,    37,   432,   100,
      46,    64,   436,  1718,   225,   257,   170,   128,   229,    55,
      56,  1879,   432,   134,  1734,   162,    82,    83,     7,    64,
     167,   976,   143,   172,    89,   980,    93,    94,    17,   153,
     232,   233,   234,   170,   155,  1903,   170,   315,  1522,   163,
    1524,  1525,    31,    89,   432,   236,   167,   110,   432,   162,
     427,   428,   429,   430,   167,   432,   292,   293,   294,   436,
    1928,  1929,   105,    52,   107,   216,   217,   269,  1101,   432,
    1790,   222,   137,   170,   139,   170,   216,   217,   218,   219,
     220,   221,   222,   223,   969,   970,   971,   972,  1956,   170,
     153,   170,   171,   172,   900,    93,    94,   978,   376,   170,
     762,   907,   908,   909,   172,   911,   991,   992,   151,   152,
     432,   996,   236,   216,   217,   236,   432,   106,   170,   222,
     223,  1002,  1003,  1004,   216,   217,   218,   219,   220,   221,
     222,   223,  2000,   444,   170,   256,   447,   258,   449,   170,
     129,  1183,  1184,  2011,   432,  1187,  1188,  1189,  1190,   432,
     271,  1193,   432,   213,  1265,  1266,   216,   217,   218,   219,
     220,   221,   222,   223,   427,   428,   429,   430,  1293,   290,
     291,   434,   432,   236,   362,   363,   364,   365,   434,   170,
     171,   172,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   170,
     171,   172,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,   346,   432,   348,   432,   350,
     362,   363,   364,   365,   432,     0,     1,   358,   359,   360,
     170,   171,   172,     8,   318,   319,   367,   368,   369,   427,
     428,   429,   430,   432,  1060,   281,   282,   283,   379,   380,
     381,   382,   383,   170,  1116,    93,    94,    32,   281,   282,
     283,   392,    37,   170,   395,  1006,  1007,  1008,  1009,  1010,
    1011,  1012,  1134,   404,   405,   406,   407,   408,   428,   170,
     171,   172,  1098,   432,   434,   435,    61,   437,   438,    64,
     170,   171,   172,    68,   267,   268,   432,   371,   372,   432,
      75,    29,    77,   249,   250,   432,    81,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   432,   427,   428,   429,   430,   249,
     250,    64,   434,   435,   432,   437,   438,   168,   169,  1201,
     168,   169,   216,   217,   218,   219,   220,   221,   222,   223,
     433,   434,   130,   432,   433,   130,   216,   217,   428,   134,
     429,   430,   433,   434,    67,   435,   436,   437,   438,   433,
     434,  1525,   351,   352,   216,   217,   218,   219,   220,   221,
     222,   223,   157,  1199,   435,   432,   435,   162,   163,  1284,
     435,  1207,   432,  1209,   432,  1211,  1212,  1213,  1293,  1290,
     435,   435,  1293,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   197,
     198,   199,   200,   201,   202,   432,    99,    73,   390,    74,
     208,   209,   210,   208,   209,   210,     8,   157,    85,    78,
     166,   297,   432,   433,    25,   172,   292,   172,   172,   172,
     172,   172,   418,   172,   172,   172,   231,   172,   172,   172,
      32,   172,   172,   238,   239,   240,   172,   242,   243,   244,
     245,   246,   172,   248,   172,   172,   172,   172,   253,   254,
     255,   172,   172,   172,   172,   260,   172,   172,   172,    61,
     172,   172,    64,   172,   172,   270,    68,   272,   273,   433,
     172,   172,   172,    75,   172,    77,   172,   172,   172,    81,
     172,   172,   172,   172,   172,   172,   432,   172,    37,   170,
     134,   170,    61,   432,   172,   300,   301,   302,   170,   170,
     170,    64,    37,    37,   172,   172,  1352,  1672,   170,   314,
     315,   170,   317,   386,   172,   320,   170,   172,   231,   172,
     172,   172,   172,   427,   428,   429,   430,   172,   130,   172,
     172,   435,   134,   437,   438,  1381,  1382,   432,   433,   432,
     432,   432,   170,   432,   432,   432,   170,   432,   432,   432,
     355,   432,   432,   172,   433,   157,   428,   432,   432,   172,
     162,   163,   432,   435,   172,   437,   438,  1459,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     432,   432,   432,   432,   432,   432,   208,   209,   210,   432,
     432,   432,   432,    37,   432,    39,    40,    41,    42,    43,
     432,   432,   432,  1469,   167,    49,   432,   172,   432,   231,
     432,   354,   432,   370,   432,   432,   238,   239,   240,   432,
     242,   243,   244,   245,   246,   432,   248,   172,   414,   432,
     432,   253,   254,   255,   170,   170,   213,   432,   260,   216,
     217,   218,   219,   220,   221,   222,   223,   170,   270,   170,
     272,   273,   432,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   113,
     114,   115,   172,    37,   118,   119,   172,   432,   300,   301,
     302,   170,    37,   432,   172,   172,   172,   172,   172,   172,
     170,   170,   314,   315,    58,   317,    13,   349,   320,   170,
      64,   170,   432,    67,    68,   432,   150,   432,   170,   170,
     170,    13,   156,   432,   170,   432,  1582,    81,   379,   432,
     172,    37,   170,   170,   170,  1670,   432,   170,   432,   432,
    1671,   170,   432,   355,   172,   170,   100,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   170,   170,   432,   432,   170,   172,   170,
     172,   205,   206,   207,   432,   432,   432,   295,   172,   172,
     313,   172,   432,   432,   432,   432,   432,   432,   432,   432,
     172,   167,   433,   227,   228,   432,   230,   432,   172,   432,
     172,   433,   433,   433,   432,   432,   432,   432,   162,   163,
     432,   167,   432,   432,   172,   432,   167,   432,   432,   432,
     172,   167,   432,   432,   172,   432,   170,   432,   432,   432,
     432,   432,   432,   432,    13,   149,   172,   172,   172,   172,
     432,   432,   432,   432,   172,   279,   280,   170,   432,   170,
     284,   285,   286,   287,   288,   289,   172,   172,   170,   172,
     427,   428,   429,   430,  1720,   206,   170,   172,   435,   436,
     437,   438,   172,   172,   172,   172,   172,   172,   172,   172,
     432,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   170,
     172,   148,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   433,   432,   171,   171,   171,   226,   172,   432,   172,
     432,  1777,   432,   172,   432,   432,   172,   172,   432,   170,
     170,   432,   432,   172,   172,  1791,   244,   248,   432,   172,
     433,   433,   172,   434,   432,   432,   172,   432,   167,   134,
     172,   172,   172,   375,   374,   170,   172,   170,   172,   170,
     172,   432,   434,   170,   432,   432,   432,   432,   432,   432,
     432,   170,   432,   170,   432,   432,   432,   432,   432,   432,
     172,   172,    64,   432,   432,   432,    64,   432,   432,   432,
     170,   432,   432,   432,   171,   170,   172,   432,   172,   172,
     432,   432,   432,   172,   170,   432,   170,   172,   432,   432,
     172,   432,   172,   172,   432,   172,   172,   432,   432,   432,
     172,   172,   156,    34,    34,    34,    34,   172,   170,   170,
     202,   170,    37,   428,   212,   171,   432,   170,   421,   370,
     432,   172,   432,   432,   432,   432,   432,   432,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   432,   172,   432,
     172,   432,   172,   172,   167,   422,   172,   370,   172,   432,
     432,   432,    34,   432,   172,   172,   172,   432,    37,   432,
     170,   207,   432,   432,   172,   432,   172,   172,   226,   432,
     432,   172,   432,   432,   432,   432,   413,   172,   172,   172,
     432,   170,   432,   432,   204,   432,   432,   432,   432,   172,
     432,   170,   432,   432,   432,   172,   251,   172,   170,   251,
     235,   172,   235,   432,   172,   172,   172,   370,   370,   172,
     432,   432,   172,   172,   172,   172,   172,   109,   172,   170,
     170,    34,   170,   172,   138,   172,   172,   172,   138,   226,
     138,   138,   172,   213,   172,   172,   172,   172,   172,   432,
     172,   354,   370,   134,   167,   172,   172,    13,   403,   136,
     172,   432,   432,   432,   172,   172,   172,   172,   432,   172,
     432,   432,   432,   417,   432,   134,   172,   172,   432,   172,
     172,   172,   167,   172,   172,   228,   432,   434,   172,   172,
     432,   432,   172,   432,   432,   432,   172,   432,   432,   432,
      13,   172,   172,   159,   432,   172,   416,   434,   388,   172,
     172,   411,   172,   432,   434,   172,   432,   432,   432,   432,
     412,   432,   434,   172,   432,   172,   432,   432,   432,   172,
     370,   172,   134,   432,   432,   432,   432,   172,   432,   138,
     172,   419,   172,   172,   172,    64,    64,   172,   247,   235,
     252,   432,   172,   172,   172,   432,   172,   172,   167,   432,
     228,   432,   170,   138,   432,   172,   172,   172,   172,   370,
     167,   172,   172,   432,   172,   170,   172,   370,   172,   172,
     172,   172,   403,   172,   172,   172,   170,   172,   170,   172,
     167,   172,   134,   432,   432,    25,   172,   124,   172,   172,
     172,   172,   172,   172,   172,   423,   172,   375,   170,   172,
     134,   172,   172,   172,   172,   432,   350,   432,   172,   172,
     432,   432,   172,   432,   172,   172,   172,   432,   432,   432,
     432,   172,   172,   170,   172,   432,   172,   172,   432,   265,
    1939,  1697,   432,  1783,  1776,  1332,  1329,  1910,   293,  1051,
    1431,  1935,   762,   518,   914,  1099,   750,   965,   495,   965,
    1204,  1920,  1236,  1202,  1792,  1739,  1501,   153,  1528,  1675,
    1680,    -1,  1677,    -1,    -1,  1044,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     839,    -1,    -1,    -1,    -1,    -1,    -1,   844,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   637,    -1,    -1,    -1,   642,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   677,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   700
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     1,   440,   445,     0,     8,    32,    61,    64,    68,
      75,    77,    81,   130,   134,   157,   162,   163,   208,   209,
     210,   231,   238,   239,   240,   242,   243,   244,   245,   246,
     248,   253,   254,   255,   260,   270,   272,   273,   300,   301,
     302,   314,   315,   317,   320,   355,   441,   443,   444,   447,
     448,   449,   450,   451,   452,   453,   457,   458,   461,   462,
     556,   558,   560,   561,   588,   590,   591,   609,   610,   616,
     617,   624,   625,   626,   647,   648,   664,   666,   779,   781,
     799,   801,   803,   814,   851,   852,   853,   854,   855,   867,
     879,   880,   881,   882,   883,   884,   885,   886,   782,   172,
     463,   667,   172,    93,    94,   627,   649,   589,   804,   805,
     806,   815,   171,   442,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   171,    93,    94,   800,   802,   172,   172,
     172,   172,   432,   318,   319,   454,    84,   100,   455,   172,
     459,   466,   170,   559,    50,    64,   594,   600,   603,   612,
     619,   652,   670,   785,    37,   446,   170,   432,   170,   170,
     172,   432,   432,   170,   170,   170,   170,   170,   170,   816,
     432,   170,   172,   432,   432,   432,   432,   856,   432,   432,
     432,   432,   432,   432,   170,   170,   432,   432,   432,   432,
     432,    93,    94,   456,   235,   432,    14,    26,    30,    37,
     106,   110,   147,   165,   295,   460,     9,    14,    15,    33,
      35,    37,    53,    86,   101,   110,   128,   134,   143,   155,
     167,   236,   256,   258,   271,   290,   291,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   348,   350,   358,   359,   360,   367,   368,   369,   379,
     380,   381,   382,   383,   392,   395,   404,   405,   406,   407,
     408,   464,   467,    29,   384,    48,    64,   110,   153,   163,
     236,   562,   569,   570,   572,   576,   577,   580,   581,    29,
     593,   604,   600,   601,   605,    37,   125,   611,   613,   614,
      37,   142,   618,   620,   623,    16,    37,   131,   141,   397,
     650,   653,   654,   655,   658,    16,    18,    36,    50,    51,
      66,    84,    88,   100,   106,   130,   131,   132,   141,   148,
     236,   266,   315,   376,   577,   665,   671,   674,   678,   683,
     684,   685,   686,   687,   688,   689,   690,   692,   694,   695,
     697,   698,   699,   747,   748,   749,   756,   758,   759,   130,
     197,   198,   199,   200,   201,   202,   696,   780,   786,   792,
     803,    67,   432,   628,   435,   435,   435,    37,    58,    64,
      67,    68,    81,   100,   162,   163,   817,   432,   432,   868,
     432,   435,   435,   432,   557,    99,    73,   390,   157,    74,
      85,    78,   166,   297,   172,    25,   172,    54,   160,   377,
     378,   544,   172,   465,   172,   172,   172,   124,   172,   172,
     172,   172,    28,    92,   122,   190,   237,   353,   543,   172,
     472,   172,   257,   433,   172,   172,   292,   293,   294,   524,
     292,   172,   478,   172,   479,   172,   172,   481,   172,   482,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   484,   371,   372,   508,   172,
     172,   172,   172,   172,   418,   490,   172,   172,   172,   172,
     172,   493,   508,   468,   433,   172,   432,   172,   578,   582,
     172,   563,   573,    37,   586,   571,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   432,   433,   579,   663,   583,   592,   170,
     170,   236,   560,   595,   596,   597,   602,    33,    72,    91,
     109,   110,   134,   167,   350,   606,   134,   170,    61,   170,
     621,    22,    96,   237,   651,   172,   656,   659,    10,    22,
      23,    24,    38,    96,   112,   237,   324,   349,   679,   757,
     691,   170,   170,   693,   663,   700,   172,   172,    10,    50,
     189,   675,   672,   267,   268,   432,    64,   751,   432,   579,
     663,    37,   668,   170,   738,   703,    37,    64,    97,   103,
     109,   162,   167,   724,   725,   734,   761,   788,   789,   790,
     170,   791,   172,   787,    37,   783,   793,   386,   631,   172,
     211,   427,   433,   810,   171,   211,   433,   812,   211,   214,
     215,   224,   433,   810,   811,   812,   231,   819,   824,   818,
     821,   825,   820,   822,   823,   249,   250,   870,   871,   872,
     249,   250,   858,   859,   860,   812,   812,   170,   172,   172,
     172,   172,   172,   172,   172,   172,   432,   172,   257,   432,
     432,   432,   170,   432,   172,   432,   172,   432,   172,   257,
     432,   432,   470,   432,   432,   432,   170,   535,   536,   432,
     433,   172,   432,   432,   172,   473,   167,   172,   296,   432,
     172,   257,   519,   432,   519,   480,   432,   519,   432,   519,
     483,   432,   432,   432,   432,   432,   432,   432,   432,   432,
     432,   172,   432,   432,   167,   362,   363,   364,   365,   546,
     172,   432,   487,   489,   432,   354,   370,    21,   366,   424,
     498,   432,   432,   432,   172,   432,   432,   663,   731,   172,
     414,   494,   663,   432,   432,   170,   170,   432,   170,   170,
     574,   575,   587,   572,   172,   172,   432,   432,   579,   103,
     109,   584,   594,   432,   170,   171,   172,   598,   170,    37,
     607,   597,    54,   160,   172,   172,   663,   172,   172,   172,
     172,   170,   172,   432,   622,   432,   432,   432,   170,    13,
     168,   169,   182,   432,   657,   660,   357,   398,    46,   133,
     144,   145,   316,   402,   681,   349,    11,    12,   105,   107,
     151,   152,   682,    55,    56,    89,   106,   133,   356,   680,
     432,   170,   170,   170,   432,   170,   432,   170,   432,    13,
     432,   432,   432,   168,   169,   182,   432,   676,   170,   673,
     677,   432,   432,   752,   750,   432,   432,   579,   669,   172,
     432,   432,    14,    27,    33,    37,    44,    45,    57,    63,
      65,    66,    70,    71,    76,    90,   104,   106,   108,   110,
     116,   117,   120,   121,   126,   127,   146,   158,   161,   164,
     236,   250,   259,   261,   262,   263,   264,   265,   325,   326,
     327,   328,   329,   330,   331,   332,   348,   385,   393,   399,
     577,   701,   704,   718,   719,   726,   379,   615,   615,   615,
      62,   615,   172,    37,   728,    37,    39,    40,    41,    42,
      43,    49,   113,   114,   115,   118,   119,   150,   156,   205,
     206,   207,   227,   228,   230,   279,   280,   284,   285,   286,
     287,   288,   289,   760,   762,   766,   767,   775,   170,   738,
     738,   168,   169,   739,   168,   169,   746,   797,   738,   784,
      37,   197,   198,   794,   432,   632,   811,   810,   810,   427,
     428,   429,   430,   432,   436,   807,   811,   812,   428,   807,
     811,   811,   810,   811,   812,   216,   217,   218,   219,   220,
     221,   222,   223,   435,   437,   438,   813,   216,   217,   222,
     223,   807,   222,   223,   813,   170,   170,   170,   170,   170,
     170,   170,   170,   172,   873,    37,   869,   871,   172,   861,
      37,   857,   859,   807,   807,   170,   432,   432,   432,   432,
     432,   432,   432,   432,   432,   433,   432,   432,   432,   433,
     125,   373,   420,   836,   837,   170,   171,   172,   432,   536,
     433,   537,   538,   172,   432,   295,   525,   172,   432,   172,
     433,   432,   432,   313,   523,   432,   432,   523,   432,   172,
     432,   172,   167,   545,   516,   172,   172,   167,   172,   167,
     432,   432,   733,   172,   167,   495,   663,   432,   432,   170,
     171,   172,   432,   575,   170,   172,   432,   615,   615,   595,
     597,   599,   432,   608,   432,   432,   432,   432,   663,   432,
      13,   149,   172,   172,   172,   172,   170,   432,   661,   432,
     432,   432,   432,   172,   170,   171,   172,   432,   677,   170,
      37,   751,   432,   170,   172,   172,     6,   111,    46,    55,
      56,    89,   702,   172,   172,   709,   170,   172,   705,   172,
     172,   706,   707,   172,   172,   172,   172,   172,   172,   172,
      56,    89,   137,   139,   722,     5,    46,   112,   723,   172,
       7,    17,    31,    52,   106,   129,   721,   172,   172,   710,
     708,   170,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   711,   714,   712,   713,   140,
     432,   663,    16,    37,   720,   170,   172,    62,   731,    62,
     731,    62,   663,   615,   663,   432,   725,   148,   172,   172,
     172,   172,   763,   172,   172,   172,   172,   764,    60,    82,
      83,   776,   206,   172,   773,   433,   771,   772,   432,   171,
     171,   171,   226,   281,   282,   283,   768,    59,   225,   229,
     770,   432,   432,   172,   172,   432,   172,   172,   432,    37,
     203,   798,   432,   170,   170,   795,   796,    64,   170,   236,
     391,   400,   401,   558,   609,   629,   633,   634,   635,   636,
     637,   640,   212,   436,   808,   434,   810,   810,   810,   810,
     808,   434,   812,   808,   434,   810,   810,   810,   811,   811,
     811,   811,   812,   812,   812,   232,   233,   234,   269,   826,
     826,   826,   826,   826,   826,   826,   826,   432,   172,   874,
     244,   807,   432,   172,   862,   248,   807,   432,   433,   539,
     540,   433,   541,   542,   838,     9,    64,   235,   299,   347,
     409,   410,   416,   839,   836,   172,   434,   538,   434,   172,
     476,   474,   663,   432,   432,   488,   491,   172,   432,   138,
     387,   389,   394,   415,   432,   517,   518,   167,   134,   172,
     499,   172,   432,   663,   732,   492,   172,   375,   485,   374,
     434,   585,   663,   607,   432,   597,   170,   432,   172,   172,
     432,   136,   432,   432,   579,   432,   432,   579,   432,   432,
     432,   432,   432,   432,   154,   432,   170,   432,   432,   172,
     170,   432,   170,   432,   432,   170,   172,   432,   432,   432,
     432,   432,   432,   432,   432,   432,   432,   432,   432,   432,
     170,   716,   717,   172,   432,    64,   847,   847,   847,   432,
     432,   847,   847,   847,   847,    64,   849,   849,   847,   849,
     362,   363,   364,   365,   715,   170,   171,   170,   432,   663,
     432,   579,   679,   724,   727,   731,   733,   731,   732,   663,
     663,   663,   735,   172,   172,   172,   172,   170,   777,   172,
     172,   172,   172,   170,   778,   432,   172,   172,   432,   172,
     432,   772,   432,   432,   432,   172,   281,   282,   283,   769,
     172,   156,    34,    34,    34,    34,   202,   172,   738,   738,
     641,   170,   171,   172,   638,   170,   170,   170,    37,   630,
     212,   810,   812,   810,   811,   812,   171,   827,   828,   170,
     432,   432,   432,   432,   432,   432,   432,   432,   172,   432,
     172,   432,   172,   434,   540,   172,   434,   542,   421,   833,
     172,   840,   172,   172,   172,   172,   172,   172,   432,   172,
     432,   432,   526,   534,   534,   663,   370,   513,   167,   354,
     509,   172,   172,   172,   172,   370,   534,   422,   553,   167,
     512,   172,   521,   172,   731,   663,   432,   432,   432,   662,
     753,    34,   432,   172,   432,   432,   432,   172,   170,   171,
     172,   432,   717,   172,   848,   432,   432,   432,   432,   432,
     432,   432,   850,   432,   432,   432,   432,   432,   432,   432,
     432,   432,   579,   432,   432,    37,   413,   729,   733,   432,
     732,   732,   663,   432,   736,   170,   432,   432,   432,   432,
     170,   432,   432,   432,   432,   432,   170,   432,   207,   172,
     172,   226,   172,   776,   172,   172,   172,   172,   204,   432,
     432,   170,   637,   639,   172,   432,   432,   170,   213,   436,
     809,   809,   809,   235,   834,   834,   251,   875,   876,   251,
     863,   864,   172,   432,   172,   432,   170,   172,   235,   370,
     370,   417,   434,   534,   172,   432,   432,   520,   172,   351,
     352,   514,   172,   172,   432,   432,   471,   172,   500,   172,
     172,   172,   432,   403,   434,   522,   663,   172,   732,   432,
     109,   755,   172,   432,   432,   432,   170,   170,   432,    20,
     134,   730,    34,   737,   732,   733,   737,   170,   432,   172,
     172,   172,   172,   765,   226,   138,   138,   138,   138,   172,
     642,   432,   637,   432,   213,   810,   812,   811,   172,   172,
     835,   835,   172,   876,   172,   864,   434,   434,   841,   843,
     172,   172,   172,   172,   432,   477,   475,   521,   354,   515,
     510,   370,   506,   507,   134,   167,   503,   172,   432,   432,
     732,   663,   754,    13,   172,   172,   432,   172,   432,   733,
     432,   432,   737,   775,   434,   432,   172,   172,   774,   172,
     172,   172,   172,   172,   432,   167,   172,   432,   432,   136,
     845,    21,   235,   298,   829,   172,   842,   844,   416,   527,
     228,   532,   532,   434,   172,   432,   411,   511,   172,   432,
     506,   172,   172,   534,   134,   486,   388,   733,   663,   755,
     172,    13,   737,   432,   432,   172,   172,   159,   172,   740,
     742,   172,   877,   865,   172,   172,   412,   846,   419,   830,
     172,   167,   296,   529,   172,   370,   172,   134,   505,   501,
     504,   172,   564,   432,   172,   138,   172,   432,   432,   172,
     172,   172,    64,    64,   432,   252,   247,   235,   172,   172,
     172,   172,   228,   533,   172,   172,   554,   534,   167,   550,
     551,   469,   170,   432,   172,   138,   172,   172,   432,   741,
     743,   643,   172,   878,   172,   866,   172,   370,   531,   530,
     172,   534,   167,   555,   502,   172,   550,   403,   496,   497,
     170,   172,   172,   172,   432,   744,   744,   645,   172,   432,
     172,   432,   172,   172,   534,   534,   528,   432,   172,   547,
     553,   172,   432,   496,   170,   172,   172,   170,   745,    14,
      35,    37,   110,   134,   271,   383,   646,   831,   432,   432,
     534,   370,   167,   548,   172,   134,   432,   172,    25,   172,
     644,   124,   172,   172,   172,   423,   832,   432,   172,   172,
     552,   172,   375,   172,   432,   170,   172,   432,   432,   432,
     134,   549,   534,   172,   432,   432,   172,   534,   172,   432,
     350,   172,   172,   172,   172,   432,   565,   566,    86,    88,
      98,   396,   567,   172,   172,   568,   172,   172,   172,   170,
     172,   172,   432,   432,   432,   172,   432
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1464 of yacc.c  */
#line 532 "lef.y"
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
      ;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 589 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 590 "lef.y"
    { 
         versionNum = convert_name2num((yyvsp[(3) - (4)].string));
         if (versionNum > 5.8) {
            char temp[120];
            sprintf(temp,
               "Lef parser 5.8 does not support lef file with version %s. Parser will stop processing.", (yyvsp[(3) - (4)].string));
            lefError(1503, temp);
            return 1;
         }
/*
         versionNum = $3;         Save the version number for future use */
         if (lefrVersionStrCbk) {
            CALLBACK(lefrVersionStrCbk, lefrVersionStrCbkType, (yyvsp[(3) - (4)].string));
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
      ;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 626 "lef.y"
    {
        if (lefrDividerCharCbk) {
          if (strcmp((yyvsp[(2) - (3)].string), "") != 0) {
             CALLBACK(lefrDividerCharCbk, lefrDividerCharCbkType, (yyvsp[(2) - (3)].string));
          } else {
             CALLBACK(lefrDividerCharCbk, lefrDividerCharCbkType, "/");
             lefWarning(2005, "DIVIDERCHAR has an invalid null value. Value is set to default /");
          }
        }
        hasDivChar = 1;
      ;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 639 "lef.y"
    {
        if (lefrBusBitCharsCbk) {
          if (strcmp((yyvsp[(2) - (3)].string), "") != 0) {
             CALLBACK(lefrBusBitCharsCbk, lefrBusBitCharsCbkType, (yyvsp[(2) - (3)].string)); 
          } else {
             CALLBACK(lefrBusBitCharsCbk, lefrBusBitCharsCbkType, "[]"); 
             lefWarning(2006, "BUSBITCHAR has an invalid null value. Value is set to default []");
          }
        }
        hasBusBit = 1;
      ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 654 "lef.y"
    { ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 657 "lef.y"
    {
        if (versionNum >= 5.6) {
           doneLib = 1;
           ge56done = 1;
        }
      ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 664 "lef.y"
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
      ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 707 "lef.y"
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
                   //lefWarning(2007, "NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
	  ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 720 "lef.y"
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
	  ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 738 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrNoWireExtensionCbk)
          CALLBACK(lefrNoWireExtensionCbk, lefrNoWireExtensionCbkType, "ON");
      } else
        if (lefrNoWireExtensionCbk) /* write warning only if cbk is set */
           if (noWireExtensionWarnings++ < lefrNoWireExtensionWarnings)
             lefWarning(2008, "NOWIREEXTENSIONATPIN statement is obsolete in version 5.6 or later.\nThe NOWIREEXTENSIONATPIN statement will be ignored.");
    ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 748 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrNoWireExtensionCbk)
          CALLBACK(lefrNoWireExtensionCbk, lefrNoWireExtensionCbkType, "OFF");
      } else
        if (lefrNoWireExtensionCbk) /* write warning only if cbk is set */
           if (noWireExtensionWarnings++ < lefrNoWireExtensionWarnings)
             lefWarning(2008, "NOWIREEXTENSIONATPIN statement is obsolete in version 5.6 or later.\nThe NOWIREEXTENSIONATPIN statement will be ignored.");
    ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 759 "lef.y"
    { 
       if (versionNum >= 5.8) {
       
          if (lefrFixedMaskCbk) {
            lefFixedMask = 1;
            CALLBACK(lefrFixedMaskCbk, lefrFixedMaskCbkType, lefFixedMask);
          }
          
          hasFixedMask = 1;
       }
    ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 772 "lef.y"
    {
      if (lefrManufacturingCbk)
        CALLBACK(lefrManufacturingCbk, lefrManufacturingCbkType, (yyvsp[(2) - (3)].dval));
      hasManufactur = 1;
    ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 779 "lef.y"
    {
    if ((strcmp((yyvsp[(2) - (4)].string), "PIN") == 0) && (versionNum >= 5.6)) {
      if (lefrUseMinSpacingCbk) /* write warning only if cbk is set */
         if (useMinSpacingWarnings++ < lefrUseMinSpacingWarnings)
            lefWarning(2009, "USEMINSPACING PIN statement is obsolete in version 5.6 or later.\n The USEMINSPACING PIN statement will be ignored.");
    } else {
        if (lefrUseMinSpacingCbk) {
          lefrUseMinSpacing.lefiUseMinSpacing::set((yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].integer));
          CALLBACK(lefrUseMinSpacingCbk, lefrUseMinSpacingCbkType,
                   &lefrUseMinSpacing);
      }
    }
  ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 794 "lef.y"
    { CALLBACK(lefrClearanceMeasureCbk, lefrClearanceMeasureCbkType, (yyvsp[(2) - (3)].string)); ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 797 "lef.y"
    {(yyval.string) = (char*)"MAXXY";;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 798 "lef.y"
    {(yyval.string) = (char*)"EUCLIDEAN";;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 801 "lef.y"
    {(yyval.string) = (char*)"OBS";;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 802 "lef.y"
    {(yyval.string) = (char*)"PIN";;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 805 "lef.y"
    {(yyval.integer) = 1;;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 806 "lef.y"
    {(yyval.integer) = 0;;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 809 "lef.y"
    { 
      if (lefrUnitsCbk)
        CALLBACK(lefrUnitsCbk, lefrUnitsCbkType, &lefrUnits);
    ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 815 "lef.y"
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
    ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 842 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setTime((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 844 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setCapacitance((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 846 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setResistance((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 848 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setPower((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 850 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setCurrent((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 852 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setVoltage((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 854 "lef.y"
    { 
      if(validNum((int)(yyvsp[(3) - (4)].dval))) {
         if (lefrUnitsCbk)
            lefrUnits.lefiUnits::setDatabase("MICRONS", (yyvsp[(3) - (4)].dval));
      }
    ;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 861 "lef.y"
    { if (lefrUnitsCbk) lefrUnits.lefiUnits::setFrequency((yyvsp[(3) - (4)].dval)); ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 865 "lef.y"
    { 
      if (lefrLayerCbk)
        CALLBACK(lefrLayerCbk, lefrLayerCbkType, &lefrLayer);
    ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 870 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 871 "lef.y"
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
        lefrLayer.lefiLayer::setName((yyvsp[(3) - (3)].string));
      useLenThr = 0;
      layerCut = 0;
      layerMastOver = 0;
      layerRout = 0;
      layerDir = 0;
      lefrHasLayer = 1;
      //strcpy(layerName, $3);
      layerName = strdup((yyvsp[(3) - (3)].string));
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
    ;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 902 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 903 "lef.y"
    { 
      if (strcmp(layerName, (yyvsp[(3) - (3)].string)) != 0) {
        if (lefrLayerCbk) { /* write error only if cbk is set */
          if (layerWarnings++ < lefrLayerWarnings) {
             outMsg = (char*)lefMalloc(10000);
             sprintf (outMsg,
                "END LAYER name %s is different from the LAYER name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), layerName);
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
              sprintf (outMsg, "The DIRECTION statement which is required in a LAYER with TYPE ROUTING is not defined in LAYER %s.\nUpdate your lef file and add the DIRECTION statement for layer %s.", (yyvsp[(3) - (3)].string), (yyvsp[(3) - (3)].string));
              lefError(1511, outMsg);
              lefFree(outMsg);
              CHKERR(); 
            }
          }
        }
      }
    ;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 961 "lef.y"
    { ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 966 "lef.y"
    { ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 970 "lef.y"
    {
       /* let setArraySpacingCutSpacing to set the data */
    ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 976 "lef.y"
    {
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::setArraySpacingCut((yyvsp[(6) - (10)].dval));
         lefrLayer.lefiLayer::addArraySpacingArray((int)(yyvsp[(8) - (10)].dval), (yyvsp[(10) - (10)].dval));
         arrayCutsVal = (int)(yyvsp[(8) - (10)].dval);  /* set the value */
         arrayCutsWar = 0;
      }
    ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 985 "lef.y"
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "ARRAYSPACING is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1685, outMsg);
         lefFree(outMsg);
         CHKERR();
      }
    ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 996 "lef.y"
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::setType((yyvsp[(2) - (3)].string));
      hasType = 1;
    ;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 1002 "lef.y"
    {
      if (versionNum < 5.8) {
          if (layerWarnings++ < lefrViaWarnings) {
              lefError(2081, "MASK information can only be defined with version 5.8");
              CHKERR(); 
          }           
      } else {
          if (lefrLayerCbk) {
            lefrLayer.lefiLayer::setMask((int)(yyvsp[(2) - (3)].dval));
          }
          
          hasMask = 1;
      }
    ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 1017 "lef.y"
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setPitch((yyvsp[(2) - (3)].dval));
      hasPitch = 1;  
    ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 1022 "lef.y"
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setPitchXY((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
      hasPitch = 1;  
    ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 1027 "lef.y"
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagPitch((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 1031 "lef.y"
    { 
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagPitchXY((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 1035 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setOffset((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 1039 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setOffsetXY((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 1043 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagWidth((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 1047 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDiagSpacing((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 1051 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setWidth((yyvsp[(2) - (3)].dval));
      hasWidth = 1;  
    ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 1056 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::setArea((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 1060 "lef.y"
    {
      hasSpCenter = 0;       /* reset to 0, only once per spacing is allowed */
      hasSpSamenet = 0;
      hasSpParallel = 0;
      hasSpLayer = 0;
      layerCutSpacing = (yyvsp[(2) - (2)].dval);  /* for error message purpose */
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
           lefrLayer.lefiLayer::setSpacingMin((yyvsp[(2) - (2)].dval));
        lefrHasSpacing = 1;
      } else { 
        if (lefrLayerCbk)
           lefrLayer.lefiLayer::setSpacingMin((yyvsp[(2) - (2)].dval));
      }
    ;}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 1096 "lef.y"
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::setSpacingTableOrtho();
      if (lefrLayerCbk) /* due to converting to C, else, convertor produce */
         lefrLayer.lefiLayer::addSpacingTableOrthoWithin((yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));/*bad code*/
    ;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 1103 "lef.y"
    {
      if (versionNum < 5.7) {
         outMsg = (char*)lefMalloc(10000);
         sprintf(outMsg,
           "SPACINGTABLE ORTHOGONAL is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
         lefError(1694, outMsg);
         lefFree(outMsg);
         CHKERR();
      }
    ;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 1114 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDirection((yyvsp[(2) - (3)].string));
      hasDirection = 1;  
    ;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 1128 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1514, "RESISTANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setResistance((yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 1140 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1515, "RESISTANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
    ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 1151 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1516, "CAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setCapacitance((yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 1163 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1517, "CAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
    ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 1174 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1518, "HEIGHT statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setHeight((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 1186 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1519, "WIREEXTENSION statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setWireExtension((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 1198 "lef.y"
    {
      if (!layerRout && (layerCut || layerMastOver)) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1520, "THICKNESS statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setThickness((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 1210 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1521, "SHRINKAGE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setShrinkage((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 1222 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1522, "CAPMULTIPLIER statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setCapMultiplier((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 1234 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1523, "EDGECAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::setEdgeCap((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 1259 "lef.y"
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

      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaLength((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 1286 "lef.y"
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setCurrentDensity((yyvsp[(2) - (3)].dval));
      } else {
         if (layerWarnings++ < lefrLayerWarnings) {
            lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            CHKERR();
         }
      }
    ;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 1305 "lef.y"
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
    ;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 1323 "lef.y"
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setCurrentPoint((yyvsp[(3) - (6)].dval), (yyvsp[(4) - (6)].dval));
      } else {
         if (layerWarnings++ < lefrLayerWarnings) {
            lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            CHKERR();
         }
      }
    ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 1341 "lef.y"
    { lefDumbMode = 10000000; lefRealNum = 1; lefInProp = 1; ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 1342 "lef.y"
    {
      lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    ;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 1348 "lef.y"
    {
      if (layerMastOver) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1527, "ACCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser will stop processing.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addAccurrentDensity((yyvsp[(2) - (2)].string));
    ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 1361 "lef.y"
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
           lefrLayer.lefiLayer::addAccurrentDensity((yyvsp[(2) - (4)].string));
           lefrLayer.lefiLayer::setAcOneEntry((yyvsp[(3) - (4)].dval));
      }
    ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 1376 "lef.y"
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
         lefrLayer.lefiLayer::setDcOneEntry((yyvsp[(3) - (4)].dval));
      }
    ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 1391 "lef.y"
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
         lefrLayer.lefiLayer::addNumber((yyvsp[(4) - (4)].dval));
      }
    ;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 1414 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addDcCutarea(); ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 1417 "lef.y"
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
         lefrLayer.lefiLayer::addNumber((yyvsp[(4) - (4)].dval));
      }
    ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 1440 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addDcWidth(); ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 1445 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaAreaRatio((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 1483 "lef.y"
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
    ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 1522 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaCumAreaRatio((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 1560 "lef.y"
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
    ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 1599 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaAreaFactor((yyvsp[(2) - (2)].dval));
      antennaType = lefiAntennaAF;
    ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 1614 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaSideAreaRatio((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 1652 "lef.y"
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
    ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 1691 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaCumSideAreaRatio((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 1729 "lef.y"
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
    ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 1768 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaSideAreaFactor((yyvsp[(2) - (2)].dval));
      antennaType = lefiAntennaSAF;
    ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 1808 "lef.y"
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
    ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 1847 "lef.y"
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
    ;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 1868 "lef.y"
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaGatePlusDiff((yyvsp[(2) - (3)].dval));
      }
    ;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 1889 "lef.y"
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setAntennaAreaMinusDiff((yyvsp[(2) - (3)].dval));
      }
    ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 1910 "lef.y"
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
         lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL((yyvsp[(3) - (4)].pt).x, (yyvsp[(3) - (4)].pt).y);
         lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL((yyvsp[(4) - (4)].pt).x, (yyvsp[(4) - (4)].pt).y);
      }
    ;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 1927 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setAntennaPWL(lefiAntennaADR, lefrAntennaPWLPtr);
    ;}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 1931 "lef.y"
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "ANTENNAAREADIFFREDUCEPWL is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1689, outMsg);
        lefFree(outMsg);
        CHKERR();
      }
    ;}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 1942 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireWidth((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireWidth((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 1966 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireLength((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWireLength((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 1990 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWidth((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotWidth((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 2014 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotLength((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setSlotLength((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 2038 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxAdjacentSlotSpacing((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxAdjacentSlotSpacing((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 2062 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxCoaxialSlotSpacing((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxCoaxialSlotSpacing((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 2086 "lef.y"
    { /* 5.4 syntax */
      if (ignoreVersion) {
         /* do nothing */
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxEdgeSlotSpacing((yyvsp[(2) - (3)].dval));
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
         if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxEdgeSlotSpacing((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 2110 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setSplitWireWidth((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 2133 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMinimumDensity((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 2151 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMaximumDensity((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 2169 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDensityCheckWindow((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 2187 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setDensityCheckStep((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 2205 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setFillActiveSpacing((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 2223 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMaxwidth((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 2248 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setMinwidth((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 2273 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::addMinenclosedarea((yyvsp[(2) - (2)].dval));
    ;}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 2290 "lef.y"
    { /* pcr 409334 */
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addMinimumcut((int)(yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval)); 
      hasLayerMincut = 0;
    ;}
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 2298 "lef.y"
    {
      if (!hasLayerMincut) {   /* FROMABOVE nor FROMBELOW is set */
         if (lefrLayerCbk)
             lefrLayer.lefiLayer::addMinimumcutConnect((char*)"");
      }
    ;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 2305 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstep((yyvsp[(2) - (2)].dval));
    ;}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 2309 "lef.y"
    {
    ;}
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 2312 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::setProtrusion((yyvsp[(2) - (7)].dval), (yyvsp[(4) - (7)].dval), (yyvsp[(6) - (7)].dval));
    ;}
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 2328 "lef.y"
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
    ;}
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 2354 "lef.y"
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
         lefrLayer.lefiLayer::addEnclosure((yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].dval));
    ;}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 2373 "lef.y"
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
         lefrLayer.lefiLayer::addPreferEnclosure((yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].dval));
    ;}
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 2391 "lef.y"
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
            lefrLayer.lefiLayer::setResPerCut((yyvsp[(2) - (3)].dval));
      }
    ;}
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 2409 "lef.y"
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
            lefrLayer.lefiLayer::setDiagMinEdgeLength((yyvsp[(2) - (3)].dval));
      }
    ;}
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 2434 "lef.y"
    {
      // Use the polygon code to retrieve the points for MINSIZE
      lefrGeometriesPtr = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
      lefrGeometriesPtr->lefiGeometries::Init();
      lefrDoGeometries = 1;
    ;}
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 2441 "lef.y"
    {
      if (lefrLayerCbk) {
         lefrGeometriesPtr->lefiGeometries::addPolygon();
         lefrLayer.lefiLayer::setMinSize(lefrGeometriesPtr);
      }
     lefrDoGeometries = 0;
      lefrGeometriesPtr->lefiGeometries::Destroy();
                                       // Don't need the object anymore
      lefFree(lefrGeometriesPtr);
    ;}
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 2455 "lef.y"
    {
        if (lefrLayerCbk)
           lefrLayer.lefiLayer::setArraySpacingLongArray();
    ;}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 2463 "lef.y"
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::setArraySpacingWidth((yyvsp[(2) - (2)].dval));
    ;}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 2474 "lef.y"
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addArraySpacingArray((int)(yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval));
         if (arrayCutsVal > (int)(yyvsp[(2) - (4)].dval)) {
            /* Mulitiple ARRAYCUTS value needs to me in ascending order */
            if (!arrayCutsWar) {
               if (layerWarnings++ < lefrLayerWarnings)
                  lefWarning(2080, "The number of cut values in multiple ARRAYSPACING ARRAYCUTS are not in increasing order.\nTo be consistent with the documentation, update the cut values to increasing order.");
               arrayCutsWar = 1;
            }
         }
         arrayCutsVal = (int)(yyvsp[(2) - (4)].dval);
    ;}
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 2490 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(2) - (2)].dval));
      hasParallel = 1;
    ;}
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 2511 "lef.y"
    {
      spParallelLength = lefrLayer.lefiLayer::getNumber();
      if (lefrLayerCbk) lefrLayer.lefiLayer::addSpParallelLength();
    ;}
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 2516 "lef.y"
    { 
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addSpParallelWidth((yyvsp[(7) - (7)].dval));
      }
    ;}
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 2522 "lef.y"
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
    ;}
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 2536 "lef.y"
    {
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(5) - (5)].dval));
    ;}
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 2540 "lef.y"
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
      if (lefrLayerCbk) lefrLayer.lefiLayer::addSpTwoWidths((yyvsp[(3) - (7)].dval), (yyvsp[(4) - (7)].dval));
      hasTwoWidths = 1;
    ;}
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 2561 "lef.y"
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "TWOWIDTHS is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1697, outMsg);
        lefFree(outMsg);
        CHKERR();
      } 
    ;}
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 2572 "lef.y"
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
         lefrLayer.lefiLayer::addSpInfluence((yyvsp[(3) - (7)].dval), (yyvsp[(5) - (7)].dval), (yyvsp[(7) - (7)].dval));
      }
    ;}
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 2602 "lef.y"
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addSpacingTableOrthoWithin((yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval));
  ;}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 2608 "lef.y"
    {(yyval.string) = (char*)"NULL";;}
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 2609 "lef.y"
    {(yyval.string) = (char*)"ABOVE";;}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 2610 "lef.y"
    {(yyval.string) = (char*)"BELOW";;}
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 2614 "lef.y"
    {
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addEnclosureWidth((yyvsp[(2) - (2)].dval));
      }
    ;}
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 2621 "lef.y"
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
            lefrLayer.lefiLayer::addEnclosureLength((yyvsp[(2) - (2)].dval));
         }
      }
    ;}
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 2638 "lef.y"
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
            lefrLayer.lefiLayer::addEnclosureExceptEC((yyvsp[(2) - (2)].dval));
         }
      }
    ;}
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 2655 "lef.y"
    {
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addPreferEnclosureWidth((yyvsp[(2) - (2)].dval));
      }
    ;}
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 2663 "lef.y"
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
            lefrLayer.lefiLayer::addMinimumcutWithin((yyvsp[(2) - (2)].dval));
         }
      }
    ;}
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 2680 "lef.y"
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

    ;}
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 2699 "lef.y"
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
    ;}
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 2719 "lef.y"
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
         lefrLayer.lefiLayer::addMinimumcutLengDis((yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval));
    ;}
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 2744 "lef.y"
    {
    if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstepType((yyvsp[(1) - (1)].string));
  ;}
    break;

  case 227:

/* Line 1464 of yacc.c  */
#line 2748 "lef.y"
    {
    if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstepLengthsum((yyvsp[(2) - (2)].dval));
  ;}
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 2752 "lef.y"
    {
    if (versionNum < 5.7) {
      outMsg = (char*)lefMalloc(10000);
      sprintf(outMsg,
        "MAXEDGES is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
      lefError(1710, outMsg);
      lefFree(outMsg);
      CHKERR();
    } else
       if (lefrLayerCbk) lefrLayer.lefiLayer::addMinstepMaxedges((int)(yyvsp[(2) - (2)].dval));
  ;}
    break;

  case 229:

/* Line 1464 of yacc.c  */
#line 2765 "lef.y"
    {(yyval.string) = (char*)"INSIDECORNER";;}
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 2766 "lef.y"
    {(yyval.string) = (char*)"OUTSIDECORNER";;}
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 2767 "lef.y"
    {(yyval.string) = (char*)"STEP";;}
    break;

  case 232:

/* Line 1464 of yacc.c  */
#line 2771 "lef.y"
    { if (lefrLayerCbk)
          lefrLayer.lefiLayer::setAntennaValue(antennaType, (yyvsp[(1) - (1)].dval)); ;}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 2774 "lef.y"
    { if (lefrLayerCbk) { /* require min 2 points, set the 1st 2 */
          lefrAntennaPWLPtr = (lefiAntennaPWL*)lefMalloc(sizeof(lefiAntennaPWL));
          lefrAntennaPWLPtr->lefiAntennaPWL::Init();
          lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL((yyvsp[(3) - (4)].pt).x, (yyvsp[(3) - (4)].pt).y);
          lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL((yyvsp[(4) - (4)].pt).x, (yyvsp[(4) - (4)].pt).y);
        }
      ;}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 2782 "lef.y"
    { if (lefrLayerCbk)
          lefrLayer.lefiLayer::setAntennaPWL(antennaType, lefrAntennaPWLPtr);
      ;}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 2795 "lef.y"
    { if (lefrLayerCbk)
      lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL((yyvsp[(1) - (1)].pt).x, (yyvsp[(1) - (1)].pt).y);
  ;}
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 2801 "lef.y"
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
      ;}
    break;

  case 240:

/* Line 1464 of yacc.c  */
#line 2834 "lef.y"
    {(yyval.string) = (char*)"PEAK";;}
    break;

  case 241:

/* Line 1464 of yacc.c  */
#line 2835 "lef.y"
    {(yyval.string) = (char*)"AVERAGE";;}
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 2836 "lef.y"
    {(yyval.string) = (char*)"RMS";;}
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 2840 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 2842 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcFrequency(); ;}
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 2845 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(9) - (9)].dval)); ;}
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 2847 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcTableEntry(); ;}
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 2851 "lef.y"
    {
      if (!layerCut) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1600, "CUTAREA statement can only be defined in LAYER with TYPE CUT.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(2) - (2)].dval));
    ;}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 2863 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcCutarea(); ;}
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 2865 "lef.y"
    {
      if (!layerRout) {
         if (lefrLayerCbk) { /* write error only if cbk is set */
            if (layerWarnings++ < lefrLayerWarnings) {
              lefError(1601, "WIDTH can only be defined in LAYER with TYPE ROUTING.");
              CHKERR();
            }
         }
      }
      if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(2) - (2)].dval));
    ;}
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 2877 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addAcWidth(); ;}
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 2881 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 2883 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addDcTableEntry(); ;}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 2887 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 2896 "lef.y"
    {
      if (lefrLayerCbk) {
        char propTp;
        propTp = lefrLayerProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
        lefrLayer.lefiLayer::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 2904 "lef.y"
    {
      if (lefrLayerCbk) {
        char propTp;
        propTp = lefrLayerProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
        lefrLayer.lefiLayer::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 2912 "lef.y"
    {
      char temp[32];
      sprintf(temp, "%.11g", (yyvsp[(2) - (2)].dval));
      if (lefrLayerCbk) {
        char propTp;
        propTp = lefrLayerProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
        lefrLayer.lefiLayer::addNumProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), temp, propTp);
      }
    ;}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 2924 "lef.y"
    { ;}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 2926 "lef.y"
    { ;}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 2929 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::setCurrentPoint((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval)); ;}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 2937 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::setCapacitancePoint((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval)); ;}
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 2942 "lef.y"
    { ;}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 2945 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::setResistancePoint((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval)); ;}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 2948 "lef.y"
    {(yyval.string) = (char*)"ROUTING"; layerRout = 1;;}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 2949 "lef.y"
    {(yyval.string) = (char*)"CUT"; layerCut = 1;;}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 2950 "lef.y"
    {(yyval.string) = (char*)"OVERLAP"; layerMastOver = 1;;}
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 2951 "lef.y"
    {(yyval.string) = (char*)"MASTERSLICE"; layerMastOver = 1;;}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 2952 "lef.y"
    {(yyval.string) = (char*)"VIRTUAL";;}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 2953 "lef.y"
    {(yyval.string) = (char*)"IMPLANT";;}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 2956 "lef.y"
    {(yyval.string) = (char*)"HORIZONTAL";;}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 2957 "lef.y"
    {(yyval.string) = (char*)"VERTICAL";;}
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 2958 "lef.y"
    {(yyval.string) = (char*)"DIAG45";;}
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 2959 "lef.y"
    {(yyval.string) = (char*)"DIAG135";;}
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 2963 "lef.y"
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addMinenclosedareaWidth((yyvsp[(2) - (2)].dval));
    ;}
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 2970 "lef.y"
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(1);
    ;}
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 2975 "lef.y"
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(2);
    ;}
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 2980 "lef.y"
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(3);
    ;}
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 2985 "lef.y"
    {
    if (lefrLayerCbk)
       lefrLayer.lefiLayer::addAntennaModel(4);
    ;}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 2991 "lef.y"
    { ;}
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 2996 "lef.y"
    { ;}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 2999 "lef.y"
    { 
      if (lefrLayerCbk) {
         lefrLayer.lefiLayer::addSpParallelWidth((yyvsp[(2) - (2)].dval));
      }
    ;}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 3005 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addSpParallelWidthSpacing(); ;}
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 3008 "lef.y"
    { ;}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 3010 "lef.y"
    { ;}
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 3024 "lef.y"
    {
       if (lefrLayerCbk) lefrLayer.lefiLayer::addNumber((yyvsp[(4) - (4)].dval));
    ;}
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 3028 "lef.y"
    {
      if (lefrLayerCbk)
         lefrLayer.lefiLayer::addSpTwoWidths((yyvsp[(2) - (6)].dval), (yyvsp[(3) - (6)].dval));
    ;}
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 3034 "lef.y"
    { (yyval.dval) = -1; ;}
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 3036 "lef.y"
    { (yyval.dval) = (yyvsp[(2) - (2)].dval); ;}
    break;

  case 296:

/* Line 1464 of yacc.c  */
#line 3039 "lef.y"
    { ;}
    break;

  case 297:

/* Line 1464 of yacc.c  */
#line 3041 "lef.y"
    { ;}
    break;

  case 298:

/* Line 1464 of yacc.c  */
#line 3044 "lef.y"
    { if (lefrLayerCbk) lefrLayer.lefiLayer::addSpInfluence((yyvsp[(2) - (6)].dval), (yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval)); ;}
    break;

  case 299:

/* Line 1464 of yacc.c  */
#line 3047 "lef.y"
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
           lefrMaxStackVia.lefiMaxStackVia::setMaxStackVia((int)(yyvsp[(2) - (3)].dval));
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
    ;}
    break;

  case 300:

/* Line 1464 of yacc.c  */
#line 3082 "lef.y"
    {lefDumbMode = 2; lefNoNum= 2;;}
    break;

  case 301:

/* Line 1464 of yacc.c  */
#line 3084 "lef.y"
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
           lefrMaxStackVia.lefiMaxStackVia::setMaxStackVia((int)(yyvsp[(2) - (7)].dval));
           lefrMaxStackVia.lefiMaxStackVia::setMaxStackViaRange((yyvsp[(5) - (7)].string), (yyvsp[(6) - (7)].string));
           CALLBACK(lefrMaxStackViaCbk, lefrMaxStackViaCbkType, &lefrMaxStackVia);
        }
      }
      lefrHasMaxVS = 1;
    ;}
    break;

  case 302:

/* Line 1464 of yacc.c  */
#line 3109 "lef.y"
    { hasViaRule_layer = 0; ;}
    break;

  case 303:

/* Line 1464 of yacc.c  */
#line 3110 "lef.y"
    { 
      if (lefrViaCbk)
        CALLBACK(lefrViaCbk, lefrViaCbkType, &lefrVia);
    ;}
    break;

  case 304:

/* Line 1464 of yacc.c  */
#line 3116 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 305:

/* Line 1464 of yacc.c  */
#line 3119 "lef.y"
    {
      /* 0 is nodefault */
      if (lefrViaCbk) lefrVia.lefiVia::setName((yyvsp[(2) - (2)].string), 0);
      viaLayer = 0;
      numVia++;
      //strcpy(viaName, $2);
      viaName = strdup((yyvsp[(2) - (2)].string));
    ;}
    break;

  case 306:

/* Line 1464 of yacc.c  */
#line 3128 "lef.y"
    {
      /* 1 is default */
      if (lefrViaCbk) lefrVia.lefiVia::setName((yyvsp[(2) - (3)].string), 1);
      viaLayer = 0;
      //strcpy(viaName, $2);
      viaName = strdup((yyvsp[(2) - (3)].string));
    ;}
    break;

  case 307:

/* Line 1464 of yacc.c  */
#line 3136 "lef.y"
    {
      /* 2 is generated */
      if (lefrViaCbk) lefrVia.lefiVia::setName((yyvsp[(2) - (3)].string), 2);
      viaLayer = 0;
      //strcpy(viaName, $2);
      viaName = strdup((yyvsp[(2) - (3)].string));
    ;}
    break;

  case 308:

/* Line 1464 of yacc.c  */
#line 3144 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 309:

/* Line 1464 of yacc.c  */
#line 3146 "lef.y"
    {lefDumbMode = 3; lefNoNum = 1; ;}
    break;

  case 310:

/* Line 1464 of yacc.c  */
#line 3149 "lef.y"
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
          if (lefrViaCbk) lefrVia.lefiVia::setViaRule((yyvsp[(3) - (24)].string), (yyvsp[(6) - (24)].dval), (yyvsp[(7) - (24)].dval), (yyvsp[(11) - (24)].string), (yyvsp[(12) - (24)].string), (yyvsp[(13) - (24)].string),
                          (yyvsp[(16) - (24)].dval), (yyvsp[(17) - (24)].dval), (yyvsp[(20) - (24)].dval), (yyvsp[(21) - (24)].dval), (yyvsp[(22) - (24)].dval), (yyvsp[(23) - (24)].dval));
       viaLayer++;
       hasViaRule_layer = 1;
    ;}
    break;

  case 314:

/* Line 1464 of yacc.c  */
#line 3175 "lef.y"
    {
       if (lefrViaCbk) lefrVia.lefiVia::setRowCol((int)(yyvsp[(2) - (4)].dval), (int)(yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 315:

/* Line 1464 of yacc.c  */
#line 3179 "lef.y"
    {
       if (lefrViaCbk) lefrVia.lefiVia::setOrigin((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 316:

/* Line 1464 of yacc.c  */
#line 3183 "lef.y"
    {
       if (lefrViaCbk) lefrVia.lefiVia::setOffset((yyvsp[(2) - (6)].dval), (yyvsp[(3) - (6)].dval), (yyvsp[(4) - (6)].dval), (yyvsp[(5) - (6)].dval));
    ;}
    break;

  case 317:

/* Line 1464 of yacc.c  */
#line 3186 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 318:

/* Line 1464 of yacc.c  */
#line 3187 "lef.y"
    {
       if (lefrViaCbk) lefrVia.lefiVia::setPattern((yyvsp[(3) - (4)].string));
    ;}
    break;

  case 324:

/* Line 1464 of yacc.c  */
#line 3204 "lef.y"
    { ;}
    break;

  case 325:

/* Line 1464 of yacc.c  */
#line 3206 "lef.y"
    { ;}
    break;

  case 326:

/* Line 1464 of yacc.c  */
#line 3208 "lef.y"
    { if (lefrViaCbk) lefrVia.lefiVia::setResistance((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 327:

/* Line 1464 of yacc.c  */
#line 3209 "lef.y"
    { lefDumbMode = 1000000; lefRealNum = 1; lefInProp = 1; ;}
    break;

  case 328:

/* Line 1464 of yacc.c  */
#line 3210 "lef.y"
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    ;}
    break;

  case 329:

/* Line 1464 of yacc.c  */
#line 3215 "lef.y"
    { 
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setTopOfStack();
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
              //lefWarning(2019, "TOPOFSTACKONLY statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later");
    ;}
    break;

  case 332:

/* Line 1464 of yacc.c  */
#line 3231 "lef.y"
    { 
      char temp[32];
      sprintf(temp, "%.11g", (yyvsp[(2) - (2)].dval));
      if (lefrViaCbk) {
         char propTp;
         propTp = lefrViaProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrVia.lefiVia::addNumProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), temp, propTp);
      }
    ;}
    break;

  case 333:

/* Line 1464 of yacc.c  */
#line 3241 "lef.y"
    {
      if (lefrViaCbk) {
         char propTp;
         propTp = lefrViaProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrVia.lefiVia::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 334:

/* Line 1464 of yacc.c  */
#line 3249 "lef.y"
    {
      if (lefrViaCbk) {
         char propTp;
         propTp = lefrViaProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrVia.lefiVia::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 335:

/* Line 1464 of yacc.c  */
#line 3259 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign((yyvsp[(1) - (2)].string), 0, 0.0, 0.0, -1);
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 336:

/* Line 1464 of yacc.c  */
#line 3268 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign((yyvsp[(1) - (3)].string), 1, (yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, -1);
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 337:

/* Line 1464 of yacc.c  */
#line 3277 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign((yyvsp[(1) - (4)].string), 1, (yyvsp[(2) - (4)].pt).x, (yyvsp[(2) - (4)].pt).y, (yyvsp[(3) - (4)].integer));
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 338:

/* Line 1464 of yacc.c  */
#line 3286 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrViaCbk) lefrVia.lefiVia::setForeign((yyvsp[(1) - (3)].string), 0, 0.0, 0.0, (yyvsp[(2) - (3)].integer));
      } else
        if (lefrViaCbk)  /* write warning only if cbk is set */
           if (viaWarnings++ < lefrViaWarnings)
             lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 339:

/* Line 1464 of yacc.c  */
#line 3295 "lef.y"
    {lefDumbMode = 1; lefNoNum= 1;;}
    break;

  case 340:

/* Line 1464 of yacc.c  */
#line 3296 "lef.y"
    { (yyval.string) = (yyvsp[(3) - (3)].string); ;}
    break;

  case 341:

/* Line 1464 of yacc.c  */
#line 3299 "lef.y"
    {(yyval.integer) = 0;;}
    break;

  case 342:

/* Line 1464 of yacc.c  */
#line 3300 "lef.y"
    {(yyval.integer) = 1;;}
    break;

  case 343:

/* Line 1464 of yacc.c  */
#line 3301 "lef.y"
    {(yyval.integer) = 2;;}
    break;

  case 344:

/* Line 1464 of yacc.c  */
#line 3302 "lef.y"
    {(yyval.integer) = 3;;}
    break;

  case 345:

/* Line 1464 of yacc.c  */
#line 3303 "lef.y"
    {(yyval.integer) = 4;;}
    break;

  case 346:

/* Line 1464 of yacc.c  */
#line 3304 "lef.y"
    {(yyval.integer) = 5;;}
    break;

  case 347:

/* Line 1464 of yacc.c  */
#line 3305 "lef.y"
    {(yyval.integer) = 6;;}
    break;

  case 348:

/* Line 1464 of yacc.c  */
#line 3306 "lef.y"
    {(yyval.integer) = 7;;}
    break;

  case 349:

/* Line 1464 of yacc.c  */
#line 3307 "lef.y"
    {(yyval.integer) = 0;;}
    break;

  case 350:

/* Line 1464 of yacc.c  */
#line 3308 "lef.y"
    {(yyval.integer) = 1;;}
    break;

  case 351:

/* Line 1464 of yacc.c  */
#line 3309 "lef.y"
    {(yyval.integer) = 2;;}
    break;

  case 352:

/* Line 1464 of yacc.c  */
#line 3310 "lef.y"
    {(yyval.integer) = 3;;}
    break;

  case 353:

/* Line 1464 of yacc.c  */
#line 3311 "lef.y"
    {(yyval.integer) = 4;;}
    break;

  case 354:

/* Line 1464 of yacc.c  */
#line 3312 "lef.y"
    {(yyval.integer) = 5;;}
    break;

  case 355:

/* Line 1464 of yacc.c  */
#line 3313 "lef.y"
    {(yyval.integer) = 6;;}
    break;

  case 356:

/* Line 1464 of yacc.c  */
#line 3314 "lef.y"
    {(yyval.integer) = 7;;}
    break;

  case 357:

/* Line 1464 of yacc.c  */
#line 3317 "lef.y"
    { ;}
    break;

  case 358:

/* Line 1464 of yacc.c  */
#line 3319 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 359:

/* Line 1464 of yacc.c  */
#line 3320 "lef.y"
    {
      if (lefrViaCbk) lefrVia.lefiVia::addLayer((yyvsp[(3) - (4)].string));
      viaLayer++;
      hasViaRule_layer = 1;
    ;}
    break;

  case 362:

/* Line 1464 of yacc.c  */
#line 3333 "lef.y"
    { 
      if (lefrViaCbk) {
        if (versionNum < 5.8 && (int)(yyvsp[(2) - (5)].integer) > 0) {
          if (viaWarnings++ < lefrViaWarnings) {
              lefError(2081, "MASK information can only be defined with version 5.8");
              CHKERR(); 
            }           
        } else {
          lefrVia.lefiVia::addRectToLayer((int)(yyvsp[(2) - (5)].integer), (yyvsp[(3) - (5)].pt).x, (yyvsp[(3) - (5)].pt).y, (yyvsp[(4) - (5)].pt).x, (yyvsp[(4) - (5)].pt).y);
        }
      }
    ;}
    break;

  case 363:

/* Line 1464 of yacc.c  */
#line 3346 "lef.y"
    {
      lefrGeometriesPtr = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
      lefrGeometriesPtr->lefiGeometries::Init();
      lefrDoGeometries = 1;
    ;}
    break;

  case 364:

/* Line 1464 of yacc.c  */
#line 3352 "lef.y"
    { 
      if (lefrViaCbk) {
        if (versionNum < 5.8 && (yyvsp[(2) - (8)].integer) > 0) {
          if (viaWarnings++ < lefrViaWarnings) {
              lefError(2083, "Color mask information can only be defined with version 5.8.");
              CHKERR(); 
            }           
        } else {
            lefrGeometriesPtr->lefiGeometries::addPolygon((int)(yyvsp[(2) - (8)].integer));
            lefrVia.lefiVia::addPolyToLayer((int)(yyvsp[(2) - (8)].integer), lefrGeometriesPtr);   // 5.6
        }
      }
      lefrGeometriesPtr->lefiGeometries::clearPolyItems(); // free items fields
      lefFree((char*)(lefrGeometriesPtr)); // Don't need anymore, poly data has
      lefrDoGeometries = 0;                // copied
    ;}
    break;

  case 365:

/* Line 1464 of yacc.c  */
#line 3369 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 366:

/* Line 1464 of yacc.c  */
#line 3370 "lef.y"
    { 
      // 10/17/2001 - Wanda da Rosa, PCR 404149
      //              Error if no layer in via
      if (!viaLayer) {
         if (lefrViaCbk) {  /* write error only if cbk is set */
            if (viaWarnings++ < lefrViaWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "A LAYER statement is missing in the VIA %s.\nAt least one LAYERis required per VIA statement.", (yyvsp[(3) - (3)].string));
              lefError(1606, outMsg);
              lefFree(outMsg);
              CHKERR();
            }
         }
      }
      if (strcmp(viaName, (yyvsp[(3) - (3)].string)) != 0) {
         if (lefrViaCbk) { /* write error only if cbk is set */
            if (viaWarnings++ < lefrViaWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                "END VIA name %s is different from the VIA name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), viaName);
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
    ;}
    break;

  case 367:

/* Line 1464 of yacc.c  */
#line 3403 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 368:

/* Line 1464 of yacc.c  */
#line 3404 "lef.y"
    { 
      if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setName((yyvsp[(3) - (3)].string));
      viaRuleLayer = 0;
      //strcpy(viaRuleName, $3);
      viaRuleName = strdup((yyvsp[(3) - (3)].string));
      isGenerate = 0;
    ;}
    break;

  case 369:

/* Line 1464 of yacc.c  */
#line 3414 "lef.y"
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
    ;}
    break;

  case 370:

/* Line 1464 of yacc.c  */
#line 3440 "lef.y"
    {
      isGenerate = 1;
    ;}
    break;

  case 371:

/* Line 1464 of yacc.c  */
#line 3444 "lef.y"
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
    ;}
    break;

  case 373:

/* Line 1464 of yacc.c  */
#line 3477 "lef.y"
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
    ;}
    break;

  case 380:

/* Line 1464 of yacc.c  */
#line 3508 "lef.y"
    { lefDumbMode = 10000000; lefRealNum = 1; lefInProp = 1; ;}
    break;

  case 381:

/* Line 1464 of yacc.c  */
#line 3509 "lef.y"
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    ;}
    break;

  case 384:

/* Line 1464 of yacc.c  */
#line 3521 "lef.y"
    {
      if (lefrViaRuleCbk) {
         char propTp;
         propTp = lefrViaRuleProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrViaRule.lefiViaRule::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 385:

/* Line 1464 of yacc.c  */
#line 3529 "lef.y"
    {
      if (lefrViaRuleCbk) {
         char propTp;
         propTp = lefrViaRuleProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrViaRule.lefiViaRule::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 386:

/* Line 1464 of yacc.c  */
#line 3537 "lef.y"
    {
      char temp[32];
      sprintf(temp, "%.11g", (yyvsp[(2) - (2)].dval));
      if (lefrViaRuleCbk) {
         char propTp;
         propTp = lefrViaRuleProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrViaRule.lefiViaRule::addNumProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), temp, propTp);
      }
    ;}
    break;

  case 387:

/* Line 1464 of yacc.c  */
#line 3548 "lef.y"
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
    ;}
    break;

  case 390:

/* Line 1464 of yacc.c  */
#line 3584 "lef.y"
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::addViaName((yyvsp[(2) - (3)].string)); ;}
    break;

  case 391:

/* Line 1464 of yacc.c  */
#line 3586 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 392:

/* Line 1464 of yacc.c  */
#line 3587 "lef.y"
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setLayer((yyvsp[(3) - (4)].string));
      viaRuleHasDir = 0;
      viaRuleHasEnc = 0;
    ;}
    break;

  case 395:

/* Line 1464 of yacc.c  */
#line 3599 "lef.y"
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
    ;}
    break;

  case 396:

/* Line 1464 of yacc.c  */
#line 3618 "lef.y"
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
    ;}
    break;

  case 397:

/* Line 1464 of yacc.c  */
#line 3637 "lef.y"
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
         if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setEnclosure((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
      }
      viaRuleHasEnc = 1;
    ;}
    break;

  case 398:

/* Line 1464 of yacc.c  */
#line 3673 "lef.y"
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setWidth((yyvsp[(2) - (5)].dval),(yyvsp[(4) - (5)].dval)); ;}
    break;

  case 399:

/* Line 1464 of yacc.c  */
#line 3675 "lef.y"
    { if (lefrViaRuleCbk)
	lefrViaRule.lefiViaRule::setRect((yyvsp[(2) - (4)].pt).x, (yyvsp[(2) - (4)].pt).y, (yyvsp[(3) - (4)].pt).x, (yyvsp[(3) - (4)].pt).y); ;}
    break;

  case 400:

/* Line 1464 of yacc.c  */
#line 3678 "lef.y"
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setSpacing((yyvsp[(2) - (5)].dval),(yyvsp[(4) - (5)].dval)); ;}
    break;

  case 401:

/* Line 1464 of yacc.c  */
#line 3680 "lef.y"
    { if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setResistance((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 402:

/* Line 1464 of yacc.c  */
#line 3682 "lef.y"
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
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setOverhang((yyvsp[(2) - (3)].dval));
      } else {
        if (lefrViaRuleCbk)  /* write warning only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings)
              lefWarning(2023, "OVERHANG statement will be translated into similar ENCLOSURE rule");
        // In 5.6 & later, set it to either ENCLOSURE overhang1 or overhang2
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setOverhangToEnclosure((yyvsp[(2) - (3)].dval));
      }
    ;}
    break;

  case 403:

/* Line 1464 of yacc.c  */
#line 3712 "lef.y"
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
        if (lefrViaRuleCbk) lefrViaRule.lefiViaRule::setMetalOverhang((yyvsp[(2) - (3)].dval));
      } else
        if (lefrViaRuleCbk)  /* write warning only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings)
             lefWarning(2024, "METALOVERHANG statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 404:

/* Line 1464 of yacc.c  */
#line 3739 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 405:

/* Line 1464 of yacc.c  */
#line 3740 "lef.y"
    {
      if (strcmp(viaRuleName, (yyvsp[(3) - (3)].string)) != 0) {
        if (lefrViaRuleCbk) {  /* write error only if cbk is set */
           if (viaRuleWarnings++ < lefrViaRuleWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END VIARULE name %s is different from the VIARULE name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), viaRuleName);
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
    ;}
    break;

  case 406:

/* Line 1464 of yacc.c  */
#line 3760 "lef.y"
    { ;}
    break;

  case 407:

/* Line 1464 of yacc.c  */
#line 3763 "lef.y"
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
    ;}
    break;

  case 408:

/* Line 1464 of yacc.c  */
#line 3785 "lef.y"
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
    ;}
    break;

  case 411:

/* Line 1464 of yacc.c  */
#line 3803 "lef.y"
    {
      if ((versionNum < 5.6) || (!ndRule)) {
        if (versionNum < 5.7) {
          if (lefrSpacingCbk) {
            lefrSpacing.lefiSpacing::set((yyvsp[(2) - (5)].string), (yyvsp[(3) - (5)].string), (yyvsp[(4) - (5)].dval), 0);
            CALLBACK(lefrSpacingCbk, lefrSpacingCbkType, &lefrSpacing);
          }
        }
      }
    ;}
    break;

  case 412:

/* Line 1464 of yacc.c  */
#line 3814 "lef.y"
    {
      if ((versionNum < 5.6) || (!ndRule)) {
        if (versionNum < 5.7) {
          if (lefrSpacingCbk) {
	    lefrSpacing.lefiSpacing::set((yyvsp[(2) - (6)].string), (yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].dval), 1);
	    CALLBACK(lefrSpacingCbk, lefrSpacingCbkType, &lefrSpacing);
          }
        }
      }
    ;}
    break;

  case 413:

/* Line 1464 of yacc.c  */
#line 3827 "lef.y"
    { lefDumbMode = 2; lefNoNum = 2; hasSamenet = 1; ;}
    break;

  case 414:

/* Line 1464 of yacc.c  */
#line 3831 "lef.y"
    { (yyval.integer) = 0; ;}
    break;

  case 415:

/* Line 1464 of yacc.c  */
#line 3833 "lef.y"
    { (yyval.integer) = (int)(yyvsp[(2) - (2)].dval); ;}
    break;

  case 416:

/* Line 1464 of yacc.c  */
#line 3836 "lef.y"
    { ;}
    break;

  case 417:

/* Line 1464 of yacc.c  */
#line 3839 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrIRDropBeginCbk) 
	  CALLBACK(lefrIRDropBeginCbk, lefrIRDropBeginCbkType, 0);
      } else
        if (lefrIRDropBeginCbk) /* write warning only if cbk is set */
          if (iRDropWarnings++ < lefrIRDropWarnings)
            lefWarning(2026, "IRDROP statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 418:

/* Line 1464 of yacc.c  */
#line 3850 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrIRDropEndCbk)
	  CALLBACK(lefrIRDropEndCbk, lefrIRDropEndCbkType, 0);
      }
    ;}
    break;

  case 421:

/* Line 1464 of yacc.c  */
#line 3864 "lef.y"
    { 
      if (versionNum < 5.4) {
        if (lefrIRDropCbk)
          CALLBACK(lefrIRDropCbk, lefrIRDropCbkType, &lefrIRDrop);
      }
    ;}
    break;

  case 424:

/* Line 1464 of yacc.c  */
#line 3877 "lef.y"
    { if (lefrIRDropCbk) lefrIRDrop.lefiIRDrop::setValues((yyvsp[(1) - (2)].dval), (yyvsp[(2) - (2)].dval)); ;}
    break;

  case 425:

/* Line 1464 of yacc.c  */
#line 3880 "lef.y"
    { if (lefrIRDropCbk) lefrIRDrop.lefiIRDrop::setTableName((yyvsp[(2) - (2)].string)); ;}
    break;

  case 426:

/* Line 1464 of yacc.c  */
#line 3883 "lef.y"
    {
    hasMinfeature = 1;
    if (versionNum < 5.4) {
       if (lefrMinFeatureCbk) {
         lefrMinFeature.lefiMinFeature::set((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval));
         CALLBACK(lefrMinFeatureCbk, lefrMinFeatureCbkType, &lefrMinFeature);
       }
    } else
       if (lefrMinFeatureCbk) /* write warning only if cbk is set */
          if (minFeatureWarnings++ < lefrMinFeatureWarnings)
            lefWarning(2027, "MINFEATURE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  ;}
    break;

  case 427:

/* Line 1464 of yacc.c  */
#line 3897 "lef.y"
    {
    if (versionNum < 5.4) {
       if (lefrDielectricCbk)
         CALLBACK(lefrDielectricCbk, lefrDielectricCbkType, (yyvsp[(2) - (3)].dval));
    } else
       if (lefrDielectricCbk) /* write warning only if cbk is set */
         if (dielectricWarnings++ < lefrDielectricWarnings)
           lefWarning(2028, "DIELECTRIC statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  ;}
    break;

  case 428:

/* Line 1464 of yacc.c  */
#line 3907 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 429:

/* Line 1464 of yacc.c  */
#line 3908 "lef.y"
    {
    (void)lefSetNonDefault((yyvsp[(3) - (3)].string));
    if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::setName((yyvsp[(3) - (3)].string));
    ndLayer = 0;
    ndRule = 1;
    numVia = 0;
    //strcpy(nonDefaultRuleName, $3);
    nonDefaultRuleName = strdup((yyvsp[(3) - (3)].string));
  ;}
    break;

  case 430:

/* Line 1464 of yacc.c  */
#line 3918 "lef.y"
    {lefNdRule = 1;;}
    break;

  case 431:

/* Line 1464 of yacc.c  */
#line 3919 "lef.y"
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
  ;}
    break;

  case 432:

/* Line 1464 of yacc.c  */
#line 3949 "lef.y"
    {
      if ((nonDefaultRuleName) && (*nonDefaultRuleName != '\0'))
        lefFree(nonDefaultRuleName);
    ;}
    break;

  case 433:

/* Line 1464 of yacc.c  */
#line 3954 "lef.y"
    {
      if (strcmp(nonDefaultRuleName, (yyvsp[(2) - (2)].string)) != 0) {
        if (lefrNonDefaultCbk) { /* write error only if cbk is set */
          if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
             outMsg = (char*)lefMalloc(10000);
             sprintf (outMsg,
                "END NONDEFAULTRULE name %s is different from the NONDEFAULTRULE name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(2) - (2)].string), nonDefaultRuleName);
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
    ;}
    break;

  case 435:

/* Line 1464 of yacc.c  */
#line 3977 "lef.y"
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
    ;}
    break;

  case 445:

/* Line 1464 of yacc.c  */
#line 4010 "lef.y"
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
             lefrNonDefault.lefiNonDefault::addUseVia((yyvsp[(2) - (3)].string));
       }
    ;}
    break;

  case 446:

/* Line 1464 of yacc.c  */
#line 4027 "lef.y"
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
             lefrNonDefault.lefiNonDefault::addUseViaRule((yyvsp[(2) - (3)].string));
       }
    ;}
    break;

  case 447:

/* Line 1464 of yacc.c  */
#line 4046 "lef.y"
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
             lefrNonDefault.lefiNonDefault::addMinCuts((yyvsp[(2) - (4)].string), (int)(yyvsp[(3) - (4)].dval));
       }
    ;}
    break;

  case 448:

/* Line 1464 of yacc.c  */
#line 4064 "lef.y"
    { lefDumbMode = 10000000; lefRealNum = 1; lefInProp = 1; ;}
    break;

  case 449:

/* Line 1464 of yacc.c  */
#line 4065 "lef.y"
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    ;}
    break;

  case 452:

/* Line 1464 of yacc.c  */
#line 4077 "lef.y"
    {
      if (lefrNonDefaultCbk) {
         char propTp;
         propTp = lefrNondefProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrNonDefault.lefiNonDefault::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 453:

/* Line 1464 of yacc.c  */
#line 4085 "lef.y"
    {
      if (lefrNonDefaultCbk) {
         char propTp;
         propTp = lefrNondefProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrNonDefault.lefiNonDefault::addProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 454:

/* Line 1464 of yacc.c  */
#line 4093 "lef.y"
    {
      if (lefrNonDefaultCbk) {
         char temp[32];
         char propTp;
         sprintf(temp, "%.11g", (yyvsp[(2) - (2)].dval));
         propTp = lefrNondefProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrNonDefault.lefiNonDefault::addNumProp((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), temp, propTp);
      }
    ;}
    break;

  case 455:

/* Line 1464 of yacc.c  */
#line 4103 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 456:

/* Line 1464 of yacc.c  */
#line 4104 "lef.y"
    {
    if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::addLayer((yyvsp[(3) - (3)].string));
    ndLayer++;
    //strcpy(layerName, $3);
    layerName = strdup((yyvsp[(3) - (3)].string));
    ndLayerWidth = 0;
    ndLayerSpace = 0;
  ;}
    break;

  case 457:

/* Line 1464 of yacc.c  */
#line 4113 "lef.y"
    { 
    ndLayerWidth = 1;
    if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::addWidth((yyvsp[(6) - (7)].dval));
  ;}
    break;

  case 458:

/* Line 1464 of yacc.c  */
#line 4117 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 459:

/* Line 1464 of yacc.c  */
#line 4118 "lef.y"
    {
    if (strcmp(layerName, (yyvsp[(12) - (12)].string)) != 0) {
      if (lefrNonDefaultCbk) { /* write error only if cbk is set */
         if (nonDefaultWarnings++ < lefrNonDefaultWarnings) {
            outMsg = (char*)lefMalloc(10000);
            sprintf (outMsg,
               "END LAYER name %s is different from the LAYER name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (12)].string), layerName);
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
  ;}
    break;

  case 462:

/* Line 1464 of yacc.c  */
#line 4166 "lef.y"
    {
      ndLayerSpace = 1;
      if (lefrNonDefaultCbk) lefrNonDefault.lefiNonDefault::addSpacing((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 463:

/* Line 1464 of yacc.c  */
#line 4171 "lef.y"
    { if (lefrNonDefaultCbk)
         lefrNonDefault.lefiNonDefault::addWireExtension((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 464:

/* Line 1464 of yacc.c  */
#line 4174 "lef.y"
    {
      if (ignoreVersion) {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addResistance((yyvsp[(3) - (4)].dval));
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
         lefrNonDefault.lefiNonDefault::addResistance((yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 465:

/* Line 1464 of yacc.c  */
#line 4198 "lef.y"
    {
      if (ignoreVersion) {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addCapacitance((yyvsp[(3) - (4)].dval));
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
         lefrNonDefault.lefiNonDefault::addCapacitance((yyvsp[(3) - (4)].dval));
    ;}
    break;

  case 466:

/* Line 1464 of yacc.c  */
#line 4221 "lef.y"
    {
      if (ignoreVersion) {
         if (lefrNonDefaultCbk)
            lefrNonDefault.lefiNonDefault::addEdgeCap((yyvsp[(2) - (3)].dval));
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
         lefrNonDefault.lefiNonDefault::addEdgeCap((yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 467:

/* Line 1464 of yacc.c  */
#line 4244 "lef.y"
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
            lefrNonDefault.lefiNonDefault::addDiagWidth((yyvsp[(2) - (3)].dval));
      }
    ;}
    break;

  case 468:

/* Line 1464 of yacc.c  */
#line 4263 "lef.y"
    { 
      if (lefrSiteCbk)
        CALLBACK(lefrSiteCbk, lefrSiteCbkType, &lefrSite);
    ;}
    break;

  case 469:

/* Line 1464 of yacc.c  */
#line 4268 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 470:

/* Line 1464 of yacc.c  */
#line 4269 "lef.y"
    { 
      if (lefrSiteCbk) lefrSite.lefiSite::setName((yyvsp[(3) - (3)].string));
      //strcpy(siteName, $3);
      siteName = strdup((yyvsp[(3) - (3)].string));
      hasSiteClass = 0;
      hasSiteSize = 0;
      hasSite = 1;
    ;}
    break;

  case 471:

/* Line 1464 of yacc.c  */
#line 4278 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 472:

/* Line 1464 of yacc.c  */
#line 4279 "lef.y"
    {
      if (strcmp(siteName, (yyvsp[(3) - (3)].string)) != 0) {
        if (lefrSiteCbk) { /* write error only if cbk is set */
           if (siteWarnings++ < lefrSiteWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END SITE name %s is different from the SITE name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), siteName);
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
    ;}
    break;

  case 475:

/* Line 1464 of yacc.c  */
#line 4316 "lef.y"
    {
/* Workaround for pcr 640902
*/
      if (lefrSiteCbk) lefrSite.lefiSite::setSize((yyvsp[(2) - (5)].dval),(yyvsp[(4) - (5)].dval));
      hasSiteSize = 1;
    ;}
    break;

  case 476:

/* Line 1464 of yacc.c  */
#line 4323 "lef.y"
    { ;}
    break;

  case 477:

/* Line 1464 of yacc.c  */
#line 4325 "lef.y"
    { 
      if (lefrSiteCbk) lefrSite.lefiSite::setClass((yyvsp[(1) - (1)].string));
      hasSiteClass = 1;
    ;}
    break;

  case 478:

/* Line 1464 of yacc.c  */
#line 4330 "lef.y"
    { ;}
    break;

  case 479:

/* Line 1464 of yacc.c  */
#line 4333 "lef.y"
    {(yyval.string) = (char*)"PAD"; ;}
    break;

  case 480:

/* Line 1464 of yacc.c  */
#line 4334 "lef.y"
    {(yyval.string) = (char*)"CORE"; ;}
    break;

  case 481:

/* Line 1464 of yacc.c  */
#line 4335 "lef.y"
    {(yyval.string) = (char*)"VIRTUAL"; ;}
    break;

  case 482:

/* Line 1464 of yacc.c  */
#line 4338 "lef.y"
    { ;}
    break;

  case 485:

/* Line 1464 of yacc.c  */
#line 4347 "lef.y"
    { if (lefrSiteCbk) lefrSite.lefiSite::setXSymmetry(); ;}
    break;

  case 486:

/* Line 1464 of yacc.c  */
#line 4349 "lef.y"
    { if (lefrSiteCbk) lefrSite.lefiSite::setYSymmetry(); ;}
    break;

  case 487:

/* Line 1464 of yacc.c  */
#line 4351 "lef.y"
    { if (lefrSiteCbk) lefrSite.lefiSite::set90Symmetry(); ;}
    break;

  case 488:

/* Line 1464 of yacc.c  */
#line 4353 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 489:

/* Line 1464 of yacc.c  */
#line 4355 "lef.y"
    { ;}
    break;

  case 492:

/* Line 1464 of yacc.c  */
#line 4362 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 493:

/* Line 1464 of yacc.c  */
#line 4363 "lef.y"
    { if (lefrSiteCbk) lefrSite.lefiSite::addRowPattern((yyvsp[(1) - (3)].string), (yyvsp[(2) - (3)].integer)); ;}
    break;

  case 494:

/* Line 1464 of yacc.c  */
#line 4367 "lef.y"
    { (yyval.pt).x = (yyvsp[(1) - (2)].dval); (yyval.pt).y = (yyvsp[(2) - (2)].dval); ;}
    break;

  case 495:

/* Line 1464 of yacc.c  */
#line 4369 "lef.y"
    { (yyval.pt).x = (yyvsp[(2) - (4)].dval); (yyval.pt).y = (yyvsp[(3) - (4)].dval); ;}
    break;

  case 496:

/* Line 1464 of yacc.c  */
#line 4372 "lef.y"
    { 
      if (lefrMacroCbk)
        CALLBACK(lefrMacroCbk, lefrMacroCbkType, &lefrMacro);
      lefrDoSite = 0;
    ;}
    break;

  case 498:

/* Line 1464 of yacc.c  */
#line 4379 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 499:

/* Line 1464 of yacc.c  */
#line 4380 "lef.y"
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
        lefrMacro.lefiMacro::setName((yyvsp[(3) - (3)].string));
        CALLBACK(lefrMacroBeginCbk, lefrMacroBeginCbkType, (yyvsp[(3) - (3)].string));
      }
      //strcpy(macroName, $3);
      macroName = strdup((yyvsp[(3) - (3)].string));
    ;}
    break;

  case 500:

/* Line 1464 of yacc.c  */
#line 4397 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 501:

/* Line 1464 of yacc.c  */
#line 4398 "lef.y"
    {
      if (strcmp(macroName, (yyvsp[(3) - (3)].string)) != 0) {
        if (lefrMacroEndCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END MACRO name %s is different from the MACRO name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), macroName);
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
        CALLBACK(lefrMacroEndCbk, lefrMacroEndCbkType, (yyvsp[(3) - (3)].string));
    ;}
    break;

  case 509:

/* Line 1464 of yacc.c  */
#line 4434 "lef.y"
    { ;}
    break;

  case 510:

/* Line 1464 of yacc.c  */
#line 4436 "lef.y"
    { ;}
    break;

  case 511:

/* Line 1464 of yacc.c  */
#line 4438 "lef.y"
    { ;}
    break;

  case 512:

/* Line 1464 of yacc.c  */
#line 4440 "lef.y"
    { ;}
    break;

  case 515:

/* Line 1464 of yacc.c  */
#line 4444 "lef.y"
    { ;}
    break;

  case 516:

/* Line 1464 of yacc.c  */
#line 4446 "lef.y"
    { ;}
    break;

  case 517:

/* Line 1464 of yacc.c  */
#line 4448 "lef.y"
    { ;}
    break;

  case 518:

/* Line 1464 of yacc.c  */
#line 4450 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setBuffer(); ;}
    break;

  case 519:

/* Line 1464 of yacc.c  */
#line 4452 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setInverter(); ;}
    break;

  case 520:

/* Line 1464 of yacc.c  */
#line 4454 "lef.y"
    { ;}
    break;

  case 521:

/* Line 1464 of yacc.c  */
#line 4456 "lef.y"
    { ;}
    break;

  case 522:

/* Line 1464 of yacc.c  */
#line 4458 "lef.y"
    { ;}
    break;

  case 523:

/* Line 1464 of yacc.c  */
#line 4460 "lef.y"
    { ;}
    break;

  case 524:

/* Line 1464 of yacc.c  */
#line 4461 "lef.y"
    {lefDumbMode = 1000000; lefRealNum = 1; lefInProp = 1; ;}
    break;

  case 525:

/* Line 1464 of yacc.c  */
#line 4462 "lef.y"
    { lefDumbMode = 0;
        lefRealNum = 0;
        lefInProp = 0;
      ;}
    break;

  case 528:

/* Line 1464 of yacc.c  */
#line 4473 "lef.y"
    {
      if (siteDef) { /* SITE is defined before SYMMETRY */
          /* pcr 283846 suppress warning */
          if (lefrMacroCbk) /* write warning only if cbk is set */
             if (macroWarnings++ < lefrMacroWarnings)
               lefWarning(2032, "A SITE statement is defined before SYMMETRY statement.\nTo avoid this warning in the future, define SITE after SYMMETRY");
      }
      symDef = 1;
    ;}
    break;

  case 531:

/* Line 1464 of yacc.c  */
#line 4490 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setXSymmetry(); ;}
    break;

  case 532:

/* Line 1464 of yacc.c  */
#line 4492 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setYSymmetry(); ;}
    break;

  case 533:

/* Line 1464 of yacc.c  */
#line 4494 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::set90Symmetry(); ;}
    break;

  case 534:

/* Line 1464 of yacc.c  */
#line 4498 "lef.y"
    {
      char temp[32];
      sprintf(temp, "%.11g", (yyvsp[(2) - (2)].dval));
      if (lefrMacroCbk) {
         char propTp;
         propTp = lefrMacroProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrMacro.lefiMacro::setNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), temp,  propTp);
      }
    ;}
    break;

  case 535:

/* Line 1464 of yacc.c  */
#line 4508 "lef.y"
    {
      if (lefrMacroCbk) {
         char propTp;
         propTp = lefrMacroProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrMacro.lefiMacro::setProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 536:

/* Line 1464 of yacc.c  */
#line 4516 "lef.y"
    {
      if (lefrMacroCbk) {
         char propTp;
         propTp = lefrMacroProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrMacro.lefiMacro::setProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 537:

/* Line 1464 of yacc.c  */
#line 4525 "lef.y"
    {
       if (lefrMacroCbk) lefrMacro.lefiMacro::setClass((yyvsp[(2) - (3)].string));
       if (lefrMacroClassTypeCbk)
          CALLBACK(lefrMacroClassTypeCbk, lefrMacroClassTypeCbkType, (yyvsp[(2) - (3)].string));
    ;}
    break;

  case 538:

/* Line 1464 of yacc.c  */
#line 4532 "lef.y"
    {(yyval.string) = (char*)"COVER"; ;}
    break;

  case 539:

/* Line 1464 of yacc.c  */
#line 4534 "lef.y"
    { (yyval.string) = (char*)"COVER BUMP";
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
    ;}
    break;

  case 540:

/* Line 1464 of yacc.c  */
#line 4552 "lef.y"
    {(yyval.string) = (char*)"RING"; ;}
    break;

  case 541:

/* Line 1464 of yacc.c  */
#line 4553 "lef.y"
    {(yyval.string) = (char*)"BLOCK"; ;}
    break;

  case 542:

/* Line 1464 of yacc.c  */
#line 4555 "lef.y"
    { (yyval.string) = (char*)"BLOCK BLACKBOX";
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
    ;}
    break;

  case 543:

/* Line 1464 of yacc.c  */
#line 4574 "lef.y"
    {
      if (ignoreVersion) {
        (yyval.string) = (char*)"BLOCK SOFT";
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
        (yyval.string) = (char*)"BLOCK SOFT";
    ;}
    break;

  case 544:

/* Line 1464 of yacc.c  */
#line 4592 "lef.y"
    {(yyval.string) = (char*)"NONE"; ;}
    break;

  case 545:

/* Line 1464 of yacc.c  */
#line 4594 "lef.y"
    {
        if (versionNum < 5.7) {
          outMsg = (char*)lefMalloc(10000);
          sprintf(outMsg,
            "BUMP is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
          lefError(1698, outMsg);
          lefFree(outMsg);
          CHKERR();
      } else
        (yyval.string) = (char*)"BUMP";
      ;}
    break;

  case 546:

/* Line 1464 of yacc.c  */
#line 4605 "lef.y"
    {(yyval.string) = (char*)"PAD"; ;}
    break;

  case 547:

/* Line 1464 of yacc.c  */
#line 4606 "lef.y"
    {(yyval.string) = (char*)"VIRTUAL"; ;}
    break;

  case 548:

/* Line 1464 of yacc.c  */
#line 4608 "lef.y"
    {  sprintf(temp_name, "PAD %s", (yyvsp[(2) - (2)].string));
        (yyval.string) = temp_name; 
        if (versionNum < 5.5) {
           if (strcmp("AREAIO", (yyvsp[(2) - (2)].string)) != 0) {
             sprintf(temp_name, "PAD %s", (yyvsp[(2) - (2)].string));
             (yyval.string) = temp_name; 
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
      ;}
    break;

  case 549:

/* Line 1464 of yacc.c  */
#line 4630 "lef.y"
    {(yyval.string) = (char*)"CORE"; ;}
    break;

  case 550:

/* Line 1464 of yacc.c  */
#line 4632 "lef.y"
    {(yyval.string) = (char*)"CORNER";
      /* This token is NOT in the spec but has shown up in 
       * some lef files.  This exception came from LEFOUT
       * in 'frameworks'
       */
      ;}
    break;

  case 551:

/* Line 1464 of yacc.c  */
#line 4639 "lef.y"
    {sprintf(temp_name, "CORE %s", (yyvsp[(2) - (2)].string));
      (yyval.string) = temp_name;;}
    break;

  case 552:

/* Line 1464 of yacc.c  */
#line 4642 "lef.y"
    {sprintf(temp_name, "ENDCAP %s", (yyvsp[(2) - (2)].string));
      (yyval.string) = temp_name;;}
    break;

  case 553:

/* Line 1464 of yacc.c  */
#line 4647 "lef.y"
    {(yyval.string) = (char*)"INPUT";;}
    break;

  case 554:

/* Line 1464 of yacc.c  */
#line 4648 "lef.y"
    {(yyval.string) = (char*)"OUTPUT";;}
    break;

  case 555:

/* Line 1464 of yacc.c  */
#line 4649 "lef.y"
    {(yyval.string) = (char*)"INOUT";;}
    break;

  case 556:

/* Line 1464 of yacc.c  */
#line 4650 "lef.y"
    {(yyval.string) = (char*)"POWER";;}
    break;

  case 557:

/* Line 1464 of yacc.c  */
#line 4651 "lef.y"
    {(yyval.string) = (char*)"SPACER";;}
    break;

  case 558:

/* Line 1464 of yacc.c  */
#line 4652 "lef.y"
    {(yyval.string) = (char*)"AREAIO";;}
    break;

  case 559:

/* Line 1464 of yacc.c  */
#line 4655 "lef.y"
    {(yyval.string) = (char*)"FEEDTHRU";;}
    break;

  case 560:

/* Line 1464 of yacc.c  */
#line 4656 "lef.y"
    {(yyval.string) = (char*)"TIEHIGH";;}
    break;

  case 561:

/* Line 1464 of yacc.c  */
#line 4657 "lef.y"
    {(yyval.string) = (char*)"TIELOW";;}
    break;

  case 562:

/* Line 1464 of yacc.c  */
#line 4659 "lef.y"
    { 
      if (ignoreVersion) {
        (yyval.string) = (char*)"SPACER";
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
        (yyval.string) = (char*)"SPACER";
    ;}
    break;

  case 563:

/* Line 1464 of yacc.c  */
#line 4678 "lef.y"
    { 
      if (ignoreVersion) {
        (yyval.string) = (char*)"ANTENNACELL";
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
        (yyval.string) = (char*)"ANTENNACELL";
    ;}
    break;

  case 564:

/* Line 1464 of yacc.c  */
#line 4697 "lef.y"
    { 
      if (ignoreVersion) {
        (yyval.string) = (char*)"WELLTAP";
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
        (yyval.string) = (char*)"WELLTAP";
    ;}
    break;

  case 565:

/* Line 1464 of yacc.c  */
#line 4717 "lef.y"
    {(yyval.string) = (char*)"PRE";;}
    break;

  case 566:

/* Line 1464 of yacc.c  */
#line 4718 "lef.y"
    {(yyval.string) = (char*)"POST";;}
    break;

  case 567:

/* Line 1464 of yacc.c  */
#line 4719 "lef.y"
    {(yyval.string) = (char*)"TOPLEFT";;}
    break;

  case 568:

/* Line 1464 of yacc.c  */
#line 4720 "lef.y"
    {(yyval.string) = (char*)"TOPRIGHT";;}
    break;

  case 569:

/* Line 1464 of yacc.c  */
#line 4721 "lef.y"
    {(yyval.string) = (char*)"BOTTOMLEFT";;}
    break;

  case 570:

/* Line 1464 of yacc.c  */
#line 4722 "lef.y"
    {(yyval.string) = (char*)"BOTTOMRIGHT";;}
    break;

  case 571:

/* Line 1464 of yacc.c  */
#line 4725 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setGenerator((yyvsp[(2) - (3)].string)); ;}
    break;

  case 572:

/* Line 1464 of yacc.c  */
#line 4728 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setGenerate((yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].string)); ;}
    break;

  case 573:

/* Line 1464 of yacc.c  */
#line 4732 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setSource("USER");
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2036, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 574:

/* Line 1464 of yacc.c  */
#line 4741 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setSource("GENERATE");
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2037, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 575:

/* Line 1464 of yacc.c  */
#line 4750 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setSource("BLOCK");
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2037, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 576:

/* Line 1464 of yacc.c  */
#line 4760 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setPower((yyvsp[(2) - (3)].dval));
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2038, "MACRO POWER statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 577:

/* Line 1464 of yacc.c  */
#line 4770 "lef.y"
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
       if (lefrMacroCbk) lefrMacro.lefiMacro::setOrigin((yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y);
       if (lefrMacroOriginCbk) {
          macroNum.x = (yyvsp[(2) - (3)].pt).x; 
          macroNum.y = (yyvsp[(2) - (3)].pt).y; 
          CALLBACK(lefrMacroOriginCbk, lefrMacroOriginCbkType, macroNum);
       }
    ;}
    break;

  case 578:

/* Line 1464 of yacc.c  */
#line 4820 "lef.y"
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign((yyvsp[(1) - (2)].string), 0, 0.0, 0.0, -1);
    ;}
    break;

  case 579:

/* Line 1464 of yacc.c  */
#line 4824 "lef.y"
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign((yyvsp[(1) - (3)].string), 1, (yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, -1);
    ;}
    break;

  case 580:

/* Line 1464 of yacc.c  */
#line 4828 "lef.y"
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign((yyvsp[(1) - (4)].string), 1, (yyvsp[(2) - (4)].pt).x, (yyvsp[(2) - (4)].pt).y, (yyvsp[(3) - (4)].integer));
    ;}
    break;

  case 581:

/* Line 1464 of yacc.c  */
#line 4832 "lef.y"
    { if (lefrMacroCbk)
      lefrMacro.lefiMacro::addForeign((yyvsp[(1) - (3)].string), 0, 0.0, 0.0, (yyvsp[(2) - (3)].integer));
    ;}
    break;

  case 582:

/* Line 1464 of yacc.c  */
#line 4838 "lef.y"
    {   
       if (lefrMacroCbk && versionNum >= 5.8) {
          lefrMacro.lefiMacro::setFixedMask(1);
       }
       if (lefrMacroFixedMaskCbk) {
          CALLBACK(lefrMacroFixedMaskCbk, lefrMacroFixedMaskCbkType, 1);
       }        
    ;}
    break;

  case 583:

/* Line 1464 of yacc.c  */
#line 4847 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 584:

/* Line 1464 of yacc.c  */
#line 4848 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setEEQ((yyvsp[(3) - (4)].string)); ;}
    break;

  case 585:

/* Line 1464 of yacc.c  */
#line 4850 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 586:

/* Line 1464 of yacc.c  */
#line 4851 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrMacroCbk) lefrMacro.lefiMacro::setLEQ((yyvsp[(3) - (4)].string));
      } else
        if (lefrMacroCbk) /* write warning only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings)
             lefWarning(2042, "LEQ statement in MACRO is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 587:

/* Line 1464 of yacc.c  */
#line 4862 "lef.y"
    {
      if (lefrMacroCbk) {
        lefrMacro.lefiMacro::setSiteName((yyvsp[(2) - (3)].string));
/* For later than 5.6 release
        lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
                              sizeof(lefiSitePattern));
        lefrSitePatternPtr->lefiSitePattern::Init();
        lefrSitePatternPtr->lefiSitePattern::setSiteName($2);
*/
      }
    ;}
    break;

  case 588:

/* Line 1464 of yacc.c  */
#line 4874 "lef.y"
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
    ;}
    break;

  case 589:

/* Line 1464 of yacc.c  */
#line 4889 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; siteDef = 1;
        if (lefrMacroCbk) lefrDoSite = 1; ;}
    break;

  case 590:

/* Line 1464 of yacc.c  */
#line 4893 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 591:

/* Line 1464 of yacc.c  */
#line 4896 "lef.y"
    { 
      if (siteDef) { /* SITE is defined before SIZE */
         /* pcr 283846 suppress warning
         if (siteWarnings++ < lefrSiteWarnings)
           lefWarning("SITE is defined before SIZE.");
         return 1; 
         */
      }
      sizeDef = 1;
      if (lefrMacroCbk) lefrMacro.lefiMacro::setSize((yyvsp[(2) - (5)].dval), (yyvsp[(4) - (5)].dval));
      if (lefrMacroSizeCbk) {
         macroNum.x = (yyvsp[(2) - (5)].dval); 
         macroNum.y = (yyvsp[(4) - (5)].dval); 
         CALLBACK(lefrMacroSizeCbk, lefrMacroSizeCbkType, macroNum);
      }
    ;}
    break;

  case 592:

/* Line 1464 of yacc.c  */
#line 4917 "lef.y"
    { 
      if (lefrPinCbk)
        CALLBACK(lefrPinCbk, lefrPinCbkType, &lefrPin);
      lefrPin.lefiPin::clear();
    ;}
    break;

  case 593:

/* Line 1464 of yacc.c  */
#line 4923 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; pinDef = 1;;}
    break;

  case 594:

/* Line 1464 of yacc.c  */
#line 4924 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setName((yyvsp[(3) - (3)].string));
      //strcpy(pinName, $3);
      pinName = strdup((yyvsp[(3) - (3)].string));
    ;}
    break;

  case 595:

/* Line 1464 of yacc.c  */
#line 4929 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 596:

/* Line 1464 of yacc.c  */
#line 4930 "lef.y"
    {
      if (strcmp(pinName, (yyvsp[(3) - (3)].string)) != 0) {
        if (lefrMacroCbk) { /* write error only if cbk is set */
           if (macroWarnings++ < lefrMacroWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END PIN name %s is different from the PIN name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), pinName);
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
    ;}
    break;

  case 597:

/* Line 1464 of yacc.c  */
#line 4951 "lef.y"
    { ;}
    break;

  case 598:

/* Line 1464 of yacc.c  */
#line 4953 "lef.y"
    { ;}
    break;

  case 599:

/* Line 1464 of yacc.c  */
#line 4957 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign((yyvsp[(1) - (2)].string), 0, 0.0, 0.0, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 600:

/* Line 1464 of yacc.c  */
#line 4966 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign((yyvsp[(1) - (3)].string), 1, (yyvsp[(2) - (3)].pt).x, (yyvsp[(2) - (3)].pt).y, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 601:

/* Line 1464 of yacc.c  */
#line 4975 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign((yyvsp[(1) - (4)].string), 1, (yyvsp[(2) - (4)].pt).x, (yyvsp[(2) - (4)].pt).y, (yyvsp[(3) - (4)].integer));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 602:

/* Line 1464 of yacc.c  */
#line 4984 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign((yyvsp[(1) - (3)].string), 0, 0.0, 0.0, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 603:

/* Line 1464 of yacc.c  */
#line 4993 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign((yyvsp[(1) - (4)].string), 1, (yyvsp[(3) - (4)].pt).x, (yyvsp[(3) - (4)].pt).y, -1);
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 604:

/* Line 1464 of yacc.c  */
#line 5002 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::addForeign((yyvsp[(1) - (5)].string), 1, (yyvsp[(3) - (5)].pt).x, (yyvsp[(3) - (5)].pt).y, (yyvsp[(4) - (5)].integer));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 605:

/* Line 1464 of yacc.c  */
#line 5010 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 606:

/* Line 1464 of yacc.c  */
#line 5011 "lef.y"
    {
      if (versionNum < 5.6) {
        if (lefrPinCbk) lefrPin.lefiPin::setLEQ((yyvsp[(3) - (4)].string));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2044, "LEQ statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
   ;}
    break;

  case 607:

/* Line 1464 of yacc.c  */
#line 5020 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setPower((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2045, "MACRO POWER statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 608:

/* Line 1464 of yacc.c  */
#line 5029 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setDirection((yyvsp[(1) - (1)].string)); ;}
    break;

  case 609:

/* Line 1464 of yacc.c  */
#line 5031 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setUse((yyvsp[(2) - (3)].string)); ;}
    break;

  case 610:

/* Line 1464 of yacc.c  */
#line 5033 "lef.y"
    { ;}
    break;

  case 611:

/* Line 1464 of yacc.c  */
#line 5035 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setLeakage((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2046, "MACRO LEAKAGE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, r emove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 612:

/* Line 1464 of yacc.c  */
#line 5044 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setRiseThresh((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2047, "MACRO RISETHRESH statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 613:

/* Line 1464 of yacc.c  */
#line 5053 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setFallThresh((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2048, "MACRO FALLTHRESH statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 614:

/* Line 1464 of yacc.c  */
#line 5062 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setRiseSatcur((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2049, "MACRO RISESATCUR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 615:

/* Line 1464 of yacc.c  */
#line 5071 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setFallSatcur((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2050, "MACRO FALLSATCUR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 616:

/* Line 1464 of yacc.c  */
#line 5080 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setVLO((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2051, "MACRO VLO statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 617:

/* Line 1464 of yacc.c  */
#line 5089 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setVHI((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2052, "MACRO VHI statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 618:

/* Line 1464 of yacc.c  */
#line 5098 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setTieoffr((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2053, "MACRO TIEOFFR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 619:

/* Line 1464 of yacc.c  */
#line 5107 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setShape((yyvsp[(2) - (3)].string)); ;}
    break;

  case 620:

/* Line 1464 of yacc.c  */
#line 5108 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 621:

/* Line 1464 of yacc.c  */
#line 5109 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setMustjoin((yyvsp[(3) - (4)].string)); ;}
    break;

  case 622:

/* Line 1464 of yacc.c  */
#line 5110 "lef.y"
    {lefDumbMode = 1;;}
    break;

  case 623:

/* Line 1464 of yacc.c  */
#line 5111 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setOutMargin((yyvsp[(3) - (5)].dval), (yyvsp[(4) - (5)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2054, "MACRO OUTPUTNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 624:

/* Line 1464 of yacc.c  */
#line 5119 "lef.y"
    {lefDumbMode = 1;;}
    break;

  case 625:

/* Line 1464 of yacc.c  */
#line 5120 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setOutResistance((yyvsp[(3) - (5)].dval), (yyvsp[(4) - (5)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2055, "MACRO OUTPUTRESISTANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 626:

/* Line 1464 of yacc.c  */
#line 5128 "lef.y"
    {lefDumbMode = 1;;}
    break;

  case 627:

/* Line 1464 of yacc.c  */
#line 5129 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setInMargin((yyvsp[(3) - (5)].dval), (yyvsp[(4) - (5)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2056, "MACRO INPUTNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 628:

/* Line 1464 of yacc.c  */
#line 5138 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setCapacitance((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2057, "MACRO CAPACITANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 629:

/* Line 1464 of yacc.c  */
#line 5147 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setMaxdelay((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 630:

/* Line 1464 of yacc.c  */
#line 5149 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setMaxload((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 631:

/* Line 1464 of yacc.c  */
#line 5151 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setResistance((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2058, "MACRO RESISTANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 632:

/* Line 1464 of yacc.c  */
#line 5160 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setPulldownres((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2059, "MACRO PULLDOWNRES statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 633:

/* Line 1464 of yacc.c  */
#line 5169 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setCurrentSource("ACTIVE");
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2060, "MACRO CURRENTSOURCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 634:

/* Line 1464 of yacc.c  */
#line 5178 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setCurrentSource("RESISTIVE");
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2061, "MACRO CURRENTSOURCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 635:

/* Line 1464 of yacc.c  */
#line 5187 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setRiseVoltage((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2062, "MACRO RISEVOLTAGETHRESHOLD statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 636:

/* Line 1464 of yacc.c  */
#line 5196 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setFallVoltage((yyvsp[(2) - (3)].dval));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2063, "MACRO FALLVOLTAGETHRESHOLD statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 637:

/* Line 1464 of yacc.c  */
#line 5205 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrPinCbk) lefrPin.lefiPin::setTables((yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].string));
      } else
        if (lefrPinCbk) /* write warning only if cbk is set */
           if (pinWarnings++ < lefrPinWarnings)
             lefWarning(2064, "MACRO IV_TABLES statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 638:

/* Line 1464 of yacc.c  */
#line 5214 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setTaperRule((yyvsp[(2) - (3)].string)); ;}
    break;

  case 639:

/* Line 1464 of yacc.c  */
#line 5215 "lef.y"
    {lefDumbMode = 1000000; lefRealNum = 1; lefInProp = 1; ;}
    break;

  case 640:

/* Line 1464 of yacc.c  */
#line 5216 "lef.y"
    { lefDumbMode = 0;
      lefRealNum = 0;
      lefInProp = 0;
    ;}
    break;

  case 641:

/* Line 1464 of yacc.c  */
#line 5221 "lef.y"
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
    ;}
    break;

  case 642:

/* Line 1464 of yacc.c  */
#line 5238 "lef.y"
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
    ;}
    break;

  case 643:

/* Line 1464 of yacc.c  */
#line 5250 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaSize((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 644:

/* Line 1464 of yacc.c  */
#line 5271 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMetalArea((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 645:

/* Line 1464 of yacc.c  */
#line 5292 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMetalLength((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 646:

/* Line 1464 of yacc.c  */
#line 5313 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setRiseSlewLimit((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 647:

/* Line 1464 of yacc.c  */
#line 5315 "lef.y"
    { if (lefrPinCbk) lefrPin.lefiPin::setFallSlewLimit((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 648:

/* Line 1464 of yacc.c  */
#line 5317 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaPartialMetalArea((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 649:

/* Line 1464 of yacc.c  */
#line 5347 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaPartialMetalSideArea((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 650:

/* Line 1464 of yacc.c  */
#line 5377 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaPartialCutArea((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 651:

/* Line 1464 of yacc.c  */
#line 5407 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaDiffArea((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 652:

/* Line 1464 of yacc.c  */
#line 5437 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaGateArea((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 653:

/* Line 1464 of yacc.c  */
#line 5467 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMaxAreaCar((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 654:

/* Line 1464 of yacc.c  */
#line 5497 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMaxSideAreaCar((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 655:

/* Line 1464 of yacc.c  */
#line 5527 "lef.y"
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
      if (lefrPinCbk) lefrPin.lefiPin::addAntennaMaxCutCar((yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].string));
    ;}
    break;

  case 656:

/* Line 1464 of yacc.c  */
#line 5557 "lef.y"
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
    ;}
    break;

  case 658:

/* Line 1464 of yacc.c  */
#line 5586 "lef.y"
    {lefDumbMode = 2; lefNoNum = 2; ;}
    break;

  case 659:

/* Line 1464 of yacc.c  */
#line 5587 "lef.y"
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
        if (lefrPinCbk) lefrPin.lefiPin::setNetExpr((yyvsp[(3) - (4)].string));
    ;}
    break;

  case 660:

/* Line 1464 of yacc.c  */
#line 5602 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 661:

/* Line 1464 of yacc.c  */
#line 5603 "lef.y"
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
        if (lefrPinCbk) lefrPin.lefiPin::setSupplySensitivity((yyvsp[(3) - (4)].string));
    ;}
    break;

  case 662:

/* Line 1464 of yacc.c  */
#line 5618 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 663:

/* Line 1464 of yacc.c  */
#line 5619 "lef.y"
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
        if (lefrPinCbk) lefrPin.lefiPin::setGroundSensitivity((yyvsp[(3) - (4)].string));
    ;}
    break;

  case 664:

/* Line 1464 of yacc.c  */
#line 5637 "lef.y"
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(1);
    ;}
    break;

  case 665:

/* Line 1464 of yacc.c  */
#line 5642 "lef.y"
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(2);
    ;}
    break;

  case 666:

/* Line 1464 of yacc.c  */
#line 5647 "lef.y"
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(3);
    ;}
    break;

  case 667:

/* Line 1464 of yacc.c  */
#line 5652 "lef.y"
    {
    if (lefrPinCbk)
       lefrPin.lefiPin::addAntennaModel(4);
    ;}
    break;

  case 670:

/* Line 1464 of yacc.c  */
#line 5664 "lef.y"
    { 
      char temp[32];
      sprintf(temp, "%.11g", (yyvsp[(2) - (2)].dval));
      if (lefrPinCbk) {
         char propTp;
         propTp = lefrPinProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrPin.lefiPin::setNumProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].dval), temp, propTp);
      }
    ;}
    break;

  case 671:

/* Line 1464 of yacc.c  */
#line 5674 "lef.y"
    {
      if (lefrPinCbk) {
         char propTp;
         propTp = lefrPinProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrPin.lefiPin::setProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 672:

/* Line 1464 of yacc.c  */
#line 5682 "lef.y"
    {
      if (lefrPinCbk) {
         char propTp;
         propTp = lefrPinProp.lefiPropType::propType((yyvsp[(1) - (2)].string));
         lefrPin.lefiPin::setProperty((yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].string), propTp);
      }
    ;}
    break;

  case 673:

/* Line 1464 of yacc.c  */
#line 5691 "lef.y"
    {(yyval.string) = (char*)"INPUT";;}
    break;

  case 674:

/* Line 1464 of yacc.c  */
#line 5692 "lef.y"
    {(yyval.string) = (char*)"OUTPUT";;}
    break;

  case 675:

/* Line 1464 of yacc.c  */
#line 5693 "lef.y"
    {(yyval.string) = (char*)"OUTPUT TRISTATE";;}
    break;

  case 676:

/* Line 1464 of yacc.c  */
#line 5694 "lef.y"
    {(yyval.string) = (char*)"INOUT";;}
    break;

  case 677:

/* Line 1464 of yacc.c  */
#line 5695 "lef.y"
    {(yyval.string) = (char*)"FEEDTHRU";;}
    break;

  case 678:

/* Line 1464 of yacc.c  */
#line 5698 "lef.y"
    {
      if (lefrPinCbk) {
	lefrDoGeometries = 1;
        hasPRP = 0;
	lefrGeometriesPtr = (lefiGeometries*)lefMalloc( sizeof(lefiGeometries));
	lefrGeometriesPtr->lefiGeometries::Init();
      }
      needGeometry = 0;  // don't need rect/path/poly define yet
      hasGeoLayer = 0;   // make sure LAYER is set before geometry
    ;}
    break;

  case 680:

/* Line 1464 of yacc.c  */
#line 5711 "lef.y"
    { if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::addClass((yyvsp[(2) - (3)].string)); ;}
    break;

  case 681:

/* Line 1464 of yacc.c  */
#line 5715 "lef.y"
    {(yyval.string) = (char*)"SIGNAL";;}
    break;

  case 682:

/* Line 1464 of yacc.c  */
#line 5716 "lef.y"
    {(yyval.string) = (char*)"ANALOG";;}
    break;

  case 683:

/* Line 1464 of yacc.c  */
#line 5717 "lef.y"
    {(yyval.string) = (char*)"POWER";;}
    break;

  case 684:

/* Line 1464 of yacc.c  */
#line 5718 "lef.y"
    {(yyval.string) = (char*)"GROUND";;}
    break;

  case 685:

/* Line 1464 of yacc.c  */
#line 5719 "lef.y"
    {(yyval.string) = (char*)"CLOCK";;}
    break;

  case 686:

/* Line 1464 of yacc.c  */
#line 5720 "lef.y"
    {(yyval.string) = (char*)"DATA";;}
    break;

  case 687:

/* Line 1464 of yacc.c  */
#line 5723 "lef.y"
    {(yyval.string) = (char*)"INPUT";;}
    break;

  case 688:

/* Line 1464 of yacc.c  */
#line 5724 "lef.y"
    {(yyval.string) = (char*)"OUTPUT";;}
    break;

  case 689:

/* Line 1464 of yacc.c  */
#line 5725 "lef.y"
    {(yyval.string) = (char*)"START";;}
    break;

  case 690:

/* Line 1464 of yacc.c  */
#line 5726 "lef.y"
    {(yyval.string) = (char*)"STOP";;}
    break;

  case 691:

/* Line 1464 of yacc.c  */
#line 5729 "lef.y"
    {(yyval.string) = (char*)""; ;}
    break;

  case 692:

/* Line 1464 of yacc.c  */
#line 5730 "lef.y"
    {(yyval.string) = (char*)"ABUTMENT";;}
    break;

  case 693:

/* Line 1464 of yacc.c  */
#line 5731 "lef.y"
    {(yyval.string) = (char*)"RING";;}
    break;

  case 694:

/* Line 1464 of yacc.c  */
#line 5732 "lef.y"
    {(yyval.string) = (char*)"FEEDTHRU";;}
    break;

  case 696:

/* Line 1464 of yacc.c  */
#line 5737 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 697:

/* Line 1464 of yacc.c  */
#line 5738 "lef.y"
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
        lefrGeometriesPtr->lefiGeometries::addLayer((yyvsp[(3) - (3)].string));
      needGeometry = 1;    // within LAYER it requires either path, rect, poly
      hasGeoLayer = 1;
    ;}
    break;

  case 699:

/* Line 1464 of yacc.c  */
#line 5755 "lef.y"
    { 
      if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else
           lefrGeometriesPtr->lefiGeometries::addWidth((yyvsp[(2) - (3)].dval)); 
      } 
    ;}
    break;

  case 700:

/* Line 1464 of yacc.c  */
#line 5767 "lef.y"
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)(yyvsp[(2) - (5)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
                lefrGeometriesPtr->lefiGeometries::addPath((int)(yyvsp[(2) - (5)].integer));
           }
        }
      }
      hasPRP = 1;
      needGeometry = 2;
    ;}
    break;

  case 701:

/* Line 1464 of yacc.c  */
#line 5788 "lef.y"
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)(yyvsp[(2) - (7)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addPathIter((int)(yyvsp[(2) - (7)].integer));
            }
         }
      } 
      hasPRP = 1;
      needGeometry = 2;
    ;}
    break;

  case 702:

/* Line 1464 of yacc.c  */
#line 5809 "lef.y"
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)(yyvsp[(2) - (5)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addRect((int)(yyvsp[(2) - (5)].integer), (yyvsp[(3) - (5)].pt).x, (yyvsp[(3) - (5)].pt).y, (yyvsp[(4) - (5)].pt).x, (yyvsp[(4) - (5)].pt).y);
           }
        }
      }
      needGeometry = 2;
    ;}
    break;

  case 703:

/* Line 1464 of yacc.c  */
#line 5829 "lef.y"
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)(yyvsp[(2) - (7)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addRectIter((int)(yyvsp[(2) - (7)].integer), (yyvsp[(4) - (7)].pt).x, (yyvsp[(4) - (7)].pt).y, (yyvsp[(5) - (7)].pt).x, (yyvsp[(5) - (7)].pt).y);
           }
        }
      }
      needGeometry = 2;
    ;}
    break;

  case 704:

/* Line 1464 of yacc.c  */
#line 5849 "lef.y"
    {
      if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)(yyvsp[(2) - (7)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addPolygon((int)(yyvsp[(2) - (7)].integer));
            }
           }
      }
      hasPRP = 1;
      needGeometry = 2;
    ;}
    break;

  case 705:

/* Line 1464 of yacc.c  */
#line 5871 "lef.y"
    { if (lefrDoGeometries) {
        if (hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (macroWarnings++ < lefrMacroWarnings) {
              lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              CHKERR();
           }
        } else {
           if (versionNum < 5.8 && (int)(yyvsp[(2) - (9)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
           } else {
              lefrGeometriesPtr->lefiGeometries::addPolygonIter((int)(yyvsp[(2) - (9)].integer));
           }
         }
      }
      hasPRP = 1;
      needGeometry = 2;
    ;}
    break;

  case 706:

/* Line 1464 of yacc.c  */
#line 5892 "lef.y"
    { ;}
    break;

  case 710:

/* Line 1464 of yacc.c  */
#line 5899 "lef.y"
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
    ;}
    break;

  case 712:

/* Line 1464 of yacc.c  */
#line 5915 "lef.y"
    { if (lefrDoGeometries) {
        if (zeroOrGt((yyvsp[(2) - (2)].dval)))
           lefrGeometriesPtr->lefiGeometries::addLayerMinSpacing((yyvsp[(2) - (2)].dval));
        else {
           outMsg = (char*)lefMalloc(10000);
           sprintf (outMsg,
              "THE SPACING statement has the value %g in MACRO OBS.\nValue has to be 0 or greater.", (yyvsp[(2) - (2)].dval));
           lefError(1659, outMsg);
           lefFree(outMsg);
           CHKERR();
        }
      }
    ;}
    break;

  case 713:

/* Line 1464 of yacc.c  */
#line 5929 "lef.y"
    { if (lefrDoGeometries) {
        if (zeroOrGt((yyvsp[(2) - (2)].dval)))
           lefrGeometriesPtr->lefiGeometries::addLayerRuleWidth((yyvsp[(2) - (2)].dval));
        else {
           outMsg = (char*)lefMalloc(10000);
           sprintf (outMsg,
              "THE DESIGNRULEWIDTH statement has the value %g in MACRO OBS.\nValue has to be 0 or greater.", (yyvsp[(2) - (2)].dval));
           lefError(1660, outMsg);
           lefFree(outMsg);
           CHKERR();
        }
      }
    ;}
    break;

  case 714:

/* Line 1464 of yacc.c  */
#line 5944 "lef.y"
    { if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::startList((yyvsp[(1) - (1)].pt).x, (yyvsp[(1) - (1)].pt).y); ;}
    break;

  case 715:

/* Line 1464 of yacc.c  */
#line 5948 "lef.y"
    { if (lefrDoGeometries)
        lefrGeometriesPtr->lefiGeometries::addToList((yyvsp[(1) - (1)].pt).x, (yyvsp[(1) - (1)].pt).y); ;}
    break;

  case 718:

/* Line 1464 of yacc.c  */
#line 5960 "lef.y"
    {lefDumbMode = 1;;}
    break;

  case 719:

/* Line 1464 of yacc.c  */
#line 5961 "lef.y"
    { 
        if (lefrDoGeometries){
            if (versionNum < 5.8 && (int)(yyvsp[(2) - (6)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
            } else {
                lefrGeometriesPtr->lefiGeometries::addVia((int)(yyvsp[(2) - (6)].integer), (yyvsp[(3) - (6)].pt).x, (yyvsp[(3) - (6)].pt).y, (yyvsp[(5) - (6)].string));
            }
        }
    ;}
    break;

  case 720:

/* Line 1464 of yacc.c  */
#line 5973 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 721:

/* Line 1464 of yacc.c  */
#line 5975 "lef.y"
    { 
        if (lefrDoGeometries) {
            if (versionNum < 5.8 && (int)(yyvsp[(3) - (8)].integer) > 0) {
              if (macroWarnings++ < lefrMacroWarnings) {
                 lefError(2083, "Color mask information can only be defined with version 5.8.");
                 CHKERR(); 
              }           
            } else {
              lefrGeometriesPtr->lefiGeometries::addViaIter((int)(yyvsp[(3) - (8)].integer), (yyvsp[(4) - (8)].pt).x, (yyvsp[(4) - (8)].pt).y, (yyvsp[(6) - (8)].string)); 
            }
        }
    ;}
    break;

  case 722:

/* Line 1464 of yacc.c  */
#line 5989 "lef.y"
    { if (lefrDoGeometries)
         lefrGeometriesPtr->lefiGeometries::addStepPattern((yyvsp[(2) - (7)].dval), (yyvsp[(4) - (7)].dval), (yyvsp[(6) - (7)].dval), (yyvsp[(7) - (7)].dval)); ;}
    break;

  case 723:

/* Line 1464 of yacc.c  */
#line 5994 "lef.y"
    {
      if (lefrDoSite) {
	lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
				   sizeof(lefiSitePattern));
	lefrSitePatternPtr->lefiSitePattern::Init();
	lefrSitePatternPtr->lefiSitePattern::set((yyvsp[(1) - (11)].string), (yyvsp[(2) - (11)].dval), (yyvsp[(3) - (11)].dval), (yyvsp[(4) - (11)].integer), (yyvsp[(6) - (11)].dval), (yyvsp[(8) - (11)].dval),
	  (yyvsp[(10) - (11)].dval), (yyvsp[(11) - (11)].dval));
	}
    ;}
    break;

  case 724:

/* Line 1464 of yacc.c  */
#line 6004 "lef.y"
    {
      if (lefrDoSite) {
	lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
				   sizeof(lefiSitePattern));
	lefrSitePatternPtr->lefiSitePattern::Init();
	lefrSitePatternPtr->lefiSitePattern::set((yyvsp[(1) - (4)].string), (yyvsp[(2) - (4)].dval), (yyvsp[(3) - (4)].dval), (yyvsp[(4) - (4)].integer), -1, -1,
	  -1, -1);
	}
    ;}
    break;

  case 725:

/* Line 1464 of yacc.c  */
#line 6016 "lef.y"
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
				sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("X", (yyvsp[(2) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }    
    ;}
    break;

  case 726:

/* Line 1464 of yacc.c  */
#line 6024 "lef.y"
    {lefDumbMode = 1000000000;;}
    break;

  case 727:

/* Line 1464 of yacc.c  */
#line 6025 "lef.y"
    { lefDumbMode = 0;;}
    break;

  case 728:

/* Line 1464 of yacc.c  */
#line 6027 "lef.y"
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("Y", (yyvsp[(2) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }    
    ;}
    break;

  case 729:

/* Line 1464 of yacc.c  */
#line 6035 "lef.y"
    {lefDumbMode = 1000000000;;}
    break;

  case 730:

/* Line 1464 of yacc.c  */
#line 6036 "lef.y"
    { lefDumbMode = 0;;}
    break;

  case 731:

/* Line 1464 of yacc.c  */
#line 6038 "lef.y"
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("X", (yyvsp[(2) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }    
    ;}
    break;

  case 732:

/* Line 1464 of yacc.c  */
#line 6047 "lef.y"
    { 
      if (lefrDoTrack) {
	lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	lefrTrackPatternPtr->lefiTrackPattern::Init();
	lefrTrackPatternPtr->lefiTrackPattern::set("Y", (yyvsp[(2) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }    
    ;}
    break;

  case 735:

/* Line 1464 of yacc.c  */
#line 6062 "lef.y"
    { if (lefrDoTrack) lefrTrackPatternPtr->lefiTrackPattern::addLayer((yyvsp[(1) - (1)].string)); ;}
    break;

  case 736:

/* Line 1464 of yacc.c  */
#line 6065 "lef.y"
    {
      if (lefrDoGcell) {
	lefrGcellPatternPtr = (lefiGcellPattern*)lefMalloc(
                                    sizeof(lefiGcellPattern));
	lefrGcellPatternPtr->lefiGcellPattern::Init();
	lefrGcellPatternPtr->lefiGcellPattern::set("X", (yyvsp[(2) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }    
    ;}
    break;

  case 737:

/* Line 1464 of yacc.c  */
#line 6074 "lef.y"
    {
      if (lefrDoGcell) {
	lefrGcellPatternPtr = (lefiGcellPattern*)lefMalloc(
                                    sizeof(lefiGcellPattern));
	lefrGcellPatternPtr->lefiGcellPattern::Init();
	lefrGcellPatternPtr->lefiGcellPattern::set("Y", (yyvsp[(2) - (6)].dval), (int)(yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }    
    ;}
    break;

  case 738:

/* Line 1464 of yacc.c  */
#line 6084 "lef.y"
    { 
      if (lefrObstructionCbk) {
	lefrObstruction.lefiObstruction::setGeometries(lefrGeometriesPtr);
	lefrGeometriesPtr = 0;
	lefrDoGeometries = 0;
        CALLBACK(lefrObstructionCbk, lefrObstructionCbkType, &lefrObstruction);
      }
      lefDumbMode = 0;
      hasGeoLayer = 0;       /* reset */
    ;}
    break;

  case 739:

/* Line 1464 of yacc.c  */
#line 6102 "lef.y"
    {
       /* The pointer has malloced in start, need to free manually */
       if (lefrGeometriesPtr) {
          lefrGeometriesPtr->lefiGeometries::Destroy();
          lefFree(lefrGeometriesPtr);
          lefrGeometriesPtr = 0;
	  lefrDoGeometries = 0;
       }
       hasGeoLayer = 0;
    ;}
    break;

  case 740:

/* Line 1464 of yacc.c  */
#line 6114 "lef.y"
    {
      obsDef = 1;
      if (lefrObstructionCbk) {
	lefrDoGeometries = 1;
	lefrGeometriesPtr = (lefiGeometries*)lefMalloc(
	    sizeof(lefiGeometries));
	lefrGeometriesPtr->lefiGeometries::Init();
	}
      hasGeoLayer = 0;
    ;}
    break;

  case 741:

/* Line 1464 of yacc.c  */
#line 6126 "lef.y"
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
    ;}
    break;

  case 744:

/* Line 1464 of yacc.c  */
#line 6150 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 745:

/* Line 1464 of yacc.c  */
#line 6151 "lef.y"
    {
      if (lefrDensityCbk)
        lefrDensity.lefiDensity::addLayer((yyvsp[(3) - (4)].string));
    ;}
    break;

  case 749:

/* Line 1464 of yacc.c  */
#line 6162 "lef.y"
    {
      if (lefrDensityCbk)
        lefrDensity.lefiDensity::addRect((yyvsp[(2) - (5)].pt).x, (yyvsp[(2) - (5)].pt).y, (yyvsp[(3) - (5)].pt).x, (yyvsp[(3) - (5)].pt).y, (yyvsp[(4) - (5)].dval)); 
    ;}
    break;

  case 750:

/* Line 1464 of yacc.c  */
#line 6167 "lef.y"
    { lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 751:

/* Line 1464 of yacc.c  */
#line 6168 "lef.y"
    { if (lefrMacroCbk) lefrMacro.lefiMacro::setClockType((yyvsp[(3) - (4)].string)); ;}
    break;

  case 752:

/* Line 1464 of yacc.c  */
#line 6171 "lef.y"
    { ;}
    break;

  case 753:

/* Line 1464 of yacc.c  */
#line 6174 "lef.y"
    { /* XXXXX for macros */ ;}
    break;

  case 754:

/* Line 1464 of yacc.c  */
#line 6177 "lef.y"
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
  ;}
    break;

  case 757:

/* Line 1464 of yacc.c  */
#line 6197 "lef.y"
    {
    if (versionNum < 5.4) {
      if (lefrTimingCbk && lefrTiming.lefiTiming::hasData())
        CALLBACK(lefrTimingCbk, lefrTimingCbkType, &lefrTiming);
    }
    lefDumbMode = 1000000000;
    lefrTiming.lefiTiming::clear();
    ;}
    break;

  case 758:

/* Line 1464 of yacc.c  */
#line 6206 "lef.y"
    { lefDumbMode = 0;;}
    break;

  case 759:

/* Line 1464 of yacc.c  */
#line 6207 "lef.y"
    {lefDumbMode = 1000000000;;}
    break;

  case 760:

/* Line 1464 of yacc.c  */
#line 6208 "lef.y"
    { lefDumbMode = 0;;}
    break;

  case 761:

/* Line 1464 of yacc.c  */
#line 6210 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFall((yyvsp[(1) - (4)].string),(yyvsp[(3) - (4)].dval),(yyvsp[(4) - (4)].dval)); ;}
    break;

  case 762:

/* Line 1464 of yacc.c  */
#line 6212 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallVariable((yyvsp[(8) - (10)].dval),(yyvsp[(9) - (10)].dval)); ;}
    break;

  case 763:

/* Line 1464 of yacc.c  */
#line 6215 "lef.y"
    { if (lefrTimingCbk) {
	if ((yyvsp[(2) - (9)].string)[0] == 'D' || (yyvsp[(2) - (9)].string)[0] == 'd') /* delay */
	  lefrTiming.lefiTiming::addDelay((yyvsp[(1) - (9)].string), (yyvsp[(4) - (9)].string), (yyvsp[(6) - (9)].dval), (yyvsp[(7) - (9)].dval), (yyvsp[(8) - (9)].dval));
	else
	  lefrTiming.lefiTiming::addTransition((yyvsp[(1) - (9)].string), (yyvsp[(4) - (9)].string), (yyvsp[(6) - (9)].dval), (yyvsp[(7) - (9)].dval), (yyvsp[(8) - (9)].dval));
      }
    ;}
    break;

  case 764:

/* Line 1464 of yacc.c  */
#line 6223 "lef.y"
    { ;}
    break;

  case 765:

/* Line 1464 of yacc.c  */
#line 6225 "lef.y"
    { ;}
    break;

  case 766:

/* Line 1464 of yacc.c  */
#line 6227 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseRS((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 767:

/* Line 1464 of yacc.c  */
#line 6229 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallRS((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 768:

/* Line 1464 of yacc.c  */
#line 6231 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseCS((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 769:

/* Line 1464 of yacc.c  */
#line 6233 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallCS((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 770:

/* Line 1464 of yacc.c  */
#line 6235 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseAtt1((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 771:

/* Line 1464 of yacc.c  */
#line 6237 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallAtt1((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 772:

/* Line 1464 of yacc.c  */
#line 6239 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setRiseTo((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 773:

/* Line 1464 of yacc.c  */
#line 6241 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setFallTo((yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval)); ;}
    break;

  case 774:

/* Line 1464 of yacc.c  */
#line 6243 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addUnateness((yyvsp[(2) - (3)].string)); ;}
    break;

  case 775:

/* Line 1464 of yacc.c  */
#line 6245 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setStable((yyvsp[(3) - (7)].dval),(yyvsp[(5) - (7)].dval),(yyvsp[(6) - (7)].string)); ;}
    break;

  case 776:

/* Line 1464 of yacc.c  */
#line 6247 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addSDF2Pins((yyvsp[(1) - (8)].string),(yyvsp[(2) - (8)].string),(yyvsp[(3) - (8)].string),(yyvsp[(5) - (8)].dval),(yyvsp[(6) - (8)].dval),(yyvsp[(7) - (8)].dval)); ;}
    break;

  case 777:

/* Line 1464 of yacc.c  */
#line 6249 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addSDF1Pin((yyvsp[(1) - (6)].string),(yyvsp[(3) - (6)].dval),(yyvsp[(4) - (6)].dval),(yyvsp[(4) - (6)].dval)); ;}
    break;

  case 778:

/* Line 1464 of yacc.c  */
#line 6251 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setSDFcondStart((yyvsp[(2) - (3)].string)); ;}
    break;

  case 779:

/* Line 1464 of yacc.c  */
#line 6253 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setSDFcondEnd((yyvsp[(2) - (3)].string)); ;}
    break;

  case 780:

/* Line 1464 of yacc.c  */
#line 6255 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::setSDFcond((yyvsp[(2) - (3)].string)); ;}
    break;

  case 781:

/* Line 1464 of yacc.c  */
#line 6257 "lef.y"
    { /* XXXXX */ ;}
    break;

  case 782:

/* Line 1464 of yacc.c  */
#line 6261 "lef.y"
    { (yyval.string) = (char*)"MPWH";;}
    break;

  case 783:

/* Line 1464 of yacc.c  */
#line 6263 "lef.y"
    { (yyval.string) = (char*)"MPWL";;}
    break;

  case 784:

/* Line 1464 of yacc.c  */
#line 6265 "lef.y"
    { (yyval.string) = (char*)"PERIOD";;}
    break;

  case 785:

/* Line 1464 of yacc.c  */
#line 6269 "lef.y"
    { (yyval.string) = (char*)"SETUP";;}
    break;

  case 786:

/* Line 1464 of yacc.c  */
#line 6271 "lef.y"
    { (yyval.string) = (char*)"HOLD";;}
    break;

  case 787:

/* Line 1464 of yacc.c  */
#line 6273 "lef.y"
    { (yyval.string) = (char*)"RECOVERY";;}
    break;

  case 788:

/* Line 1464 of yacc.c  */
#line 6275 "lef.y"
    { (yyval.string) = (char*)"SKEW";;}
    break;

  case 789:

/* Line 1464 of yacc.c  */
#line 6279 "lef.y"
    { (yyval.string) = (char*)"ANYEDGE";;}
    break;

  case 790:

/* Line 1464 of yacc.c  */
#line 6281 "lef.y"
    { (yyval.string) = (char*)"POSEDGE";;}
    break;

  case 791:

/* Line 1464 of yacc.c  */
#line 6283 "lef.y"
    { (yyval.string) = (char*)"NEGEDGE";;}
    break;

  case 792:

/* Line 1464 of yacc.c  */
#line 6287 "lef.y"
    { (yyval.string) = (char*)"ANYEDGE";;}
    break;

  case 793:

/* Line 1464 of yacc.c  */
#line 6289 "lef.y"
    { (yyval.string) = (char*)"POSEDGE";;}
    break;

  case 794:

/* Line 1464 of yacc.c  */
#line 6291 "lef.y"
    { (yyval.string) = (char*)"NEGEDGE";;}
    break;

  case 795:

/* Line 1464 of yacc.c  */
#line 6295 "lef.y"
    { (yyval.string) = (char*)"DELAY"; ;}
    break;

  case 796:

/* Line 1464 of yacc.c  */
#line 6297 "lef.y"
    { (yyval.string) = (char*)"TRANSITION"; ;}
    break;

  case 797:

/* Line 1464 of yacc.c  */
#line 6301 "lef.y"
    { ;}
    break;

  case 798:

/* Line 1464 of yacc.c  */
#line 6303 "lef.y"
    { ;}
    break;

  case 799:

/* Line 1464 of yacc.c  */
#line 6306 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addTableEntry((yyvsp[(2) - (5)].dval),(yyvsp[(3) - (5)].dval),(yyvsp[(4) - (5)].dval)); ;}
    break;

  case 800:

/* Line 1464 of yacc.c  */
#line 6310 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addTableAxisNumber((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 801:

/* Line 1464 of yacc.c  */
#line 6312 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addTableAxisNumber((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 802:

/* Line 1464 of yacc.c  */
#line 6316 "lef.y"
    { ;}
    break;

  case 803:

/* Line 1464 of yacc.c  */
#line 6318 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallSlew((yyvsp[(1) - (4)].dval),(yyvsp[(2) - (4)].dval),(yyvsp[(3) - (4)].dval),(yyvsp[(4) - (4)].dval)); ;}
    break;

  case 804:

/* Line 1464 of yacc.c  */
#line 6320 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallSlew((yyvsp[(1) - (7)].dval),(yyvsp[(2) - (7)].dval),(yyvsp[(3) - (7)].dval),(yyvsp[(4) - (7)].dval));
      if (lefrTimingCbk) lefrTiming.lefiTiming::addRiseFallSlew2((yyvsp[(5) - (7)].dval),(yyvsp[(6) - (7)].dval),(yyvsp[(7) - (7)].dval)); ;}
    break;

  case 805:

/* Line 1464 of yacc.c  */
#line 6325 "lef.y"
    { (yyval.string) = (char*)"RISE"; ;}
    break;

  case 806:

/* Line 1464 of yacc.c  */
#line 6327 "lef.y"
    { (yyval.string) = (char*)"FALL"; ;}
    break;

  case 807:

/* Line 1464 of yacc.c  */
#line 6331 "lef.y"
    { (yyval.string) = (char*)"INVERT"; ;}
    break;

  case 808:

/* Line 1464 of yacc.c  */
#line 6333 "lef.y"
    { (yyval.string) = (char*)"NONINVERT"; ;}
    break;

  case 809:

/* Line 1464 of yacc.c  */
#line 6335 "lef.y"
    { (yyval.string) = (char*)"NONUNATE"; ;}
    break;

  case 810:

/* Line 1464 of yacc.c  */
#line 6339 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addFromPin((yyvsp[(1) - (1)].string)); ;}
    break;

  case 811:

/* Line 1464 of yacc.c  */
#line 6341 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addFromPin((yyvsp[(2) - (2)].string)); ;}
    break;

  case 812:

/* Line 1464 of yacc.c  */
#line 6345 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addToPin((yyvsp[(1) - (1)].string)); ;}
    break;

  case 813:

/* Line 1464 of yacc.c  */
#line 6347 "lef.y"
    { if (lefrTimingCbk) lefrTiming.lefiTiming::addToPin((yyvsp[(2) - (2)].string)); ;}
    break;

  case 814:

/* Line 1464 of yacc.c  */
#line 6350 "lef.y"
    {
      if (lefrArrayCbk)
	CALLBACK(lefrArrayCbk, lefrArrayCbkType, &lefrArray);
      lefrArray.lefiArray::clear();
      lefrSitePatternPtr = 0;
      lefrDoSite = 0;
   ;}
    break;

  case 816:

/* Line 1464 of yacc.c  */
#line 6359 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 817:

/* Line 1464 of yacc.c  */
#line 6360 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::setName((yyvsp[(3) - (3)].string));
	CALLBACK(lefrArrayBeginCbk, lefrArrayBeginCbkType, (yyvsp[(3) - (3)].string));
      }
      //strcpy(arrayName, $3);
      arrayName = strdup((yyvsp[(3) - (3)].string));
    ;}
    break;

  case 818:

/* Line 1464 of yacc.c  */
#line 6369 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1;;}
    break;

  case 819:

/* Line 1464 of yacc.c  */
#line 6370 "lef.y"
    {
      if (lefrArrayCbk && lefrArrayEndCbk)
	CALLBACK(lefrArrayEndCbk, lefrArrayEndCbkType, (yyvsp[(3) - (3)].string));
      if (strcmp(arrayName, (yyvsp[(3) - (3)].string)) != 0) {
        if (lefrArrayCbk) { /* write error only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings) {
              outMsg = (char*)lefMalloc(10000);
              sprintf (outMsg,
                 "END ARRAY name %s is different from the ARRAY name %s.\nCorrect the LEF file before rerunning it through the LEF parser.", (yyvsp[(3) - (3)].string), arrayName);
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
    ;}
    break;

  case 820:

/* Line 1464 of yacc.c  */
#line 6393 "lef.y"
    { ;}
    break;

  case 821:

/* Line 1464 of yacc.c  */
#line 6395 "lef.y"
    { ;}
    break;

  case 822:

/* Line 1464 of yacc.c  */
#line 6398 "lef.y"
    { if (lefrArrayCbk) lefrDoSite = 1; lefDumbMode = 1; ;}
    break;

  case 823:

/* Line 1464 of yacc.c  */
#line 6400 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addSitePattern(lefrSitePatternPtr);
      }
    ;}
    break;

  case 824:

/* Line 1464 of yacc.c  */
#line 6405 "lef.y"
    {lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; ;}
    break;

  case 825:

/* Line 1464 of yacc.c  */
#line 6407 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addCanPlace(lefrSitePatternPtr);
      }
    ;}
    break;

  case 826:

/* Line 1464 of yacc.c  */
#line 6412 "lef.y"
    {lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; ;}
    break;

  case 827:

/* Line 1464 of yacc.c  */
#line 6414 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addCannotOccupy(lefrSitePatternPtr);
      }
    ;}
    break;

  case 828:

/* Line 1464 of yacc.c  */
#line 6419 "lef.y"
    { if (lefrArrayCbk) lefrDoTrack = 1; ;}
    break;

  case 829:

/* Line 1464 of yacc.c  */
#line 6420 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addTrack(lefrTrackPatternPtr);
      }
    ;}
    break;

  case 830:

/* Line 1464 of yacc.c  */
#line 6426 "lef.y"
    {
    ;}
    break;

  case 831:

/* Line 1464 of yacc.c  */
#line 6428 "lef.y"
    { if (lefrArrayCbk) lefrDoGcell = 1; ;}
    break;

  case 832:

/* Line 1464 of yacc.c  */
#line 6429 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::addGcell(lefrGcellPatternPtr);
      }
    ;}
    break;

  case 833:

/* Line 1464 of yacc.c  */
#line 6435 "lef.y"
    {
      if (lefrArrayCbk) {
	lefrArray.lefiArray::setTableSize((int)(yyvsp[(2) - (5)].dval));
      }
    ;}
    break;

  case 834:

/* Line 1464 of yacc.c  */
#line 6441 "lef.y"
    { ;}
    break;

  case 835:

/* Line 1464 of yacc.c  */
#line 6444 "lef.y"
    { if (lefrArrayCbk) lefrArray.lefiArray::addFloorPlan((yyvsp[(2) - (2)].string)); ;}
    break;

  case 836:

/* Line 1464 of yacc.c  */
#line 6448 "lef.y"
    { ;}
    break;

  case 837:

/* Line 1464 of yacc.c  */
#line 6450 "lef.y"
    { ;}
    break;

  case 838:

/* Line 1464 of yacc.c  */
#line 6453 "lef.y"
    { lefDumbMode = 1; if (lefrArrayCbk) lefrDoSite = 1; ;}
    break;

  case 839:

/* Line 1464 of yacc.c  */
#line 6455 "lef.y"
    {
      if (lefrArrayCbk)
	lefrArray.lefiArray::addSiteToFloorPlan("CANPLACE",
	lefrSitePatternPtr);
    ;}
    break;

  case 840:

/* Line 1464 of yacc.c  */
#line 6460 "lef.y"
    { if (lefrArrayCbk) lefrDoSite = 1; lefDumbMode = 1; ;}
    break;

  case 841:

/* Line 1464 of yacc.c  */
#line 6462 "lef.y"
    {
      if (lefrArrayCbk)
	lefrArray.lefiArray::addSiteToFloorPlan("CANNOTOCCUPY",
	lefrSitePatternPtr);
     ;}
    break;

  case 842:

/* Line 1464 of yacc.c  */
#line 6470 "lef.y"
    { ;}
    break;

  case 843:

/* Line 1464 of yacc.c  */
#line 6472 "lef.y"
    { ;}
    break;

  case 844:

/* Line 1464 of yacc.c  */
#line 6475 "lef.y"
    { if (lefrArrayCbk) lefrArray.lefiArray::addDefaultCap((int)(yyvsp[(2) - (5)].dval), (yyvsp[(4) - (5)].dval)); ;}
    break;

  case 845:

/* Line 1464 of yacc.c  */
#line 6478 "lef.y"
    {lefDumbMode=1;lefNlToken=TRUE;;}
    break;

  case 846:

/* Line 1464 of yacc.c  */
#line 6479 "lef.y"
    { lefAddStringMessage((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string)); ;}
    break;

  case 847:

/* Line 1464 of yacc.c  */
#line 6482 "lef.y"
    {lefDumbMode=1;lefNlToken=TRUE;;}
    break;

  case 848:

/* Line 1464 of yacc.c  */
#line 6483 "lef.y"
    { ;}
    break;

  case 849:

/* Line 1464 of yacc.c  */
#line 6486 "lef.y"
    {lefDumbMode=1;lefNlToken=TRUE;;}
    break;

  case 850:

/* Line 1464 of yacc.c  */
#line 6487 "lef.y"
    {
      if (versionNum < 5.6)
        lefAddNumDefine((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].dval));
      else
        if (lefrArrayCbk) /* write warning only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings)
             lefWarning(2067, "DEFINE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 851:

/* Line 1464 of yacc.c  */
#line 6495 "lef.y"
    {lefDumbMode=1;lefNlToken=TRUE;;}
    break;

  case 852:

/* Line 1464 of yacc.c  */
#line 6496 "lef.y"
    {
      if (versionNum < 5.6)
        lefAddStringDefine((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string));
      else
        if (lefrArrayCbk) /* write warning only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings)
             lefWarning(2068, "DEFINES statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 853:

/* Line 1464 of yacc.c  */
#line 6504 "lef.y"
    {lefDumbMode=1;lefNlToken=TRUE;;}
    break;

  case 854:

/* Line 1464 of yacc.c  */
#line 6505 "lef.y"
    {
      if (versionNum < 5.6)
        lefAddBooleanDefine((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].integer));
      else
        if (lefrArrayCbk) /* write warning only if cbk is set */
           if (arrayWarnings++ < lefrArrayWarnings)
             lefWarning(2069, "DEFINEB statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    ;}
    break;

  case 856:

/* Line 1464 of yacc.c  */
#line 6516 "lef.y"
    {lefNlToken = FALSE;;}
    break;

  case 857:

/* Line 1464 of yacc.c  */
#line 6517 "lef.y"
    {lefNlToken = FALSE;;}
    break;

  case 862:

/* Line 1464 of yacc.c  */
#line 6530 "lef.y"
    {(yyval.dval) = (yyvsp[(1) - (3)].dval) + (yyvsp[(3) - (3)].dval); ;}
    break;

  case 863:

/* Line 1464 of yacc.c  */
#line 6531 "lef.y"
    {(yyval.dval) = (yyvsp[(1) - (3)].dval) - (yyvsp[(3) - (3)].dval); ;}
    break;

  case 864:

/* Line 1464 of yacc.c  */
#line 6532 "lef.y"
    {(yyval.dval) = (yyvsp[(1) - (3)].dval) * (yyvsp[(3) - (3)].dval); ;}
    break;

  case 865:

/* Line 1464 of yacc.c  */
#line 6533 "lef.y"
    {(yyval.dval) = (yyvsp[(1) - (3)].dval) / (yyvsp[(3) - (3)].dval); ;}
    break;

  case 866:

/* Line 1464 of yacc.c  */
#line 6534 "lef.y"
    {(yyval.dval) = -(yyvsp[(2) - (2)].dval);;}
    break;

  case 867:

/* Line 1464 of yacc.c  */
#line 6535 "lef.y"
    {(yyval.dval) = (yyvsp[(2) - (3)].dval);;}
    break;

  case 868:

/* Line 1464 of yacc.c  */
#line 6537 "lef.y"
    {(yyval.dval) = ((yyvsp[(2) - (6)].integer) != 0) ? (yyvsp[(4) - (6)].dval) : (yyvsp[(6) - (6)].dval);;}
    break;

  case 869:

/* Line 1464 of yacc.c  */
#line 6538 "lef.y"
    {(yyval.dval) = (yyvsp[(1) - (1)].dval);;}
    break;

  case 870:

/* Line 1464 of yacc.c  */
#line 6541 "lef.y"
    {(yyval.integer) = comp_num((yyvsp[(1) - (3)].dval),(yyvsp[(2) - (3)].integer),(yyvsp[(3) - (3)].dval));;}
    break;

  case 871:

/* Line 1464 of yacc.c  */
#line 6542 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].dval) != 0 && (yyvsp[(3) - (3)].dval) != 0;;}
    break;

  case 872:

/* Line 1464 of yacc.c  */
#line 6543 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].dval) != 0 || (yyvsp[(3) - (3)].dval) != 0;;}
    break;

  case 873:

/* Line 1464 of yacc.c  */
#line 6544 "lef.y"
    {(yyval.integer) = comp_str((yyvsp[(1) - (3)].string),(yyvsp[(2) - (3)].integer),(yyvsp[(3) - (3)].string));;}
    break;

  case 874:

/* Line 1464 of yacc.c  */
#line 6545 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].string)[0] != 0 && (yyvsp[(3) - (3)].string)[0] != 0;;}
    break;

  case 875:

/* Line 1464 of yacc.c  */
#line 6546 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].string)[0] != 0 || (yyvsp[(3) - (3)].string)[0] != 0;;}
    break;

  case 876:

/* Line 1464 of yacc.c  */
#line 6547 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].integer) == (yyvsp[(3) - (3)].integer);;}
    break;

  case 877:

/* Line 1464 of yacc.c  */
#line 6548 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].integer) != (yyvsp[(3) - (3)].integer);;}
    break;

  case 878:

/* Line 1464 of yacc.c  */
#line 6549 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].integer) && (yyvsp[(3) - (3)].integer);;}
    break;

  case 879:

/* Line 1464 of yacc.c  */
#line 6550 "lef.y"
    {(yyval.integer) = (yyvsp[(1) - (3)].integer) || (yyvsp[(3) - (3)].integer);;}
    break;

  case 880:

/* Line 1464 of yacc.c  */
#line 6551 "lef.y"
    {(yyval.integer) = !(yyval.integer);;}
    break;

  case 881:

/* Line 1464 of yacc.c  */
#line 6552 "lef.y"
    {(yyval.integer) = (yyvsp[(2) - (3)].integer);;}
    break;

  case 882:

/* Line 1464 of yacc.c  */
#line 6554 "lef.y"
    {(yyval.integer) = ((yyvsp[(2) - (6)].integer) != 0) ? (yyvsp[(4) - (6)].integer) : (yyvsp[(6) - (6)].integer);;}
    break;

  case 883:

/* Line 1464 of yacc.c  */
#line 6555 "lef.y"
    {(yyval.integer) = 1;;}
    break;

  case 884:

/* Line 1464 of yacc.c  */
#line 6556 "lef.y"
    {(yyval.integer) = 0;;}
    break;

  case 885:

/* Line 1464 of yacc.c  */
#line 6560 "lef.y"
    {
      (yyval.string) = (char*)lefMalloc(strlen((yyvsp[(1) - (3)].string))+strlen((yyvsp[(3) - (3)].string))+1);
      strcpy((yyval.string),(yyvsp[(1) - (3)].string));
      strcat((yyval.string),(yyvsp[(3) - (3)].string));
    ;}
    break;

  case 886:

/* Line 1464 of yacc.c  */
#line 6566 "lef.y"
    { (yyval.string) = (yyvsp[(2) - (3)].string); ;}
    break;

  case 887:

/* Line 1464 of yacc.c  */
#line 6568 "lef.y"
    {
      lefDefIf = TRUE;
      if ((yyvsp[(2) - (6)].integer) != 0) {
	(yyval.string) = (yyvsp[(4) - (6)].string);	
      } else {
	(yyval.string) = (yyvsp[(6) - (6)].string);
      }
    ;}
    break;

  case 888:

/* Line 1464 of yacc.c  */
#line 6577 "lef.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 889:

/* Line 1464 of yacc.c  */
#line 6580 "lef.y"
    {(yyval.integer) = C_LE;;}
    break;

  case 890:

/* Line 1464 of yacc.c  */
#line 6581 "lef.y"
    {(yyval.integer) = C_LT;;}
    break;

  case 891:

/* Line 1464 of yacc.c  */
#line 6582 "lef.y"
    {(yyval.integer) = C_GE;;}
    break;

  case 892:

/* Line 1464 of yacc.c  */
#line 6583 "lef.y"
    {(yyval.integer) = C_GT;;}
    break;

  case 893:

/* Line 1464 of yacc.c  */
#line 6584 "lef.y"
    {(yyval.integer) = C_EQ;;}
    break;

  case 894:

/* Line 1464 of yacc.c  */
#line 6585 "lef.y"
    {(yyval.integer) = C_NE;;}
    break;

  case 895:

/* Line 1464 of yacc.c  */
#line 6586 "lef.y"
    {(yyval.integer) = C_EQ;;}
    break;

  case 896:

/* Line 1464 of yacc.c  */
#line 6587 "lef.y"
    {(yyval.integer) = C_LT;;}
    break;

  case 897:

/* Line 1464 of yacc.c  */
#line 6588 "lef.y"
    {(yyval.integer) = C_GT;;}
    break;

  case 898:

/* Line 1464 of yacc.c  */
#line 6592 "lef.y"
    { 
      if (lefrPropBeginCbk)
        CALLBACK(lefrPropBeginCbk, lefrPropBeginCbkType, 0);
      lefInPropDef = 1;  /* set flag as inside propertydefinitions */
    ;}
    break;

  case 899:

/* Line 1464 of yacc.c  */
#line 6598 "lef.y"
    { 
      if (lefrPropEndCbk)
        CALLBACK(lefrPropEndCbk, lefrPropEndCbkType, 0);
      lefRealNum = 0;     /* just want to make sure it is reset */
      lefInPropDef = 0;   /* reset flag */
    ;}
    break;

  case 900:

/* Line 1464 of yacc.c  */
#line 6607 "lef.y"
    { ;}
    break;

  case 901:

/* Line 1464 of yacc.c  */
#line 6609 "lef.y"
    { ;}
    break;

  case 902:

/* Line 1464 of yacc.c  */
#line 6612 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 903:

/* Line 1464 of yacc.c  */
#line 6614 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("library", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrLibProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 904:

/* Line 1464 of yacc.c  */
#line 6621 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 905:

/* Line 1464 of yacc.c  */
#line 6623 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("componentpin", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrCompProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 906:

/* Line 1464 of yacc.c  */
#line 6630 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 907:

/* Line 1464 of yacc.c  */
#line 6632 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("pin", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrPinProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
      
    ;}
    break;

  case 908:

/* Line 1464 of yacc.c  */
#line 6640 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 909:

/* Line 1464 of yacc.c  */
#line 6642 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("macro", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrMacroProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 910:

/* Line 1464 of yacc.c  */
#line 6649 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 911:

/* Line 1464 of yacc.c  */
#line 6651 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("via", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrViaProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 912:

/* Line 1464 of yacc.c  */
#line 6658 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 913:

/* Line 1464 of yacc.c  */
#line 6660 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("viarule", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrViaRuleProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 914:

/* Line 1464 of yacc.c  */
#line 6667 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 915:

/* Line 1464 of yacc.c  */
#line 6669 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("layer", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrLayerProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 916:

/* Line 1464 of yacc.c  */
#line 6676 "lef.y"
    {lefDumbMode = 1; lefrProp.lefiProp::clear(); ;}
    break;

  case 917:

/* Line 1464 of yacc.c  */
#line 6678 "lef.y"
    { 
      if (lefrPropCbk) {
        lefrProp.lefiProp::setPropType("nondefaultrule", (yyvsp[(3) - (5)].string));
        CALLBACK(lefrPropCbk, lefrPropCbkType, &lefrProp);
      }
      lefrNondefProp.lefiPropType::setPropType((yyvsp[(3) - (5)].string), lefPropDefType);
    ;}
    break;

  case 918:

/* Line 1464 of yacc.c  */
#line 6687 "lef.y"
    { lefRealNum = 0 ;}
    break;

  case 919:

/* Line 1464 of yacc.c  */
#line 6688 "lef.y"
    { 
      if (lefrPropCbk) lefrProp.lefiProp::setPropInteger();
      lefPropDefType = 'I'
    ;}
    break;

  case 920:

/* Line 1464 of yacc.c  */
#line 6692 "lef.y"
    { lefRealNum = 1 ;}
    break;

  case 921:

/* Line 1464 of yacc.c  */
#line 6693 "lef.y"
    { 
      if (lefrPropCbk) lefrProp.lefiProp::setPropReal();
      lefPropDefType = 'R';
      lefRealNum = 0;
    ;}
    break;

  case 922:

/* Line 1464 of yacc.c  */
#line 6699 "lef.y"
    {
      if (lefrPropCbk) lefrProp.lefiProp::setPropString();
      lefPropDefType = 'S'
    ;}
    break;

  case 923:

/* Line 1464 of yacc.c  */
#line 6704 "lef.y"
    {
      if (lefrPropCbk) lefrProp.lefiProp::setPropQString((yyvsp[(2) - (2)].string));
      lefPropDefType = 'Q'
    ;}
    break;

  case 924:

/* Line 1464 of yacc.c  */
#line 6709 "lef.y"
    {
      if (lefrPropCbk) lefrProp.lefiProp::setPropNameMapString((yyvsp[(2) - (2)].string));
      lefPropDefType = 'S'
    ;}
    break;

  case 925:

/* Line 1464 of yacc.c  */
#line 6716 "lef.y"
    { ;}
    break;

  case 926:

/* Line 1464 of yacc.c  */
#line 6718 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingRangeUseLength();
    ;}
    break;

  case 927:

/* Line 1464 of yacc.c  */
#line 6723 "lef.y"
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingRangeInfluence((yyvsp[(2) - (2)].dval));
        lefrLayer.lefiLayer::setSpacingRangeInfluenceRange(-1, -1);
      }
    ;}
    break;

  case 928:

/* Line 1464 of yacc.c  */
#line 6730 "lef.y"
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingRangeInfluence((yyvsp[(2) - (5)].dval));
        lefrLayer.lefiLayer::setSpacingRangeInfluenceRange((yyvsp[(4) - (5)].dval), (yyvsp[(5) - (5)].dval));
      }
    ;}
    break;

  case 929:

/* Line 1464 of yacc.c  */
#line 6737 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingRangeRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval));
    ;}
    break;

  case 930:

/* Line 1464 of yacc.c  */
#line 6744 "lef.y"
    { ;}
    break;

  case 931:

/* Line 1464 of yacc.c  */
#line 6746 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingParSW((yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval));
    ;}
    break;

  case 933:

/* Line 1464 of yacc.c  */
#line 6754 "lef.y"
    { ;}
    break;

  case 934:

/* Line 1464 of yacc.c  */
#line 6756 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingParTwoEdges();
    ;}
    break;

  case 935:

/* Line 1464 of yacc.c  */
#line 6763 "lef.y"
    { ;}
    break;

  case 936:

/* Line 1464 of yacc.c  */
#line 6765 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingSamenetPGonly();
    ;}
    break;

  case 937:

/* Line 1464 of yacc.c  */
#line 6772 "lef.y"
    { ;}
    break;

  case 938:

/* Line 1464 of yacc.c  */
#line 6774 "lef.y"
    {  if (lefrPropCbk) lefrProp.lefiProp::setRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval)); ;}
    break;

  case 939:

/* Line 1464 of yacc.c  */
#line 6778 "lef.y"
    { ;}
    break;

  case 940:

/* Line 1464 of yacc.c  */
#line 6780 "lef.y"
    { if (lefrPropCbk) lefrProp.lefiProp::setNumber((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 943:

/* Line 1464 of yacc.c  */
#line 6787 "lef.y"
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
    ;}
    break;

  case 944:

/* Line 1464 of yacc.c  */
#line 6811 "lef.y"
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
    ;}
    break;

  case 945:

/* Line 1464 of yacc.c  */
#line 6825 "lef.y"
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "SAMENET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1684, outMsg);
        lefFree(outMsg);
        CHKERR();
      }
    ;}
    break;

  case 946:

/* Line 1464 of yacc.c  */
#line 6836 "lef.y"
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
    ;}
    break;

  case 948:

/* Line 1464 of yacc.c  */
#line 6861 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 949:

/* Line 1464 of yacc.c  */
#line 6862 "lef.y"
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
        lefrLayer.lefiLayer::setSpacingName((yyvsp[(3) - (3)].string));
      }
    ;}
    break;

  case 951:

/* Line 1464 of yacc.c  */
#line 6878 "lef.y"
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
        lefrLayer.lefiLayer::setSpacingAdjacent((int)(yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval));
      }
    ;}
    break;

  case 953:

/* Line 1464 of yacc.c  */
#line 6903 "lef.y"
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
          lefrLayer.lefiLayer::setSpacingArea((yyvsp[(2) - (2)].dval));
        }
      }
    ;}
    break;

  case 954:

/* Line 1464 of yacc.c  */
#line 6926 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingRange((yyvsp[(2) - (3)].dval), (yyvsp[(3) - (3)].dval));
    ;}
    break;

  case 956:

/* Line 1464 of yacc.c  */
#line 6932 "lef.y"
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingLength((yyvsp[(2) - (2)].dval));
      }
    ;}
    break;

  case 957:

/* Line 1464 of yacc.c  */
#line 6938 "lef.y"
    {
      if (lefrLayerCbk) {
        lefrLayer.lefiLayer::setSpacingLength((yyvsp[(2) - (5)].dval));
        lefrLayer.lefiLayer::setSpacingLengthRange((yyvsp[(4) - (5)].dval), (yyvsp[(5) - (5)].dval));
      }
    ;}
    break;

  case 958:

/* Line 1464 of yacc.c  */
#line 6945 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingEol((yyvsp[(2) - (4)].dval), (yyvsp[(4) - (4)].dval));
    ;}
    break;

  case 959:

/* Line 1464 of yacc.c  */
#line 6950 "lef.y"
    {
      if (versionNum < 5.7) {
        outMsg = (char*)lefMalloc(10000);
        sprintf(outMsg,
          "ENDOFLINE is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", versionNum);
        lefError(1681, outMsg);
        lefFree(outMsg);
        CHKERR();
      }
    ;}
    break;

  case 960:

/* Line 1464 of yacc.c  */
#line 6961 "lef.y"
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
          lefrLayer.lefiLayer::setSpacingNotchLength((yyvsp[(2) - (2)].dval));
      }
    ;}
    break;

  case 961:

/* Line 1464 of yacc.c  */
#line 6975 "lef.y"
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
          lefrLayer.lefiLayer::setSpacingEndOfNotchWidth((yyvsp[(2) - (6)].dval), (yyvsp[(4) - (6)].dval), (yyvsp[(6) - (6)].dval));
      }
    ;}
    break;

  case 962:

/* Line 1464 of yacc.c  */
#line 6991 "lef.y"
    {;}
    break;

  case 963:

/* Line 1464 of yacc.c  */
#line 6993 "lef.y"
    {
      if (lefrLayerCbk)
        lefrLayer.lefiLayer::setSpacingLayerStack();
    ;}
    break;

  case 964:

/* Line 1464 of yacc.c  */
#line 7000 "lef.y"
    {;}
    break;

  case 965:

/* Line 1464 of yacc.c  */
#line 7002 "lef.y"
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
    ;}
    break;

  case 966:

/* Line 1464 of yacc.c  */
#line 7018 "lef.y"
    { (yyval.string) = 0; ;}
    break;

  case 967:

/* Line 1464 of yacc.c  */
#line 7019 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 968:

/* Line 1464 of yacc.c  */
#line 7020 "lef.y"
    { (yyval.string) = (yyvsp[(3) - (3)].string); ;}
    break;

  case 969:

/* Line 1464 of yacc.c  */
#line 7024 "lef.y"
    {lefDumbMode = 1; lefNoNum = 1; ;}
    break;

  case 970:

/* Line 1464 of yacc.c  */
#line 7025 "lef.y"
    { (yyval.string) = (yyvsp[(3) - (3)].string); ;}
    break;

  case 971:

/* Line 1464 of yacc.c  */
#line 7029 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrNoiseMarginCbk) {
          lefrNoiseMargin.low = (yyvsp[(2) - (4)].dval);
          lefrNoiseMargin.high = (yyvsp[(3) - (4)].dval);
          CALLBACK(lefrNoiseMarginCbk, lefrNoiseMarginCbkType, &lefrNoiseMargin);
        }
      } else
        if (lefrNoiseMarginCbk) /* write warning only if cbk is set */
          if (noiseMarginWarnings++ < lefrNoiseMarginWarnings)
            lefWarning(2070, "UNIVERSALNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 972:

/* Line 1464 of yacc.c  */
#line 7043 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrEdgeRateThreshold1Cbk) {
          CALLBACK(lefrEdgeRateThreshold1Cbk,
          lefrEdgeRateThreshold1CbkType, (yyvsp[(2) - (3)].dval));
        }
      } else
        if (lefrEdgeRateThreshold1Cbk) /* write warning only if cbk is set */
          if (edgeRateThreshold1Warnings++ < lefrEdgeRateThreshold1Warnings)
            lefWarning(2071, "EDGERATETHRESHOLD1 statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 973:

/* Line 1464 of yacc.c  */
#line 7056 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrEdgeRateThreshold2Cbk) {
	  CALLBACK(lefrEdgeRateThreshold2Cbk,
	  lefrEdgeRateThreshold2CbkType, (yyvsp[(2) - (3)].dval));
        }
      } else
        if (lefrEdgeRateThreshold2Cbk) /* write warning only if cbk is set */
          if (edgeRateThreshold2Warnings++ < lefrEdgeRateThreshold2Warnings)
            lefWarning(2072, "EDGERATETHRESHOLD2 statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 974:

/* Line 1464 of yacc.c  */
#line 7069 "lef.y"
    {
      if (versionNum < 5.4) {
        if (lefrEdgeRateScaleFactorCbk) {
	  CALLBACK(lefrEdgeRateScaleFactorCbk,
	  lefrEdgeRateScaleFactorCbkType, (yyvsp[(2) - (3)].dval));
        }
      } else
        if (lefrEdgeRateScaleFactorCbk) /* write warning only if cbk is set */
          if (edgeRateScaleFactorWarnings++ < lefrEdgeRateScaleFactorWarnings)
            lefWarning(2073, "EDGERATESCALEFACTOR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    ;}
    break;

  case 975:

/* Line 1464 of yacc.c  */
#line 7082 "lef.y"
    { if (lefrNoiseTableCbk) lefrNoiseTable.lefiNoiseTable::setup((int)(yyvsp[(2) - (2)].dval)); ;}
    break;

  case 976:

/* Line 1464 of yacc.c  */
#line 7084 "lef.y"
    { ;}
    break;

  case 977:

/* Line 1464 of yacc.c  */
#line 7088 "lef.y"
    {
    if (versionNum < 5.4) {
      if (lefrNoiseTableCbk)
        CALLBACK(lefrNoiseTableCbk, lefrNoiseTableCbkType, &lefrNoiseTable);
    } else
      if (lefrNoiseTableCbk) /* write warning only if cbk is set */
        if (noiseTableWarnings++ < lefrNoiseTableWarnings)
          lefWarning(2074, "NOISETABLE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  ;}
    break;

  case 980:

/* Line 1464 of yacc.c  */
#line 7106 "lef.y"
    { if (lefrNoiseTableCbk)
         {
            lefrNoiseTable.lefiNoiseTable::newEdge();
            lefrNoiseTable.lefiNoiseTable::addEdge((yyvsp[(2) - (3)].dval));
         }
    ;}
    break;

  case 981:

/* Line 1464 of yacc.c  */
#line 7113 "lef.y"
    { ;}
    break;

  case 982:

/* Line 1464 of yacc.c  */
#line 7116 "lef.y"
    { if (lefrNoiseTableCbk) lefrNoiseTable.lefiNoiseTable::addResistance(); ;}
    break;

  case 984:

/* Line 1464 of yacc.c  */
#line 7122 "lef.y"
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addResistanceNumber((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 985:

/* Line 1464 of yacc.c  */
#line 7125 "lef.y"
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addResistanceNumber((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 988:

/* Line 1464 of yacc.c  */
#line 7134 "lef.y"
    { if (lefrNoiseTableCbk)
	lefrNoiseTable.lefiNoiseTable::addVictimLength((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 989:

/* Line 1464 of yacc.c  */
#line 7137 "lef.y"
    { ;}
    break;

  case 990:

/* Line 1464 of yacc.c  */
#line 7141 "lef.y"
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addVictimNoise((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 991:

/* Line 1464 of yacc.c  */
#line 7144 "lef.y"
    { if (lefrNoiseTableCbk)
    lefrNoiseTable.lefiNoiseTable::addVictimNoise((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 992:

/* Line 1464 of yacc.c  */
#line 7148 "lef.y"
    { if (lefrCorrectionTableCbk)
    lefrCorrectionTable.lefiCorrectionTable::setup((int)(yyvsp[(2) - (3)].dval)); ;}
    break;

  case 993:

/* Line 1464 of yacc.c  */
#line 7151 "lef.y"
    { ;}
    break;

  case 994:

/* Line 1464 of yacc.c  */
#line 7155 "lef.y"
    {
    if (versionNum < 5.4) {
      if (lefrCorrectionTableCbk)
        CALLBACK(lefrCorrectionTableCbk, lefrCorrectionTableCbkType,
               &lefrCorrectionTable);
    } else
      if (lefrCorrectionTableCbk) /* write warning only if cbk is set */
        if (correctionTableWarnings++ < lefrCorrectionTableWarnings)
          lefWarning(2075, "CORRECTIONTABLE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  ;}
    break;

  case 997:

/* Line 1464 of yacc.c  */
#line 7173 "lef.y"
    { if (lefrCorrectionTableCbk)
         {
            lefrCorrectionTable.lefiCorrectionTable::newEdge();
            lefrCorrectionTable.lefiCorrectionTable::addEdge((yyvsp[(2) - (3)].dval));
         }
    ;}
    break;

  case 998:

/* Line 1464 of yacc.c  */
#line 7180 "lef.y"
    { ;}
    break;

  case 999:

/* Line 1464 of yacc.c  */
#line 7183 "lef.y"
    { if (lefrCorrectionTableCbk)
  lefrCorrectionTable.lefiCorrectionTable::addResistance(); ;}
    break;

  case 1000:

/* Line 1464 of yacc.c  */
#line 7186 "lef.y"
    { ;}
    break;

  case 1001:

/* Line 1464 of yacc.c  */
#line 7190 "lef.y"
    { if (lefrCorrectionTableCbk)
    lefrCorrectionTable.lefiCorrectionTable::addResistanceNumber((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 1002:

/* Line 1464 of yacc.c  */
#line 7193 "lef.y"
    { if (lefrCorrectionTableCbk)
    lefrCorrectionTable.lefiCorrectionTable::addResistanceNumber((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 1005:

/* Line 1464 of yacc.c  */
#line 7203 "lef.y"
    { if (lefrCorrectionTableCbk)
     lefrCorrectionTable.lefiCorrectionTable::addVictimLength((yyvsp[(2) - (3)].dval)); ;}
    break;

  case 1006:

/* Line 1464 of yacc.c  */
#line 7206 "lef.y"
    { ;}
    break;

  case 1007:

/* Line 1464 of yacc.c  */
#line 7210 "lef.y"
    { if (lefrCorrectionTableCbk)
	lefrCorrectionTable.lefiCorrectionTable::addVictimCorrection((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 1008:

/* Line 1464 of yacc.c  */
#line 7213 "lef.y"
    { if (lefrCorrectionTableCbk)
	lefrCorrectionTable.lefiCorrectionTable::addVictimCorrection((yyvsp[(2) - (2)].dval)); ;}
    break;

  case 1009:

/* Line 1464 of yacc.c  */
#line 7219 "lef.y"
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
          CALLBACK(lefrInputAntennaCbk, lefrInputAntennaCbkType, (yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 1010:

/* Line 1464 of yacc.c  */
#line 7243 "lef.y"
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
          CALLBACK(lefrOutputAntennaCbk, lefrOutputAntennaCbkType, (yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 1011:

/* Line 1464 of yacc.c  */
#line 7267 "lef.y"
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
          CALLBACK(lefrInoutAntennaCbk, lefrInoutAntennaCbkType, (yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 1012:

/* Line 1464 of yacc.c  */
#line 7291 "lef.y"
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
          CALLBACK(lefrAntennaInputCbk, lefrAntennaInputCbkType, (yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 1013:

/* Line 1464 of yacc.c  */
#line 7325 "lef.y"
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
          CALLBACK(lefrAntennaInoutCbk, lefrAntennaInoutCbkType, (yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 1014:

/* Line 1464 of yacc.c  */
#line 7359 "lef.y"
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
          CALLBACK(lefrAntennaOutputCbk, lefrAntennaOutputCbkType, (yyvsp[(2) - (3)].dval));
    ;}
    break;

  case 1017:

/* Line 1464 of yacc.c  */
#line 7396 "lef.y"
    { 
        if (lefrExtensionCbk)
          CALLBACK(lefrExtensionCbk, lefrExtensionCbkType, Hist_text);
        if (versionNum >= 5.6)
           ge56almostDone = 1;
    ;}
    break;



/* Line 1464 of yacc.c  */
#line 14697 "lef.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1684 of yacc.c  */
#line 7403 "lef.y"


