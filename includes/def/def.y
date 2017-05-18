/*************************************************************************
 * Copyright 2012, Cadence Design Systems
 *
 * This file is part of the Cadence LEF/DEF Open Source Distribution,
 * Product Version 5.8. 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
 * implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * For updates, support, or to become part of the LEF/DEF Community,
 * check www.openeda.org for details.
 *
 *
 *  Error message number:
 *  5000 - def reader, defrReader.cpp
 *  5500 - lex.cpph, yyerror
 *  6000 - def parser, error, lex.cpph, def.y (CALLBACK & CHKERR)
 *  6010 - defiBlockage.cpp
 *  6020 - defiComponent.cpp
 *  6030 - defiFPC.cpp
 *  6040 - defiFill.cpp
 *  6050 - defiGroup.cpp
 *  6060 - defiIOTiming.cpp
 *  6070 - defiMisc.cpp
 *  6080 - defiNet.cpp
 *  6090 - defiNonDefault.cpp
 *  6100 - defiPartition.cpp
 *  6110 - defiPinCap.cpp
 *  6120 - defiPinProp.cpp
 *  6130 - defiRegion.cpp
 *  6140 - defiRowTrack.cpp
 *  6150 - defiScanchain.cpp
 *  6160 - defiSlot.cpp
 *  6170 - defiTimingDisable.cpp
 *  6180 - defiVia.cpp
 *  6500 - def parser, error, def.y
 *  7000 - def parser, warning, lex.cpph
 *  7500 - def parser, warning, lef.y
 *  8000 - def parser, info, lex.cpph
 *  9000 - def writer
 */

%{
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "defrReader.hpp"
#include "lex.h"
#include "defiUser.hpp"
#include "defrCallBacks.hpp"

#define YYDEBUG 1         /* this is temp fix for pcr 755132 */
// TX_DIR:TRANSLATION ON

extern int dumb_mode;     // Read next N tokens in dumb mode
extern int no_num;        // No numbers for the next N tokens
extern int real_num;      // Next token will be a real number
extern int by_is_keyword;
extern int bit_is_keyword;
extern int do_is_keyword;
extern int new_is_keyword;
extern int nondef_is_keyword;
extern int step_is_keyword;
extern int mustjoin_is_keyword;
extern int routed_is_keyword;
extern int cover_is_keyword;
extern int fixed_is_keyword;
extern int orient_is_keyword;
extern int rect_is_keyword;
extern int mask_is_keyword;
extern int virtual_is_keyword;
extern char* History_text;
extern int parsing_property;
extern int doneDesign;

extern int errors;        // from lex.cpph
extern int nlines;        // from lex.cpph
extern int defInPropDef;  // from lex.cpph, tracking if insided propDef


double save_x;
double save_y;
int    specialWire_mask = 0;
char*  specialWire_routeStatus = (char*)"ROUTED";
char*  specialWire_shapeType = (char*)"";
char*  specialWire_routeStatusName = (char*)"";


static double defVersionNum = 5.7;   // default is 5.7

static int defIgnoreVersion = 0; // ignore checking version number
                                 // this is redundant, since def does not have
                                 // either 5.3 or 5.4 code

static int hasVer = 0;        // keep track VERSION is in the file
static int hasNameCase = 0;   // keep track NAMESCASESENSITIVE is in the file
static int hasDivChar = 0;    // keep track DIVIDERCHAR is in the file
static int hasBusBit = 0;     // keep track BUSBITCHARS is in the file
static int hasDes = 0;        // keep track DESIGN is in the file
static int regTypeDef;        // keep track that region type is defined 
static int aOxide = 0;        // keep track for oxide
static int netOsnet = 0;      // net = 1 & snet = 2
static int viaRule = 0;       // keep track the viarule has called first
static int hasPort = 0;       // keep track is port defined in a Pin

static int               needNPCbk = 0;     // if cbk for net path is needed
static int               needSNPCbk = 0;    // if cbk for snet path is needed
static double            xStep, yStep;
static int               hasDoStep;
static int               hasBlkLayerComp;     // only 1 BLOCKAGE/LAYER/COMP
static int               hasBlkLayerSpac;     // only 1 BLOCKAGE/LAYER/SPACING
static int               hasBlkPlaceComp;     // only 1 BLOCKAGE/PLACEMENT/COMP
static int               hasBlkPlaceTypeComp; // SOFT or PARTIAL
static int               hasBlkLayerTypeComp; // SLOTS or FILLS

// the following variables to keep track the number of warnings printed.
static int assertionWarnings = 0;
static int blockageWarnings = 0;
static int caseSensitiveWarnings = 0;
static int componentWarnings = 0;
static int constraintWarnings = 0;
static int defaultCapWarnings = 0;
static int fillWarnings = 0;
static int gcellGridWarnings = 0;
static int iOTimingWarnings = 0;
static int netWarnings = 0;
static int nonDefaultWarnings = 0;
static int pinExtWarnings = 0;
static int pinWarnings = 0;
static int regionWarnings = 0;
static int rowWarnings = 0;
static int scanchainWarnings = 0;
static int sNetWarnings = 0;
static int stylesWarnings = 0;
static int trackWarnings = 0;
static int unitsWarnings = 0;
static int versionWarnings = 0;
static int viaWarnings = 0;

int names_case_sensitive = 1; // always true in 5.6
int reader_case_sensitive = 0; // save what the reader prefer
int defRetVal;

int shield = FALSE;       // To identify if the path is shield for 5.3

static char defPropDefType;   // save the current type of the property
static char* shieldName;      // to hold the shieldNetName
static char* rowName;         // to hold the rowName for message

int bitsNum;                  // Scanchain Bits value
char *warningMsg;
char *defMsg;

/* From def_keywords.cpp */
extern char* ringCopy(const char* string);


/* Macro to describe how we handle a callback.
 * If the function was set then call it.
 * If the function returns non zero then there was an error
 * so call the error routine and exit.
 */
#define CALLBACK(func, typ, data) \
    if (!errors) {\
      if (func) { \
        if ((defRetVal = (*func)(typ, data, defrReader::get()->getUserData())) == PARSE_OK) { \
        } else if (defRetVal == STOP_PARSE) { \
	  return defRetVal; \
        } else { \
          defError(6010, "An error has been reported in callback."); \
	  return defRetVal; \
        } \
      } \
    }

#define CHKERR() \
    if (errors > 20) {\
      defError(6011, "Too many syntax errors have been reported."); \
      errors = 0; \
      return 1; \
    }

#define CHKPROPTYPE(propType, propName, name) \
    if (propType == 'N') { \
       warningMsg = (char*)defMalloc(strlen(propName)+strlen(name)+40); \
       sprintf(warningMsg, "The PropName %s is not defined for %s.", \
               propName, name); \
       defWarning(7010, warningMsg); \
       defFree(warningMsg); \
    }

int validateMaskInput(int input, int warningIndex, int getWarningsIndex) 
{
    if (defVersionNum < 5.8 && input > 0) {
      if (warningIndex++ < getWarningsIndex) {
          defMsg = (char*)defMalloc(1000);
          sprintf (defMsg,
             "The MASK statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
          defError(7415, defMsg);
          defFree(defMsg);
          CHKERR();
          
          return 0;
        }
    }   
    
    return 1; 
}

double convert_defname2num(char *versionName)
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *p1;
    char *versionNm = strdup(versionName);

    double major = 0, minor = 0, subMinor = 0;
    double version;

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    if ((p1 = strchr(minorNm, '.'))) {
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

    free(versionNm);
    return version;
}

int numIsInt (char* volt) {
    if (strchr(volt, '.'))  // a floating point
       return 0;
    else
       return 1;
}

int defValidNum(int values) {
    char *outMsg;
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
             if (defVersionNum < 5.6) {
                if (defrReader::get()->getUnitsCbk()) {
                  if (unitsWarnings++ < defrReader::get()->getUnitsWarnings()) {
                    outMsg = (char*)defMalloc(1000);
                    sprintf (outMsg,
                    "An error has been found while processing the DEF file '%s'\nUnit %d is a 5.6 or later syntax. Define the DEF file as 5.6 and then try again.",
                    defrReader::get()->getFileName(), values);
                    defError(6501, outMsg);
                    defFree(outMsg);
                  }
                }
                
                return 0;
             } else {
                return 1;
             }
    }
    if (defrReader::get()->getUnitsCbk()) {
      if (unitsWarnings++ < defrReader::get()->getUnitsWarnings()) {
        outMsg = (char*)defMalloc(10000);
        sprintf (outMsg,
          "The value %d defined for DEF UNITS DISTANCE MICRON is invalid\n. The valid values are 100, 200, 400, 800, 1000, 2000, 4000, 8000, 10000, or 20000. Specify a valid value and then try again.", values);
        defError(6502, outMsg);
        defFree(outMsg);
        CHKERR();
      }
    }
    return 0;
}

#define FIXED 1
#define COVER 2
#define PLACED 3
#define UNPLACED 4
%}

%union {
        double dval ;
        int    integer ;
        char * string ;
        int    keyword ;  /* really just a nop */
        struct defpoint pt;
        defTOKEN *tk;
}

%token <string>  QSTRING
%token <string>  T_STRING SITE_PATTERN
%token <dval>    NUMBER
%token <keyword> K_HISTORY K_NAMESCASESENSITIVE
%token <keyword> K_DESIGN K_VIAS K_TECH K_UNITS K_ARRAY K_FLOORPLAN
%token <keyword> K_SITE K_CANPLACE K_CANNOTOCCUPY K_DIEAREA
%token <keyword> K_PINS
%token <keyword> K_DEFAULTCAP K_MINPINS K_WIRECAP
%token <keyword> K_TRACKS K_GCELLGRID
%token <keyword> K_DO K_BY K_STEP K_LAYER K_ROW K_RECT
%token <keyword> K_COMPS K_COMP_GEN K_SOURCE K_WEIGHT K_EEQMASTER
%token <keyword> K_FIXED K_COVER K_UNPLACED K_PLACED K_FOREIGN K_REGION 
%token <keyword> K_REGIONS
%token <keyword> K_NETS K_START_NET K_MUSTJOIN K_ORIGINAL K_USE K_STYLE
%token <keyword> K_PATTERN K_PATTERNNAME K_ESTCAP K_ROUTED K_NEW 
%token <keyword> K_SNETS K_SHAPE K_WIDTH K_VOLTAGE K_SPACING K_NONDEFAULTRULE
%token <keyword> K_NONDEFAULTRULES
%token <keyword> K_N K_S K_E K_W K_FN K_FE K_FS K_FW
%token <keyword> K_GROUPS K_GROUP K_SOFT K_MAXX K_MAXY K_MAXHALFPERIMETER
%token <keyword> K_CONSTRAINTS K_NET K_PATH K_SUM K_DIFF 
%token <keyword> K_SCANCHAINS K_START K_FLOATING K_ORDERED K_STOP K_IN K_OUT
%token <keyword> K_RISEMIN K_RISEMAX K_FALLMIN K_FALLMAX K_WIREDLOGIC
%token <keyword> K_MAXDIST
%token <keyword> K_ASSERTIONS
%token <keyword> K_DISTANCE K_MICRONS
%token <keyword> K_END
%token <keyword> K_IOTIMINGS K_RISE K_FALL K_VARIABLE K_SLEWRATE K_CAPACITANCE
%token <keyword> K_DRIVECELL K_FROMPIN K_TOPIN K_PARALLEL
%token <keyword> K_TIMINGDISABLES K_THRUPIN K_MACRO
%token <keyword> K_PARTITIONS K_TURNOFF
%token <keyword> K_FROMCLOCKPIN K_FROMCOMPPIN K_FROMIOPIN
%token <keyword> K_TOCLOCKPIN K_TOCOMPPIN K_TOIOPIN
%token <keyword> K_SETUPRISE K_SETUPFALL K_HOLDRISE K_HOLDFALL
%token <keyword> K_VPIN K_SUBNET K_XTALK K_PIN K_SYNTHESIZED
%token <keyword> K_DEFINE K_DEFINES K_DEFINEB K_IF K_THEN K_ELSE K_FALSE K_TRUE 
%token <keyword> K_EQ K_NE K_LE K_LT K_GE K_GT K_OR K_AND K_NOT
%token <keyword> K_SPECIAL K_DIRECTION K_RANGE
%token <keyword> K_FPC K_HORIZONTAL K_VERTICAL K_ALIGN K_MIN K_MAX K_EQUAL
%token <keyword> K_BOTTOMLEFT K_TOPRIGHT K_ROWS K_TAPER K_TAPERRULE
%token <keyword> K_VERSION K_DIVIDERCHAR K_BUSBITCHARS
%token <keyword> K_PROPERTYDEFINITIONS K_STRING K_REAL K_INTEGER K_PROPERTY
%token <keyword> K_BEGINEXT K_ENDEXT K_NAMEMAPSTRING K_ON K_OFF K_X K_Y
%token <keyword> K_COMPONENT K_MASK K_MASKSHIFT K_COMPSMASKSHIFT K_SAMEMASK
%token <keyword> K_PINPROPERTIES K_TEST
%token <keyword> K_COMMONSCANPINS K_SNET K_COMPONENTPIN K_REENTRANTPATHS
%token <keyword> K_SHIELD K_SHIELDNET K_NOSHIELD K_VIRTUAL
%token <keyword> K_ANTENNAPINPARTIALMETALAREA K_ANTENNAPINPARTIALMETALSIDEAREA
%token <keyword> K_ANTENNAPINGATEAREA K_ANTENNAPINDIFFAREA
%token <keyword> K_ANTENNAPINMAXAREACAR K_ANTENNAPINMAXSIDEAREACAR
%token <keyword> K_ANTENNAPINPARTIALCUTAREA K_ANTENNAPINMAXCUTCAR
%token <keyword> K_SIGNAL K_POWER K_GROUND K_CLOCK K_TIEOFF K_ANALOG K_SCAN
%token <keyword> K_RESET K_RING K_STRIPE K_FOLLOWPIN K_IOWIRE K_COREWIRE
%token <keyword> K_BLOCKWIRE K_FILLWIRE K_BLOCKAGEWIRE K_PADRING K_BLOCKRING
%token <keyword> K_BLOCKAGES K_PLACEMENT K_SLOTS K_FILLS K_PUSHDOWN
%token <keyword> K_NETLIST K_DIST K_USER K_TIMING K_BALANCED K_STEINER K_TRUNK
%token <keyword> K_FIXEDBUMP K_FENCE K_FREQUENCY K_GUIDE K_MAXBITS
%token <keyword> K_PARTITION K_TYPE K_ANTENNAMODEL K_DRCFILL
%token <keyword> K_OXIDE1 K_OXIDE2 K_OXIDE3 K_OXIDE4
%token <keyword> K_CUTSIZE K_CUTSPACING K_DESIGNRULEWIDTH K_DIAGWIDTH
%token <keyword> K_ENCLOSURE K_HALO K_GROUNDSENSITIVITY
%token <keyword> K_HARDSPACING K_LAYERS K_MINCUTS K_NETEXPR 
%token <keyword> K_OFFSET K_ORIGIN K_ROWCOL K_STYLES
%token <keyword> K_POLYGON K_PORT K_SUPPLYSENSITIVITY K_VIA K_VIARULE K_WIREEXT
%token <keyword> K_EXCEPTPGNET K_FILLWIREOPC K_OPC K_PARTIAL K_ROUTEHALO
%type <pt>      pt opt_paren
%type <integer> comp_net_list subnet_opt_syn
%type <integer> orient pin_via_mask_opt
%type <integer> placement_status
%type <string>  net_type subnet_type track_start use_type shape_type source_type
%type <string>  pattern_type netsource_type
%type <tk>      path paths new_path
%type <string>  risefall opt_pin opt_pattern pin_layer_opt
%type <string>  vpin_status opt_plus track_type region_type
%type <string>  h_or_v turnoff_setup turnoff_hold
%type <integer> conn_opt partition_maxbits same_mask mask orient_pt

%%

def_file: version_stmt case_sens_stmt rules end_design
            ;

version_stmt:  /* empty */
    | K_VERSION { dumb_mode = 1; no_num = 1; } T_STRING ';'
      {
        defVersionNum = convert_defname2num($3);
        if (defVersionNum > 5.8) {
          char temp[300];
          sprintf(temp,
          "The execution has been stopped because the DEF parser 5.8 does not support DEF file with version %s.\nUpdate your DEF file to version 5.8 or earlier.",
                  $3);
          defError(6503, temp);
          return 1;
        }
        if (defrReader::get()->getVersionStrCbk()) {
          CALLBACK(defrReader::get()->getVersionStrCbk(), defrVersionStrCbkType, $3);
        } else if (defrReader::get()->getVersionCbk()) {
            CALLBACK(defrReader::get()->getVersionCbk(), defrVersionCbkType, defVersionNum);
        }
        if (defVersionNum > 5.3 && defVersionNum < 5.4)
          defIgnoreVersion = 1;
        if (defVersionNum < 5.6)     // default to false before 5.6
          names_case_sensitive = reader_case_sensitive;
        else
          names_case_sensitive = 1;
        hasVer = 1;
        defrReader::get()->setDoneDesign(0);
    }

