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


#include "lefrReader.hpp"
#include "lex.h"
#include <stdlib.h>
#include <string.h>
#include "lefiDebug.hpp"
#ifdef WIN32
//# include "lefMsgTable.hpp"
#endif

#define CURRENT_VERSION 5.7

#define NOCBK 100

#define NOLEFMSG 4701 // 4701 = 4700 + 1, message starts on 1

extern int lefyyparse(void);  // do the parsing
extern void lef_lex_init(void);      // start the lexer
extern void lef_lex_un_init(void);   // free up lexer space
extern int lef_nlines;               // lexer line number

// TX_DIR:TRANSLATION ON

static char lefrErrMsg[1024];

extern void lefrNerr( int num );

extern void lefrNwarn( int num );

extern int lefrRelaxMode;

extern double convert_name2num(char *versionName);

extern bool validateMaskNumber(int num);

int lefiDeltaNumberLines = 10000;

int logFileAppend = 0;

int lefDisPropStrProcess = 0;

int  nDMsgs = 0;
/*
int* disableMsgs = 0;
*/
int* disableMsgs[2] = {0};  /* 0 - disable msg numbers, 1 - warning printed */

int  dAllMsgs = 0;

int totalMsgLimit = 0;    /* to save the user set total msg limit to output */
int lefInfoMsgPrinted = 0;
int lefErrMsgPrinted = 0;
int lefWarnMsgPrinted = 0;

/* to hold the msg limit, 0 - num of limit */
/* 1 - num of message printed, 4701 = 4700 + 1, message starts on 1 */
/* 2 - warning printed */
int lefMsgLimit[3][4701] = {0};

void lefiNerr(int i) {
  sprintf(lefrErrMsg,"ERROR number %d\n", i);
  lefiError(1, 0, lefrErrMsg);
  exit(2);
}


void lefiNwarn(int i) {
  sprintf(lefrErrMsg,"WARNING number %d\n", i);
  lefiError(1, 0, lefrErrMsg);
  exit(2);
}

double convert_name2num(char *versionName)
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *p1;
    char *versionNm = strdup(versionName);

    double major = 0, minor = 0, subMinor = 0;
    double version, versionNumber;
    char finalVersion[80];

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

    lefFree(versionNm);

    sprintf(finalVersion, "%.4f", version);

    versionNumber = atof(finalVersion);

    return versionNumber;
}

bool validateMaskNumber(int num) {
    int digit = 0;
    int index = 0;

    if (num < 0 ) {
        return false;
    }

    while (num > 0) {
        digit = num % 10;

        if (digit > 3) {
            return false;
        }

        index++;
        num = num/10;
    }

    if (index > 3) {
        return false;
    }

    return true;
}

///////////////////////////////////s
//
//   Global variables
//
/////////////////////////////////////

lefiUserData lefrUserData = 0;
char* lefrFileName = 0;
static int lefrRegisterUnused = 0;
FILE* lefrFile = 0;
lefiAntennaPWL* lefrAntennaPWLPtr = 0;
lefiArray lefrArray;
lefiCorrectionTable lefrCorrectionTable;
lefiDensity lefrDensity;
lefiGcellPattern* lefrGcellPatternPtr = 0;
lefiGeometries* lefrGeometriesPtr = 0;
lefiIRDrop lefrIRDrop;
lefiLayer lefrLayer;
lefiMacro lefrMacro;
lefiMaxStackVia lefrMaxStackVia;  // 5.5
lefiMinFeature lefrMinFeature;
struct lefiNoiseMargin lefrNoiseMargin;
lefiNoiseTable lefrNoiseTable;
lefiNonDefault lefrNonDefault;
lefiObstruction lefrObstruction;
lefiPin lefrPin;
lefiProp lefrProp;
lefiSite lefrSite;
lefiSitePattern* lefrSitePatternPtr = 0;
lefiSpacing lefrSpacing;
lefiTiming lefrTiming;
lefiTrackPattern* lefrTrackPatternPtr = 0;
lefiUnits lefrUnits;
lefiUseMinSpacing lefrUseMinSpacing;
lefiVia lefrVia;
lefiViaRule lefrViaRule;
int lefrIsReset = 0;
int lefrDoGcell = 0;
int lefrDoGeometries = 0;
int lefrDoTrack = 0;
int lefrDoSite = 0;
int lefrHasLayer = 0;    // 5.5 this & lefrHasMaxVS is to keep track that
int lefrHasMaxVS = 0;    // MAXVIASTACK has to be after all layers
int lefrHasSpacingTbl;   // keep track of spacing table in a layer
int lefrHasSpacing;      // keep track of spacing in a layer
int userHasSetVer = 0;   // if user has set default version value
int doneLib = 1;         // keep track if the library is done parsing
                         // 5.6 END LIBRARY is optional.
int ge56almostDone = 0;  // have reached the EXTENSION SECTION
int ge56done = 0;        // a 5.6 and it has END LIBRARY statement

// The following global variables are for storing the propertydefination
// types.  Only real & integer need to store since the parser can
// distinguish string and quote string
lefiPropType lefrLibProp;
lefiPropType lefrCompProp;
lefiPropType lefrPinProp;
lefiPropType lefrMacroProp;
lefiPropType lefrViaProp;
lefiPropType lefrViaRuleProp;
lefiPropType lefrLayerProp;
lefiPropType lefrNondefProp;

// The following global variables are used in lef.y
int use5_3 = 0;          /* keep track if there is any 5.3 on 5.4 file */
int use5_4 = 0;          /* keep track if only 5.4 is allowed */
int hasVer = 0;          /* keep track if version is in file */
int hasNameCase = 0;     /* keep track if namescasesensitive is in file */
int hasBusBit = 0;       /* keep track if bustbitchars is in file */
int hasDivChar = 0;      /* keep track if dividerchar is in file */
int aOxide = 0;          /* keep track for oxide */
double versionNum = CURRENT_VERSION;

/* NEW CALLBACK - If you create a new global instance of a class,
 * then place it here. */


////////////////////////////////////
//
//       List of call back routines
//  These are filled in by the user.  See the
//   "set" routines at the end of the file
//
/////////////////////////////////////
lefrUnitsCbkFnType lefrUnitsCbk = 0;
lefrIntegerCbkFnType lefrCaseSensitiveCbk = 0;
lefrStringCbkFnType lefrNoWireExtensionCbk = 0;
lefrStringCbkFnType lefrDividerCharCbk = 0;
lefrStringCbkFnType lefrBusBitCharsCbk = 0;
lefrDoubleCbkFnType lefrVersionCbk = 0;
lefrStringCbkFnType lefrVersionStrCbk = 0;
lefrLayerCbkFnType lefrLayerCbk = 0;
lefrViaCbkFnType lefrViaCbk = 0;
lefrViaRuleCbkFnType lefrViaRuleCbk = 0;
lefrSpacingCbkFnType lefrSpacingCbk = 0;
lefrIRDropCbkFnType lefrIRDropCbk = 0;
lefrDoubleCbkFnType lefrDielectricCbk = 0;
lefrMinFeatureCbkFnType lefrMinFeatureCbk = 0;
lefrNonDefaultCbkFnType lefrNonDefaultCbk = 0;
lefrSiteCbkFnType lefrSiteCbk = 0;
lefrStringCbkFnType lefrMacroBeginCbk = 0;
lefrPinCbkFnType lefrPinCbk = 0;
lefrObstructionCbkFnType lefrObstructionCbk = 0;
lefrArrayCbkFnType lefrArrayCbk = 0;
lefrMacroCbkFnType lefrMacroCbk = 0;
lefrVoidCbkFnType lefrLibraryEndCbk = 0;
lefrVoidCbkFnType lefrPropBeginCbk = 0;
lefrVoidCbkFnType lefrPropEndCbk = 0;
lefrPropCbkFnType lefrPropCbk = 0;

