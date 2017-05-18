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

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <malloc.h>
#include <cstdlib>

#ifndef WIN32
#   include <unistd.h>
#else
#   include <windows.h>
#endif /* not WIN32 */
#include "../includes/lef/lefrReader.hpp"
#include "../includes/lef/lefwWriter.hpp"
#include "../includes/lef/lefiDebug.hpp"
#include "../includes/lef/lefiUtil.hpp"

//--------------------------------------------------
#include "parser.h"
#include "../common/arghandler.h"
#include "../circuit/circuit.h"

int  masterCount = 0;        // current master count
bool isCurObs = false;      // for parsing pins and macro obs
bool isShowLEFMsg = false;  // show the parsed data
//--------------------------------------------------

int printing = 0;     // Printing the output.
int parse65nm = 0;
int parseLef58Type = 0;

void checkType(lefrCallbackType_e c) {
    if (c >= 0 && c <= lefrLibraryEndCbkType) {
        // OK
    } else {
        fprintf(fout, "ERROR: callback type is out of bounds!\n");
    }
}

void lefVia(lefiVia *via) {
    return; //-------------------- return
    int i, j;

    lefrSetCaseSensitivity(1);
    fprintf(fout, "VIA %s ", via->lefiVia::name());
    if (via->lefiVia::hasDefault())
        fprintf(fout, "DEFAULT");
    else if (via->lefiVia::hasGenerated())
        fprintf(fout, "GENERATED");
    fprintf(fout, "\n");
    if (via->lefiVia::hasTopOfStack())
        fprintf(fout, "  TOPOFSTACKONLY\n");
    if (via->lefiVia::hasForeign()) {
        fprintf(fout, "  FOREIGN %s ", via->lefiVia::foreign());
        if (via->lefiVia::hasForeignPnt()) {
            fprintf(fout, "( %g %g ) ", via->lefiVia::foreignX(),
                    via->lefiVia::foreignY());
            if (via->lefiVia::hasForeignOrient())
                fprintf(fout, "%s ", orientStr(via->lefiVia::foreignOrient()));
        }
        fprintf(fout, ";\n");
    }
    if (via->lefiVia::hasProperties()) {
        fprintf(fout, "  PROPERTY ");
        for (i = 0; i < via->lefiVia::numProperties(); i++) {
            fprintf(fout, "%s ", via->lefiVia::propName(i));
            if (via->lefiVia::propIsNumber(i))
                fprintf(fout, "%g ", via->lefiVia::propNumber(i));
            if (via->lefiVia::propIsString(i))
                fprintf(fout, "%s ", via->lefiVia::propValue(i));
            /*
            if (i+1 == via->lefiVia::numProperties())  // end of properties
            fprintf(fout, ";\n");
            else      // just add new line
            fprintf(fout, "\n");
            */
            switch (via->lefiVia::propType(i)) {
            case 'R':
                fprintf(fout, "REAL ");
                break;
            case 'I':
                fprintf(fout, "INTEGER ");
                break;
            case 'S':
                fprintf(fout, "STRING ");
                break;
            case 'Q':
                fprintf(fout, "QUOTESTRING ");
                break;
            case 'N':
                fprintf(fout, "NUMBER ");
                break;
            }
        }
        fprintf(fout, ";\n");
    }
    if (via->lefiVia::hasResistance())
        fprintf(fout, "  RESISTANCE %g ;\n", via->lefiVia::resistance());
    if (via->lefiVia::numLayers() > 0) {
        for (i = 0; i < via->lefiVia::numLayers(); i++) {
            fprintf(fout, "  LAYER %s\n", via->lefiVia::layerName(i));
            for (j = 0; j < via->lefiVia::numRects(i); j++)
                if (via->lefiVia::rectColorMask(i, j)) {
                    fprintf(fout, "    RECT MASK %d ( %f %f ) ( %f %f ) ;\n",
                            via->lefiVia::rectColorMask(i, j),
                            via->lefiVia::xl(i, j), via->lefiVia::yl(i, j),
                            via->lefiVia::xh(i, j), via->lefiVia::yh(i, j));
                } else {
                    fprintf(fout, "    RECT ( %f %f ) ( %f %f ) ;\n",
                            via->lefiVia::xl(i, j), via->lefiVia::yl(i, j),
                            via->lefiVia::xh(i, j), via->lefiVia::yh(i, j));
                }
            for (j = 0; j < via->lefiVia::numPolygons(i); j++) {
                struct lefiGeomPolygon poly;
                poly = via->lefiVia::getPolygon(i, j);
                if (via->lefiVia::polyColorMask(i, j)) {
                    fprintf(fout, "    POLYGON MASK %d", via->lefiVia::polyColorMask(i, j));
                } else {
                    fprintf(fout, "    POLYGON ");
                }
                for (int k = 0; k < poly.numPoints; k++)
                    fprintf(fout, " %g %g ", poly.x[k], poly.y[k]);
                fprintf(fout, ";\n");
            }
        }
    }
    if (via->lefiVia::hasViaRule()) {
        fprintf(fout, "  VIARULE %s ;\n", via->lefiVia::viaRuleName());
        fprintf(fout, "    CUTSIZE %g %g ;\n", via->lefiVia::xCutSize(),
                via->lefiVia::yCutSize());
        fprintf(fout, "    LAYERS %s %s %s ;\n", via->lefiVia::botMetalLayer(),
                via->lefiVia::cutLayer(), via->lefiVia::topMetalLayer());
        fprintf(fout, "    CUTSPACING %g %g ;\n", via->lefiVia::xCutSpacing(),
                via->lefiVia::yCutSpacing());
        fprintf(fout, "    ENCLOSURE %g %g %g %g ;\n", via->lefiVia::xBotEnc(),
                via->lefiVia::yBotEnc(), via->lefiVia::xTopEnc(),
                via->lefiVia::yTopEnc());
        if (via->lefiVia::hasRowCol())
            fprintf(fout, "    ROWCOL %d %d ;\n", via->lefiVia::numCutRows(),
                    via->lefiVia::numCutCols());
        if (via->lefiVia::hasOrigin())
            fprintf(fout, "    ORIGIN %g %g ;\n", via->lefiVia::xOffset(),
                    via->lefiVia::yOffset());
        if (via->lefiVia::hasOffset())
            fprintf(fout, "    OFFSET %g %g %g %g ;\n", via->lefiVia::xBotOffset(),
                    via->lefiVia::yBotOffset(), via->lefiVia::xTopOffset(),
                    via->lefiVia::yTopOffset());
        if (via->lefiVia::hasCutPattern())
            fprintf(fout, "    PATTERN %s ;\n", via->lefiVia::cutPattern());
    }
    fprintf(fout, "END %s\n", via->lefiVia::name());

    return;
}

void lefSpacing(lefiSpacing* spacing) {
//    fprintf(fout, "  SAMENET %s %s %g ", spacing->lefiSpacing::name1(),
//            spacing->lefiSpacing::name2(), spacing->lefiSpacing::distance());
    if (spacing->lefiSpacing::hasStack())
        fprintf(fout, "STACK ");
//    fprintf(fout,";\n");
    return;
}

void lefViaRuleLayer(lefiViaRuleLayer* vLayer) {
    fprintf(fout, "  LAYER %s ;\n", vLayer->lefiViaRuleLayer::name());
    if (vLayer->lefiViaRuleLayer::hasDirection()) {
        if (vLayer->lefiViaRuleLayer::isHorizontal())
            fprintf(fout, "    DIRECTION HORIZONTAL ;\n");
        if (vLayer->lefiViaRuleLayer::isVertical())
            fprintf(fout, "    DIRECTION VERTICAL ;\n");
    }
    if (vLayer->lefiViaRuleLayer::hasEnclosure()) {
        fprintf(fout, "    ENCLOSURE %g %g ;\n",
                vLayer->lefiViaRuleLayer::enclosureOverhang1(),
                vLayer->lefiViaRuleLayer::enclosureOverhang2());
    }
    if (vLayer->lefiViaRuleLayer::hasWidth())
        fprintf(fout, "    WIDTH %g TO %g ;\n",
                vLayer->lefiViaRuleLayer::widthMin(),
                vLayer->lefiViaRuleLayer::widthMax());
    if (vLayer->lefiViaRuleLayer::hasResistance())
        fprintf(fout, "    RESISTANCE %g ;\n",
                vLayer->lefiViaRuleLayer::resistance());
    if (vLayer->lefiViaRuleLayer::hasOverhang())
        fprintf(fout, "    OVERHANG %g ;\n",
                vLayer->lefiViaRuleLayer::overhang());
    if (vLayer->lefiViaRuleLayer::hasMetalOverhang())
        fprintf(fout, "    METALOVERHANG %g ;\n",
                vLayer->lefiViaRuleLayer::metalOverhang());
    if (vLayer->lefiViaRuleLayer::hasSpacing())
        fprintf(fout, "    SPACING %g BY %g ;\n",
                vLayer->lefiViaRuleLayer::spacingStepX(),
                vLayer->lefiViaRuleLayer::spacingStepY());
    if (vLayer->lefiViaRuleLayer::hasRect())
        fprintf(fout, "    RECT ( %f %f ) ( %f %f ) ;\n",
                vLayer->lefiViaRuleLayer::xl(), vLayer->lefiViaRuleLayer::yl(),
                vLayer->lefiViaRuleLayer::xh(), vLayer->lefiViaRuleLayer::yh());
    return;
}

void prtGeometry(lefiGeometries *geometry) {
    // --------------------------------------------------
    int                 numItems = geometry->lefiGeometries::numItems();
    string              layerName="undefined";

    for( int i=0; i<numItems; ++i ){
        switch( geometry->lefiGeometries::itemType(i) ){

        case lefiGeomLayerE: { // metal layer
            layerName = string( geometry->lefiGeometries::getLayer(i) );
            break;
        }
        case lefiGeomRectE: {
            lefiGeomRect* lefRect = geometry->lefiGeometries::getRect(i);
            Rect rect( lefRect->xl, lefRect->yl, lefRect->xh, lefRect->yh );

            if( !isCurObs ){ // pin port (is called in pinCB)
                Pin &curpin = _cir->masterPin( _cir->numMasterPins()-1 );
                if( i==1 ){
                    curpin.setLayerName( layerName );
                    curpin.setPort( Rect( 0, 0, rect.width(), rect.height() ) );
                    double offsetX = rect.left() - _cir->master(masterCount).width()/2;
                    double offsetY = rect.bottom() - _cir->master(masterCount).height()/2;
                    curpin.setOffset( offsetX, offsetY );
                }else
                    curpin.setIsMultiPorts(true);
            }else{          // OBS
                Module &curMaster = _cir->master(masterCount);
                if( layerName == "metal1" || layerName == "LAYER1" ){ // consider only metal1
                    curMaster.addRect( rect );
                }
            }

            break;
        }
        default:
            break;
        }
    }
    // --------------------------------------------------
//    int                 numItems = geometry->lefiGeometries::numItems();
//    int                 i, j;
//    lefiGeomPath        *path;
//    lefiGeomPathIter    *pathIter;
//    lefiGeomRect        *rect;
//    lefiGeomRectIter    *rectIter;
//    lefiGeomPolygon     *polygon;
//    lefiGeomPolygonIter *polygonIter;
//    lefiGeomVia         *via;
//    lefiGeomViaIter     *viaIter;

//    for (i = 0; i < numItems; i++) {
//        switch (geometry->lefiGeometries::itemType(i)) {
//        case  lefiGeomClassE:
//            fprintf(fout, "CLASS %s ",
//                    geometry->lefiGeometries::getClass(i));
//            break;
//        case lefiGeomLayerE:
//            fprintf(fout, "      LAYER %s ;\n",
//                    geometry->lefiGeometries::getLayer(i));
//            break;
//        case lefiGeomLayerExceptPgNetE:
//            fprintf(fout, "      EXCEPTPGNET ;\n");
//            break;
//        case lefiGeomLayerMinSpacingE:
//            fprintf(fout, "      SPACING %g ;\n",
//                    geometry->lefiGeometries::getLayerMinSpacing(i));
//            break;
//        case lefiGeomLayerRuleWidthE:
//            fprintf(fout, "      DESIGNRULEWIDTH %g ;\n",
//                    geometry->lefiGeometries::getLayerRuleWidth(i));
//            break;
//        case lefiGeomWidthE:
//            fprintf(fout, "      WIDTH %g ;\n",
//                    geometry->lefiGeometries::getWidth(i));
//            break;
//        case lefiGeomPathE:
//            path = geometry->lefiGeometries::getPath(i);
//            if (path->colorMask != 0) {
//                fprintf(fout, "      PATH MASK %d ", path->colorMask);
//            } else {
//                fprintf(fout, "      PATH ");
//            }
//            for (j = 0; j < path->numPoints; j++) {
//                if (j + 1 == path->numPoints) // last one on the list
//                    fprintf(fout, "      ( %g %g ) ;\n", path->x[j], path->y[j]);
//                else
//                    fprintf(fout, "      ( %g %g )\n", path->x[j], path->y[j]);
//            }
//            break;
//        case lefiGeomPathIterE:
//            pathIter = geometry->lefiGeometries::getPathIter(i);
//            if (pathIter->colorMask != 0) {
//                fprintf(fout, "      PATH MASK %d ITERATED ", pathIter->colorMask);
//            } else {
//                fprintf(fout, "      PATH ITERATED ");
//            }
//            for (j = 0; j < pathIter->numPoints; j++)
//                fprintf(fout, "      ( %g %g )\n", pathIter->x[j],
//                        pathIter->y[j]);
//            fprintf(fout, "      DO %g BY %g STEP %g %g ;\n", pathIter->xStart,
//                    pathIter->yStart, pathIter->xStep, pathIter->yStep);
//            break;
//        case lefiGeomRectE:
//            rect = geometry->lefiGeometries::getRect(i);
//            if (rect->colorMask != 0) {
//                fprintf(fout, "      RECT MASK %d ( %f %f ) ( %f %f ) ;\n",
//                        rect->colorMask, rect->xl,
//                        rect->yl, rect->xh, rect->yh);
//            } else {
//                fprintf(fout, "      RECT ( %f %f ) ( %f %f ) ;\n", rect->xl,
//                        rect->yl, rect->xh, rect->yh);
//            }
//            break;
//        case lefiGeomRectIterE:
//            rectIter = geometry->lefiGeometries::getRectIter(i);
//            if (rectIter->colorMask != 0) {
//                fprintf(fout, "      RECT MASK %d ITERATE ( %f %f ) ( %f %f )\n",
//                        rectIter->colorMask,
//                        rectIter->xl, rectIter->yl, rectIter->xh, rectIter->yh);
//            } else {
//                fprintf(fout, "      RECT ITERATE ( %f %f ) ( %f %f )\n",
//                        rectIter->xl, rectIter->yl, rectIter->xh, rectIter->yh);
//            }
//            fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
//                    rectIter->xStart, rectIter->yStart, rectIter->xStep,
//                    rectIter->yStep);
//            break;
//        case lefiGeomPolygonE:
//            polygon = geometry->lefiGeometries::getPolygon(i);
//            if (polygon->colorMask != 0) {
//                fprintf(fout, "      POLYGON MASK %d ", polygon->colorMask);
//            } else {
//                fprintf(fout, "      POLYGON ");
//            }
//            for (j = 0; j < polygon->numPoints; j++) {
//                if (j + 1 == polygon->numPoints) // last one on the list
//                    fprintf(fout, "      ( %g %g ) ;\n", polygon->x[j],
//                            polygon->y[j]);
//                else
//                    fprintf(fout, "      ( %g %g )\n", polygon->x[j],
//                            polygon->y[j]);
//            }
//            break;
//        case lefiGeomPolygonIterE:
//            polygonIter = geometry->lefiGeometries::getPolygonIter(i);
//            if (polygonIter->colorMask != 0) {
//                fprintf(fout, "       POLYGON MASK %d ITERATE ", polygonIter->colorMask);
//            } else {
//                fprintf(fout, "      POLYGON ITERATE");
//            }
//            for (j = 0; j < polygonIter->numPoints; j++)
//                fprintf(fout, "      ( %g %g )\n", polygonIter->x[j],
//                        polygonIter->y[j]);
//            fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
//                    polygonIter->xStart, polygonIter->yStart,
//                    polygonIter->xStep, polygonIter->yStep);
//            break;
//        case lefiGeomViaE:
//            via = geometry->lefiGeometries::getVia(i);
//            if (via->topMaskNum != 0 || via->bottomMaskNum != 0 || via->cutMaskNum !=0) {
//                fprintf(fout, "      VIA MASK %d%d%d ( %g %g ) %s ;\n",
//                        via->topMaskNum, via->cutMaskNum, via->bottomMaskNum,
//                        via->x, via->y,
//                        via->name);

//            } else {
//                fprintf(fout, "      VIA ( %g %g ) %s ;\n", via->x, via->y,
//                        via->name);
//            }
//            break;
//        case lefiGeomViaIterE:
//            viaIter = geometry->lefiGeometries::getViaIter(i);
//            if (viaIter->topMaskNum != 0 || viaIter->cutMaskNum != 0 || viaIter->bottomMaskNum != 0) {
//                fprintf(fout, "      VIA ITERATE MASK %d%d%d ( %g %g ) %s\n",
//                        viaIter->topMaskNum, viaIter->cutMaskNum, viaIter->bottomMaskNum,
//                        viaIter->x,
//                        viaIter->y, viaIter->name);
//            } else {
//                fprintf(fout, "      VIA ITERATE ( %g %g ) %s\n", viaIter->x,
//                        viaIter->y, viaIter->name);
//            }
//            fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
//                    viaIter->xStart, viaIter->yStart,
//                    viaIter->xStep, viaIter->yStep);
//            break;
//        default:
//            fprintf(fout, "BOGUS geometries type.\n");
//            break;
//        }
//    }
}