case_sens_stmt: /* empty */
    | K_NAMESCASESENSITIVE K_ON ';'
      {
        if (defVersionNum < 5.6) {
          names_case_sensitive = 1;
          if (defrReader::get()->getCaseSensitiveCbk())
            CALLBACK(defrReader::get()->getCaseSensitiveCbk(), defrCaseSensitiveCbkType,
                     names_case_sensitive); 
          hasNameCase = 1;
        } else
          if (defrReader::get()->getCaseSensitiveCbk()) /* write error only if cbk is set */
             if (caseSensitiveWarnings++ < defrReader::get()->getCaseSensitiveWarnings())
               defWarning(7011, "The NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
      }
    | K_NAMESCASESENSITIVE K_OFF ';'
      {
        if (defVersionNum < 5.6) {
          names_case_sensitive = 0;
          if (defrReader::get()->getCaseSensitiveCbk())
            CALLBACK(defrReader::get()->getCaseSensitiveCbk(), defrCaseSensitiveCbkType,
                     names_case_sensitive);
          hasNameCase = 1;
        } else {
          if (defrReader::get()->getCaseSensitiveCbk()) { /* write error only if cbk is set */
            if (caseSensitiveWarnings++ < defrReader::get()->getCaseSensitiveWarnings()) {
              defError(6504, "Def parser version 5.7 and later does not support NAMESCASESENSITIVE OFF.\nEither remove this optional construct or set it to ON.");
              CHKERR();
            }
          }
        }
      }

rules: /* empty */
        | rules rule
        | error 
            ;

rule: design_section | assertions_section | blockage_section | comps_section
      | constraint_section | extension_section | fill_section | comps_maskShift_section
      | floorplan_contraints_section | groups_section | iotiming_section
      | nets_section |  nondefaultrule_section | partitions_section
      | pin_props_section | regions_section | scanchains_section
      | slot_section | snets_section | styles_section | timingdisables_section
      | via_section
            ;

design_section: array_name | bus_bit_chars | canplace | cannotoccupy |
              design_name | die_area | divider_char | 
              floorplan_name | gcellgrid | history |
              pin_cap_rule | pin_rule | prop_def_section |
              row_rule | tech_name | tracks_rule | units
            ;

/* 11/11/2009 - Changed
design_section: array_name | bus_bit_chars | canplace | cannotoccupy |
              design_name | die_area | divider_char | 
              floorplan_name | gcellgrid | history |
              pin_cap_rule | pin_rule | prop_def_section |
              row_rule | site | tech_name | tracks_rule | units
            ;
*/

design_name: K_DESIGN {dumb_mode = 1; no_num = 1; } T_STRING ';' 
	  {
            if (defrReader::get()->getDesignCbk())
              CALLBACK(defrReader::get()->getDesignCbk(), defrDesignStartCbkType, $3);
            hasDes = 1;
          }

end_design: K_END K_DESIGN
	  {
            defrReader::get()->setDoneDesign(1);
            if (defrReader::get()->getDesignEndCbk())
              CALLBACK(defrReader::get()->getDesignEndCbk(), defrDesignEndCbkType, 0);
            // 11/16/2001 - pcr 408334
            // Return 1 if there is any errors during parsing
            if (errors)
                return 1;

            if (!hasVer)
              defWarning(7012, "The DEF file is invalid because the VERSION statement is not defined in it.\nThe VERSION statement is required in DEF file. Define this statement by refering to the LEF/DEF Language Reference manual.");
            if (!hasNameCase && defVersionNum < 5.6)
              defWarning(7013, "The DEF file is invalid if NAMESCASESENSITIVE is undefined.\nNAMESCASESENSITIVE is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the NAMESCASESENSITIVE statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!hasBusBit && defVersionNum < 5.6)
              defWarning(7014, "The DEF file is invalid if BUSBITCHARS is undefined.\nBUSBITCHARS is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the BUSBITCHARS statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!hasDivChar && defVersionNum < 5.6)
              defWarning(7015, "The DEF file is invalid if DIVIDERCHAR is undefined.\nDIVIDERCHAR is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the DIVIDERCHAR statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!hasDes)
              defWarning(7016, "DESIGN is a mandatory statement in the DEF file. Ensure that it exists in the file.");
            hasVer = 0;
            hasNameCase = 0;
            hasBusBit = 0;
            hasDivChar = 0;
            hasDes = 0;

            assertionWarnings = 0;
            blockageWarnings = 0;
            caseSensitiveWarnings = 0;
            componentWarnings = 0;
            constraintWarnings = 0;
            defaultCapWarnings = 0;
            gcellGridWarnings = 0;
            iOTimingWarnings = 0;
            netWarnings = 0;
            nonDefaultWarnings = 0;
            pinExtWarnings = 0;
            pinWarnings = 0;
            regionWarnings = 0;
            rowWarnings = 0;
            scanchainWarnings = 0;
            sNetWarnings = 0;
            stylesWarnings = 0;
            trackWarnings = 0;
            unitsWarnings = 0;
            versionWarnings = 0;
            viaWarnings = 0;
          }

tech_name: K_TECH { dumb_mode = 1; no_num = 1; } T_STRING ';'
          { 
            if (defrReader::get()->getTechnologyCbk())
              CALLBACK(defrReader::get()->getTechnologyCbk(), defrTechNameCbkType, $3);
          }

array_name: K_ARRAY {dumb_mode = 1; no_num = 1;} T_STRING ';'
	  { 
            if (defrReader::get()->getArrayNameCbk())
              CALLBACK(defrReader::get()->getArrayNameCbk(), defrArrayNameCbkType, $3);
          }

floorplan_name: K_FLOORPLAN { dumb_mode = 1; no_num = 1; } T_STRING ';'
	  { 
            if (defrReader::get()->getFloorPlanNameCbk())
              CALLBACK(defrReader::get()->getFloorPlanNameCbk(), defrFloorPlanNameCbkType, $3);
          }

history: K_HISTORY
          { 
            if (defrReader::get()->getHistoryCbk())
              CALLBACK(defrReader::get()->getHistoryCbk(), defrHistoryCbkType, History_text);
          }

prop_def_section: K_PROPERTYDEFINITIONS
	  {
            parsing_property = 1;
            defInPropDef = 1;     /* set flag as inside propertydefinitions */
            if (defrReader::get()->getPropDefStartCbk())
              CALLBACK(defrReader::get()->getPropDefStartCbk(), defrPropDefStartCbkType, 0);
          }
    property_defs K_END K_PROPERTYDEFINITIONS
	  { 
            if (defrReader::get()->getPropDefEndCbk())
              CALLBACK(defrReader::get()->getPropDefEndCbk(), defrPropDefEndCbkType, 0);
            real_num = 0;     /* just want to make sure it is reset */
            parsing_property = 0;
            defInPropDef = 0;     /* reset flag */
          }

property_defs: /* empty */
        | property_defs property_def
            { }

property_def: K_DESIGN {dumb_mode = 1; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
              T_STRING property_type_and_val ';' 
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("design", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getDesignProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_NET { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("net", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getNetProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_SNET { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("specialnet", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getSNetProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_REGION { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("region", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getRegionProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_GROUP { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("group", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getGroupProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_COMPONENT { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("component", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getCompProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_ROW { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("row", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getRowProp().defiPropType::setPropType($3, defPropDefType);
            }
/* 11/11/2009 - Doesn't exist
        | K_PIN { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("pin", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getPinDefProp().defiPropType::setPropType($3, defPropDefType);
            }
*/
        | K_COMPONENTPIN
          { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defrReader::get()->getPropCbk()) {
                defrReader::get()->getProp().defiProp::setPropType("componentpin", $3);
		CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
              }
              defrReader::get()->getCompPinProp().defiPropType::setPropType($3, defPropDefType);
            }
        | K_NONDEFAULTRULE
          { dumb_mode = 1 ; no_num = 1; defrReader::get()->getProp().defiProp::clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defVersionNum < 5.6) {
                if (nonDefaultWarnings++ < defrReader::get()->getNonDefaultWarnings()) {
                  defMsg = (char*)defMalloc(1000); 
                  sprintf (defMsg,
                     "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6505, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              } else {
                if (defrReader::get()->getPropCbk()) {
                  defrReader::get()->getProp().defiProp::setPropType("nondefaultrule", $3);
		  CALLBACK(defrReader::get()->getPropCbk(), defrPropCbkType, &defrReader::get()->getProp());
                }
                defrReader::get()->getNDefProp().defiPropType::setPropType($3, defPropDefType);
              }
            }
        | error ';' { yyerrok; yyclearin;}

property_type_and_val: K_INTEGER { real_num = 0 } opt_range opt_num_val
            {
              if (defrReader::get()->getPropCbk()) defrReader::get()->getProp().defiProp::setPropInteger();
              defPropDefType = 'I';
            }
        | K_REAL { real_num = 1 } opt_range opt_num_val
            {
              if (defrReader::get()->getPropCbk()) defrReader::get()->getProp().defiProp::setPropReal();
              defPropDefType = 'R';
              real_num = 0;
            }
        | K_STRING
            {
              if (defrReader::get()->getPropCbk()) defrReader::get()->getProp().defiProp::setPropString();
              defPropDefType = 'S';
            }
        | K_STRING QSTRING
            {
              if (defrReader::get()->getPropCbk()) defrReader::get()->getProp().defiProp::setPropQString($2);
              defPropDefType = 'Q';
            }
        | K_NAMEMAPSTRING T_STRING
            {
              if (defrReader::get()->getPropCbk()) defrReader::get()->getProp().defiProp::setPropNameMapString($2);
              defPropDefType = 'S';
            }

opt_num_val: /* empty */
        | NUMBER
            { if (defrReader::get()->getPropCbk()) defrReader::get()->getProp().defiProp::setNumber($1); }

units: K_UNITS K_DISTANCE K_MICRONS NUMBER ';'
	  {
            if (defrReader::get()->getUnitsCbk()) {
              if (defValidNum((int)$4))
                CALLBACK(defrReader::get()->getUnitsCbk(),  defrUnitsCbkType, $4);
            }
          }

divider_char: K_DIVIDERCHAR QSTRING ';'
	  {
            if (defrReader::get()->getDividerCbk())
              CALLBACK(defrReader::get()->getDividerCbk(), defrDividerCbkType, $2);
            hasDivChar = 1;
          }

bus_bit_chars: K_BUSBITCHARS QSTRING ';'
	  { 
            if (defrReader::get()->getBusBitCbk())
              CALLBACK(defrReader::get()->getBusBitCbk(), defrBusBitCbkType, $2);
            hasBusBit = 1;
          }

/* 11/11/2009 - Doesn't exist
site: K_SITE { dumb_mode = 1; no_num = 1; defrReader::get()->getSite().defiSite::clear(); }
        T_STRING NUMBER NUMBER orient
        K_DO NUMBER  K_BY NUMBER  K_STEP NUMBER NUMBER ';' 
	  {
            if (defrReader::get()->getSiteCbk()) {
              defrReader::get()->getSite().defiSite::setName($3);
              defrReader::get()->getSite().defiSite::setLocation($4,$5);
              defrReader::get()->getSite().defiSite::setOrient($6);
              defrReader::get()->getSite().defiSite::setDo($8,$10,$12,$13);
	      CALLBACK(defrReader::get()->getSiteCbk(), defrSiteCbkType, &(defrReader::get()->getSite()));
            }
	  }
*/

canplace: K_CANPLACE {dumb_mode = 1;no_num = 1; } T_STRING NUMBER NUMBER
            orient K_DO NUMBER  K_BY NUMBER  K_STEP NUMBER NUMBER ';' 
            {
              if (defrReader::get()->getCanplaceCbk()) {
                defrReader::get()->getCanplace().defiSite::setName($3);
                defrReader::get()->getCanplace().defiSite::setLocation($4,$5);
                defrReader::get()->getCanplace().defiSite::setOrient($6);
                defrReader::get()->getCanplace().defiSite::setDo($8,$10,$12,$13);
		CALLBACK(defrReader::get()->getCanplaceCbk(), defrCanplaceCbkType,
		&(defrReader::get()->getCanplace()));
              }
            }
cannotoccupy: K_CANNOTOCCUPY {dumb_mode = 1;no_num = 1; } T_STRING NUMBER NUMBER
            orient K_DO NUMBER  K_BY NUMBER  K_STEP NUMBER NUMBER ';' 
            {
              if (defrReader::get()->getCannotOccupyCbk()) {
                defrReader::get()->getCannotOccupy().defiSite::setName($3);
                defrReader::get()->getCannotOccupy().defiSite::setLocation($4,$5);
                defrReader::get()->getCannotOccupy().defiSite::setOrient($6);
                defrReader::get()->getCannotOccupy().defiSite::setDo($8,$10,$12,$13);
		CALLBACK(defrReader::get()->getCannotOccupyCbk(), defrCannotOccupyCbkType,
                        &(defrReader::get()->getCannotOccupy()));
              }
            }

orient: K_N    {$$ = 0;}
        | K_W  {$$ = 1;}
        | K_S  {$$ = 2;}
        | K_E  {$$ = 3;}
        | K_FN {$$ = 4;}
        | K_FW {$$ = 5;}
        | K_FS {$$ = 6;}
        | K_FE {$$ = 7;}

die_area: K_DIEAREA
	  {
            if (!defrReader::get()->getGeomPtr()) {  // set up for more than 4 points
              defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
              defrReader::get()->getGeomPtr()->defiGeometries::Init();
            } else    // Just reset the number of points
              defrReader::get()->getGeomPtr()->defiGeometries::Reset();
	  }
          firstPt nextPt otherPts ';'
          {
	    if (defrReader::get()->getDieAreaCbk()) {
               defrReader::get()->getDieArea().defiBox::addPoint(defrReader::get()->getGeomPtr());
               CALLBACK(defrReader::get()->getDieAreaCbk(), defrDieAreaCbkType, &(defrReader::get()->getDieArea()));
            }
          }

/* 8/31/2001 - This is obsolete in 5.4 */
pin_cap_rule: start_def_cap pin_caps end_def_cap
            { }

start_def_cap: K_DEFAULTCAP NUMBER 
	{
          if (defVersionNum < 5.4) {
             if (defrReader::get()->getDefaultCapCbk())
                CALLBACK(defrReader::get()->getDefaultCapCbk(), defrDefaultCapCbkType, ROUND($2));
          } else {
             if (defrReader::get()->getDefaultCapCbk()) /* write error only if cbk is set */
                if (defaultCapWarnings++ < defrReader::get()->getDefaultCapWarnings())
                   defWarning(7017, "The DEFAULTCAP statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }

pin_caps: /* empty */
        | pin_caps pin_cap
            ;

pin_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'
	  {
            if (defVersionNum < 5.4) {
	      if (defrReader::get()->getPinCapCbk()) {
	        defrReader::get()->getPinCap().defiPinCap::setPin(ROUND($2));
	        defrReader::get()->getPinCap().defiPinCap::setCap($4);
	        CALLBACK(defrReader::get()->getPinCapCbk(), defrPinCapCbkType, &(defrReader::get()->getPinCap()));
	      }
            }
	  }

end_def_cap: K_END K_DEFAULTCAP 
            { }

pin_rule: start_pins pins end_pins
            { }

start_pins: K_PINS NUMBER ';'
	  { 
            if (defrReader::get()->getStartPinsCbk())
              CALLBACK(defrReader::get()->getStartPinsCbk(), defrStartPinsCbkType, ROUND($2));
          }

pins: /* empty */
        | pins pin
            ;

pin: '-' {dumb_mode = 1; no_num = 1; } T_STRING '+' K_NET
	 {dumb_mode = 1; no_num = 1; } T_STRING
          {
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
              defrReader::get()->getPin().defiPin::Setup($3, $7);
            }
            hasPort = 0;
          }
        pin_options ';'
          { 
            if (defrReader::get()->getPinCbk())
              CALLBACK(defrReader::get()->getPinCbk(), defrPinCbkType, &defrReader::get()->getPin());
          }

pin_options: /* empty */
        | pin_options pin_option

pin_option: '+' K_SPECIAL
          {
            if (defrReader::get()->getPinCbk())
              defrReader::get()->getPin().defiPin::setSpecial();
          }

        | extension_stmt
          { 
            if (defrReader::get()->getPinExtCbk())
              CALLBACK(defrReader::get()->getPinExtCbk(), defrPinExtCbkType, History_text);
          }

        | '+' K_DIRECTION T_STRING
          {
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::setDirection($3);
          }

        | '+' K_NETEXPR QSTRING
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The NETEXPR statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6506, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
                defrReader::get()->getPin().defiPin::setNetExpr($3);

            }
          }

        | '+' K_SUPPLYSENSITIVITY { dumb_mode = 1; } T_STRING
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The SUPPLYSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6507, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
                defrReader::get()->getPin().defiPin::setSupplySens($4);
            }
          }

        | '+' K_GROUNDSENSITIVITY { dumb_mode = 1; } T_STRING
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The GROUNDSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6508, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
                defrReader::get()->getPin().defiPin::setGroundSens($4);
            }
          }

        | '+' K_USE use_type
          {
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) defrReader::get()->getPin().defiPin::setUse($3);
          }
        | '+' K_PORT          /* 5.7 */
          {
            if (defVersionNum < 5.7) {
               if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                 if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                     (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                   defMsg = (char*)defMalloc(10000);
                   sprintf (defMsg,
                     "The PORT in PINS is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                   defError(6555, defMsg);
                   defFree(defMsg);
                   CHKERR();
                 }
               }
            } else {
               if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
                 defrReader::get()->getPin().defiPin::addPort();
               hasPort = 1;
            }
          }

        | '+' K_LAYER { dumb_mode = 1; } T_STRING
          {
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
              if (hasPort)
                 defrReader::get()->getPin().defiPin::addPortLayer($4);
              else
                 defrReader::get()->getPin().defiPin::addLayer($4);
            }
          }
          pin_layer_mask_opt pin_layer_spacing_opt pt pt
          {
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
              if (hasPort)
                 defrReader::get()->getPin().defiPin::addPortLayerPts($8.x, $8.y, $9.x, $9.y);
              else
                 defrReader::get()->getPin().defiPin::addLayerPts($8.x, $8.y, $9.x, $9.y);
            }
          }

        | '+' K_POLYGON { dumb_mode = 1; } T_STRING
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6509, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortPolygon($4);
                else
                   defrReader::get()->getPin().defiPin::addPolygon($4);
              }
            }
            if (!defrReader::get()->getGeomPtr()) {
              defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
              defrReader::get()->getGeomPtr()->defiGeometries::Init();
            } else {  // Just reset the number of points
              defrReader::get()->getGeomPtr()->defiGeometries::Reset();
            }
          }
          pin_poly_mask_opt pin_poly_spacing_opt firstPt nextPt nextPt otherPts
          {
            if (defVersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortPolygonPts(defrReader::get()->getGeomPtr());
                else
                   defrReader::get()->getPin().defiPin::addPolygonPts(defrReader::get()->getGeomPtr());
              }
            }
          }
        | '+' K_VIA { dumb_mode = 1; } T_STRING pin_via_mask_opt '(' NUMBER NUMBER ')'   // 5.7
          {
            if (defVersionNum < 5.7) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The PIN VIA statement is available in version 5.7 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6556, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortVia($4, (int)$7,
                                               (int)$8, $5);
                else
                   defrReader::get()->getPin().defiPin::addVia($4, (int)$7,
                                               (int)$8, $5);
              }
            }
          }
  
        | placement_status pt orient
          {
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
              if (hasPort)
                 defrReader::get()->getPin().defiPin::setPortPlacement($1, $2.x, $2.y, $3);
              else
                 defrReader::get()->getPin().defiPin::setPlacement($1, $2.x, $2.y, $3);
            }
          }

        /* The following is 5.4 syntax */
        | '+' K_ANTENNAPINPARTIALMETALAREA NUMBER pin_layer_opt
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINPARTIALMETALAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6510, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinPartialMetalArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINPARTIALMETALSIDEAREA NUMBER pin_layer_opt
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINPARTIALMETALSIDEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6511, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinPartialMetalSideArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINGATEAREA NUMBER pin_layer_opt
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINGATEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6512, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
                defrReader::get()->getPin().defiPin::addAPinGateArea((int)$3, $4); 
            }
        | '+' K_ANTENNAPINDIFFAREA NUMBER pin_layer_opt
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINDIFFAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6513, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinDiffArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINMAXAREACAR NUMBER K_LAYER {dumb_mode=1;} T_STRING
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINMAXAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6514, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinMaxAreaCar((int)$3, $6); 
          }
        | '+' K_ANTENNAPINMAXSIDEAREACAR NUMBER K_LAYER {dumb_mode=1;}
           T_STRING
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINMAXSIDEAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6515, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinMaxSideAreaCar((int)$3, $6); 
          }
        | '+' K_ANTENNAPINPARTIALCUTAREA NUMBER pin_layer_opt
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINPARTIALCUTAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6516, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinPartialCutArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINMAXCUTCAR NUMBER K_LAYER {dumb_mode=1;} T_STRING
          {
            if (defVersionNum <= 5.3) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAPINMAXCUTCAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6517, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAPinMaxCutCar((int)$3, $6); 
          }
        | '+' K_ANTENNAMODEL pin_oxide
          {  /* 5.5 syntax */
            if (defVersionNum < 5.5) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The ANTENNAMODEL statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6518, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
          }

pin_layer_mask_opt: /* empty */
        | K_MASK NUMBER
         { 
           if (validateMaskInput((int)$2, pinWarnings, defrReader::get()->getPinWarnings())) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortLayerMask((int)$2);
                else
                   defrReader::get()->getPin().defiPin::addLayerMask((int)$2);
              }
           }
         }
         
pin_via_mask_opt: 
        /* empty */
	{ $$ = 0; }
        | K_MASK NUMBER
         { 
           if (validateMaskInput((int)$2, pinWarnings, defrReader::get()->getPinWarnings())) {
	     $$ = (int)$2;
           }
         }
         
pin_poly_mask_opt: /* empty */
        | K_MASK NUMBER
         { 
           if (validateMaskInput((int)$2, pinWarnings, defrReader::get()->getPinWarnings())) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortPolyMask((int)$2);
                else
                   defrReader::get()->getPin().defiPin::addPolyMask((int)$2);
              }
           }
         }
   
         
pin_layer_spacing_opt: /* empty */
        | K_SPACING NUMBER
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6519, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortLayerSpacing((int)$2);
                else
                   defrReader::get()->getPin().defiPin::addLayerSpacing((int)$2);
              }
            }
          }
        | K_DESIGNRULEWIDTH NUMBER
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "DESIGNRULEWIDTH statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defVersionNum);
                  defError(6520, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortLayerDesignRuleWidth((int)$2);
                else
                   defrReader::get()->getPin().defiPin::addLayerDesignRuleWidth((int)$2);
              }
            }
          }

pin_poly_spacing_opt: /* empty */
        | K_SPACING NUMBER
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "SPACING statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defVersionNum);
                  defError(6521, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortPolySpacing((int)$2);
                else
                   defrReader::get()->getPin().defiPin::addPolySpacing((int)$2);
              }
            }
          }
        | K_DESIGNRULEWIDTH NUMBER
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if ((pinWarnings++ < defrReader::get()->getPinWarnings()) &&
                    (pinWarnings++ < defrReader::get()->getPinExtWarnings())) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The DESIGNRULEWIDTH statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                  defError(6520, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk()) {
                if (hasPort)
                   defrReader::get()->getPin().defiPin::addPortPolyDesignRuleWidth((int)$2);
                else
                   defrReader::get()->getPin().defiPin::addPolyDesignRuleWidth((int)$2);
              }
            }
          }

pin_oxide: K_OXIDE1
          { aOxide = 1;
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAntennaModel(aOxide);
          }
        | K_OXIDE2
          { aOxide = 2;
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAntennaModel(aOxide);
          }
        | K_OXIDE3
          { aOxide = 3;
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAntennaModel(aOxide);
          }
        | K_OXIDE4
          { aOxide = 4;
            if (defrReader::get()->getPinCbk() || defrReader::get()->getPinExtCbk())
              defrReader::get()->getPin().defiPin::addAntennaModel(aOxide);
          }

use_type: K_SIGNAL
          { $$ = (char*)"SIGNAL"; }
        | K_POWER
          { $$ = (char*)"POWER"; }
        | K_GROUND
          { $$ = (char*)"GROUND"; }
        | K_CLOCK
          { $$ = (char*)"CLOCK"; }
        | K_TIEOFF
          { $$ = (char*)"TIEOFF"; }
        | K_ANALOG
          { $$ = (char*)"ANALOG"; }
        | K_SCAN
          { $$ = (char*)"SCAN"; }
        | K_RESET
          { $$ = (char*)"RESET"; }

pin_layer_opt:
        /* empty */
          { $$ = (char*)""; }
        | K_LAYER {dumb_mode=1;} T_STRING
          { $$ = $3; }

end_pins: K_END K_PINS
	{ 
          if (defrReader::get()->getPinEndCbk())
            CALLBACK(defrReader::get()->getPinEndCbk(), defrPinEndCbkType, 0);
        }

row_rule: K_ROW {dumb_mode = 2; no_num = 2; } T_STRING T_STRING NUMBER NUMBER
      orient
        {
          if (defrReader::get()->getRowCbk()) {
            rowName = $3;
            defrReader::get()->getRow().defiRow::setup($3, $4, $5, $6, $7);
          }
        }
      row_do_option
      row_options ';'
        {
          if (defrReader::get()->getRowCbk()) 
	    CALLBACK(defrReader::get()->getRowCbk(), defrRowCbkType, &defrReader::get()->getRow());
        }

row_do_option: /* empty */
        {
          if (defVersionNum < 5.6) {
            if (defrReader::get()->getRowCbk()) {
              if (rowWarnings++ < defrReader::get()->getRowWarnings()) {
                defError(6523, "Invalid ROW statement defined in the DEF file. The DO statement which is required in the ROW statement is not defined.\nUpdate your DEF file with a DO statement.");
                CHKERR();
              }
            }
          }
        }
      | K_DO NUMBER K_BY NUMBER row_step_option
        {
          /* 06/05/2002 - pcr 448455 */
          /* Check for 1 and 0 in the correct position */
          /* 07/26/2002 - Commented out due to pcr 459218 */
          if (hasDoStep) {
            /* 04/29/2004 - pcr 695535 */
            /* changed the testing */
            if ((($4 == 1) && (yStep == 0)) ||
                (($2 == 1) && (xStep == 0))) {
              /* do nothing */
            } else 
              if (defVersionNum < 5.6) {
                if (defrReader::get()->getRowCbk()) {
                  if (rowWarnings++ < defrReader::get()->getRowWarnings()) {
                    defMsg = (char*)defMalloc(1000);
                    sprintf(defMsg,
                            "The DO statement in the ROW statement with the name %s has invalid syntax.\nThe valid syntax is \"DO numX BY 1 STEP spaceX 0 | DO 1 BY numY STEP 0 spaceY\".\nSpecify the valid syntax and try again.", rowName);
                    defWarning(7018, defMsg);
                    defFree(defMsg);
                    }
                  }
              }
          }
          /* pcr 459218 - Error if at least numX or numY does not equal 1 */
          if (($2 != 1) && ($4 != 1)) {
            if (defrReader::get()->getRowCbk()) {
              if (rowWarnings++ < defrReader::get()->getRowWarnings()) {
                defError(6524, "Invalid syntax specified. The valid syntax is either \"DO 1 BY num or DO num BY 1\". Specify the valid syntax and try again.");
                CHKERR();
              }
            }
          }
          if (defrReader::get()->getRowCbk())
            defrReader::get()->getRow().defiRow::setDo(ROUND($2), ROUND($4), xStep, yStep);
        }

