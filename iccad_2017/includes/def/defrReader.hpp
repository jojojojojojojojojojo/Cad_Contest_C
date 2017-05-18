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


#ifndef DEFRREADER_H
#define DEFRREADER_H

#include <stdarg.h>

#include "defiKRDefs.hpp"
#include "defiDefs.hpp"
#include "defiUser.hpp"

#define CBMAX 150    // Number of callbacks, this number need to increase
                     // if number of callbacks is greater than 150.


/*
 * An enum describing all of the types of reader callbacks.
 */
typedef enum {
  defrUnspecifiedCbkType = 0,
  defrDesignStartCbkType,
  defrTechNameCbkType,
  defrPropCbkType,
  defrPropDefEndCbkType,
  defrPropDefStartCbkType,
  defrFloorPlanNameCbkType,
  defrArrayNameCbkType,
  defrUnitsCbkType,
  defrDividerCbkType,
  defrBusBitCbkType,
  defrSiteCbkType,
  defrComponentStartCbkType,
  defrComponentCbkType,
  defrComponentEndCbkType,
  defrNetStartCbkType,
  defrNetCbkType,
  defrNetNameCbkType,
  defrNetNonDefaultRuleCbkType,
  defrNetSubnetNameCbkType,
  defrNetEndCbkType,
  defrPathCbkType,
  defrVersionCbkType,
  defrVersionStrCbkType,
  defrComponentExtCbkType,
  defrPinExtCbkType,
  defrViaExtCbkType,
  defrNetConnectionExtCbkType,
  defrNetExtCbkType,
  defrGroupExtCbkType,
  defrScanChainExtCbkType,
  defrIoTimingsExtCbkType,
  defrPartitionsExtCbkType,
  defrHistoryCbkType,
  defrDieAreaCbkType,
  defrCanplaceCbkType,
  defrCannotOccupyCbkType,
  defrPinCapCbkType,
  defrDefaultCapCbkType,
  defrStartPinsCbkType,
  defrPinCbkType,
  defrPinEndCbkType,
  defrRowCbkType,
  defrTrackCbkType,
  defrGcellGridCbkType,
  defrViaStartCbkType,
  defrViaCbkType,
  defrViaEndCbkType,
  defrRegionStartCbkType,
  defrRegionCbkType,
  defrRegionEndCbkType,
  defrSNetStartCbkType,
  defrSNetCbkType,
  defrSNetPartialPathCbkType,
  defrSNetWireCbkType,
  defrSNetEndCbkType,
  defrGroupsStartCbkType,
  defrGroupNameCbkType,
  defrGroupMemberCbkType,
  defrGroupCbkType,
  defrGroupsEndCbkType,
  defrAssertionsStartCbkType,
  defrAssertionCbkType,
  defrAssertionsEndCbkType,
  defrConstraintsStartCbkType,
  defrConstraintCbkType,
  defrConstraintsEndCbkType,
  defrScanchainsStartCbkType,
  defrScanchainCbkType,
  defrScanchainsEndCbkType,
  defrIOTimingsStartCbkType,
  defrIOTimingCbkType,
  defrIOTimingsEndCbkType,
  defrFPCStartCbkType,
  defrFPCCbkType,
  defrFPCEndCbkType,
  defrTimingDisablesStartCbkType,
  defrTimingDisableCbkType,
  defrTimingDisablesEndCbkType,
  defrPartitionsStartCbkType,
  defrPartitionCbkType,
  defrPartitionsEndCbkType,
  defrPinPropStartCbkType,
  defrPinPropCbkType,
  defrPinPropEndCbkType,
  defrBlockageStartCbkType,
  defrBlockageCbkType,
  defrBlockageEndCbkType,
  defrSlotStartCbkType,
  defrSlotCbkType,
  defrSlotEndCbkType,
  defrFillStartCbkType,
  defrFillCbkType,
  defrFillEndCbkType,
  defrCaseSensitiveCbkType,
  defrNonDefaultStartCbkType,
  defrNonDefaultCbkType,
  defrNonDefaultEndCbkType,
  defrStylesStartCbkType,
  defrStylesCbkType,
  defrStylesEndCbkType,
  defrExtensionCbkType,

  /* NEW CALLBACK - If you are creating a new callback, you must add
   * a unique item to this enum for each callback routine. When the
   * callback is called in def.y you have to supply this enum item
   * as an argument in the call. */

  defrComponentMaskShiftLayerCbkType,
  defrDesignEndCbkType
} defrCallbackType_e;
 
 
/* Declarations of function signatures for each type of callback.
 * These declarations are type-safe when compiling with ANSI C
 * or C++; you will only be able to register a function pointer
 * with the correct signature for a given type of callback.
 *
 * Each callback function is expected to return 0 if successful.
 * A non-zero return code will cause the reader to abort.
 *
 * The defrDesignStart and defrDesignEnd callback is only called once.
 * Other callbacks may be called multiple times, each time with a different
 * set of data.
 *
 * For each callback, the Def API will make the callback to the
 * function supplied by the client, which should either make a copy
 * of the Def object, or store the data in the client's own data structures.
 * The Def API will delete or reuse each object after making the callback,
 * so the client should not keep a pointer to it.
 *
 * All callbacks pass the user data pointer provided in defrRead()
 * or defrSetUserData() back to the client; this can be used by the
 * client to obtain access to the rest of the client's data structures.
 *
 * The user data pointer is obtained using defrGetUserData() immediately
 * prior to making each callback, so the client is free to change the
 * user data on the fly if necessary.
 *
 * Callbacks with the same signature are passed a callback type
 * parameter, which allows an application to write a single callback
 * function, register that function for multiple callbacks, then
 * switch based on the callback type to handle the appropriate type of
 * data.
 */
 

