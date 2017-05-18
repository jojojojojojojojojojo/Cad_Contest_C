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


#include "defrReader.hpp"
#include "defiProp.hpp"
#include "defiPropType.hpp"
#include "defrCallBacks.hpp"
#include "defiDebug.hpp"
#include "defiMisc.hpp"
#include "defiUtil.hpp"
#include "lex.h"
#include "DFEF_stringhash.h"
#include <stdlib.h>
#include <string.h>


extern int                          defyyparse(void);  // do the parsing
extern void                         lex_init(void);   // start the lexer
extern int                          names_case_sensitive;
extern int                          reader_case_sensitive;


// TX_DIR:TRANSLATION ON

extern void                         defrNerr(int num);
extern void                         NWARN(int num);

// From defiDebug.cpp
extern char                         defDebug[100];
int                                 defLogFileAppend = 0;
static char                         defrErrMsg[100];
int                                 defDisPropStrProcess = 0;

#define NODEFMSG 4013     // (9012 + 1) - 5000, def msg starts at 5000

extern int totalDefMsgLimit;
extern int defMsgPrinted;
extern int defMsgLimit[2][NODEFMSG];

void
defrNerr(int i)
{
    sprintf(defrErrMsg, "ERROR number %d\n", i);
    defiError(1, 0, defrErrMsg);
    exit(2);
}


void
defrNwarn(int i)
{
    sprintf(defrErrMsg, "WARNING number %d\n", i);
    defiError(1, 0, defrErrMsg);
    exit(2);
}



////////////////////////////////////
//
//   Global variables
//
/////////////////////////////////////
defrReader                          *defrReader::reader = 0;

int  nDDMsgs = 0;
int* disableDMsgs = 0;


// *****************************************************************
// * CLASS defrReader
// *
// * This is the main parser class.
// *****************************************************************
defrReader::defrReader()
: defrUserData(0),
  defrFileName(0),
  defrFile(0),
  defrSubnet(0),
  defrPath(0),
  defrGeomPtr(0),
  defrIsReset(0),
  doneDesign(0),
  defrDesignCbk(0),
  defrTechnologyCbk(0),
  defrDesignEndCbk(0),
  defrPropCbk(0),
  defrPropDefEndCbk(0),
  defrPropDefStartCbk(0),
  defrArrayNameCbk(0),
  defrFloorPlanNameCbk(0),
  defrUnitsCbk(0),
  defrDividerCbk(0),
  defrBusBitCbk(0),
  defrSiteCbk(0),
  defrCanplaceCbk(0),
  defrCannotOccupyCbk(0),
  defrComponentStartCbk(0),
  defrComponentEndCbk(0),
  defrComponentCbk(0),
  defrNetStartCbk(0),
  defrNetEndCbk(0),
  defrNetCbk(0),
  defrNetNameCbk(0),
  defrNetSubnetNameCbk(0),
  defrNetNonDefaultRuleCbk(0),
  defrNetPartialPathCbk(0),
  defrPathCbk(0),
  defrVersionCbk(0),
  defrVersionStrCbk(0),
  defrPinExtCbk(0),
  defrComponentExtCbk(0),
  defrViaExtCbk(0),
  defrNetConnectionExtCbk(0),
  defrNetExtCbk(0),
  defrGroupExtCbk(0),
  defrScanChainExtCbk(0),
  defrIoTimingsExtCbk(0),
  defrPartitionsExtCbk(0),
  defrHistoryCbk(0),
  defrDieAreaCbk(0),
  defrPinCapCbk(0),
  defrPinCbk(0),
  defrStartPinsCbk(0),
  defrPinEndCbk(0),
  defrDefaultCapCbk(0),
  defrRowCbk(0),
  defrTrackCbk(0),
  defrGcellGridCbk(0),
  defrViaStartCbk(0),
  defrViaEndCbk(0),
  defrViaCbk(0),
  defrRegionStartCbk(0),
  defrRegionEndCbk(0),
  defrRegionCbk(0),
  defrSNetStartCbk(0),
  defrSNetEndCbk(0),
  defrSNetCbk(0),
  defrSNetPartialPathCbk(0),
  defrSNetWireCbk(0),
  defrGroupsStartCbk(0),
  defrGroupsEndCbk(0),
  defrGroupNameCbk(0),
  defrGroupMemberCbk(0),
  defrComponentMaskShiftLayerCbk(0),
  defrGroupCbk(0),
  defrAssertionsStartCbk(0),
  defrAssertionsEndCbk(0),
  defrAssertionCbk(0),
  defrConstraintsStartCbk(0),
  defrConstraintsEndCbk(0),
  defrConstraintCbk(0),
  defrScanchainsStartCbk(0),
  defrScanchainsEndCbk(0),
  defrScanchainCbk(0),
  defrIOTimingsStartCbk(0),
  defrIOTimingsEndCbk(0),
  defrIOTimingCbk(0),
  defrFPCStartCbk(0),
  defrFPCEndCbk(0),
  defrFPCCbk(0),
  defrTimingDisablesStartCbk(0),
  defrTimingDisablesEndCbk(0),
  defrTimingDisableCbk(0),
  defrPartitionsStartCbk(0),
  defrPartitionsEndCbk(0),
  defrPartitionCbk(0),
  defrPinPropStartCbk(0),
  defrPinPropEndCbk(0),
  defrPinPropCbk(0),
  defrCaseSensitiveCbk(0),
  defrBlockageStartCbk(0),
  defrBlockageEndCbk(0),
  defrBlockageCbk(0),
  defrSlotStartCbk(0),
  defrSlotEndCbk(0),
  defrSlotCbk(0),
  defrFillStartCbk(0),
  defrFillEndCbk(0),
  defrFillCbk(0),
  defrNonDefaultStartCbk(0),
  defrNonDefaultEndCbk(0),
  defrNonDefaultCbk(0),
  defrStylesStartCbk(0),
  defrStylesEndCbk(0),
  defrStylesCbk(0),
  defrExtensionCbk(0),
  defrAddPathToNet(0),
  defrNeedPathData(0),
  defrAllowComponentNets(0),
  defrRegisterUnused(0),
  defrAssertionWarnings(999),
  defrBlockageWarnings(999),
  defrCaseSensitiveWarnings(999),
  defrComponentWarnings(999),
  defrConstraintWarnings(999),
  defrDefaultCapWarnings(999),
  defrFillWarnings(999),
  defrGcellGridWarnings(999),
  defrIOTimingWarnings(999),
  defrNetWarnings(999),
  defrNonDefaultWarnings(999),
  defrPinExtWarnings(999),
  defrPinWarnings(999),
  defrRegionWarnings(999),
  defrRowWarnings(999),
  defrTrackWarnings(999),
  defrScanchainWarnings(999),
  defrSNetWarnings(999),
  defrStylesWarnings(999),
  defrUnitsWarnings(999),
  defrVersionWarnings(999),
  defrViaWarnings(999)
{
}



void
defrReader::defrInitGlobals()
{
    int i;

    defrUserData = 0;
    defrIsReset = 0;

    // Mark added there, need to find out why
    // defrComponent.Init();

    for (i = 0; i < CBMAX; i++)
        defrUnusedCallbacks[i] = 0;

    return;
}



// Reader initialization
int
defrReader::defrInit()
{

    /* Check if the date is valid */
    /*
      if (!defiValidTime())
      exit(77);
     */

    // 05/01/2002 - Wanda da Rosa
    // PCR 438006 - Initialize global variables in a single routine
    defrInitGlobals();

    /* Check if the user is a valid one */
    /* don't need anymore
       if (!defiValidUser()) {
       printf("This DEF Reader license is valid for %s to use only!\n",
       defiUser());
       exit(77);
       }
       */
    defrReset();

    /* Normally in C++ the static initializers will take care of */
    /* these calls.  In ANSI C we must do it ourselves. */

    /* 11/25/2002 - Even C++ will take care the static initializers, but */
    /* ANSI C and HPUX linker has a problem which won't call global variables */
    /* constructors */
#ifndef __cplusplus
  defrProp.defiProp::Init();
  defrSite.defiSite::Init();
  defrComponent.defiComponent::Init();
  defrComponentMaskShiftLayer.defiComponentMaskShiftLayer::Init();
  defrNet.defiNet::Init();
  defrCannotOccupy.defiSite::Init();
  defrCanplace.defiSite::Init();
  defrRow.defiRow::Init();
  defrTrack.defiTrack::Init();
  defrGcellGrid.defiGcellGrid::Init();
  defrVia.defiVia::Init();
  defrPin.defiPin::Init();
  defrRegion.defiRegion::Init();
  defrGroup.defiGroup::Init();
  defrAssertion.defiAssertion::Init();
  defrScanchain.defiScanchain::Init();
  defrIOTiming.defiIOTiming::Init();
  defrFPC.defiFPC::Init();
  defrTimingDisable.defiTimingDisable::Init();
  defrPartition.defiPartition::Init();
  defrPinProp.defiPinProp::Init();
  defrBlockage.defiBlockage::Init();
  defrSlot.defiSlot::Init();
  defrFill.defiFill::Init();
  defrNonDefault.defiNonDefault::Init();
  defrStyles.defiStyles::Init();

  defrCompProp.defiPropType::Init();
  defrCompPinProp.defiPropType::Init();
  defrDesignProp.defiPropType::Init();
  defrGroupProp.defiPropType::Init();
  defrNDefProp.defiPropType::Init();
  defrNetProp.defiPropType::Init();
  defrPinDefProp.defiPropType::Init();
  defrRegionProp.defiPropType::Init();
  defrRowProp.defiPropType::Init();
  defrSNetProp.defiPropType::Init();

  /* NEW CALLBACK - If you had to create a global instance of a class
     * then initialize the class here (called in "C" only). */
  #endif
    return 0;
}

int
defrReader::defrReset()
{
    defrIsReset = 1;

    defrFileName = 0;
    defrFile = 0;
    defrSubnet = 0;
    defrPath = 0;
    doneDesign = 0;

    return 0;
}

defrReader*
defrReader::get()
{
    if (!defrReader::reader) {
       defrReader::reader = new defrReader;
    }

    return defrReader::reader;
}

int
defrCountUnused(defrCallbackType_e  e,
		void                *v,
		defiUserData        d)
{
    int i;
    if (defiDebug(23))
        printf("Count %d, 0x%p, 0x%p\n", (int) e, v, d);
    i = (int) e;
    if (i <= 0 || i >= CBMAX) {
        return 1;
    }
    defrReader::get()->incrUnusedCallbacks(i);

    return 0;
}


void
defrReader::defrSetRegisterUnusedCallbacks()
{
    // int i;
    defrRegisterUnused = 1;
    // for (i = 0; i < CBMAX; i++)
       // defrUnusedCallbacks[i] = 0;
    defrSetUnusedCallbacks(defrCountUnused);
}


