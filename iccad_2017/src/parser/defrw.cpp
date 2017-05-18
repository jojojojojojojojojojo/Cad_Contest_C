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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifndef WIN32
#   include <unistd.h>
#endif /* not WIN32 */
#include "../includes/def/defrReader.hpp"
#include "../includes/def/defiAlias.hpp"

//--------------------------------------------------
#include "parser.h"
#include "../common/arghandler.h"
#include "../circuit/circuit.h"
#include "../common/paramhandler.h"

int moduleCount = 0;
bool isShowDEFMsg = false;  // show the parsed data
int fregionCount = 0; // for parsing fence regions
//--------------------------------------------------

char defaultName[64];
char defaultOut[64];

// Global variables
int numObjs;
int isSumSet;      // to keep track if within SUM
int isProp = 0;    // for PROPERTYDEFINITIONS
int begOperand;    // to keep track for constraint, to print - as the 1st char
static double curVer = 0;
//static int setSNetWireCbk = 0;

// TX_DIR:TRANSLATION ON

void myLogFunction(const char* errMsg){
    fprintf(fout, "ERROR: found error: %s\n", errMsg);
}

void myWarningLogFunction(const char* errMsg){
    fprintf(fout, "WARNING: found error: %s\n", errMsg);
}

void checkType(defrCallbackType_e c) {
    if (c >= 0 && c <= defrDesignEndCbkType) {
        // OK
    } else {
        fprintf(fout, "ERROR: callback type is out of bounds!\n");
    }
}


int done(defrCallbackType_e c, void* dummy, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (dummy) {}
//    fprintf(fout, "END DESIGN\n");
    return 0;
}

int endfunc(defrCallbackType_e c, void* dummy, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (dummy) {}
    return 0;
}

int compMSL(defrCallbackType_e c, defiComponentMaskShiftLayer* co, defiUserData ud) {
    int i;

    checkType(c);
    if ((long)ud != userData) dataError();

    if (co->defiComponentMaskShiftLayer::numMaskShiftLayers()) {
        fprintf(fout, "\nCOMPONENTMASKSHIFT ");

        for (i = 0; i < co->defiComponentMaskShiftLayer::numMaskShiftLayers(); i++) {
            fprintf(fout, "%s ", co->defiComponentMaskShiftLayer::maskShiftLayer(i));
        }
        fprintf(fout, ";\n");
    }

    return 0;
}

int compf(defrCallbackType_e c, defiComponent* co, defiUserData ud) {

    checkType(c);
    if ((long)ud != userData) dataError();

    // --------------------------------------------------
    unsigned masterId = _cir->findMasterIdByName( co->defiComponent::name() );

    Module &curmaster = _cir->master( masterId );

    _cir->addModule( Module() );
    Module &curmod = _cir->module( moduleCount );

    curmod.setName( co->defiComponent::id() );
    curmod.setWidth( curmaster.width()*unitDEF );
    curmod.setHeight( curmaster.height()*unitDEF );

    for(size_t i=0; i<curmaster.numRects(); ++i){
        curmod.addRect( curmaster.oriRect(i) );
        curmod.scaleRect(i, unitDEF);
    }

    curmod.setIsFixed( co->defiComponent::isFixed() );

    if( co->defiComponent::isFixed() || co->defiComponent::isPlaced() ){
        curmod.setPosition( co->defiComponent::placementX(),
                            co->defiComponent::placementY() );
        curmod.setOrient( Orient( co->defiComponent::placementOrient() ) );
    }

    string tmpStr = "";
    if( co->defiComponent::hasSource() )
        tmpStr = " + SOURCE DIST";
    if( co->defiComponent::isFixed() )
        tmpStr += " + FIXED ( "
                + num2str(co->defiComponent::placementX()) + " "
                + num2str(co->defiComponent::placementY()) + " ) "
                + orientStr( curmod.orient() );
    else
        tmpStr += " + PLACED ";
    preFixedModules.push_back( tmpStr );


    curmod.clearPins();
    for( size_t i=0; i<curmaster.numPins(); ++i ){
        Pin &curmaspin = curmaster.pin(i);
        _cir->addPin( Pin() );
        Pin &curpin = _cir->pin( _cir->numPins()-1 );
        curpin.setOffset( curmaspin.xOffset()*unitDEF, curmaspin.yOffset()*unitDEF );
        curpin.setModuleId( moduleCount );
        curpin.setPinId( _cir->numPins()-1 );
        string pinName = curmod.name() + ":" + curmaspin.name();
        curpin.setName( pinName );
        curpin.setDir( curmaspin.dir() );
        curpin.setPort( curmaspin.port().scale( unitDEF ) );
        curpin.setLayerName( curmaspin.layerName() );
        curpin.setIsMultiPorts( curmaspin.isMultiPorts() );
        curpin.setTaperRuleName( curmaspin.taperRuleName() );
    }

    curmod.setClassType( curmaster.classType() );
    curmod.setEdgeTypes( curmaster.edgeTypeLeft(), curmaster.edgeTypeRight() );
    curmod.setMasterId( masterId );
    // to-do macro porosity OBS

    if( isShowDEFMsg ){ // module pins' will be setup at the end of readDEF
        curmod.showInfo();
    }

    ++moduleCount;

    --numObjs;

    if (numObjs <= 0)
        _cir->setNumComponents( _cir->numModules() );

    return 0;
    // --------------------------------------------------

//    int i;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    //  missing GENERATE, FOREIGN
//    fprintf(fout, "- %s %s ", co->defiComponent::id(),
//            co->defiComponent::name());
//    //    co->defiComponent::changeIdAndName("idName", "modelName");
//    //    fprintf(fout, "%s %s ", co->defiComponent::id(),
//    //            co->defiComponent::name());
//    if (co->defiComponent::hasNets()) {
//        for (i = 0; i < co->defiComponent::numNets(); i++)
//            fprintf(fout, "%s ", co->defiComponent::net(i));
//    }
//    if (co->defiComponent::isFixed())
//        fprintf(fout, "+ FIXED %d %d %s ",
//                co->defiComponent::placementX(),
//                co->defiComponent::placementY(),
//                //orientStr(co->defiComponent::placementOrient()));
//                co->defiComponent::placementOrientStr());
//    if (co->defiComponent::isCover())
//        fprintf(fout, "+ COVER %d %d %s ",
//                co->defiComponent::placementX(),
//                co->defiComponent::placementY(),
//                orientStr(co->defiComponent::placementOrient()));
//    if (co->defiComponent::isPlaced())
//        fprintf(fout,"+ PLACED %d %d %s ",
//                co->defiComponent::placementX(),
//                co->defiComponent::placementY(),
//                orientStr(co->defiComponent::placementOrient()));
//    if (co->defiComponent::isUnplaced()) {
//        fprintf(fout,"+ UNPLACED ");
//        if ((co->defiComponent::placementX() != -1) ||
//                (co->defiComponent::placementY() != -1))
//            fprintf(fout,"%d %d %s ",
//                    co->defiComponent::placementX(),
//                    co->defiComponent::placementY(),
//                    orientStr(co->defiComponent::placementOrient()));
//    }
//    if (co->defiComponent::hasSource())
//        fprintf(fout, "+ SOURCE %s ", co->defiComponent::source());
//    if (co->defiComponent::hasGenerate()) {
//        fprintf(fout, "+ GENERATE %s ", co->defiComponent::generateName());
//        if (co->defiComponent::macroName() &&
//                *(co->defiComponent::macroName()))
//            fprintf(fout, "%s ", co->defiComponent::macroName());
//    }
//    if (co->defiComponent::hasWeight())
//        fprintf(fout, "+ WEIGHT %d ", co->defiComponent::weight());
//    if (co->defiComponent::hasEEQ())
//        fprintf(fout, "+ EEQMASTER %s ", co->defiComponent::EEQ());
//    if (co->defiComponent::hasRegionName())
//        fprintf(fout, "+ REGION %s ", co->defiComponent::regionName());
//    if (co->defiComponent::hasRegionBounds()) {
//        int *xl, *yl, *xh, *yh;
//        int size;
//        co->defiComponent::regionBounds(&size, &xl, &yl, &xh, &yh);
//        for (i = 0; i < size; i++) {
//            fprintf(fout, "+ REGION %d %d %d %d \n",
//                    xl[i], yl[i], xh[i], yh[i]);
//        }
//    }
//    if (co->defiComponent::maskShiftSize()) {
//        fprintf(fout, "+ MASKSHIFT ");

//        for (int i = co->defiComponent::maskShiftSize()-1; i >= 0; i--) {
//            fprintf(fout, "%d", co->defiComponent::maskShift(i));
//        }
//        fprintf(fout, "\n");
//    }
//    if (co->defiComponent::hasHalo()) {
//        int left, bottom, right, top;
//        (void) co->defiComponent::haloEdges(&left, &bottom, &right, &top);
//        fprintf(fout, "+ HALO ");
//        if (co->defiComponent::hasHaloSoft())
//            fprintf(fout, "SOFT ");
//        fprintf(fout, "%d %d %d %d\n", left, bottom, right, top);
//    }
//    if (co->defiComponent::hasRouteHalo()) {
//        fprintf(fout, "+ ROUTEHALO %d %s %s\n", co->defiComponent::haloDist(),
//                co->defiComponent::minLayer(), co->defiComponent::maxLayer());
//    }
//    if (co->defiComponent::hasForeignName()) {
//        fprintf(fout, "+ FOREIGN %s %d %d %s %d ",
//                co->defiComponent::foreignName(), co->defiComponent::foreignX(),
//                co->defiComponent::foreignY(), co->defiComponent::foreignOri(),
//                co->defiComponent::foreignOrient());
//    }
//    if (co->defiComponent::numProps()) {
//        for (i = 0; i < co->defiComponent::numProps(); i++) {
//            fprintf(fout, "+ PROPERTY %s %s ", co->defiComponent::propName(i),
//                    co->defiComponent::propValue(i));
//            switch (co->defiComponent::propType(i)) {
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
//    }
//    fprintf(fout, ";\n");
//    --numObjs;
//    if (numObjs <= 0)
//        fprintf(fout, "END COMPONENTS\n");
//    return 0;
}


int netpath(defrCallbackType_e c, defiNet* ppath, defiUserData ud) {
    fprintf(fout, "\n");

    fprintf (fout, "Callback of partial path for net\n");
    if (c) {}
    if (ppath->isFixed()) {}
    if (ud) {}
    return 0;
}


int netNamef(defrCallbackType_e c, const char* netName, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (netName) {}
//    fprintf(fout, "- %s ", netName);
    return 0;
}

int subnetNamef(defrCallbackType_e c, const char* subnetName, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (curVer >= 5.6)
        fprintf(fout, "   + SUBNET CBK %s ", subnetName);
    return 0;
}

int nondefRulef(defrCallbackType_e c, const char* ruleName, defiUserData ud) {
    // -------------------------------------------------- to-do
    checkType(c);
    if ((long)ud != userData) dataError();
    if (ruleName) {}
//    if (curVer >= 5.6)
//        fprintf(fout, "   + NONDEFAULTRULE CBK %s ", ruleName);
    return 0;
}

