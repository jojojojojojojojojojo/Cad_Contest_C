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


#include <string.h>
#include <stdlib.h>
#include "lex.h"
#include "defiNet.hpp"
#include "defiPath.hpp"
#include "defiDebug.hpp"
#include "defiUtil.hpp"

#define	maxLimit   65536


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiSubnet
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiSubnet::defiSubnet() {
  this->defiSubnet::Init();
}


void defiSubnet::Init() {
  this->name_ = 0;
  this->defiSubnet::bumpName(16);

  this->instances_ = 0;
  this->pins_ = 0;
  this->musts_ = 0;
  this->synthesized_ = 0;
  this->numPins_ = 0;
  this->defiSubnet::bumpPins(16);

  // WMD -- this will be removed by the next release
  this->paths_ = 0;
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;

  this->numWires_ = 0;
  this->wiresAllocated_ = 0;
  this->wires_ = 0;
  this->nonDefaultRule_ = 0;

  this->defiSubnet::clear();
}


void defiSubnet::Destroy() {
  this->defiSubnet::clear();
  defFree(this->name_);
  defFree((char*)(this->instances_));
  defFree((char*)(this->pins_));
  defFree(this->musts_);
  defFree(this->synthesized_);

}


defiSubnet::~defiSubnet() {
  this->defiSubnet::Destroy();
}


void defiSubnet::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) this->defiSubnet::bumpName(len);
  strcpy(this->name_, DEFCASE(name));
}


void defiSubnet::setNonDefault(const char* name) {
  int len = strlen(name) + 1;
  this->nonDefaultRule_ = (char*)defMalloc(len);
  strcpy(this->nonDefaultRule_, DEFCASE(name));
}

 
void defiSubnet::addMustPin(const char* instance, const char* pin, int syn) {
  this->defiSubnet::addPin(instance, pin, syn);
  this->musts_[this->numPins_ - 1] = 1;
}


void defiSubnet::addPin(const char* instance, const char* pin, int syn) {
  int len;

  if (this->numPins_ == this->pinsAllocated_)
    this->defiSubnet::bumpPins(this->pinsAllocated_ * 2);

  len = strlen(instance)+ 1;
  this->instances_[this->numPins_] = (char*)defMalloc(len);
  strcpy(this->instances_[this->numPins_], DEFCASE(instance));

  len = strlen(pin)+ 1;
  this->pins_[this->numPins_] = (char*)defMalloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(pin));

  this->musts_[this->numPins_] = 0;
  this->synthesized_[this->numPins_] = syn;

  (this->numPins_)++;
}

// WMD -- this will be removed by the next release
void defiSubnet::setType(const char* typ) {
  if (*typ == 'F') {
    this->isFixed_ = 1;
  } else if (*typ == 'C') {
    this->isCover_ = 1;
  } else if (*typ == 'R') {
    this->isRouted_ = 1;
  } else {
    // Silently do nothing with bad input.
  }
 
}
 
// WMD -- this will be removed by the next release
void defiSubnet::addPath(defiPath* p, int reset, int netOsnet, int *needCbk) {
  int i;
  size_t incNumber;

  if (reset) {
     for (i = 0; i < this->numPaths_; i++) {
        this->paths_[i]->defiPath::Destroy();
        defFree((char*)(this->paths_[i]));
     }  
     this->numPaths_ = 0;
  } 
  if (this->numPaths_ >= this->pathsAllocated_) {
    // 6/17/2003 - don't want to allocate too large memory just in case
    // a net has many wires with only 1 or 2 paths
    if (this->pathsAllocated_ <= maxLimit) {
        incNumber = this->pathsAllocated_*2;
        if (incNumber > maxLimit) {
            incNumber = this->pathsAllocated_ + maxLimit;
        }
    } else {
        incNumber = this->pathsAllocated_ + maxLimit;
    }

    switch (netOsnet) {
      case 2: 
         this->defiSubnet::bumpPaths(
            this->pathsAllocated_ ? incNumber : 1000);
         break;
      default:
         this->defiSubnet::bumpPaths(
            this->pathsAllocated_ ? incNumber : 8);
         break;
     }
  }
  this->paths_[this->numPaths_++] = p;
  if (this->numPaths_ == this->pathsAllocated_)
    *needCbk = 1;   // pre-warn the parser it needs to realloc next time
}


void defiSubnet::addWire(const char* type) {
  defiWire* wire;
  if (this->numWires_ == this->wiresAllocated_) {
    defiWire** array;
    int i;
    this->wiresAllocated_ = this->wiresAllocated_ ?
                            this->wiresAllocated_ * 2 : 2 ;
    array = (defiWire**)defMalloc(sizeof(defiWire*)*this->wiresAllocated_);
    for (i = 0; i < this->numWires_; i++) {
      array[i] = this->wires_[i];
    }
    if (this->wires_) 
       defFree((char*)(this->wires_));
    this->wires_ = array;
  }
  wire = this->wires_[this->numWires_] =
           (defiWire*)defMalloc(sizeof(defiWire));
  this->numWires_ += 1;
  wire->defiWire::Init(type, NULL);
}


void defiSubnet::addWirePath(defiPath* p, int reset, int netOsnet, int *needCbk) {
  if (this->numWires_ > 0)
     this->wires_[this->numWires_-1]->defiWire::addPath(p, reset, netOsnet,
                                                        needCbk);
  else
     // Something screw up, can't be both be zero.
     defiError(0, 6080, "ERROR (DEFPARS-6080): An internal error has occurred. The index number for the SUBNET wires array is less then or equal to 0.\nContact Cadence Customer Support with this error information.");
}

const char* defiSubnet::name() const {
  return this->name_;
}


int defiSubnet::hasNonDefaultRule() const {
  return this->nonDefaultRule_ ? 1 : 0;
}


const char* defiSubnet::nonDefaultRule() const {
  return this->nonDefaultRule_;
}


int defiSubnet::numConnections() {
  return this->numPins_;
}


const char* defiSubnet::instance(int index) {
  if (index >= 0 && index < this->numPins_)
    return this->instances_[index];
  return 0;
}


const char* defiSubnet::pin(int index) {
  if (index >= 0 && index < this->numPins_)
    return this->pins_[index];
  return 0;
}


int defiSubnet::pinIsMustJoin(int index) {
  if (index >= 0 && index < this->numPins_)
    return (int)(this->musts_[index]);
  return 0;
}


int defiSubnet::pinIsSynthesized(int index) {
  if (index >= 0 && index < this->numPins_)
    return (int)(this->synthesized_[index]);
  return 0;
}

// WMD -- this will be removed by the next release
int defiSubnet::isFixed() const {
  return (int)(this->isFixed_);
}
 
 
// WMD -- this will be removed by the next release
int defiSubnet::isRouted() const {
  return (int)(this->isRouted_);
}
 
 
// WMD -- this will be removed by the next release
int defiSubnet::isCover() const {
  return (int)(this->isCover_);
}


void defiSubnet::bumpName(int size) {
  if (this->name_) defFree(this->name_);
  this->name_ = (char*)defMalloc(size);
  this->nameSize_ = size;
  this->name_[0] = '\0';
}


void defiSubnet::bumpPins(int size) {
  char** newInstances = (char**)defMalloc(sizeof(char*)*size);
  char** newPins = (char**)defMalloc(sizeof(char*)*size);
  char* newMusts = (char*)defMalloc(size);
  char* newSyn = (char*)defMalloc(size);
  int i;

  if (this->instances_) {
    for (i = 0; i < this->pinsAllocated_; i++) {
      newInstances[i] = this->instances_[i];
      newPins[i] = this->pins_[i];
      newMusts[i] = this->musts_[i];
      newSyn[i] = this->synthesized_[i];
    }
    defFree((char*)(this->instances_));
    defFree((char*)(this->pins_));
    defFree(this->musts_);
    defFree(this->synthesized_);
  }

  this->instances_ = newInstances;
  this->pins_ = newPins;
  this->musts_ = newMusts;
  this->synthesized_ = newSyn;
  this->pinsAllocated_ = size;
}


void defiSubnet::clear() {
  int i;

  // WMD -- this will be removed by the next release
  this->isFixed_ = 0;
  this->isRouted_ = 0;
  this->isCover_ = 0;
  this->name_[0] = '\0';

  for (i = 0; i < this->numPins_; i++) {
    defFree(this->instances_[i]);
    defFree(this->pins_[i]);
    this->instances_[i] = 0;
    this->pins_[i] = 0;
    this->musts_[i] = 0;
    this->synthesized_[i] = 0;
  }
  this->numPins_ = 0;

  // WMD -- this will be removed by the next release
  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      this->paths_[i]->defiPath::Destroy();
      defFree((char*)(this->paths_[i]));
    }
    defFree((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }

  if (this->nonDefaultRule_) {
    defFree(this->nonDefaultRule_);
    this->nonDefaultRule_ = 0;
  }

  if (this->numWires_) {
    for (i = 0; i < this->numWires_; i++) {
      this->wires_[i]->defiWire::Destroy();
      defFree((char*)(this->wires_[i]));
      this->wires_[i] = 0;
    }
    defFree((char*)(this->wires_));
    this->wires_ = 0;
    this->numWires_ = 0;
    this->wiresAllocated_ = 0;
  }
}


void defiSubnet::print(FILE* f) {
  int i, j;
  defiPath* p;
  defiWire* wire;

  fprintf(f, " subnet '%s'", this->name_);
  fprintf(f, "\n");

  if (this->defiSubnet::hasNonDefaultRule())
    fprintf(f, "  nondefault rule %s\n",
    this->defiSubnet::nonDefaultRule());

  if (this->defiSubnet::numConnections()) {
    fprintf(f, "  Pins:\n");
    for (i = 0; i < this->defiSubnet::numConnections(); i++) {
    fprintf(f, "   '%s' '%s'%s%s\n", 
      this->defiSubnet::instance(i),
      this->defiSubnet::pin(i),
      this->defiSubnet::pinIsMustJoin(i) ? " MUSTJOIN" : "",
      this->defiSubnet::pinIsSynthesized(i) ? " SYNTHESIZED" : "");
    }
  }

  if (this->defiSubnet::numWires()) {
    fprintf(f, "  Paths:\n");
    for (i = 0; i < this->defiSubnet::numWires(); i++) {
      wire = this->defiSubnet::wire(i);
      for (j = 0; j < wire->defiWire::numPaths(); j++) {
         p = wire->defiWire::path(j);
         p->defiPath::print(f);
      }
    }
  }
}

