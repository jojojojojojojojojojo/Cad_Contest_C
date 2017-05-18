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


#ifndef LEFRCALLBACKS_H
#define LEFRCALLBACKS_H 1

#include "lefiKRDefs.hpp"
#include "lefrReader.hpp"

//
// All declarations that are public (for the casual user) are found in
// lefrReader.hpp.
// These declarations are not seen by the general user.  Only the
// parser (lef.y ... lef.tab.c) should use them.
//


//
// These are the callback function pointers.
// They are set by calls lefined in lefrReader.hpp and lefrReader.cpp
// The grammer calls these as it parses the file.
//
extern lefrUnitsCbkFnType lefrUnitsCbk;
extern lefrLayerCbkFnType lefrLayerCbk;
extern lefrIntegerCbkFnType lefrCaseSensitiveCbk;
extern lefrIntegerCbkFnType lefrFixedMaskCbk;
extern lefrStringCbkFnType lefrNoWireExtensionCbk;
extern lefrStringCbkFnType lefrBusBitCharsCbk;
extern lefrStringCbkFnType lefrDividerCharCbk;
extern lefrDoubleCbkFnType lefrVersionCbk;
extern lefrStringCbkFnType lefrVersionStrCbk;
extern lefrViaCbkFnType lefrViaCbk;
extern lefrViaRuleCbkFnType lefrViaRuleCbk;
extern lefrSpacingCbkFnType lefrSpacingCbk;
extern lefrIRDropCbkFnType lefrIRDropCbk;
extern lefrDoubleCbkFnType lefrDielectricCbk;
extern lefrMinFeatureCbkFnType lefrMinFeatureCbk;
extern lefrNonDefaultCbkFnType lefrNonDefaultCbk;
extern lefrSiteCbkFnType lefrSiteCbk;
extern lefrStringCbkFnType lefrMacroBeginCbk;
extern lefrPinCbkFnType lefrPinCbk;
extern lefrMacroCbkFnType lefrMacroCbk;
extern lefrObstructionCbkFnType lefrObstructionCbk;
extern lefrArrayCbkFnType lefrArrayCbk;
extern lefrVoidCbkFnType lefrLibraryEndCbk;
extern lefrTimingCbkFnType lefrTimingCbk;

/* NEW CALLBACK add the extern here */

extern lefrVoidCbkFnType lefrSpacingBeginCbk;
extern lefrVoidCbkFnType lefrSpacingEndCbk;
extern lefrStringCbkFnType lefrArrayBeginCbk;
extern lefrStringCbkFnType lefrArrayEndCbk;
extern lefrVoidCbkFnType lefrIRDropBeginCbk;
extern lefrVoidCbkFnType lefrIRDropEndCbk;
extern lefrVoidCbkFnType lefrPropBeginCbk;
extern lefrVoidCbkFnType lefrPropEndCbk;
extern lefrPropCbkFnType lefrPropCbk;
extern lefrNoiseMarginCbkFnType lefrNoiseMarginCbk;
extern lefrDoubleCbkFnType lefrEdgeRateThreshold1Cbk;
extern lefrDoubleCbkFnType lefrEdgeRateThreshold2Cbk;
extern lefrDoubleCbkFnType lefrEdgeRateScaleFactorCbk;
extern lefrNoiseTableCbkFnType lefrNoiseTableCbk;
extern lefrCorrectionTableCbkFnType lefrCorrectionTableCbk;
extern lefrDoubleCbkFnType lefrInputAntennaCbk;
extern lefrDoubleCbkFnType lefrOutputAntennaCbk;
extern lefrDoubleCbkFnType lefrInoutAntennaCbk;
extern lefrDoubleCbkFnType lefrAntennaInputCbk;
extern lefrDoubleCbkFnType lefrAntennaInoutCbk;
extern lefrDoubleCbkFnType lefrAntennaOutputCbk;
extern lefrDoubleCbkFnType lefrManufacturingCbk;
extern lefrUseMinSpacingCbkFnType lefrUseMinSpacingCbk;
extern lefrStringCbkFnType lefrClearanceMeasureCbk;
extern lefrStringCbkFnType lefrMacroClassTypeCbk;
extern lefrMacroNumCbkFnType lefrMacroOriginCbk;
extern lefrMacroNumCbkFnType lefrMacroSizeCbk;
extern lefrIntegerCbkFnType lefrMacroFixedMaskCbk;
extern lefrStringCbkFnType lefrMacroEndCbk;
extern lefrMaxStackViaCbkFnType lefrMaxStackViaCbk;
extern lefrStringCbkFnType lefrExtensionCbk;
extern lefrDensityCbkFnType lefrDensityCbk;