int netf(defrCallbackType_e c, defiNet* net, defiUserData ud) {

    checkType(c);
    if ((long)ud != userData) dataError();

    // --------------------------------------------------

    _cir->addNet( Net() );
    static int netCount = 0;

    Net &curnet = _cir->net(netCount);
    curnet.setName( net->defiNet::name() );

    for( int i=0; i<net->defiNet::numConnections(); ++i ){

        if( strcmp( net->defiNet::instance(i), "PIN" )==0 ){ // the module is a "pin"
            Module &mod = _cir->module( _cir->findModuleIdByName( net->defiNet::pin(i) ) );
            Pin &curpin = mod.pin(0);
            curnet.addPin( curpin );
            curpin.setNetId( netCount );
            if( curpin.dir()==INPUT ){ curnet.setSourcePin( curpin ); }
        }else{
            unsigned macroId = _cir->findModuleIdByName( net->defiNet::instance(i) );
            Module &mod = _cir->module(macroId);
            string pinName = mod.name() + ":" + net->defiNet::pin(i);
            bool isFound = false;
            for( size_t j=0; j<mod.numPins(); ++j ){
                if( pinName==mod.pin(j).name() ){
                    Pin &curpin = mod.pin(j);
                    curnet.addPin( curpin );
                    curpin.setNetId( netCount );
                    if( curpin.dir()==OUTPUT ){ curnet.setSourcePin( curpin ); }
                    isFound = true;
                    break;
                }
            }
            if( !isFound ){
                cerr << "[ERROR] Cannot find pin(" << net->defiNet::pin(i)
                     << ") for net("<< net->defiNet::name()<< ")" << endl;
                exit(0);
            }
        }
    }

    // to-do: non-default-rule

    ++netCount;
    // --------------------------------------------------

    --numObjs;
    return 0;

//    // For net and special net.
//    int        i, j, k, w, x, y, z, count, newLayer;
//    defiPath*  p;
//    defiSubnet *s;
//    int        path;
//    defiVpin   *vpin;
//    // defiShield *noShield;
//    defiWire   *wire;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    if (c != defrNetCbkType)
//        fprintf(fout, "BOGUS NET TYPE  ");
//    if (net->defiNet::pinIsMustJoin(0))
//        fprintf(fout, "- MUSTJOIN ");
//    // 5/6/2004 - don't need since I have a callback for the name
//    //  else
//    //      fprintf(fout, "- %s ", net->defiNet::name());

//    //  net->defiNet::changeNetName("newNetName");
//    //  fprintf(fout, "%s ", net->defiNet::name());
//    count = 0;
//    // compName & pinName
//    for (i = 0; i < net->defiNet::numConnections(); i++) {
//        // set the limit of only 5 items per line
//        count++;
//        if (count >= 5) {
//            fprintf(fout, "\n");
//            count = 0;
//        }
//        fprintf(fout, "( %s %s ) ", net->defiNet::instance(i),
//                net->defiNet::pin(i));
//        //      net->defiNet::changeInstance("newInstance", i);
//        //      net->defiNet::changePin("newPin", i);
//        //      fprintf(fout, "( %s %s ) ", net->defiNet::instance(i),
//        //              net->defiNet::pin(i));
//        if (net->defiNet::pinIsSynthesized(i))
//            fprintf(fout, "+ SYNTHESIZED ");
//    }

//    if (net->hasNonDefaultRule())
//        fprintf(fout, "+ NONDEFAULTRULE %s\n", net->nonDefaultRule());

//    for (i = 0; i < net->defiNet::numVpins(); i++) {
//        vpin = net->defiNet::vpin(i);
//        fprintf(fout, "  + %s", vpin->name());
//        if (vpin->layer())
//            fprintf(fout, " %s", vpin->layer());
//        fprintf(fout, " %d %d %d %d", vpin->xl(), vpin->yl(), vpin->xh(),
//                vpin->yh());
//        if (vpin->status() != ' ') {
//            fprintf(fout, " %c", vpin->status());
//            fprintf(fout, " %d %d", vpin->xLoc(), vpin->yLoc());
//            if (vpin->orient() != -1)
//                fprintf(fout, " %s", orientStr(vpin->orient()));
//        }
//        fprintf(fout, "\n");
//    }

//    // regularWiring
//    if (net->defiNet::numWires()) {
//        for (i = 0; i < net->defiNet::numWires(); i++) {
//            newLayer = 0;
//            wire = net->defiNet::wire(i);
//            fprintf(fout, "\n  + %s ", wire->wireType());
//            count = 0;
//            for (j = 0; j < wire->defiWire::numPaths(); j++) {
//                p = wire->defiWire::path(j);
//                p->initTraverse();
//                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//                    count++;
//                    // Don't want the line to be too long
//                    if (count >= 5) {
//                        fprintf(fout, "\n");
//                        count = 0;
//                    }
//                    switch (path) {
//                    case DEFIPATH_LAYER:
//                        if (newLayer == 0) {
//                            fprintf(fout, "%s ", p->defiPath::getLayer());
//                            newLayer = 1;
//                        } else
//                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
//                        break;
//                    case DEFIPATH_MASK:
//                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
//                        break;
//                    case DEFIPATH_VIAMASK:
//                        fprintf(fout, "MASK %d%d%d ",
//                                p->defiPath::getViaTopMask(),
//                                p->defiPath::getViaCutMask(),
//                                p->defiPath::getViaBottomMask());
//                        break;
//                    case DEFIPATH_VIA:
//                        fprintf(fout, "%s ", p->defiPath::getVia());
//                        break;
//                    case DEFIPATH_VIAROTATION:
//                        fprintf(fout, "%s ",
//                                orientStr(p->defiPath::getViaRotation()));
//                        break;
//                    case DEFIPATH_RECT:
//                        p->defiPath::getViaRect(&w, &x, &y, &z);
//                        fprintf(fout, "RECT ( %d %d %d %d ) ", w, x, y, z);
//                        break;
//                    case DEFIPATH_VIRTUALPOINT:
//                        p->defiPath::getVirtualPoint(&x, &y);
//                        fprintf(fout, "VIRTUAL ( %d %d ) ", x, y);
//                        break;
//                    case DEFIPATH_WIDTH:
//                        fprintf(fout, "%d ", p->defiPath::getWidth());
//                        break;
//                    case DEFIPATH_POINT:
//                        p->defiPath::getPoint(&x, &y);
//                        fprintf(fout, "( %d %d ) ", x, y);
//                        break;
//                    case DEFIPATH_FLUSHPOINT:
//                        p->defiPath::getFlushPoint(&x, &y, &z);
//                        fprintf(fout, "( %d %d %d ) ", x, y, z);
//                        break;
//                    case DEFIPATH_TAPER:
//                        fprintf(fout, "TAPER ");
//                        break;
//                    case DEFIPATH_TAPERRULE:
//                        fprintf(fout, "TAPERRULE %s ",p->defiPath::getTaperRule());
//                        break;
//                    case DEFIPATH_STYLE:
//                        fprintf(fout, "STYLE %d ",p->defiPath::getStyle());
//                        break;
//                    }
//                }
//            }
//            fprintf(fout, "\n");
//            count = 0;
//        }
//    }

//    // SHIELDNET
//    if (net->defiNet::numShieldNets()) {
//        for (i = 0; i < net->defiNet::numShieldNets(); i++)
//            fprintf(fout, "\n  + SHIELDNET %s", net->defiNet::shieldNet(i));
//    }
//    /* obsolete in 5.4
//  if (net->defiNet::numNoShields()) {
//     for (i = 0; i < net->defiNet::numNoShields(); i++) {
//         noShield = net->defiNet::noShield(i);
//         fprintf(fout, "\n  + NOSHIELD ");
//         newLayer = 0;
//         for (j = 0; j < noShield->defiShield::numPaths(); j++) {
//            p = noShield->defiShield::path(j);
//            p->initTraverse();
//            while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//               count++;
//               // Don't want the line to be too long
//               if (count >= 5) {
//                   fprintf(fout, "\n");
//                   count = 0;
//               }
//               switch (path) {
//                 case DEFIPATH_LAYER:
//                      if (newLayer == 0) {
//                          fprintf(fout, "%s ", p->defiPath::getLayer());
//                          newLayer = 1;
//                      } else
//                          fprintf(fout, "NEW %s ", p->defiPath::getLayer());
//                      break;
//                 case DEFIPATH_VIA:
//                      fprintf(fout, "%s ", p->defiPath::getVia());
//                      break;
//                 case DEFIPATH_VIAROTATION:
//                      fprintf(fout, "%s ",
//                             orientStr(p->defiPath::getViaRotation()));
//                      break;
//                 case DEFIPATH_WIDTH:
//                      fprintf(fout, "%d ", p->defiPath::getWidth());
//                      break;
//                 case DEFIPATH_POINT:
//                      p->defiPath::getPoint(&x, &y);
//                      fprintf(fout, "( %d %d ) ", x, y);
//                      break;
//                 case DEFIPATH_FLUSHPOINT:
//                      p->defiPath::getFlushPoint(&x, &y, &z);
//                      fprintf(fout, "( %d %d %d ) ", x, y, z);
//                      break;
//                 case DEFIPATH_TAPER:
//                      fprintf(fout, "TAPER ");
//                      break;
//                 case DEFIPATH_TAPERRULE:
//                      fprintf(fout, "TAPERRULE %s ",
//                              p->defiPath::getTaperRule());
//                      break;
//               }
//            }
//         }
//     }
//  }
//*/

//    if (net->defiNet::hasSubnets()) {
//        for (i = 0; i < net->defiNet::numSubnets(); i++) {
//            s = net->defiNet::subnet(i);
//            fprintf(fout, "\n");

//            if (s->defiSubnet::numConnections()) {
//                if (s->defiSubnet::pinIsMustJoin(0))
//                    fprintf(fout, "- MUSTJOIN ");
//                else
//                    fprintf(fout, "  + SUBNET %s ", s->defiSubnet::name());
//                for (j = 0; j < s->defiSubnet::numConnections(); j++)
//                    fprintf(fout, " ( %s %s )\n", s->defiSubnet::instance(j),
//                            s->defiSubnet::pin(j));

//                // regularWiring
//                if (s->defiSubnet::numWires()) {
//                    for (k = 0; k < s->defiSubnet::numWires(); k++) {
//                        newLayer = 0;
//                        wire = s->defiSubnet::wire(k);
//                        fprintf(fout, "  %s ", wire->wireType());
//                        count = 0;
//                        for (j = 0; j < wire->defiWire::numPaths(); j++) {
//                            p = wire->defiWire::path(j);
//                            p->initTraverse();
//                            while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//                                count++;
//                                // Don't want the line to be too long
//                                if (count >= 5) {
//                                    fprintf(fout, "\n");
//                                    count = 0;
//                                }
//                                switch (path) {
//                                case DEFIPATH_LAYER:
//                                    if (newLayer == 0) {
//                                        fprintf(fout, "%s ", p->defiPath::getLayer());
//                                        newLayer = 1;
//                                    } else
//                                        fprintf(fout, "NEW %s ",
//                                                p->defiPath::getLayer());
//                                    break;
//                                case DEFIPATH_VIA:
//                                    fprintf(fout, "%s ", p->defiPath::getVia());
//                                    break;
//                                case DEFIPATH_VIAROTATION:
//                                    fprintf(fout, "%s ",
//                                            p->defiPath::getViaRotationStr());
//                                    break;
//                                case DEFIPATH_WIDTH:
//                                    fprintf(fout, "%d ", p->defiPath::getWidth());
//                                    break;
//                                case DEFIPATH_POINT:
//                                    p->defiPath::getPoint(&x, &y);
//                                    fprintf(fout, "( %d %d ) ", x, y);
//                                    break;
//                                case DEFIPATH_FLUSHPOINT:
//                                    p->defiPath::getFlushPoint(&x, &y, &z);
//                                    fprintf(fout, "( %d %d %d ) ", x, y, z);
//                                    break;
//                                case DEFIPATH_TAPER:
//                                    fprintf(fout, "TAPER ");
//                                    break;
//                                case DEFIPATH_TAPERRULE:
//                                    fprintf(fout, "TAPERRULE  %s ",
//                                            p->defiPath::getTaperRule());
//                                    break;
//                                case DEFIPATH_STYLE:
//                                    fprintf(fout, "STYLE  %d ",
//                                            p->defiPath::getStyle());
//                                    break;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//    if (net->defiNet::numProps()) {
//        for (i = 0; i < net->defiNet::numProps(); i++) {
//            fprintf(fout, "  + PROPERTY %s ", net->defiNet::propName(i));
//            switch (net->defiNet::propType(i)) {
//            case 'R': fprintf(fout, "%g REAL ", net->defiNet::propNumber(i));
//                break;
//            case 'I': fprintf(fout, "%g INTEGER ", net->defiNet::propNumber(i));
//                break;
//            case 'S': fprintf(fout, "%s STRING ", net->defiNet::propValue(i));
//                break;
//            case 'Q': fprintf(fout, "%s QUOTESTRING ", net->defiNet::propValue(i));
//                break;
//            case 'N': fprintf(fout, "%g NUMBER ", net->defiNet::propNumber(i));
//                break;
//            }
//            fprintf(fout, "\n");
//        }
//    }

//    if (net->defiNet::hasWeight())
//        fprintf(fout, "+ WEIGHT %d ", net->defiNet::weight());
//    if (net->defiNet::hasCap())
//        fprintf(fout, "+ ESTCAP %g ", net->defiNet::cap());
//    if (net->defiNet::hasSource())
//        fprintf(fout, "+ SOURCE %s ", net->defiNet::source());
//    if (net->defiNet::hasFixedbump())
//        fprintf(fout, "+ FIXEDBUMP ");
//    if (net->defiNet::hasFrequency())
//        fprintf(fout, "+ FREQUENCY %g ", net->defiNet::frequency());
//    if (net->defiNet::hasPattern())
//        fprintf(fout, "+ PATTERN %s ", net->defiNet::pattern());
//    if (net->defiNet::hasOriginal())
//        fprintf(fout, "+ ORIGINAL %s ", net->defiNet::original());
//    if (net->defiNet::hasUse())
//        fprintf(fout, "+ USE %s ", net->defiNet::use());

//    fprintf (fout, ";\n");
//    --numObjs;
//    if (numObjs <= 0)
//        fprintf(fout, "END NETS\n");
//    return 0;
}


int snetpath(defrCallbackType_e c, defiNet* ppath, defiUserData ud) {

    if (c != defrSNetPartialPathCbkType)
        return 1;
    if ((long)ud != userData) dataError();

    // --------------------------------------------------

    if( ppath->defiNet::numWires()==0 ){
        cerr << "[ERROR] Special net has no wire specified." << endl;
        exit(0);
    }

    int         x, y, count = 0;
    defiPath*   p;
    int         path;
    defiWire*   wire;
    int         layerId = -1;
    double      width = 0;
    Point      start, end;
    bool        isEnd = false;

    for (int i = 0; i < ppath->defiNet::numWires(); i++) {
        wire            = ppath->defiNet::wire(i);
        bool isTraced   = false;
        for (int j = 0; j < wire->defiWire::numPaths(); j++) {
            bool isVia  = false; // we do not consider the via
            p = wire->defiWire::path(j);
            p->initTraverse();
            while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
                count++;
                switch (path) {
                case DEFIPATH_LAYER:{
                    if( isTraced == true ){
                        layerId = mtlStr2Int( p->defiPath::getLayer() )-1;
                        if( p->defiPath::getLayer() != _cir->layer( layerId ).name() ){
                            cerr << "[ERROR] power rail layer name not aligned: "
                                 << p->defiPath::getLayer() << ", " << _cir->layer( layerId ).name() << endl;
                            exit(0);
                        }
                    }

                    break;
                }
                case DEFIPATH_VIA:{
                    isVia = true;
                    break;
                }
                case DEFIPATH_WIDTH:
                    if( isTraced == true ){ width = p->defiPath::getWidth(); }
                    break;
                case DEFIPATH_POINT:
                    p->defiPath::getPoint( &x, &y );
                    if( isTraced == true ){
                        if( isEnd == true ) { end.set_x_y( (double)x, (double)y );   isEnd = false;}
                        else                { start.set_x_y( (double)x, (double)y ); isEnd = true; }
                    }
                    break;
                }
            }
            if( isTraced ) {
                isTraced = false; isVia = true;/*only for escapping*/

                double top=0, bottom=0, left=0, right=0;
                if( start.x() == end.x() ){                 // vertical rail
                    top     = max( start.y(), end.y() );
                    bottom  = min( start.y(), end.y() );
                    left    = start.x() - width/2;
                    right   = start.x() + width/2;
                }else if( start.y() == end.y() ){           // horizontal rail
                    top     = start.y() + width/2;
                    bottom  = start.y() - width/2;
                    left    = min( start.x(), end.x() );
                    right   = max( start.x(), end.x() );
                }else{ cerr<<"[ERROR] A non-vertical/horizontal rail..."<<endl; }

                // add stripe to m_pgRails
                string ppathName(ppath->name());
                _cir->layer( layerId ).addRail( Rect( left, bottom, right, top ), ppathName  );
            }
            if( isVia == false && isTraced == false ){ // trace again
                --j; isTraced = true;
            }
        }
        count = 0;
    }
    return 0;

    // --------------------------------------------------

//    int         i, j, x, y, z, count, newLayer;
//    char*       layerName;
//    double      dist, left, right;
//    defiPath*   p;
//    defiSubnet  *s;
//    int         path;
//    defiShield* shield;
//    defiWire*   wire;
//    int         numX, numY, stepX, stepY;

//    if (c != defrSNetPartialPathCbkType)
//        return 1;
//    if ((long)ud != userData) dataError();

//    fprintf (fout, "SPECIALNET partial data\n");

//    fprintf(fout, "- %s ", ppath->defiNet::name());

//    count = 0;
//    // compName & pinName
//    for (i = 0; i < ppath->defiNet::numConnections(); i++) {
//        // set the limit of only 5 items print out in one line
//        count++;
//        if (count >= 5) {
//            fprintf(fout, "\n");
//            count = 0;
//        }
//        fprintf (fout, "( %s %s ) ", ppath->defiNet::instance(i),
//                 ppath->defiNet::pin(i));
//        if (ppath->defiNet::pinIsSynthesized(i))
//            fprintf(fout, "+ SYNTHESIZED ");
//    }

//    // specialWiring
//    // POLYGON
//    if (ppath->defiNet::numPolygons()) {
//        struct defiPoints points;
//        for (i = 0; i < ppath->defiNet::numPolygons(); i++) {
//            fprintf(fout, "\n  + POLYGON %s ", ppath->polygonName(i));
//            points = ppath->getPolygon(i);
//            for (j = 0; j < points.numPoints; j++)
//                fprintf(fout, "%d %d ", points.x[j], points.y[j]);
//        }
//    }
//    // RECT
//    if (ppath->defiNet::numRectangles()) {
//        for (i = 0; i < ppath->defiNet::numRectangles(); i++) {
//            fprintf(fout, "\n  + RECT %s %d %d %d %d", ppath->defiNet::rectName(i),
//                    ppath->defiNet::xl(i), ppath->defiNet::yl(i),
//                    ppath->defiNet::xh(i), ppath->defiNet::yh(i));
//        }
//    }

//    // COVER, FIXED, ROUTED or SHIELD
//    if (ppath->defiNet::numWires()) {
//        newLayer = 0;
//        for (i = 0; i < ppath->defiNet::numWires(); i++) {
//            newLayer = 0;
//            wire = ppath->defiNet::wire(i);
//            fprintf(fout, "\n  + %s ", wire->wireType());
//            if (strcmp (wire->wireType(), "SHIELD") == 0)
//                fprintf(fout, "%s ", wire->wireShieldNetName());
//            for (j = 0; j < wire->defiWire::numPaths(); j++) {
//                p = wire->defiWire::path(j);
//                p->initTraverse();
//                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//                    count++;
//                    // Don't want the line to be too long
//                    if (count >= 5) {
//                        fprintf(fout, "\n");
//                        count = 0;
//                    }
//                    switch (path) {
//                    case DEFIPATH_LAYER:
//                        if (newLayer == 0) {
//                            fprintf(fout, "%s ", p->defiPath::getLayer());
//                            newLayer = 1;
//                        } else
//                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
//                        break;
//                    case DEFIPATH_VIA:
//                        fprintf(fout, "%s ", p->defiPath::getVia());
//                        break;
//                    case DEFIPATH_VIAROTATION:
//                        fprintf(fout, "%s ",
//                                orientStr(p->defiPath::getViaRotation()));
//                        break;
//                    case DEFIPATH_VIADATA:
//                        p->defiPath::getViaData(&numX, &numY, &stepX, &stepY);
//                        fprintf(fout, "DO %d BY %d STEP %d %d ", numX, numY,
//                                stepX, stepY);
//                        break;
//                    case DEFIPATH_WIDTH:
//                        fprintf(fout, "%d ", p->defiPath::getWidth());
//                        break;
//                    case DEFIPATH_MASK:
//                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
//                        break;
//                    case DEFIPATH_VIAMASK:
//                        fprintf(fout, "MASK %d%d%d ",
//                                p->defiPath::getViaTopMask(),
//                                p->defiPath::getViaCutMask(),
//                                p->defiPath::getViaBottomMask());
//                        break;
//                    case DEFIPATH_POINT:
//                        p->defiPath::getPoint(&x, &y);
//                        fprintf(fout, "( %d %d ) ", x, y);
//                        break;
//                    case DEFIPATH_FLUSHPOINT:
//                        p->defiPath::getFlushPoint(&x, &y, &z);
//                        fprintf(fout, "( %d %d %d ) ", x, y, z);
//                        break;
//                    case DEFIPATH_TAPER:
//                        fprintf(fout, "TAPER ");
//                        break;
//                    case DEFIPATH_SHAPE:
//                        fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
//                        break;
//                    case DEFIPATH_STYLE:
//                        fprintf(fout, "+ STYLE %d ", p->defiPath::getStyle());
//                        break;
//                    }
//                }
//            }
//            fprintf(fout, "\n");
//            count = 0;
//        }
//    }

//    if (ppath->defiNet::hasSubnets()) {
//        for (i = 0; i < ppath->defiNet::numSubnets(); i++) {
//            s = ppath->defiNet::subnet(i);
//            if (s->defiSubnet::numConnections()) {
//                if (s->defiSubnet::pinIsMustJoin(0))
//                    fprintf(fout, "- MUSTJOIN ");
//                else
//                    fprintf(fout, "- %s ", s->defiSubnet::name());
//                for (j = 0; j < s->defiSubnet::numConnections(); j++) {
//                    fprintf(fout, " ( %s %s )\n", s->defiSubnet::instance(j),
//                            s->defiSubnet::pin(j));
//                }
//            }

//            // regularWiring
//            if (s->defiSubnet::numWires()) {
//                for (i = 0; i < s->defiSubnet::numWires(); i++) {
//                    wire = s->defiSubnet::wire(i);
//                    fprintf(fout, "  + %s ", wire->wireType());
//                    for (j = 0; j < wire->defiWire::numPaths(); j++) {
//                        p = wire->defiWire::path(j);
//                        p->defiPath::print(fout);
//                    }
//                }
//            }
//        }
//    }

//    if (ppath->defiNet::numProps()) {
//        for (i = 0; i < ppath->defiNet::numProps(); i++) {
//            if (ppath->defiNet::propIsString(i))
//                fprintf(fout, "  + PROPERTY %s %s ", ppath->defiNet::propName(i),
//                        ppath->defiNet::propValue(i));
//            if (ppath->defiNet::propIsNumber(i))
//                fprintf(fout, "  + PROPERTY %s %g ", ppath->defiNet::propName(i),
//                        ppath->defiNet::propNumber(i));
//            switch (ppath->defiNet::propType(i)) {
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
//            fprintf(fout, "\n");
//        }
//    }