/* NEW CALLBACK - Add a line here for each new callback routine. */

lefrVoidCbkFnType lefrSpacingBeginCbk = 0;
lefrVoidCbkFnType lefrSpacingEndCbk = 0;
lefrStringCbkFnType lefrArrayBeginCbk = 0;
lefrStringCbkFnType lefrArrayEndCbk = 0;
lefrTimingCbkFnType lefrTimingCbk = 0;
lefrVoidCbkFnType lefrIRDropBeginCbk = 0;
lefrVoidCbkFnType lefrIRDropEndCbk = 0;
lefrNoiseMarginCbkFnType lefrNoiseMarginCbk = 0;
lefrDoubleCbkFnType lefrEdgeRateThreshold1Cbk = 0;
lefrDoubleCbkFnType lefrEdgeRateThreshold2Cbk = 0;
lefrDoubleCbkFnType lefrEdgeRateScaleFactorCbk = 0;
lefrNoiseTableCbkFnType lefrNoiseTableCbk = 0;
lefrCorrectionTableCbkFnType lefrCorrectionTableCbk = 0;
lefrDoubleCbkFnType lefrInputAntennaCbk = 0;
lefrDoubleCbkFnType lefrOutputAntennaCbk = 0;
lefrDoubleCbkFnType lefrInoutAntennaCbk = 0;
lefrDoubleCbkFnType lefrAntennaInputCbk = 0;
lefrDoubleCbkFnType lefrAntennaInoutCbk = 0;
lefrDoubleCbkFnType lefrAntennaOutputCbk = 0;
lefrDoubleCbkFnType lefrManufacturingCbk = 0;
lefrUseMinSpacingCbkFnType lefrUseMinSpacingCbk = 0;
lefrStringCbkFnType lefrClearanceMeasureCbk = 0;
lefrStringCbkFnType lefrMacroClassTypeCbk = 0;
lefrMacroNumCbkFnType lefrMacroOriginCbk = 0;
lefrMacroNumCbkFnType lefrMacroSizeCbk = 0;
lefrIntegerCbkFnType lefrMacroFixedMaskCbk = 0;
lefrStringCbkFnType lefrMacroEndCbk = 0;
lefrMaxStackViaCbkFnType lefrMaxStackViaCbk = 0;
lefrStringCbkFnType lefrExtensionCbk = 0;
lefrDensityCbkFnType lefrDensityCbk = 0;
lefrIntegerCbkFnType lefrFixedMaskCbk = 0;


////////////////////////////////////
//
//  Flags to control number of warnings to print out, max will be 999
//
////////////////////////////////////

int lefrAntennaInoutWarnings = 999;
int lefrAntennaInputWarnings = 999;
int lefrAntennaOutputWarnings = 999;
int lefrArrayWarnings = 999;
int lefrCaseSensitiveWarnings = 999;
int lefrCorrectionTableWarnings = 999;
int lefrDielectricWarnings = 999;
int lefrEdgeRateThreshold1Warnings = 999;
int lefrEdgeRateThreshold2Warnings = 999;
int lefrEdgeRateScaleFactorWarnings = 999;
int lefrInoutAntennaWarnings = 999;
int lefrInputAntennaWarnings = 999;
int lefrIRDropWarnings = 999;
int lefrLayerWarnings = 999;
int lefrMacroWarnings = 999;
int lefrMaxStackViaWarnings = 999;
int lefrMinFeatureWarnings = 999;
int lefrNoiseMarginWarnings = 999;
int lefrNoiseTableWarnings = 999;
int lefrNonDefaultWarnings = 999;
int lefrNoWireExtensionWarnings = 999;
int lefrOutputAntennaWarnings = 999;
int lefrPinWarnings = 999;
int lefrSiteWarnings = 999;
int lefrSpacingWarnings = 999;
int lefrTimingWarnings = 999;
int lefrUnitsWarnings = 999;
int lefrUseMinSpacingWarnings = 999;
int lefrViaRuleWarnings = 999;
int lefrViaWarnings = 999;

// Function to initialize global variables.
// This make sure the global variables are initialized

void lefrInitGlobals() {

  lefrUserData = 0;
  lefrIsReset = 0;

  return;
}

////////////////////////////////////
//
//  User control warning to be printed by the parser
//
////////////////////////////////////

void lefrDisableParserMsgs (int nMsg, int* msgs) {
  int i, j;
  int *tmp[2];

  if (nMsg <= 0)
     return;

  if (nDMsgs == 0) {  /* new list */
     nDMsgs = nMsg;
     /* 0 - holds the message number, 1 keep track if warning has printed */
     disableMsgs[0] = (int*)lefMalloc(sizeof(int)*nMsg);
     disableMsgs[1] = (int*)lefMalloc(sizeof(int)*nMsg);
     for (i = 0; i < nMsg; i++) {
        disableMsgs[0][i] = msgs[i];
        disableMsgs[1][i] = 0;
     }
  } else {  /* add the list to the existing list */
     /* 1st check if the msgId is already on the list before adding it on */
     tmp[0] = (int*)lefMalloc(sizeof(int)*(nMsg + nDMsgs));
     tmp[1] = (int*)lefMalloc(sizeof(int)*(nMsg + nDMsgs));
     for (i = 0; i < nDMsgs; i++) {  /* copy the existing to the new list */
        tmp[0][i] = disableMsgs[0][i]; 
        tmp[1][i] = disableMsgs[1][i]; 
     }
     lefFree((int*)(disableMsgs[0]));
     lefFree((int*)(disableMsgs[1]));
     disableMsgs[0] = tmp[0];           /* set disableMsgs to the new list */
     disableMsgs[1] = tmp[1];
     for (i = 0; i < nMsg; i++) { /* merge the new list with the existing */
        for (j = 0; j < nDMsgs; j++) {
           if (disableMsgs[0][j] == msgs[i])
               break;             /* msgId already on the list */
        } 
        if (j == nDMsgs) {         /* msgId not on the list, add it on */
           disableMsgs[0][nDMsgs] = msgs[i]; 
           disableMsgs[1][nDMsgs] = 0; 
           nDMsgs++;
        }
     } 
  }
  return;
}

void lefrEnableParserMsgs (int nMsg, int* msgs) {
  int i, j;
  int *tmp[2];
  
  if (nDMsgs == 0)
     return;                       /* list is empty, nothing to remove */

  for (i = 0; i < nMsg; i++) {     /* loop through the given list */
     for (j = 0; j < nDMsgs; j++) {
        if (disableMsgs[0][j] == msgs[i]) {
           disableMsgs[0][j] = -1;    /* temp assign a -1 on that slot */ 
           break;
        }
     }
  } 
  /* fill up the empty slot with the next non -1 msgId */
  for (i = 0; i < nDMsgs; i++) { 
     if (disableMsgs[0][i] == -1) {
        j = i + 1;
        while (j < nDMsgs) {
           if (disableMsgs[0][j] != -1) {
              disableMsgs[0][i] = disableMsgs[0][j];
              disableMsgs[1][i] = disableMsgs[1][j];
              i++;
              j++;
           }
        }
        break;     /* break out the for loop, the list should all moved */
     }  
  }
  /* Count how many messageId left and change all -1 to 0 */
  for (j = i; j < nDMsgs; j++) {
        disableMsgs[0][j] = 0;     /* set to 0 */
        disableMsgs[1][j] = 0;
  }
  nDMsgs = i;
  return;
}