int defiSubnet::numWires() const {
  return this->numWires_;
}
 
 
defiWire* defiSubnet::wire(int index) {
  if (index >= 0 && index < this->numWires_)
    return this->wires_[index];
  return 0;
}

// WMD -- this will be removed after the next release
defiPath* defiSubnet::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}
 
 
// WMD -- this will be removed after the next release
int defiSubnet::numPaths() const {
  return this->numPaths_;
}
 
// WMD -- this will be removed after the next release
void defiSubnet::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)defMalloc(sizeof(defiPath*)*size);
 
  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];
 
  this->pathsAllocated_ = size;
  defFree((char*)(this->paths_));
  this->paths_ = newPaths;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiVpin
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiVpin::defiVpin() {
}


void defiVpin::Init(const char* name) {
  int len = strlen(name) + 1;
  this->name_ = (char*)defMalloc(len);
  strcpy(this->name_, DEFCASE(name));
  this->orient_ = -1;
  this->status_ = ' ';
  this->layer_ = 0;
}


defiVpin::~defiVpin() {
}


void defiVpin::Destroy() {
  defFree(this->name_);
  if (this->layer_) defFree(this->layer_);
}


void defiVpin::setBounds(int xl, int yl, int xh, int yh) {
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}


void defiVpin::setLayer(const char* lay) {
  int len = strlen(lay)+1;
  this->layer_ = (char*)defMalloc(len);
  strcpy(this->layer_, lay);
}


void defiVpin::setOrient(int orient) {
  this->orient_ = orient;
}


void defiVpin::setLoc(int x, int y) {
  this->xLoc_ = x;
  this->yLoc_ = y;
}


void defiVpin::setStatus(char st) {
  this->status_ = st;
}


int defiVpin::xl() const  {
  return this->xl_;
}


int defiVpin::yl() const  {
  return this->yl_;
}


int defiVpin::xh() const  {
  return this->xh_;
}


int defiVpin::yh() const  {
  return this->yh_;
}


char defiVpin::status() const {
  return this->status_;
}


int defiVpin::orient() const  {
  return this->orient_;
}


const char* defiVpin::orientStr() const  {
  return (defiOrientStr(this->orient_));
}


int defiVpin::xLoc() const {
  return this->xLoc_;
}


int defiVpin::yLoc() const {
  return this->yLoc_;
}


const char* defiVpin::name() const {
  return this->name_;
}


const char* defiVpin::layer() const {
  return this->layer_;
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiShield
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiShield::defiShield() {
}


void defiShield::Init(const char* name) {
  int len = strlen(name) + 1;
  this->name_ = (char*)defMalloc(len);
  strcpy(this->name_, DEFCASE(name));
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;
  this->paths_ = 0;
}


void defiShield::Destroy() {
  this->defiShield::clear();
}


defiShield::~defiShield() {
  this->defiShield::Destroy();
}


void defiShield::addPath(defiPath* p, int reset, int netOsnet, int *needCbk) {
  int i;
  size_t incNumber;

  if (reset) {
     for (i = 0; i < this->numPaths_; i++) {
        this->paths_[i]->defiPath::Destroy();
        defFree((char*)(this->paths_[i]));
     }
     this->numPaths_ = 0;
  }
  if (this->numPaths_ >= this->pathsAllocated_) {
    // 6/17/2003 - don't want to allocate too large memory just in case
    // a net has many wires with only 1 or 2 paths

    if (this->pathsAllocated_ <= maxLimit) {
        incNumber = this->pathsAllocated_*2;
        if (incNumber > maxLimit) {
            incNumber = this->pathsAllocated_ + maxLimit;
        }
    } else {
        incNumber = this->pathsAllocated_ + maxLimit;
    }

    switch (netOsnet) {
      case 2:
        this->defiShield::bumpPaths(
            this->pathsAllocated_ ? incNumber : 1000);
        break;
      default:
        this->defiShield::bumpPaths(
            this->pathsAllocated_ ? incNumber : 8);
        break;
    }
  }
  this->paths_[this->numPaths_++] = p;
  if (this->numPaths_ == this->pathsAllocated_)
    *needCbk = 1;   // pre-warn the parser it needs to realloc next time
}


void defiShield::clear() {
  defiPath* p;
  int       i;

  if (this->name_) defFree(this->name_);
  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      p = this->paths_[i];
      p->defiPath::Destroy();
      defFree((char*)p);
    }
    defFree((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }
}


void defiShield::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)defMalloc(sizeof(defiPath)*size);

  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];

  this->pathsAllocated_ = size;
  defFree((char*)(this->paths_));
  this->paths_ = newPaths;
}


int defiShield::numPaths() const {
  return this->numPaths_;
}


const char* defiShield::shieldName() const {
  return this->name_;
}

defiPath* defiShield::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiWire
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiWire::defiWire() {
}


void defiWire::Init(const char* type, const char* wireShieldName) {
  int len = strlen(type) + 1;
  this->type_ = (char*)defMalloc(len);
  strcpy(this->type_, DEFCASE(type));
  if (wireShieldName) {
    this->wireShieldName_ = (char*)defMalloc(strlen(wireShieldName)+1);
    strcpy(this->wireShieldName_, wireShieldName);
  } else
    this->wireShieldName_ = 0; 
  this->numPaths_ = 0;
  this->pathsAllocated_ = 0;
  this->paths_ = 0;
}


void defiWire::Destroy() {
  this->defiWire::clear();
}


defiWire::~defiWire() {
  this->defiWire::Destroy();
}


void defiWire::addPath(defiPath* p, int reset, int netOsnet, int *needCbk) {
  int i;
  size_t incNumber;

  if (reset) {
     for (i = 0; i < this->numPaths_; i++) {
        this->paths_[i]->defiPath::Destroy();
        defFree((char*)(this->paths_[i]));
     }
     this->numPaths_ = 0;
  }
  if (this->numPaths_ >= this->pathsAllocated_) {
    // 6/17/2003 - don't want to allocate too large memory just in case
    // a net has many wires with only 1 or 2 paths

    if (this->pathsAllocated_ <= maxLimit) {
        incNumber = this->pathsAllocated_*2;
        if (incNumber > maxLimit) {
            incNumber = this->pathsAllocated_ + maxLimit;
        }
    } else {
        incNumber = this->pathsAllocated_ + maxLimit;
    }

    switch (netOsnet) {
      case 2:
        this->defiWire::bumpPaths(
          this->pathsAllocated_  ? incNumber : 1000);
        break;
      default:
        this->defiWire::bumpPaths(
          this->pathsAllocated_ ? incNumber : 8);
        break;
    }
  }
  this->paths_[this->numPaths_++] = p;
  if (this->numPaths_ == this->pathsAllocated_)
    *needCbk = 1;   // pre-warn the parser it needs to realloc next time
}


void defiWire::clear() {
  defiPath* p;
  int       i;

  if (this->type_) defFree(this->type_);
  if (this->wireShieldName_) defFree(this->wireShieldName_);
  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      p = this->paths_[i];
      p->defiPath::Destroy();
      defFree((char*)p);
    }
    defFree((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }
}


void defiWire::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)defMalloc(sizeof(defiPath)*size);

  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];

  this->pathsAllocated_ = size;
  defFree((char*)(this->paths_));
  this->paths_ = newPaths;
}


int defiWire::numPaths() const {
  return this->numPaths_;
}


const char* defiWire::wireType() const {
  return this->type_;
}

const char* defiWire::wireShieldNetName() const {
  return this->wireShieldName_;
}

defiPath* defiWire::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiNet
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


defiNet::defiNet() {
  this->defiNet::Init();
}


void defiNet::Init() {
  this->name_ = 0;
  this->instances_ = 0;
  this->numPins_ = 0;
  this->numProps_ = 0;
  this->propNames_ = 0;
  this->subnets_ = 0;
  this->source_ = 0;
  this->pattern_ = 0;
  this->style_ = 0;
  this->shieldNet_ = 0;
  this->original_ = 0;
  this->use_ = 0;
  this->nonDefaultRule_ = 0;
  this->numWires_ = 0;
  this->wiresAllocated_ = 0;
  this->wires_= 0;

  this->numWidths_ = 0;
  this->widthsAllocated_ = 0;
  this->wlayers_ = 0;
  this->wdist_ = 0;

  this->numSpacing_ = 0;
  this->spacingAllocated_ = 0;
  this->slayers_ = 0;
  this->sdist_ = 0;
  this->sleft_ = 0;
  this->sright_ = 0;

  this->vpins_ = 0;
  this->numVpins_ = 0;
  this->vpinsAllocated_ = 0;

  this->shields_ = 0;
  this->numShields_ = 0;
  this->numNoShields_ = 0;
  this->shieldsAllocated_ = 0;
  this->numShieldNet_ = 0;
  this->shieldNetsAllocated_ = 0;

  this->defiNet::bumpProps(2);
  this->defiNet::bumpName(16);
  this->defiNet::bumpPins(16);
  this->defiNet::bumpSubnets(2);

  this->rectNames_ = 0;
  this->rectRouteStatus_ = 0;
  this->rectRouteStatusShieldNames_=0;
  this->rectShapeTypes_ = 0;
  this->rectMasks_ = 0;
  this->polygonNames_ = 0;
  this->polyRouteStatus_ = 0;
  this->polyShapeTypes_ = 0;
  this->polyRouteStatusShieldNames_ = 0;
  this->numPolys_ = 0;
  this->polysAllocated_ = 0;
  this->polygons_ = 0;
  this->polyMasks_ = 0;

  this->numSubnets_ = 0;
  this->paths_ = 0;
  this->numPaths_ = 0;

  this->numPts_ = 0;
  this->viaNames_ = 0;
  this->viaPts_ = 0;
  this->ptsAllocated_=0;
  this->viaMasks_ = 0;
  this->viaOrients_ = 0;
  this->viaRouteStatus_ = 0;
  this->viaShapeTypes_ = 0;
  this->viaRouteStatusShieldNames_ = 0;

  this->defiNet::clear();
}