//
// These are data structures that are filled while parsing the file.
//
extern lefiUserData lefrUserData;
extern lefiUnits lefrUnits;
extern lefiLayer lefrLayer;
extern lefiVia lefrVia;
extern lefiViaRule lefrViaRule;
extern lefiSpacing lefrSpacing;
extern lefiIRDrop lefrIRDrop;
extern lefiMinFeature lefrMinFeature;
extern lefiNonDefault lefrNonDefault;
extern lefiSite lefrSite;
extern lefiMacro lefrMacro;
extern lefiObstruction lefrObstruction;
extern lefiPin lefrPin;
extern lefiArray lefrArray;
extern lefiSitePattern* lefrSitePatternPtr;
extern lefiGeometries* lefrGeometriesPtr;
extern lefiTrackPattern* lefrTrackPatternPtr;
extern lefiGcellPattern* lefrGcellPatternPtr;
extern lefiProp lefrProp;
extern lefiTiming lefrTiming;
extern struct lefiNoiseMargin lefrNoiseMargin;
extern lefiNoiseTable lefrNoiseTable;
extern lefiCorrectionTable lefrCorrectionTable;

/* NEW CALLBACK add the one static item here */
extern lefiUseMinSpacing lefrUseMinSpacing;
extern lefiAntennaPWL* lefrAntennaPWLPtr;
extern lefiMaxStackVia lefrMaxStackVia;
extern lefiDensity lefrDensity;

//
// Flags that control parsing
//
extern int lefrDoSite;
extern int lefrDoGeometries;
extern int lefrDoTrack;
extern int lefrDoGcell;
extern int lefrHasLayer;      // 5.5
extern int lefrHasMaxVS;      // 5.5
extern int lefrHasSpacingTbl; // 5.5
extern int lefrHasSpacing;    // 5.5


//
// Variables for property definition types
//
extern lefiPropType lefrLibProp;
extern lefiPropType lefrCompProp;
extern lefiPropType lefrPinProp;
extern lefiPropType lefrMacroProp;
extern lefiPropType lefrViaProp;
extern lefiPropType lefrViaRuleProp;
extern lefiPropType lefrLayerProp;
extern lefiPropType lefrNondefProp;

//
// Variables for the reader
//
extern int use5_3;
extern int use5_4;
extern int hasVer;
extern int hasNameCase;
extern int hasBusBit;
extern int hasDivChar;
extern int aOxide;
extern double versionNum;

//
// Variables for the max number of warning for the following sections
//
extern int lefrAntennaInoutWarnings;
extern int lefrAntennaInputWarnings;
extern int lefrAntennaOutputWarnings;
extern int lefrArrayWarnings;
extern int lefrCaseSensitiveWarnings;
extern int lefrCorrectionTableWarnings;
extern int lefrDielectricWarnings;
extern int lefrEdgeRateThreshold1Warnings;
extern int lefrEdgeRateThreshold2Warnings;
extern int lefrEdgeRateScaleFactorWarnings;
extern int lefrInoutAntennaWarnings;
extern int lefrInputAntennaWarnings;
extern int lefrIRDropWarnings;
extern int lefrLayerWarnings;
extern int lefrMacroWarnings;
extern int lefrMaxStackViaWarnings;
extern int lefrMinFeatureWarnings;
extern int lefrNoiseMarginWarnings;
extern int lefrNoiseTableWarnings;
extern int lefrNonDefaultWarnings;
extern int lefrNoWireExtensionWarnings;
extern int lefrOutputAntennaWarnings;
extern int lefrPinWarnings;
extern int lefrSiteWarnings;
extern int lefrSpacingWarnings;
extern int lefrTimingWarnings;
extern int lefrUnitsWarnings;
extern int lefrUseMinSpacingWarnings;
extern int lefrViaRuleWarnings;
extern int lefrViaWarnings;

//
// Parser utility routines.
//

#endif