void lefrEnableAllMsgs () {
  nDMsgs = 0;
  lefFree((int*)(disableMsgs[0]));
  lefFree((int*)(disableMsgs[1]));
  dAllMsgs = 0;
  return;
}

void lefrSetTotalMsgLimit (int totNumMsgs) {
  totalMsgLimit = totNumMsgs;
  return;
}

void lefrSetLimitPerMsg (int msgId, int numMsg) {
  char msgStr[10];
  if ((msgId <= 0) || (msgId >= NOLEFMSG)) {
     sprintf(msgStr, "%d", msgId);
     lefError(204, msgStr);
     return;
  }
  lefMsgLimit[0][msgId] = numMsg;
  return;
}

////////////////////////////////////
//
// Since the lef parser only keep one list of disable message ids, and does
// not have a list of enable message ids, if the API lefrDisableAllMsgs is
// called to disable all message ids, user has to call API lefrEnableAllMsgs
// to enable all message ids first, before calling lefrDisableParserMsgs &
// lefrEnableParserMsgs.
// Users cannot call lefrDisableAllMsgs and call lefrEnableParserMsgs to
// enable a small list of message ids since lefrDisableAllMsgs does not have
// a list of all message ids, hence there isn't a list for lefrEnableParserMsgs
// to work on to enable the message ids.
//
////////////////////////////////////

void lefrDisableAllMsgs() {
  nDMsgs = 0;
  lefFree((int*)(disableMsgs[0]));
  lefFree((int*)(disableMsgs[1]));
  dAllMsgs = 1;
}
////////////////////////////////////
//
//   Parser control by the user.
//
/////////////////////////////////////


// Reader initialization
int lefrInit() {

  /* Check if the date is valid */
  /*
  if (!lefiValidTime()) 
      exit(77);
  */

  // 05/01/2002 - Wanda da Rosa
  // PCR 438006 - Initialize global variables in a single routine
  lefrInitGlobals();

  /* Check if the user is a valid one */
  /* Don't need anymore
  if (!lefiValidUser()) {
      printf("This LEF Reader license is valid for %s to use only!\n", 
             lefiUser());
      exit(77);
  }
  */

  /* Normally in C++ the static initializers will take care of */
  /* these calls.  In ANSI C we must do it ourselves. */
  lefrReset();

// 11/25/2002 - bug fix: submitted by Craig Files (cfiles@ftc.agilent.com)
// On HPUX machines, the global variables do not get their constructors called
// if they are not a basic type (int, char, etc).  This is a HPUX linker
// problem.  Thus, HPUX needs to be added in the ifndef statements and
// more Init() are needed.
// Need to comment out since it won't compile with the addition HPUX

  /* 10/25/04 - Moved from lefrReset to Init. */
  /* The Init should only be called once, during initialization. */
  /* Otherwise it will cause memory leaks */
#ifndef __cplusplus
  lefrUnits.lefiUnits::Init();
  lefrLayer.lefiLayer::Init();
  lefrVia.lefiVia::Init();
  lefrViaRule.lefiViaRule::Init();
  lefrSpacing.lefiSpacing::Init();
  lefrIRDrop.lefiIRDrop::Init();
  lefrProp.lefiProp::Init();
  lefrTiming.lefiTiming::Init();
  lefrNoiseTable.lefiNoiseTable::Init();
  lefrCorrectionTable.lefiCorrectionTable::Init();
  /* NEW CALLBACK - If you create a new global instance of a class,
   * then initialize it here. (needed in the "C" version" */
  lefrUseMinSpacing.lefiUseMinSpacing::Init();
  lefrMaxStackVia.lefiMaxStackVia::Init();
  lefrNonDefault.lefiNonDefault::Init();      // bug fix: submitted by
  lefrMacro.lefiMacro::Init();                // Craig Files
  lefrPin.lefiPin::Init();                    // (cfiles@ftc.agilent.com)
  lefrDensity.lefiDensity::Init();
  lefrObstruction.lefiObstruction::Init();
  lefrSite.lefiSite::Init();
#endif
  return 0;
}


int lefrReset() {
  lefrIsReset = 1;

  use5_3 = 0;
  use5_4 = 0;
  hasVer = 0;
  hasNameCase = 0;
  hasBusBit = 0;
  hasDivChar = 0;
  aOxide = 0;
  doneLib = 1;
  ge56almostDone = 0;
  ge56done = 0;
  if (!userHasSetVer)  // user hasn't set any default version value
     versionNum = CURRENT_VERSION;

  lefrFileName = 0;
  lefrFile = 0;
  lefrSitePatternPtr = 0;
  lefrDoSite = 0;
  lefrGeometriesPtr = 0;
  lefrDoGeometries = 0;
  lefrTrackPatternPtr = 0;
  lefrDoTrack = 0;
  lefrGcellPatternPtr = 0;
  lefrDoGcell = 0;
  lefrAntennaPWLPtr = 0;
  lefrHasLayer = 0;
  lefrHasMaxVS = 0;
  lefrHasSpacingTbl = 0;
  lefrHasSpacing = 0;

  return 0;
}


const char* lefrFName() {
  return lefrFileName;
}

int lefrReleaseNResetMemory() {
  lefrCorrectionTable.lefiCorrectionTable::Destroy();
  lefrDensity.lefiDensity::Destroy();
  lefrIRDrop.lefiIRDrop::Destroy();
  lefrLayer.lefiLayer::Destroy();
  lefrMacro.lefiMacro::Destroy();
  lefrMaxStackVia.lefiMaxStackVia::Destroy(); // Don't need to call Init. Same
  lefrNoiseTable.lefiNoiseTable::Destroy();   // as Destroy
  lefrNonDefault.lefiNonDefault::Destroy();
  lefrPin.lefiPin::Destroy();
  lefrProp.lefiProp::Destroy();
  lefrSpacing.lefiSpacing::Destroy();
  lefrTiming.lefiTiming::Destroy();
  lefrUnits.lefiUnits::Destroy();
  lefrObstruction.lefiObstruction::Destroy();
  lefrSite.lefiSite::Destroy();
  lefrUseMinSpacing.lefiUseMinSpacing::Destroy();
  lefrVia.lefiVia::Destroy();
  lefrViaRule.lefiViaRule::Destroy();

  lefrCorrectionTable.lefiCorrectionTable::Init();
  lefrDensity.lefiDensity::Init();
  lefrIRDrop.lefiIRDrop::Init();
  lefrLayer.lefiLayer::Init();
  lefrMacro.lefiMacro::Init();
  lefrNoiseTable.lefiNoiseTable::Init();
  lefrNonDefault.lefiNonDefault::Init();
  lefrPin.lefiPin::Init();
  lefrProp.lefiProp::Init();
  lefrSpacing.lefiSpacing::Init();
  lefrTiming.lefiTiming::Init();
  lefrUnits.lefiUnits::Init();
  lefrObstruction.lefiObstruction::Init();
  lefrSite.lefiSite::Init();
  lefrUseMinSpacing.lefiUseMinSpacing::Init();
  lefrVia.lefiVia::Init();
  lefrViaRule.lefiViaRule::Init();

  return 0;
}