//    // SHIELD
//    count = 0;
//    // testing the SHIELD for 5.3, obsolete in 5.4
//    if (ppath->defiNet::numShields()) {
//        for (i = 0; i < ppath->defiNet::numShields(); i++) {
//            shield = ppath->defiNet::shield(i);
//            fprintf(fout, "\n  + SHIELD %s ", shield->defiShield::shieldName());
//            newLayer = 0;
//            for (j = 0; j < shield->defiShield::numPaths(); j++) {
//                p = shield->defiShield::path(j);
//                p->initTraverse();
//                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//                    count++;
//                    // Don't want the line to be too long
//                    if (count >= 5) {
//                        fprintf(fout, "\n");
//                        count = 0;
//                    }
//                    switch (path) {
//                    case DEFIPATH_LAYER:
//                        if (newLayer == 0) {
//                            fprintf(fout, "%s ", p->defiPath::getLayer());
//                            newLayer = 1;
//                        } else
//                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
//                        break;
//                    case DEFIPATH_VIA:
//                        fprintf(fout, "%s ", p->defiPath::getVia());
//                        break;
//                    case DEFIPATH_VIAROTATION:
//                        if (newLayer)
//                            fprintf(fout, "%s ",
//                                    orientStr(p->defiPath::getViaRotation()));
//                        else
//                            fprintf(fout, "Str %s ",
//                                    p->defiPath::getViaRotationStr());
//                        break;
//                    case DEFIPATH_WIDTH:
//                        fprintf(fout, "%d ", p->defiPath::getWidth());
//                        break;
//                    case DEFIPATH_MASK:
//                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
//                        break;
//                    case DEFIPATH_VIAMASK:
//                        fprintf(fout, "MASK %d%d%d ",
//                                p->defiPath::getViaTopMask(),
//                                p->defiPath::getViaCutMask(),
//                                p->defiPath::getViaBottomMask());
//                        break;
//                    case DEFIPATH_POINT:
//                        p->defiPath::getPoint(&x, &y);
//                        fprintf(fout, "( %d %d ) ", x, y);
//                        break;
//                    case DEFIPATH_FLUSHPOINT:
//                        p->defiPath::getFlushPoint(&x, &y, &z);
//                        fprintf(fout, "( %d %d %d ) ", x, y, z);
//                        break;
//                    case DEFIPATH_TAPER:
//                        fprintf(fout, "TAPER ");
//                        break;
//                    case DEFIPATH_SHAPE:
//                        fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
//                        break;
//                    case DEFIPATH_STYLE:
//                        fprintf(fout, "+ STYLE %d ", p->defiPath::getStyle());
//                    }
//                }
//            }
//        }
//    }

//    // layerName width
//    if (ppath->defiNet::hasWidthRules()) {
//        for (i = 0; i < ppath->defiNet::numWidthRules(); i++) {
//            ppath->defiNet::widthRule(i, &layerName, &dist);
//            fprintf (fout, "\n  + WIDTH %s %g ", layerName, dist);
//        }
//    }

//    // layerName spacing
//    if (ppath->defiNet::hasSpacingRules()) {
//        for (i = 0; i < ppath->defiNet::numSpacingRules(); i++) {
//            ppath->defiNet::spacingRule(i, &layerName, &dist, &left, &right);
//            if (left == right)
//                fprintf (fout, "\n  + SPACING %s %g ", layerName, dist);
//            else
//                fprintf (fout, "\n  + SPACING %s %g RANGE %g %g ",
//                         layerName, dist, left, right);
//        }
//    }

//    if (ppath->defiNet::hasFixedbump())
//        fprintf(fout, "\n  + FIXEDBUMP ");
//    if (ppath->defiNet::hasFrequency())
//        fprintf(fout, "\n  + FREQUENCY %g ", ppath->defiNet::frequency());
//    if (ppath->defiNet::hasVoltage())
//        fprintf(fout, "\n  + VOLTAGE %g ", ppath->defiNet::voltage());
//    if (ppath->defiNet::hasWeight())
//        fprintf(fout, "\n  + WEIGHT %d ", ppath->defiNet::weight());
//    if (ppath->defiNet::hasCap())
//        fprintf(fout, "\n  + ESTCAP %g ", ppath->defiNet::cap());
//    if (ppath->defiNet::hasSource())
//        fprintf(fout, "\n  + SOURCE %s ", ppath->defiNet::source());
//    if (ppath->defiNet::hasPattern())
//        fprintf(fout, "\n  + PATTERN %s ", ppath->defiNet::pattern());
//    if (ppath->defiNet::hasOriginal())
//        fprintf(fout, "\n  + ORIGINAL %s ", ppath->defiNet::original());
//    if (ppath->defiNet::hasUse())
//        fprintf(fout, "\n  + USE %s ", ppath->defiNet::use());

//    fprintf(fout, "\n");

//    return 0;
}


int snetwire(defrCallbackType_e c, defiNet* ppath, defiUserData ud) {

    if (c != defrSNetWireCbkType)
        return 1;
    if ((long)ud != userData) dataError();
    // --------------------------------------------------

    if( ppath->defiNet::numWires()==0 ){
        cerr << "[ERROR] Special net has no wire specified." << endl;
        exit(0);
    }

    int         x, y, count = 0;
    defiPath*   p;
    int         path;
    defiWire*   wire;
    int         layerId = -1;
    double      width = 0;
    Point      start, end;
    bool        isEnd = false;
    //cout<<"[defrew.cpp line 1118] Net Name = "<<ppath->name()<<endl;

    for (int i = 0; i < ppath->defiNet::numWires(); i++) {
        wire            = ppath->defiNet::wire(i);
        bool isTraced   = false;
        for (int j = 0; j < wire->defiWire::numPaths(); j++) {
            bool isVia  = false; // we do not consider the via
            p = wire->defiWire::path(j);
            p->initTraverse();
            while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
                count++;
                switch (path) {
                case DEFIPATH_LAYER:{
                    if( isTraced == true ){
                        layerId = mtlStr2Int( p->defiPath::getLayer() )-1;
                        if( p->defiPath::getLayer() != _cir->layer( layerId ).name() ){
                            cerr << "[ERROR] power rail layer name not aligned: "
                                 << p->defiPath::getLayer() << ", " << _cir->layer( layerId ).name() << endl;
                            exit(0);
                        }
                    }
                    break;
                }
                case DEFIPATH_VIA:{
                    isVia = true;
                    break;
                }
                case DEFIPATH_WIDTH:
                    if( isTraced == true ){ width = p->defiPath::getWidth(); }
                    break;
                case DEFIPATH_POINT:
                    p->defiPath::getPoint( &x, &y );
                    if( isTraced == true ){
                        if( isEnd == true ) { end.set_x_y( (double)x, (double)y );   isEnd = false;}
                        else                { start.set_x_y( (double)x, (double)y ); isEnd = true; }
                    }
                    break;
                }
            }
            if( isTraced ) {
                isTraced = false; isVia = true;/*only for escapping*/

                double top=0, bottom=0, left=0, right=0;
                if( start.x() == end.x() ){                 // vertical rail
                    top     = max( start.y(), end.y() );
                    bottom  = min( start.y(), end.y() );
                    left    = start.x() - width/2;
                    right   = start.x() + width/2;
                }else if( start.y() == end.y() ){           // horizontal rail
                    top     = start.y() + width/2;
                    bottom  = start.y() - width/2;
                    left    = min( start.x(), end.x() );
                    right   = max( start.x(), end.x() );
                }else{ cerr<<"[ERROR] A non-vertical/horizontal rail..."<<endl; }

                // add stripe to m_pgRails
                string ppathName(ppath->name());
                _cir->layer( layerId ).addRail( Rect( left, bottom, right, top ), ppathName );
            }
            if( isVia == false && isTraced == false ){ // trace again
                --j; isTraced = true;
            }
        }
        count = 0;
    }
    return 0;

    // --------------------------------------------------

//    int         i, j, x, y, z, count = 0, newLayer;
//    defiPath*   p;
//    int         path;
//    defiWire*   wire;
//    defiShield* shield;
//    int         numX, numY, stepX, stepY;

//    if (c != defrSNetWireCbkType)
//        return 1;
//    if ((long)ud != userData) dataError();

//    fprintf (fout, "SPECIALNET wire data\n");

//    fprintf(fout, "- %s ", ppath->defiNet::name());


//    // POLYGON
//    if (ppath->defiNet::numPolygons()) {
//        struct defiPoints points;
//        for (i = 0; i < ppath->defiNet::numPolygons(); i++) {
//            fprintf(fout, "\n  + POLYGON %s ", ppath->polygonName(i));

//            points = ppath->getPolygon(i);

//            for (j = 0; j < points.numPoints; j++) {
//                fprintf(fout, "%d %d ", points.x[j], points.y[j]);
//            }
//        }
//    }

//    // RECT
//    if (ppath->defiNet::numRectangles()) {
//        for (i = 0; i < ppath->defiNet::numRectangles(); i++) {
//            fprintf(fout, "\n  + RECT %s %d %d %d %d", ppath->defiNet::rectName(i),
//                    ppath->defiNet::xl(i), ppath->defiNet::yl(i),
//                    ppath->defiNet::xh(i), ppath->defiNet::yh(i));
//        }
//    }

//    // VIA
//    if (ppath->defiNet::numViaSpecs()) {
//        for (i = 0; i < ppath->defiNet::numViaSpecs(); i++) {
//            fprintf(fout, "\n  + VIA %s ", ppath->defiNet::viaName(i)),
//                    fprintf(fout, " %s", ppath->defiNet::viaOrientStr(i));

//            defiPoints points = ppath->defiNet::getViaPts(i);

//            for (int j = 0; j < points.numPoints; j++) {
//                fprintf(fout, " %d %d", points.x[j], points.y[j]);
//            }
//        }
//    }

//    // specialWiring
//    if (ppath->defiNet::numWires()) {
//        newLayer = 0;
//        for (i = 0; i < ppath->defiNet::numWires(); i++) {
//            newLayer = 0;
//            wire = ppath->defiNet::wire(i);
//            fprintf(fout, "\n  + %s ", wire->wireType());
//            if (strcmp (wire->wireType(), "SHIELD") == 0)
//                fprintf(fout, "%s ", wire->wireShieldNetName());
//            for (j = 0; j < wire->defiWire::numPaths(); j++) {
//                p = wire->defiWire::path(j);
//                p->initTraverse();
//                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//                    count++;
//                    // Don't want the line to be too long
//                    if (count >= 5) {
//                        fprintf(fout, "\n");
//                        count = 0;
//                    }
//                    switch (path) {
//                    case DEFIPATH_LAYER:
//                        if (newLayer == 0) {
//                            fprintf(fout, "%s ", p->defiPath::getLayer());
//                            newLayer = 1;
//                        } else
//                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
//                        break;
//                    case DEFIPATH_VIA:
//                        fprintf(fout, "%s ", p->defiPath::getVia());
//                        break;
//                    case DEFIPATH_VIAROTATION:
//                        fprintf(fout, "%s ",
//                                orientStr(p->defiPath::getViaRotation()));
//                        break;
//                    case DEFIPATH_VIADATA:
//                        p->defiPath::getViaData(&numX, &numY, &stepX, &stepY);
//                        fprintf(fout, "DO %d BY %d STEP %d %d ", numX, numY,
//                                stepX, stepY);
//                        break;
//                    case DEFIPATH_WIDTH:
//                        fprintf(fout, "%d ", p->defiPath::getWidth());
//                        break;
//                    case DEFIPATH_MASK:
//                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
//                        break;
//                    case DEFIPATH_VIAMASK:
//                        fprintf(fout, "MASK %d%d%d ",
//                                p->defiPath::getViaTopMask(),
//                                p->defiPath::getViaCutMask(),
//                                p->defiPath::getViaBottomMask());
//                        break;
//                    case DEFIPATH_POINT:
//                        p->defiPath::getPoint(&x, &y);
//                        fprintf(fout, "( %d %d ) ", x, y);
//                        break;
//                    case DEFIPATH_FLUSHPOINT:
//                        p->defiPath::getFlushPoint(&x, &y, &z);
//                        fprintf(fout, "( %d %d %d ) ", x, y, z);
//                        break;
//                    case DEFIPATH_TAPER:
//                        fprintf(fout, "TAPER ");
//                        break;
//                    case DEFIPATH_SHAPE:
//                        fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
//                        break;
//                    case DEFIPATH_STYLE:
//                        fprintf(fout, "+ STYLE %d ", p->defiPath::getStyle());
//                        break;
//                    }
//                }
//            }
//            fprintf(fout, "\n");
//            count = 0;
//        }
//    } else if (ppath->defiNet::numShields()) {
//        for (i = 0; i < ppath->defiNet::numShields(); i++) {
//            shield = ppath->defiNet::shield(i);
//            fprintf(fout, "\n  + SHIELD %s ", shield->defiShield::shieldName());
//            newLayer = 0;
//            for (j = 0; j < shield->defiShield::numPaths(); j++) {
//                p = shield->defiShield::path(j);
//                p->initTraverse();
//                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
//                    count++;
//                    // Don't want the line to be too long
//                    if (count >= 5) {
//                        fprintf(fout, "\n");
//                        count = 0;
//                    }
//                    switch (path) {
//                    case DEFIPATH_LAYER:
//                        if (newLayer == 0) {
//                            fprintf(fout, "%s ", p->defiPath::getLayer());
//                            newLayer = 1;
//                        } else
//                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
//                        break;
//                    case DEFIPATH_VIA:
//                        fprintf(fout, "%s ", p->defiPath::getVia());
//                        break;
//                    case DEFIPATH_VIAROTATION:
//                        fprintf(fout, "%s ",
//                                orientStr(p->defiPath::getViaRotation()));
//                        break;
//                    case DEFIPATH_WIDTH:
//                        fprintf(fout, "%d ", p->defiPath::getWidth());
//                        break;
//                    case DEFIPATH_MASK:
//                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
//                        break;
//                    case DEFIPATH_VIAMASK:
//                        fprintf(fout, "MASK %d%d%d ",
//                                p->defiPath::getViaTopMask(),
//                                p->defiPath::getViaCutMask(),
//                                p->defiPath::getViaBottomMask());
//                        break;
//                    case DEFIPATH_POINT:
//                        p->defiPath::getPoint(&x, &y);
//                        fprintf(fout, "( %d %d ) ", x, y);
//                        break;
//                    case DEFIPATH_FLUSHPOINT:
//                        p->defiPath::getFlushPoint(&x, &y, &z);
//                        fprintf(fout, "( %d %d %d ) ", x, y, z);
//                        break;
//                    case DEFIPATH_TAPER:
//                        fprintf(fout, "TAPER ");
//                        break;
//                    case DEFIPATH_SHAPE:
//                        fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
//                        break;
//                    case DEFIPATH_STYLE:
//                        fprintf(fout, "+ STYLE %d ", p->defiPath::getStyle());
//                        break;
//                    }
//                }
//            }
//        }
//    }

//    fprintf(fout, "\n");

//    return 0;
}