row_step_option: /* empty */
        {
          hasDoStep = 0;
        }
      | K_STEP NUMBER NUMBER
        {
          hasDoStep = 1;
          defrReader::get()->getRow().defiRow::setHasDoStep();
          xStep = $2;
          yStep = $3;
        }

row_options: /* empty */
      | row_options row_option
      ;

row_option : '+' K_PROPERTY { dumb_mode = 10000000; parsing_property = 1;}
             row_prop_list
         { dumb_mode = 0; parsing_property = 0; }

row_prop_list : /* empty */
       | row_prop_list row_prop
       ;
       
row_prop : T_STRING NUMBER
        {
          if (defrReader::get()->getRowCbk()) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp =  defrReader::get()->getRowProp().defiPropType::propType($1);
             CHKPROPTYPE(propTp, $1, "ROW");
             /* For backword compatibility, also set the string value */
             /* We will use a temporary string to store the number.
              * The string space is borrowed from the ring buffer
              * in the lexer. */
             sprintf(str, "%g", $2);
             defrReader::get()->getRow().defiRow::addNumProperty($1, $2, str, propTp);
          }
        }
      | T_STRING QSTRING
        {
          if (defrReader::get()->getRowCbk()) {
             char propTp;
             propTp =  defrReader::get()->getRowProp().defiPropType::propType($1);
             CHKPROPTYPE(propTp, $1, "ROW");
             defrReader::get()->getRow().defiRow::addProperty($1, $2, propTp);
          }
        }
      | T_STRING T_STRING
        {
          if (defrReader::get()->getRowCbk()) {
             char propTp;
             propTp =  defrReader::get()->getRowProp().defiPropType::propType($1);
             CHKPROPTYPE(propTp, $1, "ROW");
             defrReader::get()->getRow().defiRow::addProperty($1, $2, propTp);
          }
        }