int lefrRead(FILE* f, const char* fName, lefiUserData uData) {
  int status;

  if (lefrIsReset == 0) {
    fprintf(stderr, "ERROR (LEFPARS-100): lefrRead called before lefrInit\n");
    return -1;
  }

  lefrFileName = (char*)fName;
  lefrFile = f;
  lefrUserData = uData;

  /* Setup the lexer */
  lef_lex_init();

  /* Parse the file. */
  /* If there are any callbacks they will happen here. */
  status = lefyyparse();

  // Clean up the vars.
  lef_lex_un_init();

  lefrIsReset = 0;
  return status;
}


void lefrSetUnusedCallbacks(lefrVoidCbkFnType func) {
  // Set all of the callbacks that have not been set yet to
  // the given function.

  if (lefrArrayBeginCbk == 0) lefrArrayBeginCbk = (lefrStringCbkFnType)func;
  if (lefrArrayCbk == 0) lefrArrayCbk = (lefrArrayCbkFnType)func;
  if (lefrArrayEndCbk == 0) lefrArrayEndCbk = (lefrStringCbkFnType)func;
  if (lefrDividerCharCbk == 0) lefrDividerCharCbk = (lefrStringCbkFnType)func;
  if (lefrBusBitCharsCbk == 0) lefrBusBitCharsCbk = (lefrStringCbkFnType)func;
  if (lefrCaseSensitiveCbk == 0) lefrCaseSensitiveCbk = (lefrIntegerCbkFnType)func;
  if (lefrNoWireExtensionCbk == 0) lefrNoWireExtensionCbk = (lefrStringCbkFnType)func;
  if (lefrCorrectionTableCbk == 0) lefrCorrectionTableCbk = (lefrCorrectionTableCbkFnType)func;
  if (lefrDielectricCbk == 0) lefrDielectricCbk = (lefrDoubleCbkFnType)func;
  if (lefrEdgeRateScaleFactorCbk == 0) lefrEdgeRateScaleFactorCbk = (lefrDoubleCbkFnType)func;
  if (lefrEdgeRateThreshold1Cbk == 0) lefrEdgeRateThreshold1Cbk = (lefrDoubleCbkFnType)func;
  if (lefrEdgeRateThreshold2Cbk == 0) lefrEdgeRateThreshold2Cbk = (lefrDoubleCbkFnType)func;
  if (lefrIRDropBeginCbk == 0) lefrIRDropBeginCbk = (lefrVoidCbkFnType)func;
  if (lefrIRDropCbk == 0) lefrIRDropCbk = (lefrIRDropCbkFnType)func;
  if (lefrIRDropEndCbk == 0) lefrIRDropEndCbk = (lefrVoidCbkFnType)func;
  if (lefrLayerCbk == 0) lefrLayerCbk = (lefrLayerCbkFnType)func;
  if (lefrLibraryEndCbk == 0) lefrLibraryEndCbk = (lefrVoidCbkFnType)func;
  if (lefrMacroBeginCbk == 0) lefrMacroBeginCbk = (lefrStringCbkFnType)func;
  if (lefrMacroCbk == 0) lefrMacroCbk = (lefrMacroCbkFnType)func;
  if (lefrMacroClassTypeCbk == 0) lefrMacroClassTypeCbk = (lefrStringCbkFnType)func;
  if (lefrMacroOriginCbk == 0) lefrMacroOriginCbk = (lefrMacroNumCbkFnType)func;
  if (lefrMacroSizeCbk == 0) lefrMacroSizeCbk = (lefrMacroNumCbkFnType)func;
  if (lefrMacroFixedMaskCbk == 0) lefrMacroFixedMaskCbk = (lefrIntegerCbkFnType)func;
  if (lefrTimingCbk == 0) lefrTimingCbk = (lefrTimingCbkFnType)func;
  if (lefrMinFeatureCbk == 0) lefrMinFeatureCbk = (lefrMinFeatureCbkFnType)func;
  if (lefrNoiseMarginCbk == 0) lefrNoiseMarginCbk = (lefrNoiseMarginCbkFnType)func;
  if (lefrNoiseTableCbk == 0) lefrNoiseTableCbk = (lefrNoiseTableCbkFnType)func;
  if (lefrNonDefaultCbk == 0) lefrNonDefaultCbk = (lefrNonDefaultCbkFnType)func;
  if (lefrObstructionCbk == 0) lefrObstructionCbk = (lefrObstructionCbkFnType)func;
  if (lefrPinCbk == 0) lefrPinCbk = (lefrPinCbkFnType)func;
  if (lefrPropBeginCbk == 0) lefrPropBeginCbk = (lefrVoidCbkFnType)func;
  if (lefrPropCbk == 0) lefrPropCbk = (lefrPropCbkFnType)func;
  if (lefrPropEndCbk == 0) lefrPropEndCbk = (lefrVoidCbkFnType)func;
  if (lefrSiteCbk == 0) lefrSiteCbk = (lefrSiteCbkFnType)func;
  if (lefrSpacingBeginCbk == 0) lefrSpacingBeginCbk = (lefrVoidCbkFnType)func;
  if (lefrSpacingCbk == 0) lefrSpacingCbk = (lefrSpacingCbkFnType)func;
  if (lefrSpacingEndCbk == 0) lefrSpacingEndCbk = (lefrVoidCbkFnType)func;
  if (lefrUnitsCbk == 0) lefrUnitsCbk = (lefrUnitsCbkFnType)func;
  if ((lefrVersionCbk == 0) && (lefrVersionStrCbk == 0)){
     // both version callbacks weren't set, if either one is set, it is ok
     lefrVersionCbk = (lefrDoubleCbkFnType)func;
     lefrVersionStrCbk = (lefrStringCbkFnType)func;
  }
  if (lefrViaCbk == 0) lefrViaCbk = (lefrViaCbkFnType)func;
  if (lefrViaRuleCbk == 0) lefrViaRuleCbk = (lefrViaRuleCbkFnType)func;
  if (lefrInputAntennaCbk == 0) lefrInputAntennaCbk = (lefrDoubleCbkFnType)func;
  if (lefrOutputAntennaCbk == 0) lefrOutputAntennaCbk = (lefrDoubleCbkFnType)func;
  if (lefrInoutAntennaCbk == 0) lefrInoutAntennaCbk = (lefrDoubleCbkFnType)func;

  /* NEW CALLBACK - Add a line here for each new callback routine */
  if (lefrAntennaInputCbk == 0) lefrAntennaInputCbk = (lefrDoubleCbkFnType)func;
  if (lefrAntennaInoutCbk == 0) lefrAntennaInoutCbk = (lefrDoubleCbkFnType)func;
  if (lefrAntennaOutputCbk == 0) lefrAntennaOutputCbk = (lefrDoubleCbkFnType)func;
  if (lefrManufacturingCbk == 0) lefrManufacturingCbk = (lefrDoubleCbkFnType)func;
  if (lefrUseMinSpacingCbk == 0) lefrUseMinSpacingCbk = (lefrUseMinSpacingCbkFnType)func;
  if (lefrClearanceMeasureCbk == 0) lefrClearanceMeasureCbk = (lefrStringCbkFnType)func;
  if (lefrMacroClassTypeCbk == 0) lefrMacroClassTypeCbk = (lefrStringCbkFnType)func;
  if (lefrMacroOriginCbk == 0) lefrMacroOriginCbk = (lefrMacroNumCbkFnType)func;
  if (lefrMacroSizeCbk == 0) lefrMacroSizeCbk = (lefrMacroNumCbkFnType)func;
  if (lefrMacroFixedMaskCbk == 0) lefrMacroFixedMaskCbk = (lefrIntegerCbkFnType)func;
  if (lefrMacroEndCbk == 0) lefrMacroEndCbk = (lefrStringCbkFnType)func;
  if (lefrMaxStackViaCbk == 0) lefrMaxStackViaCbk = (lefrMaxStackViaCbkFnType)func;
  if (lefrExtensionCbk == 0) lefrExtensionCbk = (lefrStringCbkFnType)func;
  if (lefrDensityCbk == 0) lefrDensityCbk = (lefrDensityCbkFnType)func;
  if (lefrFixedMaskCbk == 0) lefrFixedMaskCbk = (lefrIntegerCbkFnType)func;
}