int antennaCB(lefrCallbackType_e c, double value, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    switch (c) {
    case lefrAntennaInputCbkType:
        fprintf(fout, "ANTENNAINPUTGATEAREA %g ;\n", value);
        break;
    case lefrAntennaInoutCbkType:
        fprintf(fout, "ANTENNAINOUTDIFFAREA %g ;\n", value);
        break;
    case lefrAntennaOutputCbkType:
        fprintf(fout, "ANTENNAOUTPUTDIFFAREA %g ;\n", value);
        break;
    case lefrInputAntennaCbkType:
        fprintf(fout, "INPUTPINANTENNASIZE %g ;\n", value);
        break;
    case lefrOutputAntennaCbkType:
        fprintf(fout, "OUTPUTPINANTENNASIZE %g ;\n", value);
        break;
    case lefrInoutAntennaCbkType:
        fprintf(fout, "INOUTPINANTENNASIZE %g ;\n", value);
        break;
    default:
        fprintf(fout, "BOGUS antenna type.\n");
        break;
    }
    return 0;
}

int arrayBeginCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
    int  status;

    checkType(c);
    if ((long)ud != userData) dataError();
    // use the lef writer to write the data out
    status = lefwStartArray(name);
    if (status != LEFW_OK)
        return status;
    return 0;
}

int arrayCB(lefrCallbackType_e c, lefiArray* a, lefiUserData ud) {
    int              status, i, j, defCaps;
    lefiSitePattern* pattern;
    lefiTrackPattern* track;
    lefiGcellPattern* gcell;

    checkType(c);
    if ((long)ud != userData) dataError();

    if (a->lefiArray::numSitePattern() > 0) {
        for (i = 0; i < a->lefiArray::numSitePattern(); i++) {
            pattern = a->lefiArray::sitePattern(i);
            status = lefwArraySite(pattern->lefiSitePattern::name(),
                                   pattern->lefiSitePattern::x(),
                                   pattern->lefiSitePattern::y(),
                                   pattern->lefiSitePattern::orient(),
                                   pattern->lefiSitePattern::xStart(),
                                   pattern->lefiSitePattern::yStart(),
                                   pattern->lefiSitePattern::xStep(),
                                   pattern->lefiSitePattern::yStep());
            if (status != LEFW_OK)
                dataError();
        }
    }
    if (a->lefiArray::numCanPlace() > 0) {
        for (i = 0; i < a->lefiArray::numCanPlace(); i++) {
            pattern = a->lefiArray::canPlace(i);
            status = lefwArrayCanplace(pattern->lefiSitePattern::name(),
                                       pattern->lefiSitePattern::x(),
                                       pattern->lefiSitePattern::y(),
                                       pattern->lefiSitePattern::orient(),
                                       pattern->lefiSitePattern::xStart(),
                                       pattern->lefiSitePattern::yStart(),
                                       pattern->lefiSitePattern::xStep(),
                                       pattern->lefiSitePattern::yStep());
            if (status != LEFW_OK)
                dataError();
        }
    }
    if (a->lefiArray::numCannotOccupy() > 0) {
        for (i = 0; i < a->lefiArray::numCannotOccupy(); i++) {
            pattern = a->lefiArray::cannotOccupy(i);
            status = lefwArrayCannotoccupy(pattern->lefiSitePattern::name(),
                                           pattern->lefiSitePattern::x(),
                                           pattern->lefiSitePattern::y(),
                                           pattern->lefiSitePattern::orient(),
                                           pattern->lefiSitePattern::xStart(),
                                           pattern->lefiSitePattern::yStart(),
                                           pattern->lefiSitePattern::xStep(),
                                           pattern->lefiSitePattern::yStep());
            if (status != LEFW_OK)
                dataError();
        }
    }

    if (a->lefiArray::numTrack() > 0) {
        for (i = 0; i < a->lefiArray::numTrack(); i++) {
            track = a->lefiArray::track(i);
            fprintf(fout, "  TRACKS %s, %g DO %d STEP %g\n",
                    track->lefiTrackPattern::name(),
                    track->lefiTrackPattern::start(),
                    track->lefiTrackPattern::numTracks(),
                    track->lefiTrackPattern::space());
            if (track->lefiTrackPattern::numLayers() > 0) {
                fprintf(fout, "  LAYER ");
                for (j = 0; j < track->lefiTrackPattern::numLayers(); j++)
                    fprintf(fout, "%s ", track->lefiTrackPattern::layerName(j));
                fprintf(fout, ";\n");
            }
        }
    }

    if (a->lefiArray::numGcell() > 0) {
        // to-do: GCELL (global routing cell)
        for (i = 0; i < a->lefiArray::numGcell(); i++) {
            gcell = a->lefiArray::gcell(i);
            if( gcell->lefiGcellPattern::name() ) {}
//            fprintf(fout, "  GCELLGRID %s, %g DO %d STEP %g\n",
//                    gcell->lefiGcellPattern::name(),
//                    gcell->lefiGcellPattern::start(),
//                    gcell->lefiGcellPattern::numCRs(),
//                    gcell->lefiGcellPattern::space());
        }
    }

    if (a->lefiArray::numFloorPlans() > 0) {
        for (i = 0; i < a->lefiArray::numFloorPlans(); i++) {
            status = lefwStartArrayFloorplan(a->lefiArray::floorPlanName(i));
            if (status != LEFW_OK)
                dataError();
            for (j = 0; j < a->lefiArray::numSites(i); j++) {
                pattern = a->lefiArray::site(i, j);
                status = lefwArrayFloorplan(a->lefiArray::siteType(i, j),
                                            pattern->lefiSitePattern::name(),
                                            pattern->lefiSitePattern::x(),
                                            pattern->lefiSitePattern::y(),
                                            pattern->lefiSitePattern::orient(),
                                            (int)pattern->lefiSitePattern::xStart(),
                                            (int)pattern->lefiSitePattern::yStart(),
                                            pattern->lefiSitePattern::xStep(),
                                            pattern->lefiSitePattern::yStep());
                if (status != LEFW_OK)
                    dataError();
            }
            status = lefwEndArrayFloorplan(a->lefiArray::floorPlanName(i));
            if (status != LEFW_OK)
                dataError();
        }
    }

    defCaps = a->lefiArray::numDefaultCaps();
    if (defCaps > 0) {
        status = lefwStartArrayDefaultCap(defCaps);
        if (status != LEFW_OK)
            dataError();
        for (i = 0; i < defCaps; i++) {
            status = lefwArrayDefaultCap(a->lefiArray::defaultCapMinPins(i),
                                         a->lefiArray::defaultCap(i));
            if (status != LEFW_OK)
                dataError();
        }
        status = lefwEndArrayDefaultCap();
        if (status != LEFW_OK)
            dataError();
    }
    return 0;
}

int arrayEndCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
    int  status;

    checkType(c);
    if ((long)ud != userData) dataError();
    // use the lef writer to write the data out
    status = lefwEndArray(name);
    if (status != LEFW_OK)
        return status;
    return 0;
}

int busBitCharsCB(lefrCallbackType_e c, const char* busBit, lefiUserData ud)
{
    checkType(c);
    if ((long)ud != userData) dataError();
    if (busBit) {}
    // use the lef writer to write out the data
//    int status;
//    status = lefwBusBitChars(busBit);
//    if (status != LEFW_OK)
//        dataError();
    return 0;
}

int caseSensCB(lefrCallbackType_e c, int caseSense, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    if( caseSense ) {}

//    if (caseSense == TRUE)
//        fprintf(fout, "NAMESCASESENSITIVE ON ;\n");
//    else
//        fprintf(fout, "NAMESCASESENSITIVE OFF ;\n");
    return 0;
}

int fixedMaskCB(lefrCallbackType_e c, int fixedMask, lefiUserData ud) {
    checkType(c);

    if( ud ) { }

    if (fixedMask == 1)
        fprintf(fout, "FIXEDMASK ;\n");
    return 0;
}

int clearanceCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    if (name) {}
//    fprintf(fout, "CLEARANCEMEASURE %s ;\n", name);
    return 0;
}

int dividerCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    if (name) {}
//    fprintf(fout, "DIVIDER %s ;\n", name);
    return 0;
}

int noWireExtCB(lefrCallbackType_e c, const char* name, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    fprintf(fout, "NOWIREEXTENSION %s ;\n", name);
    return 0;
}

int noiseMarCB(lefrCallbackType_e c, lefiNoiseMargin *noise, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if(noise){}
    return 0;
}

int edge1CB(lefrCallbackType_e c, double name, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    fprintf(fout, "EDGERATETHRESHOLD1 %g ;\n", name);
    return 0;
}

int edge2CB(lefrCallbackType_e c, double name, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    fprintf(fout, "EDGERATETHRESHOLD2 %g ;\n", name);
    return 0;
}

int edgeScaleCB(lefrCallbackType_e c, double name, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    fprintf(fout, "EDGERATESCALEFACTORE %g ;\n", name);
    return 0;
}

int noiseTableCB(lefrCallbackType_e c, lefiNoiseTable *noise, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if(noise){}
    return 0;
}

int correctionCB(lefrCallbackType_e c, lefiCorrectionTable *corr, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if(corr){}
    return 0;
}

int dielectricCB(lefrCallbackType_e c, double dielectric, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    fprintf(fout, "DIELECTRIC %g ;\n", dielectric);
    return 0;
}

int irdropBeginCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
    checkType(c);
    if ((long)ud != userData) dataError();
    if(ptr){}
    fprintf(fout, "IRDROP\n");
    return 0;
}

int irdropCB(lefrCallbackType_e c, lefiIRDrop* irdrop, lefiUserData ud) {
    int i;
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "  TABLE %s ", irdrop->lefiIRDrop::name());
    for (i = 0; i < irdrop->lefiIRDrop::numValues(); i++)
        fprintf(fout, "%g %g ", irdrop->lefiIRDrop::value1(i),
                irdrop->lefiIRDrop::value2(i));
    fprintf(fout, ";\n");
    return 0;
}

int irdropEndCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
    checkType(c);
    if ((long)ud != userData) dataError();
    if (ptr){}
    fprintf(fout, "END IRDROP\n");
    return 0;
}

int layerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud) {

    // --------------------------------------------------
    checkType(c);
    if( (long)ud != userData ) dataError();
    //lefrSetCaseSensitivity(0); // do not change the case

    if( strcmp( layer->lefiLayer::type(), "ROUTING" ) == 0 )
    {
        unsigned layerId = mtlStr2Int( layer->lefiLayer::name() )-1;
        if( !_cir->addLayer( layerId ) )
            return 0; // has been added, ignore it

        Layer &curlayer = _cir->layer( layerId );
        curlayer.setName( layer->lefiLayer::name() );
        curlayer.setType( layer->lefiLayer::type() );

        // direction
        if( layer->lefiLayer::hasDirection() )
            curlayer.setDirection( layer->lefiLayer::direction() );
        else{
            curlayer.setDirection( "unknown" );
            cerr << "[ERROR] No routing layer direction specified." << endl; exit(0);
        }

        // pitch
        curlayer.setPitch( layer->lefiLayer::pitch() );

        // offset
        if( layer->lefiLayer::hasOffset() )
            curlayer.setOffset( layer->lefiLayer::offset() );
        else if( layer->lefiLayer::hasXYOffset() ){
            if( curlayer.direction() == "HORIZONTAL" )
                curlayer.setOffset( layer->lefiLayer::offsetY() );
            else
                curlayer.setOffset( layer->lefiLayer::offsetX() );
        }


        // width
        if( layer->lefiLayer::hasWidth() )
            curlayer.setWidth( layer->lefiLayer::width() );

        if( isShowLEFMsg )
            curlayer.showInfo();
    }
    return 0;
    // --------------------------------------------------
//    int i, j, k;
//    int numPoints, propNum;
//    double *widths, *current;
//    lefiLayerDensity* density;
//    lefiAntennaPWL* pwl;
//    lefiSpacingTable* spTable;
//    lefiInfluence* influence;
//    lefiParallel* parallel;
//    lefiTwoWidths* twoWidths;
//    char pType;
//    int numMinCut, numMinenclosed;
//    lefiAntennaModel* aModel;
//    lefiOrthogonal*   ortho;

//    checkType(c);
//    if ((long)ud != userData) dataError();

//    lefrSetCaseSensitivity(0);

//    // Call parse65nmRules for 5.7 syntax in 5.6
//    if (parse65nm)
//        layer->lefiLayer::parse65nmRules();

//    // Call parseLef58Type for 5.8 syntax in 5.7
//    if (parseLef58Type)
//        layer->lefiLayer::parseLEF58Layer();

//    fprintf(fout, "LAYER %s\n", layer->lefiLayer::name());
//    if (layer->lefiLayer::hasType())
//        fprintf(fout, "  TYPE %s ;\n", layer->lefiLayer::type());
//    if (layer->lefiLayer::hasLayerType())
//        fprintf(fout, "  LAYER TYPE %s ;\n", layer->lefiLayer::layerType());
//    if (layer->lefiLayer::hasMask())
//        fprintf(fout, "  MASK %d ;\n", layer->lefiLayer::mask());
//    if (layer->lefiLayer::hasPitch())
//        fprintf(fout, "  PITCH %g ;\n", layer->lefiLayer::pitch());
//    else if (layer->lefiLayer::hasXYPitch())
//        fprintf(fout, "  PITCH %g %g ;\n", layer->lefiLayer::pitchX(),
//                layer->lefiLayer::pitchY());
//    if (layer->lefiLayer::hasOffset())
//        fprintf(fout, "  OFFSET %g ;\n", layer->lefiLayer::offset());
//    else if (layer->lefiLayer::hasXYOffset())
//        fprintf(fout, "  OFFSET %g %g ;\n", layer->lefiLayer::offsetX(),
//                layer->lefiLayer::offsetY());
//    if (layer->lefiLayer::hasDiagPitch())
//        fprintf(fout, "  DIAGPITCH %g ;\n", layer->lefiLayer::diagPitch());
//    else if (layer->lefiLayer::hasXYDiagPitch())
//        fprintf(fout, "  DIAGPITCH %g %g ;\n", layer->lefiLayer::diagPitchX(),
//                layer->lefiLayer::diagPitchY());
//    if (layer->lefiLayer::hasDiagWidth())
//        fprintf(fout, "  DIAGWIDTH %g ;\n", layer->lefiLayer::diagWidth());
//    if (layer->lefiLayer::hasDiagSpacing())
//        fprintf(fout, "  DIAGSPACING %g ;\n", layer->lefiLayer::diagSpacing());
//    if (layer->lefiLayer::hasWidth())
//        fprintf(fout, "  WIDTH %g ;\n", layer->lefiLayer::width());
//    if (layer->lefiLayer::hasArea())
//        fprintf(fout, "  AREA %g ;\n", layer->lefiLayer::area());
//    if (layer->lefiLayer::hasSlotWireWidth())
//        fprintf(fout, "  SLOTWIREWIDTH %g ;\n", layer->lefiLayer::slotWireWidth());
//    if (layer->lefiLayer::hasSlotWireLength())
//        fprintf(fout, "  SLOTWIRELENGTH %g ;\n",
//                layer->lefiLayer::slotWireLength());
//    if (layer->lefiLayer::hasSlotWidth())
//        fprintf(fout, "  SLOTWIDTH %g ;\n", layer->lefiLayer::slotWidth());
//    if (layer->lefiLayer::hasSlotLength())
//        fprintf(fout, "  SLOTLENGTH %g ;\n", layer->lefiLayer::slotLength());
//    if (layer->lefiLayer::hasMaxAdjacentSlotSpacing())
//        fprintf(fout, "  MAXADJACENTSLOTSPACING %g ;\n",
//                layer->lefiLayer::maxAdjacentSlotSpacing());
//    if (layer->lefiLayer::hasMaxCoaxialSlotSpacing())
//        fprintf(fout, "  MAXCOAXIALSLOTSPACING %g ;\n",
//                layer->lefiLayer::maxCoaxialSlotSpacing());
//    if (layer->lefiLayer::hasMaxEdgeSlotSpacing())
//        fprintf(fout, "  MAXEDGESLOTSPACING %g ;\n",
//                layer->lefiLayer::maxEdgeSlotSpacing());
//    if (layer->lefiLayer::hasMaxFloatingArea())          // 5.7
//        fprintf(fout, "  MAXFLOATINGAREA %g ;\n",
//                layer->lefiLayer::maxFloatingArea());
//    if (layer->lefiLayer::hasArraySpacing()) {           // 5.7
//        fprintf(fout, "  ARRAYSPACING ");
//        if (layer->lefiLayer::hasLongArray())
//            fprintf(fout, "LONGARRAY ");
//        if (layer->lefiLayer::hasViaWidth())
//            fprintf(fout, "WIDTH %g ", layer->lefiLayer::viaWidth());
//        fprintf(fout, "CUTSPACING %g", layer->lefiLayer::cutSpacing());
//        for (i = 0; i < layer->lefiLayer::numArrayCuts(); i++)
//            fprintf(fout, "\n\tARRAYCUTS %d SPACING %g",
//                    layer->lefiLayer::arrayCuts(i),
//                    layer->lefiLayer::arraySpacing(i));
//        fprintf(fout, " ;\n");
//    }
//    if (layer->lefiLayer::hasSplitWireWidth())
//        fprintf(fout, "  SPLITWIREWIDTH %g ;\n",
//                layer->lefiLayer::splitWireWidth());
//    if (layer->lefiLayer::hasMinimumDensity())
//        fprintf(fout, "  MINIMUMDENSITY %g ;\n",
//                layer->lefiLayer::minimumDensity());
//    if (layer->lefiLayer::hasMaximumDensity())
//        fprintf(fout, "  MAXIMUMDENSITY %g ;\n",
//                layer->lefiLayer::maximumDensity());
//    if (layer->lefiLayer::hasDensityCheckWindow())
//        fprintf(fout, "  DENSITYCHECKWINDOW %g %g ;\n",
//                layer->lefiLayer::densityCheckWindowLength(),
//                layer->lefiLayer::densityCheckWindowWidth());
//    if (layer->lefiLayer::hasDensityCheckStep())
//        fprintf(fout, "  DENSITYCHECKSTEP %g ;\n",
//                layer->lefiLayer::densityCheckStep());
//    if (layer->lefiLayer::hasFillActiveSpacing())
//        fprintf(fout, "  FILLACTIVESPACING %g ;\n",
//                layer->lefiLayer::fillActiveSpacing());
//    // 5.4.1
//    numMinCut = layer->lefiLayer::numMinimumcut();
//    if (numMinCut > 0) {
//        for (i = 0; i < numMinCut; i++) {
//            fprintf(fout, "  MINIMUMCUT %d WIDTH %g ",
//                    layer->lefiLayer::minimumcut(i),
//                    layer->lefiLayer::minimumcutWidth(i));
//            if (layer->lefiLayer::hasMinimumcutWithin(i))
//                fprintf(fout, "WITHIN %g ", layer->lefiLayer::minimumcutWithin(i));
//            if (layer->lefiLayer::hasMinimumcutConnection(i))
//                fprintf(fout, "%s ", layer->lefiLayer::minimumcutConnection(i));
//            if (layer->lefiLayer::hasMinimumcutNumCuts(i))
//                fprintf(fout, "LENGTH %g WITHIN %g ",
//                        layer->lefiLayer::minimumcutLength(i),
//                        layer->lefiLayer::minimumcutDistance(i));
//            fprintf(fout, ";\n");
//        }
//    }
//    // 5.4.1
//    if (layer->lefiLayer::hasMaxwidth()) {
//        fprintf(fout, "  MAXWIDTH %g ;\n", layer->lefiLayer::maxwidth());
//    }
//    // 5.5
//    if (layer->lefiLayer::hasMinwidth()) {
//        fprintf(fout, "  MINWIDTH %g ;\n", layer->lefiLayer::minwidth());
//    }
//    // 5.5
//    numMinenclosed = layer->lefiLayer::numMinenclosedarea();
//    if (numMinenclosed > 0) {
//        for (i = 0; i < numMinenclosed; i++) {
//            fprintf(fout, "  MINENCLOSEDAREA %g ",
//                    layer->lefiLayer::minenclosedarea(i));
//            if (layer->lefiLayer::hasMinenclosedareaWidth(i))
//                fprintf(fout, "MINENCLOSEDAREAWIDTH %g ",
//                        layer->lefiLayer::minenclosedareaWidth(i));
//            fprintf (fout, ";\n");
//        }
//    }
//    // 5.4.1 & 5.6
//    if (layer->lefiLayer::hasMinstep()) {
//        for (i = 0; i < layer->lefiLayer::numMinstep(); i++) {
//            fprintf(fout, "  MINSTEP %g ", layer->lefiLayer::minstep(i));
//            if (layer->lefiLayer::hasMinstepType(i))
//                fprintf(fout, "%s ", layer->lefiLayer::minstepType(i));
//            if (layer->lefiLayer::hasMinstepLengthsum(i))
//                fprintf(fout, "LENGTHSUM %g ",
//                        layer->lefiLayer::minstepLengthsum(i));
//            if (layer->lefiLayer::hasMinstepMaxedges(i))
//                fprintf(fout, "MAXEDGES %d ", layer->lefiLayer::minstepMaxedges(i));
//            if (layer->lefiLayer::hasMinstepMinAdjLength(i))
//                fprintf(fout, "MINADJLENGTH %g ", layer->lefiLayer::minstepMinAdjLength(i));
//            if (layer->lefiLayer::hasMinstepMinBetLength(i))
//                fprintf(fout, "MINBETLENGTH %g ", layer->lefiLayer::minstepMinBetLength(i));
//            if (layer->lefiLayer::hasMinstepXSameCorners(i))
//                fprintf(fout, "XSAMECORNERS");
//            fprintf(fout, ";\n");
//        }
//    }
//    // 5.4.1
//    if (layer->lefiLayer::hasProtrusion()) {
//        fprintf(fout, "  PROTRUSIONWIDTH %g LENGTH %g WIDTH %g ;\n",
//                layer->lefiLayer::protrusionWidth1(),
//                layer->lefiLayer::protrusionLength(),
//                layer->lefiLayer::protrusionWidth2());
//    }
//    if (layer->lefiLayer::hasSpacingNumber()) {
//        for (i = 0; i < layer->lefiLayer::numSpacing(); i++) {
//            fprintf(fout, "  SPACING %g ", layer->lefiLayer::spacing(i));
//            if (layer->lefiLayer::hasSpacingName(i))
//                fprintf(fout, "LAYER %s ", layer->lefiLayer::spacingName(i));
//            if (layer->lefiLayer::hasSpacingLayerStack(i))
//                fprintf(fout, "STACK ");                           // 5.7
//            if (layer->lefiLayer::hasSpacingAdjacent(i))
//                fprintf(fout, "ADJACENTCUTS %d WITHIN %g ",
//                        layer->lefiLayer::spacingAdjacentCuts(i),
//                        layer->lefiLayer::spacingAdjacentWithin(i));
//            if (layer->lefiLayer::hasSpacingAdjacentExcept(i))    // 5.7
//                fprintf(fout, "EXCEPTSAMEPGNET ");
//            if (layer->lefiLayer::hasSpacingCenterToCenter(i))
//                fprintf(fout, "CENTERTOCENTER ");
//            if (layer->lefiLayer::hasSpacingSamenet(i))           // 5.7
//                fprintf(fout, "SAMENET ");
//            if (layer->lefiLayer::hasSpacingSamenetPGonly(i)) // 5.7
//                fprintf(fout, "PGONLY ");
//            if (layer->lefiLayer::hasSpacingArea(i))              // 5.7
//                fprintf(fout, "AREA %g ", layer->lefiLayer::spacingArea(i));
//            if (layer->lefiLayer::hasSpacingRange(i)) {
//                fprintf(fout, "RANGE %g %g ", layer->lefiLayer::spacingRangeMin(i),
//                        layer->lefiLayer::spacingRangeMax(i));
//                if (layer->lefiLayer::hasSpacingRangeUseLengthThreshold(i))
//                    fprintf(fout, "USELENGTHTHRESHOLD ");
//                else if (layer->lefiLayer::hasSpacingRangeInfluence(i)) {
//                    fprintf(fout, "INFLUENCE %g ",
//                            layer->lefiLayer::spacingRangeInfluence(i));
//                    if (layer->lefiLayer::hasSpacingRangeInfluenceRange(i))
//                        fprintf(fout, "RANGE %g %g ",
//                                layer->lefiLayer::spacingRangeInfluenceMin(i),
//                                layer->lefiLayer::spacingRangeInfluenceMax(i));
//                } else if (layer->lefiLayer::hasSpacingRangeRange(i))
//                    fprintf(fout, "RANGE %g %g ",
//                            layer->lefiLayer::spacingRangeRangeMin(i),
//                            layer->lefiLayer::spacingRangeRangeMax(i));
//            } else if (layer->lefiLayer::hasSpacingLengthThreshold(i)) {
//                fprintf(fout, "LENGTHTHRESHOLD %g ",
//                        layer->lefiLayer::spacingLengthThreshold(i));
//                if (layer->lefiLayer::hasSpacingLengthThresholdRange(i))
//                    fprintf(fout, "RANGE %g %g",
//                            layer->lefiLayer::spacingLengthThresholdRangeMin(i),
//                            layer->lefiLayer::spacingLengthThresholdRangeMax(i));
//            } else if (layer->lefiLayer::hasSpacingNotchLength(i)) {// 5.7
//                fprintf(fout, "NOTCHLENGTH %g",
//                        layer->lefiLayer::spacingNotchLength(i));
//            } else if (layer->lefiLayer::hasSpacingEndOfNotchWidth(i)) // 5.7
//                fprintf(fout, "ENDOFNOTCHWIDTH %g NOTCHSPACING %g, NOTCHLENGTH %g",
//                        layer->lefiLayer::spacingEndOfNotchWidth(i),
//                        layer->lefiLayer::spacingEndOfNotchSpacing(i),
//                        layer->lefiLayer::spacingEndOfNotchLength(i));

//            if (layer->lefiLayer::hasSpacingParallelOverlap(i))   // 5.7
//                fprintf(fout, "PARALLELOVERLAP ");
//            if (layer->lefiLayer::hasSpacingEndOfLine(i)) {       // 5.7
//                fprintf(fout, "ENDOFLINE %g WITHIN %g ",
//                        layer->lefiLayer::spacingEolWidth(i),
//                        layer->lefiLayer::spacingEolWithin(i));
//                if (layer->lefiLayer::hasSpacingParellelEdge(i)) {
//                    fprintf(fout, "PARALLELEDGE %g WITHIN %g ",
//                            layer->lefiLayer::spacingParSpace(i),
//                            layer->lefiLayer::spacingParWithin(i));
//                    if (layer->lefiLayer::hasSpacingTwoEdges(i)) {
//                        fprintf(fout, "TWOEDGES ");
//                    }
//                }
//            }
//            fprintf(fout, ";\n");
//        }
//    }
//    if (layer->lefiLayer::hasSpacingTableOrtho()) {            // 5.7
//        fprintf(fout, "SPACINGTABLE ORTHOGONAL");
//        ortho = layer->lefiLayer::orthogonal();
//        for (i = 0; i < ortho->lefiOrthogonal::numOrthogonal(); i++) {
//            fprintf(fout, "\n   WITHIN %g SPACING %g",
//                    ortho->lefiOrthogonal::cutWithin(i),
//                    ortho->lefiOrthogonal::orthoSpacing(i));
//        }
//        fprintf(fout, ";\n");
//    }
//    for (i = 0; i < layer->lefiLayer::numEnclosure(); i++) {
//        fprintf(fout, "ENCLOSURE ");
//        if (layer->lefiLayer::hasEnclosureRule(i))
//            fprintf(fout, "%s ", layer->lefiLayer::enclosureRule(i));
//        fprintf(fout, "%g %g ", layer->lefiLayer::enclosureOverhang1(i),
//                layer->lefiLayer::enclosureOverhang2(i));
//        if (layer->lefiLayer::hasEnclosureWidth(i))
//            fprintf(fout, "WIDTH %g ", layer->lefiLayer::enclosureMinWidth(i));
//        if (layer->lefiLayer::hasEnclosureExceptExtraCut(i))
//            fprintf(fout, "EXCEPTEXTRACUT %g ",
//                    layer->lefiLayer::enclosureExceptExtraCut(i));
//        if (layer->lefiLayer::hasEnclosureMinLength(i))
//            fprintf(fout, "LENGTH %g ", layer->lefiLayer::enclosureMinLength(i));
//        fprintf(fout, ";\n");
//    }
//    for (i = 0; i < layer->lefiLayer::numPreferEnclosure(); i++) {
//        fprintf(fout, "PREFERENCLOSURE ");
//        if (layer->lefiLayer::hasPreferEnclosureRule(i))
//            fprintf(fout, "%s ", layer->lefiLayer::preferEnclosureRule(i));
//        fprintf(fout, "%g %g ", layer->lefiLayer::preferEnclosureOverhang1(i),
//                layer->lefiLayer::preferEnclosureOverhang2(i));
//        if (layer->lefiLayer::hasPreferEnclosureWidth(i))
//            fprintf(fout, "WIDTH %g ",layer->lefiLayer::preferEnclosureMinWidth(i));
//        fprintf(fout, ";\n");
//    }
//    if (layer->lefiLayer::hasResistancePerCut())
//        fprintf(fout, "  RESISTANCE %g ;\n",
//                layer->lefiLayer::resistancePerCut());
//    if (layer->lefiLayer::hasCurrentDensityPoint())
//        fprintf(fout, "  CURRENTDEN %g ;\n",
//                layer->lefiLayer::currentDensityPoint());
//    if (layer->lefiLayer::hasCurrentDensityArray()) {
//        layer->lefiLayer::currentDensityArray(&numPoints, &widths, &current);
//        for (i = 0; i < numPoints; i++)
//            fprintf(fout, "  CURRENTDEN ( %g %g ) ;\n", widths[i], current[i]);
//    }
//    if (layer->lefiLayer::hasDirection())
//        fprintf(fout, "  DIRECTION %s ;\n", layer->lefiLayer::direction());
//    if (layer->lefiLayer::hasResistance())
//        fprintf(fout, "  RESISTANCE RPERSQ %g ;\n",
//                layer->lefiLayer::resistance());
//    if (layer->lefiLayer::hasCapacitance())
//        fprintf(fout, "  CAPACITANCE CPERSQDIST %g ;\n",
//                layer->lefiLayer::capacitance());
//    if (layer->lefiLayer::hasEdgeCap())
//        fprintf(fout, "  EDGECAPACITANCE %g ;\n", layer->lefiLayer::edgeCap());
//    if (layer->lefiLayer::hasHeight())
//        fprintf(fout, "  TYPE %g ;\n", layer->lefiLayer::height());
//    if (layer->lefiLayer::hasThickness())
//        fprintf(fout, "  THICKNESS %g ;\n", layer->lefiLayer::thickness());
//    if (layer->lefiLayer::hasWireExtension())
//        fprintf(fout, "  WIREEXTENSION %g ;\n", layer->lefiLayer::wireExtension());
//    if (layer->lefiLayer::hasShrinkage())
//        fprintf(fout, "  SHRINKAGE %g ;\n", layer->lefiLayer::shrinkage());
//    if (layer->lefiLayer::hasCapMultiplier())
//        fprintf(fout, "  CAPMULTIPLIER %g ;\n", layer->lefiLayer::capMultiplier());
//    if (layer->lefiLayer::hasAntennaArea())
//        fprintf(fout, "  ANTENNAAREAFACTOR %g ;\n",
//                layer->lefiLayer::antennaArea());
//    if (layer->lefiLayer::hasAntennaLength())
//        fprintf(fout, "  ANTENNALENGTHFACTOR %g ;\n",
//                layer->lefiLayer::antennaLength());

//    // 5.5 AntennaModel
//    for (i = 0; i < layer->lefiLayer::numAntennaModel(); i++) {
//        aModel = layer->lefiLayer::antennaModel(i);

//        fprintf(fout, "  ANTENNAMODEL %s ;\n",
//                aModel->lefiAntennaModel::antennaOxide());

//        if (aModel->lefiAntennaModel::hasAntennaAreaRatio())
//            fprintf(fout, "  ANTENNAAREARATIO %g ;\n",
//                    aModel->lefiAntennaModel::antennaAreaRatio());
//        if (aModel->lefiAntennaModel::hasAntennaDiffAreaRatio())
//            fprintf(fout, "  ANTENNADIFFAREARATIO %g ;\n",
//                    aModel->lefiAntennaModel::antennaDiffAreaRatio());
//        else if (aModel->lefiAntennaModel::hasAntennaDiffAreaRatioPWL()) {
//            pwl = aModel->lefiAntennaModel::antennaDiffAreaRatioPWL();
//            fprintf(fout, "  ANTENNADIFFAREARATIO PWL ( ");
//            for (j = 0; j < pwl->lefiAntennaPWL::numPWL(); j++)
//                fprintf(fout, "( %g %g ) ", pwl->lefiAntennaPWL::PWLdiffusion(j),
//                        pwl->lefiAntennaPWL::PWLratio(j));
//            fprintf(fout, ") ;\n");
//        }
//        if (aModel->lefiAntennaModel::hasAntennaCumAreaRatio())
//            fprintf(fout, "  ANTENNACUMAREARATIO %g ;\n",
//                    aModel->lefiAntennaModel::antennaCumAreaRatio());
//        if (aModel->lefiAntennaModel::hasAntennaCumDiffAreaRatio())
//            fprintf(fout, "  ANTENNACUMDIFFAREARATIO %g\n",
//                    aModel->lefiAntennaModel::antennaCumDiffAreaRatio());
//        if (aModel->lefiAntennaModel::hasAntennaCumDiffAreaRatioPWL()) {
//            pwl = aModel->lefiAntennaModel::antennaCumDiffAreaRatioPWL();
//            fprintf(fout, "  ANTENNACUMDIFFAREARATIO PWL ( ");
//            for (j = 0; j < pwl->lefiAntennaPWL::numPWL(); j++)
//                fprintf(fout, "( %g %g ) ", pwl->lefiAntennaPWL::PWLdiffusion(j),
//                        pwl->lefiAntennaPWL::PWLratio(j));
//            fprintf(fout, ") ;\n");
//        }
//        if (aModel->lefiAntennaModel::hasAntennaAreaFactor()) {
//            fprintf(fout, "  ANTENNAAREAFACTOR %g ",
//                    aModel->lefiAntennaModel::antennaAreaFactor());
//            if (aModel->lefiAntennaModel::hasAntennaAreaFactorDUO())
//                fprintf(fout, "  DIFFUSEONLY ");
//            fprintf(fout, ";\n");
//        }
//        if (aModel->lefiAntennaModel::hasAntennaSideAreaRatio())
//            fprintf(fout, "  ANTENNASIDEAREARATIO %g ;\n",
//                    aModel->lefiAntennaModel::antennaSideAreaRatio());
//        if (aModel->lefiAntennaModel::hasAntennaDiffSideAreaRatio())
//            fprintf(fout, "  ANTENNADIFFSIDEAREARATIO %g\n",
//                    aModel->lefiAntennaModel::antennaDiffSideAreaRatio());
//        else if (aModel->lefiAntennaModel::hasAntennaDiffSideAreaRatioPWL()) {
//            pwl = aModel->lefiAntennaModel::antennaDiffSideAreaRatioPWL();
//            fprintf(fout, "  ANTENNADIFFSIDEAREARATIO PWL ( ");
//            for (j = 0; j < pwl->lefiAntennaPWL::numPWL(); j++)
//                fprintf(fout, "( %g %g ) ", pwl->lefiAntennaPWL::PWLdiffusion(j),
//                        pwl->lefiAntennaPWL::PWLratio(j));
//            fprintf(fout, ") ;\n");
//        }
//        if (aModel->lefiAntennaModel::hasAntennaCumSideAreaRatio())
//            fprintf(fout, "  ANTENNACUMSIDEAREARATIO %g ;\n",
//                    aModel->lefiAntennaModel::antennaCumSideAreaRatio());
//        if (aModel->lefiAntennaModel::hasAntennaCumDiffSideAreaRatio())
//            fprintf(fout, "  ANTENNACUMDIFFSIDEAREARATIO %g\n",
//                    aModel->lefiAntennaModel::antennaCumDiffSideAreaRatio());
//        else if (aModel->lefiAntennaModel::hasAntennaCumDiffSideAreaRatioPWL()) {
//            pwl = aModel->lefiAntennaModel::antennaCumDiffSideAreaRatioPWL();
//            fprintf(fout, "  ANTENNACUMDIFFSIDEAREARATIO PWL ( ");
//            for (j = 0; j < pwl->lefiAntennaPWL::numPWL(); j++)
//                fprintf(fout, "( %g %g ) ", pwl->lefiAntennaPWL::PWLdiffusion(j),
//                        pwl->lefiAntennaPWL::PWLratio(j));
//            fprintf(fout, ") ;\n");
//        }
//        if (aModel->lefiAntennaModel::hasAntennaSideAreaFactor()) {
//            fprintf(fout, "  ANTENNASIDEAREAFACTOR %g ",
//                    aModel->lefiAntennaModel::antennaSideAreaFactor());
//            if (aModel->lefiAntennaModel::hasAntennaSideAreaFactorDUO())
//                fprintf(fout, "  DIFFUSEONLY ");
//            fprintf(fout, ";\n");
//        }
//        if (aModel->lefiAntennaModel::hasAntennaCumRoutingPlusCut())
//            fprintf(fout, "  ANTENNACUMROUTINGPLUSCUT ;\n");
//        if (aModel->lefiAntennaModel::hasAntennaGatePlusDiff())
//            fprintf(fout, "  ANTENNAGATEPLUSDIFF %g ;\n",
//                    aModel->lefiAntennaModel::antennaGatePlusDiff());
//        if (aModel->lefiAntennaModel::hasAntennaAreaMinusDiff())
//            fprintf(fout, "  ANTENNAAREAMINUSDIFF %g ;\n",
//                    aModel->lefiAntennaModel::antennaAreaMinusDiff());
//        if (aModel->lefiAntennaModel::hasAntennaAreaDiffReducePWL()) {
//            pwl = aModel->lefiAntennaModel::antennaAreaDiffReducePWL();
//            fprintf(fout, "  ANTENNAAREADIFFREDUCEPWL ( ");
//            for (j = 0; j < pwl->lefiAntennaPWL::numPWL(); j++)
//                fprintf(fout, "( %g %g ) ", pwl->lefiAntennaPWL::PWLdiffusion(j),
//                        pwl->lefiAntennaPWL::PWLratio(j));
//            fprintf(fout, ") ;\n");
//        }
//    }

//    if (layer->lefiLayer::numAccurrentDensity()) {
//        for (i = 0; i < layer->lefiLayer::numAccurrentDensity(); i++) {
//            density = layer->lefiLayer::accurrent(i);
//            fprintf(fout, "  ACCURRENTDENSITY %s", density->type());
//            if (density->hasOneEntry())
//                fprintf(fout, " %g ;\n", density->oneEntry());
//            else {
//                fprintf(fout, "\n");
//                if (density->numFrequency()) {
//                    fprintf(fout, "    FREQUENCY");
//                    for (j = 0; j < density->numFrequency(); j++)
//                        fprintf(fout, " %g", density->frequency(j));
//                    fprintf(fout, " ;\n");
//                }
//                if (density->numCutareas()) {
//                    fprintf(fout, "    CUTAREA");
//                    for (j = 0; j < density->numCutareas(); j++)
//                        fprintf(fout, " %g", density->cutArea(j));
//                    fprintf(fout, " ;\n");
//                }
//                if (density->numWidths()) {
//                    fprintf(fout, "    WIDTH");
//                    for (j = 0; j < density->numWidths(); j++)
//                        fprintf(fout, " %g", density->width(j));
//                    fprintf(fout, " ;\n");
//                }
//                if (density->numTableEntries()) {
//                    k = 5;
//                    fprintf(fout, "    TABLEENTRIES");
//                    for (j = 0; j < density->numTableEntries(); j++)
//                        if (k > 4) {
//                            fprintf(fout, "\n     %g", density->tableEntry(j));
//                            k = 1;
//                        } else {
//                            fprintf(fout, " %g", density->tableEntry(j));
//                            k++;
//                        }
//                    fprintf(fout, " ;\n");
//                }
//            }
//        }
//    }
//    if (layer->lefiLayer::numDccurrentDensity()) {
//        for (i = 0; i < layer->lefiLayer::numDccurrentDensity(); i++) {
//            density = layer->lefiLayer::dccurrent(i);
//            fprintf(fout, "  DCCURRENTDENSITY %s", density->type());
//            if (density->hasOneEntry())
//                fprintf(fout, " %g ;\n", density->oneEntry());
//            else {
//                fprintf(fout, "\n");
//                if (density->numCutareas()) {
//                    fprintf(fout, "    CUTAREA");
//                    for (j = 0; j < density->numCutareas(); j++)
//                        fprintf(fout, " %g", density->cutArea(j));
//                    fprintf(fout, " ;\n");
//                }
//                if (density->numWidths()) {
//                    fprintf(fout, "    WIDTH");
//                    for (j = 0; j < density->numWidths(); j++)
//                        fprintf(fout, " %g", density->width(j));
//                    fprintf(fout, " ;\n");
//                }
//                if (density->numTableEntries()) {
//                    fprintf(fout, "    TABLEENTRIES");
//                    for (j = 0; j < density->numTableEntries(); j++)
//                        fprintf(fout, " %g", density->tableEntry(j));
//                    fprintf(fout, " ;\n");
//                }
//            }
//        }
//    }

//    for (i = 0; i < layer->lefiLayer::numSpacingTable(); i++) {
//        spTable = layer->lefiLayer::spacingTable(i);
//        fprintf(fout, "   SPACINGTABLE\n");
//        if (spTable->lefiSpacingTable::isInfluence()) {
//            influence = spTable->lefiSpacingTable::influence();
//            fprintf(fout, "      INFLUENCE");
//            for (j = 0; j < influence->lefiInfluence::numInfluenceEntry(); j++) {
//                fprintf(fout, "\n          WIDTH %g WITHIN %g SPACING %g",
//                        influence->lefiInfluence::width(j),
//                        influence->lefiInfluence::distance(j),
//                        influence->lefiInfluence::spacing(j));
//            }
//            fprintf(fout, " ;\n");
//        } else if (spTable->lefiSpacingTable::isParallel()){
//            parallel = spTable->lefiSpacingTable::parallel();
//            fprintf(fout, "      PARALLELRUNLENGTH");
//            for (j = 0; j < parallel->lefiParallel::numLength(); j++) {
//                fprintf(fout, " %g", parallel->lefiParallel::length(j));
//            }
//            for (j = 0; j < parallel->lefiParallel::numWidth(); j++) {
//                fprintf(fout, "\n          WIDTH %g",
//                        parallel->lefiParallel::width(j));
//                for (k = 0; k < parallel->lefiParallel::numLength(); k++) {
//                    fprintf(fout, " %g", parallel->lefiParallel::widthSpacing(j, k));
//                }
//            }
//            fprintf(fout, " ;\n");
//        } else {    // 5.7 TWOWIDTHS
//            twoWidths = spTable->lefiSpacingTable::twoWidths();
//            fprintf(fout, "      TWOWIDTHS");
//            for (j = 0; j < twoWidths->lefiTwoWidths::numWidth(); j++) {
//                fprintf(fout, "\n          WIDTH %g ",
//                        twoWidths->lefiTwoWidths::width(j));
//                if (twoWidths->lefiTwoWidths::hasWidthPRL(j))
//                    fprintf(fout, "PRL %g ", twoWidths->lefiTwoWidths::widthPRL(j));
//                for (k = 0; k < twoWidths->lefiTwoWidths::numWidthSpacing(j); k++)
//                    fprintf(fout, "%g ",twoWidths->lefiTwoWidths::widthSpacing(j, k));
//            }
//            fprintf(fout, " ;\n");
//        }
//    }

//    propNum = layer->lefiLayer::numProps();
//    if (propNum > 0) {
//        fprintf(fout, "  PROPERTY ");
//        for (i = 0; i < propNum; i++) {
//            // value can either be a string or number
//            fprintf(fout, "%s ", layer->lefiLayer::propName(i));
//            if (layer->lefiLayer::propIsNumber(i))
//                fprintf(fout, "%g ", layer->lefiLayer::propNumber(i));
//            if (layer->lefiLayer::propIsString(i))
//                fprintf(fout, "%s ", layer->lefiLayer::propValue(i));
//            pType = layer->lefiLayer::propType(i);
//            switch (pType) {
//            case 'R': fprintf(fout, "REAL ");
//                break;
//            case 'I': fprintf(fout, "INTEGER ");
//                break;
//            case 'S': fprintf(fout, "STRING ");
//                break;
//            case 'Q': fprintf(fout, "QUOTESTRING ");
//                break;
//            case 'N': fprintf(fout, "NUMBER ");
//                break;
//            }
//        }
//        fprintf(fout, ";\n");
//    }
//    if (layer->lefiLayer::hasDiagMinEdgeLength())
//        fprintf(fout, "  DIAGMINEDGELENGTH %g ;\n",
//                layer->lefiLayer::diagMinEdgeLength());
//    if (layer->lefiLayer::numMinSize()) {
//        fprintf(fout, "  MINSIZE ");
//        for (i = 0; i < layer->lefiLayer::numMinSize(); i++) {
//            fprintf(fout, "%g %g ", layer->lefiLayer::minSizeWidth(i),
//                    layer->lefiLayer::minSizeLength(i));
//        }
//        fprintf(fout, ";\n");
//    }

//    fprintf(fout, "END %s\n", layer->lefiLayer::name());

//    // Set it to case sensitive from here on
//    lefrSetCaseSensitivity(1);

//    return 0;
}

int macroBeginCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud) {

    // --------------------------------------------------
    checkType(c);
    if ((long)ud != userData) dataError();

    _cir->addMaster( Module() );
    _cir->master(masterCount).setName( macroName );
    return 0;
    // --------------------------------------------------
//    checkType(c);
//    if ((long)ud != userData) dataError();
//    fprintf(fout, "MACRO %s\n",  macroName);
//    return 0;
}

int macroFixedMaskCB(lefrCallbackType_e c, int fixedMask, lefiUserData ud) {
    checkType(c);
    if(ud){} if(fixedMask){}
    return 0;
}

int macroClassTypeCB(lefrCallbackType_e c, const char* macroClassType,
                     lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (macroClassType) {}

    _cir->master( masterCount ).setClassType( macroClassType );

    // fprintf(fout, "MACRO CLASS %s\n",  macroClassType);
    return 0;
}

int macroOriginCB(lefrCallbackType_e c, lefiNum macroNum,
                  lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    if( macroNum.x != 0.0 || macroNum.y != 0.0 ){
        cerr << "[ERROR] macro origin not in (0,0)." << endl;
        exit(0);
    }
//    cir->master( masterCount ).setOrigin( macroNum.x, macroNum.y );

    // fprintf(fout, "  ORIGIN ( %g %g ) ;\n", macroNum.x, macroNum.y);
    return 0;
}