void defiNet::Destroy() {
  this->defiNet::clear();
  defFree(this->name_);
  defFree((char*)(this->instances_));
  defFree((char*)(this->pins_));
  defFree(this->musts_);
  defFree(this->synthesized_);
  defFree((char*)(this->propNames_));
  defFree((char*)(this->propValues_));
  defFree((char*)(this->propDValues_));
  defFree((char*)(this->propTypes_));
  defFree((char*)(this->subnets_));
  if (this->source_) defFree(this->source_);
  if (this->pattern_) defFree(this->pattern_);
  if (this->shieldNet_) defFree(this->shieldNet_);
  if (this->original_) defFree(this->original_);
  if (this->use_) defFree(this->use_);
  if (this->nonDefaultRule_) defFree(this->nonDefaultRule_);
  if (this->wlayers_) defFree((char*)(this->wlayers_));
  if (this->slayers_) defFree((char*)(this->slayers_));
  if (this->sdist_) defFree((char*)(this->sdist_));
  if (this->wdist_) defFree((char*)(this->wdist_));
  if (this->sleft_) defFree((char*)(this->sleft_));
  if (this->sright_) defFree((char*)(this->sright_));
}


defiNet::~defiNet() {
  this->defiNet::Destroy();
}


void defiNet::setName(const char* name) {
  int len = strlen(name) + 1;
  this->defiNet::clear();
  if (len > this->nameSize_) this->defiNet::bumpName(len);
  strcpy(this->name_, DEFCASE(name));
}


void defiNet::addMustPin(const char* instance, const char* pin, int syn) {
  this->defiNet::clear();
  this->defiNet::addPin(instance, pin, syn);
  this->musts_[this->numPins_ - 1] = 1;
}


void defiNet::addPin(const char* instance, const char* pin, int syn) {
  int len;

  if (this->numPins_ == this->pinsAllocated_)
    this->defiNet::bumpPins(this->pinsAllocated_ * 2);

  len = strlen(instance)+ 1;
  this->instances_[this->numPins_] = (char*)defMalloc(len);
  strcpy(this->instances_[this->numPins_], DEFCASE(instance));

  len = strlen(pin)+ 1;
  this->pins_[this->numPins_] = (char*)defMalloc(len);
  strcpy(this->pins_[this->numPins_], DEFCASE(pin));

  this->musts_[this->numPins_] = 0;
  this->synthesized_[this->numPins_] = syn;

  (this->numPins_)++;
}


void defiNet::setWeight(int w) {
  this->hasWeight_ = 1;
  this->weight_ = w;
}


void defiNet::addProp(const char* name, const char* value, const char type) {
  int len;

  if (this->numProps_ == this->propsAllocated_)
    this->defiNet::bumpProps(this->propsAllocated_ * 2);

  len = strlen(name)+ 1;
  this->propNames_[this->numProps_] = (char*)defMalloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));

  len = strlen(value)+ 1;
  this->propValues_[this->numProps_] = (char*)defMalloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));

  this->propDValues_[this->numProps_] = 0;
  this->propTypes_[this->numProps_] = type;

  (this->numProps_)++;
}


void defiNet::addNumProp(const char* name, const double d,
                         const char* value, const char type) {
  int len;

  if (this->numProps_ == this->propsAllocated_)
    this->defiNet::bumpProps(this->propsAllocated_ * 2);

  len = strlen(name)+ 1;
  this->propNames_[this->numProps_] = (char*)defMalloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));

  len = strlen(value)+ 1;
  this->propValues_[this->numProps_] = (char*)defMalloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));

  this->propDValues_[this->numProps_] = d;
  this->propTypes_[this->numProps_] = type;

  (this->numProps_)++;
}


void defiNet::addSubnet(defiSubnet* subnet) {

  if (this->numSubnets_ >= this->subnetsAllocated_)
    this->defiNet::bumpSubnets(this->subnetsAllocated_ * 2);

  this->subnets_[this->numSubnets_++] = subnet;
}

// WMD -- will be removed after the next release
void defiNet::setType(const char* typ) {
  if (*typ == 'F') {
    this->isFixed_ = 1;
  } else if (*typ == 'C') {
    this->isCover_ = 1;
  } else if (*typ == 'R') {
    this->isRouted_ = 1;
  } else {
    // Silently do nothing with bad input.
  }
}

void defiNet::addWire(const char* type, const char* wireShieldName) {
  defiWire* wire;
  if (this->numWires_ == this->wiresAllocated_) {
    defiWire** array;
    int i;
    this->wiresAllocated_ = this->wiresAllocated_ ?
	                    this->wiresAllocated_ * 2 : 2 ;
    array = (defiWire**)defMalloc(sizeof(defiWire*)*this->wiresAllocated_);
    for (i = 0; i < this->numWires_; i++)
      array[i] = this->wires_[i];
    if (this->wires_) defFree((char*)(this->wires_));
    this->wires_ = array;
  }
  wire = this->wires_[this->numWires_] = 
           (defiWire*)defMalloc(sizeof(defiWire));
  this->numWires_ += 1;
  wire->defiWire::Init(type, wireShieldName);
}


void defiNet::addWirePath(defiPath* p, int reset, int netOsnet, int *needCbk) {
  if (this->numWires_ > 0)
     this->wires_[this->numWires_-1]->defiWire::addPath(p, reset, netOsnet,
                                                        needCbk);
  else
     // Something screw up, can't be both be zero.
     defiError(0, 6081, "ERROR (DEFPARS-6081): An internal error has occurred. The index number for the NET PATH wires array is less then or equal to 0.\nContact Cadence Customer Support with this error information.");
}


void defiNet::addShield(const char* name) {
  defiShield* shield;
  if (this->numShields_ == this->shieldsAllocated_) {
    defiShield** array;
    int i;
    this->shieldsAllocated_ = this->shieldsAllocated_ ?
	                      this->shieldsAllocated_ * 2 : 2 ;
    array = (defiShield**)defMalloc(sizeof(defiShield*)*this->shieldsAllocated_);
    for (i = 0; i < this->numShields_; i++)
      array[i] = this->shields_[i];
    if (this->shields_) defFree((char*)(this->shields_));
    this->shields_ = array;
  }
  shield = this->shields_[this->numShields_] = 
           (defiShield*)defMalloc(sizeof(defiShield));
  this->numShields_ += 1;
  shield->defiShield::Init(name);
}


void defiNet::addShieldPath(defiPath* p, int reset, int netOsnet, int *needCbk) {
  // Since shield and noshield share the list shields_, the
  // only way to tell whether the list is currently contained
  // data for shields_ or noshields_ is from the variables
  // numShields_ and numNoShields_.
  // Since shield and noshield are mutual exclusive, only one
  // this->numShields_ or this->numNoShields will be non-zero
  // in this method.  Whichever is non-zero will be the current
  // working list
  if (this->numShields_ > 0)
     this->shields_[this->numShields_-1]->defiShield::addPath(p, reset,
              netOsnet, needCbk);
  else if (this->numNoShields_ > 0)
     this->shields_[this->numNoShields_-1]->defiShield::addPath(p, reset,
              netOsnet, needCbk);
  else
     // Something screw up, can't be both be zero.
     defiError(0, 6082, "ERROR (DEFPARS-6082): An internal error has occurred. The index number for the NET SHIELDPATH wires array is less then or equal to 0.\nContact Cadence Customer Support with this error information.");
}


void defiNet::addNoShield(const char* name) {
  defiShield* shield;
  if (this->numNoShields_ == this->shieldsAllocated_) {
    defiShield** array;
    int i;
    this->shieldsAllocated_ = this->shieldsAllocated_ ?
	                      this->shieldsAllocated_ * 2 : 2 ;
    array = (defiShield**)defMalloc(sizeof(defiShield*)*this->shieldsAllocated_);
    for (i = 0; i < this->numNoShields_; i++)
      array[i] = this->shields_[i];
    if (this->shields_) defFree((char*)(this->shields_));
    this->shields_ = array;
  }
  shield = this->shields_[this->numNoShields_] = 
           (defiShield*)defMalloc(sizeof(defiShield));
  this->numNoShields_ += 1;
  shield->defiShield::Init(name);
}


void defiNet::addShieldNet(const char* name) {
  int len;

  if (this->numShieldNet_ == this->shieldNetsAllocated_) {
     if (this->shieldNetsAllocated_ == 0)
        this->defiNet::bumpShieldNets(2);
     else
        this->defiNet::bumpShieldNets(this->shieldNetsAllocated_ * 2);

  }
 
  len = strlen(name) + 1;
  this->shieldNet_[this->numShieldNet_] = (char*)defMalloc(len);
  strcpy(this->shieldNet_[this->numShieldNet_], DEFCASE(name));
  (this->numShieldNet_)++;
}


void defiNet::changeNetName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) this->defiNet::bumpName(len);
  strcpy(this->name_, DEFCASE(name));
}