void
defrReader::defrSetUnusedCallbacks(defrVoidCbkFnType f)
{
    if (!defrDesignCbk)
        defrDesignCbk = (defrStringCbkFnType) f;
    if (!defrTechnologyCbk)
        defrTechnologyCbk = (defrStringCbkFnType) f;
    if (!defrDesignEndCbk)
        defrDesignEndCbk = (defrVoidCbkFnType) f;
    if (!defrPropCbk)
        defrPropCbk = (defrPropCbkFnType) f;
    if (!defrPropDefEndCbk)
        defrPropDefEndCbk = (defrVoidCbkFnType) f;
    if (!defrPropDefStartCbk)
        defrPropDefStartCbk = (defrVoidCbkFnType) f;
    if (!defrArrayNameCbk)
        defrArrayNameCbk = (defrStringCbkFnType) f;
    if (!defrFloorPlanNameCbk)
        defrFloorPlanNameCbk = (defrStringCbkFnType) f;
    if (!defrUnitsCbk)
        defrUnitsCbk = (defrDoubleCbkFnType) f;
    if (!defrDividerCbk)
        defrDividerCbk = (defrStringCbkFnType) f;
    if (!defrBusBitCbk)
        defrBusBitCbk = (defrStringCbkFnType) f;
    if (!defrSiteCbk)
        defrSiteCbk = (defrSiteCbkFnType) f;
    if (!defrCanplaceCbk)
        defrCanplaceCbk = (defrSiteCbkFnType) f;
    if (!defrCannotOccupyCbk)
        defrCannotOccupyCbk = (defrSiteCbkFnType) f;
    if (!defrComponentStartCbk)
        defrComponentStartCbk = (defrIntegerCbkFnType) f;
    if (!defrComponentEndCbk)
        defrComponentEndCbk = (defrVoidCbkFnType) f;
    if (!defrComponentCbk)
        defrComponentCbk = (defrComponentCbkFnType) f;
    if (!defrNetStartCbk)
        defrNetStartCbk = (defrIntegerCbkFnType) f;
    if (!defrNetEndCbk)
        defrNetEndCbk = (defrVoidCbkFnType) f;
    if (!defrNetCbk)
        defrNetCbk = (defrNetCbkFnType) f;
    //  if (! defrNetPartialPathCbk) defrNetPartialPathCbk = (defrNetCbkFnType)f;
    if (!defrPathCbk)
        defrPathCbk = (defrPathCbkFnType) f;
    if ((!defrVersionCbk) && (!defrVersionStrCbk)) {
        // both version callbacks weren't set, if either one is set, it is ok
        defrVersionCbk = (defrDoubleCbkFnType) f;
        defrVersionStrCbk = (defrStringCbkFnType) f;
    }
    if (!defrPinExtCbk)
        defrPinExtCbk = (defrStringCbkFnType) f;
    if (!defrComponentExtCbk)
        defrComponentExtCbk = (defrStringCbkFnType) f;
    if (!defrViaExtCbk)
        defrViaExtCbk = (defrStringCbkFnType) f;
    if (!defrNetConnectionExtCbk)
        defrNetConnectionExtCbk = (defrStringCbkFnType) f;
    if (!defrNetExtCbk)
        defrNetExtCbk = (defrStringCbkFnType) f;
    if (!defrGroupExtCbk)
        defrGroupExtCbk = (defrStringCbkFnType) f;
    if (!defrScanChainExtCbk)
        defrScanChainExtCbk = (defrStringCbkFnType) f;
    if (!defrIoTimingsExtCbk)
        defrIoTimingsExtCbk = (defrStringCbkFnType) f;
    if (!defrPartitionsExtCbk)
        defrPartitionsExtCbk = (defrStringCbkFnType) f;
    if (!defrHistoryCbk)
        defrHistoryCbk = (defrStringCbkFnType) f;
    if (!defrDieAreaCbk)
        defrDieAreaCbk = (defrBoxCbkFnType) f;
    if (!defrPinCapCbk)
        defrPinCapCbk = (defrPinCapCbkFnType) f;
    if (!defrPinCbk)
        defrPinCbk = (defrPinCbkFnType) f;
    if (!defrStartPinsCbk)
        defrStartPinsCbk = (defrIntegerCbkFnType) f;
    if (!defrPinEndCbk)
        defrPinEndCbk = (defrVoidCbkFnType) f;
    if (!defrDefaultCapCbk)
        defrDefaultCapCbk = (defrIntegerCbkFnType) f;
    if (!defrRowCbk)
        defrRowCbk = (defrRowCbkFnType) f;
    if (!defrTrackCbk)
        defrTrackCbk = (defrTrackCbkFnType) f;
    if (!defrGcellGridCbk)
        defrGcellGridCbk = (defrGcellGridCbkFnType) f;
    if (!defrViaStartCbk)
        defrViaStartCbk = (defrIntegerCbkFnType) f;
    if (!defrViaEndCbk)
        defrViaEndCbk = (defrVoidCbkFnType) f;
    if (!defrViaCbk)
        defrViaCbk = (defrViaCbkFnType) f;
    if (!defrRegionStartCbk)
        defrRegionStartCbk = (defrIntegerCbkFnType) f;
    if (!defrRegionEndCbk)
        defrRegionEndCbk = (defrVoidCbkFnType) f;
    if (!defrRegionCbk)
        defrRegionCbk = (defrRegionCbkFnType) f;
    if (!defrSNetStartCbk)
        defrSNetStartCbk = (defrIntegerCbkFnType) f;
    if (!defrSNetEndCbk)
        defrSNetEndCbk = (defrVoidCbkFnType) f;
    if (!defrSNetCbk)
        defrSNetCbk = (defrNetCbkFnType) f;
    //  if (! defrSNetPartialPathCbk) defrSNetPartialPathCbk = (defrNetCbkFnType)f;
    //  if (! defrSNetWireCbk) defrSNetWireCbk = (defrNetCbkFnType)f;
    if (!defrGroupsStartCbk)
        defrGroupsStartCbk = (defrIntegerCbkFnType) f;
    if (!defrGroupsEndCbk)
        defrGroupsEndCbk = (defrVoidCbkFnType) f;
    if (!defrGroupNameCbk)
        defrGroupNameCbk = (defrStringCbkFnType) f;
    if (!defrGroupMemberCbk)
        defrGroupMemberCbk = (defrStringCbkFnType) f;
    if (!defrComponentMaskShiftLayerCbk)
        defrComponentMaskShiftLayerCbk = (defrComponentMaskShiftLayerCbkFnType) f;
    if (!defrGroupCbk)
        defrGroupCbk = (defrGroupCbkFnType) f;
    if (!defrAssertionsStartCbk)
        defrAssertionsStartCbk = (defrIntegerCbkFnType) f;
    if (!defrAssertionsEndCbk)
        defrAssertionsEndCbk = (defrVoidCbkFnType) f;
    if (!defrAssertionCbk)
        defrAssertionCbk = (defrAssertionCbkFnType) f;
    if (!defrConstraintsStartCbk)
        defrConstraintsStartCbk = (defrIntegerCbkFnType) f;
    if (!defrConstraintsEndCbk)
        defrConstraintsEndCbk = (defrVoidCbkFnType) f;
    if (!defrConstraintCbk)
        defrConstraintCbk = (defrAssertionCbkFnType) f;
    if (!defrScanchainsStartCbk)
        defrScanchainsStartCbk = (defrIntegerCbkFnType) f;
    if (!defrScanchainsEndCbk)
        defrScanchainsEndCbk = (defrVoidCbkFnType) f;
    if (!defrScanchainCbk)
        defrScanchainCbk = (defrScanchainCbkFnType) f;
    if (!defrIOTimingsStartCbk)
        defrIOTimingsStartCbk = (defrIntegerCbkFnType) f;
    if (!defrIOTimingsEndCbk)
        defrIOTimingsEndCbk = (defrVoidCbkFnType) f;
    if (!defrIOTimingCbk)
        defrIOTimingCbk = (defrIOTimingCbkFnType) f;
    if (!defrFPCStartCbk)
        defrFPCStartCbk = (defrIntegerCbkFnType) f;
    if (!defrFPCEndCbk)
        defrFPCEndCbk = (defrVoidCbkFnType) f;
    if (!defrFPCCbk)
        defrFPCCbk = (defrFPCCbkFnType) f;
    if (!defrTimingDisablesStartCbk)
        defrTimingDisablesStartCbk = (defrIntegerCbkFnType) f;
    if (!defrTimingDisablesEndCbk)
        defrTimingDisablesEndCbk = (defrVoidCbkFnType) f;
    if (!defrTimingDisableCbk)
        defrTimingDisableCbk = (defrTimingDisableCbkFnType) f;
    if (!defrPartitionsStartCbk)
        defrPartitionsStartCbk = (defrIntegerCbkFnType) f;
    if (!defrPartitionsEndCbk)
        defrPartitionsEndCbk = (defrVoidCbkFnType) f;
    if (!defrPartitionCbk)
        defrPartitionCbk = (defrPartitionCbkFnType) f;
    if (!defrPinPropStartCbk)
        defrPinPropStartCbk = (defrIntegerCbkFnType) f;
    if (!defrPinPropEndCbk)
        defrPinPropEndCbk = (defrVoidCbkFnType) f;
    if (!defrPinPropCbk)
        defrPinPropCbk = (defrPinPropCbkFnType) f;
    if (!defrCaseSensitiveCbk)
        defrCaseSensitiveCbk = (defrIntegerCbkFnType) f;
    if (!defrBlockageStartCbk)
        defrBlockageStartCbk = (defrIntegerCbkFnType) f;
    if (!defrBlockageEndCbk)
        defrBlockageEndCbk = (defrVoidCbkFnType) f;
    if (!defrBlockageCbk)
        defrBlockageCbk = (defrBlockageCbkFnType) f;
    if (!defrSlotStartCbk)
        defrSlotStartCbk = (defrIntegerCbkFnType) f;
    if (!defrSlotEndCbk)
        defrSlotEndCbk = (defrVoidCbkFnType) f;
    if (!defrSlotCbk)
        defrSlotCbk = (defrSlotCbkFnType) f;
    if (!defrFillStartCbk)
        defrFillStartCbk = (defrIntegerCbkFnType) f;
    if (!defrFillEndCbk)
        defrFillEndCbk = (defrVoidCbkFnType) f;
    if (!defrFillCbk)
        defrFillCbk = (defrFillCbkFnType) f;
    if (!defrNonDefaultStartCbk)
        defrNonDefaultStartCbk = (defrIntegerCbkFnType) f;
    if (!defrNonDefaultEndCbk)
        defrNonDefaultEndCbk = (defrVoidCbkFnType) f;
    if (!defrNonDefaultCbk)
        defrNonDefaultCbk = (defrNonDefaultCbkFnType) f;
    if (!defrStylesStartCbk)
        defrStylesStartCbk = (defrIntegerCbkFnType) f;
    if (!defrStylesEndCbk)
        defrStylesEndCbk = (defrVoidCbkFnType) f;
    if (!defrStylesCbk)
        defrStylesCbk = (defrStylesCbkFnType) f;
    if (!defrExtensionCbk)
        defrExtensionCbk = (defrStringCbkFnType) f;

    /* NEW CALLBACK - Each new callback must have an entry here. */
}


void
defrReader::defrUnsetCallbacks()
{
    defrArrayNameCbk = 0;
    defrAssertionCbk = 0;
    defrAssertionsEndCbk = 0;
    defrAssertionsStartCbk = 0;
    defrBlockageCbk = 0;
    defrBlockageEndCbk = 0;
    defrBlockageStartCbk = 0;
    defrBusBitCbk = 0;
    defrCannotOccupyCbk = 0;
    defrCanplaceCbk = 0;
    defrCaseSensitiveCbk = 0;
    defrComponentCbk = 0;
    defrComponentEndCbk = 0;
    defrComponentExtCbk = 0;
    defrComponentStartCbk = 0;
    defrConstraintCbk = 0;
    defrConstraintsEndCbk = 0;
    defrConstraintsStartCbk = 0;
    defrDefaultCapCbk = 0;
    defrDesignCbk = 0;
    defrDesignEndCbk = 0;
    defrDieAreaCbk = 0;
    defrDividerCbk = 0;
    defrExtensionCbk = 0;
    defrFloorPlanNameCbk = 0;
    defrGcellGridCbk = 0;
    defrGroupCbk = 0;
    defrGroupExtCbk = 0;
    defrGroupMemberCbk = 0;
    defrComponentMaskShiftLayerCbk = 0;
    defrGroupNameCbk = 0;
    defrGroupsEndCbk = 0;
    defrGroupsStartCbk = 0;
    defrFillCbk = 0;
    defrFillEndCbk = 0;
    defrFillStartCbk = 0;
    defrFPCCbk = 0;
    defrFPCEndCbk = 0;
    defrFPCStartCbk = 0;
    defrHistoryCbk = 0;
    defrIOTimingCbk = 0;
    defrIOTimingsEndCbk = 0;
    defrIoTimingsExtCbk = 0;
    defrIOTimingsStartCbk = 0;
    defrNetCbk = 0;
    defrNetConnectionExtCbk = 0;
    defrNetEndCbk = 0;
    defrNetExtCbk = 0;
    defrNetNameCbk = 0;
    defrNetNonDefaultRuleCbk = 0;
    defrNetSubnetNameCbk = 0;
    defrNetPartialPathCbk = 0;
    defrNetStartCbk = 0;
    defrNonDefaultCbk = 0;
    defrNonDefaultEndCbk = 0;
    defrNonDefaultStartCbk = 0;
    defrPartitionCbk = 0;
    defrPartitionsEndCbk = 0;
    defrPartitionsExtCbk = 0;
    defrPartitionsStartCbk = 0;
    defrPathCbk = 0;
    defrPinCapCbk = 0;
    defrPinCbk = 0;
    defrPinEndCbk = 0;
    defrPinExtCbk = 0;
    defrPinPropCbk = 0;
    defrPinPropEndCbk = 0;
    defrPinPropStartCbk = 0;
    defrPropCbk = 0;
    defrPropDefEndCbk = 0;
    defrPropDefStartCbk = 0;
    defrRegionCbk = 0;
    defrRegionEndCbk = 0;
    defrRegionStartCbk = 0;
    defrRowCbk = 0;
    defrScanchainCbk = 0;
    defrScanchainsEndCbk = 0;
    defrScanChainExtCbk = 0;
    defrScanchainsStartCbk = 0;
    defrSiteCbk = 0;
    defrSlotCbk = 0;
    defrSlotEndCbk = 0;
    defrSlotStartCbk = 0;
    defrSNetCbk = 0;
    defrSNetEndCbk = 0;
    defrSNetPartialPathCbk = 0;
    defrSNetStartCbk = 0;
    defrSNetWireCbk = 0;
    defrStartPinsCbk = 0;
    defrStylesCbk = 0;
    defrStylesEndCbk = 0;
    defrStylesStartCbk = 0;
    defrTechnologyCbk = 0;
    defrTimingDisableCbk = 0;
    defrTimingDisablesEndCbk = 0;
    defrTimingDisablesStartCbk = 0;
    defrTrackCbk = 0;
    defrUnitsCbk = 0;
    defrVersionCbk = 0;
    defrVersionStrCbk = 0;
    defrViaCbk = 0;
    defrViaExtCbk = 0;
    defrViaEndCbk = 0;
    defrViaStartCbk = 0;
}