int macroSizeCB(lefrCallbackType_e c, lefiNum macroNum,
                lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    _cir->master( masterCount ).setWidth( macroNum.x );
    _cir->master( masterCount ).setHeight( macroNum.y );

    // fprintf(fout, "  SIZE %g BY %g ;\n", macroNum.x, macroNum.y);
    return 0;
}

int macroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData ud) {

    // --------------------------------------------------
    checkType(c);
    if ((long)ud != userData) dataError();

    Module &curmaster = _cir->master(masterCount);

//    curmaster.setClassType( macro->lefiMacro::macroClass() );
//    curmaster.setOrigin( macro->lefiMacro::originX(), macro->lefiMacro::originY() );
//    curmaster.setWidth( macro->lefiMacro::sizeX() );
//    curmaster.setHeight( macro->lefiMacro::sizeY() );

    // edge-type rule
    if( macro->lefiMacro::numProperties() == 1 ){
        char *properties    = const_cast<char *>( macro->lefiMacro::propValue(0) );
        const char *d       = " ";
        char *temp          = strtok( properties, d );
        size_t idLeft       = 0, idRight = 0;
        while( temp ){
            string st(temp);
            if( st=="LEFT" ){
                temp = strtok( NULL, d );
                idLeft = atoi( temp ); }
            else if( st=="RIGHT" ){
                temp = strtok( NULL, d );
                idRight = atoi( temp ); }
            temp = strtok( NULL, d );
        }
        curmaster.setEdgeTypes( idLeft, idRight );
        //cout<<" (idLeft, idRight) = ("<<idLeft<<", "<<idRight<<") \n";
    }

    if( curmaster.numRects()==0 )
        curmaster.addRect( Rect( 0, 0, curmaster.width(), curmaster.height() ) );

    if( isShowLEFMsg )
        curmaster.showInfo();

    ++masterCount;
    return 0;
    // --------------------------------------------------

//    lefiSitePattern* pattern;
//    int              propNum, i, hasPrtSym = 0;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    if (macro->lefiMacro::hasClass())
//        fprintf(fout, "  CLASS %s ;\n", macro->lefiMacro::macroClass());
//    if (macro->lefiMacro::isFixedMask())
//        fprintf(fout, "  FIXEDMASK ;\n");
//    if (macro->lefiMacro::hasEEQ())
//        fprintf(fout, "  EEQ %s ;\n", macro->lefiMacro::EEQ());
//    if (macro->lefiMacro::hasLEQ())
//        fprintf(fout, "  LEQ %s ;\n", macro->lefiMacro::LEQ());
//    if (macro->lefiMacro::hasSource())
//        fprintf(fout, "  SOURCE %s ;\n", macro->lefiMacro::source());
//    if (macro->lefiMacro::hasXSymmetry()) {
//        fprintf(fout, "  SYMMETRY X ");
//        hasPrtSym = 1;
//    }
//    if (macro->lefiMacro::hasYSymmetry()) {   // print X Y & R90 in one line
//        if (!hasPrtSym) {
//            fprintf(fout, "  SYMMETRY Y ");
//            hasPrtSym = 1;
//        }
//        else
//            fprintf(fout, "Y ");
//    }
//    if (macro->lefiMacro::has90Symmetry()) {
//        if (!hasPrtSym) {
//            fprintf(fout, "  SYMMETRY R90 ");
//            hasPrtSym = 1;
//        }
//        else
//            fprintf(fout, "R90 ");
//    }
//    if (hasPrtSym) {
//        fprintf (fout, ";\n");
//        hasPrtSym = 0;
//    }
//    if (macro->lefiMacro::hasSiteName())
//        fprintf(fout, "  SITE %s ;\n", macro->lefiMacro::siteName());
//    if (macro->lefiMacro::hasSitePattern()) {
//        for (i = 0; i < macro->lefiMacro::numSitePattern(); i++ ) {
//            pattern = macro->lefiMacro::sitePattern(i);
//            if (pattern->lefiSitePattern::hasStepPattern()) {
//                fprintf(fout, "  SITE %s %g %g %s DO %g BY %g STEP %g %g ;\n",
//                        pattern->lefiSitePattern::name(), pattern->lefiSitePattern::x(),
//                        pattern->lefiSitePattern::y(),
//                        orientStr(pattern->lefiSitePattern::orient()),
//                        pattern->lefiSitePattern::xStart(),
//                        pattern->lefiSitePattern::yStart(),
//                        pattern->lefiSitePattern::xStep(),
//                        pattern->lefiSitePattern::yStep());
//            } else {
//                fprintf(fout, "  SITE %s %g %g %s ;\n",
//                        pattern->lefiSitePattern::name(), pattern->lefiSitePattern::x(),
//                        pattern->lefiSitePattern::y(),
//                        orientStr(pattern->lefiSitePattern::orient()));
//            }
//        }
//    }
//    if (macro->lefiMacro::hasSize())
//        fprintf(fout, "  SIZE %g BY %g ;\n", macro->lefiMacro::sizeX(),
//                macro->lefiMacro::sizeY());

//    if (macro->lefiMacro::hasForeign()) {
//        for (i = 0; i < macro->lefiMacro::numForeigns(); i++) {
//            fprintf(fout, "  FOREIGN %s ", macro->lefiMacro::foreignName(i));
//            if (macro->lefiMacro::hasForeignPoint(i)) {
//                fprintf(fout, "( %g %g ) ", macro->lefiMacro::foreignX(i),
//                        macro->lefiMacro::foreignY(i));
//                if (macro->lefiMacro::hasForeignOrient(i))
//                    fprintf(fout, "%s ", macro->lefiMacro::foreignOrientStr(i));
//            }
//            fprintf(fout, ";\n");
//        }
//    }
//    if (macro->lefiMacro::hasOrigin())
//        fprintf(fout, "  ORIGIN ( %g %g ) ;\n", macro->lefiMacro::originX(),
//                macro->lefiMacro::originY());
//    if (macro->lefiMacro::hasPower())
//        fprintf(fout, "  POWER %g ;\n", macro->lefiMacro::power());
//    propNum = macro->lefiMacro::numProperties();
//    if (propNum > 0) {
//        fprintf(fout, "  PROPERTY ");
//        for (i = 0; i < propNum; i++) {
//            // value can either be a string or number
//            if (macro->lefiMacro::propValue(i)) {
//                fprintf(fout, "%s %s ", macro->lefiMacro::propName(i),
//                        macro->lefiMacro::propValue(i));
//            }
//            else
//                fprintf(fout, "%s %g ", macro->lefiMacro::propName(i),
//                        macro->lefiMacro::propNum(i));

//            switch (macro->lefiMacro::propType(i)) {
//            case 'R': fprintf(fout, "REAL ");
//                break;
//            case 'I': fprintf(fout, "INTEGER ");
//                break;
//            case 'S': fprintf(fout, "STRING ");
//                break;
//            case 'Q': fprintf(fout, "QUOTESTRING ");
//                break;
//            case 'N': fprintf(fout, "NUMBER ");
//                break;
//            }
//        }
//        fprintf(fout, ";\n");
//    }
//    //fprintf(fout, "END %s\n", macro->lefiMacro::name());
//    return 0;
}

int macroEndCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (macroName) {}
//    fprintf(fout, "END %s\n", macroName);
    return 0;
}

int manufacturingCB(lefrCallbackType_e c, double num, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (num) {}
//    fprintf(fout, "MANUFACTURINGGRID %g ;\n", num);
    return 0;
}