int snetf(defrCallbackType_e c, defiNet* net, defiUserData ud) {
    // For net and special net.
    int         i, j, x, y, z, count, newLayer;
    char*       layerName;
    double      dist, left, right;
    defiPath*   p;
    defiSubnet  *s;
    int         path;
    defiShield* shield;
    defiWire*   wire;    int         numX, numY, stepX, stepY;

    checkType(c);
    if ((long)ud != userData) dataError();
    if (c != defrSNetCbkType)
        fprintf(fout, "BOGUS NET TYPE  ");

    // 5/6/2004 - don't need since I have a callback for the name
    //  fprintf(fout, "- %s ", net->defiNet::name());

    count = 0;
    // compName & pinName
    for (i = 0; i < net->defiNet::numConnections(); i++) {
        // set the limit of only 5 items print out in one line
        count++;
        if (count >= 5) {
            fprintf(fout, "\n");
            count = 0;
        }
        fprintf (fout, "( %s %s ) ", net->defiNet::instance(i),
                 net->defiNet::pin(i));
        if (net->defiNet::pinIsSynthesized(i))
            fprintf(fout, "+ SYNTHESIZED ");
    }

    // specialWiring
    if (net->defiNet::numWires()) {
        newLayer = 0;
        for (i = 0; i < net->defiNet::numWires(); i++) {
            newLayer = 0;
            wire = net->defiNet::wire(i);
            fprintf(fout, "\n  + %s ", wire->wireType());
            if (strcmp (wire->wireType(), "SHIELD") == 0)
                fprintf(fout, "%s ", wire->wireShieldNetName());
            for (j = 0; j < wire->defiWire::numPaths(); j++) {
                p = wire->defiWire::path(j);
                p->initTraverse();
                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
                    count++;
                    // Don't want the line to be too long
                    if (count >= 5) {
                        fprintf(fout, "\n");
                        count = 0;
                    }
                    switch (path) {
                    case DEFIPATH_LAYER:
                        if (newLayer == 0) {
                            fprintf(fout, "%s ", p->defiPath::getLayer());
                            newLayer = 1;
                        } else
                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
                        break;
                    case DEFIPATH_VIA:
                        fprintf(fout, "%s ", p->defiPath::getVia());
                        break;
                    case DEFIPATH_VIAROTATION:
                        fprintf(fout, "%s ",
                                orientStr(p->defiPath::getViaRotation()));
                        break;
                    case DEFIPATH_VIADATA:
                        p->defiPath::getViaData(&numX, &numY, &stepX, &stepY);
                        fprintf(fout, "DO %d BY %d STEP %d %d ", numX, numY,
                                stepX, stepY);
                        break;
                    case DEFIPATH_WIDTH:
                        fprintf(fout, "%d ", p->defiPath::getWidth());
                        break;
                    case DEFIPATH_MASK:
                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
                        break;
                    case DEFIPATH_VIAMASK:
                        fprintf(fout, "MASK %d%d%d ",
                                p->defiPath::getViaTopMask(),
                                p->defiPath::getViaCutMask(),
                                p->defiPath::getViaBottomMask());
                        break;
                    case DEFIPATH_POINT:
                        p->defiPath::getPoint(&x, &y);
                        fprintf(fout, "( %d %d ) ", x, y);
                        break;
                    case DEFIPATH_FLUSHPOINT:
                        p->defiPath::getFlushPoint(&x, &y, &z);
                        fprintf(fout, "( %d %d %d ) ", x, y, z);
                        break;
                    case DEFIPATH_TAPER:
                        fprintf(fout, "TAPER ");
                        break;
                    case DEFIPATH_SHAPE:
                        fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
                        break;
                    case DEFIPATH_STYLE:
                        fprintf(fout, "+ STYLE %d ", p->defiPath::getStyle());
                        break;
                    }
                }
            }
            fprintf(fout, "\n");
            count = 0;
        }
    }

    // POLYGON
    if (net->defiNet::numPolygons()) {
        struct defiPoints points;
        //defiWire*   wire;
        for (i = 0; i < net->defiNet::numPolygons(); i++) {
            if (curVer >= 5.8 ) {
                if (strcmp(net->polyRouteStatus(i), "") != 0) {
                    fprintf(fout, "\n  + %s ", net->defiNet::polyRouteStatus(i));
                    if (strcmp(net->polyRouteStatus(i), "SHIELD") == 0) {
                        fprintf(fout, "\n  + %s ", net->defiNet::polyRouteStatusShieldName(i));
                    }
                }
                if (strcmp(net->polyShapeType(i), "") != 0) {
                    fprintf(fout, "\n  + SHAPE %s ", net->defiNet::polyShapeType(i));
                }
            }
            if (net->polyMask(i)) {
                fprintf(fout, "\n  + MASK %d + POLYGON %s ", net->polyMask(i),
                        net->polygonName(i));
            } else {
                fprintf(fout, "\n  + POLYGON %s ", net->polygonName(i));
            }
            points = net->getPolygon(i);
            for (j = 0; j < points.numPoints; j++)
                fprintf(fout, "%d %d ", points.x[j], points.y[j]);
        }
    }
    // RECT
    if (net->defiNet::numRectangles()) {
        //defiWire* wire;
        for (i = 0; i < net->defiNet::numRectangles(); i++) {
            if (curVer >= 5.8 ) {
                if (strcmp(net->rectRouteStatus(i), "") != 0) {
                    fprintf(fout, "\n  + %s ", net->defiNet::rectRouteStatus(i));
                    if (strcmp(net->rectRouteStatus(i), "SHIELD") == 0) {
                        fprintf(fout, "\n  + %s ", net->defiNet::rectRouteStatusShieldName(i));
                    }
                }
                if (strcmp(net->rectShapeType(i), "") != 0) {
                    fprintf(fout, "\n  + SHAPE %s ", net->defiNet::rectShapeType(i));
                }
            }
            if (net->rectMask(i)) {
                fprintf(fout, "\n  + MASK %d + RECT %s %d %d %d %d",
                        net->rectMask(i), net->rectName(i),
                        net->xl(i), net->yl(i), net->xh(i),
                        net->yh(i));
            } else {
//                fprintf(fout, "\n  + RECT %s %d %d %d %d",
//                        net->defiNet::rectName(i),
//                        net->defiNet::xl(i),
//                        net->defiNet::yl(i),
//                        net->defiNet::xh(i),
//                        net->defiNet::yh(i));
            }
        }
    }
    // VIA
    if (curVer >= 5.8 && net->defiNet::numViaSpecs()) {
        for (i = 0; i < net->defiNet::numViaSpecs(); i++) {
            if (strcmp(net->viaRouteStatus(i), "") != 0) {
                fprintf(fout, "\n  + %s ", net->defiNet::viaRouteStatus(i));
                if (strcmp(net->viaRouteStatus(i), "SHIELD") == 0) {
                    fprintf(fout, "\n  + %s ", net->defiNet::viaRouteStatusShieldName(i));
                }
            }
            if (strcmp(net->viaShapeType(i), "") != 0) {
                fprintf(fout, "\n  + SHAPE %s ", net->defiNet::viaShapeType(i));
            }
            if (net->topMaskNum(i) || net->cutMaskNum(i) || net->bottomMaskNum(i)) {
                fprintf(fout, "\n  + MASK %d%d%d + VIA %s ", net->topMaskNum(i),
                        net->cutMaskNum(i),
                        net->bottomMaskNum(i),
                        net->defiNet::viaName(i));
            } else {
                fprintf(fout, "\n  + VIA %s ", net->defiNet::viaName(i));
            }
            fprintf(fout, " %s", net->defiNet::viaOrientStr(i));

            defiPoints points = net->defiNet::getViaPts(i);

            for (int j = 0; j < points.numPoints; j++) {
                fprintf(fout, " %d %d", points.x[j], points.y[j]);
            }
            fprintf(fout, ";\n");

        }
    }

    if (net->defiNet::hasSubnets()) {
        for (i = 0; i < net->defiNet::numSubnets(); i++) {
            s = net->defiNet::subnet(i);
            if (s->defiSubnet::numConnections()) {
                if (s->defiSubnet::pinIsMustJoin(0))
                    fprintf(fout, "- MUSTJOIN ");
                else
                    fprintf(fout, "- %s ", s->defiSubnet::name());
                for (j = 0; j < s->defiSubnet::numConnections(); j++) {
                    fprintf(fout, " ( %s %s )\n", s->defiSubnet::instance(j),
                            s->defiSubnet::pin(j));
                }
            }

            // regularWiring
            if (s->defiSubnet::numWires()) {
                for (i = 0; i < s->defiSubnet::numWires(); i++) {
                    wire = s->defiSubnet::wire(i);
                    fprintf(fout, "  + %s ", wire->wireType());
                    for (j = 0; j < wire->defiWire::numPaths(); j++) {
                        p = wire->defiWire::path(j);
                        p->defiPath::print(fout);
                    }
                }
            }
        }
    }

    if (net->defiNet::numProps()) {
        for (i = 0; i < net->defiNet::numProps(); i++) {
            if (net->defiNet::propIsString(i))
                fprintf(fout, "  + PROPERTY %s %s ", net->defiNet::propName(i),
                        net->defiNet::propValue(i));
            if (net->defiNet::propIsNumber(i))
                fprintf(fout, "  + PROPERTY %s %g ", net->defiNet::propName(i),
                        net->defiNet::propNumber(i));
            switch (net->defiNet::propType(i)) {
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
            fprintf(fout, "\n");
        }
    }

    // SHIELD
    count = 0;
    // testing the SHIELD for 5.3, obsolete in 5.4
    if (net->defiNet::numShields()) {
        for (i = 0; i < net->defiNet::numShields(); i++) {
            shield = net->defiNet::shield(i);
            fprintf(fout, "\n  + SHIELD %s ", shield->defiShield::shieldName());
            newLayer = 0;
            for (j = 0; j < shield->defiShield::numPaths(); j++) {
                p = shield->defiShield::path(j);
                p->initTraverse();
                while ((path = (int)p->defiPath::next()) != DEFIPATH_DONE) {
                    count++;
                    // Don't want the line to be too long
                    if (count >= 5) {
                        fprintf(fout, "\n");
                        count = 0;
                    }
                    switch (path) {
                    case DEFIPATH_LAYER:
                        if (newLayer == 0) {
                            fprintf(fout, "%s ", p->defiPath::getLayer());
                            newLayer = 1;
                        } else
                            fprintf(fout, "NEW %s ", p->defiPath::getLayer());
                        break;
                    case DEFIPATH_VIA:
                        fprintf(fout, "%s ", p->defiPath::getVia());
                        break;
                    case DEFIPATH_VIAROTATION:
                        fprintf(fout, "%s ",
                                orientStr(p->defiPath::getViaRotation()));
                        break;
                    case DEFIPATH_WIDTH:
                        fprintf(fout, "%d ", p->defiPath::getWidth());
                        break;
                    case DEFIPATH_MASK:
                        fprintf(fout, "MASK %d ", p->defiPath::getMask());
                        break;
                    case DEFIPATH_VIAMASK:
                        fprintf(fout, "MASK %d%d%d ",
                                p->defiPath::getViaTopMask(),
                                p->defiPath::getViaCutMask(),
                                p->defiPath::getViaBottomMask());
                        break;
                    case DEFIPATH_POINT:
                        p->defiPath::getPoint(&x, &y);
                        fprintf(fout, "( %d %d ) ", x, y);
                        break;
                    case DEFIPATH_FLUSHPOINT:
                        p->defiPath::getFlushPoint(&x, &y, &z);
                        fprintf(fout, "( %d %d %d ) ", x, y, z);
                        break;
                    case DEFIPATH_TAPER:
                        fprintf(fout, "TAPER ");
                        break;
                    case DEFIPATH_SHAPE:
                        fprintf(fout, "+ SHAPE %s ", p->defiPath::getShape());
                        break;
                    case DEFIPATH_STYLE:
                        fprintf(fout, "+ STYLE %d ", p->defiPath::getStyle());
                        break;
                    }
                }
            }
        }
    }

    // layerName width
    if (net->defiNet::hasWidthRules()) {
        for (i = 0; i < net->defiNet::numWidthRules(); i++) {
            net->defiNet::widthRule(i, &layerName, &dist);
            fprintf (fout, "\n  + WIDTH %s %g ", layerName, dist);
        }
    }

    // layerName spacing
    if (net->defiNet::hasSpacingRules()) {
        for (i = 0; i < net->defiNet::numSpacingRules(); i++) {
            net->defiNet::spacingRule(i, &layerName, &dist, &left, &right);
            if (left == right)
                fprintf (fout, "\n  + SPACING %s %g ", layerName, dist);
            else
                fprintf (fout, "\n  + SPACING %s %g RANGE %g %g ",
                         layerName, dist, left, right);
        }
    }

    if (net->defiNet::hasFixedbump())
        fprintf(fout, "\n  + FIXEDBUMP ");
    if (net->defiNet::hasFrequency())
        fprintf(fout, "\n  + FREQUENCY %g ", net->defiNet::frequency());
    if (net->defiNet::hasVoltage())
        fprintf(fout, "\n  + VOLTAGE %g ", net->defiNet::voltage());
    if (net->defiNet::hasWeight())
        fprintf(fout, "\n  + WEIGHT %d ", net->defiNet::weight());
    if (net->defiNet::hasCap())
        fprintf(fout, "\n  + ESTCAP %g ", net->defiNet::cap());
    if (net->defiNet::hasSource())
        fprintf(fout, "\n  + SOURCE %s ", net->defiNet::source());
    if (net->defiNet::hasPattern())
        fprintf(fout, "\n  + PATTERN %s ", net->defiNet::pattern());
    if (net->defiNet::hasOriginal())
        fprintf(fout, "\n  + ORIGINAL %s ", net->defiNet::original());
//    if (net->defiNet::hasUse())
//        fprintf(fout, "\n  + USE %s ", net->defiNet::use());

//    fprintf (fout, ";\n");
    --numObjs;
//    if (numObjs <= 0)
//        fprintf(fout, "END SPECIALNETS\n");
    return 0;
}


int ndr(defrCallbackType_e c, defiNonDefault* nd, defiUserData ud) {
    // -------------------------------------------------- to-do
    checkType(c);
    if ((long)ud != userData) dataError();
    if (nd->defiNonDefault::name()) {}
    --numObjs;
    return 0;
    // --------------------------------------------------

    // For nondefaultrule
//    int i;

//    checkType(c);
//    if ((long)ud != userData) dataError();
//    if (c != defrNonDefaultCbkType)
//        fprintf(fout, "BOGUS NONDEFAULTRULE TYPE  ");
//    fprintf(fout, "- %s\n", nd->defiNonDefault::name());
//    if (nd->defiNonDefault::hasHardspacing())
//        fprintf(fout, "   + HARDSPACING\n");
//    for (i = 0; i < nd->defiNonDefault::numLayers(); i++) {
//        fprintf(fout, "   + LAYER %s", nd->defiNonDefault::layerName(i));
//        fprintf(fout, " WIDTH %d", nd->defiNonDefault::layerWidthVal(i));
//        if (nd->defiNonDefault::hasLayerDiagWidth(i))
//            fprintf(fout, " DIAGWIDTH %d",
//                    nd->defiNonDefault::layerDiagWidthVal(i));
//        if (nd->defiNonDefault::hasLayerSpacing(i))
//            fprintf(fout, " SPACING %d", nd->defiNonDefault::layerSpacingVal(i));
//        if (nd->defiNonDefault::hasLayerWireExt(i))
//            fprintf(fout, " WIREEXT %d", nd->defiNonDefault::layerWireExtVal(i));
//        fprintf(fout, "\n");
//    }
//    for (i = 0; i < nd->defiNonDefault::numVias(); i++)
//        fprintf(fout, "   + VIA %s\n", nd->defiNonDefault::viaName(i));
//    for (i = 0; i < nd->defiNonDefault::numViaRules(); i++)
//        fprintf(fout, "   + VIARULE %s\n", nd->defiNonDefault::viaRuleName(i));
//    for (i = 0; i < nd->defiNonDefault::numMinCuts(); i++)
//        fprintf(fout, "   + MINCUTS %s %d\n", nd->defiNonDefault::cutLayerName(i),
//                nd->defiNonDefault::numCuts(i));
//    for (i = 0; i < nd->defiNonDefault::numProps(); i++) {
//        fprintf(fout, "   + PROPERTY %s %s ", nd->defiNonDefault::propName(i),
//                nd->defiNonDefault::propValue(i));
//        switch (nd->defiNonDefault::propType(i)) {
//        case 'R': fprintf(fout, "REAL\n");
//            break;
//        case 'I': fprintf(fout, "INTEGER\n");
//            break;
//        case 'S': fprintf(fout, "STRING\n");
//            break;
//        case 'Q': fprintf(fout, "QUOTESTRING\n");
//            break;
//        case 'N': fprintf(fout, "NUMBER\n");
//            break;
//        }
//    }
//    --numObjs;
//    if (numObjs <= 0)
//        fprintf(fout, "END NONDEFAULTRULES\n");
//    return 0;
}

int tname(defrCallbackType_e c, const char* string, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "TECHNOLOGY %s ;\n", string);
    return 0;
}

int dname(defrCallbackType_e c, const char* string, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    // --------------------------------------------------
    _cir->setName( string );
    // --------------------------------------------------
    //  fprintf(fout, "DESIGN %s ;\n", string);

    return 0;
}


char* address(const char* in) {
    return ((char*)in);
}

int cs(defrCallbackType_e c, int num, defiUserData ud) {
    char* name;

    checkType(c);

    if ((long)ud != userData) dataError();

    switch (c) {
    case defrComponentStartCbkType : name = address("COMPONENTS"); break;
    case defrNetStartCbkType : name = address("NETS"); break;
    case defrStartPinsCbkType : name = address("PINS"); break;
    case defrViaStartCbkType : name = address("VIAS"); break;
    case defrRegionStartCbkType : name = address("REGIONS"); break;
    case defrSNetStartCbkType : name = address("SPECIALNETS"); break;
    case defrGroupsStartCbkType : name = address("GROUPS"); break;
    case defrScanchainsStartCbkType : name = address("SCANCHAINS"); break;
    case defrIOTimingsStartCbkType : name = address("IOTIMINGS"); break;
    case defrFPCStartCbkType : name = address("FLOORPLANCONSTRAINTS"); break;
    case defrTimingDisablesStartCbkType : name = address("TIMING DISABLES"); break;
    case defrPartitionsStartCbkType : name = address("PARTITIONS"); break;
    case defrPinPropStartCbkType : name = address("PINPROPERTIES"); break;
    case defrBlockageStartCbkType : name = address("BLOCKAGES"); break;
    case defrSlotStartCbkType : name = address("SLOTS"); break;
    case defrFillStartCbkType : name = address("FILLS"); break;
    case defrNonDefaultStartCbkType : name = address("NONDEFAULTRULES"); break;
    case defrStylesStartCbkType : name = address("STYLES"); break;
    default : name = address("BOGUS"); return 1;
    }
    if (name) {}
//    fprintf(fout, "\n%s %d ;\n", name, num);
    numObjs = num;
    return 0;
}

int constraintst(defrCallbackType_e c, int num, defiUserData ud) {
    // Handles both constraints and assertions
    checkType(c);
    if ((long)ud != userData) dataError();
    if (c == defrConstraintsStartCbkType)
        fprintf(fout, "\nCONSTRAINTS %d ;\n\n", num);
    else
        fprintf(fout, "\nASSERTIONS %d ;\n\n", num);
    numObjs = num;
    return 0;
}

void operand(defrCallbackType_e c, defiAssertion* a, int ind) {
    int i, first = 1;
    char* netName;
    char* fromInst, * fromPin, * toInst, * toPin;

    if (a->defiAssertion::isSum()) {
        // Sum in operand, recursively call operand
        fprintf(fout, "- SUM ( ");
        a->defiAssertion::unsetSum();
        isSumSet = 1;
        begOperand = 0;
        operand (c, a, ind);
        fprintf(fout, ") ");
    } else {
        // operand
        if (ind >= a->defiAssertion::numItems()) {
            fprintf(fout, "ERROR: when writing out SUM in Constraints.\n");
            return;
        }
        if (begOperand) {
            fprintf(fout, "- ");
            begOperand = 0;
        }
        for (i = ind; i < a->defiAssertion::numItems(); i++) {
            if (a->defiAssertion::isNet(i)) {
                a->defiAssertion::net(i, &netName);
                if (!first)
                    fprintf(fout, ", "); // print , as separator
                fprintf(fout, "NET %s ", netName);
            } else if (a->defiAssertion::isPath(i)) {
                a->defiAssertion::path(i, &fromInst, &fromPin, &toInst,
                                       &toPin);
                if (!first)
                    fprintf(fout, ", ");
                fprintf(fout, "PATH %s %s %s %s ", fromInst, fromPin, toInst,
                        toPin);
            } else if (isSumSet) {
                // SUM within SUM, reset the flag
                a->defiAssertion::setSum();
                operand(c, a, i);
            }
            first = 0;
        }

    }
}