const char *
typeToString(defrCallbackType_e num)
{
    switch ((int) num) {

    case defrUnspecifiedCbkType:
        return "Unspecified";
    case defrDesignStartCbkType:
        return "Design Start";
    case defrTechNameCbkType:
        return "Tech Name";
    case defrPropCbkType:
        return "Property";
    case defrPropDefEndCbkType:
        return "Property Definitions Section End";
    case defrPropDefStartCbkType:
        return "Property Definitions Section Start";
    case defrFloorPlanNameCbkType:
        return "FloorPlanName";
    case defrArrayNameCbkType:
        return "Array Name";
    case defrUnitsCbkType:
        return "Units";
    case defrDividerCbkType:
        return "Divider";
    case defrBusBitCbkType:
        return "BusBit Character";
    case defrSiteCbkType:
        return "Site";
    case defrComponentMaskShiftLayerCbkType:
        return "ComponentMaskShiftLayer";
    case defrComponentStartCbkType:
        return "Components Section Start";
    case defrComponentCbkType:
        return "Component";
    case defrComponentEndCbkType:
        return "Components Section End";
    case defrNetStartCbkType:
        return "Nets Section Start";
    case defrNetCbkType:
        return "Net";
    case defrNetNameCbkType:
        return "Net Name";
    case defrNetNonDefaultRuleCbkType:
        return "Net Nondefaultrule";
    case defrNetSubnetNameCbkType:
        return "Net Subnet Name";
    case defrNetEndCbkType:
        return "Nets Section End";
    case defrPathCbkType:
        return "Path";
    case defrVersionCbkType:
        return "Version";
    case defrVersionStrCbkType:
        return "Version";
    case defrComponentExtCbkType:
        return "Component User Extention";
    case defrPinExtCbkType:
        return "Pin User Extension";
    case defrViaExtCbkType:
        return "Via User Extension";
    case defrNetConnectionExtCbkType:
        return "NetConnection User Extention";
    case defrNetExtCbkType:
        return "Net User Extension";
    case defrGroupExtCbkType:
        return "Group User Extension";
    case defrScanChainExtCbkType:
        return "ScanChain User Extension";
    case defrIoTimingsExtCbkType:
        return "IoTimings User Extension";
    case defrPartitionsExtCbkType:
        return "Partitions User Extension";
    case defrHistoryCbkType:
        return "History";
    case defrDieAreaCbkType:
        return "DieArea";
    case defrCanplaceCbkType:
        return "Canplace";
    case defrCannotOccupyCbkType:
        return "CannotOccupy";
    case defrPinCapCbkType:
        return "PinCap";
    case defrDefaultCapCbkType:
        return "DefaultCap";
    case defrStartPinsCbkType:
        return "Start Pins Section";
    case defrPinCbkType:
        return "Pin";
    case defrPinEndCbkType:
        return "End Pins Section";
    case defrRowCbkType:
        return "Row";
    case defrTrackCbkType:
        return "Track";
    case defrGcellGridCbkType:
        return "GcellGrid";
    case defrViaStartCbkType:
        return "Start Vias Section";
    case defrViaCbkType:
        return "Via";
    case defrViaEndCbkType:
        return "End Vias Section";
    case defrRegionStartCbkType:
        return "Region Section Start";
    case defrRegionCbkType:
        return "Region";
    case defrRegionEndCbkType:
        return "Region Section End";
    case defrSNetStartCbkType:
        return "Special Net Section Start";
    case defrSNetCbkType:
        return "Special Net";
    case defrSNetEndCbkType:
        return "Special Net Section End";
    case defrGroupsStartCbkType:
        return "Groups Section Start";
    case defrGroupNameCbkType:
        return "Group Name";
    case defrGroupMemberCbkType:
        return "Group Member";
    case defrGroupCbkType:
        return "Group";
    case defrGroupsEndCbkType:
        return "Groups Section End";
    case defrAssertionsStartCbkType:
        return "Assertions Section Start";
    case defrAssertionCbkType:
        return "Assertion";
    case defrAssertionsEndCbkType:
        return "Assertions Section End";
    case defrConstraintsStartCbkType:
        return "Constraints Section Start";
    case defrConstraintCbkType:
        return "Constraint";
    case defrConstraintsEndCbkType:
        return "Constraints Section End";
    case defrScanchainsStartCbkType:
        return "Scanchains Section Start";
    case defrScanchainCbkType:
        return "Scanchain";
    case defrScanchainsEndCbkType:
        return "Scanchains Section End";
    case defrIOTimingsStartCbkType:
        return "IOTimings Section Start";
    case defrIOTimingCbkType:
        return "IOTiming";
    case defrIOTimingsEndCbkType:
        return "IOTimings Section End";
    case defrFPCStartCbkType:
        return "Floor Plan Constraints Section Start";
    case defrFPCCbkType:
        return "Floor Plan Constraint";
    case defrFPCEndCbkType:
        return "Floor Plan Constraints Section End";
    case defrTimingDisablesStartCbkType:
        return "TimingDisables Section Start";
    case defrTimingDisableCbkType:
        return "TimingDisable";
    case defrTimingDisablesEndCbkType:
        return "TimingDisables Section End";
    case defrPartitionsStartCbkType:
        return "Partitions Section Start";
    case defrPartitionCbkType:
        return "Partition";
    case defrPartitionsEndCbkType:
        return "Partitions Section End";
    case defrPinPropStartCbkType:
        return "PinProp Section Start";
    case defrPinPropCbkType:
        return "PinProp";
    case defrPinPropEndCbkType:
        return "PinProp Section End";
    case defrCaseSensitiveCbkType:
        return "CaseSensitive";
    case defrBlockageStartCbkType:
        return "Blockage Section Start";
    case defrBlockageCbkType:
        return "Blockage";
    case defrBlockageEndCbkType:
        return "Blockage Section End";
    case defrSlotStartCbkType:
        return "Slots Section Start";
    case defrSlotCbkType:
        return "Slots";
    case defrSlotEndCbkType:
        return "Slots Section End";
    case defrFillStartCbkType:
        return "Fills Section Start";
    case defrFillCbkType:
        return "Fills";
    case defrFillEndCbkType:
        return "Fills Section End";
    case defrNonDefaultStartCbkType:
        return "NonDefaultRule Section Start";
    case defrNonDefaultCbkType:
        return "NonDefault";
    case defrNonDefaultEndCbkType:
        return "NonDefaultRule Section End";
    case defrStylesStartCbkType:
        return "Styles Section Start";
    case defrStylesCbkType:
        return "Styles";
    case defrStylesEndCbkType:
        return "Styles Section End";
    case defrExtensionCbkType:
        return "Extension";

        /* NEW CALLBACK - If you created a new callback then add the
           * type enums that you created here for debug printing. */

    case defrDesignEndCbkType:
        return "DesignEnd";
    default:
        break;
    }
    return "BOGUS";
}

void
defrReader::defrPrintUnusedCallbacks(FILE *log)
{
    int i;
    int first = 1;

    for (i = 0; i < CBMAX; i++) {
        if (defrUnusedCallbacks[i]) {
            if (first) {
                fprintf(log,
                        "WARNING (DEFPARS-5001): DEF statement found in the def file with no callback set.\n");
                first = 0;
            }
            fprintf(log, "%5d %s\n", defrUnusedCallbacks[i],
                    typeToString((defrCallbackType_e) i));
        }
    }
}




int
defrCatchAll(defrCallbackType_e typ,
	     void               *data,
	     defiUserData       ud)
{
    if (defDebug[3])
        printf("CatchAll(%d, 0x%p, 0x%p)\n", typ, data, ud);
    
    if ((int) typ >= 0 && (int) typ < CBMAX) {
	defrReader::get()->incrUnusedCallbacks((int) typ);
    } else {
	defrReader::get()->incrUnusedCallbacks(0);
        return 1;
    }

    return 0;
}



int
defrReader::defrReleaseNResetMemory()
{
    defrAssertion.defiAssertion::Destroy();
    defrBlockage.defiBlockage::Destroy();
    defrCannotOccupy.defiSite::Destroy();
    defrCanplace.defiSite::Destroy();
    defrComponent.defiComponent::Destroy();
    defrComponentMaskShiftLayer.defiComponentMaskShiftLayer::Destroy();
    defrCompPinProp.defiPropType::Destroy();
    defrCompProp.defiPropType::Destroy();
    defrDesignProp.defiPropType::Destroy();
    defrFill.defiFill::Destroy();
    defrFPC.defiFPC::Destroy();
    defrGcellGrid.defiGcellGrid::Destroy();
    defrGroup.defiGroup::Destroy();
    defrGroupProp.defiPropType::Destroy();
    defrIOTiming.defiIOTiming::Destroy();
    defrNDefProp.defiPropType::Destroy();
    defrNet.defiNet::Destroy();
    defrNetProp.defiPropType::Destroy();
    defrNonDefault.defiNonDefault::Destroy();
    defrPartition.defiPartition::Destroy();
    defrPin.defiPin::Destroy();
    defrPinDefProp.defiPropType::Destroy();
    defrPinProp.defiPinProp::Destroy();
    defrProp.defiProp::Destroy();
    defrRegion.defiRegion::Destroy();
    defrRegionProp.defiPropType::Destroy();
    defrRow.defiRow::Destroy();
    defrRowProp.defiPropType::Destroy();
    defrScanchain.defiScanchain::Destroy();
    defrSite.defiSite::Destroy();
    defrSlot.defiSlot::Destroy();
    defrSNetProp.defiPropType::Destroy();
    defrStyles.defiStyles::Destroy();
    defrTimingDisable.defiTimingDisable::Destroy();
    defrTrack.defiTrack::Destroy();
    defrVia.defiVia::Destroy();

    defrAssertion.defiAssertion::Init();
    defrBlockage.defiBlockage::Init();
    defrCannotOccupy.defiSite::Init();
    defrCanplace.defiSite::Init();
    defrComponent.defiComponent::Init();
    defrComponentMaskShiftLayer.defiComponentMaskShiftLayer::Init();
    defrCompPinProp.defiPropType::Init();
    defrCompProp.defiPropType::Init();
    defrDesignProp.defiPropType::Init();
    defrFill.defiFill::Init();
    defrFPC.defiFPC::Init();
    defrGcellGrid.defiGcellGrid::Init();
    defrGroup.defiGroup::Init();
    defrGroupProp.defiPropType::Init();
    defrIOTiming.defiIOTiming::Init();
    defrNDefProp.defiPropType::Init();
    defrNet.defiNet::Init();
    defrNetProp.defiPropType::Init();
    defrNonDefault.defiNonDefault::Init();
    defrPartition.defiPartition::Init();
    defrPin.defiPin::Init();
    defrPinDefProp.defiPropType::Init();
    defrPinProp.defiPinProp::Init();
    defrProp.defiProp::Init();
    defrRegion.defiRegion::Init();
    defrRegionProp.defiPropType::Init();
    defrRow.defiRow::Init();
    defrRowProp.defiPropType::Init();
    defrScanchain.defiScanchain::Init();
    defrSite.defiSite::Init();
    defrSlot.defiSlot::Init();
    defrSNetProp.defiPropType::Init();
    defrStyles.defiStyles::Init();
    defrTimingDisable.defiTimingDisable::Init();
    defrTrack.defiTrack::Init();
    defrVia.defiVia::Init();

    return 0;
}


int
defrReader::defrRead(FILE           *f,
                     const char     *fName,
                     defiUserData   uData,
                     int            case_sensitive)
{

    int status;

    if (defrIsReset == 0) {
        fprintf(stderr, "ERROR (DEFPARS-5000): defrRead called before defrInit\n");
        return -1;
    }

    defrFileName = (char*) fName;
    defrFile = f;
    defrUserData = uData;
    reader_case_sensitive = case_sensitive;

    // Create a path pointer that is all ready to go just in case
    // we need it later.

    defrNeedPathData = (
        ((defrNetCbk || defrSNetCbk) && defrAddPathToNet) || defrPathCbk) ? 1 : 0;
    if (defrNeedPathData) {
        defrPath = (defiPath*) malloc(sizeof(defiPath));
        defrPath->defiPath::Init();
    }


    /* Setup the lexer */
    lex_init();

    /* Parse the file. */
    /* If there are any callbacks they will happen here. */
    status = defyyparse();

    // Clean up the vars.
    defrProp.defiProp::clear();
    defrSite.defiSite::clear();
    defrComponent.defiComponent::clear();
    defrComponentMaskShiftLayer.defiComponentMaskShiftLayer::clear();
    defrNet.defiNet::clear();
    defrCannotOccupy.defiSite::clear();
    defrCanplace.defiSite::clear();
    defrScanchain.defiScanchain::clear();
    defrIOTiming.defiIOTiming::clear();
    defrFPC.defiFPC::clear();
    defrTimingDisable.defiTimingDisable::clear();
    defrPartition.defiPartition::clear();
    if (defrNeedPathData) { // clean-up the last defrPath 
        if (defrPath) {
            defrPath->defiPath::Destroy();
            defFree((char*) (defrPath));
        }
    }
    lex_un_init();

    defrIsReset = 0;
    return status;
}