tracks_rule: track_start NUMBER
        {
          if (defrReader::get()->getTrackCbk()) {
            defrReader::get()->getTrack().defiTrack::setup($1);
          }
        }
        K_DO NUMBER K_STEP NUMBER track_opts ';' 
        {
          if (($5 <= 0) && (defVersionNum >= 5.4)) {
            if (defrReader::get()->getTrackCbk())
              if (trackWarnings++ < defrReader::get()->getTrackWarnings()) {
                defMsg = (char*)defMalloc(1000);
                sprintf (defMsg,
                   "The DO number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $5);
                defError(6525, defMsg);
                defFree(defMsg);
              }
          }
          if ($7 < 0) {
            if (defrReader::get()->getTrackCbk())
              if (trackWarnings++ < defrReader::get()->getTrackWarnings()) {
                defMsg = (char*)defMalloc(1000);
                sprintf (defMsg,
                   "The STEP number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $7);
                defError(6526, defMsg);
                defFree(defMsg);
              }
          }
          if (defrReader::get()->getTrackCbk()) {
            defrReader::get()->getTrack().defiTrack::setDo(ROUND($2), ROUND($5), $7);
	    CALLBACK(defrReader::get()->getTrackCbk(), defrTrackCbkType, &defrReader::get()->getTrack());
          }
        }

track_start: K_TRACKS track_type
        {
          $$ = $2;
        }

track_type: K_X
            { $$ = (char*)"X";}
        | K_Y
            { $$ = (char*)"Y";}
            
track_opts: track_mask_statement track_layer_statement
        
track_mask_statement: /* empty */
        | K_MASK NUMBER same_mask
           { 
              if (validateMaskInput((int)$2, trackWarnings, defrReader::get()->getTrackWarnings())) {
                  if (defrReader::get()->getTrackCbk()) {
                    defrReader::get()->getTrack().defiTrack::addMask((int)$2, $3);
                  }
               }
            }
            
same_mask: 
        /* empty */
        { $$ = 0; }
        | K_SAMEMASK
        { $$ = 1; }
        
track_layer_statement: /* empty */
        | K_LAYER { dumb_mode = 1000; } track_layer track_layers
            { dumb_mode = 0; }

track_layers: /* empty */
        | track_layer track_layers 
            ;

track_layer: T_STRING
        {
          if (defrReader::get()->getTrackCbk())
            defrReader::get()->getTrack().defiTrack::addLayer($1);
        }

gcellgrid: K_GCELLGRID track_type NUMBER 
     K_DO NUMBER K_STEP NUMBER ';'
	{
          if ($5 <= 0) {
            if (defrReader::get()->getGcellGridCbk())
              if (gcellGridWarnings++ < defrReader::get()->getGcellGridWarnings()) {
                defMsg = (char*)defMalloc(1000);
                sprintf (defMsg,
                   "The DO number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $5);
                defError(6527, defMsg);
                defFree(defMsg);
              }
          }
          if ($7 < 0) {
            if (defrReader::get()->getGcellGridCbk())
              if (gcellGridWarnings++ < defrReader::get()->getGcellGridWarnings()) {
                defMsg = (char*)defMalloc(1000);
                sprintf (defMsg,
                   "The STEP number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $7);
                defError(6528, defMsg);
                defFree(defMsg);
              }
          }
	  if (defrReader::get()->getGcellGridCbk()) {
	    defrReader::get()->getGcellGrid().defiGcellGrid::setup($2, ROUND($3), ROUND($5), $7);
	    CALLBACK(defrReader::get()->getGcellGridCbk(), defrGcellGridCbkType, &defrReader::get()->getGcellGrid());
	  }
	}

extension_section: K_BEGINEXT
        {
          if (defrReader::get()->getExtensionCbk())
             CALLBACK(defrReader::get()->getExtensionCbk(), defrExtensionCbkType, History_text);
        }

extension_stmt: '+' K_BEGINEXT
        { }

via_section: via via_declarations via_end
            ;
        
via: K_VIAS NUMBER ';' 
        {
          if (defrReader::get()->getViaStartCbk())
            CALLBACK(defrReader::get()->getViaStartCbk(), defrViaStartCbkType, ROUND($2));
        }

via_declarations: /* empty */
        | via_declarations via_declaration
            ;

via_declaration: '-' {dumb_mode = 1;no_num = 1; } T_STRING
            {
              if (defrReader::get()->getViaCbk()) defrReader::get()->getVia().defiVia::setup($3);
              viaRule = 0;
            }
        layer_stmts ';'
            {
              if (defrReader::get()->getViaCbk())
                CALLBACK(defrReader::get()->getViaCbk(), defrViaCbkType, &defrReader::get()->getVia());
              defrReader::get()->getVia().defiVia::clear();
            }

layer_stmts: /* empty */
        | layer_stmts layer_stmt
            ;

layer_stmt: '+' K_RECT {dumb_mode = 1;no_num = 1; } T_STRING mask pt pt 
            { 
              if (defrReader::get()->getViaCbk())
                if (validateMaskInput($5, viaWarnings, defrReader::get()->getViaWarnings())) {
                    defrReader::get()->getVia().defiVia::addLayer($4, $6.x, $6.y, $7.x, $7.y, $5);
                }
            }
        | '+' K_POLYGON { dumb_mode = 1; } T_STRING mask
            {
              if (defVersionNum < 5.6) {
                if (defrReader::get()->getViaCbk()) {
                  if (viaWarnings++ < defrReader::get()->getViaWarnings()) {
                    defMsg = (char*)defMalloc(1000);
                    sprintf (defMsg,
                       "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                    defError(6509, defMsg);
                    defFree(defMsg);
                    CHKERR();
                  }
                }
              }
              if (!defrReader::get()->getGeomPtr()) {
                defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
                defrReader::get()->getGeomPtr()->defiGeometries::Init();
              } else {  // Just reset the number of points
                defrReader::get()->getGeomPtr()->defiGeometries::Reset();
              }
            }
            firstPt nextPt nextPt otherPts
            {
              if (defVersionNum >= 5.6) {  // only add if 5.6 or beyond
                if (defrReader::get()->getViaCbk())
                  if (validateMaskInput($5, viaWarnings, defrReader::get()->getViaWarnings())) {
                    defrReader::get()->getVia().defiVia::addPolygon($4, defrReader::get()->getGeomPtr(), $5);
                  }
              }
            }
        | '+' K_PATTERNNAME {dumb_mode = 1;no_num = 1; } T_STRING
            {
              if (defVersionNum < 5.6) {
                if (defrReader::get()->getViaCbk())
                  defrReader::get()->getVia().defiVia::addPattern($4);
              } else
                if (defrReader::get()->getViaCbk())
                  if (viaWarnings++ < defrReader::get()->getViaWarnings())
                    defWarning(7019, "The PATTERNNAME statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement."); 
            }
        | '+' K_VIARULE {dumb_mode = 1;no_num = 1; } T_STRING
          '+' K_CUTSIZE NUMBER NUMBER
          '+' K_LAYERS {dumb_mode = 3;no_num = 1; } T_STRING T_STRING T_STRING
          '+' K_CUTSPACING NUMBER NUMBER
          '+' K_ENCLOSURE NUMBER NUMBER NUMBER NUMBER
            {
               viaRule = 1;
               if (defVersionNum < 5.6) {
                if (defrReader::get()->getViaCbk()) {
                  if (viaWarnings++ < defrReader::get()->getViaWarnings()) {
                    defMsg = (char*)defMalloc(1000);
                    sprintf (defMsg,
                       "The VIARULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                    defError(6557, defMsg);
                    defFree(defMsg);
                    CHKERR();
                  }
                }
              } else {
                if (defrReader::get()->getViaCbk())
                   defrReader::get()->getVia().defiVia::addViaRule($4, (int)$7, (int)$8, $12, $13,
                                             $14, (int)$17, (int)$18, (int)$21,
                                             (int)$22, (int)$23, (int)$24); 
              }
            }
        | layer_viarule_opts
        | extension_stmt
          { 
            if (defrReader::get()->getViaExtCbk())
              CALLBACK(defrReader::get()->getViaExtCbk(), defrViaExtCbkType, History_text);
          }

layer_viarule_opts: '+' K_ROWCOL NUMBER NUMBER
            {
              if (!viaRule) {
                if (defrReader::get()->getViaCbk()) {
                  if (viaWarnings++ < defrReader::get()->getViaWarnings()) {
                    defError (6559, "The ROWCOL statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defrReader::get()->getViaCbk())
                 defrReader::get()->getVia().defiVia::addRowCol((int)$3, (int)$4);
            }
        | '+' K_ORIGIN NUMBER NUMBER
            {
              if (!viaRule) {
                if (defrReader::get()->getViaCbk()) {
                  if (viaWarnings++ < defrReader::get()->getViaWarnings()) {
                    defError (6560, "The ORIGIN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defrReader::get()->getViaCbk())
                 defrReader::get()->getVia().defiVia::addOrigin((int)$3, (int)$4);
            }
        | '+' K_OFFSET NUMBER NUMBER NUMBER NUMBER
            {
              if (!viaRule) {
                if (defrReader::get()->getViaCbk()) {
                  if (viaWarnings++ < defrReader::get()->getViaWarnings()) {
                    defError (6561, "The OFFSET statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defrReader::get()->getViaCbk())
                 defrReader::get()->getVia().defiVia::addOffset((int)$3, (int)$4, (int)$5, (int)$6);
            }
        | '+' K_PATTERN {dumb_mode = 1;no_num = 1; } T_STRING 
            {
              if (!viaRule) {
                if (defrReader::get()->getViaCbk()) {
                  if (viaWarnings++ < defrReader::get()->getViaWarnings()) {
                    defError (6562, "The PATTERN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defrReader::get()->getViaCbk())
                 defrReader::get()->getVia().defiVia::addCutPattern($4);
            }

firstPt: pt
          { defrReader::get()->getGeomPtr()->defiGeometries::startList($1.x, $1.y); }

nextPt: pt
          { defrReader::get()->getGeomPtr()->defiGeometries::addToList($1.x, $1.y); }

otherPts: /* empty */
        | otherPts nextPt
        ;

pt: '(' NUMBER NUMBER ')'
          {
            save_x = $2;
            save_y = $3;
            $$.x = ROUND($2);
            $$.y = ROUND($3);
          }
        | '(' '*' NUMBER ')'
          {
            save_y = $3;
            $$.x = ROUND(save_x);
            $$.y = ROUND($3);
          }
        | '(' NUMBER '*' ')'
          {
            save_x = $2;
            $$.x = ROUND($2);
            $$.y = ROUND(save_y);
          }
        | '(' '*' '*' ')'
          {
            $$.x = ROUND(save_x);
            $$.y = ROUND(save_y);
          }
          
mask: /* empty */
      { $$ = 0; }
      | '+' K_MASK NUMBER
      { $$ = (int)$3; }

via_end: K_END K_VIAS
	{ 
          if (defrReader::get()->getViaEndCbk())
            CALLBACK(defrReader::get()->getViaEndCbk(), defrViaEndCbkType, 0);
        }

regions_section: regions_start regions_stmts K_END K_REGIONS
	{
          if (defrReader::get()->getRegionEndCbk())
            CALLBACK(defrReader::get()->getRegionEndCbk(), defrRegionEndCbkType, 0);
        }

regions_start: K_REGIONS NUMBER ';'
	{
          if (defrReader::get()->getRegionStartCbk())
            CALLBACK(defrReader::get()->getRegionStartCbk(), defrRegionStartCbkType, ROUND($2));
        }

regions_stmts: /* empty */
        | regions_stmts regions_stmt
            {}

regions_stmt: '-' { dumb_mode = 1; no_num = 1; } T_STRING
        {
          if (defrReader::get()->getRegionCbk())
             defrReader::get()->getRegion().defiRegion::setup($3);
          regTypeDef = 0;
        }
     rect_list region_options ';'
        { CALLBACK(defrReader::get()->getRegionCbk(), defrRegionCbkType, &defrReader::get()->getRegion()); }

rect_list :
      pt pt
        { if (defrReader::get()->getRegionCbk())
	  defrReader::get()->getRegion().defiRegion::addRect($1.x, $1.y, $2.x, $2.y); }
      | rect_list pt pt
        { if (defrReader::get()->getRegionCbk())
	  defrReader::get()->getRegion().defiRegion::addRect($2.x, $2.y, $3.x, $3.y); }
      ;

region_options: /* empty */
      | region_options region_option
      ;

region_option : '+' K_PROPERTY { dumb_mode = 10000000; parsing_property = 1; }
                region_prop_list
         { dumb_mode = 0; parsing_property = 0; }
      | '+' K_TYPE region_type      /* 5.4.1 */
         {
           if (regTypeDef) {
              if (defrReader::get()->getRegionCbk()) {
                if (regionWarnings++ < defrReader::get()->getRegionWarnings()) {
                  defError(6563, "The TYPE statement already exists. It has been defined in the REGION statement.");
                  CHKERR();
                }
              }
           }
           if (defrReader::get()->getRegionCbk()) defrReader::get()->getRegion().defiRegion::setType($3);
           regTypeDef = 1;
         }
      ;

region_prop_list : /* empty */
       | region_prop_list region_prop
       ;
       
region_prop : T_STRING NUMBER
        {
          if (defrReader::get()->getRegionCbk()) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp = defrReader::get()->getRegionProp().defiPropType::propType($1);
             CHKPROPTYPE(propTp, $1, "REGION");
             /* For backword compatibility, also set the string value */
             /* We will use a temporary string to store the number.
              * The string space is borrowed from the ring buffer
              * in the lexer. */
             sprintf(str, "%g", $2);
             defrReader::get()->getRegion().defiRegion::addNumProperty($1, $2, str, propTp);
          }
        }
      | T_STRING QSTRING
        {
          if (defrReader::get()->getRegionCbk()) {
             char propTp;
             propTp = defrReader::get()->getRegionProp().defiPropType::propType($1);
             CHKPROPTYPE(propTp, $1, "REGION");
             defrReader::get()->getRegion().defiRegion::addProperty($1, $2, propTp);
          }
        }
      | T_STRING T_STRING
        {
          if (defrReader::get()->getRegionCbk()) {
             char propTp;
             propTp = defrReader::get()->getRegionProp().defiPropType::propType($1);
             CHKPROPTYPE(propTp, $1, "REGION");
             defrReader::get()->getRegion().defiRegion::addProperty($1, $2, propTp);
          }
        }

region_type: K_FENCE
            { $$ = (char*)"FENCE"; }
      | K_GUIDE
            { $$ = (char*)"GUIDE"; }
  
comps_maskShift_section : K_COMPSMASKSHIFT  layer_statement ';'
         {
           if (defVersionNum < 5.8) {
                if (componentWarnings++ < defrReader::get()->getComponentWarnings()) {
                   defMsg = (char*)defMalloc(10000);
                   sprintf (defMsg,
                     "The MASKSHIFT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
                   defError(7415, defMsg);
                   defFree(defMsg);
                   CHKERR();
                }
            }
            if (defrReader::get()->getComponentMaskShiftLayerCbk()) {
	        CALLBACK(defrReader::get()->getComponentMaskShiftLayerCbk(), defrComponentMaskShiftLayerCbkType, &defrReader::get()->getComponentMaskShiftLayer());
	    }
         }
                 
comps_section: start_comps comps_rule end_comps
            ;
         
start_comps: K_COMPS NUMBER ';'
         { 
            if (defrReader::get()->getComponentStartCbk())
              CALLBACK(defrReader::get()->getComponentStartCbk(), defrComponentStartCbkType,
                       ROUND($2));
         }
         
layer_statement : /* empty */
         | layer_statement maskLayer
         ;
         
maskLayer: T_STRING
        {
            if (defrReader::get()->getComponentMaskShiftLayerCbk()) {
              defrReader::get()->getComponentMaskShiftLayer().defiComponentMaskShiftLayer::addMaskShiftLayer($1);
            }
        } 

comps_rule: /* empty */
        | comps_rule comp
            ;

comp: comp_start comp_options ';'
	 {
            if (defrReader::get()->getComponentCbk())
              CALLBACK(defrReader::get()->getComponentCbk(), defrComponentCbkType, &defrReader::get()->getComponent());
         }

comp_start: comp_id_and_name comp_net_list
         {
            dumb_mode = 0;
            no_num = 0;
            /* The net connections were added to the array in
             * reverse order so fix the order now */
            /* Do not need to reverse the order since the storing is
               in sequence now  08/07/2001
            if (defrReader::get()->getComponentCbk())
              defrReader::get()->getComponent().defiComponent::reverseNetOrder();
            */
         }

comp_id_and_name: '-' {dumb_mode = 1000000000; no_num = 10000000; }
       T_STRING T_STRING
         {
            if (defrReader::get()->getComponentCbk())
              defrReader::get()->getComponent().defiComponent::IdAndName($3, $4);
         }

comp_net_list: /* empty */
        { }
        | comp_net_list '*'
            {
              if (defrReader::get()->getComponentCbk())
                defrReader::get()->getComponent().defiComponent::addNet("*");
            }
        | comp_net_list T_STRING
            {
              if (defrReader::get()->getComponentCbk())
                defrReader::get()->getComponent().defiComponent::addNet($2);
            }
            
comp_options: /* empty */
        |     comp_options comp_option
            ;
    
comp_option:  comp_generate | comp_source | comp_type | weight | maskShift |
              comp_foreign | comp_region | comp_eeq | comp_halo |
              comp_routehalo | comp_property | comp_extension_stmt
            ;

comp_extension_stmt: extension_stmt
        {
          if (defrReader::get()->getComponentCbk())
            CALLBACK(defrReader::get()->getComponentExtCbk(), defrComponentExtCbkType,
                     History_text);
        }

comp_eeq: '+' K_EEQMASTER {dumb_mode=1; no_num = 1; } T_STRING
        {
          if (defrReader::get()->getComponentCbk())
            defrReader::get()->getComponent().defiComponent::setEEQ($4);
        }

comp_generate: '+' K_COMP_GEN { dumb_mode = 2;  no_num = 2; } T_STRING
    opt_pattern
        {
          if (defrReader::get()->getComponentCbk())
             defrReader::get()->getComponent().defiComponent::setGenerate($4, $5);
        }
opt_pattern :
    /* empty */
      { $$ = (char*)""; }
    | T_STRING
      { $$ = $1; }

comp_source: '+' K_SOURCE source_type 
        {
          if (defrReader::get()->getComponentCbk())
            defrReader::get()->getComponent().defiComponent::setSource($3);
        }

source_type: K_NETLIST
            { $$ = (char*)"NETLIST"; }
        | K_DIST
            { $$ = (char*)"DIST"; }
        | K_USER
            { $$ = (char*)"USER"; }
        | K_TIMING
            { $$ = (char*)"TIMING"; }


comp_region:
	comp_region_start comp_pnt_list
        { }
        | comp_region_start T_STRING 
        {
	  if (defrReader::get()->getComponentCbk())
	    defrReader::get()->getComponent().defiComponent::setRegionName($2);
	}

comp_pnt_list: pt pt
	{ 
          /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
          if (defVersionNum < 5.5) {
            if (defrReader::get()->getComponentCbk())
	       defrReader::get()->getComponent().defiComponent::setRegionBounds($1.x, $1.y, 
                                                            $2.x, $2.y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
	} 
    | comp_pnt_list pt pt
	{ 
          /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
          if (defVersionNum < 5.5) {
	    if (defrReader::get()->getComponentCbk())
	       defrReader::get()->getComponent().defiComponent::setRegionBounds($2.x, $2.y,
                                                            $3.x, $3.y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
	} 

comp_halo: '+' K_HALO                    /* 5.7 */
        {
          if (defVersionNum < 5.6) {
             if (defrReader::get()->getComponentCbk()) {
               if (componentWarnings++ < defrReader::get()->getComponentWarnings()) {
                 defMsg = (char*)defMalloc(1000);
                 sprintf (defMsg,
                    "The HALO statement is a version 5.6 and later syntax.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                 defError(6529, defMsg);
                 defFree(defMsg);
                 CHKERR();
               }
             }
          }
        }
        halo_soft NUMBER NUMBER NUMBER NUMBER 
        {
          if (defrReader::get()->getComponentCbk())
            defrReader::get()->getComponent().defiComponent::setHalo((int)$5, (int)$6,
                                                 (int)$7, (int)$8);
        }

halo_soft: /* empty */                 /* 5.7 */
    | K_SOFT
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getComponentCbk()) {
             if (componentWarnings++ < defrReader::get()->getComponentWarnings()) {
                defMsg = (char*)defMalloc(10000);
                sprintf (defMsg,
                  "The HALO SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                defError(6550, defMsg);
                defFree(defMsg);
                CHKERR();
             }
           }
        } else {
           if (defrReader::get()->getComponentCbk())
             defrReader::get()->getComponent().defiComponent::setHaloSoft();
        }
      }

/* 5.7 */
comp_routehalo: '+' K_ROUTEHALO NUMBER { dumb_mode = 2; no_num = 2; } T_STRING T_STRING
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getComponentCbk()) {
             if (componentWarnings++ < defrReader::get()->getComponentWarnings()) {
                defMsg = (char*)defMalloc(10000);
                sprintf (defMsg,
                  "The ROUTEHALO is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                defError(6551, defMsg);
                defFree(defMsg);
                CHKERR();
             }
           }
        } else {
           if (defrReader::get()->getComponentCbk())
             defrReader::get()->getComponent().defiComponent::setRouteHalo(
                            (int)$3, $5, $6);
        }
      }

comp_property: '+' K_PROPERTY { dumb_mode = 10000000; parsing_property = 1; }
      comp_prop_list
      { dumb_mode = 0; parsing_property = 0; }

comp_prop_list: comp_prop
    | comp_prop_list comp_prop
          ;

comp_prop: T_STRING NUMBER
        {
          if (defrReader::get()->getComponentCbk()) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defrReader::get()->getCompProp().defiPropType::propType($1);
            CHKPROPTYPE(propTp, $1, "COMPONENT");
            /* For backword compatibility, also set the string value */
            /* We will use a temporary string to store the number.
             * The string space is borrowed from the ring buffer
             * in the lexer. */
            sprintf(str, "%g", $2);
            defrReader::get()->getComponent().defiComponent::addNumProperty($1, $2, str, propTp);
          }
        }
     | T_STRING QSTRING
        {
          if (defrReader::get()->getComponentCbk()) {
            char propTp;
            propTp = defrReader::get()->getCompProp().defiPropType::propType($1);
            CHKPROPTYPE(propTp, $1, "COMPONENT");
            defrReader::get()->getComponent().defiComponent::addProperty($1, $2, propTp);
          }
        }
     | T_STRING T_STRING
        {
          if (defrReader::get()->getComponentCbk()) {
            char propTp;
            propTp = defrReader::get()->getCompProp().defiPropType::propType($1);
            CHKPROPTYPE(propTp, $1, "COMPONENT");
            defrReader::get()->getComponent().defiComponent::addProperty($1, $2, propTp);
          }
        }

comp_region_start: '+' K_REGION
        { dumb_mode = 1; no_num = 1; }

comp_foreign: '+' K_FOREIGN { dumb_mode = 1; no_num = 1; } T_STRING
        opt_paren orient
        { 
          if (defVersionNum < 5.6) {
            if (defrReader::get()->getComponentCbk()) {
              defrReader::get()->getComponent().defiComponent::setForeignName($4);
              defrReader::get()->getComponent().defiComponent::setForeignLocation($5.x, $5.y, $6);
            }
         } else
            if (defrReader::get()->getComponentCbk())
              if (componentWarnings++ < defrReader::get()->getComponentWarnings())
                defWarning(7021, "The FOREIGN statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
         }

opt_paren:
       pt
	 { $$ = $1; }
       | NUMBER NUMBER
	 { $$.x = ROUND($1); $$.y = ROUND($2); }

comp_type: placement_status pt orient
        {
          if (defrReader::get()->getComponentCbk()) {
            defrReader::get()->getComponent().defiComponent::setPlacementStatus($1);
            defrReader::get()->getComponent().defiComponent::setPlacementLocation($2.x, $2.y, $3);
          }
        }
        | '+' K_UNPLACED
        {
          if (defrReader::get()->getComponentCbk())
            defrReader::get()->getComponent().defiComponent::setPlacementStatus(
                                         DEFI_COMPONENT_UNPLACED);
            defrReader::get()->getComponent().defiComponent::setPlacementLocation(-1, -1, -1);
        }
        | '+' K_UNPLACED pt orient
        {
          if (defVersionNum < 5.4) {   /* PCR 495463 */
            if (defrReader::get()->getComponentCbk()) {
              defrReader::get()->getComponent().defiComponent::setPlacementStatus(
                                          DEFI_COMPONENT_UNPLACED);
              defrReader::get()->getComponent().defiComponent::setPlacementLocation($3.x, $3.y, $4);
            }
          } else {
            if (componentWarnings++ < defrReader::get()->getComponentWarnings())
               defWarning(7022, "In the COMPONENT UNPLACED statement, point and orient are invalid in version 5.4 and later.\nThe DEF parser will ignore this statement.");
            /* Should not be called.  Def parser should ignore the statement
            if (defrReader::get()->getComponentCbk())
              defrReader::get()->getComponent().defiComponent::setPlacementStatus(
                                           DEFI_COMPONENT_UNPLACED);
              defrReader::get()->getComponent().defiComponent::setPlacementLocation($3.x, $3.y, $4);
            */
          }
        }

maskShift: '+' K_MASKSHIFT NUMBER
        {  
          if (defrReader::get()->getComponentCbk()) {
            if (validateMaskInput((int)$3, componentWarnings, defrReader::get()->getComponentWarnings())) {
                defrReader::get()->getComponent().defiComponent::setMaskShift(int($3));
            }
          }
        }
        
placement_status: '+' K_FIXED 
        { $$ = DEFI_COMPONENT_FIXED; }
        | '+' K_COVER 
        { $$ = DEFI_COMPONENT_COVER; }
        | '+' K_PLACED
        { $$ = DEFI_COMPONENT_PLACED; }

weight: '+' K_WEIGHT NUMBER 
        {
          if (defrReader::get()->getComponentCbk())
            defrReader::get()->getComponent().defiComponent::setWeight(ROUND($3));
        }

end_comps: K_END K_COMPS
        { 
          if (defrReader::get()->getComponentCbk())
            CALLBACK(defrReader::get()->getComponentEndCbk(), defrComponentEndCbkType, 0);
        }

nets_section:  start_nets net_rules end_nets
        ;

start_nets: K_NETS NUMBER ';'
	{ 
          if (defrReader::get()->getNetStartCbk())
            CALLBACK(defrReader::get()->getNetStartCbk(), defrNetStartCbkType, ROUND($2));
          netOsnet = 1;
        }

net_rules: /* empty */
        | net_rules one_net
            ;

one_net: net_and_connections net_options ';'
	{ 
          if (defrReader::get()->getNetCbk())
            CALLBACK(defrReader::get()->getNetCbk(), defrNetCbkType, &defrReader::get()->getNet());
        }
/*
** net_and_connections: net_start {dumb_mode = 1000000000; no_num = 10000000;}
**                      net_connections
** wmd -- this can be used to replace
**        | '(' K_PIN {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')' (???)
*/
net_and_connections: net_start
        {dumb_mode = 0; no_num = 0; }

/* pcr 235555 & 236210 */
net_start: '-' {dumb_mode = 1000000000; no_num = 10000000; nondef_is_keyword = TRUE; mustjoin_is_keyword = TRUE;} net_name 

net_name: T_STRING
        {
          /* 9/22/1999 */
          /* this is shared by both net and special net */
          if ((defrReader::get()->getNetCbk() && (netOsnet==1)) || (defrReader::get()->getSNetCbk() && (netOsnet==2)))
            defrReader::get()->getNet().defiNet::setName($1);
          if (defrReader::get()->getNetNameCbk())
            CALLBACK(defrReader::get()->getNetNameCbk(), defrNetNameCbkType, $1);
        } net_connections
        | K_MUSTJOIN '(' T_STRING {dumb_mode = 1; no_num = 1;} T_STRING ')'
        {
          if ((defrReader::get()->getNetCbk() && (netOsnet==1)) || (defrReader::get()->getSNetCbk() && (netOsnet==2)))
            defrReader::get()->getNet().defiNet::addMustPin($3, $5, 0);
          dumb_mode = 3;
          no_num = 3;
        }

net_connections: /* empty */
        | net_connections net_connection 
        ;

net_connection: '(' T_STRING {dumb_mode = 1000000000; no_num = 10000000;}
                T_STRING conn_opt ')'
        {
          /* 9/22/1999 */
          /* since the code is shared by both net & special net, */
          /* need to check on both flags */
          if ((defrReader::get()->getNetCbk() && (netOsnet==1)) || (defrReader::get()->getSNetCbk() && (netOsnet==2)))
            defrReader::get()->getNet().defiNet::addPin($2, $4, $5);
          /* 1/14/2000 - pcr 289156 */
          /* reset dumb_mode & no_num to 3 , just in case */
          /* the next statement is another net_connection */
          dumb_mode = 3;
          no_num = 3;
        }
        | '(' '*' {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')'
        {
          if ((defrReader::get()->getNetCbk() && (netOsnet==1)) || (defrReader::get()->getSNetCbk() && (netOsnet==2)))
            defrReader::get()->getNet().defiNet::addPin("*", $4, $5);
          dumb_mode = 3;
          no_num = 3;
        }
        | '(' K_PIN {dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')'
        {
          if ((defrReader::get()->getNetCbk() && (netOsnet==1)) || (defrReader::get()->getSNetCbk() && (netOsnet==2)))
            defrReader::get()->getNet().defiNet::addPin("PIN", $4, $5);
          dumb_mode = 3;
          no_num = 3;
        }

conn_opt: /* empty */
          { $$ = 0; }
        | extension_stmt
        {
          if (defrReader::get()->getNetConnectionExtCbk())
	    CALLBACK(defrReader::get()->getNetConnectionExtCbk(), defrNetConnectionExtCbkType,
              History_text);
          $$ = 0;
        }
        | '+' K_SYNTHESIZED
        { $$ = 1; }
        
        
/* These are all the optional fields for a net that go after the '+' */
net_options: /* empty */
        | net_options net_option
        ;

net_option: '+' net_type 
        {  
          if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addWire($2, NULL);
        }
         paths
        {
          by_is_keyword = FALSE;
          do_is_keyword = FALSE;
          new_is_keyword = FALSE;
          nondef_is_keyword = FALSE;
          mustjoin_is_keyword = FALSE;
          step_is_keyword = FALSE;
          orient_is_keyword = FALSE;
          virtual_is_keyword = FALSE;
          rect_is_keyword = FALSE;
          mask_is_keyword = FALSE;
          needNPCbk = 0;
        }

        | '+' K_SOURCE netsource_type
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setSource($3); }

        | '+' K_FIXEDBUMP
        {
          if (defVersionNum < 5.5) {
            if (defrReader::get()->getNetCbk()) {
              if (netWarnings++ < defrReader::get()->getNetWarnings()) {
                 defMsg = (char*)defMalloc(1000);
                 sprintf (defMsg,
                    "The FIXEDBUMP statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                 defError(6530, defMsg);
                 defFree(defMsg);
                 CHKERR();
              }
            }
          }
          if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setFixedbump();
        } 

        | '+' K_FREQUENCY { real_num = 1; } NUMBER
        {
          if (defVersionNum < 5.5) {
            if (defrReader::get()->getNetCbk()) {
              if (netWarnings++ < defrReader::get()->getNetWarnings()) {
                 defMsg = (char*)defMalloc(1000);
                 sprintf (defMsg,
                    "The FREQUENCY statement is a version 5.5 and later syntax.\nHowever, your DEF file is defined with version %g", defVersionNum);
                 defError(6558, defMsg);
                 defFree(defMsg);
                 CHKERR();
              }
            }
          }
          if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setFrequency($4);
          real_num = 0;
        }

        | '+' K_ORIGINAL {dumb_mode = 1; no_num = 1;} T_STRING
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setOriginal($4); }

        | '+' K_PATTERN pattern_type
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setPattern($3); }

        | '+' K_WEIGHT NUMBER
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setWeight(ROUND($3)); }

        | '+' K_XTALK NUMBER
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setXTalk(ROUND($3)); }

        | '+' K_ESTCAP NUMBER
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setCap($3); }

        | '+' K_USE use_type 
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setUse($3); }

        | '+' K_STYLE NUMBER
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setStyle((int)$3); }

        | '+' K_NONDEFAULTRULE { dumb_mode = 1; no_num = 1; } T_STRING
        { 
          if (defrReader::get()->getNetCbk() && defrReader::get()->getNetNonDefaultRuleCbk()) {
             /* User wants a callback on nondefaultrule */
             CALLBACK(defrReader::get()->getNetNonDefaultRuleCbk(),
                      defrNetNonDefaultRuleCbkType, $4);
          }
          /* Still save data in the class */
          if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::setNonDefaultRule($4);
        }

        | vpin_stmt

        | '+' K_SHIELDNET { dumb_mode = 1; no_num = 1; } T_STRING
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addShieldNet($4); }

        | '+' K_NOSHIELD { dumb_mode = 1; no_num = 1; }
        { /* since the parser still support 5.3 and earlier, can't */
          /* move NOSHIELD in net_type */
          if (defVersionNum < 5.4) {   /* PCR 445209 */
            if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addNoShield("");
            by_is_keyword = FALSE;
            do_is_keyword = FALSE;
            new_is_keyword = FALSE;
            step_is_keyword = FALSE;
            orient_is_keyword = FALSE;
            virtual_is_keyword = FALSE;
            mask_is_keyword = FALSE;
            rect_is_keyword = FALSE;
            shield = TRUE;    /* save the path info in the shield paths */
          } else
            if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addWire("NOSHIELD", NULL);
        }
        paths
        {
          if (defVersionNum < 5.4) {   /* PCR 445209 */
            shield = FALSE;
            by_is_keyword = FALSE;
            do_is_keyword = FALSE;
            new_is_keyword = FALSE;
            step_is_keyword = FALSE;
            nondef_is_keyword = FALSE;
            mustjoin_is_keyword = FALSE;
            orient_is_keyword = FALSE;
            virtual_is_keyword = FALSE;
            rect_is_keyword = FALSE;
            mask_is_keyword = FALSE;
          } else {
            by_is_keyword = FALSE;
            do_is_keyword = FALSE;
            new_is_keyword = FALSE;
            step_is_keyword = FALSE;
            nondef_is_keyword = FALSE;
            mustjoin_is_keyword = FALSE;
            orient_is_keyword = FALSE;
            virtual_is_keyword = FALSE;
            rect_is_keyword = FALSE;
            mask_is_keyword = FALSE;
          }
          needNPCbk = 0;
        }

        | '+' K_SUBNET
        { dumb_mode = 1; no_num = 1;
          if (defrReader::get()->getNetCbk()) {
            defrReader::get()->setSubnet((defiSubnet*)defMalloc(sizeof(defiSubnet)));
            defrReader::get()->getSubnet()->defiSubnet::Init();
          }
        }
        T_STRING {
          if (defrReader::get()->getNetCbk() && defrReader::get()->getNetSubnetNameCbk()) {
            /* User wants a callback on Net subnetName */
            CALLBACK(defrReader::get()->getNetSubnetNameCbk(), defrNetSubnetNameCbkType, $4);
          }
          /* Still save the subnet name in the class */
          if (defrReader::get()->getNetCbk()) {
            defrReader::get()->getSubnet()->defiSubnet::setName($4);
          }
        } 
        comp_names {
          routed_is_keyword = TRUE;
          fixed_is_keyword = TRUE;
          cover_is_keyword = TRUE;
        } subnet_options {
          if (defrReader::get()->getNetCbk()) {
            defrReader::get()->getNet().defiNet::addSubnet(defrReader::get()->getSubnet());
            defrReader::get()->setSubnet(0);
            routed_is_keyword = FALSE;
            fixed_is_keyword = FALSE;
            cover_is_keyword = FALSE;
          }
        }

        | '+' K_PROPERTY {dumb_mode = 10000000; parsing_property = 1; }
          net_prop_list
        { dumb_mode = 0; parsing_property = 0; }

        | extension_stmt
        { 
          if (defrReader::get()->getNetExtCbk())
            CALLBACK(defrReader::get()->getNetExtCbk(), defrNetExtCbkType, History_text);
        }

net_prop_list: net_prop
      | net_prop_list net_prop
      ;

net_prop: T_STRING NUMBER
        {
          if (defrReader::get()->getNetCbk()) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defrReader::get()->getNetProp().defiPropType::propType($1);
            CHKPROPTYPE(propTp, $1, "NET");
            /* For backword compatibility, also set the string value */
            /* We will use a temporary string to store the number.
             * The string space is borrowed from the ring buffer
             * in the lexer. */
            sprintf(str, "%g", $2);
            defrReader::get()->getNet().defiNet::addNumProp($1, $2, str, propTp);
          }
        }
        | T_STRING QSTRING
        {
          if (defrReader::get()->getNetCbk()) {
            char propTp;
            propTp = defrReader::get()->getNetProp().defiPropType::propType($1);
            CHKPROPTYPE(propTp, $1, "NET");
            defrReader::get()->getNet().defiNet::addProp($1, $2, propTp);
          }
        }
        | T_STRING T_STRING
        {
          if (defrReader::get()->getNetCbk()) {
            char propTp;
            propTp = defrReader::get()->getNetProp().defiPropType::propType($1);
            CHKPROPTYPE(propTp, $1, "NET");
            defrReader::get()->getNet().defiNet::addProp($1, $2, propTp);
          }
        }

netsource_type: K_NETLIST
        { $$ = (char*)"NETLIST"; }
        | K_DIST
        { $$ = (char*)"DIST"; }
        | K_USER
        { $$ = (char*)"USER"; }
        | K_TIMING
        { $$ = (char*)"TIMING"; }
        | K_TEST
        { $$ = (char*)"TEST"; }

vpin_stmt: vpin_begin vpin_layer_opt pt pt 
        {
          /* vpin_options may have to deal with orient */
          orient_is_keyword = TRUE;
        }
        vpin_options
        { if (defrReader::get()->getNetCbk())
            defrReader::get()->getNet().defiNet::addVpinBounds($3.x, $3.y, $4.x, $4.y);
          orient_is_keyword = FALSE;
        }

vpin_begin: '+' K_VPIN {dumb_mode = 1; no_num = 1;} T_STRING
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addVpin($4); }

vpin_layer_opt: /* empty */
        | K_LAYER {dumb_mode=1;} T_STRING
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addVpinLayer($3); }

vpin_options: /* empty */
        | vpin_status pt orient
        { if (defrReader::get()->getNetCbk()) defrReader::get()->getNet().defiNet::addVpinLoc($1, $2.x, $2.y, $3); }

vpin_status: K_PLACED 
        { $$ = (char*)"PLACED"; }
        | K_FIXED 
        { $$ = (char*)"FIXED"; }
        | K_COVER
        { $$ = (char*)"COVER"; }

net_type: K_FIXED
        { $$ = (char*)"FIXED"; dumb_mode = 1; }
        | K_COVER
        { $$ = (char*)"COVER"; dumb_mode = 1; }
        | K_ROUTED
        { $$ = (char*)"ROUTED"; dumb_mode = 1; }

paths:
    path   // not necessary to do partial callback for net yet
      { if (defrReader::get()->needPathData() && defrReader::get()->getNetCbk())
          defrReader::get()->pathIsDone(shield, 0, netOsnet, &needNPCbk);
      }
    | paths new_path
      { }

new_path: K_NEW { dumb_mode = 1; } path
      { if (defrReader::get()->needPathData() && defrReader::get()->getNetCbk())
          defrReader::get()->pathIsDone(shield, 0, netOsnet, &needNPCbk);
      }

path:  T_STRING
      {
        /* 9/26/2002 - pcr 449514
        Check if $1 is equal to TAPER or TAPERRULE, layername
        is missing */
        if ((strcmp($1, "TAPER") == 0) || (strcmp($1, "TAPERRULE") == 0)) {
          if (defrReader::get()->needPathData() && defrReader::get()->getNetCbk()) {
            if (netWarnings++ < defrReader::get()->getNetWarnings()) {
              defError(6531, "The layerName which is required in path is missing. Include the layerName in the path and then try again.");
              CHKERR();
            }
          }
          /* Since there is already error, the next token is insignificant */
          dumb_mode = 1; no_num = 1;
        } else {
          /* CCR 766289 - Do not accummulate the layer information if there */
          /* is not a callback set */
          if (defrReader::get()->needPathData() && defrReader::get()->getNetCbk())
              defrReader::get()->getPath()->defiPath::addLayer($1);
          dumb_mode = 0; no_num = 0;
        }
      }
    opt_taper_style_s  path_pt
      { dumb_mode = 1000000000; by_is_keyword = TRUE; do_is_keyword = TRUE;
/*
       dumb_mode = 1; by_is_keyword = TRUE; do_is_keyword = TRUE;
*/
        new_is_keyword = TRUE; step_is_keyword = TRUE; 
        orient_is_keyword = TRUE; virtual_is_keyword = TRUE;
        mask_is_keyword = TRUE, rect_is_keyword = TRUE;  }
    
       path_item_list
     
      { dumb_mode = 0;   virtual_is_keyword = FALSE; mask_is_keyword = FALSE,
       rect_is_keyword = FALSE; }
    
virtual_statement :
    K_VIRTUAL virtual_pt
    {
      if (defVersionNum < 5.8) {
              if (defrReader::get()->getSNetCbk()) {
                if (sNetWarnings++ < defrReader::get()->getSNetWarnings()) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The VIRTUAL statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
                  defError(6536, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
          }
    } 
  
rect_statement : 
    K_RECT rect_pts
    {
      if (defVersionNum < 5.8) {
              if (defrReader::get()->getSNetCbk()) {
                if (sNetWarnings++ < defrReader::get()->getSNetWarnings()) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The RECT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
                  defError(6536, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
      }
    } 
              
    
path_item_list: /* empty */
    | path_item_list path_item
    ;


path_item:
     T_STRING
      {
        if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
            (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
          if (strcmp($1, "TAPER") == 0)
            defrReader::get()->getPath()->defiPath::setTaper();
          else {
            defrReader::get()->getPath()->defiPath::addVia($1);
            }
        }
      }
    | K_MASK NUMBER T_STRING
      {
        if (validateMaskInput((int)$2, sNetWarnings, defrReader::get()->getSNetWarnings())) {
            if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
                (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
              if (strcmp($3, "TAPER") == 0)
                defrReader::get()->getPath()->defiPath::setTaper();
              else {
                defrReader::get()->getPath()->defiPath::addViaMask((int)$2);
                defrReader::get()->getPath()->defiPath::addVia($3);
                }
            }
        }
      }
    | T_STRING orient
      { if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
            (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
            defrReader::get()->getPath()->defiPath::addVia($1);
            defrReader::get()->getPath()->defiPath::addViaRotation($2);
        }
      }
    | K_MASK NUMBER T_STRING orient
      { 
        if (validateMaskInput((int)$2, sNetWarnings, defrReader::get()->getSNetWarnings())) {
            if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
                (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
                defrReader::get()->getPath()->defiPath::addViaMask((int)$2);
                defrReader::get()->getPath()->defiPath::addVia($3);
                defrReader::get()->getPath()->defiPath::addViaRotation($4);
            }
        }
      }
    | K_MASK NUMBER T_STRING K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (validateMaskInput((int)$2, sNetWarnings, defrReader::get()->getSNetWarnings())) {      
            if (($5 == 0) || ($7 == 0)) {
              if (defrReader::get()->needPathData() &&
                  defrReader::get()->getSNetCbk()) {
                if (netWarnings++ < defrReader::get()->getNetWarnings()) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->needPathData() && (defrReader::get()->getSNetCbk() && (netOsnet==2))) {
                defrReader::get()->getPath()->defiPath::addViaMask((int)$2);
                defrReader::get()->getPath()->defiPath::addVia($3);
                defrReader::get()->getPath()->defiPath::addViaData((int)$5, (int)$7, (int)$9, (int)$10);
            }  else if (defrReader::get()->needPathData() && (defrReader::get()->getNetCbk() && (netOsnet==1))) {
              if (netWarnings++ < defrReader::get()->getNetWarnings()) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
    | T_STRING K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (defVersionNum < 5.5) {
          if (defrReader::get()->needPathData() && 
              defrReader::get()->getSNetCbk()) {
            if (netWarnings++ < defrReader::get()->getNetWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
              defError(6532, defMsg);
              defFree(defMsg);
              CHKERR();
            }
          }
        }
        if (($3 == 0) || ($5 == 0)) {
          if (defrReader::get()->needPathData() &&
              defrReader::get()->getSNetCbk()) {
            if (netWarnings++ < defrReader::get()->getNetWarnings()) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defrReader::get()->needPathData() && (defrReader::get()->getSNetCbk() && (netOsnet==2))) {
            defrReader::get()->getPath()->defiPath::addVia($1);
            defrReader::get()->getPath()->defiPath::addViaData((int)$3, (int)$5, (int)$7, (int)$8);
        }  else if (defrReader::get()->needPathData() && (defrReader::get()->getNetCbk() && (netOsnet==1))) {
          if (netWarnings++ < defrReader::get()->getNetWarnings()) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
    | T_STRING orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (defVersionNum < 5.5) {
          if (defrReader::get()->needPathData() &&
              defrReader::get()->getSNetCbk()) {
            if (netWarnings++ < defrReader::get()->getNetWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
              defError(6532, defMsg);
              CHKERR();
            }
          }
        }
        if (($4 == 0) || ($6 == 0)) {
          if (defrReader::get()->needPathData() &&
              defrReader::get()->getSNetCbk()) {
            if (netWarnings++ < defrReader::get()->getNetWarnings()) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defrReader::get()->needPathData() && (defrReader::get()->getSNetCbk() && (netOsnet==2))) {
            defrReader::get()->getPath()->defiPath::addVia($1);
            defrReader::get()->getPath()->defiPath::addViaRotation($2);
            defrReader::get()->getPath()->defiPath::addViaData((int)$4, (int)$6, (int)$8, (int)$9);
        } else if (defrReader::get()->needPathData() && (defrReader::get()->getNetCbk() && (netOsnet==1))) {
          if (netWarnings++ < defrReader::get()->getNetWarnings()) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
    | K_MASK NUMBER T_STRING orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (validateMaskInput((int)$2, sNetWarnings, defrReader::get()->getSNetWarnings())) {
            if (($6 == 0) || ($8 == 0)) {
              if (defrReader::get()->needPathData() &&
                  defrReader::get()->getSNetCbk()) {
                if (netWarnings++ < defrReader::get()->getNetWarnings()) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->needPathData() && (defrReader::get()->getSNetCbk() && (netOsnet==2))) {
                defrReader::get()->getPath()->defiPath::addViaMask((int)$2); 
                defrReader::get()->getPath()->defiPath::addVia($3);
                defrReader::get()->getPath()->defiPath::addViaRotation($4);;
                defrReader::get()->getPath()->defiPath::addViaData((int)$6, (int)$8, (int)$10, (int)$11);
            } else if (defrReader::get()->needPathData() && (defrReader::get()->getNetCbk() && (netOsnet==1))) {
              if (netWarnings++ < defrReader::get()->getNetWarnings()) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
   | virtual_statement
   | rect_statement 
   | K_MASK NUMBER K_RECT { dumb_mode = 6; } '(' NUMBER NUMBER NUMBER NUMBER ')'
    {
      if (validateMaskInput((int)$2, sNetWarnings, defrReader::get()->getSNetWarnings())) {
        if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
	  defrReader::get()->getPath()->defiPath::addMask((int)$2);
	  defrReader::get()->getPath()->defiPath::addViaRect((int)$6, (int)$7, (int)$8, (int)$9);
	}
      }
    }
   | K_MASK NUMBER
    {
       if (validateMaskInput((int)$2, sNetWarnings, defrReader::get()->getSNetWarnings())) {
        if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
	  defrReader::get()->getPath()->defiPath::addMask((int)$2); 
	}
       }  
    }
    path_pt
  | path_pt 
    {
       // reset dumb_mode to 1 just incase the next token is a via of the path
        // 2/5/2004 - pcr 686781
        dumb_mode = 100000; by_is_keyword = TRUE; do_is_keyword = TRUE;
        new_is_keyword = TRUE; step_is_keyword = TRUE;
        orient_is_keyword = TRUE;
    }  

      
path_pt :
     '(' NUMBER NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addPoint(ROUND($2), ROUND($3)); 
	save_x = $2;
	save_y = $3; 
      }
    | '(' '*' NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addPoint(ROUND(save_x), ROUND($3)); 
	save_y = $3;
      }
    | '(' NUMBER '*' ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addPoint(ROUND($2), ROUND(save_y)); 
	save_x = $2;
      }
    | '(' '*' '*' ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
            (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addPoint(ROUND(save_x), ROUND(save_y)); 
      }
    | '(' NUMBER NUMBER NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
            (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addFlushPoint(ROUND($2), ROUND($3), ROUND($4)); 
	save_x = $2;
	save_y = $3;
      }
    | '(' '*' NUMBER NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addFlushPoint(ROUND(save_x), ROUND($3),
	  ROUND($4)); 
	save_y = $3;
      }
    | '(' NUMBER '*' NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addFlushPoint(ROUND($2), ROUND(save_y),
	  ROUND($4)); 
	save_x = $2;
      }
    | '(' '*' '*' NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addFlushPoint(ROUND(save_x), ROUND(save_y),
	  ROUND($4)); 
      }

virtual_pt :
     '(' NUMBER NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addVirtualPoint(ROUND($2), ROUND($3)); 
	save_x = $2;
	save_y = $3;
      }
    | '(' '*' NUMBER ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addVirtualPoint(ROUND(save_x), ROUND($3)); 
	save_y = $3;
      }
    | '(' NUMBER '*' ')'
      {
	if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
	  defrReader::get()->getPath()->defiPath::addVirtualPoint(ROUND($2), ROUND(save_y)); 
	save_x = $2;
      }    
 
rect_pts :
    '(' NUMBER NUMBER NUMBER NUMBER ')'
    {
        if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
	  defrReader::get()->getPath()->defiPath::addViaRect((int)$2, (int)$3, (int)$4, (int)$5); 
	}    
    }   
    
                
opt_taper_style_s: /* empty */
    | opt_taper_style_s opt_taper_style
    ;
opt_taper_style: opt_style
    | opt_taper
    ;

opt_taper: K_TAPER
      { if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
          defrReader::get()->getPath()->defiPath::setTaper(); }
    | K_TAPERRULE { dumb_mode = 1; } T_STRING
      { if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
          defrReader::get()->getPath()->defiPath::addTaperRule($3); }

opt_style: K_STYLE NUMBER
      { 
        if (defVersionNum < 5.6) {
           if (defrReader::get()->needPathData() && (defrReader::get()->getNetCbk() ||
               defrReader::get()->getSNetCbk())) {
             if (netWarnings++ < defrReader::get()->getNetWarnings()) {
               defMsg = (char*)defMalloc(1000);
               sprintf (defMsg,
                  "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
               defError(6534, defMsg);
               defFree(defMsg);
               CHKERR();
             }
           }
        } else
           if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
             (defrReader::get()->getSNetCbk() && (netOsnet==2))))
             defrReader::get()->getPath()->defiPath::addStyle((int)$2);
      }

opt_spaths: /* empty */
    | opt_spaths opt_shape_style
    ;

opt_shape_style:
    '+' K_SHAPE shape_type
      { if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
          (defrReader::get()->getSNetCbk() && (netOsnet==2))))
          defrReader::get()->getPath()->defiPath::addShape($3); }
    | '+' K_STYLE NUMBER
      { if (defVersionNum < 5.6) {
          if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
            (defrReader::get()->getSNetCbk() && (netOsnet==2)))) {
            if (netWarnings++ < defrReader::get()->getNetWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
              defError(6534, defMsg);
              defFree(defMsg);
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->needPathData() && ((defrReader::get()->getNetCbk() && (netOsnet==1)) ||
            (defrReader::get()->getSNetCbk() && (netOsnet==2))))
            defrReader::get()->getPath()->defiPath::addStyle((int)$3);
        }
      }

end_nets: K_END K_NETS 
	  { 
            CALLBACK(defrReader::get()->getNetEndCbk(), defrNetEndCbkType, 0);
            netOsnet = 0;
          }

shape_type: K_RING
            { $$ = (char*)"RING"; }
        | K_STRIPE
            { $$ = (char*)"STRIPE"; }
        | K_FOLLOWPIN
            { $$ = (char*)"FOLLOWPIN"; }
        | K_IOWIRE
            { $$ = (char*)"IOWIRE"; }
        | K_COREWIRE
            { $$ = (char*)"COREWIRE"; }
        | K_BLOCKWIRE
            { $$ = (char*)"BLOCKWIRE"; }
        | K_FILLWIRE
            { $$ = (char*)"FILLWIRE"; }
        | K_FILLWIREOPC                         /* 5.7 */
            {
              if (defVersionNum < 5.7) {
                 if (defrReader::get()->needPathData()) {
                   if (fillWarnings++ < defrReader::get()->getFillWarnings()) {
                     defMsg = (char*)defMalloc(10000);
                     sprintf (defMsg,
                       "The FILLWIREOPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                     defError(6552, defMsg);
                     defFree(defMsg);
                     CHKERR();
                  }
                }
              }
              $$ = (char*)"FILLWIREOPC";
            }
        | K_DRCFILL
            { $$ = (char*)"DRCFILL"; }
        | K_BLOCKAGEWIRE
            { $$ = (char*)"BLOCKAGEWIRE"; }
        | K_PADRING
            { $$ = (char*)"PADRING"; }
        | K_BLOCKRING
            { $$ = (char*)"BLOCKRING"; }

snets_section :  start_snets snet_rules end_snets
            ;

snet_rules: /* empty */
        | snet_rules snet_rule
            ;

snet_rule: net_and_connections snet_options ';'
	{ CALLBACK(defrReader::get()->getSNetCbk(), defrSNetCbkType, &defrReader::get()->getNet()); }

snet_options: /* empty */
        | snet_options snet_option
            ;

snet_option: snet_width | snet_voltage | 
             snet_spacing | snet_other_option
             ;

snet_other_option: '+' net_type
            {
             if (defVersionNum >= 5.8) {
	        specialWire_routeStatus = $2;
	     } else {
                 if (defrReader::get()->getSNetCbk()) {   /* PCR 902306 */
                   defMsg = (char*)defMalloc(1024);
                   sprintf(defMsg, "The SPECIAL NET statement, with type %s, does not have any net statement defined.\nThe DEF parser will ignore this statemnet.", $2);
                   defWarning(7023, defMsg);
                   defFree(defMsg);
                 }
             }
            }
        |  '+' net_type
            {
            if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::addWire($2, NULL);
            }
            spaths
            {
            // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
            if (defrReader::get()->getSNetWireCbk()) {
               CALLBACK(defrReader::get()->getSNetWireCbk(), defrSNetWireCbkType, &defrReader::get()->getNet());
               defrReader::get()->getNet().defiNet::freeWire();
            }
            by_is_keyword = FALSE;
            do_is_keyword = FALSE;
            new_is_keyword = FALSE;
            step_is_keyword = FALSE;
            orient_is_keyword = FALSE;
            virtual_is_keyword = FALSE;
            mask_is_keyword = FALSE;
            rect_is_keyword = FALSE;
            needSNPCbk = 0;
            }
 
        | '+' K_SHIELD { dumb_mode = 1; no_num = 1; } T_STRING
            { shieldName = $4; 
              specialWire_routeStatus = (char*)"SHIELD";
              specialWire_routeStatusName = $4; 
            }
            shield_layer
            
        | '+' K_SHAPE shape_type
          {  
            specialWire_shapeType = $3;
          }
        | '+' K_MASK NUMBER
          {
            if (validateMaskInput((int)$3, sNetWarnings, defrReader::get()->getSNetWarnings())) {
                specialWire_mask = (int)$3;
            }     
          }
        | '+' K_POLYGON { dumb_mode = 1; } T_STRING
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getSNetCbk()) {
                if (sNetWarnings++ < defrReader::get()->getSNetWarnings()) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
                  defError(6535, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (!defrReader::get()->getGeomPtr()) {
              defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
              defrReader::get()->getGeomPtr()->defiGeometries::Init();
            } else {  // Just reset the number of points
              defrReader::get()->getGeomPtr()->defiGeometries::Reset();
            }
          }
          firstPt nextPt nextPt otherPts
          {
            if (defVersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defrReader::get()->getSNetCbk()) {
                // needSNPCbk will indicate that it has reach the max
                // memory and if user has set partialPathCBk, def parser
                // will make the callback.
                // This will improve performance
                // This construct is only in specialnet
                defrReader::get()->getNet().defiNet::addPolygon($4, defrReader::get()->getGeomPtr(), &needSNPCbk, specialWire_mask, specialWire_routeStatus, specialWire_shapeType,
                                                                specialWire_routeStatusName);
                specialWire_mask = 0;
                specialWire_routeStatus = (char*)"ROUTED";
		specialWire_shapeType = (char*)"";
                if (needSNPCbk && defrReader::get()->getSNetPartialPathCbk()) {
                   CALLBACK(defrReader::get()->getSNetPartialPathCbk(), defrSNetPartialPathCbkType,
                            &defrReader::get()->getNet());
                   defrReader::get()->getNet().defiNet::clearRectPolyNPath();
		   defrReader::get()->getNet().defiNet::clearVia();
                }
              }
            }
          }

        | '+' K_RECT { dumb_mode = 1; } T_STRING pt pt
          {
            if (defVersionNum < 5.6) {
              if (defrReader::get()->getSNetCbk()) {
                if (sNetWarnings++ < defrReader::get()->getSNetWarnings()) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The RECT statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
                  defError(6536, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
            }
            if (defrReader::get()->getSNetCbk()) {
              // needSNPCbk will indicate that it has reach the max
              // memory and if user has set partialPathCBk, def parser
              // will make the callback.
              // This will improve performance
              // This construct is only in specialnet
              defrReader::get()->getNet().defiNet::addRect($4, $5.x, $5.y, $6.x, $6.y, &needSNPCbk, specialWire_mask, specialWire_routeStatus, specialWire_shapeType, specialWire_routeStatusName);
              specialWire_mask = 0;
              specialWire_routeStatus = (char*)"ROUTED";
	      specialWire_shapeType = (char*)"";
	      specialWire_routeStatusName = (char*)"";
              if (needSNPCbk && defrReader::get()->getSNetPartialPathCbk()) {
                 CALLBACK(defrReader::get()->getSNetPartialPathCbk(), defrSNetPartialPathCbkType,
                          &defrReader::get()->getNet());
                 defrReader::get()->getNet().defiNet::clearRectPolyNPath();
		 defrReader::get()->getNet().defiNet::clearVia();
              }
            }
          }
        | '+' K_VIA { dumb_mode = 1; } T_STRING orient_pt
        {
          if (defVersionNum < 5.8) {
              if (defrReader::get()->getSNetCbk()) {
                if (sNetWarnings++ < defrReader::get()->getSNetWarnings()) {
                  defMsg = (char*)defMalloc(1000);
                  sprintf (defMsg,
                     "The VIA statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
                  defError(6536, defMsg);
                  defFree(defMsg);
                  CHKERR();
                }
              }
          }
        }
        firstPt otherPts
        {
          if (defVersionNum >= 5.8 && defrReader::get()->getSNetCbk()) {
              defrReader::get()->getNet().defiNet::addPts($4, $5, defrReader::get()->getGeomPtr(), &needSNPCbk, specialWire_mask, specialWire_routeStatus, specialWire_shapeType,
                                                          specialWire_routeStatusName);
              specialWire_mask = 0;
              specialWire_routeStatus = (char*)"ROUTED";
	      specialWire_shapeType = (char*)"";
	      specialWire_routeStatusName = (char*)"";
              if (needSNPCbk && defrReader::get()->getSNetPartialPathCbk()) {
                 CALLBACK(defrReader::get()->getSNetPartialPathCbk(), defrSNetPartialPathCbkType,
                          &defrReader::get()->getNet());
		 defrReader::get()->getNet().defiNet::clearRectPolyNPath();
                 defrReader::get()->getNet().defiNet::clearVia();
              }
            }
        }
 
        | '+' K_SOURCE source_type
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setSource($3); }

        | '+' K_FIXEDBUMP
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setFixedbump(); }
 
        | '+' K_FREQUENCY NUMBER
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setFrequency($3); }

        | '+' K_ORIGINAL {dumb_mode = 1; no_num = 1;} T_STRING
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setOriginal($4); }
 
        | '+' K_PATTERN pattern_type
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setPattern($3); }
 
        | '+' K_WEIGHT NUMBER
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setWeight(ROUND($3)); }
 
        | '+' K_ESTCAP NUMBER
            { 
              /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
              if (defVersionNum < 5.5)
                 if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setCap($3);
              else
                 defWarning(7024, "The ESTCAP statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }
 
        | '+' K_USE use_type
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setUse($3); }
 
        | '+' K_STYLE NUMBER
            { if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setStyle((int)$3); }
 
        | '+' K_PROPERTY {dumb_mode = 10000000; parsing_property = 1; }
          snet_prop_list
            { dumb_mode = 0; parsing_property = 0; }
 
        | extension_stmt
          { CALLBACK(defrReader::get()->getNetExtCbk(), defrNetExtCbkType, History_text); }

orient_pt: /* empty */
        { $$ = 0; }
        | K_N  {$$ = 0;}
        | K_W  {$$ = 1;}
        | K_S  {$$ = 2;}
        | K_E  {$$ = 3;}
        | K_FN {$$ = 4;}
        | K_FW {$$ = 5;}
        | K_FS {$$ = 6;}
        | K_FE {$$ = 7;}
        
shield_layer: /* empty */  /* PCR 902306 */
            {
             if (defrReader::get()->getSNetCbk()) {
               defMsg = (char*)defMalloc(1024);
               sprintf(defMsg, "The SPECIAL NET SHIELD statement, does not have shield net statement defined.\nThe DEF parser will ignore this statemnet.");
               defWarning(7025, defMsg);
               defFree(defMsg);
             }
            }
        |
            { /* since the parser still supports 5.3 and earlier, */
              /* can't just move SHIELD in net_type */
              if (defVersionNum < 5.4) { /* PCR 445209 */
                if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::addShield(shieldName);
                by_is_keyword = FALSE;
                do_is_keyword = FALSE;
                new_is_keyword = FALSE;
                step_is_keyword = FALSE;
                orient_is_keyword = FALSE;
                virtual_is_keyword = FALSE;
                mask_is_keyword = FALSE;
                rect_is_keyword = FALSE;
                specialWire_routeStatus = (char*)"ROUTED";
	        specialWire_routeStatusName = (char*)"";
                shield = TRUE;   /* save the path info in the shield paths */
              } else
                if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::addWire("SHIELD", shieldName);
                specialWire_routeStatus = (char*)"ROUTED";
	        specialWire_routeStatusName = (char*)"";
            }
            spaths
            {
              // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
              if (defrReader::get()->getSNetWireCbk()) {
                 CALLBACK(defrReader::get()->getSNetWireCbk(), defrSNetWireCbkType, &defrReader::get()->getNet());
                 if (defVersionNum < 5.4)
                   defrReader::get()->getNet().defiNet::freeShield();
                 else
                   defrReader::get()->getNet().defiNet::freeWire();
              }
              if (defVersionNum < 5.4) {  /* PCR 445209 */
                shield = FALSE;
                by_is_keyword = FALSE;
                do_is_keyword = FALSE;
                new_is_keyword = FALSE;
                step_is_keyword = FALSE;
                nondef_is_keyword = FALSE;
                mustjoin_is_keyword = FALSE;
                orient_is_keyword = FALSE;
                virtual_is_keyword = FALSE;
                mask_is_keyword = FALSE;
                rect_is_keyword = FALSE;
              } else {
                by_is_keyword = FALSE;
                do_is_keyword = FALSE;
                new_is_keyword = FALSE;
                step_is_keyword = FALSE;
                orient_is_keyword = FALSE;
                virtual_is_keyword = FALSE;
                mask_is_keyword = FALSE;
                rect_is_keyword = FALSE;
              }
              needSNPCbk = 0;
            }

snet_width: '+' K_WIDTH { dumb_mode = 1; } T_STRING NUMBER
            {
              /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
              if (defVersionNum < 5.5)
                 if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setWidth($4, $5);
              else
                 defWarning(7026, "The WIDTH statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }

snet_voltage: '+' K_VOLTAGE  { dumb_mode = 1; no_num = 1; } T_STRING
            {
              if (numIsInt($4)) {
                 if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setVoltage(atoi($4));
              } else {
                 if (defrReader::get()->getSNetCbk()) {
                   if (sNetWarnings++ < defrReader::get()->getSNetWarnings()) {
                     defMsg = (char*)defMalloc(1000);
                     sprintf (defMsg,
                        "The value %s for statement VOLTAGE is invalid. The value can only be integer.\nSpecify a valid value in units of millivolts", $4);
                     defError(6537, defMsg);
                     defFree(defMsg);
                     CHKERR();
                   }
                 }
              }
            }

snet_spacing: '+' K_SPACING { dumb_mode = 1; } T_STRING NUMBER
            {
              if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setSpacing($4,$5);
            }
        opt_snet_range
            {
            }

snet_prop_list: snet_prop
      | snet_prop_list snet_prop
      ;

snet_prop: T_STRING NUMBER
            {
              if (defrReader::get()->getSNetCbk()) {
                char propTp;
                char* str = ringCopy("                       ");
                propTp = defrReader::get()->getSNetProp().defiPropType::propType($1);
                CHKPROPTYPE(propTp, $1, "SPECIAL NET");
                /* For backword compatibility, also set the string value */
                /* We will use a temporary string to store the number.
                 * The string space is borrowed from the ring buffer
                 * in the lexer. */
                sprintf(str, "%g", $2);
                defrReader::get()->getNet().defiNet::addNumProp($1, $2, str, propTp);
              }
            }
         | T_STRING QSTRING
            {
              if (defrReader::get()->getSNetCbk()) {
                char propTp;
                propTp = defrReader::get()->getSNetProp().defiPropType::propType($1);
                CHKPROPTYPE(propTp, $1, "SPECIAL NET");
                defrReader::get()->getNet().defiNet::addProp($1, $2, propTp);
              }
            }
         | T_STRING T_STRING
            {
              if (defrReader::get()->getSNetCbk()) {
                char propTp;
                propTp = defrReader::get()->getSNetProp().defiPropType::propType($1);
                CHKPROPTYPE(propTp, $1, "SPECIAL NET");
                defrReader::get()->getNet().defiNet::addProp($1, $2, propTp);
              }
            }

opt_snet_range: /* nothing */
        | K_RANGE NUMBER NUMBER
            {
              if (defrReader::get()->getSNetCbk()) defrReader::get()->getNet().defiNet::setRange($2,$3);
            }

opt_range: /* nothing */
        | K_RANGE NUMBER NUMBER
            { defrReader::get()->getProp().defiProp::setRange($2, $3); }

pattern_type: K_BALANCED
            { $$ = (char*)"BALANCED"; }
        | K_STEINER
            { $$ = (char*)"STEINER"; }
        | K_TRUNK
            { $$ = (char*)"TRUNK"; }
        | K_WIREDLOGIC
            { $$ = (char*)"WIREDLOGIC"; }

spaths:
    spath
      { 
        if (defrReader::get()->needPathData() && defrReader::get()->getSNetCbk()) {
           if (needSNPCbk && defrReader::get()->getSNetPartialPathCbk()) { 
              // require a callback before proceed because needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defrReader::get()->getSNetPartialPathCbk(), defrSNetPartialPathCbkType,
                       &defrReader::get()->getNet());
              needSNPCbk = 0;   // reset the flag
              defrReader::get()->pathIsDone(shield, 1, netOsnet, &needSNPCbk);
	      defrReader::get()->getNet().defiNet::clearRectPolyNPath();
              defrReader::get()->getNet().defiNet::clearVia();
           } else
              defrReader::get()->pathIsDone(shield, 0, netOsnet, &needSNPCbk);
        }
      }
    | spaths snew_path
      { }

snew_path: K_NEW { dumb_mode = 1; } spath
      { if (defrReader::get()->needPathData() && defrReader::get()->getSNetCbk()) {
           if (needSNPCbk && defrReader::get()->getSNetPartialPathCbk()) {
              // require a callback before proceed because needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defrReader::get()->getSNetPartialPathCbk(), defrSNetPartialPathCbkType,
                       &defrReader::get()->getNet());
              needSNPCbk = 0;   // reset the flag
              defrReader::get()->pathIsDone(shield, 1, netOsnet, &needSNPCbk);
              // reset any poly or rect in special wiring statement
              defrReader::get()->getNet().defiNet::clearRectPolyNPath();
	      defrReader::get()->getNet().defiNet::clearVia();
           } else
              defrReader::get()->pathIsDone(shield, 0, netOsnet, &needSNPCbk);
        }
      }

spath:  T_STRING
      { if (defrReader::get()->needPathData() && defrReader::get()->getSNetCbk())
           defrReader::get()->getPath()->defiPath::addLayer($1);
        dumb_mode = 0; no_num = 0;
      }
    width opt_spaths path_pt
/*
      { dumb_mode = 1; new_is_keyword = TRUE; }
*/
      { dumb_mode = 1000000000; by_is_keyword = TRUE; do_is_keyword = TRUE;
        new_is_keyword = TRUE; step_is_keyword = TRUE;
         orient_is_keyword = TRUE; rect_is_keyword = TRUE, mask_is_keyword = TRUE; 
         virtual_is_keyword = TRUE;  }

    path_item_list
      { dumb_mode = 0; rect_is_keyword = FALSE, mask_is_keyword = FALSE, virtual_is_keyword = FALSE; }

width: NUMBER
      { if (defrReader::get()->needPathData() && defrReader::get()->getSNetCbk())
          defrReader::get()->getPath()->defiPath::addWidth(ROUND($1));
      }

start_snets: K_SNETS NUMBER ';'
      { 
        if (defrReader::get()->getSNetStartCbk())
          CALLBACK(defrReader::get()->getSNetStartCbk(), defrSNetStartCbkType, ROUND($2));
        netOsnet = 2;
      }

end_snets: K_END K_SNETS 
      { 
        if (defrReader::get()->getSNetEndCbk())
          CALLBACK(defrReader::get()->getSNetEndCbk(), defrSNetEndCbkType, 0);
        netOsnet = 0;
      }

groups_section: groups_start group_rules groups_end
      ;

groups_start: K_GROUPS NUMBER ';'
      {
        if (defrReader::get()->getGroupsStartCbk())
           CALLBACK(defrReader::get()->getGroupsStartCbk(), defrGroupsStartCbkType, ROUND($2));
      }

group_rules: /* empty */
      | group_rules group_rule
      ;

group_rule: start_group group_members group_options ';'
      {
        if (defrReader::get()->getGroupCbk())
           CALLBACK(defrReader::get()->getGroupCbk(), defrGroupCbkType, &defrReader::get()->getGroup());
      }

start_group: '-' { dumb_mode = 1; no_num = 1; } T_STRING 
      {
        dumb_mode = 1000000000;
        no_num = 1000000000;
        /* dumb_mode is automatically turned off at the first
         * + in the options or at the ; at the end of the group */
        if (defrReader::get()->getGroupCbk()) defrReader::get()->getGroup().defiGroup::setup($3);
        if (defrReader::get()->getGroupNameCbk())
           CALLBACK(defrReader::get()->getGroupNameCbk(), defrGroupNameCbkType, $3);
      }

group_members: 
      | group_members group_member
      {  }

group_member: T_STRING
      {
        /* if (defrReader::get()->getGroupCbk()) defrReader::get()->getGroup().defiGroup::addMember($1); */
        if (defrReader::get()->getGroupMemberCbk())
          CALLBACK(defrReader::get()->getGroupMemberCbk(), defrGroupMemberCbkType, $1);
      }

group_options: /* empty */
      | group_options group_option 
      ;

group_option: '+' K_SOFT group_soft_options
      { }
      |     '+' K_PROPERTY { dumb_mode = 10000000; parsing_property = 1; }
            group_prop_list
      { dumb_mode = 0; parsing_property = 0; }
      |     '+' K_REGION { dumb_mode = 1;  no_num = 1; } group_region
      { }
      | extension_stmt
      { 
        if (defrReader::get()->getGroupMemberCbk())
          CALLBACK(defrReader::get()->getGroupExtCbk(), defrGroupExtCbkType, History_text);
      }

group_region: pt pt
      {
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        if (defVersionNum < 5.5) {
          if (defrReader::get()->getGroupCbk())
            defrReader::get()->getGroup().defiGroup::addRegionRect($1.x, $1.y, $2.x, $2.y);
        }
        else
          defWarning(7027, "The GROUP REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
      | T_STRING
      { if (defrReader::get()->getGroupCbk())
          defrReader::get()->getGroup().defiGroup::setRegionName($1);
      }

group_prop_list : /* empty */
      | group_prop_list group_prop
      ;

group_prop : T_STRING NUMBER
      {
        if (defrReader::get()->getGroupCbk()) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defrReader::get()->getGroupProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "GROUP");
          /* For backword compatibility, also set the string value */
          /* We will use a temporary string to store the number.
           * The string space is borrowed from the ring buffer
           * in the lexer. */
          sprintf(str, "%g", $2);
          defrReader::get()->getGroup().defiGroup::addNumProperty($1, $2, str, propTp);
        }
      }
      | T_STRING QSTRING
      {
        if (defrReader::get()->getGroupCbk()) {
          char propTp;
          propTp = defrReader::get()->getGroupProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "GROUP");
          defrReader::get()->getGroup().defiGroup::addProperty($1, $2, propTp);
        }
      }
      | T_STRING T_STRING
      {
        if (defrReader::get()->getGroupCbk()) {
          char propTp;
          propTp = defrReader::get()->getGroupProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "GROUP");
          defrReader::get()->getGroup().defiGroup::addProperty($1, $2, propTp);
        }
      }

group_soft_options: /* empty */
      | group_soft_options group_soft_option 
      { }

group_soft_option: K_MAXX NUMBER
      {
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        if (defVersionNum < 5.5)
          if (defrReader::get()->getGroupCbk()) defrReader::get()->getGroup().defiGroup::setMaxX(ROUND($2));
        else
          defWarning(7028, "The GROUP SOFT MAXX statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
      | K_MAXY NUMBER
      { 
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        if (defVersionNum < 5.5)
          if (defrReader::get()->getGroupCbk()) defrReader::get()->getGroup().defiGroup::setMaxY(ROUND($2));
        else
          defWarning(7029, "The GROUP SOFT MAXY statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
      | K_MAXHALFPERIMETER NUMBER
      { 
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        if (defVersionNum < 5.5)
          if (defrReader::get()->getGroupCbk()) defrReader::get()->getGroup().defiGroup::setPerim(ROUND($2));
        else
          defWarning(7030, "The GROUP SOFT MAXHALFPERIMETER statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }

groups_end: K_END K_GROUPS 
      { 
        if (defrReader::get()->getGroupsEndCbk())
          CALLBACK(defrReader::get()->getGroupsEndCbk(), defrGroupsEndCbkType, 0);
      }

/* 8/31/2001 - This is obsolete in 5.4 */
assertions_section: assertions_start constraint_rules assertions_end
      ;

/* 8/31/2001 - This is obsolete in 5.4 */
constraint_section: constraints_start constraint_rules constraints_end
      ;

assertions_start: K_ASSERTIONS NUMBER ';'
      {
        if ((defVersionNum < 5.4) && (defrReader::get()->getAssertionsStartCbk())) {
          CALLBACK(defrReader::get()->getAssertionsStartCbk(), defrAssertionsStartCbkType,
	           ROUND($2));
        } else {
          if (defrReader::get()->getAssertionCbk())
            if (assertionWarnings++ < defrReader::get()->getAssertionWarnings())
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defrReader::get()->getAssertionCbk())
          defrReader::get()->getAssertion().defiAssertion::setAssertionMode();
      }

constraints_start: K_CONSTRAINTS NUMBER ';'
      {
        if ((defVersionNum < 5.4) && (defrReader::get()->getConstraintsStartCbk())) {
          CALLBACK(defrReader::get()->getConstraintsStartCbk(), defrConstraintsStartCbkType,
                   ROUND($2));
        } else {
          if (defrReader::get()->getConstraintCbk())
            if (constraintWarnings++ < defrReader::get()->getConstraintWarnings())
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defrReader::get()->getConstraintCbk())
          defrReader::get()->getAssertion().defiAssertion::setConstraintMode();
      }

constraint_rules: /* empty */
      | constraint_rules constraint_rule 
      ;

constraint_rule:   operand_rule 
      | wiredlogic_rule 
      {
        if ((defVersionNum < 5.4) && (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())) {
          if (defrReader::get()->getAssertion().defiAssertion::isConstraint()) 
            CALLBACK(defrReader::get()->getConstraintCbk(), defrConstraintCbkType, &defrReader::get()->getAssertion());
          if (defrReader::get()->getAssertion().defiAssertion::isAssertion()) 
            CALLBACK(defrReader::get()->getAssertionCbk(), defrAssertionCbkType, &defrReader::get()->getAssertion());
        }
      }

operand_rule: '-' operand delay_specs ';'
      { 
        if ((defVersionNum < 5.4) && (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())) {
          if (defrReader::get()->getAssertion().defiAssertion::isConstraint()) 
	    CALLBACK(defrReader::get()->getConstraintCbk(), defrConstraintCbkType, &defrReader::get()->getAssertion());
          if (defrReader::get()->getAssertion().defiAssertion::isAssertion()) 
            CALLBACK(defrReader::get()->getAssertionCbk(), defrAssertionCbkType, &defrReader::get()->getAssertion());
        }
   
        // reset all the flags and everything
        defrReader::get()->getAssertion().defiAssertion::clear();
      }

operand: K_NET { dumb_mode = 1; no_num = 1; } T_STRING 
      {
         if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
           defrReader::get()->getAssertion().defiAssertion::addNet($3);
      }
      | K_PATH {dumb_mode = 4; no_num = 4;} T_STRING T_STRING T_STRING T_STRING
      {
         if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
           defrReader::get()->getAssertion().defiAssertion::addPath($3, $4, $5, $6);
      }
      | K_SUM  '(' operand_list ')'
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
           defrReader::get()->getAssertion().defiAssertion::setSum();
      }
      | K_DIFF '(' operand_list ')'
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
           defrReader::get()->getAssertion().defiAssertion::setDiff();
      }

operand_list: operand 
      | operand_list operand
      { }
      | operand_list ',' operand

wiredlogic_rule: '-' K_WIREDLOGIC { dumb_mode = 1; no_num = 1; } T_STRING
      opt_plus K_MAXDIST NUMBER ';'
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
          defrReader::get()->getAssertion().defiAssertion::setWiredlogic($4, $7);
      }

opt_plus:
      /* empty */
      { $$ = (char*)""; }
      | '+'
      { $$ = (char*)"+"; }

delay_specs: /* empty */
      | delay_specs delay_spec
      ;

delay_spec: '+' K_RISEMIN NUMBER 
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
          defrReader::get()->getAssertion().defiAssertion::setRiseMin($3);
      }
      | '+' K_RISEMAX NUMBER 
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
          defrReader::get()->getAssertion().defiAssertion::setRiseMax($3);
      }
      | '+' K_FALLMIN NUMBER 
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
          defrReader::get()->getAssertion().defiAssertion::setFallMin($3);
      }
      | '+' K_FALLMAX NUMBER 
      {
        if (defrReader::get()->getConstraintCbk() || defrReader::get()->getAssertionCbk())
          defrReader::get()->getAssertion().defiAssertion::setFallMax($3);
      }

constraints_end: K_END K_CONSTRAINTS
      { if ((defVersionNum < 5.4) && defrReader::get()->getConstraintsEndCbk()) {
          CALLBACK(defrReader::get()->getConstraintsEndCbk(), defrConstraintsEndCbkType, 0);
        } else {
          if (defrReader::get()->getConstraintsEndCbk()) {
            if (constraintWarnings++ < defrReader::get()->getConstraintWarnings())
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }

assertions_end: K_END K_ASSERTIONS
      { if ((defVersionNum < 5.4) && defrReader::get()->getAssertionsEndCbk()) {
          CALLBACK(defrReader::get()->getAssertionsEndCbk(), defrAssertionsEndCbkType, 0);
        } else {
          if (defrReader::get()->getAssertionsEndCbk()) {
            if (assertionWarnings++ < defrReader::get()->getAssertionWarnings())
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }

scanchains_section: scanchain_start scanchain_rules scanchain_end
      ;

scanchain_start: K_SCANCHAINS NUMBER ';'
      { if (defrReader::get()->getScanchainsStartCbk())
          CALLBACK(defrReader::get()->getScanchainsStartCbk(), defrScanchainsStartCbkType,
                   ROUND($2));
      }

scanchain_rules: /* empty */
      | scanchain_rules scan_rule
      {}

scan_rule: start_scan scan_members ';' 
      { 
        if (defrReader::get()->getScanchainCbk())
          CALLBACK(defrReader::get()->getScanchainCbk(), defrScanchainCbkType, &defrReader::get()->getScanchain());
      }

start_scan: '-' {dumb_mode = 1; no_num = 1;} T_STRING 
      {
        if (defrReader::get()->getScanchainCbk())
          defrReader::get()->getScanchain().defiScanchain::setName($3);
        bit_is_keyword = TRUE;
      }

scan_members: 
      | scan_members scan_member
      ;

opt_pin :
      /* empty */
      { $$ = (char*)""; }
      | T_STRING
      { $$ = $1; }

scan_member: '+' K_START {dumb_mode = 2; no_num = 2;} T_STRING opt_pin
      { if (defrReader::get()->getScanchainCbk())
          defrReader::get()->getScanchain().defiScanchain::setStart($4, $5);
      }
      | '+' K_FLOATING { dumb_mode = 1; no_num = 1; } floating_inst_list
      { dumb_mode = 0; no_num = 0; }
      | '+' K_ORDERED
      {
         dumb_mode = 1;
         no_num = 1;
         if (defrReader::get()->getScanchainCbk())
           defrReader::get()->getScanchain().defiScanchain::addOrderedList();
      }
      ordered_inst_list
      { dumb_mode = 0; no_num = 0; }
      | '+' K_STOP {dumb_mode = 2; no_num = 2; } T_STRING opt_pin
      { if (defrReader::get()->getScanchainCbk())
          defrReader::get()->getScanchain().defiScanchain::setStop($4, $5);
      }
      | '+' K_COMMONSCANPINS { dumb_mode = 10; no_num = 10; } opt_common_pins
      { dumb_mode = 0;  no_num = 0; }
      | '+' K_PARTITION { dumb_mode = 1; no_num = 1; } T_STRING  /* 5.5 */
      partition_maxbits
      {
        if (defVersionNum < 5.5) {
          if (defrReader::get()->getScanchainCbk()) {
            if (scanchainWarnings++ < defrReader::get()->getScanchainWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The PARTITION statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
              defError(6538, defMsg);
              defFree(defMsg);
              CHKERR();
            }
          }
        }
        if (defrReader::get()->getScanchainCbk())
          defrReader::get()->getScanchain().defiScanchain::setPartition($4, $5);
      }
      | extension_stmt
      {
        if (defrReader::get()->getScanChainExtCbk())
          CALLBACK(defrReader::get()->getScanChainExtCbk(), defrScanChainExtCbkType, History_text);
      }

opt_common_pins: /* empty */
      { }
      | '(' T_STRING T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::setCommonIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::setCommonOut($3);
	}
      }
      | '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::setCommonIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::setCommonOut($3);
	  if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::setCommonIn($7);
	  else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::setCommonOut($7);
	}
      }

floating_inst_list: /* empty */
      | floating_inst_list one_floating_inst
      ;

one_floating_inst: T_STRING
      {
	dumb_mode = 1000;
	no_num = 1000;
	if (defrReader::get()->getScanchainCbk())
	  defrReader::get()->getScanchain().defiScanchain::addFloatingInst($1);
      }
      floating_pins
      { dumb_mode = 1; no_num = 1; }

floating_pins: /* empty */ 
      { }
      | '(' T_STRING  T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            bitsNum = atoi($3);
            defrReader::get()->getScanchain().defiScanchain::setFloatingBits(bitsNum);
          }
	}
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingOut($3);
	  else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            bitsNum = atoi($3);
	    defrReader::get()->getScanchain().defiScanchain::setFloatingBits(bitsNum);
          }
	  if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingIn($7);
	  else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingOut($7);
	  else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            bitsNum = atoi($7);
	    defrReader::get()->getScanchain().defiScanchain::setFloatingBits(bitsNum);
          }
	}
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')' '(' T_STRING
      T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingOut($3);
	  else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            bitsNum = atoi($3);
	    defrReader::get()->getScanchain().defiScanchain::setFloatingBits(bitsNum);
          }
	  if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingIn($7);
	  else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingOut($7);
	  else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            bitsNum = atoi($7);
	    defrReader::get()->getScanchain().defiScanchain::setFloatingBits(bitsNum);
          }
	  if (strcmp($10, "IN") == 0 || strcmp($10, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingIn($11);
	  else if (strcmp($10, "OUT") == 0 || strcmp($10, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addFloatingOut($11);
	  else if (strcmp($10, "BITS") == 0 || strcmp($10, "bits") == 0) {
            bitsNum = atoi($11);
	    defrReader::get()->getScanchain().defiScanchain::setFloatingBits(bitsNum);
          }
	}
      }
    
ordered_inst_list: /* empty */
      | ordered_inst_list one_ordered_inst
      ;

one_ordered_inst: T_STRING
      { dumb_mode = 1000; no_num = 1000; 
	if (defrReader::get()->getScanchainCbk())
	  defrReader::get()->getScanchain().defiScanchain::addOrderedInst($1);
      }
      ordered_pins
      { dumb_mode = 1; no_num = 1; }

ordered_pins: /* empty */ 
      { }
      | '(' T_STRING  T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            bitsNum = atoi($3);
            defrReader::get()->getScanchain().defiScanchain::setOrderedBits(bitsNum);
         }
	}
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedOut($3);
	  else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            bitsNum = atoi($3);
            defrReader::get()->getScanchain().defiScanchain::setOrderedBits(bitsNum);
          }
	  if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedIn($7);
	  else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedOut($7);
	  else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            bitsNum = atoi($7);
            defrReader::get()->getScanchain().defiScanchain::setOrderedBits(bitsNum);
          }
	}
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')' '(' T_STRING
      T_STRING ')'
      {
	if (defrReader::get()->getScanchainCbk()) {
	  if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedIn($3);
	  else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedOut($3);
	  else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            bitsNum = atoi($3);
	    defrReader::get()->getScanchain().defiScanchain::setOrderedBits(bitsNum);
          }
	  if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedIn($7);
	  else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedOut($7);
	  else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            bitsNum = atoi($7);
	    defrReader::get()->getScanchain().defiScanchain::setOrderedBits(bitsNum);
          }
	  if (strcmp($10, "IN") == 0 || strcmp($10, "in") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedIn($11);
	  else if (strcmp($10, "OUT") == 0 || strcmp($10, "out") == 0)
	    defrReader::get()->getScanchain().defiScanchain::addOrderedOut($11);
	  else if (strcmp($10, "BITS") == 0 || strcmp($10, "bits") == 0) {
            bitsNum = atoi($11);
	    defrReader::get()->getScanchain().defiScanchain::setOrderedBits(bitsNum);
          }
	}
      }
    