void defiNet::changeInstance(const char* instance, int index) {
  int len;
  char errMsg[128];

  if ((index < 0) || (index > this->numPins_)) {
     sprintf (errMsg, "ERROR (DEFPARS-6083): The index number %d specified for the NET INSTANCE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numPins_);
     defiError(0, 6083, errMsg);
  }

  len = strlen(instance)+ 1;
  if (this->instances_[index])
    defFree((char*)(this->instances_[index]));
  this->instances_[index] = (char*)defMalloc(len);
  strcpy(this->instances_[index], DEFCASE(instance));
  return;
}

void defiNet::changePin(const char* pin, int index) {
  int len;
  char errMsg[128];

  if ((index < 0) || (index > this->numPins_)) {
     sprintf (errMsg, "ERROR (DEFPARS-6084): The index number %d specified for the NET PIN is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numPins_);
     defiError(0, 6084, errMsg);
  }

  len = strlen(pin)+ 1;
  if (this->pins_[index])
    defFree((char*)(this->pins_[index]));
  this->pins_[index] = (char*)defMalloc(len);
  strcpy(this->pins_[index], DEFCASE(pin));
  return;
}

const char* defiNet::name() const {
  return this->name_;
}


int defiNet::weight() const {
  return this->weight_;
}


int defiNet::numProps() const {
  return this->numProps_;
}


int defiNet::hasProps() const {
  return this->numProps_ ? 1 : 0 ;
}


int defiNet::hasWeight() const {
  return (int)(this->hasWeight_);
}


const char* defiNet::propName(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propNames_[index];
  return 0;
}


const char* defiNet::propValue(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propValues_[index];
  return 0;
}


double defiNet::propNumber(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propDValues_[index];
  return 0;
}


const char defiNet::propType(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propTypes_[index];
  return 0;
}


int defiNet::propIsNumber(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propDValues_[index] ? 1 : 0;
  return 0; 
}


int defiNet::propIsString(int index) const {
  if (index >= 0 &&  index < this->numProps_)
    return this->propDValues_[index] ? 0 : 1;
  return 0; 
}


int defiNet::numConnections() const {
  return this->numPins_;
}


int defiNet::numShieldNets() const {
  return this->numShieldNet_;
}


const char* defiNet::instance(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return this->instances_[index];
  return 0;
}


const char* defiNet::pin(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return this->pins_[index];
  return 0;
}


int defiNet::pinIsMustJoin(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return (int)(this->musts_[index]);
  return 0;
}


int defiNet::pinIsSynthesized(int index) const {
  if (index >= 0 &&  index < this->numPins_)
    return (int)(this->synthesized_[index]);
  return 0;
}


int defiNet::hasSubnets() const {
  return this->numSubnets_ ? 1 : 0 ;
}


int defiNet::numSubnets() const {
  return this->numSubnets_;
}


defiSubnet* defiNet::subnet(int index) {
  if (index >= 0 &&  index < this->numSubnets_)
    return this->subnets_[index];
  return 0;
}


int defiNet::isFixed() const {
  return (int)(this->isFixed_);
}
 
 
int defiNet::isRouted() const {
  return (int)(this->isRouted_);
}
 
 
int defiNet::isCover() const {
  return (int)(this->isCover_);
}
 

// this method will only call if the callback defrSNetWireCbk is set
// which will callback every wire.  Therefore, only one wire should be here
void defiNet::freeWire() {
  int i;

  if (this->numWires_) {
    for (i = 0; i < this->numWires_; i++) {
      this->wires_[i]->defiWire::Destroy();
      defFree((char*)(this->wires_[i]));
      this->wires_[i] = 0;
    }
    this->numWires_ = 0;
    this->wiresAllocated_ = 0;
  }

  clearRectPoly();
  clearVia();
}


void defiNet::freeShield() {
  int i;

  if (this->numShields_) {
    for (i = 0; i < this->numShields_; i++) {
      this->shields_[i]->defiShield::Destroy();
      defFree((char*)(this->shields_[i]));
      this->shields_[i] = 0;
    }
    this->numShields_ = 0;
    this->shieldsAllocated_ = 0;
  }
}


void defiNet::print(FILE* f) {
  int i, j, x, y, newLayer;
  int numX, numY, stepX, stepY;
  defiPath* p;
  defiSubnet* s;
  defiVpin* vp;
  defiWire* wire;
  int path;

  fprintf(f, "Net '%s'", this->name_);
  fprintf(f, "\n");

  if (this->defiNet::hasWeight())
    fprintf(f, "  weight=%d\n", this->defiNet::weight());

  if (this->defiNet::hasFixedbump())
    fprintf(f, "  fixedbump\n");

  if (this->defiNet::hasFrequency())
    fprintf(f, "  frequency=%f\n", this->defiNet::frequency());

  if (this->defiNet::hasCap())
    fprintf(f, "  cap=%f\n", this->defiNet::cap());

  if (this->defiNet::hasSource())
    fprintf(f, "  source='%s'\n", this->defiNet::source());

  if (this->defiNet::hasPattern())
    fprintf(f, "  pattern='%s'\n", this->defiNet::pattern());

  if (this->defiNet::hasOriginal())
    fprintf(f, "  original='%s'\n", this->defiNet::original());

  if (this->defiNet::hasUse())
    fprintf(f, "  use='%s'\n", this->defiNet::use());

  if (this->defiNet::hasNonDefaultRule())
    fprintf(f, "  nonDefaultRule='%s'\n", this->defiNet::nonDefaultRule());

  if (this->defiNet::hasXTalk())
    fprintf(f, "  xtalk=%d\n", this->defiNet::XTalk());

  if (this->defiNet::hasStyle())
    fprintf(f, "  style='%d'\n", this->defiNet::style());

  if (this->defiNet::hasProps()) {
    fprintf(f, " Props:\n");
    for (i = 0; i < this->defiNet::numProps(); i++) {
      fprintf(f, "  '%s' '%s'\n", this->defiNet::propName(i),
      this->defiNet::propValue(i));
    }
  }

  if (this->defiNet::numConnections()) {
    fprintf(f, " Pins:\n");
    for (i = 0; i < this->defiNet::numConnections(); i++) {
    fprintf(f, "  '%s' '%s'%s%s\n", 
      this->defiNet::instance(i),
      this->defiNet::pin(i),
      this->defiNet::pinIsMustJoin(i) ? " MUSTJOIN" : "",
      this->defiNet::pinIsSynthesized(i) ? " SYNTHESIZED" : "");
    }
  }
 
  for (i = 0; i < this->numVpins_; i++) {
    vp = this->defiNet::vpin(i);
    fprintf(f,
    "  VPIN %s status '%c' layer %s %d,%d orient %s bounds %d,%d to %d,%d\n",
    vp->defiVpin::name(),
    vp->defiVpin::status(),
    vp->defiVpin::layer() ? vp->defiVpin::layer() : "",
    vp->defiVpin::xLoc(),
    vp->defiVpin::yLoc(),
    vp->defiVpin::orientStr(),
    vp->defiVpin::xl(),
    vp->defiVpin::yl(),
    vp->defiVpin::xh(),
    vp->defiVpin::yh());
  }

  for (i = 0; i < this->numWires_; i++) {
    newLayer = 0;
    wire = this->defiNet::wire(i);
            fprintf(f, "+ %s ", wire->defiWire::wireType());
    for (j = 0; j < wire->defiWire::numPaths(); j++) {
      p = wire->defiWire::path(j);
      p->defiPath::initTraverse();
      while ((path = (int)(p->defiPath::next())) != DEFIPATH_DONE) {
         switch (path) {
           case DEFIPATH_LAYER:
                if (newLayer == 0) {
                    fprintf(f, "%s ", p->defiPath::getLayer());
                    newLayer = 1;
                } else
                    fprintf(f, "NEW %s ", p->defiPath::getLayer());
                break;
           case DEFIPATH_VIA:
                fprintf(f, "%s\n", p->defiPath::getVia());
                break;
           case DEFIPATH_VIAROTATION:
                fprintf(f, "%d\n", p->defiPath::getViaRotation());
                break;
           case DEFIPATH_VIADATA:
                p->defiPath::getViaData(&numX, &numY, &stepX, &stepY);
                fprintf(f, "%d %d %d %d\n", numX, numY, stepX, stepY);
                break;
           case DEFIPATH_WIDTH:
                fprintf(f, "%d\n", p->defiPath::getWidth());
                break;
           case DEFIPATH_POINT:
                p->defiPath::getPoint(&x, &y);
                fprintf(f, "( %d %d )\n", x, y);
                break;
           case DEFIPATH_TAPER:
                fprintf(f, "TAPER\n");
                break;
         }
      }
    }
  }

  if (this->defiNet::hasSubnets()) {
    fprintf(f, " Subnets:\n");
    for (i = 0; i < this->defiNet::numSubnets(); i++) {
      s = this->defiNet::subnet(i);
      s->defiSubnet::print(f);
    }
  }

}


void defiNet::bumpName(int size) {
  if (this->name_) defFree(this->name_);
  this->name_ = (char*)defMalloc(size);
  this->nameSize_ = size;
  this->name_[0] = '\0';
}


void defiNet::bumpPins(int size) {
  char** newInstances = (char**)defMalloc(sizeof(char*)*size);
  char** newPins = (char**)defMalloc(sizeof(char*)*size);
  char* newMusts = (char*)defMalloc(size);
  char* newSyn = (char*)defMalloc(size);
  int i;

  if (this->instances_) {
    for (i = 0; i < this->pinsAllocated_; i++) {
      newInstances[i] = this->instances_[i];
      newPins[i] = this->pins_[i];
      newMusts[i] = this->musts_[i];
      newSyn[i] = this->synthesized_[i];
    }
    defFree((char*)(this->instances_));
    defFree((char*)(this->pins_));
    defFree(this->musts_);
    defFree(this->synthesized_);
  }

  this->instances_ = newInstances;
  this->pins_ = newPins;
  this->musts_ = newMusts;
  this->synthesized_ = newSyn;
  this->pinsAllocated_ = size;
}


void defiNet::bumpProps(int size) {
  char**  newNames = (char**)defMalloc(sizeof(char*)*size);
  char**  newValues = (char**)defMalloc(sizeof(char*)*size);
  double* newDValues = (double*)defMalloc(sizeof(double)*size);
  char*   newTypes = (char*)defMalloc(sizeof(char)*size);
  int i;

  if (this->propNames_) {
    for (i = 0; i < this->numProps_; i++) {
      newNames[i] = this->propNames_[i];
      newValues[i] = this->propValues_[i];
      newDValues[i] = this->propDValues_[i];
      newTypes[i] = this->propTypes_[i];
    }
    defFree((char*)(this->propNames_));
    defFree((char*)(this->propValues_));
    defFree((char*)(this->propDValues_));
    defFree((char*)(this->propTypes_));
  }

  this->propNames_ = newNames;
  this->propValues_ = newValues;
  this->propDValues_ = newDValues;
  this->propTypes_ = newTypes;
  this->propsAllocated_ = size;
}


void defiNet::bumpSubnets(int size) {
  defiSubnet** newSubnets = (defiSubnet**)defMalloc(sizeof(defiSubnet*)*size);
  int i;
  if (this->subnets_) {
    for (i = 0; i < this->numSubnets_; i++) {
      newSubnets[i] = this->subnets_[i];
    }
    defFree((char*)(this->subnets_));
  }

  this->subnets_ = newSubnets;
  this->subnetsAllocated_ = size;
}


void defiNet::clear() {
  int i;
  defiSubnet* s;
  defiPath* p;
  defiVpin* vp;

  // WMD -- this will be removed by the next release
  this->isFixed_ = 0;
  this->isRouted_ = 0;
  this->isCover_ = 0;

  this->hasWeight_ = 0;
  this->hasCap_ = 0;
  this->hasFrequency_ = 0;
  this->hasVoltage_ = 0;
  this->xTalk_ = -1;

  if (this->vpins_) {
    for (i = 0; i < this->numVpins_; i++) {
      vp = this->vpins_[i];
      vp->defiVpin::Destroy();
      defFree((char*)vp);
    }
    defFree((char*)this->vpins_);
    this->vpins_  = 0;
    this->numVpins_ = 0;
    this->vpinsAllocated_ = 0;
  }

  for (i = 0; i < this->numProps_; i++) {
    defFree(this->propNames_[i]);
    defFree(this->propValues_[i]);
    this->propNames_[i] = 0;
    this->propValues_[i] = 0;
    this->propDValues_[i] = 0;
  }
  this->numProps_ = 0;

  for (i = 0; i < this->numPins_; i++) {
    defFree(this->instances_[i]);
    defFree(this->pins_[i]);
    this->instances_[i] = 0;
    this->pins_[i] = 0;
    this->musts_[i] = 0;
    this->synthesized_[i] = 0;
  }
  this->numPins_ = 0;

  for (i = 0; i < this->numSubnets_; i++) {
    s = this->subnets_[i];
    s->defiSubnet::Destroy();
    defFree((char*)(this->subnets_[i]));
    this->subnets_[i] = 0;
  }
  this->numSubnets_ = 0;

  if (this->name_)
     this->name_[0] = '\0';

  // WMD -- this will be removed by the next release
  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      p = this->paths_[i];
      p->defiPath::Destroy();
      defFree((char*)p);
    }
    defFree((char*)(this->paths_));
    this->paths_ = 0;
    this->numPaths_ = 0;
    this->pathsAllocated_ = 0;
  }

  // 5.4.1
  this->fixedbump_ = 0;

  if (this->source_) { defFree(this->source_); this->source_ = 0; }
  if (this->pattern_) { defFree(this->pattern_); this->pattern_ = 0; }
  if (this->original_) { defFree(this->original_); this->original_ = 0; }
  if (this->use_) { defFree(this->use_); this->use_ = 0; }
  if (this->nonDefaultRule_) { defFree(this->nonDefaultRule_);
		    this->nonDefaultRule_ = 0; }
  this->style_ = 0;
 
  if (this->numWires_) {
    for (i = 0; i < this->numWires_; i++) {
      this->wires_[i]->defiWire::Destroy();
      defFree((char*)(this->wires_[i]));
      this->wires_[i] = 0;
    }
    defFree((char*)(this->wires_));
    this->wires_ = 0;
    this->numWires_ = 0;
    this->wiresAllocated_ = 0;
  }

  if (this->numShields_) {
    for (i = 0; i < this->numShields_; i++) {
      this->shields_[i]->defiShield::Destroy();
      defFree((char*)(this->shields_[i]));
      this->shields_[i] = 0;
    }
    this->numShields_ = 0;
    this->shieldsAllocated_ = 0;
  }

  if (this->numNoShields_) {
    for (i = 0; i < this->numNoShields_; i++) {
      this->shields_[i]->defiShield::Destroy();
      defFree((char*)(this->shields_[i]));
      this->shields_[i] = 0;
    }
    this->numNoShields_ = 0;
    this->shieldsAllocated_ = 0;
  }
  if (this->shields_)
    defFree((char*)(this->shields_));

  this->shields_ = 0;

  if (this->numWidths_) {
   for (i = 0; i < this->numWidths_; i++)
     defFree(this->wlayers_[i]);
  this->numWidths_ = 0;
  }

  if (this->numSpacing_) {
   for (i = 0; i < this->numSpacing_; i++)
     defFree(this->slayers_[i]);
  this->numSpacing_ = 0;
  }

  if (this->numShieldNet_) {
   for (i = 0; i < this->numShieldNet_; i++)
     defFree(this->shieldNet_[i]);
   this->numShieldNet_ = 0;
  }

  if (this->polygonNames_) {
    struct defiPoints* p;
    for (i = 0; i < this->numPolys_; i++) {
      if (this->polygonNames_[i]){
	  defFree((char*)(this->polygonNames_[i]));
      }
      if (this->polyRouteStatus_[i]) {
	  defFree((char*)(this->polyRouteStatus_[i]));
      }
      if (this->polyShapeTypes_[i]) {
	  defFree((char*)(this->polyShapeTypes_[i]));
      }
      if (this->polyRouteStatusShieldNames_[i]) {
          defFree((char*)(this->polyRouteStatusShieldNames_[i]));
      }
      p = this->polygons_[i];
      defFree((char*)(p->x));
      defFree((char*)(p->y));
      defFree((char*)(this->polygons_[i]));
    }
    defFree((char*)(this->polygonNames_));
    defFree((char*)(this->polygons_));
    defFree((char*)(this->polyMasks_));
    defFree((char*)(this->polyRouteStatus_));
    defFree((char*)(this->polyShapeTypes_));
    defFree((char*)(this->polyRouteStatusShieldNames_));
    this->polygonNames_ = 0;
    this->polygons_ = 0;
    this->polyMasks_ = 0;
    this->polyRouteStatus_ = 0;
    this->polyShapeTypes_ = 0;
    this->polyRouteStatusShieldNames_ = 0;
  }
  this->numPolys_ = 0;
  this->polysAllocated_ = 0;

  if (this->rectNames_) {
    for (i = 0; i < this->numRects_; i++) {
      if (this->rectNames_[i]) { 
	  defFree ((char*)(this->rectNames_[i]));
      }
      if (this->rectRouteStatus_[i]) { 
	  defFree ((char*)(this->rectRouteStatus_[i]));
      }
      if (this->rectRouteStatusShieldNames_[i]) {
          defFree ((char*)(this->rectRouteStatusShieldNames_[i]));
      }
      if (this->rectShapeTypes_[i]) { 
	  defFree ((char*)(this->rectShapeTypes_[i]));
      }
    }
    defFree((char*)(this->rectNames_));
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    defFree((char*)(this->rectMasks_));
    defFree((char*)(this->rectRouteStatus_));
    defFree((char*)(this->rectRouteStatusShieldNames_));
    defFree((char*)(this->rectShapeTypes_));
  }
  this->rectNames_ = 0;
  this->rectRouteStatus_ = 0;
  this->rectShapeTypes_ = 0;
  this->rectRouteStatusShieldNames_ = 0;
  this->numRects_ = 0;
  this->rectsAllocated_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->rectMasks_ = 0;

  if (this->viaNames_) {
      struct defiPoints* p;

      for (i = 0; i < this->numPts_; i++) {
          p = this->viaPts_[i];
          defFree((char*)(p->x));
          defFree((char*)(p->y));
          defFree((char*)(this->viaPts_[i]));
	  if (this->viaNames_[i]) {
	      defFree ((char*)(this->viaNames_[i]));
	  }
	  if (this->viaRouteStatus_[i]) {
	      defFree ((char*)(this->viaRouteStatus_[i]));
	  }
	  if (this->viaShapeTypes_[i]) {
	      defFree ((char*)(this->viaShapeTypes_[i]));
	  }
          if (this->viaRouteStatusShieldNames_[i]) {
              defFree ((char*)(this->viaRouteStatusShieldNames_[i]));
          }
      }
      defFree((char*)(this->viaNames_));
      defFree((char*)(this->viaPts_));
      defFree((char*)(this->viaMasks_));
      defFree((char*)(this->viaOrients_));
      defFree((char*)(this->viaRouteStatus_));
      defFree((char*)(this->viaShapeTypes_));
      defFree((char*)(this->viaRouteStatusShieldNames_));
      this->viaNames_ = 0;
      this->viaPts_ = 0;
      this->viaRouteStatus_ = 0;
      this->viaShapeTypes_ = 0;
      this->viaRouteStatusShieldNames_ = 0 ;
  }
  this->numPts_ = 0;
  this->ptsAllocated_ = 0;
  this->viaOrients_ = 0;
  this->viaMasks_ = 0;
}