void
defrReader::pathIsDone(int  shield,
                       int  reset,
                       int  netOsnet,
                       int  *needCbk)
{
    if ((defrNetCbk || defrSNetCbk) && defrAddPathToNet) {
        //defrPath->reverseOrder();
        if (defrSubnet) {
            // if (shield)
            //    defrSubnet->defiSubnet::addShieldPath(defrPath);
            // else 
            defrSubnet->defiSubnet::addWirePath(defrPath, reset, netOsnet,
                                                needCbk);

        } else {
            if (shield)
                defrNet.defiNet::addShieldPath(defrPath, reset, netOsnet, needCbk);
            else
                defrNet.defiNet::addWirePath(defrPath, reset, netOsnet, needCbk);

        }

    } else if (defrPathCbk) {
        //defrPath->reverseOrder();
        (*defrPathCbk)(defrPathCbkType, defrPath, defrUserData);
        defrPath->defiPath::Destroy();
        free((char*) defrPath);
    }

    defrPath = (defiPath*) malloc(sizeof(defiPath));

    defrPath->defiPath::Init();

}

void defrReader::setUserData(defiUserData d) {defrUserData = d;};
defiUserData defrReader::getUserData() {return defrUserData;}
void defrReader::setDesignCbk(defrStringCbkFnType f) {defrDesignCbk = f;}
void defrReader::setTechnologyCbk(defrStringCbkFnType f) {defrTechnologyCbk = f;}
void defrReader::setDesignEndCbk(defrVoidCbkFnType f){defrDesignEndCbk = f;}
void defrReader::setPropCbk(defrPropCbkFnType f){defrPropCbk = f;}
void defrReader::setPropDefEndCbk(defrVoidCbkFnType f){defrPropDefEndCbk = f;}
void defrReader::setPropDefStartCbk(defrVoidCbkFnType f){defrPropDefStartCbk = f;}
void defrReader::setArrayNameCbk(defrStringCbkFnType f){defrArrayNameCbk = f;}
void defrReader::setFloorPlanNameCbk(defrStringCbkFnType f){defrFloorPlanNameCbk = f;}
void defrReader::setUnitsCbk(defrDoubleCbkFnType f){defrUnitsCbk = f;}
void defrReader::setVersionCbk(defrDoubleCbkFnType f){defrVersionCbk = f;}
void defrReader::setVersionStrCbk(defrStringCbkFnType f){defrVersionStrCbk = f;}
void defrReader::setDividerCbk(defrStringCbkFnType f){defrDividerCbk = f;}
void defrReader::setBusBitCbk(defrStringCbkFnType f){defrBusBitCbk = f;}
void defrReader::setSiteCbk(defrSiteCbkFnType f){defrSiteCbk = f;}
void defrReader::setCanplaceCbk(defrSiteCbkFnType f){defrCanplaceCbk = f;}
void defrReader::setCannotOccupyCbk(defrSiteCbkFnType f) {defrCannotOccupyCbk = f;}
void defrReader::setComponentStartCbk(defrIntegerCbkFnType f) {defrComponentStartCbk = f ;}
void defrReader::setComponentEndCbk(defrVoidCbkFnType f) {defrComponentEndCbk = f;}
void defrReader::setComponentCbk(defrComponentCbkFnType f) {defrComponentCbk = f;}
void defrReader::setComponentMaskShiftLayerCbk(defrComponentMaskShiftLayerCbkFnType f) {defrComponentMaskShiftLayerCbk = f;}
void defrReader::setNetStartCbk(defrIntegerCbkFnType f) {defrNetStartCbk = f;}
void defrReader::setNetEndCbk(defrVoidCbkFnType f) {defrNetEndCbk = f;}
void defrReader::setNetCbk(defrNetCbkFnType f) {defrNetCbk = f;}
void defrReader::setNetNameCbk(defrStringCbkFnType f) {defrNetNameCbk = f;}
void defrReader::setNetSubnetNameCbk(defrStringCbkFnType f) {defrNetSubnetNameCbk = f;}
void defrReader::setNetNonDefaultRuleCbk(defrStringCbkFnType f) {defrNetNonDefaultRuleCbk = f;}
void defrReader::setNetPartialPathCbk(defrNetCbkFnType f) {defrNetPartialPathCbk = f;}
void defrReader::setSNetStartCbk(defrIntegerCbkFnType f) {defrSNetStartCbk = f;}
void defrReader::setSNetEndCbk(defrVoidCbkFnType f) {defrSNetEndCbk = f;}
void defrReader::setSNetCbk(defrNetCbkFnType f) {defrSNetCbk = f;}
void defrReader::setSNetPartialPathCbk(defrNetCbkFnType f) {defrSNetPartialPathCbk = f;}
void defrReader::setSNetWireCbk(defrNetCbkFnType f) {defrSNetWireCbk = f;}
void defrReader::setPathCbk(defrPathCbkFnType f) {defrPathCbk = f;}
void defrReader::setAddPathToNet() {defrAddPathToNet = 1;}
void defrReader::setAllowComponentNets() {defrAllowComponentNets = 1;}
void defrReader::setComponentExtCbk(defrStringCbkFnType f) {defrComponentExtCbk = f;}
void defrReader::setPinExtCbk(defrStringCbkFnType f) {defrPinExtCbk = f;}
void defrReader::setViaExtCbk(defrStringCbkFnType f) {defrViaExtCbk = f;}
void defrReader::setNetConnectionExtCbk(defrStringCbkFnType f) {defrNetConnectionExtCbk = f;}
void defrReader::setNetExtCbk(defrStringCbkFnType f) {defrNetExtCbk = f;}
void defrReader::setGroupExtCbk(defrStringCbkFnType f) {defrGroupExtCbk = f;}
void defrReader::setScanChainExtCbk(defrStringCbkFnType f) {defrScanChainExtCbk = f;}
void defrReader::setIoTimingsExtCbk(defrStringCbkFnType f) {defrIoTimingsExtCbk = f;}
void defrReader::setPartitionsExtCbk(defrStringCbkFnType f) {defrPartitionsExtCbk = f;}
void defrReader::setHistoryCbk(defrStringCbkFnType f) {defrHistoryCbk = f;}
void defrReader::setDieAreaCbk(defrBoxCbkFnType f) {defrDieAreaCbk = f;}
void defrReader::setPinCapCbk(defrPinCapCbkFnType f) {defrPinCapCbk = f;}
void defrReader::setPinEndCbk(defrVoidCbkFnType f) {defrPinEndCbk = f;}
void defrReader::setStartPinsCbk(defrIntegerCbkFnType f) {defrStartPinsCbk = f;}
void defrReader::setDefaultCapCbk(defrIntegerCbkFnType f) {defrDefaultCapCbk = f;}
void defrReader::setPinCbk(defrPinCbkFnType f) {defrPinCbk = f;}
void defrReader::setRowCbk(defrRowCbkFnType f) {defrRowCbk = f;}
void defrReader::setTrackCbk(defrTrackCbkFnType f) {defrTrackCbk = f;}
void defrReader::setGcellGridCbk(defrGcellGridCbkFnType f) {defrGcellGridCbk = f;}
void defrReader::setViaStartCbk(defrIntegerCbkFnType f) {defrViaStartCbk = f;}
void defrReader::setViaEndCbk(defrVoidCbkFnType f) {defrViaEndCbk = f;}
void defrReader::setViaCbk(defrViaCbkFnType f) {defrViaCbk = f;}
void defrReader::setRegionStartCbk(defrIntegerCbkFnType f) {defrRegionStartCbk = f;}
void defrReader::setRegionEndCbk(defrVoidCbkFnType f) {defrRegionEndCbk = f;}
void defrReader::setRegionCbk(defrRegionCbkFnType f) {defrRegionCbk = f;}
void defrReader::setGroupsStartCbk(defrIntegerCbkFnType f) {defrGroupsStartCbk = f;}
void defrReader::setGroupsEndCbk(defrVoidCbkFnType f) {defrGroupsEndCbk = f;}
void defrReader::setGroupNameCbk(defrStringCbkFnType f) {defrGroupNameCbk = f;}
void defrReader::setGroupMemberCbk(defrStringCbkFnType f) {defrGroupMemberCbk = f;}
void defrReader::setGroupCbk(defrGroupCbkFnType f) {defrGroupCbk = f;}
void defrReader::setAssertionsStartCbk(defrIntegerCbkFnType f) {defrAssertionsStartCbk = f;}
void defrReader::setAssertionsEndCbk(defrVoidCbkFnType f) {defrAssertionsEndCbk = f;}
void defrReader::setAssertionCbk(defrAssertionCbkFnType f) {defrAssertionCbk = f;}
void defrReader::setConstraintsStartCbk(defrIntegerCbkFnType f) {defrConstraintsStartCbk = f;}
void defrReader::setConstraintsEndCbk(defrVoidCbkFnType f) {defrConstraintsEndCbk = f;}
void defrReader::setConstraintCbk(defrAssertionCbkFnType f) {defrConstraintCbk = f;}
void defrReader::setScanchainsStartCbk(defrIntegerCbkFnType f) {defrScanchainsStartCbk = f;}
void defrReader::setScanchainsEndCbk(defrVoidCbkFnType f) {defrScanchainsEndCbk = f;}
void defrReader::setScanchainCbk(defrScanchainCbkFnType f) {defrScanchainCbk = f;}
void defrReader::setIOTimingsStartCbk(defrIntegerCbkFnType f) {defrIOTimingsStartCbk = f ;}
void defrReader::setIOTimingsEndCbk(defrVoidCbkFnType f) {defrIOTimingsEndCbk = f;}
void defrReader::setIOTimingCbk(defrIOTimingCbkFnType f) {defrIOTimingCbk = f;}
void defrReader::setFPCStartCbk(defrIntegerCbkFnType f) {defrFPCStartCbk = f;}
void defrReader::setFPCEndCbk(defrVoidCbkFnType f) {defrFPCEndCbk = f;}
void defrReader::setFPCCbk(defrFPCCbkFnType f) {defrFPCCbk = f;}
void defrReader::setTimingDisablesStartCbk(defrIntegerCbkFnType f) {defrTimingDisablesStartCbk = f;}
void defrReader::setTimingDisablesEndCbk(defrVoidCbkFnType f) {defrTimingDisablesEndCbk = f;}
void defrReader::setTimingDisableCbk(defrTimingDisableCbkFnType f) {defrTimingDisableCbk = f;}
void defrReader::setPartitionsStartCbk(defrIntegerCbkFnType f) {defrPartitionsStartCbk = f;}
void defrReader::setPartitionsEndCbk(defrVoidCbkFnType f) {defrPartitionsEndCbk = f;}
void defrReader::setPartitionCbk(defrPartitionCbkFnType f) {defrPartitionCbk = f;}
void defrReader::setPinPropStartCbk(defrIntegerCbkFnType f) {defrPinPropStartCbk = f;}
void defrReader::setPinPropEndCbk(defrVoidCbkFnType f) {defrPinPropEndCbk = f;}
void defrReader::setPinPropCbk(defrPinPropCbkFnType f) {defrPinPropCbk = f;}
void defrReader::setCaseSensitiveCbk(defrIntegerCbkFnType f) {defrCaseSensitiveCbk = f;}
void defrReader::setBlockageStartCbk(defrIntegerCbkFnType f) {defrBlockageStartCbk = f;}
void defrReader::setBlockageEndCbk(defrVoidCbkFnType f) {defrBlockageEndCbk = f;}
void defrReader::setBlockageCbk(defrBlockageCbkFnType f) {defrBlockageCbk = f;}
void defrReader::setSlotStartCbk(defrIntegerCbkFnType f) {defrSlotStartCbk = f;}
void defrReader::setSlotEndCbk(defrVoidCbkFnType f) {defrSlotEndCbk = f;}
void defrReader::setSlotCbk(defrSlotCbkFnType f) {defrSlotCbk = f;}
void defrReader::setFillStartCbk(defrIntegerCbkFnType f) {defrFillStartCbk = f;}
void defrReader::setFillEndCbk(defrVoidCbkFnType f) {defrFillEndCbk = f;}
void defrReader::setFillCbk(defrFillCbkFnType f) {defrFillCbk = f;}
void defrReader::setNonDefaultStartCbk(defrIntegerCbkFnType f) {defrNonDefaultStartCbk = f;}
void defrReader::setNonDefaultEndCbk(defrVoidCbkFnType f) {defrNonDefaultEndCbk = f;}
void defrReader::setNonDefaultCbk(defrNonDefaultCbkFnType f) {defrNonDefaultCbk = f;}
void defrReader::setStylesStartCbk(defrIntegerCbkFnType f) {defrStylesStartCbk = f;}
void defrReader::setStylesEndCbk(defrVoidCbkFnType f) {defrStylesEndCbk = f;}
void defrReader::setStylesCbk(defrStylesCbkFnType f) {defrStylesCbk = f;}
void defrReader::setExtensionCbk(defrStringCbkFnType f) {defrExtensionCbk = f;}