/* A declaration of the signature of all callbacks that return nothing. */
typedef int (*defrVoidCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
		   void* v,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a string. */
typedef int (*defrStringCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   const char *string,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a integer. */
typedef int (*defrIntegerCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   int number,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a double. */
typedef int (*defrDoubleCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   double number,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a defiProp. */
typedef int (*defrPropCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiProp *prop,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a defiSite. */
typedef int (*defrSiteCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiSite *site,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a defComponent. */
typedef int (*defrComponentCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiComponent *comp,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defComponentMaskShiftLayer. */
typedef int (*defrComponentMaskShiftLayerCbkFnType)
PROTO_PARAMS(( defrCallbackType_e,
             defiComponentMaskShiftLayer *comp,
             defiUserData ));
 
/* A declaration of the signature of all callbacks that return a defNet. */
typedef int (*defrNetCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiNet *net,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defPath. */
typedef int (*defrPathCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiPath *path,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a defiBox. */
typedef int (*defrBoxCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiBox *box,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiPinCap. */
typedef int (*defrPinCapCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiPinCap *pincap,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiPin. */
typedef int (*defrPinCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiPin *pin,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiRow. */
typedef int (*defrRowCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiRow *row,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiTrack. */
typedef int (*defrTrackCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiTrack *track,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiGcellGrid. */
typedef int (*defrGcellGridCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiGcellGrid *grid,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiVia. */
typedef int (*defrViaCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiVia *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiRegion. */
typedef int (*defrRegionCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiRegion *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiGroup. */
typedef int (*defrGroupCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiGroup *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiAssertion. */
typedef int (*defrAssertionCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiAssertion *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiScanChain. */
typedef int (*defrScanchainCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiScanchain *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiIOTiming. */
typedef int (*defrIOTimingCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiIOTiming *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiFPC. */
typedef int (*defrFPCCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiFPC *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiTimingDisable. */
typedef int (*defrTimingDisableCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiTimingDisable *,
                   defiUserData ));
 

/* A declaration of the signature of all callbacks that return a defiPartition. */
typedef int (*defrPartitionCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiPartition *,
                   defiUserData ));
 
/* A declaration of the signature of all callbacks that return a defiPinProp. */
typedef int (*defrPinPropCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiPinProp *,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defiBlockage. */
typedef int (*defrBlockageCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiBlockage *,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defiSlot. */
typedef int (*defrSlotCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiSlot *,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defiFill. */
typedef int (*defrFillCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiFill *,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defiNonDefault.
*/
typedef int (*defrNonDefaultCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiNonDefault *,
                   defiUserData ));

/* A declaration of the signature of all callbacks that return a defiStyles. */
typedef int (*defrStylesCbkFnType)
    PROTO_PARAMS(( defrCallbackType_e,
                   defiStyles *,
                   defiUserData ));

/* NEW CALLBACK - Each callback must return user data, enum, and
 *   OUR-DATA item.  We must define a callback function type for
 *   each type of OUR-DATA.  Some routines return a string, some
 *   return an integer, and some return a pointer to a class.
 *   If you create a new class, then you must create a new function
 *   type here to return that class to the user. */
 



// *****************************************************************
// * CLASS defrReader
// *
// * This is the main parser class.
// *****************************************************************
class defrReader {

    defrReader();  // singleton, disable constructor
    
public:
    int defrInit();
    int defrReset();
    int defrRead(FILE* f, const char* fName, defiUserData uData, int case_sensitive);
    void defrInitGlobals();
    void defrSetRegisterUnusedCallbacks();
    void defrSetUnusedCallbacks(defrVoidCbkFnType f);
    void defrUnsetCallbacks();
    void defrPrintUnusedCallbacks(FILE* log);
    int defrReleaseNResetMemory();

    // Control def parser output warning & info messages
    void defrDisableParserMsgs(int nMsg, int* msgs);
    void defrEnableParserMsgs(int nMsg, int* msgs);
    void defrEnableAllMsgs();
    void defrSetTotalMsgLimit(int totNumMsgs);
    void defrSetLimitPerMsg(int msgId, int numMsg);

    static defrReader *get();
    
    // Set Callback functions
    void setUserData(defiUserData d);
    defiUserData getUserData();
    void setDesignCbk(defrStringCbkFnType f);
    void setTechnologyCbk(defrStringCbkFnType f);
    void setDesignEndCbk(defrVoidCbkFnType f);
    void setPropCbk(defrPropCbkFnType f);
    void setPropDefEndCbk(defrVoidCbkFnType f);
    void setPropDefStartCbk(defrVoidCbkFnType f);
    void setArrayNameCbk(defrStringCbkFnType f);
    void setFloorPlanNameCbk(defrStringCbkFnType f);
    void setUnitsCbk(defrDoubleCbkFnType f);
    void setVersionCbk(defrDoubleCbkFnType f);
    void setVersionStrCbk(defrStringCbkFnType f);
    void setDividerCbk(defrStringCbkFnType f);
    void setBusBitCbk(defrStringCbkFnType f);
    void setSiteCbk(defrSiteCbkFnType f);
    void setCanplaceCbk(defrSiteCbkFnType f);
    void setCannotOccupyCbk(defrSiteCbkFnType f);
    void setComponentStartCbk(defrIntegerCbkFnType f);
    void setComponentEndCbk(defrVoidCbkFnType f);
    void setComponentCbk(defrComponentCbkFnType f);
    void setComponentMaskShiftLayerCbk(defrComponentMaskShiftLayerCbkFnType f);
    void setNetStartCbk(defrIntegerCbkFnType f);
    void setNetEndCbk(defrVoidCbkFnType f);
    void setNetCbk(defrNetCbkFnType f);
    void setNetNameCbk(defrStringCbkFnType f);
    void setNetSubnetNameCbk(defrStringCbkFnType f);
    void setNetNonDefaultRuleCbk(defrStringCbkFnType f);
    void setNetPartialPathCbk(defrNetCbkFnType f);
    void setSNetStartCbk(defrIntegerCbkFnType f);
    void setSNetEndCbk(defrVoidCbkFnType f);
    void setSNetCbk(defrNetCbkFnType f);
    void setSNetPartialPathCbk(defrNetCbkFnType f);
    void setSNetWireCbk(defrNetCbkFnType f);
    void setPathCbk(defrPathCbkFnType f);
    void setAddPathToNet();
    void setAllowComponentNets();
    void setComponentExtCbk(defrStringCbkFnType f);
    void setPinExtCbk(defrStringCbkFnType f);
    void setViaExtCbk(defrStringCbkFnType f);
    void setNetConnectionExtCbk(defrStringCbkFnType f);
    void setNetExtCbk(defrStringCbkFnType f);
    void setGroupExtCbk(defrStringCbkFnType f);
    void setScanChainExtCbk(defrStringCbkFnType f);
    void setIoTimingsExtCbk(defrStringCbkFnType f);
    void setPartitionsExtCbk(defrStringCbkFnType f);
    void setHistoryCbk(defrStringCbkFnType f);
    void setDieAreaCbk(defrBoxCbkFnType f);
    void setPinCapCbk(defrPinCapCbkFnType f);
    void setPinEndCbk(defrVoidCbkFnType f);
    void setStartPinsCbk(defrIntegerCbkFnType f);
    void setDefaultCapCbk(defrIntegerCbkFnType f);
    void setPinCbk(defrPinCbkFnType f);
    void setRowCbk(defrRowCbkFnType f);
    void setTrackCbk(defrTrackCbkFnType f);
    void setGcellGridCbk(defrGcellGridCbkFnType f);
    void setViaStartCbk(defrIntegerCbkFnType f);
    void setViaEndCbk(defrVoidCbkFnType f);
    void setViaCbk(defrViaCbkFnType f);
    void setRegionStartCbk(defrIntegerCbkFnType f);
    void setRegionEndCbk(defrVoidCbkFnType f);
    void setRegionCbk(defrRegionCbkFnType f);
    void setGroupsStartCbk(defrIntegerCbkFnType f);
    void setGroupsEndCbk(defrVoidCbkFnType f);
    void setGroupNameCbk(defrStringCbkFnType f);
    void setGroupMemberCbk(defrStringCbkFnType f);
    void setGroupCbk(defrGroupCbkFnType f);
    void setAssertionsStartCbk(defrIntegerCbkFnType f);
    void setAssertionsEndCbk(defrVoidCbkFnType f);
    void setAssertionCbk(defrAssertionCbkFnType f);
    void setConstraintsStartCbk(defrIntegerCbkFnType f);
    void setConstraintsEndCbk(defrVoidCbkFnType f);
    void setConstraintCbk(defrAssertionCbkFnType f);
    void setScanchainsStartCbk(defrIntegerCbkFnType f);
    void setScanchainsEndCbk(defrVoidCbkFnType f);
    void setScanchainCbk(defrScanchainCbkFnType f);
    void setIOTimingsStartCbk(defrIntegerCbkFnType f);
    void setIOTimingsEndCbk(defrVoidCbkFnType f);
    void setIOTimingCbk(defrIOTimingCbkFnType f);
    void setFPCStartCbk(defrIntegerCbkFnType f);
    void setFPCEndCbk(defrVoidCbkFnType f);
    void setFPCCbk(defrFPCCbkFnType f);
    void setTimingDisablesStartCbk(defrIntegerCbkFnType f);
    void setTimingDisablesEndCbk(defrVoidCbkFnType f);
    void setTimingDisableCbk(defrTimingDisableCbkFnType f);
    void setPartitionsStartCbk(defrIntegerCbkFnType f);
    void setPartitionsEndCbk(defrVoidCbkFnType f);
    void setPartitionCbk(defrPartitionCbkFnType f);
    void setPinPropStartCbk(defrIntegerCbkFnType f);
    void setPinPropEndCbk(defrVoidCbkFnType f);
    void setPinPropCbk(defrPinPropCbkFnType f);
    void setCaseSensitiveCbk(defrIntegerCbkFnType f);
    void setBlockageStartCbk(defrIntegerCbkFnType f);
    void setBlockageEndCbk(defrVoidCbkFnType f);
    void setBlockageCbk(defrBlockageCbkFnType f);
    void setSlotStartCbk(defrIntegerCbkFnType f);
    void setSlotEndCbk(defrVoidCbkFnType f);
    void setSlotCbk(defrSlotCbkFnType f);
    void setFillStartCbk(defrIntegerCbkFnType f);
    void setFillEndCbk(defrVoidCbkFnType f);
    void setFillCbk(defrFillCbkFnType f);
    void setNonDefaultStartCbk(defrIntegerCbkFnType f);
    void setNonDefaultEndCbk(defrVoidCbkFnType f);
    void setNonDefaultCbk(defrNonDefaultCbkFnType f);
    void setStylesStartCbk(defrIntegerCbkFnType f);
    void setStylesEndCbk(defrVoidCbkFnType f);
    void setStylesCbk(defrStylesCbkFnType f);
    void setExtensionCbk(defrStringCbkFnType f);

    void setGeomPtr(defiGeometries* p);
    void setSubnet(defiSubnet* p);

    void setAssertionWarnings(int warn);
    void setBlockageWarnings(int warn);
    void setCaseSensitiveWarnings(int warn);
    void setComponentWarnings(int warn);
    void setConstraintWarnings(int warn);
    void setDefaultCapWarnings(int warn);
    void setFillWarnings(int warn);
    void setGcellGridWarnings(int warn);
    void setIOTimingWarnings(int warn);
    void setNetWarnings(int warn);
    void setNonDefaultWarnings(int warn);
    void setPinExtWarnings(int warn);
    void setPinWarnings(int warn);
    void setRegionWarnings(int warn);
    void setRowWarnings(int warn);
    void setTrackWarnings(int warn);
    void setScanchainWarnings(int warn);
    void setSNetWarnings(int warn);
    void setStylesWarnings(int warn);
    void setUnitsWarnings(int warn);
    void setVersionWarnings(int warn);
    void setViaWarnings(int warn);
    
    // Get callback functions
    defrStringCbkFnType getDesignCbk();
    defrStringCbkFnType getTechnologyCbk();
    defrVoidCbkFnType getDesignEndCbk();
    defrPropCbkFnType getPropCbk();
    defrVoidCbkFnType getPropDefEndCbk();
    defrVoidCbkFnType getPropDefStartCbk();
    defrStringCbkFnType getArrayNameCbk();
    defrStringCbkFnType getFloorPlanNameCbk();
    defrDoubleCbkFnType getUnitsCbk();
    defrDoubleCbkFnType getVersionCbk();
    defrStringCbkFnType getVersionStrCbk();
    defrStringCbkFnType getDividerCbk();
    defrStringCbkFnType getBusBitCbk();
    defrSiteCbkFnType getSiteCbk();
    defrSiteCbkFnType getCanplaceCbk();
    defrSiteCbkFnType getCannotOccupyCbk();
    defrIntegerCbkFnType getComponentStartCbk();
    defrVoidCbkFnType getComponentEndCbk();
    defrComponentCbkFnType getComponentCbk();
    defrComponentMaskShiftLayerCbkFnType getComponentMaskShiftLayerCbk();
    defrIntegerCbkFnType getNetStartCbk();
    defrVoidCbkFnType getNetEndCbk();
    defrNetCbkFnType getNetCbk();
    defrStringCbkFnType getNetNameCbk();
    defrStringCbkFnType getNetSubnetNameCbk();
    defrStringCbkFnType getNetNonDefaultRuleCbk();
    defrNetCbkFnType getNetPartialPathCbk();
    defrIntegerCbkFnType getSNetStartCbk();
    defrVoidCbkFnType getSNetEndCbk();
    defrNetCbkFnType getSNetCbk();
    defrNetCbkFnType getSNetPartialPathCbk();
    defrNetCbkFnType getSNetWireCbk();
    defrPathCbkFnType getPathCbk();
    int getAddPathToNet();
    int getAllowComponentNets();
    defrStringCbkFnType getComponentExtCbk();
    defrStringCbkFnType getPinExtCbk();
    defrStringCbkFnType getViaExtCbk();
    defrStringCbkFnType getNetConnectionExtCbk();
    defrStringCbkFnType getNetExtCbk();
    defrStringCbkFnType getGroupExtCbk();
    defrStringCbkFnType getScanChainExtCbk();
    defrStringCbkFnType getIoTimingsExtCbk();
    defrStringCbkFnType getPartitionsExtCbk();
    defrStringCbkFnType getHistoryCbk();
    defrBoxCbkFnType getDieAreaCbk();
    defrPinCapCbkFnType getPinCapCbk();
    defrVoidCbkFnType getPinEndCbk();
    defrIntegerCbkFnType getStartPinsCbk();
    defrIntegerCbkFnType getDefaultCapCbk();
    defrPinCbkFnType getPinCbk();
    defrRowCbkFnType getRowCbk();
    defrTrackCbkFnType getTrackCbk();
    defrGcellGridCbkFnType getGcellGridCbk();
    defrIntegerCbkFnType getViaStartCbk();
    defrVoidCbkFnType getViaEndCbk();
    defrViaCbkFnType getViaCbk();
    defrIntegerCbkFnType getRegionStartCbk();
    defrVoidCbkFnType getRegionEndCbk();
    defrRegionCbkFnType getRegionCbk();
    defrIntegerCbkFnType getGroupsStartCbk();
    defrVoidCbkFnType getGroupsEndCbk();
    defrStringCbkFnType getGroupNameCbk();
    defrStringCbkFnType getGroupMemberCbk();
    defrGroupCbkFnType getGroupCbk();
    defrIntegerCbkFnType getAssertionsStartCbk();
    defrVoidCbkFnType getAssertionsEndCbk();
    defrAssertionCbkFnType getAssertionCbk();
    defrIntegerCbkFnType getConstraintsStartCbk();
    defrVoidCbkFnType getConstraintsEndCbk();
    defrAssertionCbkFnType getConstraintCbk();
    defrIntegerCbkFnType getScanchainsStartCbk();
    defrVoidCbkFnType getScanchainsEndCbk();
    defrScanchainCbkFnType getScanchainCbk();
    defrIntegerCbkFnType getIOTimingsStartCbk();
    defrVoidCbkFnType getIOTimingsEndCbk();
    defrIOTimingCbkFnType getIOTimingCbk();
    defrIntegerCbkFnType getFPCStartCbk();
    defrVoidCbkFnType getFPCEndCbk();
    defrFPCCbkFnType getFPCCbk();
    defrIntegerCbkFnType getTimingDisablesStartCbk();
    defrVoidCbkFnType getTimingDisablesEndCbk();
    defrTimingDisableCbkFnType getTimingDisableCbk();
    defrIntegerCbkFnType getPartitionsStartCbk();
    defrVoidCbkFnType getPartitionsEndCbk();
    defrPartitionCbkFnType getPartitionCbk();
    defrIntegerCbkFnType getPinPropStartCbk();
    defrVoidCbkFnType getPinPropEndCbk();
    defrPinPropCbkFnType getPinPropCbk();
    defrIntegerCbkFnType getCaseSensitiveCbk();
    defrIntegerCbkFnType getBlockageStartCbk();
    defrVoidCbkFnType getBlockageEndCbk();
    defrBlockageCbkFnType getBlockageCbk();
    defrIntegerCbkFnType getSlotStartCbk();
    defrVoidCbkFnType getSlotEndCbk();
    defrSlotCbkFnType getSlotCbk();
    defrIntegerCbkFnType getFillStartCbk();
    defrVoidCbkFnType getFillEndCbk();
    defrFillCbkFnType getFillCbk();
    defrIntegerCbkFnType getNonDefaultStartCbk();
    defrVoidCbkFnType getNonDefaultEndCbk();
    defrNonDefaultCbkFnType getNonDefaultCbk();
    defrIntegerCbkFnType getStylesStartCbk();
    defrVoidCbkFnType getStylesEndCbk();
    defrStylesCbkFnType getStylesCbk();
    defrStringCbkFnType getExtensionCbk();


    defiSubnet* getSubnet();
    defiPath* getPath();
    defiProp& getProp();
    defiSite& getSite();
    defiComponent& getComponent();
    defiComponentMaskShiftLayer& getComponentMaskShiftLayer();
    defiNet& getNet();
    defiPinCap& getPinCap();
    defiSite& getCannotOccupy();
    defiSite& getCanplace();
    defiBox& getDieArea();
    defiPin& getPin();
    defiRow& getRow();
    defiTrack& getTrack();
    defiGcellGrid& getGcellGrid();
    defiVia& getVia();
    defiRegion& getRegion();
    defiGroup& getGroup();
    defiAssertion& getAssertion();
    defiScanchain& getScanchain();
    defiIOTiming& getIOTiming();
    defiFPC& getFPC();
    defiTimingDisable& getTimingDisable();
    defiPartition& getPartition();
    defiPinProp& getPinProp();
    defiBlockage& getBlockage();
    defiSlot& getSlot();
    defiFill& getFill();
    defiNonDefault& getNonDefault();
    defiStyles& getStyles();
    defiGeometries* getGeomPtr();
    defiPropType& getCompProp();
    defiPropType& getCompPinProp();
    defiPropType& getDesignProp();
    defiPropType& getGroupProp();
    defiPropType& getNDefProp();
    defiPropType& getNetProp();
    defiPropType& getPinDefProp();
    defiPropType& getRegionProp();
    defiPropType& getRowProp();
    defiPropType& getSNetProp();

    int getAssertionWarnings();
    int getBlockageWarnings();
    int getCaseSensitiveWarnings();
    int getComponentWarnings();
    int getConstraintWarnings();
    int getDefaultCapWarnings();
    int getFillWarnings();
    int getGcellGridWarnings();
    int getIOTimingWarnings();
    int getNetWarnings();
    int getNonDefaultWarnings();
    int getPinExtWarnings();
    int getPinWarnings();
    int getRegionWarnings();
    int getRowWarnings();
    int getTrackWarnings();
    int getScanchainWarnings();
    int getSNetWarnings();
    int getStylesWarnings();
    int getUnitsWarnings();
    int getVersionWarnings();
    int getViaWarnings();

    FILE* getFile();
    const char* getFileName();
    int addPathToNet();
    int needPathData();
    int isReset();
    int isDoneDesign();
    void setDoneDesign(int d);
    void pathIsDone(int shield, int reset, int netOsnet, int *needCbk);
    void incrUnusedCallbacks(int i);
    int* getUnusedCallbacks();

private:

    defiUserData defrUserData;
    char* defrFileName;
    FILE* defrFile;
    defiSubnet* defrSubnet;
    defiPath* defrPath;
    defiProp defrProp;
    defiSite defrSite;
    defiComponent defrComponent;
    defiComponentMaskShiftLayer defrComponentMaskShiftLayer;
    defiNet defrNet;
    defiPinCap defrPinCap;
    defiSite defrCannotOccupy;
    defiSite defrCanplace;
    defiBox defrDieArea;
    defiPin defrPin;
    defiRow defrRow;
    defiTrack defrTrack;
    defiGcellGrid defrGcellGrid;
    defiVia defrVia;
    defiRegion defrRegion;
    defiGroup defrGroup;
    defiAssertion defrAssertion;
    defiScanchain defrScanchain;
    defiIOTiming defrIOTiming;
    defiFPC defrFPC;
    defiTimingDisable defrTimingDisable;
    defiPartition defrPartition;
    defiPinProp defrPinProp;
    defiBlockage defrBlockage;
    defiSlot defrSlot;
    defiFill defrFill;
    defiNonDefault defrNonDefault;
    defiStyles defrStyles;
    defiGeometries* defrGeomPtr;
    int defrIsReset;
    int doneDesign;      // keep track if the Design is done parsing
    
    // The following global variables are for storing the propertydefination
    // types.  Only real & integer need to store since the parser can
    // distinguish string and quote string
    defiPropType defrCompProp;
    defiPropType defrCompPinProp;
    defiPropType defrDesignProp;
    defiPropType defrGroupProp;
    defiPropType defrNDefProp;
    defiPropType defrNetProp;
    defiPropType defrPinDefProp;
    defiPropType defrRegionProp;
    defiPropType defrRowProp;
    defiPropType defrSNetProp;
    
    ////////////////////////////////////
    //
    //       List of call back routines
    //  These are filled in by the user.  See the
    //   "set" routines at the end of the file
    //
    /////////////////////////////////////
    
    defrStringCbkFnType defrDesignCbk;
    defrStringCbkFnType defrTechnologyCbk;
    defrVoidCbkFnType defrDesignEndCbk;
    defrPropCbkFnType defrPropCbk;
    defrVoidCbkFnType defrPropDefEndCbk;
    defrVoidCbkFnType defrPropDefStartCbk;
    defrStringCbkFnType defrArrayNameCbk;
    defrStringCbkFnType defrFloorPlanNameCbk;
    defrDoubleCbkFnType defrUnitsCbk;
    defrStringCbkFnType defrDividerCbk;
    defrStringCbkFnType defrBusBitCbk;
    defrSiteCbkFnType defrSiteCbk;
    defrSiteCbkFnType defrCanplaceCbk;
    defrSiteCbkFnType defrCannotOccupyCbk;
    defrIntegerCbkFnType defrComponentStartCbk;
    defrVoidCbkFnType defrComponentEndCbk;
    defrComponentCbkFnType defrComponentCbk;
    defrComponentMaskShiftLayerCbkFnType defrComponentMaskShiftLayerCbk;
    defrIntegerCbkFnType defrNetStartCbk;
    defrVoidCbkFnType defrNetEndCbk;
    defrNetCbkFnType defrNetCbk;
    defrStringCbkFnType defrNetNameCbk;
    defrStringCbkFnType defrNetSubnetNameCbk;
    defrStringCbkFnType defrNetNonDefaultRuleCbk;
    defrNetCbkFnType defrNetPartialPathCbk;
    defrPathCbkFnType defrPathCbk;
    defrDoubleCbkFnType defrVersionCbk;
    defrStringCbkFnType defrVersionStrCbk;
    defrStringCbkFnType defrPinExtCbk;
    defrStringCbkFnType defrComponentExtCbk;
    defrStringCbkFnType defrViaExtCbk;
    defrStringCbkFnType defrNetConnectionExtCbk;
    defrStringCbkFnType defrNetExtCbk;
    defrStringCbkFnType defrGroupExtCbk;
    defrStringCbkFnType defrScanChainExtCbk;
    defrStringCbkFnType defrIoTimingsExtCbk;
    defrStringCbkFnType defrPartitionsExtCbk;
    defrStringCbkFnType defrHistoryCbk;
    defrBoxCbkFnType defrDieAreaCbk;
    defrPinCapCbkFnType defrPinCapCbk;
    defrPinCbkFnType defrPinCbk;
    defrIntegerCbkFnType defrStartPinsCbk;
    defrVoidCbkFnType defrPinEndCbk;
    defrIntegerCbkFnType defrDefaultCapCbk;
    defrRowCbkFnType defrRowCbk;
    defrTrackCbkFnType defrTrackCbk;
    defrGcellGridCbkFnType defrGcellGridCbk;
    defrIntegerCbkFnType defrViaStartCbk;
    defrVoidCbkFnType defrViaEndCbk;
    defrViaCbkFnType defrViaCbk;
    defrIntegerCbkFnType defrRegionStartCbk;
    defrVoidCbkFnType defrRegionEndCbk;
    defrRegionCbkFnType defrRegionCbk;
    defrIntegerCbkFnType defrSNetStartCbk;
    defrVoidCbkFnType defrSNetEndCbk;
    defrNetCbkFnType defrSNetCbk;
    defrNetCbkFnType defrSNetPartialPathCbk;
    defrNetCbkFnType defrSNetWireCbk;
    defrIntegerCbkFnType defrGroupsStartCbk;
    defrVoidCbkFnType defrGroupsEndCbk;
    defrStringCbkFnType defrGroupNameCbk;
    defrStringCbkFnType defrGroupMemberCbk;
    defrGroupCbkFnType defrGroupCbk;
    defrIntegerCbkFnType defrAssertionsStartCbk;
    defrVoidCbkFnType defrAssertionsEndCbk;
    defrAssertionCbkFnType defrAssertionCbk;
    defrIntegerCbkFnType defrConstraintsStartCbk;
    defrVoidCbkFnType defrConstraintsEndCbk;
    defrAssertionCbkFnType defrConstraintCbk;
    defrIntegerCbkFnType defrScanchainsStartCbk;
    defrVoidCbkFnType defrScanchainsEndCbk;
    defrScanchainCbkFnType defrScanchainCbk;
    defrIntegerCbkFnType defrIOTimingsStartCbk;
    defrVoidCbkFnType defrIOTimingsEndCbk;
    defrIOTimingCbkFnType defrIOTimingCbk;
    defrIntegerCbkFnType defrFPCStartCbk;
    defrVoidCbkFnType defrFPCEndCbk;
    defrFPCCbkFnType defrFPCCbk;
    defrIntegerCbkFnType defrTimingDisablesStartCbk;
    defrVoidCbkFnType defrTimingDisablesEndCbk;
    defrTimingDisableCbkFnType defrTimingDisableCbk;
    defrIntegerCbkFnType defrPartitionsStartCbk;
    defrVoidCbkFnType defrPartitionsEndCbk;
    defrPartitionCbkFnType defrPartitionCbk;
    defrIntegerCbkFnType defrPinPropStartCbk;
    defrVoidCbkFnType defrPinPropEndCbk;
    defrPinPropCbkFnType defrPinPropCbk;
    defrIntegerCbkFnType defrCaseSensitiveCbk;
    defrIntegerCbkFnType defrBlockageStartCbk;
    defrVoidCbkFnType defrBlockageEndCbk;
    defrBlockageCbkFnType defrBlockageCbk;
    defrIntegerCbkFnType defrSlotStartCbk;
    defrVoidCbkFnType defrSlotEndCbk;
    defrSlotCbkFnType defrSlotCbk;
    defrIntegerCbkFnType defrFillStartCbk;
    defrVoidCbkFnType defrFillEndCbk;
    defrFillCbkFnType defrFillCbk;
    defrIntegerCbkFnType defrNonDefaultStartCbk;
    defrVoidCbkFnType defrNonDefaultEndCbk;
    defrNonDefaultCbkFnType defrNonDefaultCbk;
    defrIntegerCbkFnType defrStylesStartCbk;
    defrVoidCbkFnType defrStylesEndCbk;
    defrStylesCbkFnType defrStylesCbk;
    defrStringCbkFnType defrExtensionCbk;
    
    
    /* NEW CALLBACK - Here is the actual location where the address of
     *  the callback routines is stored.   Place each callback that you
     *  create here. */
    
    ////////////////////////////////////
    //
    //       Flags to control what happens
    //
    ////////////////////////////////////
    
    int defrAddPathToNet;
    int defrNeedPathData;
    int defrAllowComponentNets;
    int defrRegisterUnused;
    int defrUnusedCallbacks[CBMAX];
    
    ////////////////////////////////////
    //
    //       Flags to control number of warnings to print out, max will be 999
    //
    ////////////////////////////////////
    
    int defrAssertionWarnings;
    int defrBlockageWarnings;
    int defrCaseSensitiveWarnings;
    int defrComponentWarnings;
    int defrConstraintWarnings;
    int defrDefaultCapWarnings;
    int defrFillWarnings;
    int defrGcellGridWarnings;
    int defrIOTimingWarnings;
    int defrNetWarnings;
    int defrNonDefaultWarnings;
    int defrPinExtWarnings;
    int defrPinWarnings;
    int defrRegionWarnings;
    int defrRowWarnings;
    int defrTrackWarnings;
    int defrScanchainWarnings;
    int defrSNetWarnings;
    int defrStylesWarnings;
    int defrUnitsWarnings;
    int defrVersionWarnings;
    int defrViaWarnings;
    
    static defrReader *reader;
};


/* The reader initialization.  Must be called before defrRead().
 */
EXTERN int defrInit PROTO_PARAMS(( void ));

/* The reader must initialize some variables before calling the
 * 2nd or 3rd, ... def file */
EXTERN int defrReset PROTO_PARAMS(( void ));

/* Change the comment character in the DEF file.  The default
 * is '#' */
EXTERN void defrSetCommentChar PROTO_PARAMS (( char c ));

/* Functions to call to set specific actions in the parser.
 */
EXTERN void defrSetAddPathToNet PROTO_PARAMS((void));
EXTERN void defrSetAllowComponentNets PROTO_PARAMS((void));
EXTERN void defrSetCaseSensitivity PROTO_PARAMS((int caseSense));

/* Functions to keep track of callbacks that the user did not
 * supply.  Normally all parts of the DEF file that the user
 * does not supply a callback for will be ignored.  These
 * routines tell the parser count the DEF constructs that are
 * present in the input file, but did not trigger a callback.
 * This should help you find any "important" DEF constructs that
 * you are ignoring.
 */
EXTERN void defrSetRegisterUnusedCallbacks PROTO_PARAMS((void));
EXTERN void defrPrintUnusedCallbacks PROTO_PARAMS((FILE* log));
EXTERN int* defrUnusedCallbackCount PROTO_PARAMS((void));
EXTERN int  defrReleaseNResetMemory PROTO_PARAMS((void));

/*
 * The main reader function.
 * The file should already be opened.  This requirement allows
 * the reader to be used with stdin or a pipe.  The file name
 * is only used for error messages.
 */
EXTERN int defrRead
  PROTO_PARAMS(( FILE *file,
                 const char *fileName,
                 defiUserData userData,
                 int case_sensitive ));

/*
 * Set/get the client-provided user data.  defi doesn't look at
 * this data at all, it simply passes the opaque defiUserData pointer
 * back to the application with each callback.  The client can
 * change the data at any time, and it will take effect on the
 * next callback.  The defi reader and writer maintain separate
 * user data pointers.
 */
EXTERN void defrSetUserData PROTO_PARAMS(( defiUserData ));
EXTERN defiUserData defrGetUserData PROTO_PARAMS(( void ));
 
/*
 * The error and warning functions.  These take a printf style
 * argument list, and will call the corresponding callbacks
 * (defrErrorCbkFnType and defrWarningCbkFnType) if they are supplied.
 * The messages are formatted before calling the callbacks.
 */
/*
 * EXTERN void defrError PROTO_PARAMS(( const char *, ... ));
 * EXTERN void defrWarning PROTO_PARAMS(( const char *, ... ));
 * EXTERN void defrVError PROTO_PARAMS(( const char *, va_list ));
 * EXTERN void defrVWarning PROTO_PARAMS(( const char *, va_list ));
 * EXTERN int  defrGetCurrentLineNumber PROTO_PARAMS((void));
 * EXTERN const char *defrGetCurrentFileName PROTO_PARAMS((void));
 */
 
/* Functions to call to register a callback function or get the function
* pointer after it has been registered.
*/

/* Register one function for all callbacks with the same signature */
EXTERN void defrSetArrayNameCbk (defrStringCbkFnType);
EXTERN void defrSetAssertionCbk (defrAssertionCbkFnType);
EXTERN void defrSetAssertionsStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetAssertionsEndCbk (defrVoidCbkFnType);
EXTERN void defrSetBlockageCbk (defrBlockageCbkFnType);
EXTERN void defrSetBlockageStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetBlockageEndCbk (defrVoidCbkFnType);
EXTERN void defrSetBusBitCbk (defrStringCbkFnType);
EXTERN void defrSetCannotOccupyCbk (defrSiteCbkFnType);
EXTERN void defrSetCanplaceCbk (defrSiteCbkFnType);
EXTERN void defrSetCaseSensitiveCbk (defrIntegerCbkFnType);
EXTERN void defrSetComponentCbk (defrComponentCbkFnType);
EXTERN void defrSetComponentExtCbk (defrStringCbkFnType);
EXTERN void defrSetComponentStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetComponentEndCbk (defrVoidCbkFnType);
EXTERN void defrSetConstraintCbk (defrAssertionCbkFnType);
EXTERN void defrSetConstraintsStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetConstraintsEndCbk (defrVoidCbkFnType);
EXTERN void defrSetDefaultCapCbk (defrIntegerCbkFnType);
EXTERN void defrSetDesignCbk (defrStringCbkFnType);
EXTERN void defrSetDesignEndCbk (defrVoidCbkFnType);
EXTERN void defrSetDieAreaCbk (defrBoxCbkFnType);
EXTERN void defrSetDividerCbk (defrStringCbkFnType);
EXTERN void defrSetExtensionCbk (defrStringCbkFnType);
EXTERN void defrSetFillCbk (defrFillCbkFnType);
EXTERN void defrSetFillStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetFillEndCbk (defrVoidCbkFnType);
EXTERN void defrSetFPCCbk (defrFPCCbkFnType);
EXTERN void defrSetFPCStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetFPCEndCbk (defrVoidCbkFnType);
EXTERN void defrSetFloorPlanNameCbk (defrStringCbkFnType);
EXTERN void defrSetGcellGridCbk (defrGcellGridCbkFnType);
EXTERN void defrSetGroupNameCbk (defrStringCbkFnType);
EXTERN void defrSetGroupMemberCbk (defrStringCbkFnType);
EXTERN void defrSetComponentMaskShiftLayerCbk (defrComponentMaskShiftLayerCbkFnType);
EXTERN void defrSetGroupCbk (defrGroupCbkFnType);
EXTERN void defrSetGroupExtCbk (defrStringCbkFnType);
EXTERN void defrSetGroupsStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetGroupsEndCbk (defrVoidCbkFnType);
EXTERN void defrSetHistoryCbk (defrStringCbkFnType);
EXTERN void defrSetIOTimingCbk (defrIOTimingCbkFnType);
EXTERN void defrSetIOTimingsStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetIOTimingsEndCbk (defrVoidCbkFnType);
EXTERN void defrSetIoTimingsExtCbk (defrStringCbkFnType);
EXTERN void defrSetNetCbk (defrNetCbkFnType);
EXTERN void defrSetNetNameCbk (defrStringCbkFnType);
EXTERN void defrSetNetNonDefaultRuleCbk (defrStringCbkFnType);
EXTERN void defrSetNetConnectionExtCbk (defrStringCbkFnType);
EXTERN void defrSetNetExtCbk (defrStringCbkFnType);
EXTERN void defrSetNetPartialPathCbk (defrNetCbkFnType);
EXTERN void defrSetNetSubnetNameCbk (defrStringCbkFnType);
EXTERN void defrSetNetStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetNetEndCbk (defrVoidCbkFnType);
EXTERN void defrSetNonDefaultCbk (defrNonDefaultCbkFnType);
EXTERN void defrSetNonDefaultStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetNonDefaultEndCbk (defrVoidCbkFnType);
EXTERN void defrSetPartitionCbk (defrPartitionCbkFnType);
EXTERN void defrSetPartitionsExtCbk (defrStringCbkFnType);
EXTERN void defrSetPartitionsStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetPartitionsEndCbk (defrVoidCbkFnType);
EXTERN void defrSetPathCbk (defrPathCbkFnType);
EXTERN void defrSetPinCapCbk (defrPinCapCbkFnType);
EXTERN void defrSetPinCbk (defrPinCbkFnType);
EXTERN void defrSetPinExtCbk (defrStringCbkFnType);
EXTERN void defrSetPinPropCbk (defrPinPropCbkFnType);
EXTERN void defrSetPinPropStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetPinPropEndCbk (defrVoidCbkFnType);
EXTERN void defrSetPropCbk (defrPropCbkFnType);
EXTERN void defrSetPropDefEndCbk (defrVoidCbkFnType);
EXTERN void defrSetPropDefStartCbk (defrVoidCbkFnType);
EXTERN void defrSetRegionCbk (defrRegionCbkFnType);
EXTERN void defrSetRegionStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetRegionEndCbk (defrVoidCbkFnType);
EXTERN void defrSetRowCbk (defrRowCbkFnType);
EXTERN void defrSetSNetCbk (defrNetCbkFnType);
EXTERN void defrSetSNetStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetSNetEndCbk (defrVoidCbkFnType);
EXTERN void defrSetSNetPartialPathCbk (defrNetCbkFnType);
EXTERN void defrSetSNetWireCbk (defrNetCbkFnType);
EXTERN void defrSetScanChainExtCbk (defrStringCbkFnType);
EXTERN void defrSetScanchainCbk (defrScanchainCbkFnType);
EXTERN void defrSetScanchainsStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetScanchainsEndCbk (defrVoidCbkFnType);
EXTERN void defrSetSiteCbk (defrSiteCbkFnType);
EXTERN void defrSetSlotCbk (defrSlotCbkFnType);
EXTERN void defrSetSlotStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetSlotEndCbk (defrVoidCbkFnType);
EXTERN void defrSetStartPinsCbk (defrIntegerCbkFnType);
EXTERN void defrSetStylesCbk (defrStylesCbkFnType);
EXTERN void defrSetStylesStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetStylesEndCbk (defrVoidCbkFnType);
EXTERN void defrSetPinEndCbk (defrVoidCbkFnType);
EXTERN void defrSetTechnologyCbk (defrStringCbkFnType);
EXTERN void defrSetTimingDisableCbk (defrTimingDisableCbkFnType);
EXTERN void defrSetTimingDisablesStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetTimingDisablesEndCbk (defrVoidCbkFnType);
EXTERN void defrSetTrackCbk (defrTrackCbkFnType);
EXTERN void defrSetUnitsCbk (defrDoubleCbkFnType);
EXTERN void defrSetVersionCbk (defrDoubleCbkFnType);
EXTERN void defrSetVersionStrCbk (defrStringCbkFnType);
EXTERN void defrSetViaCbk (defrViaCbkFnType);
EXTERN void defrSetViaExtCbk (defrStringCbkFnType);
EXTERN void defrSetViaStartCbk (defrIntegerCbkFnType);
EXTERN void defrSetViaEndCbk (defrVoidCbkFnType);

/* NEW CALLBACK - For each new callback you create, you must
*  create a routine that allows the user to set it.  Add the
*  setting routines here. */

/*
* Set all of the callbacks that have not yet been set to the following
* function.  This is especially useful if you want to check to see
* if you forgot anything.
*/
EXTERN void defrUnsetCallbacks PROTO_PARAMS((void));

/* Functions to call to unregister a callback function.
*/
EXTERN void defrUnsetArrayNameCbk ();
EXTERN void defrUnsetAssertionCbk ();
EXTERN void defrUnsetAssertionsStartCbk ();
EXTERN void defrUnsetAssertionsEndCbk ();
EXTERN void defrUnsetBlockageCbk ();
EXTERN void defrUnsetBlockageStartCbk ();
EXTERN void defrUnsetBlockageEndCbk ();
EXTERN void defrUnsetBusBitCbk ();
EXTERN void defrUnsetCannotOccupyCbk ();
EXTERN void defrUnsetCanplaceCbk ();
EXTERN void defrUnsetCaseSensitiveCbk ();
EXTERN void defrUnsetComponentCbk ();
EXTERN void defrUnsetComponentExtCbk ();
EXTERN void defrUnsetComponentStartCbk ();
EXTERN void defrUnsetComponentEndCbk ();
EXTERN void defrUnsetConstraintCbk ();
EXTERN void defrUnsetConstraintsStartCbk ();
EXTERN void defrUnsetConstraintsEndCbk ();
EXTERN void defrUnsetDefaultCapCbk ();
EXTERN void defrUnsetDesignCbk ();
EXTERN void defrUnsetDesignEndCbk ();
EXTERN void defrUnsetDieAreaCbk ();
EXTERN void defrUnsetDividerCbk ();
EXTERN void defrUnsetExtensionCbk ();
EXTERN void defrUnsetFillCbk ();
EXTERN void defrUnsetFillStartCbk ();
EXTERN void defrUnsetFillEndCbk ();
EXTERN void defrUnsetFPCCbk ();
EXTERN void defrUnsetFPCStartCbk ();
EXTERN void defrUnsetFPCEndCbk ();
EXTERN void defrUnsetFloorPlanNameCbk ();
EXTERN void defrUnsetGcellGridCbk ();
EXTERN void defrUnsetGroupCbk ();
EXTERN void defrUnsetGroupExtCbk ();
EXTERN void defrUnsetGroupMemberCbk ();
EXTERN void defrUnsetComponentMaskShiftLayerCbk ();
EXTERN void defrUnsetGroupNameCbk ();
EXTERN void defrUnsetGroupsStartCbk ();
EXTERN void defrUnsetGroupsEndCbk ();
EXTERN void defrUnsetHistoryCbk ();
EXTERN void defrUnsetIOTimingCbk ();
EXTERN void defrUnsetIOTimingsStartCbk ();
EXTERN void defrUnsetIOTimingsEndCbk ();
EXTERN void defrUnsetIOTimingsExtCbk ();
EXTERN void defrUnsetNetCbk ();
EXTERN void defrUnsetNetNameCbk ();
EXTERN void defrUnsetNetNonDefaultRuleCbk ();
EXTERN void defrUnsetNetConnectionExtCbk ();
EXTERN void defrUnsetNetExtCbk ();
EXTERN void defrUnsetNetPartialPathCbk ();
EXTERN void defrUnsetNetSubnetNameCbk ();
EXTERN void defrUnsetNetStartCbk ();
EXTERN void defrUnsetNetEndCbk ();
EXTERN void defrUnsetNonDefaultCbk ();
EXTERN void defrUnsetNonDefaultStartCbk ();
EXTERN void defrUnsetNonDefaultEndCbk ();
EXTERN void defrUnsetPartitionCbk ();
EXTERN void defrUnsetPartitionsExtCbk ();
EXTERN void defrUnsetPartitionsStartCbk ();
EXTERN void defrUnsetPartitionsEndCbk ();
EXTERN void defrUnsetPathCbk ();
EXTERN void defrUnsetPinCapCbk ();
EXTERN void defrUnsetPinCbk ();
EXTERN void defrUnsetPinEndCbk ();
EXTERN void defrUnsetPinExtCbk ();
EXTERN void defrUnsetPinPropCbk ();
EXTERN void defrUnsetPinPropStartCbk ();
EXTERN void defrUnsetPinPropEndCbk ();
EXTERN void defrUnsetPropCbk ();
EXTERN void defrUnsetPropDefEndCbk ();
EXTERN void defrUnsetPropDefStartCbk ();
EXTERN void defrUnsetRegionCbk ();
EXTERN void defrUnsetRegionStartCbk ();
EXTERN void defrUnsetRegionEndCbk ();
EXTERN void defrUnsetRowCbk ();
EXTERN void defrUnsetScanChainExtCbk ();
EXTERN void defrUnsetScanchainCbk ();
EXTERN void defrUnsetScanchainsStartCbk ();
EXTERN void defrUnsetScanchainsEndCbk ();
EXTERN void defrUnsetSiteCbk ();
EXTERN void defrUnsetSlotCbk ();
EXTERN void defrUnsetSlotStartCbk ();
EXTERN void defrUnsetSlotEndCbk ();
EXTERN void defrUnsetSNetWireCbk ();
EXTERN void defrUnsetSNetCbk ();
EXTERN void defrUnsetSNetStartCbk ();
EXTERN void defrUnsetSNetEndCbk ();
EXTERN void defrUnsetSNetPartialPathCbk ();
EXTERN void defrUnsetStartPinsCbk ();
EXTERN void defrUnsetStylesCbk ();
EXTERN void defrUnsetStylesStartCbk ();
EXTERN void defrUnsetStylesEndCbk ();
EXTERN void defrUnsetTechnologyCbk ();
EXTERN void defrUnsetTimingDisableCbk ();
EXTERN void defrUnsetTimingDisablesStartCbk ();
EXTERN void defrUnsetTimingDisablesEndCbk ();
EXTERN void defrUnsetTrackCbk ();
EXTERN void defrUnsetUnitsCbk ();
EXTERN void defrUnsetVersionCbk ();
EXTERN void defrUnsetVersionStrCbk ();
EXTERN void defrUnsetViaCbk ();
EXTERN void defrUnsetViaExtCbk ();
EXTERN void defrUnsetViaStartCbk ();
EXTERN void defrUnsetViaEndCbk ();

/* Routine to set all unused callbacks. This is useful for checking
* to see if you missed something. */
EXTERN void defrSetUnusedCallbacks PROTO_PARAMS ((defrVoidCbkFnType func ));

/* Return the current line number in the input file. */
EXTERN int defrLineNumber PROTO_PARAMS(( void ));

/* Routine to set the message logging routine for errors */
typedef void (*DEFI_LOG_FUNCTION) PROTO_PARAMS((const char*));
void defrSetLogFunction( DEFI_LOG_FUNCTION );

/* Routine to set the message logging routine for warnings */
typedef void (*DEFI_WARNING_LOG_FUNCTION) PROTO_PARAMS((const char*));
void defrSetWarningLogFunction( DEFI_WARNING_LOG_FUNCTION );

/* Routine to set the user defined malloc routine */
typedef void* (*DEFI_MALLOC_FUNCTION) PROTO_PARAMS((size_t));
void defrSetMallocFunction( DEFI_MALLOC_FUNCTION );

/* Routine to set the user defined realloc routine */
typedef void* (*DEFI_REALLOC_FUNCTION) PROTO_PARAMS((void*, size_t));
void defrSetReallocFunction( DEFI_REALLOC_FUNCTION );

/* Routine to set the user defined free routine */
typedef void (*DEFI_FREE_FUNCTION) PROTO_PARAMS((void *));
void defrSetFreeFunction( DEFI_FREE_FUNCTION );

/* Routine to set the line number of the file that is parsing routine */
typedef void (*DEFI_LINE_NUMBER_FUNCTION)  PROTO_PARAMS((int));
void defrSetLineNumberFunction( DEFI_LINE_NUMBER_FUNCTION );

/* Set the number of lines before calling the line function callback routine */
/* Default is 10000 */
void defrSetDeltaNumberLines  PROTO_PARAMS(( int ));

/* Routine to set the read function */
typedef size_t (*DEFI_READ_FUNCTION)  PROTO_PARAMS((FILE*, char*, size_t));
void defrSetReadFunction( DEFI_READ_FUNCTION );
void defrUnsetReadFunction PROTO_PARAMS(( void ));

/* Routine to set the defrWarning.log to open as append instead for write */
/* New in 5.7 */
void defrSetOpenLogFileAppend PROTO_PARAMS(( void ));
void defrUnsetOpenLogFileAppend PROTO_PARAMS(( void ));

/* Routine to set the magic comment found routine */
typedef void (*DEFI_MAGIC_COMMENT_FOUND_FUNCTION) PROTO_PARAMS(( void ));
void defrSetMagicCommentFoundFunction( DEFI_MAGIC_COMMENT_FOUND_FUNCTION );

/* Routine to set the magic comment string */
void defrSetMagicCommentString( char * );

/* Routine to disable string property value process, default it will process */
/* the value string */
EXTERN void defrDisablePropStrProcess PROTO_PARAMS(( void ));

/* Routine to set the max number of warnings for a perticular section */

EXTERN void defrSetAssertionWarnings(int warn);
EXTERN void defrSetBlockageWarnings(int warn);
EXTERN void defrSetCaseSensitiveWarnings(int warn);
EXTERN void defrSetComponentWarnings(int warn);
EXTERN void defrSetConstraintWarnings(int warn);
EXTERN void defrSetDefaultCapWarnings(int warn);
EXTERN void defrSetGcellGridWarnings(int warn);
EXTERN void defrSetIOTimingWarnings(int warn);
EXTERN void defrSetNetWarnings(int warn);
EXTERN void defrSetNonDefaultWarnings(int warn);
EXTERN void defrSetPinExtWarnings(int warn);
EXTERN void defrSetPinWarnings(int warn);
EXTERN void defrSetRegionWarnings(int warn);
EXTERN void defrSetRowWarnings(int warn);
EXTERN void defrSetScanchainWarnings(int warn);
EXTERN void defrSetSNetWarnings(int warn);
EXTERN void defrSetStylesWarnings(int warn);
EXTERN void defrSetTrackWarnings(int warn);
EXTERN void defrSetUnitsWarnings(int warn);
EXTERN void defrSetVersionWarnings(int warn);
EXTERN void defrSetViaWarnings(int warn);

/* Handling output messages */
EXTERN void defrDisableParserMsgs(int nMsg, int* msgs);
EXTERN void defrEnableParserMsgs(int nMsg, int* msgs);
EXTERN void defrEnableAllMsgs();
EXTERN void defrSetTotalMsgLimit(int totNumMsgs);
EXTERN void defrSetLimitPerMsg(int msgId, int numMsg);

/* Return codes for the user callbacks.
* The user should return one of these values. */
#define PARSE_OK 0      /* continue parsing */
#define STOP_PARSE 1    /* stop parsing with no error message */
#define PARSE_ERROR 2   /* stop parsing, print an error message */

/* Add this alias to the list for the parser */
EXTERN void defrAddAlias PROTO_PARAMS((const char* key, const char* value,
                                      int marked));

#endif