void defiNet::clearRectPolyNPath() {
  int i;

  if (this->paths_) {
    for (i = 0; i < this->numPaths_; i++) {
      this->paths_[i]->defiPath::Destroy();
      defFree((char*)(this->paths_[i]));
    }
    this->numPaths_ = 0;
  }

  clearRectPoly();
}

void defiNet::clearRectPoly() {
  int i;

  if (this->polygonNames_) {
    struct defiPoints* p;
    for (i = 0; i < this->numPolys_; i++) {
      if (this->polygonNames_[i]){
	  defFree((char*)(this->polygonNames_[i]));
      }
      if (this->polyRouteStatus_[i]) {
	  defFree((char*)(this->polyRouteStatus_[i]));
      }
      if (this->polyShapeTypes_[i]) {
	  defFree((char*)(this->polyShapeTypes_[i]));
      }
      if (this->polyRouteStatusShieldNames_[i]) {
          defFree((char*)(this->polyRouteStatusShieldNames_[i]));
      }
      p = this->polygons_[i];
      defFree((char*)(p->x));
      defFree((char*)(p->y));
      defFree((char*)(this->polygons_[i]));
    }
    defFree((char*)(this->polyMasks_));
    defFree((char*)(this->polygonNames_));
    defFree((char*)(this->polygons_));
    defFree((char*)(this->polyRouteStatus_));
    defFree((char*)(this->polyShapeTypes_));
    defFree((char*)(this->polyRouteStatusShieldNames_));
  }
  this->numPolys_ = 0;
  this->polysAllocated_ = 0;
  this->polyMasks_ = 0;
  this->polygonNames_ = 0;
  this->polyRouteStatus_= 0;
  this->polyShapeTypes_= 0;
  this->polyRouteStatusShieldNames_ = 0;
  this->polygons_ = 0;

  if (this->rectNames_) {
    for (i = 0; i < this->numRects_; i++) {
      if (this->rectNames_[i]){
	  defFree((char*)(this->rectNames_[i]));
      }
      if (this->rectRouteStatus_[i]){
	  defFree((char*)(this->rectRouteStatus_[i]));
      }
      if (this->rectShapeTypes_[i]) {
	  defFree((char*)(this->rectShapeTypes_[i]));
      }
      if (this->rectRouteStatusShieldNames_[i]) {
          defFree((char*)(this->rectRouteStatusShieldNames_[i]));
      }
    }
    defFree((char*)(this->rectMasks_));
    defFree((char*)(this->rectNames_));
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    defFree((char*)(this->rectShapeTypes_));
    defFree((char*)(this->rectRouteStatus_));
    defFree((char*)(this->rectRouteStatusShieldNames_));
  }
  this->rectNames_ = 0;
  this->rectsAllocated_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->numRects_ = 0;
  this->rectMasks_ = 0;
  this->rectRouteStatus_ = 0;
  this->rectShapeTypes_ = 0;
  this->rectRouteStatusShieldNames_=0;
}