void defrReader::setGeomPtr(defiGeometries* p) {defrGeomPtr = p;}
void defrReader::setSubnet(defiSubnet* p) {defrSubnet = p;}

void defrReader::setAssertionWarnings(int warn) {defrAssertionWarnings = warn;}
void defrReader::setBlockageWarnings(int warn) {defrBlockageWarnings = warn;}
void defrReader::setCaseSensitiveWarnings(int warn) {defrCaseSensitiveWarnings = warn;}
void defrReader::setComponentWarnings(int warn) {defrComponentWarnings = warn;}
void defrReader::setConstraintWarnings(int warn) {defrConstraintWarnings = warn;}
void defrReader::setDefaultCapWarnings(int warn) {defrDefaultCapWarnings = warn;}
void defrReader::setFillWarnings(int warn) {defrFillWarnings = warn;}
void defrReader::setGcellGridWarnings(int warn) {defrGcellGridWarnings = warn;}
void defrReader::setIOTimingWarnings(int warn) {defrIOTimingWarnings = warn;}
void defrReader::setNetWarnings(int warn) {defrNetWarnings = warn;}
void defrReader::setNonDefaultWarnings(int warn) {defrNonDefaultWarnings = warn;}
void defrReader::setPinExtWarnings(int warn) {defrPinExtWarnings = warn;}
void defrReader::setPinWarnings(int warn) {defrPinWarnings = warn;}
void defrReader::setRegionWarnings(int warn) {defrRegionWarnings = warn;}
void defrReader::setRowWarnings(int warn) {defrRowWarnings = warn;}
void defrReader::setTrackWarnings(int warn) {defrTrackWarnings = warn;}
void defrReader::setScanchainWarnings(int warn) {defrScanchainWarnings = warn;}
void defrReader::setSNetWarnings(int warn) {defrSNetWarnings = warn;}
void defrReader::setStylesWarnings(int warn) {defrStylesWarnings = warn;}
void defrReader::setUnitsWarnings(int warn) {defrUnitsWarnings = warn;}
void defrReader::setVersionWarnings(int warn) {defrVersionWarnings = warn;}
void defrReader::setViaWarnings(int warn) {defrViaWarnings = warn;}
defrStringCbkFnType defrReader::getDesignCbk() {return defrDesignCbk;}
defrStringCbkFnType defrReader::getTechnologyCbk() {return defrTechnologyCbk;}
defrVoidCbkFnType defrReader::getDesignEndCbk() {return defrDesignEndCbk;}
defrPropCbkFnType defrReader::getPropCbk() {return defrPropCbk;}
defrVoidCbkFnType defrReader::getPropDefEndCbk() {return defrPropDefEndCbk;}
defrVoidCbkFnType defrReader::getPropDefStartCbk() {return defrPropDefStartCbk;}
defrStringCbkFnType defrReader::getArrayNameCbk() {return defrArrayNameCbk;}
defrStringCbkFnType defrReader::getFloorPlanNameCbk() {return defrFloorPlanNameCbk;}
defrDoubleCbkFnType defrReader::getUnitsCbk() {return defrUnitsCbk;}
defrDoubleCbkFnType defrReader::getVersionCbk() {return defrVersionCbk;}
defrStringCbkFnType defrReader::getVersionStrCbk() {return defrVersionStrCbk;}
defrStringCbkFnType defrReader::getDividerCbk() {return defrDividerCbk;}
defrStringCbkFnType defrReader::getBusBitCbk() {return defrBusBitCbk;}
defrSiteCbkFnType defrReader::getSiteCbk() {return defrSiteCbk;}
defrSiteCbkFnType defrReader::getCanplaceCbk() {return defrCanplaceCbk;}
defrSiteCbkFnType defrReader::getCannotOccupyCbk() {return defrCannotOccupyCbk;}
defrIntegerCbkFnType defrReader::getComponentStartCbk() {return defrComponentStartCbk;}
defrVoidCbkFnType defrReader::getComponentEndCbk() {return defrComponentEndCbk;}
defrComponentCbkFnType defrReader::getComponentCbk() {return defrComponentCbk;}
defrIntegerCbkFnType defrReader::getNetStartCbk() {return defrNetStartCbk;}
defrVoidCbkFnType defrReader::getNetEndCbk() {return defrNetEndCbk;}
defrNetCbkFnType defrReader::getNetCbk() {return defrNetCbk;}
defrStringCbkFnType defrReader::getNetNameCbk() {return defrNetNameCbk;}
defrStringCbkFnType defrReader::getNetSubnetNameCbk() {return defrNetSubnetNameCbk;}
defrStringCbkFnType defrReader::getNetNonDefaultRuleCbk() {return defrNetNonDefaultRuleCbk;}
defrNetCbkFnType defrReader::getNetPartialPathCbk() {return defrNetPartialPathCbk;}
defrIntegerCbkFnType defrReader::getSNetStartCbk() {return defrSNetStartCbk;}
defrVoidCbkFnType defrReader::getSNetEndCbk() {return defrSNetEndCbk;}
defrNetCbkFnType defrReader::getSNetCbk() {return defrSNetCbk;}
defrNetCbkFnType defrReader::getSNetPartialPathCbk() {return defrSNetPartialPathCbk;}
defrNetCbkFnType defrReader::getSNetWireCbk() {return defrSNetWireCbk;}
defrPathCbkFnType defrReader::getPathCbk() {return defrPathCbk;}
int defrReader::getAddPathToNet() {return defrAddPathToNet;}
int defrReader::getAllowComponentNets() {return defrAllowComponentNets;}
defrStringCbkFnType defrReader::getComponentExtCbk() {return defrComponentExtCbk;}
defrStringCbkFnType defrReader::getPinExtCbk() {return defrPinExtCbk;}
defrStringCbkFnType defrReader::getViaExtCbk() {return defrViaExtCbk;}
defrStringCbkFnType defrReader::getNetConnectionExtCbk() {return defrNetConnectionExtCbk;}
defrStringCbkFnType defrReader::getNetExtCbk() {return defrNetExtCbk;}
defrStringCbkFnType defrReader::getGroupExtCbk() {return defrGroupExtCbk;}
defrStringCbkFnType defrReader::getScanChainExtCbk() {return defrScanChainExtCbk;}
defrStringCbkFnType defrReader::getIoTimingsExtCbk() {return defrIoTimingsExtCbk;}
defrStringCbkFnType defrReader::getPartitionsExtCbk() {return defrPartitionsExtCbk;}
defrStringCbkFnType defrReader::getHistoryCbk() {return defrHistoryCbk;}
defrBoxCbkFnType defrReader::getDieAreaCbk() {return defrDieAreaCbk;}
defrPinCapCbkFnType defrReader::getPinCapCbk() {return defrPinCapCbk;}
defrVoidCbkFnType defrReader::getPinEndCbk() {return defrPinEndCbk;}
defrIntegerCbkFnType defrReader::getStartPinsCbk() {return defrStartPinsCbk;}
defrIntegerCbkFnType defrReader::getDefaultCapCbk() {return defrDefaultCapCbk;}
defrPinCbkFnType defrReader::getPinCbk() {return defrPinCbk;}
defrRowCbkFnType defrReader::getRowCbk() {return defrRowCbk;}
defrTrackCbkFnType defrReader::getTrackCbk() {return defrTrackCbk;}
defrGcellGridCbkFnType defrReader::getGcellGridCbk() {return defrGcellGridCbk;}
defrIntegerCbkFnType defrReader::getViaStartCbk() {return defrViaStartCbk;}
defrVoidCbkFnType defrReader::getViaEndCbk() {return defrViaEndCbk;}
defrViaCbkFnType defrReader::getViaCbk() {return defrViaCbk;}
defrIntegerCbkFnType defrReader::getRegionStartCbk() {return defrRegionStartCbk;}
defrVoidCbkFnType defrReader::getRegionEndCbk() {return defrRegionEndCbk;}
defrRegionCbkFnType defrReader::getRegionCbk() {return defrRegionCbk;}
defrIntegerCbkFnType defrReader::getGroupsStartCbk() {return defrGroupsStartCbk;}
defrVoidCbkFnType defrReader::getGroupsEndCbk() {return defrGroupsEndCbk;}
defrStringCbkFnType defrReader::getGroupNameCbk() {return defrGroupNameCbk;}
defrStringCbkFnType defrReader::getGroupMemberCbk() {return defrGroupMemberCbk;}
defrComponentMaskShiftLayerCbkFnType defrReader::getComponentMaskShiftLayerCbk() {return defrComponentMaskShiftLayerCbk;}
defrGroupCbkFnType defrReader::getGroupCbk() {return defrGroupCbk;}
defrIntegerCbkFnType defrReader::getAssertionsStartCbk() {return defrAssertionsStartCbk;}
defrVoidCbkFnType defrReader::getAssertionsEndCbk() {return defrAssertionsEndCbk;}
defrAssertionCbkFnType defrReader::getAssertionCbk() {return defrAssertionCbk;}
defrIntegerCbkFnType defrReader::getConstraintsStartCbk() {return defrConstraintsStartCbk;}
defrVoidCbkFnType defrReader::getConstraintsEndCbk() {return defrConstraintsEndCbk;}
defrAssertionCbkFnType defrReader::getConstraintCbk() {return defrConstraintCbk;}
defrIntegerCbkFnType defrReader::getScanchainsStartCbk() {return defrScanchainsStartCbk;}
defrVoidCbkFnType defrReader::getScanchainsEndCbk() {return defrScanchainsEndCbk;}
defrScanchainCbkFnType defrReader::getScanchainCbk() {return defrScanchainCbk;}
defrIntegerCbkFnType defrReader::getIOTimingsStartCbk() {return defrIOTimingsStartCbk;}
defrVoidCbkFnType defrReader::getIOTimingsEndCbk() {return defrIOTimingsEndCbk;}
defrIOTimingCbkFnType defrReader::getIOTimingCbk() {return defrIOTimingCbk;}
defrIntegerCbkFnType defrReader::getFPCStartCbk() {return defrFPCStartCbk;}
defrVoidCbkFnType defrReader::getFPCEndCbk() {return defrFPCEndCbk;}
defrFPCCbkFnType defrReader::getFPCCbk() {return defrFPCCbk;}
defrIntegerCbkFnType defrReader::getTimingDisablesStartCbk() {return defrTimingDisablesStartCbk;}
defrVoidCbkFnType defrReader::getTimingDisablesEndCbk() {return defrTimingDisablesEndCbk;}
defrTimingDisableCbkFnType defrReader::getTimingDisableCbk() {return defrTimingDisableCbk;}
defrIntegerCbkFnType defrReader::getPartitionsStartCbk() {return defrPartitionsStartCbk;}
defrVoidCbkFnType defrReader::getPartitionsEndCbk() {return defrPartitionsEndCbk;}
defrPartitionCbkFnType defrReader::getPartitionCbk() {return defrPartitionCbk;}
defrIntegerCbkFnType defrReader::getPinPropStartCbk() {return defrPinPropStartCbk;}
defrVoidCbkFnType defrReader::getPinPropEndCbk() {return defrPinPropEndCbk;}
defrPinPropCbkFnType defrReader::getPinPropCbk() {return defrPinPropCbk;}
defrIntegerCbkFnType defrReader::getCaseSensitiveCbk() {return defrCaseSensitiveCbk;}
defrIntegerCbkFnType defrReader::getBlockageStartCbk() {return defrBlockageStartCbk;}
defrVoidCbkFnType defrReader::getBlockageEndCbk() {return defrBlockageEndCbk;}
defrBlockageCbkFnType defrReader::getBlockageCbk() {return defrBlockageCbk;}
defrIntegerCbkFnType defrReader::getSlotStartCbk() {return defrSlotStartCbk;}
defrVoidCbkFnType defrReader::getSlotEndCbk() {return defrSlotEndCbk;}
defrSlotCbkFnType defrReader::getSlotCbk() {return defrSlotCbk;}
defrIntegerCbkFnType defrReader::getFillStartCbk() {return defrFillStartCbk;}
defrVoidCbkFnType defrReader::getFillEndCbk() {return defrFillEndCbk;}
defrFillCbkFnType defrReader::getFillCbk() {return defrFillCbk;}
defrIntegerCbkFnType defrReader::getNonDefaultStartCbk() {return defrNonDefaultStartCbk;}
defrVoidCbkFnType defrReader::getNonDefaultEndCbk() {return defrNonDefaultEndCbk;}
defrNonDefaultCbkFnType defrReader::getNonDefaultCbk() {return defrNonDefaultCbk;}
defrIntegerCbkFnType defrReader::getStylesStartCbk() {return defrStylesStartCbk;}
defrVoidCbkFnType defrReader::getStylesEndCbk() {return defrStylesEndCbk;}
defrStylesCbkFnType defrReader::getStylesCbk() {return defrStylesCbk;}
defrStringCbkFnType defrReader::getExtensionCbk() {return defrExtensionCbk;}