int maxStackViaCB(lefrCallbackType_e c, lefiMaxStackVia* maxStack,
                  lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (maxStack->lefiMaxStackVia::maxStackVia()) {}
//    fprintf(fout, "MAXVIASTACK %d ", maxStack->lefiMaxStackVia::maxStackVia());
//    if (maxStack->lefiMaxStackVia::hasMaxStackViaRange())
//        fprintf(fout, "RANGE %s %s ",
//                maxStack->lefiMaxStackVia::maxStackViaBottomLayer(),
//                maxStack->lefiMaxStackVia::maxStackViaTopLayer());
//    fprintf(fout, ";\n");
    return 0;
}

int minFeatureCB(lefrCallbackType_e c, lefiMinFeature* min, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "MINFEATURE %g %g ;\n", min->lefiMinFeature::one(),
            min->lefiMinFeature::two());
    return 0;
}

int nonDefaultCB(lefrCallbackType_e c, lefiNonDefault* def, lefiUserData ud) {
    int          i;
    lefiVia*     via;
    lefiSpacing* spacing;

    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "NONDEFAULTRULE %s\n", def->lefiNonDefault::name());
    if (def->lefiNonDefault::hasHardspacing())
        fprintf(fout, "  HARDSPACING ;\n");
    for (i = 0; i < def->lefiNonDefault::numLayers(); i++) {
        fprintf(fout, "  LAYER %s\n", def->lefiNonDefault::layerName(i));
        if (def->lefiNonDefault::hasLayerWidth(i))
            fprintf(fout, "    WIDTH %g ;\n", def->lefiNonDefault::layerWidth(i));
        if (def->lefiNonDefault::hasLayerSpacing(i))
            fprintf(fout, "    SPACING %g ;\n",
                    def->lefiNonDefault::layerSpacing(i));
        if (def->lefiNonDefault::hasLayerDiagWidth(i))
            fprintf(fout, "    DIAGWIDTH %g ;\n",
                    def->lefiNonDefault::layerDiagWidth(i));
        if (def->lefiNonDefault::hasLayerWireExtension(i))
            fprintf(fout, "    WIREEXTENSION %g ;\n",
                    def->lefiNonDefault::layerWireExtension(i));
        if (def->lefiNonDefault::hasLayerResistance(i))
            fprintf(fout, "    RESISTANCE RPERSQ %g ;\n",
                    def->lefiNonDefault::layerResistance(i));
        if (def->lefiNonDefault::hasLayerCapacitance(i))
            fprintf(fout, "    CAPACITANCE CPERSQDIST %g ;\n",
                    def->lefiNonDefault::layerCapacitance(i));
        if (def->lefiNonDefault::hasLayerEdgeCap(i))
            fprintf(fout, "    EDGECAPACITANCE %g ;\n",
                    def->lefiNonDefault::layerEdgeCap(i));
        fprintf(fout, "  END %s\n", def->lefiNonDefault::layerName(i));
    }

    // handle via in nondefaultrule
    for (i = 0; i < def->lefiNonDefault::numVias(); i++) {
        via = def->lefiNonDefault::viaRule(i);
        lefVia(via);
    }

    // handle spacing in nondefaultrule
    for (i = 0; i < def->lefiNonDefault::numSpacingRules(); i++) {
        spacing = def->lefiNonDefault::spacingRule(i);
        lefSpacing(spacing);
    }

    // handle usevia
    for (i = 0; i < def->lefiNonDefault::numUseVia(); i++)
        fprintf(fout, "    USEVIA %s ;\n", def->lefiNonDefault::viaName(i));

    // handle useviarule
    for (i = 0; i < def->lefiNonDefault::numUseViaRule(); i++)
        fprintf(fout, "    USEVIARULE %s ;\n",
                def->lefiNonDefault::viaRuleName(i));

    // handle mincuts
    for (i = 0; i < def->lefiNonDefault::numMinCuts(); i++) {
        fprintf(fout, "   MINCUTS %s %d ;\n", def->lefiNonDefault::cutLayerName(i),
                def->lefiNonDefault::numCuts(i));
    }

    // handle property in nondefaultrule
    if (def->lefiNonDefault::numProps() > 0) {
        fprintf(fout, "   PROPERTY ");
        for (i = 0; i < def->lefiNonDefault::numProps(); i++) {
            fprintf(fout, "%s ", def->lefiNonDefault::propName(i));
            if (def->lefiNonDefault::propIsNumber(i))
                fprintf(fout, "%g ", def->lefiNonDefault::propNumber(i));
            if (def->lefiNonDefault::propIsString(i))
                fprintf(fout, "%s ", def->lefiNonDefault::propValue(i));
            switch(def->lefiNonDefault::propType(i)) {
            case 'R': fprintf(fout, "REAL ");
                break;
            case 'I': fprintf(fout, "INTEGER ");
                break;
            case 'S': fprintf(fout, "STRING ");
                break;
            case 'Q': fprintf(fout, "QUOTESTRING ");
                break;
            case 'N': fprintf(fout, "NUMBER ");
                break;
            }
        }
        fprintf(fout, ";\n");
    }
    fprintf(fout, "END %s ;\n", def->lefiNonDefault::name());

    return 0;
}

int obstructionCB(lefrCallbackType_e c, lefiObstruction* obs,
                  lefiUserData ud) {
    lefiGeometries* geometry;

    checkType(c);
    if ((long)ud != userData) dataError();
    //fprintf(fout, "  OBS\n");
    isCurObs = true;
    geometry = obs->lefiObstruction::geometries();
    prtGeometry(geometry);
    isCurObs = false;
    //fprintf(fout, "  END\n");
    return 0;
}

int pinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData ud) {
    // --------------------------------------------------
    checkType(c);
    if ((long)ud != userData) dataError();

    _cir->addMasterPin(Pin());

    Pin &curpin = _cir->masterPin( _cir->numMasterPins()-1 );
    string pinName = /*cir->master(masterCount).name() + "_" +*/ pin->lefiPin::name();
    curpin.setName( pinName );

    curpin.setModuleId( masterCount );

    lefiGeometries* geometry = pin->lefiPin::port( 0 );
    prtGeometry( geometry );

    // non-default rule (NDR) (specify the rule when tapering a wire to the pin)
    if (pin->lefiPin::hasTaperRule())
        curpin.setTaperRuleName( pin->lefiPin::taperRule() );

    // pin direction
    if( pin->lefiPin::hasDirection() )
        curpin.setDirStr( pin->lefiPin::direction() );

    if( isShowLEFMsg )
        curpin.showInfo();

    return 0;
    // --------------------------------------------------
//    int                  numPorts, i, j;
//    lefiGeometries*      geometry;
//    lefiPinAntennaModel* aModel;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    fprintf(fout, "  PIN %s\n", pin->lefiPin::name());
//    if (pin->lefiPin::hasForeign()) {
//        for (i = 0; i < pin->lefiPin::numForeigns(); i++) {
//            if (pin->lefiPin::hasForeignOrient(i))
//                fprintf(fout, "    FOREIGN %s STRUCTURE ( %g %g ) %s ;\n",
//                        pin->lefiPin::foreignName(i), pin->lefiPin::foreignX(i),
//                        pin->lefiPin::foreignY(i),
//                        pin->lefiPin::foreignOrientStr(i));
//            else if (pin->lefiPin::hasForeignPoint(i))
//                fprintf(fout, "    FOREIGN %s STRUCTURE ( %g %g ) ;\n",
//                        pin->lefiPin::foreignName(i), pin->lefiPin::foreignX(i),
//                        pin->lefiPin::foreignY(i));
//            else
//                fprintf(fout, "    FOREIGN %s ;\n", pin->lefiPin::foreignName(i));
//        }
//    }
//    if (pin->lefiPin::hasLEQ())
//        fprintf(fout, "    LEQ %s ;\n", pin->lefiPin::LEQ());
//    if (pin->lefiPin::hasDirection())
//        fprintf(fout, "    DIRECTION %s ;\n", pin->lefiPin::direction());
//    if (pin->lefiPin::hasUse())
//        fprintf(fout, "    USE %s ;\n", pin->lefiPin::use());
//    if (pin->lefiPin::hasShape())
//        fprintf(fout, "    SHAPE %s ;\n", pin->lefiPin::shape());
//    if (pin->lefiPin::hasMustjoin())
//        fprintf(fout, "    MUSTJOIN %s ;\n", pin->lefiPin::mustjoin());
//    if (pin->lefiPin::hasOutMargin())
//        fprintf(fout, "    OUTPUTNOISEMARGIN %g %g ;\n",
//                pin->lefiPin::outMarginHigh(), pin->lefiPin::outMarginLow());
//    if (pin->lefiPin::hasOutResistance())
//        fprintf(fout, "    OUTPUTRESISTANCE %g %g ;\n",
//                pin->lefiPin::outResistanceHigh(),
//                pin->lefiPin::outResistanceLow());
//    if (pin->lefiPin::hasInMargin())
//        fprintf(fout, "    INPUTNOISEMARGIN %g %g ;\n",
//                pin->lefiPin::inMarginHigh(), pin->lefiPin::inMarginLow());
//    if (pin->lefiPin::hasPower())
//        fprintf(fout, "    POWER %g ;\n", pin->lefiPin::power());
//    if (pin->lefiPin::hasLeakage())
//        fprintf(fout, "    LEAKAGE %g ;\n", pin->lefiPin::leakage());
//    if (pin->lefiPin::hasMaxload())
//        fprintf(fout, "    MAXLOAD %g ;\n", pin->lefiPin::maxload());
//    if (pin->lefiPin::hasCapacitance())
//        fprintf(fout, "    CAPACITANCE %g ;\n", pin->lefiPin::capacitance());
//    if (pin->lefiPin::hasResistance())
//        fprintf(fout, "    RESISTANCE %g ;\n", pin->lefiPin::resistance());
//    if (pin->lefiPin::hasPulldownres())
//        fprintf(fout, "    PULLDOWNRES %g ;\n", pin->lefiPin::pulldownres());
//    if (pin->lefiPin::hasTieoffr())
//        fprintf(fout, "    TIEOFFR %g ;\n", pin->lefiPin::tieoffr());
//    if (pin->lefiPin::hasVHI())
//        fprintf(fout, "    VHI %g ;\n", pin->lefiPin::VHI());
//    if (pin->lefiPin::hasVLO())
//        fprintf(fout, "    VLO %g ;\n", pin->lefiPin::VLO());
//    if (pin->lefiPin::hasRiseVoltage())
//        fprintf(fout, "    RISEVOLTAGETHRESHOLD %g ;\n",
//                pin->lefiPin::riseVoltage());
//    if (pin->lefiPin::hasFallVoltage())
//        fprintf(fout, "    FALLVOLTAGETHRESHOLD %g ;\n",
//                pin->lefiPin::fallVoltage());
//    if (pin->lefiPin::hasRiseThresh())
//        fprintf(fout, "    RISETHRESH %g ;\n", pin->lefiPin::riseThresh());
//    if (pin->lefiPin::hasFallThresh())
//        fprintf(fout, "    FALLTHRESH %g ;\n", pin->lefiPin::fallThresh());
//    if (pin->lefiPin::hasRiseSatcur())
//        fprintf(fout, "    RISESATCUR %g ;\n", pin->lefiPin::riseSatcur());
//    if (pin->lefiPin::hasFallSatcur())
//        fprintf(fout, "    FALLSATCUR %g ;\n", pin->lefiPin::fallSatcur());
//    if (pin->lefiPin::hasRiseSlewLimit())
//        fprintf(fout, "    RISESLEWLIMIT %g ;\n", pin->lefiPin::riseSlewLimit());
//    if (pin->lefiPin::hasFallSlewLimit())
//        fprintf(fout, "    FALLSLEWLIMIT %g ;\n", pin->lefiPin::fallSlewLimit());
//    if (pin->lefiPin::hasCurrentSource())
//        fprintf(fout, "    CURRENTSOURCE %s ;\n", pin->lefiPin::currentSource());
//    if (pin->lefiPin::hasTables())
//        fprintf(fout, "    IV_TABLES %s %s ;\n", pin->lefiPin::tableHighName(),
//                pin->lefiPin::tableLowName());
//    if (pin->lefiPin::hasTaperRule())
//        fprintf(fout, "    TAPERRULE %s ;\n", pin->lefiPin::taperRule());
//    if (pin->lefiPin::hasNetExpr())
//        fprintf(fout, "    NETEXPR \"%s\" ;\n", pin->lefiPin::netExpr());
//    if (pin->lefiPin::hasSupplySensitivity())
//        fprintf(fout, "    SUPPLYSENSITIVITY %s ;\n",
//                pin->lefiPin::supplySensitivity());
//    if (pin->lefiPin::hasGroundSensitivity())
//        fprintf(fout, "    GROUNDSENSITIVITY %s ;\n",
//                pin->lefiPin::groundSensitivity());
//    if (pin->lefiPin::hasAntennaSize()) {
//        for (i = 0; i < pin->lefiPin::numAntennaSize(); i++) {
//            fprintf(fout, "    ANTENNASIZE %g ", pin->lefiPin::antennaSize(i));
//            if (pin->lefiPin::antennaSizeLayer(i))
//                fprintf(fout, "LAYER %s ", pin->lefiPin::antennaSizeLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }
//    if (pin->lefiPin::hasAntennaMetalArea()) {
//        for (i = 0; i < pin->lefiPin::numAntennaMetalArea(); i++) {
//            fprintf(fout, "    ANTENNAMETALAREA %g ",
//                    pin->lefiPin::antennaMetalArea(i));
//            if (pin->lefiPin::antennaMetalAreaLayer(i))
//                fprintf(fout, "LAYER %s ", pin->lefiPin::antennaMetalAreaLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }
//    if (pin->lefiPin::hasAntennaMetalLength()) {
//        for (i = 0; i < pin->lefiPin::numAntennaMetalLength(); i++) {
//            fprintf(fout, "    ANTENNAMETALLENGTH %g ",
//                    pin->lefiPin::antennaMetalLength(i));
//            if (pin->lefiPin::antennaMetalLengthLayer(i))
//                fprintf(fout, "LAYER %s ", pin->lefiPin::antennaMetalLengthLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }

//    if (pin->lefiPin::hasAntennaPartialMetalArea()) {
//        for (i = 0; i < pin->lefiPin::numAntennaPartialMetalArea(); i++) {
//            fprintf(fout, "    ANTENNAPARTIALMETALAREA %g ",
//                    pin->lefiPin::antennaPartialMetalArea(i));
//            if (pin->lefiPin::antennaPartialMetalAreaLayer(i))
//                fprintf(fout, "LAYER %s ",
//                        pin->lefiPin::antennaPartialMetalAreaLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }

//    if (pin->lefiPin::hasAntennaPartialMetalSideArea()) {
//        for (i = 0; i < pin->lefiPin::numAntennaPartialMetalSideArea(); i++) {
//            fprintf(fout, "    ANTENNAPARTIALMETALSIDEAREA %g ",
//                    pin->lefiPin::antennaPartialMetalSideArea(i));
//            if (pin->lefiPin::antennaPartialMetalSideAreaLayer(i))
//                fprintf(fout, "LAYER %s ",
//                        pin->lefiPin::antennaPartialMetalSideAreaLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }

//    if (pin->lefiPin::hasAntennaPartialCutArea()) {
//        for (i = 0; i < pin->lefiPin::numAntennaPartialCutArea(); i++) {
//            fprintf(fout, "    ANTENNAPARTIALCUTAREA %g ",
//                    pin->lefiPin::antennaPartialCutArea(i));
//            if (pin->lefiPin::antennaPartialCutAreaLayer(i))
//                fprintf(fout, "LAYER %s ",
//                        pin->lefiPin::antennaPartialCutAreaLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }

//    if (pin->lefiPin::hasAntennaDiffArea()) {
//        for (i = 0; i < pin->lefiPin::numAntennaDiffArea(); i++) {
//            fprintf(fout, "    ANTENNADIFFAREA %g ",
//                    pin->lefiPin::antennaDiffArea(i));
//            if (pin->lefiPin::antennaDiffAreaLayer(i))
//                fprintf(fout, "LAYER %s ", pin->lefiPin::antennaDiffAreaLayer(i));
//            fprintf(fout, ";\n");
//        }
//    }

//    for (j = 0; j < pin->lefiPin::numAntennaModel(); j++) {
//        aModel = pin->lefiPin::antennaModel(j);

//        fprintf(fout, "    ANTENNAMODEL %s ;\n",
//                aModel->lefiPinAntennaModel::antennaOxide());

//        if (aModel->lefiPinAntennaModel::hasAntennaGateArea()) {
//            for (i = 0; i < aModel->lefiPinAntennaModel::numAntennaGateArea(); i++)
//            {
//                fprintf(fout, "    ANTENNAGATEAREA %g ",
//                        aModel->lefiPinAntennaModel::antennaGateArea(i));
//                if (aModel->lefiPinAntennaModel::antennaGateAreaLayer(i))
//                    fprintf(fout, "LAYER %s ",
//                            aModel->lefiPinAntennaModel::antennaGateAreaLayer(i));
//                fprintf(fout, ";\n");
//            }
//        }

//        if (aModel->lefiPinAntennaModel::hasAntennaMaxAreaCar()) {
//            for (i = 0; i < aModel->lefiPinAntennaModel::numAntennaMaxAreaCar();
//                 i++) {
//                fprintf(fout, "    ANTENNAMAXAREACAR %g ",
//                        aModel->lefiPinAntennaModel::antennaMaxAreaCar(i));
//                if (aModel->lefiPinAntennaModel::antennaMaxAreaCarLayer(i))
//                    fprintf(fout, "LAYER %s ",
//                            aModel->lefiPinAntennaModel::antennaMaxAreaCarLayer(i));
//                fprintf(fout, ";\n");
//            }
//        }

//        if (aModel->lefiPinAntennaModel::hasAntennaMaxSideAreaCar()) {
//            for (i = 0; i < aModel->lefiPinAntennaModel::numAntennaMaxSideAreaCar();
//                 i++) {
//                fprintf(fout, "    ANTENNAMAXSIDEAREACAR %g ",
//                        aModel->lefiPinAntennaModel::antennaMaxSideAreaCar(i));
//                if (aModel->lefiPinAntennaModel::antennaMaxSideAreaCarLayer(i))
//                    fprintf(fout, "LAYER %s ",
//                            aModel->lefiPinAntennaModel::antennaMaxSideAreaCarLayer(i));
//                fprintf(fout, ";\n");
//            }
//        }

//        if (aModel->lefiPinAntennaModel::hasAntennaMaxCutCar()) {
//            for (i = 0; i < aModel->lefiPinAntennaModel::numAntennaMaxCutCar(); i++)
//            {
//                fprintf(fout, "    ANTENNAMAXCUTCAR %g ",
//                        aModel->lefiPinAntennaModel::antennaMaxCutCar(i));
//                if (aModel->lefiPinAntennaModel::antennaMaxCutCarLayer(i))
//                    fprintf(fout, "LAYER %s ",
//                            aModel->lefiPinAntennaModel::antennaMaxCutCarLayer(i));
//                fprintf(fout, ";\n");
//            }
//        }
//    }

//    if (pin->lefiPin::numProperties() > 0) {
//        fprintf(fout, "    PROPERTY ");
//        for (i = 0; i < pin->lefiPin::numProperties(); i++) {
//            // value can either be a string or number
//            if (pin->lefiPin::propValue(i)) {
//                fprintf(fout, "%s %s ", pin->lefiPin::propName(i),
//                        pin->lefiPin::propValue(i));
//            }
//            else
//                fprintf(fout, "%s %g ", pin->lefiPin::propName(i),
//                        pin->lefiPin::propNum(i));
//            switch (pin->lefiPin::propType(i)) {
//            case 'R': fprintf(fout, "REAL ");
//                break;
//            case 'I': fprintf(fout, "INTEGER ");
//                break;
//            case 'S': fprintf(fout, "STRING ");
//                break;
//            case 'Q': fprintf(fout, "QUOTESTRING ");
//                break;
//            case 'N': fprintf(fout, "NUMBER ");
//                break;
//            }
//        }
//        fprintf(fout, ";\n");
//    }

//    numPorts = pin->lefiPin::numPorts();
//    for (i = 0; i < numPorts; i++) {
//        fprintf(fout,"    PORT\n");
//        geometry = pin->lefiPin::port(i);
//        prtGeometry(geometry);
//        fprintf(fout, "    END\n");
//    }
//    fprintf(fout, "  END %s\n", pin->lefiPin::name());
//    return 0;
}