partition_maxbits: /* empty */
      { $$ = -1; }
      | K_MAXBITS NUMBER
      { $$ = ROUND($2); }
    
scanchain_end: K_END K_SCANCHAINS
      { 
        if (defrReader::get()->getScanchainsEndCbk())
          CALLBACK(defrReader::get()->getScanchainsEndCbk(), defrScanchainsEndCbkType, 0);
        bit_is_keyword = FALSE;
        dumb_mode = 0; no_num = 0;
      }

/* 8/31/2001 - This is obsolete in 5.4 */
iotiming_section: iotiming_start iotiming_rules iotiming_end
      ;

iotiming_start: K_IOTIMINGS NUMBER ';'
      {
        if (defVersionNum < 5.4 && defrReader::get()->getIOTimingsStartCbk()) {
          CALLBACK(defrReader::get()->getIOTimingsStartCbk(), defrIOTimingsStartCbkType, ROUND($2));
        } else {
          if (defrReader::get()->getIOTimingsStartCbk())
            if (iOTimingWarnings++ < defrReader::get()->getIOTimingWarnings())
              defWarning(7035, "The IOTIMINGS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
      }

iotiming_rules: /* empty */
      | iotiming_rules iotiming_rule
      { }

iotiming_rule: start_iotiming iotiming_members ';' 
      { 
        if (defVersionNum < 5.4 && defrReader::get()->getIOTimingCbk())
          CALLBACK(defrReader::get()->getIOTimingCbk(), defrIOTimingCbkType, &defrReader::get()->getIOTiming());
      } 

start_iotiming: '-' '(' {dumb_mode = 2; no_num = 2; } T_STRING T_STRING ')'
      {
        if (defrReader::get()->getIOTimingCbk())
          defrReader::get()->getIOTiming().defiIOTiming::setName($4, $5);
      }

iotiming_members: 
      | iotiming_members iotiming_member
      ;

iotiming_member:
      '+' risefall K_VARIABLE NUMBER NUMBER
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setVariable($2, $4, $5);
      }
      | '+' risefall K_SLEWRATE NUMBER NUMBER
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setSlewRate($2, $4, $5);
      }
      | '+' K_CAPACITANCE NUMBER
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setCapacitance($3);
      }
      | '+' K_DRIVECELL {dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setDriveCell($4);
      } iotiming_drivecell_opt
      /*| '+' K_FROMPIN   {dumb_mode = 1; no_num = 1; } T_STRING*/