defiSubnet* defrReader::getSubnet() {return defrSubnet;}
defiPath* defrReader::getPath() {return defrPath;}
defiProp& defrReader::getProp() {return defrProp;}
defiSite& defrReader::getSite() {return defrSite;}
defiComponent& defrReader::getComponent() {return defrComponent;}
defiComponentMaskShiftLayer& defrReader::getComponentMaskShiftLayer() { return defrComponentMaskShiftLayer;}
defiNet& defrReader::getNet() {return defrNet;}
defiPinCap& defrReader::getPinCap() {return defrPinCap;}
defiSite& defrReader::getCannotOccupy() {return defrCannotOccupy;}
defiSite& defrReader::getCanplace() {return defrCanplace;}
defiBox& defrReader::getDieArea() {return defrDieArea;}
defiPin& defrReader::getPin() {return defrPin;}
defiRow& defrReader::getRow() {return defrRow;}
defiTrack& defrReader::getTrack() {return defrTrack;}
defiGcellGrid& defrReader::getGcellGrid() {return defrGcellGrid;}
defiVia& defrReader::getVia() {return defrVia;}
defiRegion& defrReader::getRegion() {return defrRegion;}
defiGroup& defrReader::getGroup() {return defrGroup;}
defiAssertion& defrReader::getAssertion() {return defrAssertion;}
defiScanchain& defrReader::getScanchain() {return defrScanchain;}
defiIOTiming& defrReader::getIOTiming() {return defrIOTiming;}
defiFPC& defrReader::getFPC() {return defrFPC;}
defiTimingDisable& defrReader::getTimingDisable() {return defrTimingDisable;}
defiPartition& defrReader::getPartition() {return defrPartition;}
defiPinProp& defrReader::getPinProp() {return defrPinProp;}
defiBlockage& defrReader::getBlockage() {return defrBlockage;}
defiSlot& defrReader::getSlot() {return defrSlot;}
defiFill& defrReader::getFill() {return defrFill;}
defiNonDefault& defrReader::getNonDefault() {return defrNonDefault;}
defiStyles& defrReader::getStyles() {return defrStyles;}
defiGeometries* defrReader::getGeomPtr() {return defrGeomPtr;}
defiPropType& defrReader::getCompProp() {return defrCompProp;}
defiPropType& defrReader::getCompPinProp() {return defrCompPinProp;}
defiPropType& defrReader::getDesignProp() {return defrDesignProp;}
defiPropType& defrReader::getGroupProp() {return defrGroupProp;}
defiPropType& defrReader::getNDefProp() {return defrNDefProp;}
defiPropType& defrReader::getNetProp() {return defrNetProp;}
defiPropType& defrReader::getPinDefProp() {return defrPinDefProp;}
defiPropType& defrReader::getRegionProp() {return defrRegionProp;}
defiPropType& defrReader::getRowProp() {return defrRowProp;}
defiPropType& defrReader::getSNetProp() {return defrSNetProp;}

int defrReader::getAssertionWarnings() {return defrAssertionWarnings;}
int defrReader::getBlockageWarnings() {return defrBlockageWarnings;}
int defrReader::getCaseSensitiveWarnings() {return defrCaseSensitiveWarnings;}
int defrReader::getComponentWarnings() {return defrComponentWarnings;}
int defrReader::getConstraintWarnings() {return defrConstraintWarnings;}
int defrReader::getDefaultCapWarnings() {return defrDefaultCapWarnings;}
int defrReader::getFillWarnings() {return defrFillWarnings;}
int defrReader::getGcellGridWarnings() {return defrGcellGridWarnings;}
int defrReader::getIOTimingWarnings() {return defrIOTimingWarnings;}
int defrReader::getNetWarnings() {return defrNetWarnings;}
int defrReader::getNonDefaultWarnings() {return defrNonDefaultWarnings;}
int defrReader::getPinExtWarnings() {return defrPinExtWarnings;}
int defrReader::getPinWarnings() {return defrPinWarnings;}
int defrReader::getRegionWarnings() {return defrRegionWarnings;}
int defrReader::getRowWarnings() {return defrRowWarnings;}
int defrReader::getTrackWarnings() {return defrTrackWarnings;}
int defrReader::getScanchainWarnings() {return defrScanchainWarnings;}
int defrReader::getSNetWarnings() {return defrSNetWarnings;}
int defrReader::getStylesWarnings() {return defrStylesWarnings;}
int defrReader::getUnitsWarnings() {return defrUnitsWarnings;}
int defrReader::getVersionWarnings() {return defrVersionWarnings;}
int defrReader::getViaWarnings() {return defrViaWarnings;}

FILE* defrReader::getFile() {return defrFile;}
const char* defrReader::getFileName() {return defrFileName;}
int defrReader::addPathToNet() {return defrAddPathToNet;}
int defrReader::needPathData() {return defrNeedPathData;}
int defrReader::isReset() {return defrIsReset;}
int defrReader::isDoneDesign() {return doneDesign;}
void defrReader::setDoneDesign(int d) {doneDesign = d;}
void defrReader::incrUnusedCallbacks(int i) {defrUnusedCallbacks[i] += 1;}
int* defrReader::getUnusedCallbacks() {return defrUnusedCallbacks;}

void defrReader::defrDisableParserMsgs (int nMsg, int* msgs) {
  int i, j;
  int *tmp;

  if (nDDMsgs == 0) {  /* new list */
     nDDMsgs = nMsg;
     disableDMsgs = (int*)defMalloc(sizeof(int)*nMsg);
     for (i = 0; i < nMsg; i++)
        disableDMsgs[i] = msgs[i];
  } else {  /* add the list to the existing list */
     /* 1st check if the msgId is already on the list before adding it on */
     tmp = (int*)defMalloc(sizeof(int)*(nMsg + nDDMsgs));
     for (i = 0; i < nDDMsgs; i++)  /* copy the existing to the new list */
        tmp[i] = disableDMsgs[i];
     defFree((int*)(disableDMsgs));
     disableDMsgs = tmp;           /* set disableDMsgs to the new list */
     for (i = 0; i < nMsg; i++) { /* merge the new list with the existing */
        for (j = 0; j < nDDMsgs; j++) {
           if (disableDMsgs[j] == msgs[i])
               break;             /* msgId already on the list */
        }
        if (j == nDDMsgs)           /* msgId not on the list, add it on */
           disableDMsgs[nDDMsgs++] = msgs[i];
     }
  }
  return;
}

void defrReader::defrEnableParserMsgs (int nMsg, int* msgs) {
  int i, j;
  int *tmp;

  if (nDDMsgs == 0)
     return;                       /* list is empty, nothing to remove */

  for (i = 0; i < nMsg; i++) {     /* loop through the given list */
     for (j = 0; j < nDDMsgs; j++) {
        if (disableDMsgs[j] == msgs[i]) {
           disableDMsgs[j] = -1;    /* temp assign a -1 on that slot */
           break;
        }
     }
  }
  /* fill up the empty slot with the next non -1 msgId */
  for (i = 0; i < nDDMsgs; i++) {
     if (disableDMsgs[i] == -1) {
        j = i + 1;
        while (j < nDDMsgs) {
           if (disableDMsgs[j] != -1)
              disableDMsgs[i++] = disableDMsgs[j++];
        }
        break;     /* break out the for loop, the list should all moved */
     }
  }
  /* Count how many messageId left and change all -1 to 0 */
  for (j = i; j < nDDMsgs; j++) {
      disableDMsgs[j] = 0;     /* set to 0 */
  }
  nDDMsgs = i;
  return;
}

void defrReader::defrEnableAllMsgs () {
  nDDMsgs = 0;
  defFree((int*)(disableDMsgs));
  return;
}

void defrReader::defrSetTotalMsgLimit (int totNumMsgs) {
  totalDefMsgLimit = totNumMsgs;
  return;
}

void defrReader::defrSetLimitPerMsg (int msgId, int numMsg) {
  char msgStr[10];

  if ((msgId <= 0) || ((msgId-5000) >= NODEFMSG)) {   // Def starts at 5000
     sprintf(msgStr, "%d", msgId);
     defError(204, msgStr);
     return;
  }
  defMsgLimit[0][msgId-5000] = numMsg;
  return;
}

// *****************************************************************
// Wrapper functions.
//
// These functions provide access to the class member functions
// for compatibility with previous parser kits.
// *****************************************************************
int defrInit PROTO_PARAMS(( void ))
{
    return defrReader::get()->defrInit();
}

int defrReset PROTO_PARAMS(( void ))
{
    return defrReader::get()->defrReset();
}

void defrInitGlobals PROTO_PARAMS(( void ))
{
    defrReader::get()->defrInitGlobals();
}

void defrSetRegisterUnusedCallbacks PROTO_PARAMS(( void ))
{
    defrReader::get()->defrSetRegisterUnusedCallbacks();
}

void defrSetUnusedCallbacks PROTO_PARAMS(( defrVoidCbkFnType f ))
{
    defrReader::get()->defrSetUnusedCallbacks(f);
}

void defrUnsetCallbacks PROTO_PARAMS(( void ))
{
    defrReader::get()->defrUnsetCallbacks();
}

void defrPrintUnusedCallbacks PROTO_PARAMS(( FILE* log ))
{
    defrReader::get()->defrPrintUnusedCallbacks(log);
}

int defrReleaseNResetMemory PROTO_PARAMS(( void ))
{
    return defrReader::get()->defrReleaseNResetMemory();
}

void
defrUnsetArrayNameCbk()
{
    defrReader::get()->setArrayNameCbk(0);
}

void
defrUnsetAssertionCbk()
{
    defrReader::get()->setAssertionCbk(0);
}

void
defrUnsetAssertionsStartCbk()
{
    defrReader::get()->setAssertionsStartCbk(0);
}

void
defrUnsetAssertionsEndCbk()
{
    defrReader::get()->setAssertionsEndCbk(0);
}

void
defrUnsetBlockageCbk()
{
    defrReader::get()->setBlockageCbk(0);
}

void
defrUnsetBlockageStartCbk()
{
    defrReader::get()->setBlockageStartCbk(0);
}

void
defrUnsetBlockageEndCbk()
{
    defrReader::get()->setBlockageEndCbk(0);
}

void
defrUnsetBusBitCbk()
{
    defrReader::get()->setBusBitCbk(0);
}

void
defrUnsetCannotOccupyCbk()
{
    defrReader::get()->setCannotOccupyCbk(0);
}

void
defrUnsetCanplaceCbk()
{
    defrReader::get()->setCanplaceCbk(0);
}

void
defrUnsetCaseSensitiveCbk()
{
    defrReader::get()->setCaseSensitiveCbk(0);
}

void
defrUnsetComponentCbk()
{
    defrReader::get()->setComponentCbk(0);
}

void
defrUnsetComponentExtCbk()
{
    defrReader::get()->setComponentExtCbk(0);
}

void
defrUnsetComponentStartCbk()
{
    defrReader::get()->setComponentStartCbk(0);
}

void
defrUnsetComponentEndCbk()
{
    defrReader::get()->setComponentEndCbk(0);
}

void
defrUnsetConstraintCbk()
{
    defrReader::get()->setConstraintCbk(0);
}

void
defrUnsetConstraintsStartCbk()
{
    defrReader::get()->setConstraintsStartCbk(0);
}

void
defrUnsetConstraintsEndCbk()
{
    defrReader::get()->setConstraintsEndCbk(0);
}

void
defrUnsetDefaultCapCbk()
{
    defrReader::get()->setDefaultCapCbk(0);
}

void
defrUnsetDesignCbk()
{
    defrReader::get()->setDesignCbk(0);
}

void
defrUnsetDesignEndCbk()
{
    defrReader::get()->setDesignEndCbk(0);
}

void
defrUnsetDieAreaCbk()
{
    defrReader::get()->setDieAreaCbk(0);
}

void
defrUnsetDividerCbk()
{
    defrReader::get()->setDividerCbk(0);
}

void
defrUnsetExtensionCbk()
{
    defrReader::get()->setExtensionCbk(0);
}

void
defrUnsetFillCbk()
{
    defrReader::get()->setFillCbk(0);
}

void
defrUnsetFillStartCbk()
{
    defrReader::get()->setFillStartCbk(0);
}

void
defrUnsetFillEndCbk()
{
    defrReader::get()->setFillEndCbk(0);
}

void
defrUnsetFPCCbk()
{
    defrReader::get()->setFPCCbk(0);
}

void
defrUnsetFPCStartCbk()
{
    defrReader::get()->setFPCStartCbk(0);
}

void
defrUnsetFPCEndCbk()
{
    defrReader::get()->setFPCEndCbk(0);
}

void
defrUnsetFloorPlanNameCbk()
{
    defrReader::get()->setFloorPlanNameCbk(0);
}

void
defrUnsetGcellGridCbk()
{
    defrReader::get()->setGcellGridCbk(0);
}

void
defrUnsetGroupCbk()
{
    defrReader::get()->setGroupCbk(0);
}