int defiNet::hasSource() const {
   return this->source_ ? 1 : 0;
}


int defiNet::hasFixedbump() const {
   return this->fixedbump_ ? 1 : 0;
}


int defiNet::hasFrequency() const {
  return (int)(this->hasFrequency_);
}


int defiNet::hasPattern() const {
   return this->pattern_ ? 1 : 0;
}


int defiNet::hasOriginal() const {
   return this->original_ ? 1 : 0;
}


int defiNet::hasCap() const {
  return (int)(this->hasCap_);
}


int defiNet::hasUse() const {
   return this->use_ ? 1 : 0;
}


int defiNet::hasStyle() const {
   return this->style_ ? 1 : 0;
}


int defiNet::hasXTalk() const {
   return (this->xTalk_ != -1) ? 1 : 0;
}


int defiNet::hasNonDefaultRule() const {
   return this->nonDefaultRule_ ? 1 : 0;
}


void defiNet::setSource(const char* typ) {
  int len;
  if (this->source_) defFree(this->source_);
  len = strlen(typ) + 1;
  this->source_ = (char*)defMalloc(len);
  strcpy(this->source_, DEFCASE(typ));
}


void defiNet::setFixedbump() {
  this->fixedbump_ = 1;
}


void defiNet::setFrequency(double frequency) {
  this->frequency_ = frequency;
  this->hasFrequency_ = 1;
}


void defiNet::setOriginal(const char* typ) {
  int len;
  if (this->original_) defFree(this->original_);
  len = strlen(typ) + 1;
  this->original_ = (char*)defMalloc(len);
  strcpy(this->original_, DEFCASE(typ));
}


void defiNet::setPattern(const char* typ) {
  int len;
  if (this->pattern_) defFree(this->pattern_);
  len = strlen(typ) + 1;
  this->pattern_ = (char*)defMalloc(len);
  strcpy(this->pattern_, DEFCASE(typ));
}


void defiNet::setCap(double w) {
  this->cap_ = w;
  this->hasCap_ = 1;
}


void defiNet::setUse(const char* typ) {
  int len;
  if (this->use_) defFree(this->use_);
  len = strlen(typ) + 1;
  this->use_ = (char*)defMalloc(len);
  strcpy(this->use_, DEFCASE(typ));
}


void defiNet::setStyle(int style) {
  this->style_ = style;
}


void defiNet::setNonDefaultRule(const char* typ) {
  int len;
  if (this->nonDefaultRule_) defFree(this->nonDefaultRule_);
  len = strlen(typ) + 1;
  this->nonDefaultRule_ = (char*)defMalloc(len);
  strcpy(this->nonDefaultRule_, DEFCASE(typ));
}


const char* defiNet::source() const {
  return this->source_;
}


const char* defiNet::original() const {
  return this->original_;
}


const char* defiNet::pattern() const {
  return this->pattern_;
}


double defiNet::cap() const {
  return (this->hasCap_ ? this->cap_ : 0.0);
}


double defiNet::frequency() const {
  return (this->hasFrequency_ ? this->frequency_ : 0.0);
}


const char* defiNet::use() const {
  return this->use_;
}


int defiNet::style() const {
  return this->style_;
}


const char* defiNet::shieldNet(int index) const {
  return this->shieldNet_[index];
}


const char* defiNet::nonDefaultRule() const {
  return this->nonDefaultRule_;
}

// WMD -- this will be removed by the next release
void defiNet::bumpPaths(int size) {
  int i;
  defiPath** newPaths = (defiPath**)defMalloc(sizeof(defiPath)*size);
 
  for (i = 0; i < this->numPaths_; i++)
    newPaths[i] = this->paths_[i];
 
  this->pathsAllocated_ = size;
  defFree((char*)(this->paths_));
  this->paths_ = newPaths;
}
 
// WMD -- this will be removed by the next release
int defiNet::numPaths() const {
  return this->numPaths_;
}
 
// WMD -- this will be removed by the next release
defiPath* defiNet::path(int index) {
  if (index >= 0 && index < this->numPaths_)
    return this->paths_[index];
  return 0;
}


int defiNet::numWires() const {
  return this->numWires_;
}


defiWire* defiNet::wire(int index) {
  if (index >= 0 && index < this->numWires_)
    return this->wires_[index];
  return 0;
}


void defiNet::bumpShieldNets(int size) {
  char** newShieldNets = (char**)defMalloc(sizeof(char*)*size);
  int i;
 
  if (this->shieldNet_) {
    for (i = 0; i < this->shieldNetsAllocated_; i++) {
      newShieldNets[i] = this->shieldNet_[i];
    }
    defFree((char*)(this->shieldNet_));
  }
 
  this->shieldNet_ = newShieldNets;
  this->shieldNetsAllocated_ = size;
}


int defiNet::numShields() const {
  return this->numShields_;
}


defiShield* defiNet::shield(int index) {
  if (index >= 0 && index < this->numShields_)
    return this->shields_[index];
  return 0;
}


int defiNet::numNoShields() const {
  return this->numNoShields_;
}


defiShield* defiNet::noShield(int index) {
  if (index >= 0 && index < this->numNoShields_)
    return this->shields_[index];
  return 0;
}


int defiNet::hasVoltage() const {
  return (int)(this->hasVoltage_);
}


double defiNet::voltage() const {
  return this->voltage_;
}


int defiNet::numWidthRules() const {
  return this->numWidths_;
}


int defiNet::numSpacingRules() const {
  return this->numSpacing_;
}


int defiNet::hasWidthRules() const {
  return this->numWidths_;
}


int defiNet::hasSpacingRules() const {
  return this->numSpacing_;
}


void defiNet::setXTalk(int i) {
  this->xTalk_ = i;
}


int defiNet::XTalk() const {
  return this->xTalk_;
}


void defiNet::addVpin(const char* name) {
  defiVpin* vp;
  if (this->numVpins_ == this->vpinsAllocated_) {
    defiVpin** array;
    int i;
    this->vpinsAllocated_ = this->vpinsAllocated_ ?
	      this->vpinsAllocated_ * 2 : 2 ;
    array = (defiVpin**)defMalloc(sizeof(defiVpin*)*this->vpinsAllocated_);
    for (i = 0; i < this->numVpins_; i++)
      array[i] = this->vpins_[i];
    if (this->vpins_) defFree((char*)(this->vpins_));
    this->vpins_ = array;
  }
  vp = this->vpins_[this->numVpins_] = (defiVpin*)defMalloc(sizeof(defiVpin));
  this->numVpins_ += 1;
  vp->defiVpin::Init(name);
}


void defiNet::addVpinLayer(const char* name) {
  defiVpin* vp = this->vpins_[this->numVpins_-1];
  vp->defiVpin::setLayer(name);
}


void defiNet::addVpinLoc(const char* status, int x, int y, int orient) {
  defiVpin* vp = this->vpins_[this->numVpins_-1];
  vp->defiVpin::setStatus(*status);
  vp->defiVpin::setLoc(x,y);
  vp->defiVpin::setOrient(orient);
}


void defiNet::addVpinBounds(int xl, int yl, int xh, int yh) {
  defiVpin* vp = this->vpins_[this->numVpins_-1];
  vp->defiVpin::setBounds(xl, yl, xh, yh);
}


int defiNet::numVpins() const {
  return this->numVpins_;
}