/*
      | K_FROMPIN {dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setFrom($3);
      }
      K_TOPIN {dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setTo($7);
      }
*/
      /*| '+' K_PARALLEL NUMBER*/
/*
      |  K_PARALLEL NUMBER
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setParallel($2);
      }
*/
      | extension_stmt
      {
        if (defVersionNum < 5.4 && defrReader::get()->getIoTimingsExtCbk())
          CALLBACK(defrReader::get()->getIoTimingsExtCbk(), defrIoTimingsExtCbkType, History_text);
      }

iotiming_drivecell_opt: iotiming_frompin
      K_TOPIN {dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getIOTimingCbk()) 
          defrReader::get()->getIOTiming().defiIOTiming::setTo($4);
      }
      iotiming_parallel

iotiming_frompin: /* empty */
      | K_FROMPIN {dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getIOTimingCbk())
          defrReader::get()->getIOTiming().defiIOTiming::setFrom($3);
      }

iotiming_parallel: /* empty */
      | K_PARALLEL NUMBER
      {
        if (defrReader::get()->getIOTimingCbk())
          defrReader::get()->getIOTiming().defiIOTiming::setParallel($2);
      }

risefall: K_RISE { $$ = (char*)"RISE"; } | K_FALL { $$ = (char*)"FALL"; }