void
defrUnsetGroupExtCbk()
{
    defrReader::get()->setGroupExtCbk(0);
}

void
defrUnsetGroupMemberCbk()
{
    defrReader::get()->setGroupMemberCbk(0);
}

void
defrUnsetComponentMaskShiftLayerCbk()
{
    defrReader::get()->setComponentMaskShiftLayerCbk(0);
}

void
defrUnsetGroupNameCbk()
{
    defrReader::get()->setGroupNameCbk(0);
}

void
defrUnsetGroupsStartCbk()
{
    defrReader::get()->setGroupsStartCbk(0);
}

void
defrUnsetGroupsEndCbk()
{
    defrReader::get()->setGroupsEndCbk(0);
}

void
defrUnsetHistoryCbk()
{
    defrReader::get()->setHistoryCbk(0);
}

void
defrUnsetIOTimingCbk()
{
    defrReader::get()->setIOTimingCbk(0);
}

void
defrUnsetIOTimingsStartCbk()
{
    defrReader::get()->setIOTimingsStartCbk(0);
}

void
defrUnsetIOTimingsEndCbk()
{
    defrReader::get()->setIOTimingsEndCbk(0);
}

void
defrUnsetIOTimingsExtCbk()
{
    defrReader::get()->setIoTimingsExtCbk(0);
}

void
defrUnsetNetCbk()
{
    defrReader::get()->setNetCbk(0);
}

void
defrUnsetNetNameCbk()
{
    defrReader::get()->setNetNameCbk(0);
}

void
defrUnsetNetNonDefaultRuleCbk()
{
    defrReader::get()->setNetNonDefaultRuleCbk(0);
}

void
defrUnsetNetConnectionExtCbk()
{
    defrReader::get()->setNetConnectionExtCbk(0);
}

void
defrUnsetNetExtCbk()
{
    defrReader::get()->setNetExtCbk(0);
}

void
defrUnsetNetPartialPathCbk()
{
    defrReader::get()->setNetPartialPathCbk(0);
}

void
defrUnsetNetSubnetNameCbk()
{
    defrReader::get()->setNetSubnetNameCbk(0);
}

void
defrUnsetNetStartCbk()
{
    defrReader::get()->setNetStartCbk(0);
}

void
defrUnsetNetEndCbk()
{
    defrReader::get()->setNetEndCbk(0);
}

void
defrUnsetNonDefaultCbk()
{
    defrReader::get()->setNonDefaultCbk(0);
}

void
defrUnsetNonDefaultStartCbk()
{
    defrReader::get()->setNonDefaultStartCbk(0);
}

void
defrUnsetNonDefaultEndCbk()
{
    defrReader::get()->setNonDefaultEndCbk(0);
}

void
defrUnsetPartitionCbk()
{
    defrReader::get()->setPartitionCbk(0);
}

void
defrUnsetPartitionsExtCbk()
{
    defrReader::get()->setPartitionsExtCbk(0);
}

void
defrUnsetPartitionsStartCbk()
{
    defrReader::get()->setPartitionsStartCbk(0);
}

void
defrUnsetPartitionsEndCbk()
{
    defrReader::get()->setPartitionsEndCbk(0);
}

void
defrUnsetPathCbk()
{
    defrReader::get()->setPathCbk(0);
}

void
defrUnsetPinCapCbk()
{
    defrReader::get()->setPinCapCbk(0);
}

void
defrUnsetPinCbk()
{
    defrReader::get()->setPinCbk(0);
}

void
defrUnsetPinEndCbk()
{
    defrReader::get()->setPinEndCbk(0);
}

void
defrUnsetPinExtCbk()
{
    defrReader::get()->setPinExtCbk(0);
}

void
defrUnsetPinPropCbk()
{
    defrReader::get()->setPinPropCbk(0);
}

void
defrUnsetPinPropStartCbk()
{
    defrReader::get()->setPinPropStartCbk(0);
}

void
defrUnsetPinPropEndCbk()
{
    defrReader::get()->setPinPropEndCbk(0);
}

void
defrUnsetPropCbk()
{
    defrReader::get()->setPropCbk(0);
}

void
defrUnsetPropDefEndCbk()
{
    defrReader::get()->setPropDefEndCbk(0);
}

void
defrUnsetPropDefStartCbk()
{
    defrReader::get()->setPropDefStartCbk(0);
}

void
defrUnsetRegionCbk()
{
    defrReader::get()->setRegionCbk(0);
}

void
defrUnsetRegionStartCbk()
{
    defrReader::get()->setRegionStartCbk(0);
}

void
defrUnsetRegionEndCbk()
{
    defrReader::get()->setRegionEndCbk(0);
}

void
defrUnsetRowCbk()
{
    defrReader::get()->setRowCbk(0);
}

void
defrUnsetScanChainExtCbk()
{
    defrReader::get()->setScanChainExtCbk(0);
}

void
defrUnsetScanchainCbk()
{
    defrReader::get()->setScanchainCbk(0);
}

void
defrUnsetScanchainsStartCbk()
{
    defrReader::get()->setScanchainsStartCbk(0);
}

void
defrUnsetScanchainsEndCbk()
{
    defrReader::get()->setScanchainsEndCbk(0);
}

void
defrUnsetSiteCbk()
{
    defrReader::get()->setSiteCbk(0);
}

void
defrUnsetSlotCbk()
{
    defrReader::get()->setSlotCbk(0);
}

void
defrUnsetSlotStartCbk()
{
    defrReader::get()->setSlotStartCbk(0);
}

void
defrUnsetSlotEndCbk()
{
    defrReader::get()->setSlotEndCbk(0);
}

void
defrUnsetSNetWireCbk()
{
    defrReader::get()->setSNetWireCbk(0);
}

void
defrUnsetSNetCbk()
{
    defrReader::get()->setSNetCbk(0);
}

void
defrUnsetSNetStartCbk()
{
    defrReader::get()->setSNetStartCbk(0);
}

void
defrUnsetSNetEndCbk()
{
    defrReader::get()->setSNetEndCbk(0);
}

void
defrUnsetSNetPartialPathCbk()
{
    defrReader::get()->setSNetPartialPathCbk(0);
}

void
defrUnsetStartPinsCbk()
{
    defrReader::get()->setStartPinsCbk(0);
}

void
defrUnsetStylesCbk()
{
    defrReader::get()->setStylesCbk(0);
}

void
defrUnsetStylesStartCbk()
{
    defrReader::get()->setStylesStartCbk(0);
}

void
defrUnsetStylesEndCbk()
{
    defrReader::get()->setStylesEndCbk(0);
}

void
defrUnsetTechnologyCbk()
{
    defrReader::get()->setTechnologyCbk(0);
}

void
defrUnsetTimingDisableCbk()
{
    defrReader::get()->setTimingDisableCbk(0);
}

void
defrUnsetTimingDisablesStartCbk()
{
    defrReader::get()->setTimingDisablesStartCbk(0);
}

void
defrUnsetTimingDisablesEndCbk()
{
    defrReader::get()->setTimingDisablesEndCbk(0);
}

void
defrUnsetTrackCbk()
{
    defrReader::get()->setTrackCbk(0);
}

void
defrUnsetUnitsCbk()
{
    defrReader::get()->setUnitsCbk(0);
}

void
defrUnsetVersionCbk()
{
    defrReader::get()->setVersionCbk(0);
}

void
defrUnsetVersionStrCbk()
{
    defrReader::get()->setVersionStrCbk(0);
}

void
defrUnsetViaCbk()
{
    defrReader::get()->setViaCbk(0);
}

void
defrUnsetViaExtCbk()
{
    defrReader::get()->setViaExtCbk(0);
}

void
defrUnsetViaStartCbk()
{
    defrReader::get()->setViaStartCbk(0);
}


void
defrUnsetViaEndCbk()
{
    defrReader::get()->setViaEndCbk(0);
}

int *
defUnusedCallbackCount()
{
    return defrReader::get()->getUnusedCallbacks();
}


const char *
defrFName()
{
    return defrReader::get()->getFileName();
}


int
defrRead(FILE           *f,
         const char     *fName,
         defiUserData   uData,
         int            case_sensitive)
{
    return defrReader::get()->defrRead(f, fName, uData, case_sensitive);
}

void
defrSetUserData(defiUserData d)
{
    defrReader::get()->setUserData(d);
}


defiUserData
defrGetUserData()
{
    return defrReader::get()->getUserData();
}


void
defrSetDesignCbk(defrStringCbkFnType f)
{
    defrReader::get()->setDesignCbk(f);
}


void
defrSetTechnologyCbk(defrStringCbkFnType f)
{
    defrReader::get()->setTechnologyCbk(f);
}


void
defrSetDesignEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setDesignEndCbk(f);
}


void
defrSetPropCbk(defrPropCbkFnType f)
{
    defrReader::get()->setPropCbk(f);
}


void
defrSetPropDefEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setPropDefEndCbk(f);
}


void
defrSetPropDefStartCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setPropDefStartCbk(f);
}


void
defrSetArrayNameCbk(defrStringCbkFnType f)
{
    defrReader::get()->setArrayNameCbk(f);
}


void
defrSetFloorPlanNameCbk(defrStringCbkFnType f)
{
    defrReader::get()->setFloorPlanNameCbk(f);
}


void
defrSetUnitsCbk(defrDoubleCbkFnType f)
{
    defrReader::get()->setUnitsCbk(f);
}


void
defrSetVersionCbk(defrDoubleCbkFnType f)
{
    defrReader::get()->setVersionCbk(f);
}


void
defrSetVersionStrCbk(defrStringCbkFnType f)
{
    defrReader::get()->setVersionStrCbk(f);
}


void
defrSetDividerCbk(defrStringCbkFnType f)
{
    defrReader::get()->setDividerCbk(f);
}


void
defrSetBusBitCbk(defrStringCbkFnType f)
{
    defrReader::get()->setBusBitCbk(f);
}


void
defrSetSiteCbk(defrSiteCbkFnType f)
{
    defrReader::get()->setSiteCbk(f);
}


void
defrSetCanplaceCbk(defrSiteCbkFnType f)
{
    defrReader::get()->setCanplaceCbk(f);
}


void
defrSetCannotOccupyCbk(defrSiteCbkFnType f)
{
    defrReader::get()->setCannotOccupyCbk(f);
}


void
defrSetComponentStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setComponentStartCbk(f);
}


void
defrSetComponentEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setComponentEndCbk(f);
}

void
defrSetComponentCbk(defrComponentCbkFnType f)
{
    defrReader::get()->setComponentCbk(f);
}

void
defrSetNetStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setNetStartCbk(f);
}

void
defrSetNetEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setNetEndCbk(f);
}

void
defrSetNetCbk(defrNetCbkFnType f)
{
    defrReader::get()->setNetCbk(f);
}

void
defrSetNetNameCbk(defrStringCbkFnType f)
{
    defrReader::get()->setNetNameCbk(f);
}

void
defrSetNetSubnetNameCbk(defrStringCbkFnType f)
{
    defrReader::get()->setNetSubnetNameCbk(f);
}

void
defrSetNetNonDefaultRuleCbk(defrStringCbkFnType f)
{
    defrReader::get()->setNetNonDefaultRuleCbk(f);
}

void
defrSetNetPartialPathCbk(defrNetCbkFnType f)
{
    defrReader::get()->setNetPartialPathCbk(f);
}

void
defrSetSNetStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setSNetStartCbk(f);
}

void
defrSetSNetEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setSNetEndCbk(f);
}

void
defrSetSNetCbk(defrNetCbkFnType f)
{
    defrReader::get()->setSNetCbk(f);
}

void
defrSetSNetPartialPathCbk(defrNetCbkFnType f)
{
    defrReader::get()->setSNetPartialPathCbk(f);
}

void
defrSetSNetWireCbk(defrNetCbkFnType f)
{
    defrReader::get()->setSNetWireCbk(f);
}

void
defrSetPathCbk(defrPathCbkFnType f)
{
    defrReader::get()->setPathCbk(f);
}

void
defrSetAddPathToNet()
{
    defrReader::get()->setAddPathToNet();
}

void
defrSetAllowComponentNets()
{
    defrReader::get()->setAllowComponentNets();
}

void
defrSetComponentExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setComponentExtCbk(f);
}

void
defrSetPinExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setPinExtCbk(f);
}

void
defrSetViaExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setViaExtCbk(f);
}

void
defrSetNetConnectionExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setNetConnectionExtCbk(f);
}

void
defrSetNetExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setNetExtCbk(f);
}

void
defrSetGroupExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setGroupExtCbk(f);
}

void
defrSetScanChainExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setScanChainExtCbk(f);
}

void
defrSetIoTimingsExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setIoTimingsExtCbk(f);
}

void
defrSetPartitionsExtCbk(defrStringCbkFnType f)
{
    defrReader::get()->setPartitionsExtCbk(f);
}

void
defrSetHistoryCbk(defrStringCbkFnType f)
{
    defrReader::get()->setHistoryCbk(f);
}