int constraint(defrCallbackType_e c, defiAssertion* a, defiUserData ud) {
    // Handles both constraints and assertions

    checkType(c);
    if ((long)ud != userData) dataError();
    if (a->defiAssertion::isWiredlogic())
        // Wirelogic
        fprintf(fout, "- WIREDLOGIC %s + MAXDIST %g ;\n",
                // Wiredlogic dist is also store in fallMax
                //              a->defiAssertion::netName(), a->defiAssertion::distance());
                a->defiAssertion::netName(), a->defiAssertion::fallMax());
    else {
        // Call the operand function
        isSumSet = 0;    // reset the global variable
        begOperand = 1;
        operand (c, a, 0);
        // Get the Rise and Fall
        if (a->defiAssertion::hasRiseMax())
            fprintf(fout, "+ RISEMAX %g ", a->defiAssertion::riseMax());
        if (a->defiAssertion::hasFallMax())
            fprintf(fout, "+ FALLMAX %g ", a->defiAssertion::fallMax());
        if (a->defiAssertion::hasRiseMin())
            fprintf(fout, "+ RISEMIN %g ", a->defiAssertion::riseMin());
        if (a->defiAssertion::hasFallMin())
            fprintf(fout, "+ FALLMIN %g ", a->defiAssertion::fallMin());
        fprintf(fout, ";\n");
    }
    --numObjs;
    if (numObjs <= 0) {
        if (c == defrConstraintCbkType)
            fprintf(fout, "END CONSTRAINTS\n");
        else
            fprintf(fout, "END ASSERTIONS\n");
    }
    return 0;
}


int propstart(defrCallbackType_e c, void* dummy, defiUserData ud) {
    checkType(c);
    //fprintf(fout, "\nPROPERTYDEFINITIONS\n");
    if (dummy) {}
    if (ud) {}
    isProp = 1;

    return 0;
}


int prop(defrCallbackType_e c, defiProp* p, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    if (strcmp(p->defiProp::propType(), "design") == 0) {}
    return 0;
//    if (strcmp(p->defiProp::propType(), "design") == 0)
//        fprintf(fout, "DESIGN %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "net") == 0)
//        fprintf(fout, "NET %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "component") == 0)
//        fprintf(fout, "COMPONENT %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "specialnet") == 0)
//        fprintf(fout, "SPECIALNET %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "group") == 0)
//        fprintf(fout, "GROUP %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "row") == 0)
//        fprintf(fout, "ROW %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "componentpin") == 0)
//        fprintf(fout, "COMPONENTPIN %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "region") == 0)
//        fprintf(fout, "REGION %s ", p->defiProp::propName());
//    else if (strcmp(p->defiProp::propType(), "nondefaultrule") == 0)
//        fprintf(fout, "NONDEFAULTRULE %s ", p->defiProp::propName());
//    if (p->defiProp::dataType() == 'I')
//        fprintf(fout, "INTEGER ");
//    if (p->defiProp::dataType() == 'R')
//        fprintf(fout, "REAL ");
//    if (p->defiProp::dataType() == 'S')
//        fprintf(fout, "STRING ");
//    if (p->defiProp::dataType() == 'Q')
//        fprintf(fout, "STRING ");
//    if (p->defiProp::hasRange()) {
//        fprintf(fout, "RANGE %g %g ", p->defiProp::left(),
//                p->defiProp::right());
//    }
//    if (p->defiProp::hasNumber())
//        fprintf(fout, "%g ", p->defiProp::number());
//    if (p->defiProp::hasString())
//        fprintf(fout, "\"%s\" ", p->defiProp::string());
//    fprintf(fout, ";\n");

//    return 0;
}


int propend(defrCallbackType_e c, void* dummy, defiUserData ud) {
    checkType(c);
    if (isProp) {
        //fprintf(fout, "END PROPERTYDEFINITIONS\n\n");
        isProp = 0;
    }
    if (dummy) {}
    if (ud) {}

    defrSetCaseSensitivity(1);
    return 0;
}


int hist(defrCallbackType_e c, const char* h, defiUserData ud) {
    checkType(c);
    defrSetCaseSensitivity(0);
    if ((long)ud != userData) dataError();
    fprintf(fout, "HISTORY %s ;\n", h);
    defrSetCaseSensitivity(1);
    return 0;
}


int an(defrCallbackType_e c, const char* h, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "ARRAY %s ;\n", h);
    return 0;
}


int fn(defrCallbackType_e c, const char* h, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "FLOORPLAN %s ;\n", h);
    return 0;
}


int bbn(defrCallbackType_e c, const char* h, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (h) {}
    //  fprintf(fout, "BUSBITCHARS \"%s\" ;\n", h);
    return 0;
}


int vers(defrCallbackType_e c, double d, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (d) {}
    //  fprintf(fout, "VERSION %g ;\n", d);
    //    curVer = d;

    //  // wmd -- testing the alias
    //  defrAddAlias ("alias1", "aliasValue1", 1);
    //  defrAddAlias ("alias2", "aliasValue2", 0);
    //  defiAlias_itr *aliasStore;
    //  aliasStore = (defiAlias_itr*)malloc(sizeof(defiAlias_itr*));
    //  aliasStore->Init();
    //  while (aliasStore->defiAlias_itr::Next()) {
    //     fprintf(fout, "ALIAS %s %s %d ;\n", aliasStore->defiAlias_itr::Key(),
    //                   aliasStore->defiAlias_itr::Data(),
    //                   aliasStore->defiAlias_itr::Marked());
    //  }
    //  free(aliasStore);
    return 0;
}


int versStr(defrCallbackType_e c, const char* versionName, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "VERSION %s ;\n", versionName);
    return 0;
}


int units(defrCallbackType_e c, double d, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();

    // --------------------------------------------------
    unitDEF = d;
    if( isShowDEFMsg )
        cout << "1 micron = " << unitDEF << " database." << endl;
    // --------------------------------------------------

    //  fprintf(fout, "UNITS DISTANCE MICRONS %g ;\n", d);
    return 0;
}


int casesens(defrCallbackType_e c, int d, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (d == 1)
        fprintf(fout, "NAMESCASESENSITIVE ON ;\n");
    else
        fprintf(fout, "NAMESCASESENSITIVE OFF ;\n");
    return 0;
}