iotiming_end: K_END K_IOTIMINGS
      {
        if (defVersionNum < 5.4 && defrReader::get()->getIOTimingsEndCbk())
          CALLBACK(defrReader::get()->getIOTimingsEndCbk(), defrIOTimingsEndCbkType, 0);
      }

floorplan_contraints_section: fp_start fp_stmts K_END K_FPC
      { 
        if (defrReader::get()->getFPCEndCbk())
          CALLBACK(defrReader::get()->getFPCEndCbk(), defrFPCEndCbkType, 0);
      }

fp_start: K_FPC NUMBER ';'
      {
        if (defrReader::get()->getFPCStartCbk())
          CALLBACK(defrReader::get()->getFPCStartCbk(), defrFPCStartCbkType, ROUND($2));
      }

fp_stmts: /* empty */
      | fp_stmts fp_stmt
      {}

fp_stmt: '-' { dumb_mode = 1; no_num = 1;  } T_STRING h_or_v
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setName($3, $4); }
      constraint_type constrain_what_list ';'
      { if (defrReader::get()->getFPCCbk()) CALLBACK(defrReader::get()->getFPCCbk(), defrFPCCbkType, &defrReader::get()->getFPC()); }

h_or_v: K_HORIZONTAL 
      { $$ = (char*)"HORIZONTAL"; }
      | K_VERTICAL
      { $$ = (char*)"VERTICAL"; }

constraint_type: K_ALIGN
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setAlign(); }
      | K_MAX NUMBER
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setMax($2); }
      | K_MIN NUMBER
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setMin($2); }
      | K_EQUAL NUMBER
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setEqual($2); }

constrain_what_list: /* empty */
      | constrain_what_list constrain_what
      ;

constrain_what: '+' K_BOTTOMLEFT
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setDoingBottomLeft(); }
      row_or_comp_list 
      |       '+' K_TOPRIGHT
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::setDoingTopRight(); }
      row_or_comp_list 
      ;

row_or_comp_list: /* empty */
      | row_or_comp_list row_or_comp

row_or_comp: '(' K_ROWS  {dumb_mode = 1; no_num = 1; } T_STRING ')'
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::addRow($4); }
      |    '(' K_COMPS {dumb_mode = 1; no_num = 1; } T_STRING ')'
      { if (defrReader::get()->getFPCCbk()) defrReader::get()->getFPC().defiFPC::addComps($4); }

timingdisables_section: timingdisables_start timingdisables_rules
      timingdisables_end
      ;

timingdisables_start: K_TIMINGDISABLES NUMBER ';'
      { 
        if (defrReader::get()->getTimingDisablesStartCbk())
          CALLBACK(defrReader::get()->getTimingDisablesStartCbk(), defrTimingDisablesStartCbkType,
                   ROUND($2));
      }

timingdisables_rules: /* empty */
      | timingdisables_rules timingdisables_rule
      {}

timingdisables_rule: '-' K_FROMPIN { dumb_mode = 2; no_num = 2;  } T_STRING
      T_STRING K_TOPIN { dumb_mode = 2; no_num = 2;  } T_STRING T_STRING ';'
      {
        if (defrReader::get()->getTimingDisableCbk()) {
          defrReader::get()->getTimingDisable().defiTimingDisable::setFromTo($4, $5, $8, $9);
          CALLBACK(defrReader::get()->getTimingDisableCbk(), defrTimingDisableCbkType,
                &defrReader::get()->getTimingDisable());
        }
      }
      | '-' K_THRUPIN {dumb_mode = 2; no_num = 2; } T_STRING T_STRING ';'
      {
        if (defrReader::get()->getTimingDisableCbk()) {
          defrReader::get()->getTimingDisable().defiTimingDisable::setThru($4, $5);
          CALLBACK(defrReader::get()->getTimingDisableCbk(), defrTimingDisableCbkType,
                   &defrReader::get()->getTimingDisable());
        }
      }
      | '-' K_MACRO {dumb_mode = 1; no_num = 1;} T_STRING td_macro_option ';'
      {
        if (defrReader::get()->getTimingDisableCbk()) {
          defrReader::get()->getTimingDisable().defiTimingDisable::setMacro($4);
          CALLBACK(defrReader::get()->getTimingDisableCbk(), defrTimingDisableCbkType,
                &defrReader::get()->getTimingDisable());
        }
      }
      | '-' K_REENTRANTPATHS ';'
      { if (defrReader::get()->getTimingDisableCbk())
          defrReader::get()->getTimingDisable().defiTimingDisable::setReentrantPathsFlag();
      }


td_macro_option: K_FROMPIN {dumb_mode = 1; no_num = 1;} T_STRING K_TOPIN
      {dumb_mode=1; no_num = 1;} T_STRING
      {
        if (defrReader::get()->getTimingDisableCbk())
          defrReader::get()->getTimingDisable().defiTimingDisable::setMacroFromTo($3,$6);
      }
      |        K_THRUPIN {dumb_mode=1; no_num = 1;} T_STRING
      {
        if (defrReader::get()->getTimingDisableCbk())
          defrReader::get()->getTimingDisable().defiTimingDisable::setMacroThru($3);
      }

timingdisables_end: K_END K_TIMINGDISABLES
      { 
        if (defrReader::get()->getTimingDisablesEndCbk())
          CALLBACK(defrReader::get()->getTimingDisablesEndCbk(), defrTimingDisablesEndCbkType, 0);
      }


partitions_section: partitions_start partition_rules partitions_end
      ;

partitions_start: K_PARTITIONS NUMBER ';'
      {
        if (defrReader::get()->getPartitionsStartCbk())
          CALLBACK(defrReader::get()->getPartitionsStartCbk(), defrPartitionsStartCbkType,
                   ROUND($2));
      }

partition_rules: /* empty */
      | partition_rules partition_rule
      { }

partition_rule: start_partition partition_members ';' 
      { 
        if (defrReader::get()->getPartitionCbk())
          CALLBACK(defrReader::get()->getPartitionCbk(), defrPartitionCbkType, &defrReader::get()->getPartition());
      }

start_partition: '-' { dumb_mode = 1; no_num = 1; } T_STRING turnoff
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setName($3);
      }

turnoff: /* empty */
      | K_TURNOFF turnoff_setup turnoff_hold
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::addTurnOff($2, $3);
      }

turnoff_setup: /* empty */
      { $$ = (char*)" "; }
      | K_SETUPRISE
      { $$ = (char*)"R"; }
      | K_SETUPFALL
      { $$ = (char*)"F"; }

turnoff_hold: /* empty */
      { $$ = (char*)" "; }
      | K_HOLDRISE
      { $$ = (char*)"R"; }
      | K_HOLDFALL
      { $$ = (char*)"F"; }

partition_members: /* empty */
      | partition_members partition_member
      ;

partition_member: '+' K_FROMCLOCKPIN {dumb_mode=2; no_num = 2;}
      T_STRING T_STRING risefall minmaxpins
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setFromClockPin($4, $5);
      }
      | '+' K_FROMCOMPPIN {dumb_mode=2; no_num = 2; }
      T_STRING T_STRING risefallminmax2_list
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setFromCompPin($4, $5);
      }
      | '+' K_FROMIOPIN {dumb_mode=1; no_num = 1; } T_STRING
      risefallminmax1_list
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setFromIOPin($4);
      }
      | '+' K_TOCLOCKPIN {dumb_mode=2; no_num = 2; }
      T_STRING T_STRING risefall minmaxpins
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setToClockPin($4, $5);
      }
      | '+' K_TOCOMPPIN {dumb_mode=2; no_num = 2; }
      T_STRING T_STRING risefallminmax2_list
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setToCompPin($4, $5);
      }
      | '+' K_TOIOPIN {dumb_mode=1; no_num = 2; } T_STRING risefallminmax1_list
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setToIOPin($4);
      }
      | extension_stmt
      { 
        if (defrReader::get()->getPartitionsExtCbk())
          CALLBACK(defrReader::get()->getPartitionsExtCbk(), defrPartitionsExtCbkType,
                   History_text);
      }

minmaxpins: min_or_max_list K_PINS
      { dumb_mode = 1000000000; no_num = 10000000; } pin_list
      { dumb_mode = 0; no_num = 0; }

min_or_max_list: /* empty */
      | min_or_max_list min_or_max_member
      { }

min_or_max_member: K_MIN NUMBER NUMBER
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setMin($2, $3);
      }
      | K_MAX NUMBER NUMBER
      {
        if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::setMax($2, $3);
      }

pin_list: /* empty */
      | pin_list T_STRING
      { if (defrReader::get()->getPartitionCbk()) defrReader::get()->getPartition().defiPartition::addPin($2); }

risefallminmax1_list: /* empty */
      | risefallminmax1_list risefallminmax1

risefallminmax1: K_RISEMIN NUMBER
      { if (defrReader::get()->getPartitionCbk()) defrReader::get()->getPartition().defiPartition::addRiseMin($2); }
      | K_FALLMIN NUMBER
      { if (defrReader::get()->getPartitionCbk()) defrReader::get()->getPartition().defiPartition::addFallMin($2); }
      | K_RISEMAX NUMBER
      { if (defrReader::get()->getPartitionCbk()) defrReader::get()->getPartition().defiPartition::addRiseMax($2); }
      | K_FALLMAX NUMBER
      { if (defrReader::get()->getPartitionCbk()) defrReader::get()->getPartition().defiPartition::addFallMax($2); }

risefallminmax2_list:
      risefallminmax2
      | risefallminmax2_list risefallminmax2
      ;

risefallminmax2: K_RISEMIN NUMBER NUMBER
      { if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::addRiseMinRange($2, $3); }
      | K_FALLMIN NUMBER NUMBER
      { if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::addFallMinRange($2, $3); }
      | K_RISEMAX NUMBER NUMBER
      { if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::addRiseMaxRange($2, $3); }
      | K_FALLMAX NUMBER NUMBER
      { if (defrReader::get()->getPartitionCbk())
          defrReader::get()->getPartition().defiPartition::addFallMaxRange($2, $3); }

partitions_end: K_END K_PARTITIONS
      { if (defrReader::get()->getPartitionsEndCbk())
          CALLBACK(defrReader::get()->getPartitionsEndCbk(), defrPartitionsEndCbkType, 0); }

comp_names: /* empty */
      | comp_names comp_name
      { }

comp_name: '(' {dumb_mode=2; no_num = 2; } T_STRING
      T_STRING subnet_opt_syn ')'
      {
        /* note that the first T_STRING could be the keyword VPIN */
        if (defrReader::get()->getNetCbk())
          defrReader::get()->getSubnet()->defiSubnet::addPin($3, $4, $5);
      }

subnet_opt_syn: /* empty */
      { $$ = 0; }
      | '+' K_SYNTHESIZED
      { $$ = 1; }

subnet_options: /* empty */
      | subnet_options subnet_option

subnet_option: subnet_type
      {  
        if (defrReader::get()->getNetCbk()) defrReader::get()->getSubnet()->defiSubnet::addWire($1);
      }
      paths
      {  
        by_is_keyword = FALSE;
        do_is_keyword = FALSE;
        new_is_keyword = FALSE;
        step_is_keyword = FALSE;
        orient_is_keyword = FALSE;
        needNPCbk = 0;
      }
      | K_NONDEFAULTRULE { dumb_mode = 1; no_num = 1; } T_STRING
      { if (defrReader::get()->getNetCbk()) defrReader::get()->getSubnet()->defiSubnet::setNonDefault($3); }

subnet_type: K_FIXED
      { $$ = (char*)"FIXED"; dumb_mode = 1; }
      | K_COVER
      { $$ = (char*)"COVER"; dumb_mode = 1; }
      | K_ROUTED
      { $$ = (char*)"ROUTED"; dumb_mode = 1; }
      | K_NOSHIELD
      { $$ = (char*)"NOSHIELD"; dumb_mode = 1; }

pin_props_section: begin_pin_props pin_prop_list end_pin_props ;

begin_pin_props: K_PINPROPERTIES NUMBER opt_semi
      { if (defrReader::get()->getPinPropStartCbk())
          CALLBACK(defrReader::get()->getPinPropStartCbk(), defrPinPropStartCbkType, ROUND($2)); }

opt_semi:
      /* empty */
      { }
      | ';'
      { }

end_pin_props: K_END K_PINPROPERTIES
      { if (defrReader::get()->getPinPropEndCbk())
          CALLBACK(defrReader::get()->getPinPropEndCbk(), defrPinPropEndCbkType, 0); }

pin_prop_list: /* empty */
      | pin_prop_list pin_prop_terminal
      ;

pin_prop_terminal: '-' { dumb_mode = 2; no_num = 2; } T_STRING T_STRING
      { if (defrReader::get()->getPinPropCbk()) defrReader::get()->getPinProp().defiPinProp::setName($3, $4); }
      pin_prop_options ';'
      { if (defrReader::get()->getPinPropCbk()) {
          CALLBACK(defrReader::get()->getPinPropCbk(), defrPinPropCbkType, &defrReader::get()->getPinProp());
         // reset the property number
         defrReader::get()->getPinProp().defiPinProp::clear();
        }
      }

pin_prop_options : /* empty */
      | pin_prop_options pin_prop ;

pin_prop: '+' K_PROPERTY { dumb_mode = 10000000; parsing_property = 1; }
      pin_prop_name_value_list 
      { dumb_mode = 0; parsing_property = 0; }

pin_prop_name_value_list : /* empty */
      | pin_prop_name_value_list pin_prop_name_value
      ;

pin_prop_name_value : T_STRING NUMBER
      {
        if (defrReader::get()->getPinPropCbk()) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defrReader::get()->getCompPinProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "PINPROPERTIES");
          /* For backword compatibility, also set the string value */
          /* We will use a temporary string to store the number.
           * The string space is borrowed from the ring buffer
           * in the lexer. */
          sprintf(str, "%g", $2);
          defrReader::get()->getPinProp().defiPinProp::addNumProperty($1, $2, str, propTp);
        }
      }
 | T_STRING QSTRING
      {
        if (defrReader::get()->getPinPropCbk()) {
          char propTp;
          propTp = defrReader::get()->getCompPinProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "PINPROPERTIES");
          defrReader::get()->getPinProp().defiPinProp::addProperty($1, $2, propTp);
        }
      }
 | T_STRING T_STRING
      {
        if (defrReader::get()->getPinPropCbk()) {
          char propTp;
          propTp = defrReader::get()->getCompPinProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "PINPROPERTIES");
          defrReader::get()->getPinProp().defiPinProp::addProperty($1, $2, propTp);
        }
      }

blockage_section: blockage_start blockage_defs blockage_end ;

blockage_start: K_BLOCKAGES NUMBER ';'
      { if (defrReader::get()->getBlockageStartCbk())
          CALLBACK(defrReader::get()->getBlockageStartCbk(), defrBlockageStartCbkType, ROUND($2)); }

blockage_end: K_END K_BLOCKAGES
      { if (defrReader::get()->getBlockageEndCbk())
          CALLBACK(defrReader::get()->getBlockageEndCbk(), defrBlockageEndCbkType, 0); }

blockage_defs: /* empty */
      | blockage_defs blockage_def
      ;

blockage_def: blockage_rule rectPoly_blockage rectPoly_blockage_rules
      ';'
      {
        if (defrReader::get()->getBlockageCbk()) {
          CALLBACK(defrReader::get()->getBlockageCbk(), defrBlockageCbkType, &defrReader::get()->getBlockage());
          defrReader::get()->getBlockage().defiBlockage::clear();
        }
      }