/* These count up the number of times an unset callback is called... */
static int lefrUnusedCount[NOCBK];


int lefrCountFunc(lefrCallbackType_e e, void* v, lefiUserData d) {
  int i = (int)e;
  if (lefiDebug(23)) printf("count %d 0x%p 0x%p\n", (int)e, v, d);
  if (i >= 0 && i < NOCBK) {
    lefrUnusedCount[i] += 1;
    return 0;
  }
  return 1;
}


void lefrSetRegisterUnusedCallbacks() {
  int i;
  lefrRegisterUnused = 1;
  lefrSetUnusedCallbacks(lefrCountFunc);
  for (i = 0; i < NOCBK; i++)
    lefrUnusedCount[i] = 0;
}


void lefrPrintUnusedCallbacks(FILE* f) {
  int i;
  int first = 1;
  int trueCB = 1;

  if (lefrRegisterUnused == 0) {
    fprintf(f,
    "ERROR (LEFPARS-101): lefrSetRegisterUnusedCallbacks was not called to setup this data.\n");
    return;
  }

  for (i = 0; i < NOCBK; i++) {
    if (lefrUnusedCount[i]) {
      // Do not need to print yet if i is:
      //  lefrMacroClassTypeCbkType
      //  lefrMacroOriginCbkType
      //  lefrMacroSizeCbkType
      //  lefrMacroEndCbkType
      // it will be taken care later
      if (first &&
          (lefrCallbackType_e) i != lefrMacroClassTypeCbkType &&
          (lefrCallbackType_e) i != lefrMacroOriginCbkType &&
          (lefrCallbackType_e) i != lefrMacroSizeCbkType &&
          (lefrCallbackType_e) i != lefrMacroFixedMaskCbkType &&
          (lefrCallbackType_e) i != lefrMacroEndCbkType ) {
	fprintf(f,
	"WARNING (LEFPARS-201): LEF items that were present but ignored because of no callback:\n");
        first = 0;
      }
      switch ((lefrCallbackType_e) i) {
        case lefrArrayBeginCbkType: fprintf(f, "ArrayBegin"); break;
        case lefrArrayCbkType: fprintf(f, "Array"); break;
        case lefrArrayEndCbkType: fprintf(f, "ArrayEnd"); break;
        case lefrDividerCharCbkType: fprintf(f, "DividerChar"); break;
        case lefrBusBitCharsCbkType: fprintf(f, "BusBitChars"); break;
        case lefrNoWireExtensionCbkType: fprintf(f, "NoWireExtensionAtPins");
             break;
        case lefrCaseSensitiveCbkType: fprintf(f, "CaseSensitive"); break;
        case lefrCorrectionTableCbkType: fprintf(f, "CorrectionTable"); break;
        case lefrDielectricCbkType: fprintf(f, "Dielectric"); break;
        case lefrEdgeRateScaleFactorCbkType: fprintf(f, "EdgeRateScaleFactor");
             break;
        case lefrEdgeRateThreshold1CbkType: fprintf(f, "EdgeRateThreshold1");
             break;
        case lefrEdgeRateThreshold2CbkType: fprintf(f, "EdgeRateThreshold2");
             break;
        case lefrIRDropBeginCbkType: fprintf(f, "IRDropBegin"); break;
        case lefrIRDropCbkType: fprintf(f, "IRDrop"); break;
        case lefrIRDropEndCbkType: fprintf(f, "IRDropEnd"); break;
        case lefrLayerCbkType: fprintf(f, "Layer"); break;
        case lefrLibraryEndCbkType: fprintf(f, "LibraryEnd"); break;
        case lefrMacroBeginCbkType: fprintf(f, "MacroBegin"); break;
        case lefrMacroCbkType: fprintf(f, "Macro"); break;
        case lefrMinFeatureCbkType: fprintf(f, "MinFeature"); break;
        case lefrNoiseMarginCbkType: fprintf(f, "NoiseMargin"); break;
        case lefrNoiseTableCbkType: fprintf(f, "NoiseTable"); break;
        case lefrNonDefaultCbkType: fprintf(f, "NonDefault"); break;
        case lefrObstructionCbkType: fprintf(f, "Obstruction"); break;
        case lefrPinCbkType: fprintf(f, "Pin"); break;
        case lefrPropBeginCbkType: fprintf(f, "PropBegin"); break;
        case lefrPropCbkType: fprintf(f, "Prop"); break;
        case lefrPropEndCbkType: fprintf(f, "PropEnd"); break;
        case lefrSiteCbkType: fprintf(f, "Site"); break;
        case lefrSpacingBeginCbkType: fprintf(f, "SpacingBegin"); break;
        case lefrSpacingCbkType: fprintf(f, "Spacing"); break;
        case lefrSpacingEndCbkType: fprintf(f, "SpacingEnd"); break;
        case lefrUnitsCbkType: fprintf(f, "Units"); break;
        case lefrVersionCbkType: fprintf(f, "Version"); break;
        case lefrVersionStrCbkType: fprintf(f, "Version"); break;
        case lefrViaCbkType: fprintf(f, "Via"); break;
        case lefrViaRuleCbkType: fprintf(f, "ViaRule"); break;
        case lefrInputAntennaCbkType: fprintf(f, "InputAntenna"); break;
        case lefrOutputAntennaCbkType: fprintf(f, "OutputAntenna"); break;
        case lefrInoutAntennaCbkType: fprintf(f, "InoutAntenna"); break;
        case lefrAntennaInputCbkType: fprintf(f, "AntennaInput"); break;
        case lefrAntennaInoutCbkType: fprintf(f, "AntennaInout"); break;
        case lefrAntennaOutputCbkType: fprintf(f, "AntennaOutput"); break;
        case lefrManufacturingCbkType: fprintf(f, "Manufacturing"); break;
        case lefrUseMinSpacingCbkType: fprintf(f, "UseMinSpacing"); break;
        case lefrClearanceMeasureCbkType: fprintf(f, "ClearanceMeasure"); break;
        case lefrTimingCbkType: fprintf(f, "Timing"); break;
        case lefrMaxStackViaCbkType: fprintf(f, "MaxStackVia"); break;
        case lefrExtensionCbkType: fprintf(f, "Extension"); break;
        // 07/13/2001 - Wanda da Rosa
        // Don't need to print MacroClassType if it is not set,
        // since this is an extra CB for Ambit only.
        // Other users should not have to deal with it.
        // case lefrMacroClassTypeCbkType: fprintf(f, "MacroClassType"); break;
        case lefrMacroClassTypeCbkType:
        case lefrMacroOriginCbkType:
        case lefrMacroSizeCbkType:
        case lefrMacroFixedMaskCbkType:
        case lefrMacroEndCbkType:
             trueCB = 0;
             break;
        /* NEW CALLBACK  add the print here */
        case lefrDensityCbkType: fprintf(f, "Density"); break;
        case lefrFixedMaskCbkType: fprintf(f, "FixedMask"); break;
        default: fprintf(f, "BOGUS ENTRY"); break;
      }
      if (trueCB)
        fprintf(f, " %d\n", lefrUnusedCount[i]);
      else
         trueCB = 1;
    }
  }
}