defiVpin* defiNet::vpin(int index) const {
  if (index < 0 || index >= this->numVpins_) return 0;
  return this->vpins_[index];
}


void defiNet::spacingRule(int index, char** layer, double* dist,
	 double* left, double* right) {
  if (index >= 0 && index < this->numSpacing_) {
    if (layer) *layer = this->slayers_[index];
    if (dist) *dist = this->sdist_[index];
    if (left) *left = this->sleft_[index];
    if (right) *right = this->sright_[index];
  }
}


void defiNet::widthRule(int index, char** layer, double* dist) {
  if (index >= 0 && index < this->numWidths_) {
    if (layer) *layer = this->wlayers_[index];
    if (dist) *dist = this->wdist_[index];
  }
}


void defiNet::setVoltage(double v) {
  this->voltage_ = v;
  this->hasVoltage_ = 1;
}


void defiNet::setWidth(const char* layer, double d) {
  int len = strlen(layer) + 1;
  char* l = (char*)defMalloc(len);
  strcpy(l, DEFCASE(layer));

  if (this->numWidths_ >= this->widthsAllocated_) {
    int i;
    char** nl;
    double* nd;
    this->widthsAllocated_ = this->widthsAllocated_ ?
       this->widthsAllocated_ * 2 : 4 ;
    nl = (char**)defMalloc(sizeof(char*) * this->widthsAllocated_);
    nd = (double*)defMalloc(sizeof(double) * this->widthsAllocated_);
    for (i = 0; i < this->numWidths_; i++) {
      nl[i] = this->wlayers_[i];
      nd[i] = this->wdist_[i];
    }
    defFree((char*)(this->wlayers_));
    defFree((char*)(this->wdist_));
    this->wlayers_ = nl;
    this->wdist_ = nd;
  }

  this->wlayers_[this->numWidths_] = l;
  this->wdist_[this->numWidths_] = d;
  (this->numWidths_)++;
}


void defiNet::setSpacing(const char* layer, double d) {
  int len = strlen(layer) + 1;
  char* l = (char*)defMalloc(len);
  strcpy(l, DEFCASE(layer));

  if (this->numSpacing_ >= this->spacingAllocated_) {
    int i;
    char** nl;
    double* nd;
    double* n1;
    double* n2;
    this->spacingAllocated_ = this->spacingAllocated_ ?
       this->spacingAllocated_ * 2 : 4 ;
    nl = (char**)defMalloc(sizeof(char*) * this->spacingAllocated_);
    nd = (double*)defMalloc(sizeof(double) * this->spacingAllocated_);
    n1 = (double*)defMalloc(sizeof(double) * this->spacingAllocated_);
    n2 = (double*)defMalloc(sizeof(double) * this->spacingAllocated_);
    for (i = 0; i < this->numSpacing_; i++) {
      nl[i] = this->slayers_[i];
      nd[i] = this->sdist_[i];
      n1[i] = this->sleft_[i];
      n2[i] = this->sright_[i];
    }
    defFree((char*)(this->slayers_));
    defFree((char*)(this->sdist_));
    defFree((char*)(this->sleft_));
    defFree((char*)(this->sright_));
    this->slayers_ = nl;
    this->sdist_ = nd;
    this->sleft_ = n1;
    this->sright_ = n2;
  }

  this->slayers_[this->numSpacing_] = l;
  this->sdist_[this->numSpacing_] = d;
  this->sleft_[this->numSpacing_] = d;
  this->sright_[this->numSpacing_] = d;
  (this->numSpacing_)++;
}


void defiNet::setRange(double left, double right) {
  // This is always called right after setSpacing.
  this->sleft_[this->numSpacing_-1] = left;
  this->sright_[this->numSpacing_-1] = right;
}

// 5.6
void defiNet::addPolygon(const char* layerName, defiGeometries* geom,
                         int *needCbk, int colorMask,
			 const char* routeStatus,
			 const char* shapeType,
                         const char* routeStatusShieldName) {
  struct defiPoints* p;
  int x, y;
  int i;

  // This method will only call by specialnet, need to change if net also
  // calls it.
  *needCbk = 0;
  if (this->numPolys_ == this->polysAllocated_) {
    char** newn;
    char** newRS;
    char** newST;
    char** newRSN;
    int* maskn;
    struct defiPoints** poly;
    this->polysAllocated_ = (this->polysAllocated_ == 0) ?
          1000 : this->polysAllocated_ * 2;
    newn = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    newRS = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    newST = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    newRSN = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    maskn = (int*)defMalloc(sizeof(int) * this->polysAllocated_);
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->polysAllocated_);
    for (i = 0; i < this->numPolys_; i++) {
      newn[i] = this->polygonNames_[i];
      poly[i] = this->polygons_[i];
      maskn[i] = this->polyMasks_[i];
      newRS[i] = this->polyRouteStatus_[i];
      newST[i] = this->polyShapeTypes_[i];
      newRSN[i] = this->polyRouteStatusShieldNames_[i];

    }
    if (this->polygons_)
      defFree((char*)(this->polygons_));
    if (this->polygonNames_)
      defFree((char*)(this->polygonNames_));
    if (this->polyMasks_)
      defFree((char*)(this->polyMasks_));
    if (this->polyRouteStatus_)
      defFree((char*)(this->polyRouteStatus_));
    if (this->polyShapeTypes_)
      defFree((char*)(this->polyShapeTypes_));
    if (this->polyRouteStatusShieldNames_)
      defFree((char*)(this->polyRouteStatusShieldNames_));
    this->polygonNames_ = newn;
    this->polygons_ = poly;
    this->polyMasks_ = maskn;
    this->polyShapeTypes_ = newST;
    this->polyRouteStatus_= newRS;
    this->polyRouteStatusShieldNames_ = newRSN;
  }
  this->polygonNames_[this->numPolys_] = strdup(layerName);
  this->polyRouteStatus_[this->numPolys_] = strdup(routeStatus);
  this->polyShapeTypes_[this->numPolys_] = strdup(shapeType);
  this->polyRouteStatusShieldNames_[this->numPolys_] = strdup(routeStatusShieldName);
  p = (struct defiPoints*)defMalloc(sizeof(struct defiPoints));
  p->numPoints = geom->defiGeometries::numPoints();
  p->x = (int*)defMalloc(sizeof(int)*p->numPoints);
  p->y = (int*)defMalloc(sizeof(int)*p->numPoints);
  for (i = 0; i < p->numPoints; i++) {
    geom->defiGeometries::points(i, &x, &y);
    p->x[i] = x;
    p->y[i] = y;
  }
  this->polyMasks_[this->numPolys_] = colorMask;
  this->polygons_[this->numPolys_] = p;
  this->numPolys_ += 1;
  if (this->numPolys_ == 1000)  // Want to invoke the partial callback if set
     *needCbk = 1;
}


// 5.6
int defiNet::numPolygons() const {
  return this->numPolys_;
}


// 5.6
const char* defiNet::polygonName(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numPolys_) {
     sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numPolys_);
     defiError(0, 6085, errMsg);
     return 0;
  }
  return this->polygonNames_[index];
}

const char* defiNet::polyRouteStatus(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numPolys_) {
     sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numPolys_);
     defiError(0, 6085, errMsg);
     return 0;
  }
  return this->polyRouteStatus_[index];
}

const char* defiNet::polyRouteStatusShieldName(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPolys_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPolys_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return this->polyRouteStatusShieldNames_[index];
}

const char* defiNet::polyShapeType(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numPolys_) {
     sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numPolys_);
     defiError(0, 6085, errMsg);
     return 0;
  }
  return this->polyShapeTypes_[index];
}

int defiNet::polyMask(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numPolys_) {
     sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numPolys_);
     defiError(0, 6085, errMsg);
     return 0;
  }
  return this->polyMasks_[index];
}

// 5.6
struct defiPoints defiNet::getPolygon(int index) const {
  return *(this->polygons_[index]);
}

// 5.6
void defiNet::addRect(const char* layerName, int xl, int yl, int xh, int yh,
                      int *needCbk, 
		      int colorMask,
		      const char* routeStatus,
		      const char* shapeType,
                      const char* routeStatusName) {
  // This method will only call by specialnet, need to change if net also
  // calls it.
  *needCbk = 0;
  if (this->numRects_ == this->rectsAllocated_) {
    int    i;
    int    max;
    char** newn;
    int*   newxl;
    int*   newyl;
    int*   newxh;
    int*   newyh;
    int*   newMask;
    char** newRS;
    char** newST;
    char** newRSN;

    max = this->rectsAllocated_ = (this->rectsAllocated_ == 0) ? 1000 :
              this->rectsAllocated_ * 2;
    newn = (char**)defMalloc(sizeof(char*)*max);
    newRS = (char**)defMalloc(sizeof(char*)*max);
    newST = (char**)defMalloc(sizeof(char*)*max);
    newRSN = (char**)defMalloc(sizeof(char*)*max);
    newxl = (int*)defMalloc(sizeof(int)*max);
    newyl = (int*)defMalloc(sizeof(int)*max);
    newxh = (int*)defMalloc(sizeof(int)*max);
    newyh = (int*)defMalloc(sizeof(int)*max);
    newMask = (int*)defMalloc(sizeof(int)*max);
    for (i = 0; i < this->numRects_; i++) {
      newn[i] = this->rectNames_[i];
      newxl[i] = this->xl_[i];
      newyl[i] = this->yl_[i];
      newxh[i] = this->xh_[i];
      newyh[i] = this->yh_[i];
      newMask[i] = this->rectMasks_[i];
      newRS[i] = this->rectRouteStatus_[i];
      newST[i] = this->rectShapeTypes_[i];
      newRSN[i] = this->rectRouteStatusShieldNames_[i];
    }
    if (this->rectNames_)
      defFree((char*)(this->rectNames_));
    if (this->rectRouteStatus_)
      defFree((char*)(this->rectRouteStatus_));
    if (this->rectShapeTypes_)
      defFree((char*)(this->rectShapeTypes_));
    if (this->rectRouteStatusShieldNames_)
      defFree((char*)(this->rectRouteStatusShieldNames_));
    if (this->xl_) {
      defFree((char*)(this->xl_));
      defFree((char*)(this->yl_));
      defFree((char*)(this->xh_));
      defFree((char*)(this->yh_));
      defFree((char*)(this->rectMasks_));
    }
    this->rectNames_ = newn;
    this->xl_ = newxl;
    this->yl_ = newyl;
    this->xh_ = newxh;
    this->yh_ = newyh;
    this->rectMasks_ = newMask;
    this->rectRouteStatus_ = newRS;
    this->rectShapeTypes_ = newST;
    this->rectRouteStatusShieldNames_ = newRSN;
  }
  this->rectNames_[this->numRects_] = strdup(layerName);
  this->xl_[this->numRects_] = xl;
  this->yl_[this->numRects_] = yl;
  this->xh_[this->numRects_] = xh;
  this->yh_[this->numRects_] = yh;
  this->rectMasks_[this->numRects_] = colorMask;
  this->rectRouteStatus_[this->numRects_] = strdup(routeStatus);
  this->rectShapeTypes_[this->numRects_] = strdup(shapeType);
  this->rectRouteStatusShieldNames_[this->numRects_] = strdup(routeStatusName);
  this->numRects_ += 1;
  if (this->numRects_ == 1000)  // Want to invoke the partial callback if set
     *needCbk = 1;
}