int densityCB(lefrCallbackType_e c, lefiDensity* density,
              lefiUserData ud) {

    struct lefiGeomRect rect;

    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "  DENSITY\n");
    for (int i = 0; i < density->lefiDensity::numLayer(); i++) {
        fprintf(fout, "    LAYER %s ;\n", density->lefiDensity::layerName(i));
        for (int j = 0; j < density->lefiDensity::numRects(i); j++) {
            rect = density->lefiDensity::getRect(i,j);
            fprintf(fout, "      RECT %g %g %g %g ", rect.xl, rect.yl, rect.xh,
                    rect.yh);
            fprintf(fout, "%g ;\n", density->lefiDensity::densityValue(i,j));
        }
    }
    fprintf(fout, "  END\n");
    return 0;
}

int propDefBeginCB(lefrCallbackType_e c, void* ptr, lefiUserData ud) {

    checkType(c);
    if ((long)ud != userData) dataError();
    if (ptr) {}
//    fprintf(fout, "PROPERTYDEFINITIONS\n");
    return 0;
}

vector< vector<double> > Module::edgeTypeTable( Module::edgeTypeTableSize, vector<double>( Module::edgeTypeTableSize, 0 ) );
int propDefCB(lefrCallbackType_e c, lefiProp* prop, lefiUserData ud) {

    // --------------------------------------------------
    checkType(c);
    if ((long)ud != userData) dataError();

    if( strcmp( prop->lefiProp::propName(), "LEF58_CELLEDGESPACINGTABLE" ) == 0 ){
        char *properties    = const_cast<char *>(prop->lefiProp::string());
        const char *d       = " ";
        char *temp          = strtok( properties, d );

        while( temp ){
            string st(temp);
            if( st == "EDGETYPE" ){
                temp = strtok( NULL, d );
                size_t id1 = atoi( temp );
                temp = strtok( NULL, d );
                size_t id2 = atoi( temp );
                temp = strtok( NULL, d );
                double spacing = atof( temp );
                Module::edgeTypeTable[id1][id2] = spacing;
                Module::edgeTypeTable[id2][id1] = spacing;
            }
            temp = strtok( NULL, d );
        }
    }
    return 0;
    // --------------------------------------------------
//    checkType(c);
//    if ((long)ud != userData) dataError();
//    fprintf(fout, " %s %s", prop->lefiProp::propType(),
//            prop->lefiProp::propName());
//    switch(prop->lefiProp::dataType()) {
//    case 'I':
//        fprintf(fout, " INTEGER");
//        break;
//    case 'R':
//        fprintf(fout, " REAL");
//        break;
//    case 'S':
//        fprintf(fout, " STRING");
//        break;
//    }
//    if (prop->lefiProp::hasNumber())
//        fprintf(fout, " %g", prop->lefiProp::number());
//    if (prop->lefiProp::hasRange())
//        fprintf(fout, " RANGE %g %g", prop->lefiProp::left(),
//                prop->lefiProp::right());
//    if (prop->lefiProp::hasString())
//        fprintf(fout, " %s", prop->lefiProp::string());
//    fprintf(fout, "\n");
//    return 0;
}

int propDefEndCB(lefrCallbackType_e c, void* ptr, lefiUserData ud) {

    checkType(c);
    if ((long)ud != userData) dataError();
    if (ptr) {}
//    fprintf(fout, "END PROPERTYDEFINITIONS\n");
    return 0;
}

Site Row::site;

int siteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData ud) {

    // --------------------------------------------------
    checkType(c);
    if ((long)ud != userData) dataError();

    if( strcmp( site->lefiSite::siteClass(), "CORE" ) == 0 ||
        strcmp( site->lefiSite::siteClass(), "core" ) == 0 ){
        Row::site.setType( site->lefiSite::siteClass() );
        Row::site.setWidth( site->lefiSite::sizeX() );
        Row::site.setHeight( site->lefiSite::sizeY() );
    }
    if( isShowLEFMsg )
        Row::site.showInfo();

    return 0;
    // --------------------------------------------------
//    int hasPrtSym = 0;
//    int i;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    fprintf(fout, "SITE %s\n", site->lefiSite::name());
//    if (site->lefiSite::hasClass())
//        fprintf(fout, "  CLASS %s ;\n", site->lefiSite::siteClass());
//    if (site->lefiSite::hasXSymmetry()) {
//        fprintf(fout, "  SYMMETRY X ");
//        hasPrtSym = 1;
//    }
//    if (site->lefiSite::hasYSymmetry()) {
//        if (hasPrtSym)
//            fprintf(fout, "Y ");
//        else {
//            fprintf(fout, "  SYMMETRY Y ");
//            hasPrtSym = 1;
//        }
//    }
//    if (site->lefiSite::has90Symmetry()) {
//        if (hasPrtSym)
//            fprintf(fout, "R90 ");
//        else {
//            fprintf(fout, "  SYMMETRY R90 ");
//            hasPrtSym = 1;
//        }
//    }
//    if (hasPrtSym)
//        fprintf(fout, ";\n");
//    if (site->lefiSite::hasSize())
//        fprintf(fout, "  SIZE %g BY %g ;\n", site->lefiSite::sizeX(),
//                site->lefiSite::sizeY());

//    if (site->hasRowPattern()) {
//        fprintf(fout, "  ROWPATTERN ");
//        for (i = 0; i < site->lefiSite::numSites(); i++)
//            fprintf(fout, "  %s %s ", site->lefiSite::siteName(i),
//                    site->lefiSite::siteOrientStr(i));
//        fprintf(fout, ";\n");
//    }

//    fprintf(fout, "END %s\n", site->lefiSite::name());
//    return 0;
}

int spacingBeginCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
    checkType(c);
    if ((long)ud != userData) dataError();
    if (ptr) {}
    //fprintf(fout, "SPACING\n");
    return 0;
}

int spacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    lefSpacing(spacing);
    return 0;
}

int spacingEndCB(lefrCallbackType_e c, void* ptr, lefiUserData ud){
    checkType(c);
    if ((long)ud != userData) dataError();
    if (ptr) {}
    //fprintf(fout, "END SPACING\n");
    return 0;
}

int timingCB(lefrCallbackType_e c, lefiTiming* timing, lefiUserData ud) {
    int i;
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "TIMING\n");
    for (i = 0; i < timing->numFromPins(); i++)
        fprintf(fout, " FROMPIN %s ;\n", timing->fromPin(i));
    for (i = 0; i < timing->numToPins(); i++)
        fprintf(fout, " TOPIN %s ;\n", timing->toPin(i));
    fprintf(fout, " RISE SLEW1 %g %g %g %g ;\n", timing->riseSlewOne(),
            timing->riseSlewTwo(), timing->riseSlewThree(),
            timing->riseSlewFour());
    if (timing->hasRiseSlew2())
        fprintf(fout, " RISE SLEW2 %g %g %g ;\n", timing->riseSlewFive(),
                timing->riseSlewSix(), timing->riseSlewSeven());
    if (timing->hasFallSlew())
        fprintf(fout, " FALL SLEW1 %g %g %g %g ;\n", timing->fallSlewOne(),
                timing->fallSlewTwo(), timing->fallSlewThree(),
                timing->fallSlewFour());
    if (timing->hasFallSlew2())
        fprintf(fout, " FALL SLEW2 %g %g %g ;\n", timing->fallSlewFive(),
                timing->fallSlewSix(), timing->riseSlewSeven());
    if (timing->hasRiseIntrinsic()) {
        fprintf(fout, "TIMING RISE INTRINSIC %g %g ;\n",
                timing->riseIntrinsicOne(), timing->riseIntrinsicTwo());
        fprintf(fout, "TIMING RISE VARIABLE %g %g ;\n",
                timing->riseIntrinsicThree(), timing->riseIntrinsicFour());
    }
    if (timing->hasFallIntrinsic()) {
        fprintf(fout, "TIMING FALL INTRINSIC %g %g ;\n",
                timing->fallIntrinsicOne(), timing->fallIntrinsicTwo());
        fprintf(fout, "TIMING RISE VARIABLE %g %g ;\n",
                timing->fallIntrinsicThree(), timing->fallIntrinsicFour());
    }
    if (timing->hasRiseRS())
        fprintf(fout, "TIMING RISERS %g %g ;\n",
                timing->riseRSOne(), timing->riseRSTwo());
    if (timing->hasRiseCS())
        fprintf(fout, "TIMING RISECS %g %g ;\n",
                timing->riseCSOne(), timing->riseCSTwo());
    if (timing->hasFallRS())
        fprintf(fout, "TIMING FALLRS %g %g ;\n",
                timing->fallRSOne(), timing->fallRSTwo());
    if (timing->hasFallCS())
        fprintf(fout, "TIMING FALLCS %g %g ;\n",
                timing->fallCSOne(), timing->fallCSTwo());
    if (timing->hasUnateness())
        fprintf(fout, "TIMING UNATENESS %s ;\n", timing->unateness());
    if (timing->hasRiseAtt1())
        fprintf(fout, "TIMING RISESATT1 %g %g ;\n", timing->riseAtt1One(),
                timing->riseAtt1Two());
    if (timing->hasFallAtt1())
        fprintf(fout, "TIMING FALLSATT1 %g %g ;\n", timing->fallAtt1One(),
                timing->fallAtt1Two());
    if (timing->hasRiseTo())
        fprintf(fout, "TIMING RISET0 %g %g ;\n", timing->riseToOne(),
                timing->riseToTwo());
    if (timing->hasFallTo())
        fprintf(fout, "TIMING FALLT0 %g %g ;\n", timing->fallToOne(),
                timing->fallToTwo());
    if (timing->hasSDFonePinTrigger())
        fprintf(fout, " %s TABLEDIMENSION %g %g %g ;\n",
                timing->SDFonePinTriggerType(), timing->SDFtriggerOne(),
                timing->SDFtriggerTwo(), timing->SDFtriggerThree());
    if (timing->hasSDFtwoPinTrigger())
        fprintf(fout, " %s %s %s TABLEDIMENSION %g %g %g ;\n",
                timing->SDFtwoPinTriggerType(), timing->SDFfromTrigger(),
                timing->SDFtoTrigger(), timing->SDFtriggerOne(),
                timing->SDFtriggerTwo(), timing->SDFtriggerThree());
    fprintf(fout, "END TIMING\n");
    return 0;
}