void lefrUnsetCallbacks() {
  lefrAntennaInputCbk = 0;
  lefrAntennaInoutCbk = 0;
  lefrAntennaOutputCbk = 0;
  lefrArrayBeginCbk = 0;
  lefrArrayCbk = 0;
  lefrArrayEndCbk = 0;
  lefrDividerCharCbk = 0;
  lefrBusBitCharsCbk = 0;
  lefrCaseSensitiveCbk = 0;
  lefrClearanceMeasureCbk = 0;
  lefrCorrectionTableCbk = 0;
  lefrDensityCbk = 0;
  lefrDielectricCbk = 0;
  lefrEdgeRateScaleFactorCbk = 0;
  lefrEdgeRateThreshold1Cbk = 0;
  lefrEdgeRateThreshold2Cbk = 0;
  lefrExtensionCbk = 0;
  lefrInoutAntennaCbk = 0;
  lefrInputAntennaCbk = 0;
  lefrIRDropBeginCbk = 0;
  lefrIRDropCbk = 0;
  lefrIRDropEndCbk = 0;
  lefrLayerCbk = 0;
  lefrLibraryEndCbk = 0;
  lefrMacroEndCbk = 0;
  lefrMacroBeginCbk = 0;
  lefrMacroCbk = 0;
  lefrMacroClassTypeCbk = 0;
  lefrMacroOriginCbk = 0;
  lefrMacroSizeCbk = 0;
  lefrMacroFixedMaskCbk = 0;
  lefrManufacturingCbk = 0;
  lefrMaxStackViaCbk = 0;
  lefrTimingCbk = 0;
  lefrMinFeatureCbk = 0;
  lefrNoiseMarginCbk = 0;
  lefrNoiseTableCbk = 0;
  lefrNonDefaultCbk = 0;
  lefrNoWireExtensionCbk = 0;
  lefrObstructionCbk = 0;
  lefrOutputAntennaCbk = 0;
  lefrPinCbk = 0;
  lefrPropBeginCbk = 0;
  lefrPropCbk = 0;
  lefrPropEndCbk = 0;
  lefrSiteCbk = 0;
  lefrSpacingBeginCbk = 0;
  lefrSpacingCbk = 0;
  lefrSpacingEndCbk = 0;
  lefrUnitsCbk = 0;
  lefrUseMinSpacingCbk = 0;
  lefrVersionCbk = 0;
  lefrVersionStrCbk = 0;
  lefrViaCbk = 0;
  lefrViaRuleCbk = 0;
  lefrFixedMaskCbk = 0;
}

void lefrUnsetAntennaInputCbk() {
  lefrAntennaInputCbk = 0;
}

void lefrUnsetAntennaInoutCbk() {
  lefrAntennaInoutCbk = 0;
}

void lefrUnsetAntennaOutputCbk() {
  lefrAntennaOutputCbk = 0;
}

void lefrUnsetArrayBeginCbk() {
  lefrArrayBeginCbk = 0;
}

void lefrUnsetArrayCbk() {
  lefrArrayCbk = 0;
}

void lefrUnsetArrayEndCbk() {
  lefrArrayEndCbk = 0;
}

void lefrUnsetBusBitCharsCbk() {
  lefrBusBitCharsCbk = 0;
}

void lefrUnsetCaseSensitiveCbk() {
  lefrCaseSensitiveCbk = 0;
}

void lefrUnsetClearanceMeasureCbk() {
  lefrClearanceMeasureCbk = 0;
}

void lefrUnsetCorrectionTableCbk() {
  lefrCorrectionTableCbk = 0;
}

void lefrUnsetDensityCbk() {
  lefrDensityCbk = 0;
}

void lefrUnsetDielectricCbk() {
  lefrDielectricCbk = 0;
}

void lefrUnsetDividerCharCbk() {
  lefrDividerCharCbk = 0;
}

void lefrUnsetEdgeRateScaleFactorCbk() {
  lefrEdgeRateScaleFactorCbk = 0;
}

void lefrUnsetEdgeRateThreshold1Cbk() {
  lefrEdgeRateThreshold1Cbk = 0;
}

void lefrUnsetEdgeRateThreshold2Cbk() {
  lefrEdgeRateThreshold2Cbk = 0;
}

void lefrUnsetExtensionCbk() {
  lefrExtensionCbk = 0;
}

void lefrUnsetInoutAntennaCbk() {
  lefrInoutAntennaCbk = 0;
}

void lefrUnsetInputAntennaCbk() {
  lefrInputAntennaCbk = 0;
}

void lefrUnsetIRDropBeginCbk() {
  lefrIRDropBeginCbk = 0;
}

void lefrUnsetIRDropCbk() {
  lefrIRDropCbk = 0;
}

void lefrUnsetIRDropEndCbk() {
  lefrIRDropEndCbk = 0;
}

void lefrUnsetLayerCbk() {
  lefrLayerCbk = 0;
}

void lefrUnsetFixedMaskCbk() {
  lefrFixedMaskCbk = 0;
}

void lefrUnsetLibraryEndCbk() {
  lefrLibraryEndCbk = 0;
}

void lefrUnsetMacroBeginCbk() {
  lefrMacroBeginCbk = 0;
}

void lefrUnsetMacroCbk() {
  lefrMacroCbk = 0;
}

void lefrUnsetMacroClassTypeCbk() {
  lefrMacroClassTypeCbk = 0;
}

void lefrUnsetMacroEndCbk() {
  lefrMacroEndCbk = 0;
}

void lefrUnsetMacroOriginCbk() {
  lefrMacroOriginCbk = 0;
}

void lefrUnsetMacroSizeCbk() {
  lefrMacroSizeCbk = 0;
}

void lefrUnsetMacroFixedMaskCbk() {
  lefrMacroFixedMaskCbk = 0;
}

void lefrUnsetManufacturingCbk() {
  lefrManufacturingCbk = 0;
}

void lefrUnsetMaxStackViaCbk() {
  lefrMaxStackViaCbk = 0;
}

void lefrUnsetMinFeatureCbk() {
  lefrMinFeatureCbk = 0;
}

void lefrUnsetNoiseMarginCbk() {
  lefrNoiseMarginCbk = 0;
}

void lefrUnsetNoiseTableCbk() {
  lefrNoiseTableCbk = 0;
}

void lefrUnsetNonDefaultCbk() {
  lefrNonDefaultCbk = 0;
}

void lefrUnsetNoWireExtensionCbk() {
  lefrNoWireExtensionCbk = 0;
}

void lefrUnsetObstructionCbk() {
  lefrObstructionCbk = 0;
}

void lefrUnsetOutputAntennaCbk() {
  lefrOutputAntennaCbk = 0;
}

void lefrUnsetPinCbk() {
  lefrPinCbk = 0;
}

void lefrUnsetPropBeginCbk() {
  lefrPropBeginCbk = 0;
}

void lefrUnsetPropCbk() {
  lefrPropCbk = 0;
}

void lefrUnsetPropEndCbk() {
  lefrPropEndCbk = 0;
}

void lefrUnsetSiteCbk() {
  lefrSiteCbk = 0;
}