// 5.6
int defiNet::numRectangles() const {
  return this->numRects_;
}

// 5.6
const char* defiNet::rectName(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->rectNames_[index];
}

const char* defiNet::rectRouteStatus(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->rectRouteStatus_[index];
}

const char* defiNet::rectRouteStatusShieldName(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numRects_) {
        sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numRects_);
        defiError(0, 6086, errMsg);
        return 0;
    }
    return this->rectRouteStatusShieldNames_[index];
}

const char* defiNet::rectShapeType(int index) const {
  char errMsg[128];
  if (index < 0 || index > this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->rectShapeTypes_[index];
}

// 5.6
int defiNet::xl(int index) const {
  char errMsg[128];
  if (index < 0 || index >= this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->xl_[index];
}

// 5.6
int defiNet::yl(int index) const {
  char errMsg[128];
  if (index < 0 || index >= this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->yl_[index];
}

// 5.6
int defiNet::xh(int index) const {
  char errMsg[128];
  if (index < 0 || index >= this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->xh_[index];
}

// 5.6
int defiNet::yh(int index) const {
  char errMsg[128];
  if (index < 0 || index >= this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->yh_[index];
}

int defiNet::rectMask(int index) const {
  char errMsg[128];
  if (index < 0 || index >= this->numRects_) {
     sprintf (errMsg, "ERROR (DEFPARS-6086): The index number %d specified for the NET RECTANGLE is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
             index, this->numRects_);
     defiError(0, 6086, errMsg);
     return 0;
  }
  return this->rectMasks_[index];
}


void defiNet::addPts(const char* viaName, int o, defiGeometries* geom, 
	             int *needCbk, int colorMask,
		     const char* routeStatus,
		     const char* shapeType,
                     const char* routeStatusShieldName) {
    struct defiPoints* p;
    int x, y;
    int i;

    *needCbk = 0;
    if (this->numPts_ == this->ptsAllocated_) {
        struct defiPoints** pts;
        char** newn;
	char** newRS;
	char** newST;
        char** newRSN;
        int*   orientn;
	int*   maskn;

        this->ptsAllocated_ = (this->ptsAllocated_ == 0) ?
            1000 : this->ptsAllocated_ * 2;
        newn = (char**)defMalloc(sizeof(char*) *this->ptsAllocated_);
        newRS = (char**)defMalloc(sizeof(char*) *this->ptsAllocated_);
        newST = (char**)defMalloc(sizeof(char*) *this->ptsAllocated_);
        newRSN = (char**)defMalloc(sizeof(char*) *this->ptsAllocated_);
        orientn = (int*)defMalloc(sizeof(int) *this->ptsAllocated_);
        pts= (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->ptsAllocated_);
	maskn = (int*)defMalloc(sizeof(int) *this->ptsAllocated_);
        for (i = 0; i < this->numPts_; i++) {
            pts[i] = this->viaPts_[i];
            newn[i] = this->viaNames_[i];
            newRS[i] = this->viaRouteStatus_[i];
            newST[i] = this->viaShapeTypes_[i];
            newRSN[i] = this->viaRouteStatusShieldNames_[i];
            orientn[i] = this->viaOrients_[i];
	    maskn[i] = this->viaMasks_[i];
        }
        if (this->viaPts_)
            defFree((char*)(this->viaPts_));
        if (this->viaNames_) 
            defFree((char*)(this->viaNames_));
        if (this->viaOrients_)
            defFree((char*)(this->viaOrients_));
        if (this->viaMasks_)
	    defFree((char*)(this->viaMasks_));
        if (this->viaRouteStatus_)
	    defFree((char*)(this->viaRouteStatus_));
        if (this->viaShapeTypes_)
	    defFree((char*)(this->viaShapeTypes_));
        if (this->viaRouteStatusShieldNames_)
            defFree((char*)(this->viaRouteStatusShieldNames_));
    
        this->viaPts_ = pts;
        this->viaNames_ = newn;
        this->viaOrients_ = orientn;
	this->viaMasks_ = maskn;
	this->viaShapeTypes_= newST;
	this->viaRouteStatus_ = newRS;
        this->viaRouteStatusShieldNames_ = newRSN;
    }
    this->viaNames_[this->numPts_] = strdup(viaName);
    this->viaShapeTypes_[this->numPts_] = strdup(shapeType);
    this->viaRouteStatus_[this->numPts_] = strdup(routeStatus);
    this->viaRouteStatusShieldNames_[this->numPts_] = strdup(routeStatusShieldName);
    this->viaOrients_[this->numPts_] = o;
    this->viaMasks_[this->numPts_] = colorMask;
    p = (struct defiPoints*)defMalloc(sizeof(struct defiPoints));
    p->numPoints = geom->defiGeometries::numPoints();
    p->x = (int*)defMalloc(sizeof(int)*p->numPoints);
    p->y = (int*)defMalloc(sizeof(int)*p->numPoints);
    for (i = 0; i < p->numPoints; i++) {
        geom->defiGeometries::points(i, &x, &y);
        p->x[i] = x;
        p->y[i] = y;
    }
    this->viaPts_[this->numPts_] = p;
    this->numPts_ += 1;
    if (this->numPts_ == 1000)  // Want to invoke the partial callback if set
     *needCbk = 1;
}

int defiNet::numViaSpecs() const {
    return this->numPts_;
}

const char* defiNet::viaName(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return this->viaNames_[index];
}

const char* defiNet::viaRouteStatus(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return this->viaRouteStatus_[index];
}

const char* defiNet::viaRouteStatusShieldName(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return this->viaRouteStatusShieldNames_[index];
}

const char* defiNet::viaShapeType(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return this->viaShapeTypes_[index];
}

const int defiNet::viaOrient(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return this->viaOrients_[index];
}

const char* defiNet::viaOrientStr(int index) const  {
    char errMsg[128];

    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }
    return (defiOrientStr(viaOrients_[index]));
}

const int defiNet::topMaskNum(int index) const {
   char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }

    return viaMasks_[index] / 100;
}

const int defiNet::cutMaskNum(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }

    return viaMasks_[index] / 10 % 10;
}

const int defiNet::bottomMaskNum(int index) const {
    char errMsg[128];
    if (index < 0 || index > this->numPts_) {
        sprintf (errMsg, "ERROR (DEFPARS-6085): The index number %d specified for the NET POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again.",
            index, this->numPts_);
        defiError(0, 6085, errMsg);
        return 0;
    }

    return viaMasks_[index] % 10;
}

struct defiPoints defiNet::getViaPts(int index)const {
    return *(this->viaPts_[index]);
}

void defiNet::clearVia() {
    if (this->viaNames_) {
        struct defiPoints* p;
        for (int i = 0; i < this->numPts_; i++) {
            if (this->viaNames_[i]) {
		defFree((char*)(this->viaNames_[i]));
	    }
	    if (this->viaRouteStatus_[i]) {
	      defFree ((char*)(this->viaRouteStatus_[i]));
	    }
	    if (this->viaShapeTypes_[i]) {
	      defFree ((char*)(this->viaShapeTypes_[i]));
	    }
            if (this->viaRouteStatusShieldNames_[i]) {
              defFree ((char*)(this->viaRouteStatusShieldNames_[i]));
            }
            p = this->viaPts_[i];
            defFree((char*)(p->x));
            defFree((char*)(p->y));
            defFree((char*)(this->viaPts_[i]));
        }
	if (this->viaMasks_) {
	    defFree((char*)(this->viaMasks_));
	}
	if (this->viaOrients_) {
	    defFree((char*)(this->viaOrients_));
	}
	if (this->viaNames_) {
	    defFree((char*)(this->viaNames_));
	}
	if (this->viaRouteStatus_) {
	    defFree((char*)(this->viaRouteStatus_));
	}
	if (this->viaShapeTypes_) {
	    defFree((char*)(this->viaShapeTypes_));
	}
	if (this->viaRouteStatusShieldNames_) {
	    defFree((char*)(this->viaRouteStatusShieldNames_));
	}
	if (this->viaPts_) {
	    defFree((char*)(this->viaPts_));
	}
    }

    this->viaMasks_ = 0;
    this->viaOrients_ = 0;
    this->numPts_ = 0;
    this->ptsAllocated_ = 0;
    this->viaPts_ = 0;
    this->viaRouteStatus_ = 0;
    this->viaShapeTypes_ = 0;
    this->viaRouteStatusShieldNames_ = 0;
    this->viaNames_ = 0;
}