blockage_rule: '-' K_LAYER { dumb_mode = 1; no_num = 1; } T_STRING 
      {
        if (defrReader::get()->getBlockageCbk()) {
          if (defrReader::get()->getBlockage().defiBlockage::hasPlacement() != 0) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defrReader::get()->getBlockage().defiBlockage::setLayer($4);
          defrReader::get()->getBlockage().defiBlockage::clearPoly(); // free poly, if any
        }
        hasBlkLayerComp = 0;
        hasBlkLayerSpac = 0;
        hasBlkLayerTypeComp = 0;
      }

      layer_blockage_rules
      /* 10/29/2001 - enhancement */
      | '-' K_PLACEMENT
      {
        if (defrReader::get()->getBlockageCbk()) {
          if (defrReader::get()->getBlockage().defiBlockage::hasLayer() != 0) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defrReader::get()->getBlockage().defiBlockage::setPlacement();
          defrReader::get()->getBlockage().defiBlockage::clearPoly(); // free poly, if any
        }
        hasBlkPlaceComp = 0;
        hasBlkPlaceTypeComp = 0;
      }
      placement_comp_rules
      
layer_blockage_rules: /* empty */
      | layer_blockage_rules layer_blockage_rule
      ;

layer_blockage_rule: '+' K_SPACING NUMBER
      {
        if (defVersionNum < 5.6) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
              defError(6540, defMsg);
              defFree(defMsg);
              CHKERR();
            }
          }
        } else if (hasBlkLayerSpac) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk())
            defrReader::get()->getBlockage().defiBlockage::setSpacing((int)$3);
          hasBlkLayerSpac = 1;
        }
      }
      | '+' K_DESIGNRULEWIDTH NUMBER
      {
        if (defVersionNum < 5.6) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else if (hasBlkLayerSpac) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk())
            defrReader::get()->getBlockage().defiBlockage::setDesignRuleWidth((int)$3);
          hasBlkLayerSpac = 1;
        }
      }
      | mask_blockage_rule
      | comp_blockage_rule
      
mask_blockage_rule: 
      '+' K_MASK NUMBER
      {      
        if (validateMaskInput((int)$3, blockageWarnings, defrReader::get()->getBlockageWarnings())) {
          defrReader::get()->getBlockage().defiBlockage::setMask((int)$3);
        }
      } 

comp_blockage_rule:
      /* 06/20/2001 - pcr 383204 = pcr 378102 */
      '+' K_COMPONENT { dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (hasBlkLayerComp) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk()) {
            defrReader::get()->getBlockage().defiBlockage::setComponent($4);
          }
          if (defVersionNum < 5.8) {
            hasBlkLayerComp = 1;
          }
        }
      }
      /* 8/30/2001 - pcr 394394 */
      | '+' K_SLOTS
      {
        if (hasBlkLayerComp || hasBlkLayerTypeComp) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk()) {
            defrReader::get()->getBlockage().defiBlockage::setSlots();
          }
          if (defVersionNum < 5.8) {
            hasBlkLayerComp = 1;
          }
          if (defVersionNum == 5.8) {
            hasBlkLayerTypeComp = 1;
          }
        }
      }
      | '+' K_FILLS
      {
        if (hasBlkLayerComp || hasBlkLayerTypeComp) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk()){
            defrReader::get()->getBlockage().defiBlockage::setFills();
          }
          if (defVersionNum < 5.8) {
            hasBlkLayerComp = 1;
          }
          if (defVersionNum == 5.8) {
            hasBlkLayerTypeComp = 1;
          }
        }
      }
      | '+' K_PUSHDOWN
      {
        if (hasBlkLayerComp) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk()){
            defrReader::get()->getBlockage().defiBlockage::setPushdown();
          }
          if (defVersionNum < 5.8) {
            hasBlkLayerComp = 1;
          }
        }
      }
      | '+' K_EXCEPTPGNET              /* 5.7 */
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getBlockageCbk()) {
             if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
               defMsg = (char*)defMalloc(10000);
               sprintf (defMsg,
                 "The EXCEPTPGNET is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
               defError(6549, defMsg);
               defFree(defMsg);
               CHKERR();
              }
           }
        } else {
           if (hasBlkLayerComp) {
             if (defrReader::get()->getBlockageCbk()) {
               if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
                 defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
                 CHKERR();
               }
             }
           } else {
             if (defrReader::get()->getBlockageCbk()){
               defrReader::get()->getBlockage().defiBlockage::setExceptpgnet();
             }
             if (defVersionNum < 5.8){
               hasBlkLayerComp = 1;
             }
           }
        }
      }

placement_comp_rules: /* empty */
      | placement_comp_rules placement_comp_rule
      ;
      
placement_comp_rule: /* empty */
      /* 10/29/2001 - enhancement */
      '+' K_COMPONENT { dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (hasBlkPlaceComp) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk()){
            defrReader::get()->getBlockage().defiBlockage::setComponent($4);
          }
          if (defVersionNum < 5.8) {
            hasBlkPlaceComp = 1;
          }
        }
      }
      | '+' K_PUSHDOWN
      {
        if (hasBlkPlaceComp) {
          if (defrReader::get()->getBlockageCbk()) {
            if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defrReader::get()->getBlockageCbk()){
            defrReader::get()->getBlockage().defiBlockage::setPushdown();
          }
          if (defVersionNum < 5.8) {
            hasBlkPlaceComp = 1;
          }
        }
      }
      | '+' K_SOFT                   // 5.7
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getBlockageCbk()) {
             if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
               defMsg = (char*)defMalloc(10000);
               sprintf (defMsg,
                 "The PLACEMENT SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
               defError(6547, defMsg);
               defFree(defMsg);
               CHKERR();
             }
           }
        } else {
           if (hasBlkPlaceComp || hasBlkPlaceTypeComp) {
             if (defrReader::get()->getBlockageCbk()) {
               if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defrReader::get()->getBlockageCbk()){
               defrReader::get()->getBlockage().defiBlockage::setSoft();
             }
             if (defVersionNum < 5.8) {
               hasBlkPlaceComp = 1;
             }
             if (defVersionNum == 5.8) {
               hasBlkPlaceTypeComp = 1;
             }
           }
        }
      }
      | '+' K_PARTIAL NUMBER         // 5.7
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getBlockageCbk()) {
             if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
                defMsg = (char*)defMalloc(10000);
                sprintf (defMsg,
                  "The PARTIAL is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
                defError(6548, defMsg);
                defFree(defMsg);
                CHKERR();
             }
           }
        } else {
           if (hasBlkPlaceComp || hasBlkPlaceTypeComp) {
             if (defrReader::get()->getBlockageCbk()) {
               if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defrReader::get()->getBlockageCbk()){
               defrReader::get()->getBlockage().defiBlockage::setPartial($3);
             } 
             if (defVersionNum < 5.8) {
               hasBlkPlaceComp = 1;
             }
             if (defVersionNum == 5.8) {
               hasBlkPlaceTypeComp = 1;
             }
           }
         }
      }
           

rectPoly_blockage_rules: /* empty */
      | rectPoly_blockage_rules rectPoly_blockage
      ;
  
rectPoly_blockage: K_RECT pt pt
      {
        if (defrReader::get()->getBlockageCbk())
          defrReader::get()->getBlockage().defiBlockage::addRect($2.x, $2.y, $3.x, $3.y);
      }
      | K_POLYGON
      {
        if (defrReader::get()->getBlockageCbk()) {
          if (!defrReader::get()->getGeomPtr()) {
            defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
            defrReader::get()->getGeomPtr()->defiGeometries::Init();
          } else   // Just reset the number of points
            defrReader::get()->getGeomPtr()->defiGeometries::Reset();
        }
      }
      firstPt nextPt nextPt otherPts
      {
        if (defrReader::get()->getBlockageCbk()) {
          if (defVersionNum >= 5.6) {  // only 5.6 and beyond
            if (defrReader::get()->getBlockage().defiBlockage::hasLayer()) {  // only in layer
              if (defrReader::get()->getBlockageCbk())
                defrReader::get()->getBlockage().defiBlockage::addPolygon(defrReader::get()->getGeomPtr());
            } else {
              if (defrReader::get()->getBlockageCbk()) {
                if (blockageWarnings++ < defrReader::get()->getBlockageWarnings()) {
                  defError(6544, "A POLYGON statement is defined in the BLOCKAGE statement,\nbut it is not defined in the BLOCKAGE LAYER statement.\nUpdate your DEF file to either remove the POLYGON statement from the BLOCKAGE statement or\ndefine the POLYGON statement in a BLOCKAGE LAYER statement.");
                  CHKERR();
                }
              }
            }
          }
        }
      }

/* 8/31/2001 - 5.4 enhancement */
slot_section: slot_start slot_defs slot_end ;

slot_start: K_SLOTS NUMBER ';'
      { if (defrReader::get()->getSlotStartCbk())
          CALLBACK(defrReader::get()->getSlotStartCbk(), defrSlotStartCbkType, ROUND($2)); }

slot_end: K_END K_SLOTS
      { if (defrReader::get()->getSlotEndCbk())
          CALLBACK(defrReader::get()->getSlotEndCbk(), defrSlotEndCbkType, 0); }

slot_defs: /* empty */
      | slot_defs slot_def
      ;

slot_def: slot_rule geom_slot_rules ';'
      {
        if (defrReader::get()->getSlotCbk()) {
          CALLBACK(defrReader::get()->getSlotCbk(), defrSlotCbkType, &defrReader::get()->getSlot());
          defrReader::get()->getSlot().defiSlot::clear();
        }
      }

slot_rule: '-' K_LAYER { dumb_mode = 1; no_num = 1; } T_STRING 
      {
        if (defrReader::get()->getSlotCbk()) {
          defrReader::get()->getSlot().defiSlot::setLayer($4);
          defrReader::get()->getSlot().defiSlot::clearPoly();     // free poly, if any
        }
      } geom_slot

geom_slot_rules: /* empty */
      | geom_slot_rules geom_slot
      ;

geom_slot: K_RECT pt pt
      {
        if (defrReader::get()->getSlotCbk())
          defrReader::get()->getSlot().defiSlot::addRect($2.x, $2.y, $3.x, $3.y);
      }
      | K_POLYGON
      {
        if (!defrReader::get()->getGeomPtr()) {
          defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
          defrReader::get()->getGeomPtr()->defiGeometries::Init();
        } else   // Just reset the number of points
          defrReader::get()->getGeomPtr()->defiGeometries::Reset();
      }
      firstPt nextPt nextPt otherPts
      {
        if (defVersionNum >= 5.6) {  // only 5.6 and beyond
          if (defrReader::get()->getSlotCbk())
            defrReader::get()->getSlot().defiSlot::addPolygon(defrReader::get()->getGeomPtr());
        }
      }

/* 8/31/2001 -  5.4 enhancement */
fill_section: fill_start fill_defs fill_end ;

fill_start: K_FILLS NUMBER ';'
      { if (defrReader::get()->getFillStartCbk())
          CALLBACK(defrReader::get()->getFillStartCbk(), defrFillStartCbkType, ROUND($2)); }

fill_end: K_END K_FILLS
      { if (defrReader::get()->getFillEndCbk())
          CALLBACK(defrReader::get()->getFillEndCbk(), defrFillEndCbkType, 0); }

fill_defs: /* empty */
      | fill_defs fill_def
      ;

fill_def: fill_rule geom_fill_rules ';'
      {
        if (defrReader::get()->getFillCbk()) {
          CALLBACK(defrReader::get()->getFillCbk(), defrFillCbkType, &defrReader::get()->getFill());
          defrReader::get()->getFill().defiFill::clear();
        }
      }
      | '-' K_VIA { dumb_mode = 1; no_num = 1; } T_STRING  // 5.7
      {
        if (defrReader::get()->getFillCbk()) {
          defrReader::get()->getFill().defiFill::setVia($4);
          defrReader::get()->getFill().defiFill::clearPts();
          if (!defrReader::get()->getGeomPtr()) {
            defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
            defrReader::get()->getGeomPtr()->defiGeometries::Init();
          } else   // Just reset the number of points
            defrReader::get()->getGeomPtr()->defiGeometries::Reset();
        }
      }
      fill_via_mask_opc_opt fill_via_pt ';'

fill_rule: '-' K_LAYER { dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getFillCbk()) {
          defrReader::get()->getFill().defiFill::setLayer($4);
          defrReader::get()->getFill().defiFill::clearPoly();    // free poly, if any
        }
      } 
      fill_layer_mask_opc_opt geom_fill            // 5.7

geom_fill_rules: /* empty */
      | geom_fill_rules geom_fill
      ;

geom_fill: K_RECT pt pt
      {
        if (defrReader::get()->getFillCbk())
          defrReader::get()->getFill().defiFill::addRect($2.x, $2.y, $3.x, $3.y);
      }
      | K_POLYGON
      {
        if (!defrReader::get()->getGeomPtr()) {
          defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
          defrReader::get()->getGeomPtr()->defiGeometries::Init();
        } else   // Just reset the number of points
          defrReader::get()->getGeomPtr()->defiGeometries::Reset();
      }
      firstPt nextPt nextPt otherPts
      {
        if (defVersionNum >= 5.6) {  // only 5.6 and beyond
          if (defrReader::get()->getFillCbk())
            defrReader::get()->getFill().defiFill::addPolygon(defrReader::get()->getGeomPtr());
        } else {
            defMsg = (char*)defMalloc(10000);
            sprintf (defMsg,
              "POLYGON statement in FILLS LAYER is a version 5.6 and later syntax.\nYour def file is defined with version %g.", defVersionNum);
            defError(6564, defMsg);
            defFree(defMsg);
            CHKERR();
        }
      }

fill_layer_mask_opc_opt: /* empty */
    | fill_layer_mask_opc_opt opt_mask_opc_l
    ;
opt_mask_opc_l: fill_layer_opc
    | fill_mask
    ;
        
// 5.7
fill_layer_opc: 
      '+' K_OPC
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getFillCbk()) {
             if (fillWarnings++ < defrReader::get()->getFillWarnings()) {
               defMsg = (char*)defMalloc(10000);
               sprintf (defMsg,
                 "The LAYER OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
               defError(6553, defMsg);
               defFree(defMsg);
               CHKERR();
             }
           }
        } else {
           if (defrReader::get()->getFillCbk())
             defrReader::get()->getFill().defiFill::setLayerOpc();
        }
      }

fill_via_pt: firstPt otherPts
    {
        if (defrReader::get()->getFillCbk()) {
          defrReader::get()->getFill().defiFill::addPts(defrReader::get()->getGeomPtr());
          CALLBACK(defrReader::get()->getFillCbk(), defrFillCbkType, &defrReader::get()->getFill());
          defrReader::get()->getFill().defiFill::clear();
        }
    }
      

fill_via_mask_opc_opt: /* empty */
    | fill_via_mask_opc_opt opt_mask_opc
    ;
opt_mask_opc: fill_via_opc
    | fill_viaMask
    ;
    
// 5.7
fill_via_opc:
      '+' K_OPC
      {
        if (defVersionNum < 5.7) {
           if (defrReader::get()->getFillCbk()) {
             if (fillWarnings++ < defrReader::get()->getFillWarnings()) {
               defMsg = (char*)defMalloc(10000);
               sprintf (defMsg,
                 "The VIA OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
               defError(6554, defMsg);
               defFree(defMsg);
               CHKERR();
             }
           }
        } else {
           if (defrReader::get()->getFillCbk())
             defrReader::get()->getFill().defiFill::setViaOpc();
        }
      }
      
fill_mask:
      '+' K_MASK NUMBER
      { 
        if (validateMaskInput((int)$3, fillWarnings, defrReader::get()->getFillWarnings())) {
             if (defrReader::get()->getFillCbk()) {
                defrReader::get()->getFill().defiFill::setMask((int)$3);
             }
        }
      }

fill_viaMask:
      '+' K_MASK NUMBER
      { 
        if (validateMaskInput((int)$3, fillWarnings, defrReader::get()->getFillWarnings())) {
             if (defrReader::get()->getFillCbk()) {
                defrReader::get()->getFill().defiFill::setMask((int)$3);
             }
        }
      }
      
/* 11/17/2003 - 5.6 enhancement */
nondefaultrule_section: nondefault_start nondefault_def nondefault_defs
    nondefault_end ;

nondefault_start: K_NONDEFAULTRULES NUMBER ';'
      { 
        if (defVersionNum < 5.6) {
          if (defrReader::get()->getNonDefaultStartCbk()) {
            if (nonDefaultWarnings++ < defrReader::get()->getNonDefaultWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defVersionNum);
              defError(6545, defMsg);
              defFree(defMsg);
              CHKERR();
            }
          }
        } else if (defrReader::get()->getNonDefaultStartCbk())
          CALLBACK(defrReader::get()->getNonDefaultStartCbk(), defrNonDefaultStartCbkType,
                   ROUND($2));
      }

nondefault_end: K_END K_NONDEFAULTRULES
      { if (defrReader::get()->getNonDefaultEndCbk())
          CALLBACK(defrReader::get()->getNonDefaultEndCbk(), defrNonDefaultEndCbkType, 0); }

nondefault_defs: /* empty */
      | nondefault_defs nondefault_def
      ;

nondefault_def: '-' { dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::clear(); 
          defrReader::get()->getNonDefault().defiNonDefault::setName($3);
        }
      }
      nondefault_options ';'
      { if (defrReader::get()->getNonDefaultCbk())
          CALLBACK(defrReader::get()->getNonDefaultCbk(), defrNonDefaultCbkType, &defrReader::get()->getNonDefault()); }

nondefault_options: /* empty */ 
      | nondefault_options nondefault_option
      ;

nondefault_option: '+' K_HARDSPACING
      {
        if (defrReader::get()->getNonDefaultCbk())
          defrReader::get()->getNonDefault().defiNonDefault::setHardspacing();
      }
      | '+' K_LAYER { dumb_mode = 1; no_num = 1; } T_STRING
        K_WIDTH NUMBER
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addLayer($4);
          defrReader::get()->getNonDefault().defiNonDefault::addWidth($6);
        }
      }
      nondefault_layer_options
      | '+' K_VIA { dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addVia($4);
        }
      }
      | '+' K_VIARULE { dumb_mode = 1; no_num = 1; } T_STRING
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addViaRule($4);
        }
      }
      | '+' K_MINCUTS { dumb_mode = 1; no_num = 1; } T_STRING NUMBER
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addMinCuts($4, (int)$5);
        }
      }
      | nondefault_prop_opt
      ;

nondefault_layer_options: /* empty */
      | nondefault_layer_options nondefault_layer_option

nondefault_layer_option:
      K_DIAGWIDTH NUMBER
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addDiagWidth($2);
        }
      }
      | K_SPACING NUMBER
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addSpacing($2);
        }
      }
      | K_WIREEXT NUMBER
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          defrReader::get()->getNonDefault().defiNonDefault::addWireExt($2);
        }
      }
      ;

nondefault_prop_opt: '+' K_PROPERTY { dumb_mode = 10000; parsing_property = 1; }
                     nondefault_prop_list
      { dumb_mode = 0; parsing_property = 0; }

nondefault_prop_list: /* empty */
      | nondefault_prop_list nondefault_prop
      ;

nondefault_prop: T_STRING NUMBER
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defrReader::get()->getNDefProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "NONDEFAULTRULE");
          sprintf(str, "%g", $2);
          defrReader::get()->getNonDefault().defiNonDefault::addNumProperty($1, $2, str, propTp);
        }
      }
      | T_STRING QSTRING
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          char propTp;
          propTp = defrReader::get()->getNDefProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "NONDEFAULTRULE");
          defrReader::get()->getNonDefault().defiNonDefault::addProperty($1, $2, propTp);
        }
      }
      | T_STRING T_STRING
      {
        if (defrReader::get()->getNonDefaultCbk()) {
          char propTp;
          propTp = defrReader::get()->getNDefProp().defiPropType::propType($1);
          CHKPROPTYPE(propTp, $1, "NONDEFAULTRULE");
          defrReader::get()->getNonDefault().defiNonDefault::addProperty($1, $2, propTp);
        }
      }

/* 12/2/2003 - 5.6 enhancement */
styles_section: styles_start styles_rules styles_end ;

styles_start: K_STYLES NUMBER ';'
      {
        if (defVersionNum < 5.6) {
          if (defrReader::get()->getStylesStartCbk()) {
            if (stylesWarnings++ < defrReader::get()->getStylesWarnings()) {
              defMsg = (char*)defMalloc(1000);
              sprintf (defMsg,
                 "The STYLES statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defVersionNum);
              defError(6546, defMsg);
              defFree(defMsg);
              CHKERR();
            }
          }
        } else if (defrReader::get()->getStylesStartCbk())
          CALLBACK(defrReader::get()->getStylesStartCbk(), defrStylesStartCbkType, ROUND($2));
      }

styles_end: K_END K_STYLES
      { if (defrReader::get()->getStylesEndCbk())
          CALLBACK(defrReader::get()->getStylesEndCbk(), defrStylesEndCbkType, 0); }

styles_rules: /* empty */
      | styles_rules styles_rule
      ;

styles_rule: '-' K_STYLE NUMBER
      {
        if (defrReader::get()->getStylesCbk()) defrReader::get()->getStyles().defiStyles::setStyle((int)$3);
        if (!defrReader::get()->getGeomPtr()) {
          defrReader::get()->setGeomPtr((defiGeometries*)defMalloc(sizeof(defiGeometries)));
          defrReader::get()->getGeomPtr()->defiGeometries::Init();
        } else   // Just reset the number of points
          defrReader::get()->getGeomPtr()->defiGeometries::Reset();
      }
      firstPt nextPt otherPts ';'
      {
        if (defVersionNum >= 5.6) {  // only 5.6 and beyond will call the callback
          if (defrReader::get()->getStylesCbk()) {
            defrReader::get()->getStyles().defiStyles::setPolygon(defrReader::get()->getGeomPtr());
            CALLBACK(defrReader::get()->getStylesCbk(), defrStylesCbkType, &defrReader::get()->getStyles());
          }
        }
      }
      

%%