void lefrUnsetSpacingBeginCbk() {
  lefrSpacingBeginCbk = 0;
}

void lefrUnsetSpacingCbk() {
  lefrSpacingCbk = 0;
}

void lefrUnsetSpacingEndCbk() {
  lefrSpacingEndCbk = 0;
}

void lefrUnsetTimingCbk() {
  lefrTimingCbk = 0;
}

void lefrUnsetUnitsCbk() {
  lefrUnitsCbk = 0;
}

void lefrUnsetUseMinSpacingCbk() {
  lefrUseMinSpacingCbk = 0;
}

void lefrUnsetVersionCbk() {
  lefrVersionCbk = 0;
}

void lefrUnsetVersionStrCbk() {
  lefrVersionStrCbk = 0;
}

void lefrUnsetViaCbk() {
  lefrViaCbk = 0;
}

void lefrUnsetViaRuleCbk() {
  lefrViaRuleCbk = 0;
}

void lefrSetUserData(lefiUserData d) {
  lefrUserData = d;
}


lefiUserData lefrGetUserData() {
  return lefrUserData;
}


void lefrSetUnitsCbk(lefrUnitsCbkFnType f) {
  lefrUnitsCbk = f;
}


void lefrSetDividerCharCbk(lefrStringCbkFnType f) {
  lefrDividerCharCbk = f;
}


void lefrSetNoWireExtensionCbk(lefrStringCbkFnType f) {
  lefrNoWireExtensionCbk = f;
}


void lefrSetBusBitCharsCbk(lefrStringCbkFnType f) {
  lefrBusBitCharsCbk = f;
}


void lefrSetCaseSensitiveCbk(lefrIntegerCbkFnType f) {
  lefrCaseSensitiveCbk = f;
}

void lefrSetFixedMaskCbk(lefrIntegerCbkFnType f) {
    lefrFixedMaskCbk = f;
}


void lefrSetVersionCbk(lefrDoubleCbkFnType f) {
  lefrVersionCbk = f;
}


void lefrSetVersionStrCbk(lefrStringCbkFnType f) {
  lefrVersionStrCbk = f;
}


void lefrSetLayerCbk(lefrLayerCbkFnType f) {
  lefrLayerCbk = f;
}


void lefrSetViaCbk(lefrViaCbkFnType f) {
  lefrViaCbk = f;
}


void lefrSetViaRuleCbk(lefrViaRuleCbkFnType f) {
  lefrViaRuleCbk = f;
}


void lefrSetSpacingCbk(lefrSpacingCbkFnType f) {
  lefrSpacingCbk = f;
}


void lefrSetIRDropCbk(lefrIRDropCbkFnType f) {
  lefrIRDropCbk = f;
}


void lefrSetDielectricCbk(lefrDoubleCbkFnType f) {
  lefrDielectricCbk = f;
}


void lefrSetMinFeatureCbk(lefrMinFeatureCbkFnType f) {
  lefrMinFeatureCbk = f;
}


void lefrSetNonDefaultCbk(lefrNonDefaultCbkFnType f) {
  lefrNonDefaultCbk = f;
}


void lefrSetSiteCbk(lefrSiteCbkFnType f) {
  lefrSiteCbk = f;
}


void lefrSetMacroCbk(lefrMacroCbkFnType f) {
  lefrMacroCbk = f;
}


void lefrSetMacroBeginCbk(lefrStringCbkFnType f) {
  lefrMacroBeginCbk = f;
}


void lefrSetPinCbk(lefrPinCbkFnType f) {
  lefrPinCbk = f;
}


void lefrSetObstructionCbk(lefrObstructionCbkFnType f) {
  lefrObstructionCbk = f;
}


void lefrSetArrayCbk(lefrArrayCbkFnType f) {
  lefrArrayCbk = f;
}


void lefrSetLibraryEndCbk(lefrVoidCbkFnType f) {
  lefrLibraryEndCbk = f;
}


/* NEW CALLBACK - Each callback routine must have a routine that allows
 * the user to set it.  The set routines go here. */

void lefrSetPropBeginCbk(lefrVoidCbkFnType f) {
  lefrPropBeginCbk = f;
}


void lefrSetPropEndCbk(lefrVoidCbkFnType f) {
  lefrPropEndCbk = f;
}


void lefrSetPropCbk(lefrPropCbkFnType f) {
  lefrPropCbk = f;
}


void lefrSetSpacingBeginCbk(lefrVoidCbkFnType f) {
  lefrSpacingBeginCbk = f;
}


void lefrSetSpacingEndCbk(lefrVoidCbkFnType f) {
  lefrSpacingEndCbk = f;
}


void lefrSetArrayBeginCbk(lefrStringCbkFnType f) {
  lefrArrayBeginCbk = f;
}


void lefrSetArrayEndCbk(lefrStringCbkFnType f) {
  lefrArrayEndCbk = f;
}


void lefrSetIRDropBeginCbk(lefrVoidCbkFnType f) {
  lefrIRDropBeginCbk = f;
}


void lefrSetIRDropEndCbk(lefrVoidCbkFnType f) {
  lefrIRDropEndCbk = f;
}


void lefrSetNoiseMarginCbk(lefrNoiseMarginCbkFnType f) {
  lefrNoiseMarginCbk = f;
}


void lefrSetEdgeRateThreshold1Cbk(lefrDoubleCbkFnType f) {
  lefrEdgeRateThreshold1Cbk = f;
}


void lefrSetEdgeRateThreshold2Cbk(lefrDoubleCbkFnType f) { 
  lefrEdgeRateThreshold2Cbk = f;
}


void lefrSetEdgeRateScaleFactorCbk(lefrDoubleCbkFnType f) { 
  lefrEdgeRateScaleFactorCbk = f;
}


void lefrSetTimingCbk(lefrTimingCbkFnType f) { 
  lefrTimingCbk = f;
}


void lefrSetNoiseTableCbk(lefrNoiseTableCbkFnType f) { 
  lefrNoiseTableCbk = f;
}


void lefrSetCorrectionTableCbk(lefrCorrectionTableCbkFnType f) {
  lefrCorrectionTableCbk = f;
}


void lefrSetInputAntennaCbk(lefrDoubleCbkFnType f) {
  lefrInputAntennaCbk = f;
}


void lefrSetOutputAntennaCbk(lefrDoubleCbkFnType f) {
  lefrOutputAntennaCbk = f;
}


void lefrSetInoutAntennaCbk(lefrDoubleCbkFnType f) {
  lefrInoutAntennaCbk = f;
}


void lefrSetAntennaInputCbk(lefrDoubleCbkFnType f) {
  lefrAntennaInputCbk = f;
}


void lefrSetAntennaInoutCbk(lefrDoubleCbkFnType f) {
  lefrAntennaInoutCbk = f;
}


void lefrSetAntennaOutputCbk(lefrDoubleCbkFnType f) {
  lefrAntennaOutputCbk = f;
}


void lefrSetManufacturingCbk(lefrDoubleCbkFnType f) {
  lefrManufacturingCbk = f;
}


void lefrSetUseMinSpacingCbk(lefrUseMinSpacingCbkFnType f) {
  lefrUseMinSpacingCbk = f;
}


void lefrSetClearanceMeasureCbk(lefrStringCbkFnType f) {
  lefrClearanceMeasureCbk = f;
}


// 5.5
void lefrSetMaxStackViaCbk(lefrMaxStackViaCbkFnType f) {
  lefrMaxStackViaCbk = f;
}