int unitsCB(lefrCallbackType_e c, lefiUnits* unit, lefiUserData ud) {

    // --------------------------------------------------
    checkType(c);
    if ((long)ud != userData) dataError();

    if( unit->lefiUnits::hasDatabase() ){
        if( strcmp( unit->lefiUnits::databaseName(), "MICRONS" )==0 ){
            unitLEF = unit->lefiUnits::databaseNumber();
        }
    }
    if( isShowLEFMsg )
        cout << "1 micron = " << unitLEF << " database." << endl;
    return 0;
    // --------------------------------------------------
//    checkType(c);
//    if ((long)ud != userData) dataError();
//    fprintf(fout, "UNITS\n");
//    if (unit->lefiUnits::hasDatabase())
//        fprintf(fout, "  DATABASE %s %g ;\n", unit->lefiUnits::databaseName(),
//                unit->lefiUnits::databaseNumber());
//    if (unit->lefiUnits::hasCapacitance())
//        fprintf(fout, "  CAPACITANCE PICOFARADS %g ;\n",
//                unit->lefiUnits::capacitance());
//    if (unit->lefiUnits::hasResistance())
//        fprintf(fout, "  RESISTANCE OHMS %g ;\n", unit->lefiUnits::resistance());
//    if (unit->lefiUnits::hasPower())
//        fprintf(fout, "  POWER MILLIWATTS %g ;\n", unit->lefiUnits::power());
//    if (unit->lefiUnits::hasCurrent())
//        fprintf(fout, "  CURRENT MILLIAMPS %g ;\n", unit->lefiUnits::current());
//    if (unit->lefiUnits::hasVoltage())
//        fprintf(fout, "  VOLTAGE VOLTS %g ;\n", unit->lefiUnits::voltage());
//    if (unit->lefiUnits::hasFrequency())
//        fprintf(fout, "  FREQUENCY MEGAHERTZ %g ;\n",
//                unit->lefiUnits::frequency());
//    fprintf(fout, "END UNITS\n");
//    return 0;
}

int useMinSpacingCB(lefrCallbackType_e c, lefiUseMinSpacing* spacing,
                    lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (spacing->lefiUseMinSpacing::name()) {}
//    fprintf(fout, "USEMINSPACING %s ", spacing->lefiUseMinSpacing::name());
//    if (spacing->lefiUseMinSpacing::value())
//        fprintf(fout, "ON ;\n");
//    else
//        fprintf(fout, "OFF ;\n");
    return 0;
}

int versionCB(lefrCallbackType_e c, double num, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (num) {}
//    fprintf(fout, "VERSION %g ;\n", num);
    return 0;
}

int versionStrCB(lefrCallbackType_e c, const char* versionName, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "VERSION %s ;\n", versionName);
    return 0;
}

int viaCB(lefrCallbackType_e c, lefiVia* via, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    lefVia(via);
    return 0;
}

int viaRuleCB(lefrCallbackType_e c, lefiViaRule* viaRule, lefiUserData ud) {
    // -------------------------------------------------- (ignore it) wlkb83
    checkType(c);
    if ((long)ud != userData) dataError();

    if( viaRule->lefiViaRule::name() ) {}
    return 0;
    // --------------------------------------------------
//    int               numLayers, numVias, i;
//    lefiViaRuleLayer* vLayer;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    fprintf(fout, "VIARULE %s", viaRule->lefiViaRule::name());
//    if (viaRule->lefiViaRule::hasGenerate())
//        fprintf(fout, " GENERATE");
//    if (viaRule->lefiViaRule::hasDefault())
//        fprintf(fout, " DEFAULT");
//    fprintf(fout, "\n");

//    numLayers = viaRule->lefiViaRule::numLayers();
//    // if numLayers == 2, it is VIARULE without GENERATE and has via name
//    // if numLayers == 3, it is VIARULE with GENERATE, and the 3rd layer is cut
//    for (i = 0; i < numLayers; i++) {
//        vLayer = viaRule->lefiViaRule::layer(i);
//        lefViaRuleLayer(vLayer);
//    }

//    if (numLayers == 2 && !(viaRule->lefiViaRule::hasGenerate())) {
//        // should have vianames
//        numVias = viaRule->lefiViaRule::numVias();
//        if (numVias == 0)
//            fprintf(fout, "Should have via names in VIARULE.\n");
//        else {
//            for (i = 0; i < numVias; i++)
//                fprintf(fout, "  VIA %s ;\n", viaRule->lefiViaRule::viaName(i));
//        }
//    }
//    if (viaRule->lefiViaRule::numProps() > 0) {
//        fprintf(fout, "  PROPERTY ");
//        for (i = 0; i < viaRule->lefiViaRule::numProps(); i++) {
//            fprintf(fout, "%s ", viaRule->lefiViaRule::propName(i));
//            if (viaRule->lefiViaRule::propValue(i))
//                fprintf(fout, "%s ", viaRule->lefiViaRule::propValue(i));
//            switch (viaRule->lefiViaRule::propType(i)) {
//            case 'R': fprintf(fout, "REAL ");
//                break;
//            case 'I': fprintf(fout, "INTEGER ");
//                break;
//            case 'S': fprintf(fout, "STRING ");
//                break;
//            case 'Q': fprintf(fout, "QUOTESTRING ");
//                break;
//            case 'N': fprintf(fout, "NUMBER ");
//                break;
//            }
//        }
//        fprintf(fout, ";\n");
//    }
//    fprintf(fout, "END %s\n", viaRule->lefiViaRule::name());
//    return 0;
}

int extensionCB(lefrCallbackType_e c, const char* extsn, lefiUserData ud) {
    checkType(c);
    // lefrSetCaseSensitivity(0);
    if ((long)ud != userData) dataError();
    fprintf(fout, "BEGINEXT %s ;\n", extsn);
    // lefrSetCaseSensitivity(1);
    return 0;
}

int doneCB(lefrCallbackType_e c, void* ptr, lefiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (ptr) {}
//    fprintf(fout, "END LIBRARY\n");
    return 0;
}

void errorCB(const char* msg) {
    printf ("%s : %s\n", (char *)lefrGetUserData(), msg);
}

void warningCB(const char* msg) {
    printf ("%s : %s\n", (char *)lefrGetUserData(), msg);
}

void Parser::readLEF(const vector<string> &fileNames)
{
    isShowLEFMsg = gArg.checkExist( "LEFMsg" );
    isCurObs = false;
    FILE *f;
    fout = stdout;

    lefrSetAntennaInputCbk(antennaCB);
    lefrSetAntennaInoutCbk(antennaCB);
    lefrSetAntennaOutputCbk(antennaCB);
    lefrSetArrayBeginCbk(arrayBeginCB);
    lefrSetArrayCbk(arrayCB);
    lefrSetArrayEndCbk(arrayEndCB);
    lefrSetBusBitCharsCbk(busBitCharsCB);
    lefrSetCaseSensitiveCbk(caseSensCB);
    lefrSetFixedMaskCbk(fixedMaskCB);
    lefrSetClearanceMeasureCbk(clearanceCB);
    lefrSetDensityCbk(densityCB);
    lefrSetDividerCharCbk(dividerCB);
    lefrSetNoWireExtensionCbk(noWireExtCB);
    lefrSetNoiseMarginCbk(noiseMarCB);
    lefrSetEdgeRateThreshold1Cbk(edge1CB);
    lefrSetEdgeRateThreshold2Cbk(edge2CB);
    lefrSetEdgeRateScaleFactorCbk(edgeScaleCB);
    lefrSetExtensionCbk(extensionCB);
    lefrSetNoiseTableCbk(noiseTableCB);
    lefrSetCorrectionTableCbk(correctionCB);
    lefrSetDielectricCbk(dielectricCB);
    lefrSetIRDropBeginCbk(irdropBeginCB);
    lefrSetIRDropCbk(irdropCB);
    lefrSetIRDropEndCbk(irdropEndCB);
    lefrSetLayerCbk(layerCB);
    lefrSetLibraryEndCbk(doneCB);
    lefrSetMacroBeginCbk(macroBeginCB);
    lefrSetMacroCbk(macroCB);
    lefrSetMacroClassTypeCbk(macroClassTypeCB);
    lefrSetMacroOriginCbk(macroOriginCB);
    lefrSetMacroSizeCbk(macroSizeCB);
    lefrSetMacroFixedMaskCbk(macroFixedMaskCB);
    lefrSetMacroEndCbk(macroEndCB);
    lefrSetManufacturingCbk(manufacturingCB);
    lefrSetMaxStackViaCbk(maxStackViaCB);
    lefrSetMinFeatureCbk(minFeatureCB);
    lefrSetNonDefaultCbk(nonDefaultCB);
    lefrSetObstructionCbk(obstructionCB);
    lefrSetPinCbk(pinCB);
    lefrSetPropBeginCbk(propDefBeginCB);
    lefrSetPropCbk(propDefCB);
    lefrSetPropEndCbk(propDefEndCB);
    lefrSetSiteCbk(siteCB);
    lefrSetSpacingBeginCbk(spacingBeginCB);
    lefrSetSpacingCbk(spacingCB);
    lefrSetSpacingEndCbk(spacingEndCB);
    lefrSetTimingCbk(timingCB);
    lefrSetUnitsCbk(unitsCB);
    lefrSetUseMinSpacingCbk(useMinSpacingCB);
    lefrSetUserData((void*)3);
    lefrSetVersionCbk(versionCB);
    lefrSetViaCbk(viaCB);
    lefrSetViaRuleCbk(viaRuleCB);
    lefrSetInputAntennaCbk(antennaCB);
    lefrSetOutputAntennaCbk(antennaCB);
    lefrSetInoutAntennaCbk(antennaCB);
    lefrSetLogFunction(errorCB);
    //lefrSetWarningLogFunction(warningCB);

    lefrSetMallocFunction(mallocCB);
    lefrSetReallocFunction(reallocCB);
    lefrSetFreeFunction(freeCB);

    //lefrSetLineNumberFunction(lineNumberCB);
    //lefrSetDeltaNumberLines(50);

    lefrSetRegisterUnusedCallbacks();

    lefrSetAntennaInoutWarnings(30);
    lefrSetAntennaInputWarnings(30);
    lefrSetAntennaOutputWarnings(30);
    lefrSetArrayWarnings(30);
    lefrSetCaseSensitiveWarnings(30);
    lefrSetCorrectionTableWarnings(30);
    lefrSetDielectricWarnings(30);
    lefrSetEdgeRateThreshold1Warnings(30);
    lefrSetEdgeRateThreshold2Warnings(30);
    lefrSetEdgeRateScaleFactorWarnings(30);
    lefrSetInoutAntennaWarnings(30);
    lefrSetInputAntennaWarnings(30);
    lefrSetIRDropWarnings(30);
    lefrSetLayerWarnings(30);
    lefrSetMacroWarnings(30);
    lefrSetMaxStackViaWarnings(30);
    lefrSetMinFeatureWarnings(30);
    lefrSetNoiseMarginWarnings(30);
    lefrSetNoiseTableWarnings(30);
    lefrSetNonDefaultWarnings(30);
    lefrSetNoWireExtensionWarnings(30);
    lefrSetOutputAntennaWarnings(30);
    lefrSetPinWarnings(30);
    lefrSetSiteWarnings(30);
    lefrSetSpacingWarnings(30);
    lefrSetTimingWarnings(30);
    lefrSetUnitsWarnings(30);
    lefrSetUseMinSpacingWarnings(30);
    lefrSetViaRuleWarnings(30);
    lefrSetViaWarnings(30);

    (void) lefrSetShiftCase();  // will shift name to uppercase if caseinsensitive
                                // is set to off or not set

    (void) lefrSetOpenLogFileAppend();

    for( size_t i = 0; i < fileNames.size(); ++i ){
        lefrInit();

        if( !( f = fopen( fileNames[i].c_str(), "r" ) ) ){
            cerr << "[ERROR] Cannot open file : " << fileNames[i] << endl;
            cout << "i : " << i << endl;
            cout << fileNames[0] << endl;
            exit(0); }
        else
            cout << " read LEF : " << fileNames[i] << " ..." << endl;

        FILE *lefFile;
        lefFile = tmpfile();
        //lefFile = fopen( "temp.lef", "w+r" );
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        while((read=getdelim(&line,&len,'\n',f))!= -1)
        {
            string str(line);
            if(str.find("VERSION") != string::npos) { str = "VERSION 5.7 ;\n";}
            fputs(str.c_str(), lefFile);
            //cout<<str<<endl;
            //cin.get();
        }
        rewind(lefFile);
        fclose(f);
        //fclose(lefFile);
        //f = fopen( "temp.lef", "r" );
        int result = lefrRead(lefFile, fileNames[i].c_str(), (void*)(intptr_t)userData);
        if( result ) { cerr << "[ERROR] Reader return bad status on " << fileNames[i]; exit(0); }

        (void)lefrPrintUnusedCallbacks(fout);
        (void)lefrReleaseNResetMemory();
        //fclose( f );
        fclose(lefFile);
    }
    _cir->createMasterNameMap();
    _cir->addPins4EachMaster();
}