void
defrSetDieAreaCbk(defrBoxCbkFnType f)
{
    defrReader::get()->setDieAreaCbk(f);
}

void
defrSetPinCapCbk(defrPinCapCbkFnType f)
{
    defrReader::get()->setPinCapCbk(f);
}

void
defrSetPinEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setPinEndCbk(f);
}

void
defrSetStartPinsCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setStartPinsCbk(f);
}

void
defrSetDefaultCapCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setDefaultCapCbk(f);
}

void
defrSetPinCbk(defrPinCbkFnType f)
{
    defrReader::get()->setPinCbk(f);
}

void
defrSetRowCbk(defrRowCbkFnType f)
{
    defrReader::get()->setRowCbk(f);
}

void
defrSetTrackCbk(defrTrackCbkFnType f)
{
    defrReader::get()->setTrackCbk(f);
}

void
defrSetGcellGridCbk(defrGcellGridCbkFnType f)
{
    defrReader::get()->setGcellGridCbk(f);
}

void
defrSetViaStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setViaStartCbk(f);
}

void
defrSetViaEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setViaEndCbk(f);
}

void
defrSetViaCbk(defrViaCbkFnType f)
{
    defrReader::get()->setViaCbk(f);
}

void
defrSetRegionStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setRegionStartCbk(f);
}

void
defrSetRegionEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setRegionEndCbk(f);
}

void
defrSetRegionCbk(defrRegionCbkFnType f)
{
    defrReader::get()->setRegionCbk(f);
}

void
defrSetGroupsStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setGroupsStartCbk(f);
}

void
defrSetGroupsEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setGroupsEndCbk(f);
}

void
defrSetGroupNameCbk(defrStringCbkFnType f)
{
    defrReader::get()->setGroupNameCbk(f);
}

void
defrSetGroupMemberCbk(defrStringCbkFnType f)
{
    defrReader::get()->setGroupMemberCbk(f);
}

void
defrSetComponentMaskShiftLayerCbk(defrComponentMaskShiftLayerCbkFnType f)
{
    defrReader::get()->setComponentMaskShiftLayerCbk(f);
}

void
defrSetGroupCbk(defrGroupCbkFnType f)
{
    defrReader::get()->setGroupCbk(f);
}

void
defrSetAssertionsStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setAssertionsStartCbk(f);
}

void
defrSetAssertionsEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setAssertionsEndCbk(f);
}

void
defrSetAssertionCbk(defrAssertionCbkFnType f)
{
    defrReader::get()->setAssertionCbk(f);
}

void
defrSetConstraintsStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setConstraintsStartCbk(f);
}

void
defrSetConstraintsEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setConstraintsEndCbk(f);
}

void
defrSetConstraintCbk(defrAssertionCbkFnType f)
{
    defrReader::get()->setConstraintCbk(f);
}

void
defrSetScanchainsStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setScanchainsStartCbk(f);
}

void
defrSetScanchainsEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setScanchainsEndCbk(f);
}

void
defrSetScanchainCbk(defrScanchainCbkFnType f)
{
    defrReader::get()->setScanchainCbk(f);
}

void
defrSetIOTimingsStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setIOTimingsStartCbk(f);
}

void
defrSetIOTimingsEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setIOTimingsEndCbk(f);
}

void
defrSetIOTimingCbk(defrIOTimingCbkFnType f)
{
    defrReader::get()->setIOTimingCbk(f);
}

void
defrSetFPCStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setFPCStartCbk(f);
}

void
defrSetFPCEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setFPCEndCbk(f);
}

void
defrSetFPCCbk(defrFPCCbkFnType f)
{
    defrReader::get()->setFPCCbk(f);
}

void
defrSetTimingDisablesStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setTimingDisablesStartCbk(f);
}

void
defrSetTimingDisablesEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setTimingDisablesEndCbk(f);
}

void
defrSetTimingDisableCbk(defrTimingDisableCbkFnType f)
{
    defrReader::get()->setTimingDisableCbk(f);
}

void
defrSetPartitionsStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setPartitionsStartCbk(f);
}

void
defrSetPartitionsEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setPartitionsEndCbk(f);
}

void
defrSetPartitionCbk(defrPartitionCbkFnType f)
{
    defrReader::get()->setPartitionCbk(f);
}

void
defrSetPinPropStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setPinPropStartCbk(f);
}

void
defrSetPinPropEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setPinPropEndCbk(f);
}

void
defrSetPinPropCbk(defrPinPropCbkFnType f)
{
    defrReader::get()->setPinPropCbk(f);
}

void
defrSetCaseSensitiveCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setCaseSensitiveCbk(f);
}

void
defrSetBlockageStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setBlockageStartCbk(f);
}

void
defrSetBlockageEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setBlockageEndCbk(f);
}

void
defrSetBlockageCbk(defrBlockageCbkFnType f)
{
    defrReader::get()->setBlockageCbk(f);
}

void
defrSetSlotStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setSlotStartCbk(f);
}

void
defrSetSlotEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setSlotEndCbk(f);
}

void
defrSetSlotCbk(defrSlotCbkFnType f)
{
    defrReader::get()->setSlotCbk(f);
}

void
defrSetFillStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setFillStartCbk(f);
}

void
defrSetFillEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setFillEndCbk(f);
}

void
defrSetFillCbk(defrFillCbkFnType f)
{
    defrReader::get()->setFillCbk(f);
}

void
defrSetNonDefaultStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setNonDefaultStartCbk(f);
}

void
defrSetNonDefaultEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setNonDefaultEndCbk(f);
}

void
defrSetNonDefaultCbk(defrNonDefaultCbkFnType f)
{
    defrReader::get()->setNonDefaultCbk(f);
}

void
defrSetStylesStartCbk(defrIntegerCbkFnType f)
{
    defrReader::get()->setStylesStartCbk(f);
}

void
defrSetStylesEndCbk(defrVoidCbkFnType f)
{
    defrReader::get()->setStylesEndCbk(f);
}

void
defrSetStylesCbk(defrStylesCbkFnType f)
{
    defrReader::get()->setStylesCbk(f);
}

void
defrSetExtensionCbk(defrStringCbkFnType f)
{
    defrReader::get()->setExtensionCbk(f);
}



/* NEW CALLBACK - Put the set functions for the new callbacks here. */



void defrSetAssertionWarnings(int warn)
{
  defrReader::get()->setAssertionWarnings(warn);
}


void defrSetBlockageWarnings(int warn)
{
  defrReader::get()->setBlockageWarnings(warn);
}


void defrSetCaseSensitiveWarnings(int warn)
{
  defrReader::get()->setCaseSensitiveWarnings(warn);
}


void defrSetComponentWarnings(int warn)
{
  defrReader::get()->setComponentWarnings(warn);
}


void defrSetConstraintWarnings(int warn)
{
  defrReader::get()->setConstraintWarnings(warn);
}


void defrSetDefaultCapWarnings(int warn)
{
  defrReader::get()->setDefaultCapWarnings(warn);
}


void defrSetGcellGridWarnings(int warn)
{
  defrReader::get()->setGcellGridWarnings(warn);
}


void defrSetIOTimingWarnings(int warn)
{
  defrReader::get()->setIOTimingWarnings(warn);
}


void defrSetNetWarnings(int warn)
{
  defrReader::get()->setNetWarnings(warn);
}


void defrSetNonDefaultWarnings(int warn)
{
  defrReader::get()->setNonDefaultWarnings(warn);
}


void defrSetPinExtWarnings(int warn)
{
  defrReader::get()->setPinExtWarnings(warn);
}


void defrSetPinWarnings(int warn)
{
  defrReader::get()->setPinWarnings(warn);
}


void defrSetRegionWarnings(int warn)
{
  defrReader::get()->setRegionWarnings(warn);
}


void defrSetRowWarnings(int warn)
{
  defrReader::get()->setRowWarnings(warn);
}


void defrSetScanchainWarnings(int warn)
{
  defrReader::get()->setScanchainWarnings(warn);
}


void defrSetSNetWarnings(int warn)
{
  defrReader::get()->setSNetWarnings(warn);
}


void defrSetStylesWarnings(int warn)
{
  defrReader::get()->setStylesWarnings(warn);
}


void defrSetTrackWarnings(int warn)
{
  defrReader::get()->setTrackWarnings(warn);
}


void defrSetUnitsWarnings(int warn)
{
  defrReader::get()->setUnitsWarnings(warn);
}


void defrSetVersionWarnings(int warn)
{
  defrReader::get()->setVersionWarnings(warn);
}


void defrSetViaWarnings(int warn)
{
  defrReader::get()->setViaWarnings(warn);
}


void defrDisableParserMsgs(int nMsg, int* msgs)
{
  defrReader::get()->defrDisableParserMsgs(nMsg, msgs);
}


void defrEnableParserMsgs(int nMsg, int* msgs)
{
  defrReader::get()->defrEnableParserMsgs(nMsg, msgs);
}


void defrEnableAllMsgs()
{
  defrReader::get()->defrEnableAllMsgs();
}


void defrSetTotalMsgLimit(int totNumMsgs)
{
  defrReader::get()->defrSetTotalMsgLimit(totNumMsgs);
}


void defrSetLimitPerMsg(int msgId, int numMsg)
{
  defrReader::get()->defrSetLimitPerMsg(msgId, numMsg);
}


// *****************************************************************
// Utility functions
//
// These are utility functions. Note: this part still contains some
// global variables. Ideally they would be part of the main class.
// *****************************************************************
extern DEFI_MAGIC_COMMENT_FOUND_FUNCTION  defiMagicCommentFoundFunction;

void
defrSetMagicCommentFoundFunction(DEFI_MAGIC_COMMENT_FOUND_FUNCTION f)
{
    defiMagicCommentFoundFunction = f;
}

extern char *defiMagicCommentString;

void
defrSetMagicCommentString(char *s)
{
    defiMagicCommentString = strdup(s);
}

extern char *defiMagicCommentString;

extern DEFI_LOG_FUNCTION            defiErrorLogFunction;

void
defrSetLogFunction(DEFI_LOG_FUNCTION f)
{
    defiErrorLogFunction = f;
}

extern DEFI_WARNING_LOG_FUNCTION    defiWarningLogFunction;

void
defrSetWarningLogFunction(DEFI_WARNING_LOG_FUNCTION f)
{
    defiWarningLogFunction = f;
}

DEFI_MALLOC_FUNCTION                defiMallocFunction = 0;

void
defrSetMallocFunction(DEFI_MALLOC_FUNCTION f)
{
    defiMallocFunction = f;
}

DEFI_REALLOC_FUNCTION               defiReallocFunction = 0;

void
defrSetReallocFunction(DEFI_REALLOC_FUNCTION f)
{
    defiReallocFunction = f;
}

DEFI_FREE_FUNCTION                  defiFreeFunction = 0;

void
defrSetFreeFunction(DEFI_FREE_FUNCTION f)
{
    defiFreeFunction = f;
}

extern DEFI_LINE_NUMBER_FUNCTION    defiLineNumberFunction;

void
defrSetLineNumberFunction(DEFI_LINE_NUMBER_FUNCTION f)
{
    defiLineNumberFunction = f;
}

extern int                          defiDeltaNumberLines;

void
defrSetDeltaNumberLines(int numLines)
{
    defiDeltaNumberLines = numLines;
}

extern char                         commentChar;  /* from lexer */

void
defrSetCommentChar(char c)
{
    commentChar = c;
}

void
defrSetCaseSensitivity(int caseSense)
{
    names_case_sensitive = caseSense;
    reader_case_sensitive = caseSense;
}

extern void                         *def_alias_set;

void
defrAddAlias(const char *key,
             const char *value,
             int        marked)
{
    // Since the alias data is stored in the hash table, the hash table
    // only takes the key and the data, the marked data will be stored
    // at the end of the value data

    char    *k1;
    char    *v1;
    int     len = strlen(key) + 1;
    k1 = (char*) malloc(len);
    strcpy(k1, key);
    len = strlen(value) + 1 + 1;   // 1 for the marked
    v1 = (char*) malloc(len);
    //strcpy(v1, value);
    if (marked != 0)
        marked = 1;                 // make sure only 1 digit
    sprintf(v1, "%d%s", marked, value);

    if (def_alias_set == NULL)
        def_alias_set = STR_make_stringtab(TABLE_CALLER_ALLOCS,
                                           "&alias", TABLE_USE_CASE);

    STR_add_to_stringtab(def_alias_set, k1, v1);
}

void
defrSetOpenLogFileAppend()
{
    defLogFileAppend = TRUE;
}

void
defrUnsetOpenLogFileAppend()
{
    defLogFileAppend = FALSE;
}

/* gzip read function set by defzlib for gzip only */
DEFI_READ_FUNCTION                  defiReadFunction = 0;

void
defrSetReadFunction(DEFI_READ_FUNCTION f)
{
    defiReadFunction = f;
}

void
defrUnsetReadFunction()
{
    defiReadFunction = 0;
}

void
defrDisablePropStrProcess()
{
    defDisPropStrProcess = 1;
}

// TX_DIR:TRANSLATON ON