// 4/4/2003
void lefrSetExtensionCbk(lefrStringCbkFnType f) {
  lefrExtensionCbk = f;
}


// 06-20-2001, Wanda da Rosa, PCR 377398
// This callback is added only for PKS, they need a extra call
// back of the Macro type to handle big number of obstructions.
// Nothing change on the existing code for the lefiMacro class.
void lefrSetMacroClassTypeCbk(lefrStringCbkFnType f) {
  lefrMacroClassTypeCbk = f;
}

// 09-10-2002, Wanda da Rosa
// The callbacks MacroOrigin & MacroSize is for FE only
void lefrSetMacroOriginCbk(lefrMacroNumCbkFnType f) {
  lefrMacroOriginCbk = f;
}

void lefrSetMacroSizeCbk(lefrMacroNumCbkFnType f) {
  lefrMacroSizeCbk = f;
}

void lefrSetMacroFixedMaskCbk(lefrIntegerCbkFnType f) {
  lefrMacroFixedMaskCbk = f;
}

void lefrSetMacroEndCbk(lefrStringCbkFnType f) {
  lefrMacroEndCbk = f;
}


// 5.6
void lefrSetDensityCbk(lefrDensityCbkFnType f) {
  lefrDensityCbk = f;
}


int lefrLineNumber() {
  return lef_nlines;
}

/* User defined error message function */
LEFI_LOG_FUNCTION lefiErrorLogFunction = 0;

void lefrSetLogFunction(LEFI_LOG_FUNCTION f) {
  lefiErrorLogFunction = f;
}

/* User defined warning message function */
LEFI_WARNING_LOG_FUNCTION lefiWarningLogFunction = 0;

void lefrSetWarningLogFunction(LEFI_WARNING_LOG_FUNCTION f) {
  lefiWarningLogFunction = f;
}

/* User defined malloc function */
LEFI_MALLOC_FUNCTION lefiMallocFunction = 0;

void lefrSetMallocFunction(LEFI_MALLOC_FUNCTION f) {
  lefiMallocFunction = f;
}

/* User defined realloc function */
LEFI_REALLOC_FUNCTION lefiReallocFunction = 0;

void lefrSetReallocFunction(LEFI_REALLOC_FUNCTION f) {
  lefiReallocFunction = f;
}

/* User defined free function */
LEFI_FREE_FUNCTION lefiFreeFunction = 0;

void lefrSetFreeFunction(LEFI_FREE_FUNCTION f) {
  lefiFreeFunction = f;
}

/* Callback routine with the parsed line number */
LEFI_LINE_NUMBER_FUNCTION lefiLineNumberFunction = 0;

void lefrSetLineNumberFunction(LEFI_LINE_NUMBER_FUNCTION f) {
  lefiLineNumberFunction = f;
}

void lefrSetDeltaNumberLines (int numLines) {
  lefiDeltaNumberLines = numLines;
}

/* from the lexer */
extern char lefcommentChar;


void lefrSetShiftCase() {
  lefrShiftCase = 1;
}


void lefrSetCommentChar(char c) {
  lefcommentChar = c;
}

void lefrSetCaseSensitivity(int caseSense) {
  lefNamesCaseSensitive = caseSense;
  lefReaderCaseSensitive = caseSense;
}

void lefrSetRelaxMode() {
  lefrRelaxMode = TRUE;
}

void lefrUnsetRelaxMode() {
  lefrRelaxMode = FALSE;
}

void lefrSetVersionValue (char* version) {
  userHasSetVer = 1;
  versionNum = convert_name2num(version);  
}

void lefrSetOpenLogFileAppend() {
  logFileAppend = TRUE;
}

void lefrUnsetOpenLogFileAppend() {
  logFileAppend = FALSE;
}

/* gzip read function set by lefzlib for gzip only */
LEFI_READ_FUNCTION lefiReadFunction = 0;

void lefrSetReadFunction(LEFI_READ_FUNCTION f) {
  lefiReadFunction = f;
}

void lefrUnsetReadFunction() {
  lefiReadFunction = 0;
}

////////////////////////////////////
//
//   Set the maximum number of warnings
//
/////////////////////////////////////

void lefrSetAntennaInoutWarnings(int warn) {
  lefrAntennaInoutWarnings = warn;
}

void lefrSetAntennaInputWarnings(int warn) {
  lefrAntennaInputWarnings = warn;
}

void lefrSetAntennaOutputWarnings(int warn) {
  lefrAntennaOutputWarnings = warn;
}

void lefrSetArrayWarnings(int warn) {
  lefrArrayWarnings = warn;
}

void lefrSetCaseSensitiveWarnings(int warn) {
  lefrCaseSensitiveWarnings = warn;
}

void lefrSetCorrectionTableWarnings(int warn) {
  lefrCorrectionTableWarnings = warn;
}

void lefrSetDielectricWarnings(int warn) {
  lefrDielectricWarnings = warn;
}

void lefrSetEdgeRateThreshold1Warnings(int warn) {
  lefrEdgeRateThreshold1Warnings = warn;
}

void lefrSetEdgeRateThreshold2Warnings(int warn) {
  lefrEdgeRateThreshold2Warnings = warn;
}

void lefrSetEdgeRateScaleFactorWarnings(int warn) {
  lefrEdgeRateScaleFactorWarnings = warn;
}

void lefrSetInoutAntennaWarnings(int warn) {
  lefrInoutAntennaWarnings = warn;
}

void lefrSetInputAntennaWarnings(int warn) {
  lefrInputAntennaWarnings = warn;
}

void lefrSetIRDropWarnings(int warn) {
  lefrIRDropWarnings = warn;
}

void lefrSetLayerWarnings(int warn) {
  lefrLayerWarnings = warn;
}

void lefrSetMacroWarnings(int warn) {
  lefrMacroWarnings = warn;
}

void lefrSetMaxStackViaWarnings(int warn) {
  lefrMaxStackViaWarnings = warn;
}

void lefrSetMinFeatureWarnings(int warn) {
  lefrMinFeatureWarnings = warn;
}

void lefrSetNoiseMarginWarnings(int warn) {
  lefrNoiseMarginWarnings = warn;
}

void lefrSetNoiseTableWarnings(int warn) {
  lefrNoiseTableWarnings = warn;
}

void lefrSetNonDefaultWarnings(int warn) {
  lefrNonDefaultWarnings = warn;
}

void lefrSetNoWireExtensionWarnings(int warn) {
  lefrNoWireExtensionWarnings = warn;
}

void lefrSetOutputAntennaWarnings(int warn) {
  lefrOutputAntennaWarnings = warn;
}

void lefrSetPinWarnings(int warn) {
  lefrPinWarnings = warn;
}

void lefrSetSiteWarnings(int warn) {
  lefrSiteWarnings = warn;
}

void lefrSetSpacingWarnings(int warn) {
  lefrSpacingWarnings = warn;
}

void lefrSetTimingWarnings(int warn) {
  lefrTimingWarnings = warn;
}

void lefrSetUnitsWarnings(int warn) {
  lefrUnitsWarnings = warn;
}

void lefrSetUseMinSpacingWarnings(int warn) {
  lefrUseMinSpacingWarnings = warn;
}

void lefrSetViaRuleWarnings(int warn) {
  lefrViaRuleWarnings = warn;
}

void lefrSetViaWarnings(int warn) {
  lefrViaWarnings = warn;
}

void lefrDisablePropStrProcess() {
  lefDisPropStrProcess = 1;
}

// TX_DIR:TRANSLATON ON