int cls(defrCallbackType_e c, void* cl, defiUserData ud) {
    defiSite* site;  // Site and Canplace and CannotOccupy
    defiBox* box;  // DieArea and
    defiPinCap* pc;
    defiPin* pin;
    int i, j;
//    int k;
    defiRow* row;
    defiTrack* track;
    defiGcellGrid* gcg;
    defiVia* via;
    defiRegion* re;
//    defiGroup* group;
    //defiComponentMaskShiftLayer* maskShiftLayer;
    defiScanchain* sc;
    defiIOTiming* iot;
    defiFPC* fpc;
    defiTimingDisable* td;
    defiPartition* part;
    defiPinProp* pprop;
    defiBlockage* block;
    defiSlot* slots;
    defiFill* fills;
    defiStyles* styles;
    int xl, yl, xh, yh;
    char *name, *a1, *b1;
    char **inst, **inPin, **outPin;
    int  *bits;
    int  size;
    int corner, typ;
    const char *itemT;
    char dir;
//    defiPinAntennaModel* aModel;
    struct defiPoints points;

    checkType(c);
    if ((long)ud != userData) dataError();
    switch (c) {

    case defrSiteCbkType :
        site = (defiSite*)cl;
        fprintf(fout, "SITE %s %g %g %s ", site->defiSite::name(),
                site->defiSite::x_orig(), site->defiSite::y_orig(),
                orientStr(site->defiSite::orient()));
        fprintf(fout, "DO %g BY %g STEP %g %g ;\n",
                site->defiSite::x_num(), site->defiSite::y_num(),
                site->defiSite::x_step(), site->defiSite::y_step());
        break;
    case defrCanplaceCbkType :
        site = (defiSite*)cl;
        fprintf(fout, "CANPLACE %s %g %g %s ", site->defiSite::name(),
                site->defiSite::x_orig(), site->defiSite::y_orig(),
                orientStr(site->defiSite::orient()));
        fprintf(fout, "DO %g BY %g STEP %g %g ;\n",
                site->defiSite::x_num(), site->defiSite::y_num(),
                site->defiSite::x_step(), site->defiSite::y_step());
        break;
    case defrCannotOccupyCbkType :
        site = (defiSite*)cl;
        fprintf(fout, "CANNOTOCCUPY %s %g %g %s ",
                site->defiSite::name(), site->defiSite::x_orig(),
                site->defiSite::y_orig(), orientStr(site->defiSite::orient()));
        fprintf(fout, "DO %g BY %g STEP %g %g ;\n",
                site->defiSite::x_num(), site->defiSite::y_num(),
                site->defiSite::x_step(), site->defiSite::y_step());
        break;
    case defrDieAreaCbkType :
        box = (defiBox*)cl;

        // --------------------------------------------------
        _cir->setRectChip( Rect( box->defiBox::xl(), box->defiBox::yl(), box->defiBox::xh(), box->defiBox::yh() ) );
        // --------------------------------------------------

//        fprintf(fout, "DIEAREA %d %d %d %d ;\n",
//                box->defiBox::xl(), box->defiBox::yl(), box->defiBox::xh(),
//                box->defiBox::yh());
//        fprintf(fout, "DIEAREA ");
//        points = box->defiBox::getPoint();
//        for (i = 0; i < points.numPoints; i++)
//            fprintf(fout, "%d %d ", points.x[i], points.y[i]);
//        fprintf(fout, ";\n");
        break;
    case defrPinCapCbkType :
        pc = (defiPinCap*)cl;
        fprintf(fout, "MINPINS %d WIRECAP %g ;\n",
                pc->defiPinCap::pin(), pc->defiPinCap::cap());
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END DEFAULTCAP\n");
        break;
    case defrPinCbkType :{
        pin = (defiPin*)cl;

        // --------------------------------------------------
        // treat the fixed pin as a fixed module


        Point pinPos;
        int pinOrient(-1);
        string pinLayer("NULL");
        bool isMultiPorts = false;
        if( pin->defiPin::hasPlacement() ) {
            pinPos.set_x_y( pin->defiPin::placementX(), pin->defiPin::placementY() );
            pinOrient = pin->defiPin::orient();
            if( pin->defiPin::hasLayer() ){
                pinLayer = pin->defiPin::layer(0);
            }
            pin->defiPin::bounds(0, &xl, &yl, &xh, &yh); // set the pin position to that of the first layer port
        }else if (pin->defiPin::hasPort()) { // only consider the first port
            defiPinPort* port;
            isMultiPorts = pin->defiPin::numPorts() > 0;
            for (j = 0; j < pin->defiPin::numPorts(); j++) {
                port = pin->defiPin::pinPort(j);
                if (port->defiPinPort::isFixed()) {
                    pinPos.set_x_y( port->defiPinPort::placementX(), port->defiPinPort::placementY() );
                    pinOrient = port->defiPinPort::orient();

                    for (i = 0; i < port->defiPinPort::numLayer(); i++) {
                        pinLayer = port->defiPinPort::layer(i);
                        port->defiPinPort::bounds(i, &xl, &yl, &xh, &yh);
                    }

                    break;
                }
            }
        }

        if( pinPos.x()==MAX_DOUBLE && pinPos.y()==MAX_DOUBLE && pinOrient==-1 ){
            cerr << "[ERROR] pin " << pin->defiPin::pinName() << " is not placed in .def" << endl;
            exit(0);
        }
        if( pinLayer=="NULL" ){
            cerr << "[ERROR] pin " << pin->defiPin::pinName() << " is not bounded to any layer in .def" << endl;
            exit(0);
        }

        _cir->addModule( Module() );
        Module &curmod = _cir->module( moduleCount );
        curmod.setName( pin->defiPin::pinName() );

        curmod.setPosition( pinPos.x()+xl, pinPos.y()+yl );
        curmod.setWidth( xh-xl );
        curmod.setHeight( yh-yl );
        curmod.addRect( Rect( 0, 0, curmod.width(), curmod.height()  ) );
        curmod.setIsFixed( true );
        curmod.setOrient( Orient( pinOrient ) );
        curmod.setClassType( "psuedopin" );
        curmod.clearPins();

        _cir->addPin( Pin() );
        Pin &curpin = _cir->pin( _cir->numPins()-1 );
        curpin.setName( pin->defiPin::pinName() );
        curpin.setOffset( -curmod.width()/2, -curmod.height()/2 );
        curpin.setModuleId( moduleCount );
        curpin.setPinId( _cir->numPins()-1 );
        curpin.setDirStr( pin->defiPin::direction() );
        curpin.setIsIOPin();
        curpin.setPort( Rect( 0,0,xh-xl,yh-yl ) );
        curpin.setLayerName( pinLayer );
        curpin.setIsMultiPorts( isMultiPorts );
        curpin.setTaperRuleName("");

        if( isShowDEFMsg || gArg.checkExist( "showParsedIOPin" ) ){
            curmod.showInfo();
            curpin.showInfo();
        }

        ++moduleCount;
        // --------------------------------------------------
        --numObjs;

        if (numObjs <= 0){ // pins and modules are all set
            _cir->createPinNameMap();
            _cir->createModuleNameMap();
            _cir->addPins4EachModule();
        }

        break;
//        fprintf(fout, "- %s + NET %s ", pin->defiPin::pinName(),
//                pin->defiPin::netName());
//        //         pin->defiPin::changePinName("pinName");
//        //         fprintf(fout, "%s ", pin->defiPin::pinName());
//        if (pin->defiPin::hasDirection())
//            fprintf(fout, "+ DIRECTION %s ", pin->defiPin::direction());
//        if (pin->defiPin::hasUse())
//            fprintf(fout, "+ USE %s ", pin->defiPin::use());
//        if (pin->defiPin::hasNetExpr())
//            fprintf(fout, "+ NETEXPR \"%s\" ", pin->defiPin::netExpr());
//        if (pin->defiPin::hasSupplySensitivity())
//            fprintf(fout, "+ SUPPLYSENSITIVITY %s ",
//                    pin->defiPin::supplySensitivity());
//        if (pin->defiPin::hasGroundSensitivity())
//            fprintf(fout, "+ GROUNDSENSITIVITY %s ",
//                    pin->defiPin::groundSensitivity());
//        if (pin->defiPin::hasLayer()) {
//            struct defiPoints points;
//            for (i = 0; i < pin->defiPin::numLayer(); i++) {
//                fprintf(fout, "\n  + LAYER %s ", pin->defiPin::layer(i));
//                if (pin->defiPin::layerMask(i))
//                    fprintf(fout, "MASK %d ",
//                            pin->defiPin::layerMask(i));
//                if (pin->defiPin::hasLayerSpacing(i))
//                    fprintf(fout, "SPACING %d ",
//                            pin->defiPin::layerSpacing(i));
//                if (pin->defiPin::hasLayerDesignRuleWidth(i))
//                    fprintf(fout, "DESIGNRULEWIDTH %d ",
//                            pin->defiPin::layerDesignRuleWidth(i));
//                pin->defiPin::bounds(i, &xl, &yl, &xh, &yh);
//                fprintf(fout, "%d %d %d %d ", xl, yl, xh, yh);
//            }
//            for (i = 0; i < pin->defiPin::numPolygons(); i++) {
//                fprintf(fout, "\n  + POLYGON %s ",
//                        pin->defiPin::polygonName(i));
//                if (pin->defiPin::polygonMask(i))
//                    fprintf(fout, "MASK %d ",
//                            pin->defiPin::polygonMask(i));
//                if (pin->defiPin::hasPolygonSpacing(i))
//                    fprintf(fout, "SPACING %d ",
//                            pin->defiPin::polygonSpacing(i));
//                if (pin->defiPin::hasPolygonDesignRuleWidth(i))
//                    fprintf(fout, "DESIGNRULEWIDTH %d ",
//                            pin->defiPin::polygonDesignRuleWidth(i));
//                points = pin->defiPin::getPolygon(i);
//                for (j = 0; j < points.numPoints; j++)
//                    fprintf(fout, "%d %d ", points.x[j], points.y[j]);
//            }
//            for (i = 0; i < pin->defiPin::numVias(); i++) {
//                if (pin->defiPin::viaTopMask(i) || pin->defiPin::viaCutMask(i) || pin->defiPin::viaBottomMask(i)) {
//                    fprintf(fout, "\n  + VIA %s MASK %d%d%d %d %d ",
//                            pin->defiPin::viaName(i),
//                            pin->defiPin::viaTopMask(i),
//                            pin->defiPin::viaCutMask(i),
//                            pin->defiPin::viaBottomMask(i),
//                            pin->defiPin::viaPtX(i),
//                            pin->defiPin::viaPtY(i));
//                } else {
//                    fprintf(fout, "\n  + VIA %s %d %d ", pin->defiPin::viaName(i),
//                            pin->defiPin::viaPtX(i), pin->defiPin::viaPtY(i));
//                }
//            }
//        }
//        if (pin->defiPin::hasPort()) {
//            struct defiPoints points;
//            defiPinPort* port;
//            for (j = 0; j < pin->defiPin::numPorts(); j++) {
//                port = pin->defiPin::pinPort(j);
//                fprintf(fout, "\n  + PORT");
//                for (i = 0; i < port->defiPinPort::numLayer(); i++) {
//                    fprintf(fout, "\n     + LAYER %s ",
//                            port->defiPinPort::layer(i));
//                    if (port->defiPinPort::layerMask(i))
//                        fprintf(fout, "MASK %d ",
//                                port->defiPinPort::layerMask(i));
//                    if (port->defiPinPort::hasLayerSpacing(i))
//                        fprintf(fout, "SPACING %d ",
//                                port->defiPinPort::layerSpacing(i));
//                    if (port->defiPinPort::hasLayerDesignRuleWidth(i))
//                        fprintf(fout, "DESIGNRULEWIDTH %d ",
//                                port->defiPinPort::layerDesignRuleWidth(i));
//                    port->defiPinPort::bounds(i, &xl, &yl, &xh, &yh);
//                    fprintf(fout, "%d %d %d %d ", xl, yl, xh, yh);
//                }
//                for (i = 0; i < port->defiPinPort::numPolygons(); i++) {
//                    fprintf(fout, "\n     + POLYGON %s ",
//                            port->defiPinPort::polygonName(i));
//                    if (port->defiPinPort::polygonMask(i))
//                        fprintf(fout, "MASK %d ",
//                                port->defiPinPort::polygonMask(i));
//                    if (port->defiPinPort::hasPolygonSpacing(i))
//                        fprintf(fout, "SPACING %d ",
//                                port->defiPinPort::polygonSpacing(i));
//                    if (port->defiPinPort::hasPolygonDesignRuleWidth(i))
//                        fprintf(fout, "DESIGNRULEWIDTH %d ",
//                                port->defiPinPort::polygonDesignRuleWidth(i));
//                    points = port->defiPinPort::getPolygon(i);
//                    for (k = 0; k < points.numPoints; k++)
//                        fprintf(fout, "( %d %d ) ", points.x[k], points.y[k]);
//                }
//                for (i = 0; i < port->defiPinPort::numVias(); i++) {
//                    if (port->defiPinPort::viaTopMask(i) || port->defiPinPort::viaCutMask(i)
//                            || port->defiPinPort::viaBottomMask(i)) {
//                        fprintf(fout, "\n     + VIA %s MASK %d%d%d ( %d %d ) ",
//                                port->defiPinPort::viaName(i),
//                                port->defiPinPort::viaTopMask(i),
//                                port->defiPinPort::viaCutMask(i),
//                                port->defiPinPort::viaBottomMask(i),
//                                port->defiPinPort::viaPtX(i),
//                                port->defiPinPort::viaPtY(i));
//                    } else {
//                        fprintf(fout, "\n     + VIA %s ( %d %d ) ",
//                                port->defiPinPort::viaName(i),
//                                port->defiPinPort::viaPtX(i),
//                                port->defiPinPort::viaPtY(i));
//                    }
//                }
//                if (port->defiPinPort::hasPlacement()) {
//                    if (port->defiPinPort::isPlaced()) {
//                        fprintf(fout, "\n     + PLACED ");
//                        fprintf(fout, "( %d %d ) %s ",
//                                port->defiPinPort::placementX(),
//                                port->defiPinPort::placementY(),
//                                orientStr(port->defiPinPort::orient()));
//                    }
//                    if (port->defiPinPort::isCover()) {
//                        fprintf(fout, "\n     + COVER ");
//                        fprintf(fout, "( %d %d ) %s ",
//                                port->defiPinPort::placementX(),
//                                port->defiPinPort::placementY(),
//                                orientStr(port->defiPinPort::orient()));
//                    }
//                    if (port->defiPinPort::isFixed()) {
//                        fprintf(fout, "\n     + FIXED ");
//                        fprintf(fout, "( %d %d ) %s ",
//                                port->defiPinPort::placementX(),
//                                port->defiPinPort::placementY(),
//                                orientStr(port->defiPinPort::orient()));
//                    }
//                }
//            }
//        }
//        if (pin->defiPin::hasPlacement()) {
//            if (pin->defiPin::isPlaced()) {
//                fprintf(fout, "+ PLACED ");
//                fprintf(fout, "( %d %d ) %s ", pin->defiPin::placementX(),
//                        pin->defiPin::placementY(),
//                        orientStr(pin->defiPin::orient()));
//            }
//            if (pin->defiPin::isCover()) {
//                fprintf(fout, "+ COVER ");
//                fprintf(fout, "( %d %d ) %s ", pin->defiPin::placementX(),
//                        pin->defiPin::placementY(),
//                        orientStr(pin->defiPin::orient()));
//            }
//            if (pin->defiPin::isFixed()) {
//                fprintf(fout, "+ FIXED ");
//                fprintf(fout, "( %d %d ) %s ", pin->defiPin::placementX(),
//                        pin->defiPin::placementY(),
//                        orientStr(pin->defiPin::orient()));
//            }
//            if (pin->defiPin::isUnplaced())
//                fprintf(fout, "+ UNPLACED ");
//        }
//        if (pin->defiPin::hasSpecial()) {
//            fprintf(fout, "+ SPECIAL ");
//        }
//        if (pin->hasAPinPartialMetalArea()) {
//            for (i = 0; i < pin->defiPin::numAPinPartialMetalArea(); i++) {
//                fprintf(fout, "ANTENNAPINPARTIALMETALAREA %d",
//                        pin->APinPartialMetalArea(i));
//                if (*(pin->APinPartialMetalAreaLayer(i)))
//                    fprintf(fout, " LAYER %s",
//                            pin->APinPartialMetalAreaLayer(i));
//                fprintf(fout, "\n");
//            }
//        }
//        if (pin->hasAPinPartialMetalSideArea()) {
//            for (i = 0; i < pin->defiPin::numAPinPartialMetalSideArea(); i++) {
//                fprintf(fout, "ANTENNAPINPARTIALMETALSIDEAREA %d",
//                        pin->APinPartialMetalSideArea(i));
//                if (*(pin->APinPartialMetalSideAreaLayer(i)))
//                    fprintf(fout, " LAYER %s",
//                            pin->APinPartialMetalSideAreaLayer(i));
//                fprintf(fout, "\n");
//            }
//        }
//        if (pin->hasAPinDiffArea()) {
//            for (i = 0; i < pin->defiPin::numAPinDiffArea(); i++) {
//                fprintf(fout, "ANTENNAPINDIFFAREA %d", pin->APinDiffArea(i));
//                if (*(pin->APinDiffAreaLayer(i)))
//                    fprintf(fout, " LAYER %s", pin->APinDiffAreaLayer(i));
//                fprintf(fout, "\n");
//            }
//        }
//        if (pin->hasAPinPartialCutArea()) {
//            for (i = 0; i < pin->defiPin::numAPinPartialCutArea(); i++) {
//                fprintf(fout, "ANTENNAPINPARTIALCUTAREA %d",
//                        pin->APinPartialCutArea(i));
//                if (*(pin->APinPartialCutAreaLayer(i)))
//                    fprintf(fout, " LAYER %s", pin->APinPartialCutAreaLayer(i));
//                fprintf(fout, "\n");
//            }
//        }

//        for (j = 0; j < pin->numAntennaModel(); j++) {
//            aModel = pin->antennaModel(j);

//            fprintf(fout, "ANTENNAMODEL %s\n",
//                    aModel->defiPinAntennaModel::antennaOxide());

//            if (aModel->hasAPinGateArea()) {
//                for (i = 0; i < aModel->defiPinAntennaModel::numAPinGateArea();
//                     i++) {
//                    fprintf(fout, "ANTENNAPINGATEAREA %d",
//                            aModel->APinGateArea(i));
//                    if (aModel->hasAPinGateAreaLayer(i))
//                        fprintf(fout, " LAYER %s", aModel->APinGateAreaLayer(i));
//                    fprintf(fout, "\n");
//                }
//            }
//            if (aModel->hasAPinMaxAreaCar()) {
//                for (i = 0;
//                     i < aModel->defiPinAntennaModel::numAPinMaxAreaCar(); i++) {
//                    fprintf(fout, "ANTENNAPINMAXAREACAR %d",
//                            aModel->APinMaxAreaCar(i));
//                    if (aModel->hasAPinMaxAreaCarLayer(i))
//                        fprintf(fout,
//                                " LAYER %s", aModel->APinMaxAreaCarLayer(i));
//                    fprintf(fout, "\n");
//                }
//            }
//            if (aModel->hasAPinMaxSideAreaCar()) {
//                for (i = 0;
//                     i < aModel->defiPinAntennaModel::numAPinMaxSideAreaCar();
//                     i++) {
//                    fprintf(fout, "ANTENNAPINMAXSIDEAREACAR %d",
//                            aModel->APinMaxSideAreaCar(i));
//                    if (aModel->hasAPinMaxSideAreaCarLayer(i))
//                        fprintf(fout,
//                                " LAYER %s", aModel->APinMaxSideAreaCarLayer(i));
//                    fprintf(fout, "\n");
//                }
//            }
//            if (aModel->hasAPinMaxCutCar()) {
//                for (i = 0; i < aModel->defiPinAntennaModel::numAPinMaxCutCar();
//                     i++) {
//                    fprintf(fout, "ANTENNAPINMAXCUTCAR %d",
//                            aModel->APinMaxCutCar(i));
//                    if (aModel->hasAPinMaxCutCarLayer(i))
//                        fprintf(fout, " LAYER %s",
//                                aModel->APinMaxCutCarLayer(i));
//                    fprintf(fout, "\n");
//                }
//            }
//        }
//        fprintf(fout, ";\n");
//        --numObjs;
//        if (numObjs <= 0)
//            fprintf(fout, "END PINS\n");
//        break;
    }
    case defrDefaultCapCbkType :
        i = (long)cl;
        fprintf(fout, "DEFAULTCAP %d\n", i);
        numObjs = i;
        break;
    case defrRowCbkType :{
        row = (defiRow*)cl;

        // --------------------------------------------------
        static int count = 0;
        if( count==0 ){
            Row::site.setStep( row->defiRow::xStep() );
            Row::site.setHeight( Row::site.height() * unitDEF );
            assert( row->defiRow::yStep()==0 );
            _cir->setRectChip( _cir->chipRect() );
            ++count;
        }

        _cir->addRow( Row() );
        Row &currow = _cir->row( _cir->numRows()-1 );
        currow.setPosition( row->defiRow::x(), row->defiRow::y() );
        currow.setNumSites( row->defiRow::xNum() );
        currow.setOrient( (util::Orient) row->defiRow::orient() );
        currow.setInterval( row->defiRow::x(), row->defiRow::x() + row->defiRow::xNum()*_cir->siteStep() );

        // check if a defined row totally cover currow
        for( int i=_cir->numRows()-2; i>0; --i ){
            Row &oldRow = _cir->row(i);
            if( currow.x() != oldRow.x() )
                continue;
            for( size_t j=0; j<oldRow.numInterval(); ++j ){
                if( oldRow.interval(j).first  < currow.interval(0).first &&
                    oldRow.interval(j).second > currow.interval(0).second ){
                    cerr << "overlapping rows: " << currow.name() << " , " << oldRow.name() << endl;
                    break;
                }
            }
        }
        if( isShowDEFMsg || gArg.checkExist( "showDefRow" ) )
            currow.showInfo();
        break;
        // --------------------------------------------------

//        fprintf(fout, "ROW %s %s %g %g %s ", row->defiRow::name(),
//                row->defiRow::macro(), row->defiRow::x(), row->defiRow::y(),
//                orientStr(row->defiRow::orient()));
//        if (row->defiRow::hasDo()) {
//            fprintf(fout, "DO %g BY %g ",
//                    row->defiRow::xNum(), row->defiRow::yNum());
//            if (row->defiRow::hasDoStep())
//                fprintf(fout, "STEP %g %g ;\n",
//                        row->defiRow::xStep(), row->defiRow::yStep());
//            else
//                fprintf(fout, ";\n");
//        } else
//            fprintf(fout, ";\n");
//        if (row->defiRow::numProps() > 0) {
//            for (i = 0; i < row->defiRow::numProps(); i++) {
//                fprintf(fout, "  + PROPERTY %s %s ",
//                        row->defiRow::propName(i),
//                        row->defiRow::propValue(i));
//                switch (row->defiRow::propType(i)) {
//                case 'R': fprintf(fout, "REAL ");
//                    break;
//                case 'I': fprintf(fout, "INTEGER ");
//                    break;
//                case 'S': fprintf(fout, "STRING ");
//                    break;
//                case 'Q': fprintf(fout, "QUOTESTRING ");
//                    break;
//                case 'N': fprintf(fout, "NUMBER ");
//                    break;
//                }
//            }
//            fprintf(fout, ";\n");
//        }
//        break;
    }
    case defrTrackCbkType :{
        track = (defiTrack*)cl;
        // --------------------------------------------------
        string direction = ( strcmp( track->defiTrack::macro(), "Y" )==0 ) ? "HORIZONTAL" : "VERTICAL";

        for( int j=0; j<track->defiTrack::numLayers(); ++j ){
            Layer &curlayer = _cir->layer( mtlStr2Int( track->defiTrack::layer(j) )-1 );

            if( direction!=curlayer.direction() ){
                curlayer.setDirection( direction );
                if( param.bmt != ParamHandler::BMT_ICCAD17 )
                    cout << "[WARNNING] " << curlayer.name() << " directions in .lef("
                         << curlayer.direction() << ") .def(" << direction << ") do not align: follows .def." << endl;
            }
            double offset = track->defiTrack::x();

            // check offset
            if( offset!=curlayer.offset()*unitDEF ){ // in DBU
                if( curlayer.offset()==0 )
                    curlayer.setOffset( offset );
                else if( param.bmt != ParamHandler::BMT_ICCAD17 ){
                    cout << "[WARNNING] " << curlayer.name() << " offset in .lef .def do not align." << endl;
                    cout << "           offset @ .def : " << track->defiTrack::x() << " dbu" << endl;
                    cout << "           offset @ .lef : " << curlayer.offset() << " um" << endl;
                    cout << "           unitDEF       : " << unitDEF << endl;
                    cout << "           unitLEF       : " << unitLEF << endl;
                }
            }
            curlayer.setNumTracks( track->defiTrack::xNum() );
            if( track->defiTrack::xStep() != curlayer.pitch()*unitDEF ){ // in microns
                if( param.bmt != ParamHandler::BMT_ICCAD17 ){
                    cout << "[ERROR] " << curlayer.name() << " pitch in .lef(" <<
                            track->defiTrack::xStep() << ") .def(" << curlayer.pitch()*unitDEF << ") do not align." << endl;
                }
            }
        }
        break;
        // --------------------------------------------------

//        if (track->firstTrackMask()) {
//            if (track->sameMask()) {
//                fprintf(fout, "TRACKS %s %g DO %g STEP %g MASK %d SAMEMASK LAYER ",
//                        track->defiTrack::macro(), track->defiTrack::x(),
//                        track->defiTrack::xNum(), track->defiTrack::xStep(),
//                        track->defiTrack::firstTrackMask());
//            } else {
//                fprintf(fout, "TRACKS %s %g DO %g STEP %g MASK %d LAYER ",
//                        track->defiTrack::macro(), track->defiTrack::x(),
//                        track->defiTrack::xNum(), track->defiTrack::xStep(),
//                        track->defiTrack::firstTrackMask());
//            }
//        } else {
//            fprintf(fout, "TRACKS %s %g DO %g STEP %g LAYER ",
//                    track->defiTrack::macro(), track->defiTrack::x(),
//                    track->defiTrack::xNum(), track->defiTrack::xStep());
//        }
//        for (i = 0; i < track->defiTrack::numLayers(); i++)
//            fprintf(fout, "%s ", track->defiTrack::layer(i));
//        fprintf(fout, ";\n");
//        break;
    }
    case defrGcellGridCbkType :
        // to-do GCELL (global routing cell)
        gcg = (defiGcellGrid*)cl;
        if( gcg->defiGcellGrid::macro() ) {}
//        fprintf(fout, "GCELLGRID %s %d DO %d STEP %g ;\n",
//                gcg->defiGcellGrid::macro(), gcg->defiGcellGrid::x(),
//                gcg->defiGcellGrid::xNum(), gcg->defiGcellGrid::xStep());
        break;
    case defrViaCbkType :
        via = (defiVia*)cl;
        if( via->defiVia::name() ) {}
        --numObjs;
        break;
//        via = (defiVia*)cl;
//        fprintf(fout, "- %s ", via->defiVia::name());
//        if (via->defiVia::hasPattern())
//            fprintf(fout, "+ PATTERNNAME %s ", via->defiVia::pattern());
//        for (i = 0; i < via->defiVia::numLayers(); i++) {
//            via->defiVia::layer(i, &name, &xl, &yl, &xh, &yh);
//            int rectMask = via->defiVia::rectMask(i);

//            if (rectMask) {
//                fprintf(fout, "+ RECT %s + MASK %d %d %d %d %d \n",
//                        name, rectMask, xl, yl, xh, yh);
//            } else {
//                fprintf(fout, "+ RECT %s %d %d %d %d \n",
//                        name, xl, yl, xh, yh);
//            }
//        }
//        // POLYGON
//        if (via->defiVia::numPolygons()) {
//            struct defiPoints points;
//            for (i = 0; i < via->defiVia::numPolygons(); i++) {
//                int polyMask = via->defiVia::polyMask(i);

//                if (polyMask) {
//                    fprintf(fout, "\n  + POLYGON %s + MASK %d ",
//                            via->polygonName(i), polyMask);
//                } else {
//                    fprintf(fout, "\n  + POLYGON %s ", via->polygonName(i));
//                }
//                points = via->getPolygon(i);
//                for (j = 0; j < points.numPoints; j++)
//                    fprintf(fout, "%d %d ", points.x[j], points.y[j]);
//            }
//        }
//        fprintf(fout, " ;\n");
//        if (via->defiVia::hasViaRule()) {
//            char *vrn, *bl, *cl, *tl;
//            int xs, ys, xcs, ycs, xbe, ybe, xte, yte;
//            int cr, cc, xo, yo, xbo, ybo, xto, yto;
//            (void)via->defiVia::viaRule(&vrn, &xs, &ys, &bl, &cl, &tl, &xcs,
//                                        &ycs, &xbe, &ybe, &xte, &yte);
//            fprintf(fout, "+ VIARULE '%s'\n", vrn);
//            fprintf(fout, "  + CUTSIZE %d %d\n", xs, ys);
//            fprintf(fout, "  + LAYERS %s %s %s\n", bl, cl, tl);
//            fprintf(fout, "  + CUTSPACING %d %d\n", xcs, ycs);
//            fprintf(fout, "  + ENCLOSURE %d %d %d %d\n", xbe, ybe, xte, yte);
//            if (via->defiVia::hasRowCol()) {
//                (void)via->defiVia::rowCol(&cr, &cc);
//                fprintf(fout, "  + ROWCOL %d %d\n", cr, cc);
//            }
//            if (via->defiVia::hasOrigin()) {
//                (void)via->defiVia::origin(&xo, &yo);
//                fprintf(fout, "  + ORIGIN %d %d\n", xo, yo);
//            }
//            if (via->defiVia::hasOffset()) {
//                (void)via->defiVia::offset(&xbo, &ybo, &xto, &yto);
//                fprintf(fout, "  + OFFSET %d %d %d %d\n", xbo, ybo, xto, yto);
//            }
//            if (via->defiVia::hasCutPattern())
//                fprintf(fout, "  + PATTERN '%s'\n", via->defiVia::cutPattern());
//        }
//        --numObjs;
//        if (numObjs <= 0)
//            fprintf(fout, "END VIAS\n");
//        break;
    case defrRegionCbkType :
        re = (defiRegion*)cl;
        // --------------------------------------------------
        _cir->addFregion( Fregion() );

        for (i = 0; i < re->defiRegion::numRectangles(); i++){
            Rect rect( re->defiRegion::xl(i), re->defiRegion::yl(i),
                       re->defiRegion::xh(i), re->defiRegion::yh(i) );
            _cir->fregion(fregionCount).add_rect( rect );
        }

        ++fregionCount;
        --numObjs;

        if( numObjs <= 0 )
            fregionCount = 0;
        break;
        // --------------------------------------------------
//        fprintf(fout, "- %s ", re->defiRegion::name());
//        for (i = 0; i < re->defiRegion::numRectangles(); i++)
//            fprintf(fout, "%d %d %d %d \n", re->defiRegion::xl(i),
//                    re->defiRegion::yl(i), re->defiRegion::xh(i),
//                    re->defiRegion::yh(i));
//        if (re->defiRegion::hasType())
//            fprintf(fout, "+ TYPE %s\n", re->defiRegion::type());
//        if (re->defiRegion::numProps()) {
//            for (i = 0; i < re->defiRegion::numProps(); i++) {
//                fprintf(fout, "+ PROPERTY %s %s ", re->defiRegion::propName(i),
//                        re->defiRegion::propValue(i));
//                switch (re->defiRegion::propType(i)) {
//                case 'R': fprintf(fout, "REAL ");
//                    break;
//                case 'I': fprintf(fout, "INTEGER ");
//                    break;
//                case 'S': fprintf(fout, "STRING ");
//                    break;
//                case 'Q': fprintf(fout, "QUOTESTRING ");
//                    break;
//                case 'N': fprintf(fout, "NUMBER ");
//                    break;
//                }
//            }
//        }
//        fprintf(fout, ";\n");
//        --numObjs;
//        if (numObjs <= 0) {
//            fprintf(fout, "END REGIONS\n");
//        }
//        break;
    case defrGroupNameCbkType :
        // --------------------------------------------------
        _cir->fregion(fregionCount).setName( (char*)cl );
        // --------------------------------------------------
//        if ((char*)cl) {
//            fprintf(fout, "- %s", (char*)cl);
//        }
        break;
    case defrGroupMemberCbkType :
        // --------------------------------------------------
        _cir->fregion(fregionCount).add_prefix( (char*)cl );
        // --------------------------------------------------
//        if ((char*)cl) {
//            fprintf(fout, " %s", (char*)cl);
//        }
        break;
    case defrComponentMaskShiftLayerCbkType :
        fprintf(fout, "COMPONENTMASKSHIFT ");
        
        //        for (i = 0; i < maskShiftLayer->defiComponentMaskShiftLayer::numMaskShiftLayers(); i++) {
        //           fprintf(fout, "%s ", maskShiftLayer->defiComponentMaskShiftLayer::maskShiftLayer(i));
        //        }
        fprintf(fout, ";\n");
        break;
    case defrGroupCbkType :
        // --------------------------------------------------
        ++fregionCount;

        --numObjs;
        break;
        // --------------------------------------------------
//        group = (defiGroup*)cl;
//        if (group->defiGroup::hasMaxX() | group->defiGroup::hasMaxY()
//                | group->defiGroup::hasPerim()) {
//            fprintf(fout, "\n  + SOFT ");
//            if (group->defiGroup::hasPerim())
//                fprintf(fout, "MAXHALFPERIMETER %d ",
//                        group->defiGroup::perim());
//            if (group->defiGroup::hasMaxX())
//                fprintf(fout, "MAXX %d ", group->defiGroup::maxX());
//            if (group->defiGroup::hasMaxY())
//                fprintf(fout, "MAXY %d ", group->defiGroup::maxY());
//        }
//        if (group->defiGroup::hasRegionName())
//            fprintf(fout, "\n  + REGION %s ", group->defiGroup::regionName());
//        if (group->defiGroup::hasRegionBox()) {
//            int *gxl, *gyl, *gxh, *gyh;
//            int size;
//            group->defiGroup::regionRects(&size, &gxl, &gyl, &gxh, &gyh);
//            for (i = 0; i < size; i++)
//                fprintf(fout, "REGION %d %d %d %d ", gxl[i], gyl[i],
//                        gxh[i], gyh[i]);
//        }
//        if (group->defiGroup::numProps()) {
//            for (i = 0; i < group->defiGroup::numProps(); i++) {
//                fprintf(fout, "\n  + PROPERTY %s %s ",
//                        group->defiGroup::propName(i),
//                        group->defiGroup::propValue(i));
//                switch (group->defiGroup::propType(i)) {
//                case 'R': fprintf(fout, "REAL ");
//                    break;
//                case 'I': fprintf(fout, "INTEGER ");
//                    break;
//                case 'S': fprintf(fout, "STRING ");
//                    break;
//                case 'Q': fprintf(fout, "QUOTESTRING ");
//                    break;
//                case 'N': fprintf(fout, "NUMBER ");
//                    break;
//                }
//            }
//        }
//        fprintf(fout, " ;\n");
//        --numObjs;
//        if (numObjs <= 0)
//            fprintf(fout, "END GROUPS\n");
//        break;
    case defrScanchainCbkType :
        sc = (defiScanchain*)cl;
        fprintf(fout, "- %s\n", sc->defiScanchain::name());
        if (sc->defiScanchain::hasStart()) {
            sc->defiScanchain::start(&a1, &b1);
            fprintf(fout, "  + START %s %s\n", a1, b1);
        }
        if (sc->defiScanchain::hasStop()) {
            sc->defiScanchain::stop(&a1, &b1);
            fprintf(fout, "  + STOP %s %s\n", a1, b1);
        }
        if (sc->defiScanchain::hasCommonInPin() ||
                sc->defiScanchain::hasCommonOutPin()) {
            fprintf(fout, "  + COMMONSCANPINS ");
            if (sc->defiScanchain::hasCommonInPin())
                fprintf(fout, " ( IN %s ) ", sc->defiScanchain::commonInPin());
            if (sc->defiScanchain::hasCommonOutPin())
                fprintf(fout, " ( OUT %s ) ",sc->defiScanchain::commonOutPin());
            fprintf(fout, "\n");
        }
        if (sc->defiScanchain::hasFloating()) {
            sc->defiScanchain::floating(&size, &inst, &inPin, &outPin, &bits);
            if (size > 0)
                fprintf(fout, "  + FLOATING\n");
            for (i = 0; i < size; i++) {
                fprintf(fout, "    %s ", inst[i]);
                if (inPin[i])
                    fprintf(fout, "( IN %s ) ", inPin[i]);
                if (outPin[i])
                    fprintf(fout, "( OUT %s ) ", outPin[i]);
                if (bits[i] != -1)
                    fprintf(fout, "( BITS %d ) ", bits[i]);
                fprintf(fout, "\n");
            }
        }

        if (sc->defiScanchain::hasOrdered()) {
            for (i = 0; i < sc->defiScanchain::numOrderedLists(); i++) {
                sc->defiScanchain::ordered(i, &size, &inst, &inPin, &outPin,
                                           &bits);
                if (size > 0)
                    fprintf(fout, "  + ORDERED\n");
                for (j = 0; j < size; j++) {
                    fprintf(fout, "    %s ", inst[j]);
                    if (inPin[j])
                        fprintf(fout, "( IN %s ) ", inPin[j]);
                    if (outPin[j])
                        fprintf(fout, "( OUT %s ) ", outPin[j]);
                    if (bits[j] != -1)
                        fprintf(fout, "( BITS %d ) ", bits[j]);
                    fprintf(fout, "\n");
                }
            }
        }

        if (sc->defiScanchain::hasPartition()) {
            fprintf(fout, "  + PARTITION %s ",
                    sc->defiScanchain::partitionName());
            if (sc->defiScanchain::hasPartitionMaxBits())
                fprintf(fout, "MAXBITS %d ",
                        sc->defiScanchain::partitionMaxBits());
        }
        fprintf(fout, ";\n");
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END SCANCHAINS\n");
        break;
    case defrIOTimingCbkType :
        iot = (defiIOTiming*)cl;
        fprintf(fout, "- ( %s %s )\n", iot->defiIOTiming::inst(),
                iot->defiIOTiming::pin());
        if (iot->defiIOTiming::hasSlewRise())
            fprintf(fout, "  + RISE SLEWRATE %g %g\n",
                    iot->defiIOTiming::slewRiseMin(),
                    iot->defiIOTiming::slewRiseMax());
        if (iot->defiIOTiming::hasSlewFall())
            fprintf(fout, "  + FALL SLEWRATE %g %g\n",
                    iot->defiIOTiming::slewFallMin(),
                    iot->defiIOTiming::slewFallMax());
        if (iot->defiIOTiming::hasVariableRise())
            fprintf(fout, "  + RISE VARIABLE %g %g\n",
                    iot->defiIOTiming::variableRiseMin(),
                    iot->defiIOTiming::variableRiseMax());
        if (iot->defiIOTiming::hasVariableFall())
            fprintf(fout, "  + FALL VARIABLE %g %g\n",
                    iot->defiIOTiming::variableFallMin(),
                    iot->defiIOTiming::variableFallMax());
        if (iot->defiIOTiming::hasCapacitance())
            fprintf(fout, "  + CAPACITANCE %g\n",
                    iot->defiIOTiming::capacitance());
        if (iot->defiIOTiming::hasDriveCell()) {
            fprintf(fout, "  + DRIVECELL %s ",
                    iot->defiIOTiming::driveCell());
            if (iot->defiIOTiming::hasFrom())
                fprintf(fout, "  FROMPIN %s ",
                        iot->defiIOTiming::from());
            if (iot->defiIOTiming::hasTo())
                fprintf(fout, "  TOPIN %s ",
                        iot->defiIOTiming::to());
            if (iot->defiIOTiming::hasParallel())
                fprintf(fout, "PARALLEL %g",
                        iot->defiIOTiming::parallel());
            fprintf(fout, "\n");
        }
        fprintf(fout, ";\n");
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END IOTIMINGS\n");
        break;
    case defrFPCCbkType :
        fpc = (defiFPC*)cl;
        fprintf(fout, "- %s ", fpc->defiFPC::name());
        if (fpc->defiFPC::isVertical())
            fprintf(fout, "VERTICAL ");
        if (fpc->defiFPC::isHorizontal())
            fprintf(fout, "HORIZONTAL ");
        if (fpc->defiFPC::hasAlign())
            fprintf(fout, "ALIGN ");
        if (fpc->defiFPC::hasMax())
            fprintf(fout, "%g ", fpc->defiFPC::alignMax());
        if (fpc->defiFPC::hasMin())
            fprintf(fout, "%g ", fpc->defiFPC::alignMin());
        if (fpc->defiFPC::hasEqual())
            fprintf(fout, "%g ", fpc->defiFPC::equal());
        for (i = 0; i < fpc->defiFPC::numParts(); i++) {
            fpc->defiFPC::getPart(i, &corner, &typ, &name);
            if (corner == 'B')
                fprintf(fout, "BOTTOMLEFT ");
            else
                fprintf(fout, "TOPRIGHT ");
            if (typ == 'R')
                fprintf(fout, "ROWS %s ", name);
            else
                fprintf(fout, "COMPS %s ", name);
        }
        fprintf(fout, ";\n");
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END FLOORPLANCONSTRAINTS\n");
        break;
    case defrTimingDisableCbkType :
        td = (defiTimingDisable*)cl;
        if (td->defiTimingDisable::hasFromTo())
            fprintf(fout, "- FROMPIN %s %s ",
                    //td->defiTimingDisable::fromInst(),
                    td->defiTimingDisable::fromPin(),
                    //td->defiTimingDisable::toInst(),
                    td->defiTimingDisable::toPin());
        if (td->defiTimingDisable::hasThru())
            fprintf(fout, "- THRUPIN %s %s ",
                    td->defiTimingDisable::thruInst(),
                    td->defiTimingDisable::thruPin());
        if (td->defiTimingDisable::hasMacroFromTo())
            fprintf(fout, "- MACRO %s FROMPIN %s %s ",
                    td->defiTimingDisable::macroName(),
                    td->defiTimingDisable::fromPin(),
                    td->defiTimingDisable::toPin());
        if (td->defiTimingDisable::hasMacroThru())
            fprintf(fout, "- MACRO %s THRUPIN %s %s ",
                    td->defiTimingDisable::macroName(),
                    td->defiTimingDisable::fromPin(),
                    td->defiTimingDisable::toPin());
        fprintf(fout, ";\n");
        break;
    case defrPartitionCbkType :
        part = (defiPartition*)cl;
        fprintf(fout, "- %s ", part->defiPartition::name());
        if (part->defiPartition::isSetupRise() |
                part->defiPartition::isSetupFall() |
                part->defiPartition::isHoldRise() |
                part->defiPartition::isHoldFall()) {
            // has turnoff
            fprintf(fout, "TURNOFF ");
            if (part->defiPartition::isSetupRise())
                fprintf(fout, "SETUPRISE ");
            if (part->defiPartition::isSetupFall())
                fprintf(fout, "SETUPFALL ");
            if (part->defiPartition::isHoldRise())
                fprintf(fout, "HOLDRISE ");
            if (part->defiPartition::isHoldFall())
                fprintf(fout, "HOLDFALL ");
        }
        itemT = part->defiPartition::itemType();
        dir = part->defiPartition::direction();
        if (strcmp(itemT, "CLOCK") == 0) {
            if (dir == 'T')    // toclockpin
                fprintf(fout, "+ TOCLOCKPIN %s %s ",
                        part->defiPartition::instName(),
                        part->defiPartition::pinName());
            if (dir == 'F')    // fromclockpin
                fprintf(fout, "+ FROMCLOCKPIN %s %s ",
                        part->defiPartition::instName(),
                        part->defiPartition::pinName());
            if (part->defiPartition::hasMin())
                fprintf(fout, "MIN %g %g ",
                        part->defiPartition::partitionMin(),
                        part->defiPartition::partitionMax());
            if (part->defiPartition::hasMax())
                fprintf(fout, "MAX %g %g ",
                        part->defiPartition::partitionMin(),
                        part->defiPartition::partitionMax());
            fprintf(fout, "PINS ");
            for (i = 0; i < part->defiPartition::numPins(); i++)
                fprintf(fout, "%s ", part->defiPartition::pin(i));
        } else if (strcmp(itemT, "IO") == 0) {
            if (dir == 'T')    // toiopin
                fprintf(fout, "+ TOIOPIN %s %s ",
                        part->defiPartition::instName(),
                        part->defiPartition::pinName());
            if (dir == 'F')    // fromiopin
                fprintf(fout, "+ FROMIOPIN %s %s ",
                        part->defiPartition::instName(),
                        part->defiPartition::pinName());
        } else if (strcmp(itemT, "COMP") == 0) {
            if (dir == 'T')    // tocomppin
                fprintf(fout, "+ TOCOMPPIN %s %s ",
                        part->defiPartition::instName(),
                        part->defiPartition::pinName());
            if (dir == 'F')    // fromcomppin
                fprintf(fout, "+ FROMCOMPPIN %s %s ",
                        part->defiPartition::instName(),
                        part->defiPartition::pinName());
        }
        fprintf(fout, ";\n");
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END PARTITIONS\n");
        break;

    case defrPinPropCbkType :
        pprop = (defiPinProp*)cl;
        if (pprop->defiPinProp::isPin())
            fprintf(fout, "- PIN %s ", pprop->defiPinProp::pinName());
        else
            fprintf(fout, "- %s %s ",
                    pprop->defiPinProp::instName(),
                    pprop->defiPinProp::pinName());
        fprintf(fout, ";\n");
        if (pprop->defiPinProp::numProps() > 0) {
            for (i = 0; i < pprop->defiPinProp::numProps(); i++) {
                fprintf(fout, "  + PROPERTY %s %s ",
                        pprop->defiPinProp::propName(i),
                        pprop->defiPinProp::propValue(i));
                switch (pprop->defiPinProp::propType(i)) {
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
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END PINPROPERTIES\n");
        break;
    case defrBlockageCbkType :{
        block = (defiBlockage*)cl;

        // --------------------------------------------------
        if( block->defiBlockage::hasPlacement() ){ // placement blockage
            for( i=0; i<block->defiBlockage::numRectangles(); ++i ){
                Rect rect( block->defiBlockage::xl(i), block->defiBlockage::yl(i),
                           block->defiBlockage::xh(i), block->defiBlockage::yh(i) );
                _cir->addBlockage( rect );
            }
        }else if( block->defiBlockage::hasLayer() ){ // routing blockage
            Layer &curlayer = _cir->layer( mtlStr2Int( block->defiBlockage::layerName() )-1 );
            for( i=0; i<block->defiBlockage::numRectangles(); ++i ){
                Rect rect( block->defiBlockage::xl(i), block->defiBlockage::yl(i),
                           block->defiBlockage::xh(i), block->defiBlockage::yh(i) );
                curlayer.addBlockage( rect );
            }
        }

        // --------------------------------------------------
        --numObjs;
        break;
//        if (block->defiBlockage::hasLayer()) {
//            fprintf(fout, "- LAYER %s\n", block->defiBlockage::layerName());
//            if (block->defiBlockage::hasComponent())
//                fprintf(fout, "   + COMPONENT %s\n",
//                        block->defiBlockage::layerComponentName());
//            if (block->defiBlockage::hasSlots())
//                fprintf(fout, "   + SLOTS\n");
//            if (block->defiBlockage::hasFills())
//                fprintf(fout, "   + FILLS\n");
//            if (block->defiBlockage::hasPushdown())
//                fprintf(fout, "   + PUSHDOWN\n");
//            if (block->defiBlockage::hasExceptpgnet())
//                fprintf(fout, "   + EXCEPTPGNET\n");
//            if (block->defiBlockage::hasMask())
//                fprintf(fout, "   + MASK %d\n", block->defiBlockage::mask());
//            if (block->defiBlockage::hasSpacing())
//                fprintf(fout, "   + SPACING %d\n",
//                        block->defiBlockage::minSpacing());
//            if (block->defiBlockage::hasDesignRuleWidth())
//                fprintf(fout, "   + DESIGNRULEWIDTH %d\n",
//                        block->defiBlockage::designRuleWidth());
//        }
//        else if (block->defiBlockage::hasPlacement()) {
//            fprintf(fout, "- PLACEMENT\n");
//            if (block->defiBlockage::hasSoft())
//                fprintf(fout, "   + SOFT\n");
//            if (block->defiBlockage::hasPartial())
//                fprintf(fout, "   + PARTIAL %g\n",
//                        block->defiBlockage::placementMaxDensity());
//            if (block->defiBlockage::hasComponent())
//                fprintf(fout, "   + COMPONENT %s\n",
//                        block->defiBlockage::placementComponentName());
//            if (block->defiBlockage::hasPushdown())
//                fprintf(fout, "   + PUSHDOWN\n");
//        }

//        for (i = 0; i < block->defiBlockage::numRectangles(); i++) {
//            fprintf(fout, "   RECT %d %d %d %d\n",
//                    block->defiBlockage::xl(i), block->defiBlockage::yl(i),
//                    block->defiBlockage::xh(i), block->defiBlockage::yh(i));
//        }

//        for (i = 0; i < block->defiBlockage::numPolygons(); i++) {
//            fprintf(fout, "   POLYGON ");
//            points = block->getPolygon(i);
//            for (j = 0; j < points.numPoints; j++)
//                fprintf(fout, "%d %d ", points.x[j], points.y[j]);
//            fprintf(fout, "\n");
//        }
//        fprintf(fout, ";\n");
//        --numObjs;
//        if (numObjs <= 0)
//            fprintf(fout, "END BLOCKAGES\n");
//        break;
    }
    case defrSlotCbkType :
        slots = (defiSlot*)cl;
        if (slots->defiSlot::hasLayer())
            fprintf(fout, "- LAYER %s\n", slots->defiSlot::layerName());

        for (i = 0; i < slots->defiSlot::numRectangles(); i++) {
            fprintf(fout, "   RECT %d %d %d %d\n",
                    slots->defiSlot::xl(i), slots->defiSlot::yl(i),
                    slots->defiSlot::xh(i), slots->defiSlot::yh(i));
        }
        for (i = 0; i < slots->defiSlot::numPolygons(); i++) {
            fprintf(fout, "   POLYGON ");
            points = slots->getPolygon(i);
            for (j = 0; j < points.numPoints; j++)
                fprintf(fout, "%d %d ", points.x[j], points.y[j]);
            fprintf(fout, ";\n");
        }
        fprintf(fout, ";\n");
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END SLOTS\n");
        break;
    case defrFillCbkType :
        fills = (defiFill*)cl;
        if (fills->defiFill::hasLayer()) {
            fprintf(fout, "- LAYER %s", fills->defiFill::layerName());
            if (fills->defiFill::layerMask()) {
                fprintf(fout, " + MASK %d", fills->defiFill::layerMask());
            }
            if (fills->defiFill::hasLayerOpc())
                fprintf(fout, " + OPC");
            fprintf(fout, "\n");

            for (i = 0; i < fills->defiFill::numRectangles(); i++) {
                fprintf(fout, "   RECT %d %d %d %d\n",
                        fills->defiFill::xl(i), fills->defiFill::yl(i),
                        fills->defiFill::xh(i), fills->defiFill::yh(i));
            }
            for (i = 0; i < fills->defiFill::numPolygons(); i++) {
                fprintf(fout, "   POLYGON ");
                points = fills->getPolygon(i);
                for (j = 0; j < points.numPoints; j++)
                    fprintf(fout, "%d %d ", points.x[j], points.y[j]);
                fprintf(fout, ";\n");
            }
            fprintf(fout, ";\n");
        }
        --numObjs;
        if (fills->defiFill::hasVia()) {
            fprintf(fout, "- VIA %s", fills->defiFill::viaName());
            if (fills->defiFill::viaTopMask() || fills->defiFill::viaCutMask()
                    || fills->defiFill::viaBottomMask()) {
                fprintf(fout, " + MASK %d%d%d",
                        fills->defiFill::viaTopMask(),
                        fills->defiFill::viaCutMask(),
                        fills->defiFill::viaBottomMask());
            }
            if (fills->defiFill::hasViaOpc())
                fprintf(fout, " + OPC");
            fprintf(fout, "\n");

            for (i = 0; i < fills->defiFill::numViaPts(); i++) {
                points = fills->getViaPts(i);
                for (j = 0; j < points.numPoints; j++)
                    fprintf(fout, " %d %d", points.x[j], points.y[j]);
                fprintf(fout, ";\n");
            }
            fprintf(fout, ";\n");
        }
        if (numObjs <= 0)
            fprintf(fout, "END FILLS\n");
        break;
    case defrStylesCbkType :
        struct defiPoints points;
        styles = (defiStyles*)cl;
        fprintf(fout, "- STYLE %d ", styles->defiStyles::style());
        points = styles->defiStyles::getPolygon();
        for (j = 0; j < points.numPoints; j++)
            fprintf(fout, "%d %d ", points.x[j], points.y[j]);
        fprintf(fout, ";\n");
        --numObjs;
        if (numObjs <= 0)
            fprintf(fout, "END STYLES\n");
        break;

    default: fprintf(fout, "BOGUS callback to cls.\n"); return 1;
    }
    return 0;
}


int dn(defrCallbackType_e c, const char* h, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    if (h) {}
    //  fprintf(fout, "DIVIDERCHAR \"%s\" ;\n",h);
    return 0;
}


int ext(defrCallbackType_e t, const char* c, defiUserData ud) {
    char* name;

    checkType(t);
    if ((long)ud != userData) dataError();

    switch (t) {
    case defrNetExtCbkType : name = address("net"); break;
    case defrComponentExtCbkType : name = address("component"); break;
    case defrPinExtCbkType : name = address("pin"); break;
    case defrViaExtCbkType : name = address("via"); break;
    case defrNetConnectionExtCbkType : name = address("net connection"); break;
    case defrGroupExtCbkType : name = address("group"); break;
    case defrScanChainExtCbkType : name = address("scanchain"); break;
    case defrIoTimingsExtCbkType : name = address("io timing"); break;
    case defrPartitionsExtCbkType : name = address("partition"); break;
    default: name = address("BOGUS"); return 1;
    }
    fprintf(fout, "  %s extension %s\n", name, c);
    return 0;
}

int extension(defrCallbackType_e c, const char* extsn, defiUserData ud) {
    checkType(c);
    if ((long)ud != userData) dataError();
    fprintf(fout, "BEGINEXT %s\n", extsn);
    return 0;
}

void* mallocCB(size_t size) {
    return malloc(size);
}

void* reallocCB(void* name, size_t size) {
    return realloc(name, size);
}

int unUsedCB(defrCallbackType_e c, void* any, defiUserData ud) {
    fprintf(fout, "This callback is not used.\n");
    if (c) {}
    if (any) {}
    if (ud) {}
    return 0;
}

void Parser::readDEF(const string &fileName)
{
    isShowDEFMsg = gArg.checkExist( "DEFMsg" );
    moduleCount = 0;
    FILE *f;
    fout = stdout;

    defrSetUserData((void*)3);
    defrSetDesignCbk(dname);
    defrSetTechnologyCbk(tname);
    defrSetExtensionCbk(extension);
    defrSetDesignEndCbk(done);
    defrSetPropDefStartCbk(propstart);
    defrSetPropCbk(prop);
    defrSetPropDefEndCbk(propend);
    defrSetNetCbk(netf);
    defrSetNetNameCbk(netNamef);
    defrSetNetNonDefaultRuleCbk(nondefRulef);
    defrSetNetSubnetNameCbk(subnetNamef);
    defrSetNetPartialPathCbk(netpath);
    defrSetSNetCbk(snetf);
    defrSetSNetPartialPathCbk(snetpath);
    defrSetSNetWireCbk(snetwire);
    defrSetComponentMaskShiftLayerCbk(compMSL);
    defrSetComponentCbk(compf);
    defrSetAddPathToNet();
    defrSetHistoryCbk(hist);
    defrSetConstraintCbk(constraint);
    defrSetAssertionCbk(constraint);
    defrSetArrayNameCbk(an);
    defrSetFloorPlanNameCbk(fn);
    defrSetDividerCbk(dn);
    defrSetBusBitCbk(bbn);
    defrSetNonDefaultCbk(ndr);

    defrSetAssertionsStartCbk(constraintst);
    defrSetConstraintsStartCbk(constraintst);
    defrSetComponentStartCbk(cs);
    defrSetPinPropStartCbk(cs);
    defrSetNetStartCbk(cs);
    defrSetStartPinsCbk(cs);
    defrSetViaStartCbk(cs);
    defrSetRegionStartCbk(cs);
    defrSetSNetStartCbk(cs);
    defrSetGroupsStartCbk(cs);
    defrSetScanchainsStartCbk(cs);
    defrSetIOTimingsStartCbk(cs);
    defrSetFPCStartCbk(cs);
    defrSetTimingDisablesStartCbk(cs);
    defrSetPartitionsStartCbk(cs);
    defrSetBlockageStartCbk(cs);
    defrSetSlotStartCbk(cs);
    defrSetFillStartCbk(cs);
    defrSetNonDefaultStartCbk(cs);
    defrSetStylesStartCbk(cs);

    // All of the extensions point to the same function.
    defrSetNetExtCbk(ext);
    defrSetComponentExtCbk(ext);
    defrSetPinExtCbk(ext);
    defrSetViaExtCbk(ext);
    defrSetNetConnectionExtCbk(ext);
    defrSetGroupExtCbk(ext);
    defrSetScanChainExtCbk(ext);
    defrSetIoTimingsExtCbk(ext);
    defrSetPartitionsExtCbk(ext);

    defrSetUnitsCbk(units);
    defrSetVersionCbk(vers);
    defrSetCaseSensitiveCbk(casesens);

    // The following calls are an example of using one function "cls"
    // to be the callback for many DIFFERENT types of constructs.
    // We have to cast the function type to meet the requirements
    // of each different set function.
    defrSetSiteCbk((defrSiteCbkFnType)cls);
    defrSetCanplaceCbk((defrSiteCbkFnType)cls);
    defrSetCannotOccupyCbk((defrSiteCbkFnType)cls);
    defrSetDieAreaCbk((defrBoxCbkFnType)cls);
    defrSetPinCapCbk((defrPinCapCbkFnType)cls);
    defrSetPinCbk((defrPinCbkFnType)cls);
    defrSetPinPropCbk((defrPinPropCbkFnType)cls);
    defrSetDefaultCapCbk((defrIntegerCbkFnType)cls);
    defrSetRowCbk((defrRowCbkFnType)cls);
    defrSetTrackCbk((defrTrackCbkFnType)cls);
    defrSetGcellGridCbk((defrGcellGridCbkFnType)cls);
    defrSetViaCbk((defrViaCbkFnType)cls);
    defrSetRegionCbk((defrRegionCbkFnType)cls);
    defrSetGroupNameCbk((defrStringCbkFnType)cls);
    defrSetGroupMemberCbk((defrStringCbkFnType)cls);
    defrSetGroupCbk((defrGroupCbkFnType)cls);
    defrSetScanchainCbk((defrScanchainCbkFnType)cls);
    defrSetIOTimingCbk((defrIOTimingCbkFnType)cls);
    defrSetFPCCbk((defrFPCCbkFnType)cls);
    defrSetTimingDisableCbk((defrTimingDisableCbkFnType)cls);
    defrSetPartitionCbk((defrPartitionCbkFnType)cls);
    defrSetBlockageCbk((defrBlockageCbkFnType)cls);
    defrSetSlotCbk((defrSlotCbkFnType)cls);
    defrSetFillCbk((defrFillCbkFnType)cls);
    defrSetStylesCbk((defrStylesCbkFnType)cls);

    defrSetAssertionsEndCbk(endfunc);
    defrSetComponentEndCbk(endfunc);
    defrSetConstraintsEndCbk(endfunc);
    defrSetNetEndCbk(endfunc);
    defrSetFPCEndCbk(endfunc);
    defrSetFPCEndCbk(endfunc);
    defrSetGroupsEndCbk(endfunc);
    defrSetIOTimingsEndCbk(endfunc);
    defrSetNetEndCbk(endfunc);
    defrSetPartitionsEndCbk(endfunc);
    defrSetRegionEndCbk(endfunc);
    defrSetSNetEndCbk(endfunc);
    defrSetScanchainsEndCbk(endfunc);
    defrSetPinEndCbk(endfunc);
    defrSetTimingDisablesEndCbk(endfunc);
    defrSetViaEndCbk(endfunc);
    defrSetPinPropEndCbk(endfunc);
    defrSetBlockageEndCbk(endfunc);
    defrSetSlotEndCbk(endfunc);
    defrSetFillEndCbk(endfunc);
    defrSetNonDefaultEndCbk(endfunc);
    defrSetStylesEndCbk(endfunc);

    defrSetMallocFunction(mallocCB);
    defrSetReallocFunction(reallocCB);
    defrSetFreeFunction(freeCB);

//    defrSetLineNumberFunction(lineNumberCB);
//    defrSetDeltaNumberLines(50);

    //defrSetRegisterUnusedCallbacks();

    // Testing to set the number of warnings
    defrSetAssertionWarnings(3);
    defrSetBlockageWarnings(3);
    defrSetCaseSensitiveWarnings(3);
    defrSetComponentWarnings(3);
    defrSetConstraintWarnings(0);
    defrSetDefaultCapWarnings(3);
    defrSetGcellGridWarnings(3);
    defrSetIOTimingWarnings(3);
    defrSetNetWarnings(3);
    defrSetNonDefaultWarnings(3);
    defrSetPinExtWarnings(3);
    defrSetPinWarnings(3);
    defrSetRegionWarnings(3);
    defrSetRowWarnings(3);
    defrSetScanchainWarnings(3);
    defrSetSNetWarnings(3);
    defrSetStylesWarnings(3);
    defrSetTrackWarnings(3);
    defrSetUnitsWarnings(3);
    defrSetVersionWarnings(3);
    defrSetViaWarnings(3);

    (void) defrSetOpenLogFileAppend();

    defrInit();

    if( !( f = fopen( fileName.c_str(), "r" ) ) ){
        cerr << "[ERROR] Cannot open file : " << fileName << endl; exit(0); }
    else
        cout << " read DEF : " << fileName << " ..." << endl;

    int result = defrRead(f, fileName.c_str(), (void*)(intptr_t)userData, 0);
    if( result ) { cerr << "[ERROR] Reader return bad status on " << fileName; exit(0); }

    (void)defrPrintUnusedCallbacks(fout);
    (void)defrReleaseNResetMemory();
    fclose( f );

    // rows
    _cir->sortRows();
    _cir->remove_sites();

    // edge-type table
    for( unsigned i=0; i<Module::edgeTypeTableSize; ++i )
        for( unsigned j=0; j<Module::edgeTypeTableSize; ++j )
            Module::edgeTypeTable[i][j] *= unitDEF;

    // fence-region cells
    for( unsigned i=0; i<_cir->numModules(); ++i ){
        for( unsigned j=0; j<_cir->numFregions(); ++j ){
            _cir->fregion(j).check_and_add_module( _cir->module(i) );
        }
    }
    return;
}
