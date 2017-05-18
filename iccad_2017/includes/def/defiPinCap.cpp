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
#include "defiPinCap.hpp"
#include "defiComponent.hpp"
#include "defiDebug.hpp"
#include "defiUtil.hpp"


///////////////////////////////////////////////
///////////////////////////////////////////////
//
//     defiPinCap
//
///////////////////////////////////////////////
///////////////////////////////////////////////


void defiPinCap::setPin(int p) {
  this->pin_ = p;
}


void defiPinCap::setCap(double d) {
  this->cap_ = d;
}


int defiPinCap::pin() const {
  return this->pin_;
}


double defiPinCap::cap() const {
  return this->cap_;
}


void defiPinCap::print(FILE* f) const {
  fprintf(f, "PinCap  %d %5.2f\n", this->pin_, this->cap_);
}


///////////////////////////////////////////////
///////////////////////////////////////////////
//
//     defiPinAntennaModel
//
///////////////////////////////////////////////
///////////////////////////////////////////////

defiPinAntennaModel::defiPinAntennaModel() {
  this->defiPinAntennaModel::Init();
}


void defiPinAntennaModel::Init() {
  this->numAPinGateArea_ = 0;                       // 5.4
  this->APinGateAreaAllocated_ = 0;                 // 5.4
  this->APinGateArea_ = 0;
  this->APinGateAreaLayer_ = 0;
  this->numAPinMaxAreaCar_ = 0;                     // 5.4
  this->APinMaxAreaCarAllocated_ = 0;               // 5.4
  this->APinMaxAreaCar_ = 0;
  this->APinMaxAreaCarLayer_ = 0;
  this->numAPinMaxSideAreaCar_ = 0;                 // 5.4
  this->APinMaxSideAreaCarAllocated_ = 0;           // 5.4
  this->APinMaxSideAreaCar_ = 0;
  this->APinMaxSideAreaCarLayer_ = 0;
  this->numAPinMaxCutCar_ = 0;                      // 5.4
  this->APinMaxCutCarAllocated_ = 0;                // 5.4
  this->APinMaxCutCar_ = 0;
  this->APinMaxCutCarLayer_ = 0;
}

defiPinAntennaModel::~defiPinAntennaModel() {
  this->defiPinAntennaModel::Destroy();
}


void defiPinAntennaModel::clear() {
  int i;

  if (this->oxide_)
    defFree((char*)(this->oxide_));
  this->oxide_ = 0;

  for (i = 0; i < this->numAPinGateArea_; i++) {
    if (this->APinGateAreaLayer_[i])
       defFree(this->APinGateAreaLayer_[i]);
  }
  this->numAPinGateArea_ = 0;

  for (i = 0; i < this->numAPinMaxAreaCar_; i++) {
    if (this->APinMaxAreaCarLayer_[i])
       defFree(this->APinMaxAreaCarLayer_[i]);
  }
  this->numAPinMaxAreaCar_ = 0;

  for (i = 0; i < this->numAPinMaxSideAreaCar_; i++) {
    if (this->APinMaxSideAreaCarLayer_[i])
       defFree(this->APinMaxSideAreaCarLayer_[i]);
  }
  this->numAPinMaxSideAreaCar_ = 0;

  for (i = 0; i < this->numAPinMaxCutCar_; i++) {
    if (this->APinMaxCutCarLayer_[i])
       defFree(this->APinMaxCutCarLayer_[i]);
  }
  this->numAPinMaxCutCar_ = 0;
}

void defiPinAntennaModel::Destroy() {
  this->defiPinAntennaModel::clear();
  if (this->APinGateArea_)
     defFree((char*)(this->APinGateArea_));
  if (this->APinGateAreaLayer_)
     defFree((char*)(this->APinGateAreaLayer_));
  if (this->APinMaxAreaCar_)
     defFree((char*)(this->APinMaxAreaCar_));
  if (this->APinMaxAreaCarLayer_)
     defFree((char*)(this->APinMaxAreaCarLayer_));
  if (this->APinMaxSideAreaCar_)
     defFree((char*)(this->APinMaxSideAreaCar_));
  if (this->APinMaxSideAreaCarLayer_)
     defFree((char*)(this->APinMaxSideAreaCarLayer_));
  if (this->APinMaxCutCar_)
     defFree((char*)(this->APinMaxCutCar_));
  if (this->APinMaxCutCarLayer_)
     defFree((char*)(this->APinMaxCutCarLayer_));
}

// 5.5
void defiPinAntennaModel::setAntennaModel(int aOxide) {
  switch (aOxide) {
     case 2:
           this->oxide_ = strdup("OXIDE2");
           break;
     case 3:
           this->oxide_ = strdup("OXIDE3");
           break;
     case 4:
           this->oxide_ = strdup("OXIDE4");
           break;
     default:
           this->oxide_ = strdup("OXIDE1");
           break;
  }
}

void defiPinAntennaModel::addAPinGateArea(int value, const char* layer) {
  if (this->numAPinGateArea_ == this->APinGateAreaAllocated_) {
     int i;
     int max;
     int lim = this->numAPinGateArea_;
     int* nd;
     char**  nl;

     if (this->APinGateAreaAllocated_ == 0)
        max = this->APinGateAreaAllocated_ = 2;
     else
        max = this->APinGateAreaAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinGateArea_[i];
        nl[i] = this->APinGateAreaLayer_[i];
     }
     defFree((char*)(this->APinGateArea_));
     defFree((char*)(this->APinGateAreaLayer_));
     this->APinGateArea_ = nd;
     this->APinGateAreaLayer_ = nl;

  }
  this->APinGateArea_[this->numAPinGateArea_] = value;
  if (layer) {
    this->APinGateAreaLayer_[this->numAPinGateArea_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinGateAreaLayer_[this->numAPinGateArea_],
       DEFCASE(layer));
  } else
    this->APinGateAreaLayer_[this->numAPinGateArea_] = NULL;
  this->numAPinGateArea_ += 1;
}

void defiPinAntennaModel::addAPinMaxAreaCar(int value, const char* layer) {
  if (this->numAPinMaxAreaCar_ == this->APinMaxAreaCarAllocated_) {
     int i;
     int max;
     int lim = this->numAPinMaxAreaCar_;
     int* nd;
     char**  nl;

     if (this->APinMaxAreaCarAllocated_ == 0)
        max = this->APinMaxAreaCarAllocated_ = 2;
     else
        max = this->APinMaxAreaCarAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinMaxAreaCar_[i];
        nl[i] = this->APinMaxAreaCarLayer_[i];
     }
     defFree((char*)(this->APinMaxAreaCar_));
     defFree((char*)(this->APinMaxAreaCarLayer_));
     this->APinMaxAreaCar_ = nd;
     this->APinMaxAreaCarLayer_ = nl;

  }
  this->APinMaxAreaCar_[this->numAPinMaxAreaCar_] = value;
  if (layer) {
    this->APinMaxAreaCarLayer_[this->numAPinMaxAreaCar_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinMaxAreaCarLayer_[this->numAPinMaxAreaCar_],
       DEFCASE(layer));
  } else
    this->APinMaxAreaCarLayer_[this->numAPinMaxAreaCar_] = NULL;
  this->numAPinMaxAreaCar_ += 1;
}

void defiPinAntennaModel::addAPinMaxSideAreaCar(int value, const char* layer) {
  if (this->numAPinMaxSideAreaCar_ == this->APinMaxSideAreaCarAllocated_) {
     int i;
     int max;
     int lim = this->numAPinMaxSideAreaCar_;
     int* nd;
     char**  nl;

     if (this->APinMaxSideAreaCarAllocated_ == 0)
        max = this->APinMaxSideAreaCarAllocated_ = 2;
     else
        max = this->APinMaxSideAreaCarAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinMaxSideAreaCar_[i];
        nl[i] = this->APinMaxSideAreaCarLayer_[i];
     }
     defFree((char*)(this->APinMaxSideAreaCar_));
     defFree((char*)(this->APinMaxSideAreaCarLayer_));
     this->APinMaxSideAreaCar_ = nd;
     this->APinMaxSideAreaCarLayer_ = nl;

  }
  this->APinMaxSideAreaCar_[this->numAPinMaxSideAreaCar_] = value;
  if (layer) {
    this->APinMaxSideAreaCarLayer_[this->numAPinMaxSideAreaCar_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinMaxSideAreaCarLayer_[this->numAPinMaxSideAreaCar_],
       DEFCASE(layer));
  } else
    this->APinMaxSideAreaCarLayer_[this->numAPinMaxSideAreaCar_] = NULL;
  this->numAPinMaxSideAreaCar_ += 1;
}

void defiPinAntennaModel::addAPinMaxCutCar(int value, const char* layer) {
  if (this->numAPinMaxCutCar_ == this->APinMaxCutCarAllocated_) {
     int i;
     int max;
     int lim = this->numAPinMaxCutCar_;
     int* nd;
     char**  nl;

     if (this->APinMaxCutCarAllocated_ == 0)
        max = this->APinMaxCutCarAllocated_ = 2;
     else
        max = this->APinMaxCutCarAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinMaxCutCar_[i];
        nl[i] = this->APinMaxCutCarLayer_[i];
     }
     defFree((char*)(this->APinMaxCutCar_));
     defFree((char*)(this->APinMaxCutCarLayer_));
     this->APinMaxCutCar_ = nd;
     this->APinMaxCutCarLayer_ = nl;

  }
  this->APinMaxCutCar_[this->numAPinMaxCutCar_] = value;
  if (layer) {
    this->APinMaxCutCarLayer_[this->numAPinMaxCutCar_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinMaxCutCarLayer_[this->numAPinMaxCutCar_],
       DEFCASE(layer));
  } else
    this->APinMaxCutCarLayer_[this->numAPinMaxCutCar_] = NULL;
  this->numAPinMaxCutCar_ += 1;
}

// 5.5
char* defiPinAntennaModel::antennaOxide() const {
  return this->oxide_;
}

int defiPinAntennaModel::hasAPinGateArea() const {
  return this->numAPinGateArea_ ? 1 : 0 ;
}

int defiPinAntennaModel::hasAPinMaxAreaCar() const {
  return this->numAPinMaxAreaCar_ ? 1 : 0 ;
}

int defiPinAntennaModel::hasAPinMaxSideAreaCar() const {
  return this->numAPinMaxSideAreaCar_ ? 1 : 0 ;
}

int defiPinAntennaModel::hasAPinMaxCutCar() const {
  return this->numAPinMaxCutCar_ ? 1 : 0 ;
}

int defiPinAntennaModel::numAPinGateArea() const {
  return this->numAPinGateArea_;
}

int defiPinAntennaModel::numAPinMaxAreaCar() const {
  return this->numAPinMaxAreaCar_;
}

int defiPinAntennaModel::numAPinMaxSideAreaCar() const {
  return this->numAPinMaxSideAreaCar_;
}

int defiPinAntennaModel::numAPinMaxCutCar() const {
  return this->numAPinMaxCutCar_;
}

int defiPinAntennaModel::APinGateArea(int i) const {
  return this->APinGateArea_[i];
}

int defiPinAntennaModel::hasAPinGateAreaLayer(int i) const {
  return (this->APinGateAreaLayer_[i] && *(this->APinGateAreaLayer_[i])) ?
          1 : 0;
}

const char* defiPinAntennaModel::APinGateAreaLayer(int i) const {
  return this->APinGateAreaLayer_[i];
}

int defiPinAntennaModel::APinMaxAreaCar(int i) const {
  return this->APinMaxAreaCar_[i];
}

int defiPinAntennaModel::hasAPinMaxAreaCarLayer(int i) const {
  return (this->APinMaxAreaCarLayer_[i] && *(this->APinMaxAreaCarLayer_[i])) ?
          1 : 0;
}

const char* defiPinAntennaModel::APinMaxAreaCarLayer(int i) const {
  return this->APinMaxAreaCarLayer_[i];
}

int defiPinAntennaModel::APinMaxSideAreaCar(int i) const {
  return this->APinMaxSideAreaCar_[i];
}

int defiPinAntennaModel::hasAPinMaxSideAreaCarLayer(int i) const {
  return (this->APinMaxSideAreaCarLayer_[i] &&
          *(this->APinMaxSideAreaCarLayer_[i])) ? 1 : 0;
}

const char* defiPinAntennaModel::APinMaxSideAreaCarLayer(int i) const {
  return this->APinMaxSideAreaCarLayer_[i];
}

int defiPinAntennaModel::APinMaxCutCar(int i) const {
  return this->APinMaxCutCar_[i];
}

int defiPinAntennaModel::hasAPinMaxCutCarLayer(int i) const {
  return (this->APinMaxCutCarLayer_[i] &&
          *(this->APinMaxCutCarLayer_[i])) ? 1 : 0;
}

const char* defiPinAntennaModel::APinMaxCutCarLayer(int i) const {
  return this->APinMaxCutCarLayer_[i];
}


///////////////////////////////////////////////
///////////////////////////////////////////////
//
//     defiPinPort
//
///////////////////////////////////////////////
///////////////////////////////////////////////

defiPinPort::defiPinPort() {
  this->defiPinPort::Init();
}

void defiPinPort::Init() {
  this->layersAllocated_ = 0;
  this->numLayers_ = 0;
  this->layers_ = 0;
  this->layerMinSpacing_ = 0;
  this->layerMask_ = 0;
  this->layerEffectiveWidth_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->polysAllocated_ = 0;
  this->numPolys_ = 0;
  this->polygonNames_ = 0;
  this->polyMinSpacing_ = 0;
  this->polyMask_ = 0;
  this->polyEffectiveWidth_ = 0;
  this->polygons_ = 0;
  this->viasAllocated_ = 0;
  this->numVias_ = 0;
  this->viaNames_ = 0;
  this->viaX_ = 0;
  this->viaY_ = 0;
  this->viaMask_ = 0;
  this->placeType_ = 0;
  this->x_ = 0;
  this-> y_ = 0;
  this->orient_ = 0;
}

defiPinPort::~defiPinPort() {
  this->defiPinPort::clear();
}

void defiPinPort::clear() {
  int i;

  this->placeType_ = 0;
  this->orient_ = 0;
  this->x_ = 0;
  this->y_ = 0;

  if (this->layers_) {
    for (i = 0; i < this->numLayers_; i++)
      if (this->layers_[i]) defFree(this->layers_[i]);
    defFree((char*)(this->layers_));
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    defFree((char*)(this->layerMinSpacing_));
    defFree((char*)(this->layerMask_));
    defFree((char*)(this->layerEffectiveWidth_));
  }
  this->layers_ = 0;
  this->layerMinSpacing_ = 0;
  this->layerEffectiveWidth_ = 0;
  this->layerMask_ = 0;
  this->numLayers_ = 0;
  this->layersAllocated_ = 0;
  if (this->polygonNames_) {
    struct defiPoints* p;
    for (i = 0; i < this->numPolys_; i++) {
      if (this->polygonNames_[i]) defFree((char*)(this->polygonNames_[i]));
      p = this->polygons_[i];
      defFree((char*)(p->x));
      defFree((char*)(p->y));
      defFree((char*)(this->polygons_[i]));
    }
    defFree((char*)(this->polygonNames_));
    defFree((char*)(this->polygons_));
    defFree((char*)(this->polyMinSpacing_));
    defFree((char*)(this->polyMask_));
    defFree((char*)(this->polyEffectiveWidth_));
    this->polygonNames_ = 0;
    this->polygons_ = 0;
    this->polyMinSpacing_ = 0;
    this->polyEffectiveWidth_ = 0;
    this->polyMask_ = 0;
  }
  this->numPolys_ = 0;
  this->polysAllocated_ = 0;
  if (this->viaNames_) {
    for (i = 0; i < this->numVias_; i++)
      if (this->viaNames_[i]) defFree(this->viaNames_[i]);
    defFree((char*)(this->viaNames_));
    defFree((char*)(this->viaX_));
    defFree((char*)(this->viaY_));
    defFree((char*)(this->viaMask_));
  }
  this->viaNames_ = 0;
  this->numVias_ = 0;
  this->viasAllocated_ = 0;
  this->viaMask_ = 0;
}

void defiPinPort::addLayer(const char* layer) {
  if (this->numLayers_ >= this->layersAllocated_) {
    int i;
    char** newl;
    int *nxl, *nyl, *nxh, *nyh;
    int *lms, *lew, *lm;

    this->layersAllocated_ = this->layersAllocated_ ?
                             this->layersAllocated_ * 2 : 8;
    newl = (char**)defMalloc(this->layersAllocated_ * sizeof(char*));
    nxl = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    nyl = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    nxh = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    nyh = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    lms = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    lew = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    lm = (int*)defMalloc(this->layersAllocated_ * sizeof(int));

    for (i = 0; i < this->numLayers_; i++) {
       newl[i] = this->layers_[i];
       nxl[i] = this->xl_[i];
       nyl[i] = this->yl_[i];
       nxh[i] = this->xh_[i];
       nyh[i] = this->yh_[i];
       lms[i] = this->layerMinSpacing_[i];
       lew[i] = this->layerEffectiveWidth_[i];
       lm[i] = this->layerMask_[i];
    }
    if (this->numLayers_ > 0) {
       defFree((char*)this->layers_);
       defFree((char*)this->xl_);
       defFree((char*)this->yl_);
       defFree((char*)this->xh_);
       defFree((char*)this->yh_);
       defFree((char*)this->layerMinSpacing_);
       defFree((char*)this->layerEffectiveWidth_);
       defFree((char*)this->layerMask_);
    }
    this->layers_ = newl;
    this->xl_ = nxl;
    this->yl_ = nyl;
    this->xh_ = nxh;
    this->yh_ = nyh;
    this->layerMinSpacing_ = lms;
    this->layerEffectiveWidth_ = lew;
    this->layerMask_ = lm;
  }
  this->layers_[this->numLayers_] = (char*)defMalloc(strlen(layer)+1);
  strcpy(this->layers_[this->numLayers_], DEFCASE(layer));
  this->xl_[this->numLayers_] = 0;
  this->yl_[this->numLayers_] = 0;
  this->xh_[this->numLayers_] = 0;
  this->yh_[this->numLayers_] = 0;
  this->layerMinSpacing_[this->numLayers_] = -1;
  this->layerEffectiveWidth_[this->numLayers_] = -1;
  this->layerMask_[this->numLayers_] = 0;
  this->numLayers_ += 1;
}

void defiPinPort::addLayerSpacing(int minSpacing) {
  this->layerMinSpacing_[this->numLayers_-1] = minSpacing;
}

void defiPinPort::addLayerMask(int mask) {
  this->layerMask_[this->numLayers_-1] = mask;
}

void defiPinPort::addLayerDesignRuleWidth(int effectiveWidth) {
  this->layerEffectiveWidth_[this->numLayers_-1] = effectiveWidth;
}

void defiPinPort::addLayerPts(int xl, int yl, int xh, int yh) {
  this->xl_[this->numLayers_-1] = xl;
  this->yl_[this->numLayers_-1] = yl;
  this->xh_[this->numLayers_-1] = xh;
  this->yh_[this->numLayers_-1] = yh;
}

void defiPinPort::addPolygon(const char* layerName) {
  int *pms, *pdw, *pm;
  int i;

  if (this->numPolys_ == this->polysAllocated_) {
    char** newn;
    struct defiPoints** poly;
    this->polysAllocated_ = (this->polysAllocated_ == 0) ?
          2 : this->polysAllocated_ * 2;
    newn = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->polysAllocated_);
    pms = (int*)defMalloc(this->polysAllocated_ * sizeof(int));
    pdw = (int*)defMalloc(this->polysAllocated_ * sizeof(int));
    pm = (int*)defMalloc(this->polysAllocated_ * sizeof(int));

    for (i = 0; i < this->numPolys_; i++) {
      newn[i] = this->polygonNames_[i];
      poly[i] = this->polygons_[i];
      pms[i]  = this->polyMinSpacing_[i];
      pdw[i]  = this->polyEffectiveWidth_[i];
      pm[i] = this->polyMask_[i];
    }
    if (this->numPolys_ > 0) {
      defFree((char*)(this->polygons_));
      defFree((char*)(this->polygonNames_));
      defFree((char*)(this->polyMinSpacing_));
      defFree((char*)(this->polyEffectiveWidth_));
      defFree((char*)(this->polyMask_));
    }
    this->polygonNames_ = newn;
    this->polygons_ = poly;
    this->polyMinSpacing_ = pms;
    this->polyEffectiveWidth_ = pdw;
    this->polyMask_ = pm;
  }
  this->polygonNames_[this->numPolys_] = strdup(layerName);
  this->polygons_[this->numPolys_] = 0;
  this->polyMinSpacing_[this->numPolys_] = -1;
  this->polyEffectiveWidth_[this->numPolys_] = -1;
  this->polyMask_[this->numPolys_] = 0;
  this->numPolys_ += 1;
}

void defiPinPort::addPolySpacing(int minSpacing) {
  this->polyMinSpacing_[this->numPolys_-1] = minSpacing;
}

void defiPinPort::addPolyMask(int color) {
    this->polyMask_[this->numPolys_-1] = color;
}

void defiPinPort::addPolyDesignRuleWidth(int effectiveWidth) {
  this->polyEffectiveWidth_[this->numPolys_-1] = effectiveWidth;
}

void defiPinPort::addPolygonPts(defiGeometries* geom) {
  struct defiPoints* p;
  int x, y;
  int i;

  p = (struct defiPoints*)defMalloc(sizeof(struct defiPoints));
  p->numPoints = geom->defiGeometries::numPoints();
  p->x = (int*)defMalloc(sizeof(int)*p->numPoints);
  p->y = (int*)defMalloc(sizeof(int)*p->numPoints);
  for (i = 0; i < p->numPoints; i++) {
    geom->defiGeometries::points(i, &x, &y);
    p->x[i] = x;
    p->y[i] = y;
  }
  this->polygons_[this->numPolys_-1] = p;
}


void defiPinPort::addVia(const char* viaName, int ptX, int ptY, int color) {
  if (this->numVias_ >= this->viasAllocated_) {
    int i;
    char** newl;
    int *nx, *ny, *nm;

    this->viasAllocated_ = this->viasAllocated_ ?
                           this->viasAllocated_ * 2 : 8;
    newl = (char**)defMalloc(this->viasAllocated_ * sizeof(char*));
    nx = (int*)defMalloc(this->viasAllocated_ * sizeof(int));
    ny = (int*)defMalloc(this->viasAllocated_ * sizeof(int));
    nm = (int*)defMalloc(this->viasAllocated_ * sizeof(int));
    for (i = 0; i < this->numVias_; i++) {
       newl[i] = this->viaNames_[i];
       nx[i] = this->viaX_[i];
       ny[i] = this->viaY_[i];
       nm[i] = this->viaMask_[i];
    }
    if (this->numVias_ > 0) {
       defFree((char*)this->viaNames_);
       defFree((char*)this->viaX_);
       defFree((char*)this->viaY_);
       defFree((char*)this->viaMask_);
    }
    this->viaNames_ = newl;
    this->viaX_ = nx;
    this->viaY_ = ny;
    this->viaMask_ = nm;
  }
  this->viaNames_[this->numVias_] = (char*)defMalloc(strlen(viaName)+1);
  strcpy(this->viaNames_[this->numVias_], DEFCASE(viaName));
  this->viaX_[this->numVias_] = ptX;
  this->viaY_[this->numVias_] = ptY;
  this->viaMask_[this->numVias_] = color;
  this->numVias_ += 1;
}

void defiPinPort::setPlacement(int typ, int x, int y, int orient) {
  this->x_ = x;
  this->y_ = y;
  this->orient_ = orient;
  this->placeType_ = typ;
}

int defiPinPort::numLayer() const {
  return this->numLayers_;
}

const char* defiPinPort::layer(int index) const {
  return this->layers_[index];
}

void defiPinPort::bounds(int index, int* xl, int* yl, int* xh, int* yh) const {
  if (xl) *xl = this->xl_[index];
  if (yl) *yl = this->yl_[index];
  if (xh) *xh = this->xh_[index];
  if (yh) *yh = this->yh_[index];
}

int defiPinPort::hasLayerSpacing(int index) const{
  if (this->layerMinSpacing_[index] == -1)
    return 0;
  return 1;
}

int defiPinPort::hasLayerDesignRuleWidth(int index) const{
  if (this->layerEffectiveWidth_[index] == -1)
    return 0;
  return 1;
}

int defiPinPort::layerSpacing(int index) const {
  return this->layerMinSpacing_[index];
}

int defiPinPort::layerMask(int index) const {
    return this->layerMask_[index];
}

int defiPinPort::layerDesignRuleWidth(int index) const {
  return this->layerEffectiveWidth_[index];
}

int defiPinPort::numPolygons() const {
  return this->numPolys_;
}

const char* defiPinPort::polygonName(int index) const {
  if (index < 0 || index > this->numPolys_) {
    defiError(1, 0, "index out of bounds");
    return 0;
  }
  return this->polygonNames_[index];
}

struct defiPoints defiPinPort::getPolygon(int index) const {
  return *(this->polygons_[index]);
}

int defiPinPort::hasPolygonSpacing(int index) const{
  if (this->polyMinSpacing_[index] == -1)
    return 0;
  return 1;
}

int defiPinPort::hasPolygonDesignRuleWidth(int index) const{
  if (this->polyEffectiveWidth_[index] == -1)
    return 0;
  return 1;
}

int defiPinPort::polygonSpacing(int index) const {
  return this->polyMinSpacing_[index];
}

int defiPinPort::polygonMask(int index) const {
    return this->polyMask_[index];
}

int defiPinPort::polygonDesignRuleWidth(int index) const {
  return this->polyEffectiveWidth_[index];
}

int defiPinPort::numVias() const {
    return this->numVias_;
}

const char* defiPinPort::viaName(int index) const {
  if (index < 0 || index > this->numVias_) {
    defiError(1, 0, "index out of bounds");
    return 0;
  }
  return this->viaNames_[index];
}

int defiPinPort::viaPtX(int index) const {
  return this->viaX_[index];
}

int defiPinPort::viaPtY(int index) const {
  return this->viaY_[index];
}

int defiPinPort::viaBottomMask(int index) const {
    return this->viaMask_[index] % 10;
}

int defiPinPort::viaTopMask(int index) const {
    return this->viaMask_[index] / 100;
}

int defiPinPort::viaCutMask(int index) const {
    return this->viaMask_[index] / 10 % 10;
}

int defiPinPort::hasPlacement() const {
  return this->placeType_ == 0 ? 0 : 1 ;
}

int defiPinPort::isPlaced() const {
  return this->placeType_ == DEFI_COMPONENT_PLACED ? 1 : 0 ;
}

int defiPinPort::isCover() const {
  return this->placeType_ == DEFI_COMPONENT_COVER ? 1 : 0 ;
}

int defiPinPort::isFixed() const {
  return this->placeType_ == DEFI_COMPONENT_FIXED ? 1 : 0 ;
}

int defiPinPort::placementX() const {
  return this->x_;
}

int defiPinPort::placementY() const {
  return this->y_;
}

int defiPinPort::orient() const {
  return this->orient_;
}

const char* defiPinPort::orientStr() const {
  return (defiOrientStr(this->orient_));
}

///////////////////////////////////////////////
///////////////////////////////////////////////
//
//     defiPin
//
///////////////////////////////////////////////
///////////////////////////////////////////////

defiPin::defiPin() {
  this->defiPin::Init();
}


void defiPin::Init() {
  this->pinNameLength_ = 0;
  this->pinName_ = 0;
  this->netNameLength_ = 0;
  this->netName_ = 0;
  this->useLength_ = 0;
  this->use_ = 0;
  this->directionLength_ = 0;
  this->direction_ = 0;
  this->hasDirection_ = 0;
  this->hasUse_ = 0;
  this->placeType_ = 0;
  this->orient_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->x_ = 0;
  this->y_ = 0;
  this->netExprLength_ = 0;                         // 5.6
  this->netExpr_ = 0;                               // 5.6
  this->hasNetExpr_ = 0;                            // 5.6
  this->supplySensLength_ = 0;                      // 5.6
  this->supplySens_ = 0;                            // 5.6
  this->hasSupplySens_ = 0;                         // 5.6
  this->groundSensLength_ = 0;                      // 5.6
  this->groundSens_ = 0;                            // 5.6
  this->hasGroundSens_ = 0;                         // 5.6
  this->layers_ = 0;                                // 5.6
  this->layersAllocated_ = 0;                       // 5.6
  this->numLayers_ = 0;                             // 5.6
  this->polygonNames_ = 0;                          // 5.6
  this->numPolys_ = 0;                              // 5.6
  this->polysAllocated_ = 0;                        // 5.6
  this->polygons_ = 0;                              // 5.6
  this->numAPinPartialMetalArea_ = 0;               // 5.4
  this->APinPartialMetalAreaAllocated_ = 0;         // 5.4
  this->APinPartialMetalArea_ = 0;
  this->APinPartialMetalAreaLayer_ = 0;
  this->numAPinPartialMetalSideArea_ = 0;           // 5.4
  this->APinPartialMetalSideAreaAllocated_ = 0;     // 5.4
  this->APinPartialMetalSideArea_ = 0;
  this->APinPartialMetalSideAreaLayer_ = 0;
  this->numAPinDiffArea_ = 0;                       // 5.4
  this->APinDiffAreaAllocated_ = 0;                 // 5.4
  this->APinDiffArea_ = 0;
  this->APinDiffAreaLayer_ = 0;
  this->numAPinPartialCutArea_ = 0;                 // 5.4
  this->APinPartialCutAreaAllocated_ = 0;           // 5.4
  this->APinPartialCutArea_ = 0;
  this->APinPartialCutAreaLayer_ = 0;
  this->antennaModel_ = 0;
  this->viaNames_ = 0;                              // 5.7
  this->viasAllocated_ = 0;                         // 5.7
  this->numVias_ = 0;                               // 5.7
  this->viaX_ = 0;                                  // 5.7
  this->viaY_ = 0;                                  // 5.7
  this->numPorts_ = 0;                              // 5.7
  this->pinPort_ = 0;                               // 5.7
  this->numAntennaModel_ = 0;
  this->antennaModelAllocated_ = 0;
}


defiPin::~defiPin() {
  this->defiPin::Destroy();
}


void defiPin::clear() {
  int i;
  defiPinAntennaModel* aModel;

  this->hasDirection_ = 0;
  this->hasNetExpr_ = 0;
  this->hasSupplySens_ = 0;
  this->hasGroundSens_ = 0;
  this->hasUse_ = 0;
  this->hasSpecial_ = 0;
  this->placeType_ = 0;
  this->orient_ = 0;
  this->x_ = 0;
  this->y_ = 0;

  if (this->layers_) {
    for (i = 0; i < this->numLayers_; i++)
      if (this->layers_[i]) defFree(this->layers_[i]);
    defFree((char*)(this->layers_));
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    defFree((char*)(this->layerMinSpacing_));
    defFree((char*)(this->layerMask_));
    defFree((char*)(this->layerEffectiveWidth_));
  }
  this->layers_ = 0;
  this->layerMinSpacing_ = 0;
  this->layerMask_ = 0;
  this->layerEffectiveWidth_ = 0;
  this->numLayers_ = 0;
  this->layersAllocated_ = 0;
  // 5.6
  if (this->polygonNames_) {
    struct defiPoints* p;
    for (i = 0; i < this->numPolys_; i++) {
      if (this->polygonNames_[i]) defFree((char*)(this->polygonNames_[i]));
      p = this->polygons_[i];
      defFree((char*)(p->x));
      defFree((char*)(p->y));
      defFree((char*)(this->polygons_[i])); 
    }
    defFree((char*)(this->polygonNames_));
    defFree((char*)(this->polygons_)); 
    defFree((char*)(this->polyMinSpacing_)); 
    defFree((char*)(this->polyMask_));
    defFree((char*)(this->polyEffectiveWidth_)); 
    this->polygonNames_ = 0;
    this->polygons_ = 0;
    this->polyMinSpacing_ = 0;
    this->polyMask_ = 0;
    this->polyEffectiveWidth_ = 0;
  }
  this->numPolys_ = 0;
  this->polysAllocated_ = 0;
  // 5.7
  if (this->viaNames_) {
    for (i = 0; i < this->numVias_; i++)
      if (this->viaNames_[i]) defFree(this->viaNames_[i]);
    defFree((char*)(this->viaNames_));
    defFree((char*)(this->viaX_));
    defFree((char*)(this->viaY_));
    defFree((char*)(this->viaMask_));
  }
  this->viaNames_ = 0;
  this->numVias_ = 0;
  this->viaMask_ = 0;
  this->viasAllocated_ = 0;
  // 5.7
  if (this->pinPort_) {
    for (i = 0; i < this->numPorts_; i++) {
      if (this->pinPort_[i]) {
         this->pinPort_[i]->defiPinPort::clear();
         defFree(this->pinPort_[i]);
      }
    }
    defFree(this->pinPort_);
  }
  this->pinPort_ = 0;
  this->numPorts_ = 0;
  this->portsAllocated_ = 0;

  for (i = 0; i < this->numAPinPartialMetalArea_; i++) {
    if (this->APinPartialMetalAreaLayer_[i])
       defFree(this->APinPartialMetalAreaLayer_[i]);
  }
  this->numAPinPartialMetalArea_ = 0;

  for (i = 0; i < this->numAPinPartialMetalSideArea_; i++) {
    if (this->APinPartialMetalSideAreaLayer_[i])
       defFree(this->APinPartialMetalSideAreaLayer_[i]);
  }
  this->numAPinPartialMetalSideArea_ = 0;

  for (i = 0; i < this->numAPinDiffArea_; i++) {
    if (this->APinDiffAreaLayer_[i])
       defFree(this->APinDiffAreaLayer_[i]);
  }
  this->numAPinDiffArea_ = 0;

  for (i = 0; i < this->numAPinPartialCutArea_; i++) {
    if (this->APinPartialCutAreaLayer_[i])
       defFree(this->APinPartialCutAreaLayer_[i]);
  }
  this->numAPinPartialCutArea_ = 0;

  for (i = 0; i < this->antennaModelAllocated_; i++) { // 5.5
    aModel = this->antennaModel_[i];
    if (i < this->numAntennaModel_) {
       // 1/21/2003 -  pcr 495084, free data in antennaModel
       aModel->defiPinAntennaModel::Destroy();
    }
    defFree(aModel);
  }
  this->numAntennaModel_ = 0;
  this->antennaModelAllocated_ = 0;
}


void defiPin::Destroy() {
  if (this->pinName_) defFree(this->pinName_);
  if (this->netName_) defFree(this->netName_);
  if (this->use_) defFree(this->use_);
  if (this->direction_) defFree(this->direction_);
  if (this->netExpr_) defFree(this->netExpr_);
  if (this->supplySens_) defFree(this->supplySens_);
  if (this->groundSens_) defFree(this->groundSens_);
  this->pinName_ = 0;
  this->netName_ = 0;
  this->use_ = 0;
  this->direction_ = 0;
  this->netExpr_ = 0;
  this->supplySens_ = 0;
  this->groundSens_ = 0;
  this->pinNameLength_ = 0;
  this->netNameLength_ = 0;
  this->useLength_ = 0;
  this->directionLength_ = 0;
  this->netExprLength_ = 0;
  this->supplySensLength_ = 0;
  this->groundSensLength_ = 0;
  this->layersAllocated_ = 0;
  this->defiPin::clear();

  // 5.4
  if (this->APinPartialMetalArea_)
     defFree((char*)(this->APinPartialMetalArea_));
  if (this->APinPartialMetalAreaLayer_)
     defFree((char*)(this->APinPartialMetalAreaLayer_));
  if (this->APinPartialMetalSideArea_)
     defFree((char*)(this->APinPartialMetalSideArea_));
  if (this->APinPartialMetalSideAreaLayer_)
     defFree((char*)(this->APinPartialMetalSideAreaLayer_));
  if (this->APinDiffArea_)
     defFree((char*)(this->APinDiffArea_));
  if (this->APinDiffAreaLayer_)
     defFree((char*)(this->APinDiffAreaLayer_));
  if (this->APinPartialCutArea_)
     defFree((char*)(this->APinPartialCutArea_));
  if (this->APinPartialCutAreaLayer_)
     defFree((char*)(this->APinPartialCutAreaLayer_));
  if (this->antennaModel_)
     defFree((char*)(this->antennaModel_));
}


void defiPin::Setup(const char* pinName, const char* netName) {
  int len = strlen(pinName) + 1;
  if (this->pinNameLength_ < len) {
    if (this->pinName_) defFree(this->pinName_);
    this->pinName_ = (char*)defMalloc(len);
    this->pinNameLength_ = len;
  }
  strcpy(this->pinName_, DEFCASE(pinName));

  len = strlen(netName) + 1;
  if (this->netNameLength_ < len) {
    if (this->netName_) defFree(this->netName_);
    this->netName_ = (char*)defMalloc(len);
    this->netNameLength_ = len;
  }
  strcpy(this->netName_, DEFCASE(netName));

  this->defiPin::clear();

}


void defiPin::setDirection(const char* dir) {
  int len = strlen(dir) + 1;
  if (this->directionLength_ < len) {
    if (this->direction_) defFree(this->direction_);
    this->direction_ = (char*)defMalloc(len);
    this->directionLength_ = len;
  }
  strcpy(this->direction_, DEFCASE(dir));
  this->hasDirection_ = 1;
}


void defiPin::setNetExpr(const char* name) {
  int len = strlen(name) + 1;
  if (this->netExprLength_ < len) {
    if (this->netExpr_) defFree(this->netExpr_);
    this->netExpr_ = (char*)defMalloc(len);
    this->netExprLength_ = len;
  }
  strcpy(this->netExpr_, DEFCASE(name));
  this->hasNetExpr_ = 1;
}


void defiPin::setSupplySens(const char* name) {
  int len = strlen(name) + 1;
  if (this->supplySensLength_ < len) {
    if (this->supplySens_) defFree(this->supplySens_);
    this->supplySens_ = (char*)defMalloc(len);
    this->supplySensLength_ = len;
  }
  strcpy(this->supplySens_, DEFCASE(name));
  this->hasSupplySens_ = 1;
}


void defiPin::setGroundSens(const char* name) {
  int len = strlen(name) + 1;
  if (this->groundSensLength_ < len) {
    if (this->groundSens_) defFree(this->groundSens_);
    this->groundSens_ = (char*)defMalloc(len);
    this->groundSensLength_ = len;
  }
  strcpy(this->groundSens_, DEFCASE(name));
  this->hasGroundSens_ = 1;
}


void defiPin::setUse(const char* use) {
  int len = strlen(use) + 1;
  if (this->useLength_ < len) {
    if (this->use_) defFree(this->use_);
    this->use_ = (char*)defMalloc(len);
    this->useLength_ = len;
  }
  strcpy(this->use_, DEFCASE(use));
  this->hasUse_ = 1;
}


// 5.6, renamed from setLayer to addLayer for multiple layers allowed
void defiPin::addLayer(const char* layer) {

  if (this->numLayers_ >= this->layersAllocated_) {
    int i;
    char** newl;
    int *nxl, *nyl, *nxh, *nyh;
    int *lms, *lew, *lm;

    this->layersAllocated_ = this->layersAllocated_ ?
                             this->layersAllocated_ * 2 : 8;
    newl = (char**)defMalloc(this->layersAllocated_ * sizeof(char*));
    nxl = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    nyl = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    nxh = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    nyh = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    lms = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    lew = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
    lm = (int*)defMalloc(this->layersAllocated_ * sizeof(int));
 
    for (i = 0; i < this->numLayers_; i++) {
       newl[i] = this->layers_[i];
       nxl[i] = this->xl_[i];
       nyl[i] = this->yl_[i];
       nxh[i] = this->xh_[i];
       nyh[i] = this->yh_[i];
       lms[i] = this->layerMinSpacing_[i];
       lew[i] = this->layerEffectiveWidth_[i];
       lm[i] = this->layerMask_[i];
    }
    if (this->numLayers_ > 0) {
       defFree((char*)this->layers_);
       defFree((char*)this->xl_);
       defFree((char*)this->yl_);
       defFree((char*)this->xh_);
       defFree((char*)this->yh_);
       defFree((char*)this->layerMinSpacing_);
       defFree((char*)this->layerMask_);
       defFree((char*)this->layerEffectiveWidth_);
    }
    this->layers_ = newl;
    this->xl_ = nxl;
    this->yl_ = nyl;
    this->xh_ = nxh;
    this->yh_ = nyh;
    this->layerMinSpacing_ = lms;
    this->layerEffectiveWidth_ = lew;
    this->layerMask_ = lm;
  }
  this->layers_[this->numLayers_] = (char*)defMalloc(strlen(layer)+1);
  strcpy(this->layers_[this->numLayers_], DEFCASE(layer));
  this->xl_[this->numLayers_] = 0;
  this->yl_[this->numLayers_] = 0;
  this->xh_[this->numLayers_] = 0;
  this->yh_[this->numLayers_] = 0;
  this->layerMinSpacing_[this->numLayers_] = -1;
  this->layerMask_[this->numLayers_] = 0;
  this->layerEffectiveWidth_[this->numLayers_] = -1;
  this->numLayers_ += 1;
}

// 5.6
void defiPin::addLayerPts(int xl, int yl, int xh, int yh) {
  this->xl_[this->numLayers_-1] = xl;
  this->yl_[this->numLayers_-1] = yl;
  this->xh_[this->numLayers_-1] = xh;
  this->yh_[this->numLayers_-1] = yh;
}

// 5.6
void defiPin::addLayerSpacing(int minSpacing) {
  this->layerMinSpacing_[this->numLayers_-1] = minSpacing;
}

void defiPin::addLayerMask(int mask) {
    this->layerMask_[this->numLayers_-1] = mask;
}

// 5.6
void defiPin::addLayerDesignRuleWidth(int effectiveWidth) {
  this->layerEffectiveWidth_[this->numLayers_-1] = effectiveWidth;
}


void defiPin::setPlacement(int typ, int x, int y, int orient) {
  this->x_ = x;
  this->y_ = y;
  this->orient_ = orient;
  this->placeType_ = typ;
}


const char* defiPin::pinName() const {
  return this->pinName_;
}


const char* defiPin::netName() const {
  return this->netName_;
}


void defiPin::changePinName(const char* pinName) {
  int len = strlen(pinName) + 1;
  if (this->pinNameLength_ < len) {
    if (this->pinName_) defFree(this->pinName_);
    this->pinName_ = (char*)defMalloc(len);
    this->pinNameLength_ = len;
  }
  strcpy(this->pinName_, DEFCASE(pinName));
}


int defiPin::hasDirection() const {
  return (int)(this->hasDirection_);
}


int defiPin::hasUse() const {
  return (int)(this->hasUse_);
}


int defiPin::hasLayer() const {
  if (this->numLayers_ || this->numPolys_)  // 5.6, either layer or polygon is
    return 1;                               // non-zero
  else
    return 0;
}


int defiPin::hasPlacement() const {
  return this->placeType_ == 0 ? 0 : 1 ;
}


int defiPin::isUnplaced() const {
  return this->placeType_ == DEFI_COMPONENT_UNPLACED ? 1 : 0 ;
}


int defiPin::isPlaced() const {
  return this->placeType_ == DEFI_COMPONENT_PLACED ? 1 : 0 ;
}


int defiPin::isCover() const {
  return this->placeType_ == DEFI_COMPONENT_COVER ? 1 : 0 ;
}


int defiPin::isFixed() const {
  return this->placeType_ == DEFI_COMPONENT_FIXED ? 1 : 0 ;
}


int defiPin::placementX() const {
  return this->x_;
}


int defiPin::placementY() const {
  return this->y_;
}


const char* defiPin::direction() const {
  return this->direction_;
}


const char* defiPin::use() const {
  return this->use_;
}


int defiPin::numLayer() const {
  return this->numLayers_;
}

const char* defiPin::layer(int index) const {
  return this->layers_[index];
}


void defiPin::bounds(int index, int* xl, int* yl, int* xh, int* yh) const {
  if (xl) *xl = this->xl_[index];
  if (yl) *yl = this->yl_[index];
  if (xh) *xh = this->xh_[index];
  if (yh) *yh = this->yh_[index];
}

// 5.6
int defiPin::hasLayerSpacing(int index) const{
  if (this->layerMinSpacing_[index] == -1)
    return 0;
  return 1;
}

// 5.6
int defiPin::hasLayerDesignRuleWidth(int index) const{
  if (this->layerEffectiveWidth_[index] == -1)
    return 0;
  return 1;
}

// 5.6
int defiPin::layerSpacing(int index) const {
  return this->layerMinSpacing_[index];
}

int defiPin::layerMask(int index) const {
    return this->layerMask_[index];
}

// 5.6
int defiPin::layerDesignRuleWidth(int index) const {
  return this->layerEffectiveWidth_[index];
}

int defiPin::orient() const {
  return this->orient_;
}

const char* defiPin::orientStr() const {
  return (defiOrientStr(this->orient_));
}


void defiPin::setSpecial() {
  this->hasSpecial_ = 1;
}

// 5.5
void defiPin::addAntennaModel(int oxide) {
  // For version 5.5 only OXIDE1, OXIDE2, OXIDE3, & OXIDE4
  // are defined within a pin
  defiPinAntennaModel* amo;
  int i;

  if (this->numAntennaModel_ == 0) {   // does not have antennaModel
     if (!this->antennaModel_)         // only need to malloc if it is nill
        this->antennaModel_ = (defiPinAntennaModel**)
                 defMalloc(sizeof(defiPinAntennaModel*)*4);
     this->antennaModelAllocated_ = 4;
     for (i = 0; i < 4; i++) {
        this->antennaModel_[i] = (defiPinAntennaModel*)
                                 defMalloc(sizeof(defiPinAntennaModel));
     }
     this->numAntennaModel_++;
     this->antennaModelAllocated_ = 4;
     amo = this->antennaModel_[0];
  } else {
     amo = this->antennaModel_[this->numAntennaModel_];
     this->numAntennaModel_++;
  }
  amo->defiPinAntennaModel::Init();
  amo->defiPinAntennaModel::setAntennaModel(oxide);
  return;
}

// 5.5
int defiPin::numAntennaModel() const {
  return this->numAntennaModel_;
}

// 5.5
defiPinAntennaModel* defiPin::antennaModel(int index) const {
  return this->antennaModel_[index];
}

void defiPin::addAPinPartialMetalArea(int value, const char* layer) {
  if (this->numAPinPartialMetalArea_ == this->APinPartialMetalAreaAllocated_) {
     int i;
     int max;
     int lim = this->numAPinPartialMetalArea_;
     int* nd;
     char**  nl;

     if (this->APinPartialMetalAreaAllocated_ == 0)
        max = this->APinPartialMetalAreaAllocated_ = 2;
     else
        max = this->APinPartialMetalAreaAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinPartialMetalArea_[i];
        nl[i] = this->APinPartialMetalAreaLayer_[i];
     }
     defFree((char*)(this->APinPartialMetalArea_));
     defFree((char*)(this->APinPartialMetalAreaLayer_));
     this->APinPartialMetalArea_ = nd;
     this->APinPartialMetalAreaLayer_ = nl;
 
  }
  this->APinPartialMetalArea_[this->numAPinPartialMetalArea_] = value;
  if (layer) {
    this->APinPartialMetalAreaLayer_[this->numAPinPartialMetalArea_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinPartialMetalAreaLayer_[this->numAPinPartialMetalArea_],
       DEFCASE(layer));
  } else
    this->APinPartialMetalAreaLayer_[this->numAPinPartialMetalArea_] = NULL;
  this->numAPinPartialMetalArea_ += 1;
}


void defiPin::addAPinPartialMetalSideArea(int value, const char* layer) {
  if (this->numAPinPartialMetalSideArea_ == this->APinPartialMetalSideAreaAllocated_) {
     int i;
     int max;
     int lim = this->numAPinPartialMetalSideArea_;
     int* nd;
     char**  nl;

     if (this->APinPartialMetalSideAreaAllocated_ == 0)
        max = this->APinPartialMetalSideAreaAllocated_ = 2;
     else
        max = this->APinPartialMetalSideAreaAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinPartialMetalSideArea_[i];
        nl[i] = this->APinPartialMetalSideAreaLayer_[i];
     }
     defFree((char*)(this->APinPartialMetalSideArea_));
     defFree((char*)(this->APinPartialMetalSideAreaLayer_));
     this->APinPartialMetalSideArea_ = nd;
     this->APinPartialMetalSideAreaLayer_ = nl;

  }
  this->APinPartialMetalSideArea_[this->numAPinPartialMetalSideArea_] = value;
  if (layer) {
    this->APinPartialMetalSideAreaLayer_[this->numAPinPartialMetalSideArea_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinPartialMetalSideAreaLayer_[this->numAPinPartialMetalSideArea_],
       DEFCASE(layer));
  } else
    this->APinPartialMetalSideAreaLayer_[this->numAPinPartialMetalSideArea_] = NULL;
  this->numAPinPartialMetalSideArea_ += 1;
}


void defiPin::addAPinGateArea(int value, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->defiPin::addAntennaModel(1);
  this->antennaModel_[this->numAntennaModel_-1]->defiPinAntennaModel::addAPinGateArea(value, layer);
}


void defiPin::addAPinDiffArea(int value, const char* layer) {
  if (this->numAPinDiffArea_ == this->APinDiffAreaAllocated_) {
     int i;
     int max;
     int lim = this->numAPinDiffArea_;
     int* nd;
     char**  nl;

     if (this->APinDiffAreaAllocated_ == 0)
        max = this->APinDiffAreaAllocated_ = 2;
     else
        max = this->APinDiffAreaAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinDiffArea_[i];
        nl[i] = this->APinDiffAreaLayer_[i];
     }
     defFree((char*)(this->APinDiffArea_));
     defFree((char*)(this->APinDiffAreaLayer_));
     this->APinDiffArea_ = nd;
     this->APinDiffAreaLayer_ = nl;

  }
  this->APinDiffArea_[this->numAPinDiffArea_] = value;
  if (layer) {
    this->APinDiffAreaLayer_[this->numAPinDiffArea_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinDiffAreaLayer_[this->numAPinDiffArea_],
       DEFCASE(layer));
  } else
    this->APinDiffAreaLayer_[this->numAPinDiffArea_] = NULL;
  this->numAPinDiffArea_ += 1;
}


void defiPin::addAPinMaxAreaCar(int value, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->defiPin::addAntennaModel(1);
  this->antennaModel_[this->numAntennaModel_-1]->defiPinAntennaModel::addAPinMaxAreaCar(value, layer);
}


void defiPin::addAPinMaxSideAreaCar(int value, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->defiPin::addAntennaModel(1);
  this->antennaModel_[this->numAntennaModel_-1]->defiPinAntennaModel::addAPinMaxSideAreaCar(value, layer);
}


void defiPin::addAPinPartialCutArea(int value, const char* layer) {
  if (this->numAPinPartialCutArea_ == this->APinPartialCutAreaAllocated_) {
     int i;
     int max;
     int lim = this->numAPinPartialCutArea_;
     int* nd;
     char**  nl;

     if (this->APinPartialCutAreaAllocated_ == 0)
        max = this->APinPartialCutAreaAllocated_ = 2;
     else
        max = this->APinPartialCutAreaAllocated_ *= 2;
     nd = (int*)defMalloc(sizeof(int)*max);
     nl = (char**)defMalloc(sizeof(char*)*max);
     for (i = 0; i < lim; i++) {
        nd[i] = this->APinPartialCutArea_[i];
        nl[i] = this->APinPartialCutAreaLayer_[i];
     }
     defFree((char*)(this->APinPartialCutArea_));
     defFree((char*)(this->APinPartialCutAreaLayer_));
     this->APinPartialCutArea_ = nd;
     this->APinPartialCutAreaLayer_ = nl;

  }
  this->APinPartialCutArea_[this->numAPinPartialCutArea_] = value;
  if (layer) {
    this->APinPartialCutAreaLayer_[this->numAPinPartialCutArea_] =
       (char*)defMalloc(strlen(layer)+1);
    strcpy(this->APinPartialCutAreaLayer_[this->numAPinPartialCutArea_],
       DEFCASE(layer));
  } else
    this->APinPartialCutAreaLayer_[this->numAPinPartialCutArea_] = NULL;
  this->numAPinPartialCutArea_ += 1;
}


void defiPin::addAPinMaxCutCar(int value, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->defiPin::addAntennaModel(1);
  this->antennaModel_[this->numAntennaModel_-1]->defiPinAntennaModel::addAPinMaxCutCar(value, layer);
}


int defiPin::hasSpecial() const {
  return (int)this->hasSpecial_;
}


int defiPin::hasAPinPartialMetalArea() const {
  return this->numAPinPartialMetalArea_ ? 1 : 0 ;
}


int defiPin::hasAPinPartialMetalSideArea() const {
  return this->numAPinPartialMetalSideArea_ ? 1 : 0 ;
}


int defiPin::hasAPinDiffArea() const {
  return this->numAPinDiffArea_ ? 1 : 0 ;
}


int defiPin::hasAPinPartialCutArea() const {
  return this->numAPinPartialCutArea_ ? 1 : 0 ;
}


int defiPin::numAPinPartialMetalArea() const {
  return this->numAPinPartialMetalArea_;
}


int defiPin::numAPinPartialMetalSideArea() const {
  return this->numAPinPartialMetalSideArea_;
}


int defiPin::numAPinDiffArea() const {
  return this->numAPinDiffArea_;
}


int defiPin::numAPinPartialCutArea() const {
  return this->numAPinPartialCutArea_;
}


int defiPin::APinPartialMetalArea(int i) const {
  return this->APinPartialMetalArea_[i];
}


int defiPin::hasAPinPartialMetalAreaLayer(int i) const {
  return (this->APinPartialMetalAreaLayer_[i] &&
          *(this->APinPartialMetalAreaLayer_[i])) ? 1 : 0 ;
}


const char* defiPin::APinPartialMetalAreaLayer(int i) const {
  return this->APinPartialMetalAreaLayer_[i];
}


int defiPin::APinPartialMetalSideArea(int i) const {
  return this->APinPartialMetalSideArea_[i];
}


int defiPin::hasAPinPartialMetalSideAreaLayer(int i) const {
  return (this->APinPartialMetalSideAreaLayer_[i] &&
          *(this->APinPartialMetalSideAreaLayer_[i])) ? 1 : 0 ;
}


const char* defiPin::APinPartialMetalSideAreaLayer(int i) const {
  return this->APinPartialMetalSideAreaLayer_[i];
}


int defiPin::APinDiffArea(int i) const {
  return this->APinDiffArea_[i];
}


int defiPin::hasAPinDiffAreaLayer(int i) const {
  return (this->APinDiffAreaLayer_[i] && *(this->APinDiffAreaLayer_[i])) ?
          1 : 0 ;
}


const char* defiPin::APinDiffAreaLayer(int i) const {
  return this->APinDiffAreaLayer_[i];
}


int defiPin::APinPartialCutArea(int i) const {
  return this->APinPartialCutArea_[i];
}


int defiPin::hasAPinPartialCutAreaLayer(int i) const {
  return (this->APinPartialCutAreaLayer_[i] &&
          *(this->APinPartialCutAreaLayer_[i])) ? 1 : 0 ;
}


const char* defiPin::APinPartialCutAreaLayer(int i) const {
  return this->APinPartialCutAreaLayer_[i];
}


// 5.6
void defiPin::addPolygon(const char* layerName) {
  int *pms, *pdw, *pm;
  int i;

  if (this->numPolys_ == this->polysAllocated_) {
    char** newn;
    struct defiPoints** poly;
    this->polysAllocated_ = (this->polysAllocated_ == 0) ?
          2 : this->polysAllocated_ * 2;
    newn = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->polysAllocated_);
    pms = (int*)defMalloc(this->polysAllocated_ * sizeof(int));
    pdw = (int*)defMalloc(this->polysAllocated_ * sizeof(int));
    pm = (int*)defMalloc(this->polysAllocated_ * sizeof(int));

    for (i = 0; i < this->numPolys_; i++) {
      newn[i] = this->polygonNames_[i];
      poly[i] = this->polygons_[i];
      pms[i]  = this->polyMinSpacing_[i];
      pdw[i]  = this->polyEffectiveWidth_[i];
      pm[i] = this->polyMask_[i];
    }
    if (this->numPolys_ > 0) {
      defFree((char*)(this->polygons_));
      defFree((char*)(this->polygonNames_));
      defFree((char*)(this->polyMinSpacing_));
      defFree((char*)(this->polyEffectiveWidth_));
      defFree((char*)(this->polyMask_));
    }
    this->polygonNames_ = newn;
    this->polygons_ = poly;
    this->polyMinSpacing_ = pms;
    this->polyEffectiveWidth_ = pdw;
    this->polyMask_= pm;
  }
  this->polygonNames_[this->numPolys_] = strdup(layerName);
  this->polygons_[this->numPolys_] = 0;
  this->polyMinSpacing_[this->numPolys_] = -1;
  this->polyEffectiveWidth_[this->numPolys_] = -1;
  this->polyMask_[this->numPolys_] = 0;
  this->numPolys_ += 1;
}


// 5.6
void defiPin::addPolygonPts(defiGeometries* geom) {
  struct defiPoints* p;
  int x, y;
  int i;

  p = (struct defiPoints*)defMalloc(sizeof(struct defiPoints));
  p->numPoints = geom->defiGeometries::numPoints();
  p->x = (int*)defMalloc(sizeof(int)*p->numPoints);
  p->y = (int*)defMalloc(sizeof(int)*p->numPoints);
  for (i = 0; i < p->numPoints; i++) {
    geom->defiGeometries::points(i, &x, &y);
    p->x[i] = x;
    p->y[i] = y;
  }
  this->polygons_[this->numPolys_-1] = p;
}


// 5.6
void defiPin::addPolySpacing(int minSpacing) {
  this->polyMinSpacing_[this->numPolys_-1] = minSpacing;
}

void defiPin::addPolyMask(int color) {
    this->polyMask_[this->numPolys_-1] = color;
}

// 5.6
void defiPin::addPolyDesignRuleWidth(int effectiveWidth) {
  this->polyEffectiveWidth_[this->numPolys_-1] = effectiveWidth;
}


// 5.6
int defiPin::numPolygons() const {
  return this->numPolys_;
}


// 5.6
const char* defiPin::polygonName(int index) const {
  if (index < 0 || index > this->numPolys_) {
    defiError(1, 0, "index out of bounds");
    return 0;
  }
  return this->polygonNames_[index];
}

// 5.6
struct defiPoints defiPin::getPolygon(int index) const {
  return *(this->polygons_[index]);
}

// 5.6
int defiPin::hasPolygonSpacing(int index) const{
  if (this->polyMinSpacing_[index] == -1)
    return 0;
  return 1;
}

// 5.6
int defiPin::hasPolygonDesignRuleWidth(int index) const{
  if (this->polyEffectiveWidth_[index] == -1)
    return 0;
  return 1;
}

// 5.6
int defiPin::polygonSpacing(int index) const {
  return this->polyMinSpacing_[index];
}

int defiPin::polygonMask(int index) const {
    return this->polyMask_[index];
}

// 5.6
int defiPin::polygonDesignRuleWidth(int index) const {
  return this->polyEffectiveWidth_[index];
}

// 5.6
int defiPin::hasNetExpr() const {
  return (int)(this->hasNetExpr_);
}

// 5.6
const char* defiPin::netExpr() const {
  return this->netExpr_;
}

// 5.6
int defiPin::hasSupplySensitivity() const {
  return (int)(this->hasSupplySens_);
}

// 5.6
const char* defiPin::supplySensitivity() const {
  return this->supplySens_;
}

// 5.6
int defiPin::hasGroundSensitivity() const {
  return (int)(this->hasGroundSens_);
}

// 5.6
const char* defiPin::groundSensitivity() const {
  return this->groundSens_;
}

// 5.7
void defiPin::addVia(const char* viaName, int ptX, int ptY, int color) {

  if (this->numVias_ >= this->viasAllocated_) {
    int i;
    char** newl;
    int *nx, *ny, *nm;

    this->viasAllocated_ = this->viasAllocated_ ?
                           this->viasAllocated_ * 2 : 8;
    newl = (char**)defMalloc(this->viasAllocated_ * sizeof(char*));
    nx = (int*)defMalloc(this->viasAllocated_ * sizeof(int));
    ny = (int*)defMalloc(this->viasAllocated_ * sizeof(int));
    nm = (int*)defMalloc(this->viasAllocated_ * sizeof(int));
 
    for (i = 0; i < this->numVias_; i++) {
       newl[i] = this->viaNames_[i];
       nx[i] = this->viaX_[i];
       ny[i] = this->viaY_[i];
       nm[i] = this->viaMask_[i];
    }
    if (this->numVias_ > 0) {
       defFree((char*)this->viaNames_);
       defFree((char*)this->viaX_);
       defFree((char*)this->viaY_);
       defFree((char*)this->viaMask_);
    }
    this->viaNames_ = newl;
    this->viaX_ = nx;
    this->viaY_ = ny;
    this->viaMask_ = nm;
  }
  this->viaNames_[this->numVias_] = (char*)defMalloc(strlen(viaName)+1);
  strcpy(this->viaNames_[this->numVias_], DEFCASE(viaName));
  this->viaX_[this->numVias_] = ptX;
  this->viaY_[this->numVias_] = ptY;
  this->viaMask_[this->numVias_] = color;
  this->numVias_ += 1;
}

// 5.7
int defiPin::numVias() const {
    return this->numVias_;
}

// 5.7
const char* defiPin::viaName(int index) const {
  if (index < 0 || index > this->numVias_) {
    defiError(1, 0, "index out of bounds");
    return 0;
  }
  return this->viaNames_[index];
}

// 5.7
int defiPin::viaPtX(int index) const {
  return this->viaX_[index];
}

// 5.7
int defiPin::viaPtY(int index) const {
  return this->viaY_[index];
}

int defiPin::viaTopMask(int index) const {
    int cutMaskNum = this->viaMask_[index] / 10;

    if (cutMaskNum) {
        return cutMaskNum /= 10;
    } else {
        return 0;
    }
}

int defiPin::viaCutMask(int index) const {
    int cutMaskNum = this->viaMask_[index] / 10;

    if (cutMaskNum) {
        return cutMaskNum % 10;
    } else {
        return 0;
    }
}

int defiPin::viaBottomMask(int index) const {
    return this->viaMask_[index] % 10;
}
// 5.7
void defiPin::addPort() {
  defiPinPort** pp;
  defiPinPort* pv;
  int i;

  if (this->numPorts_ >= this->portsAllocated_) {
     if (this->portsAllocated_ == 0) {
        this->pinPort_ = (defiPinPort**) defMalloc(sizeof(defiPinPort*)*4);
        this->portsAllocated_ = 4;
     } else {
        this->portsAllocated_ = this->portsAllocated_ * 2;
        pp = (defiPinPort**) defMalloc(sizeof(defiPinPort*) *
             this->portsAllocated_);
        for (i = 0; i < this->numPorts_; i++)
           pp[i] = this->pinPort_[i];
        defFree((char*)(this->pinPort_));
        this->pinPort_ = pp;
     }
  }
  pv = (defiPinPort*)defMalloc(sizeof(defiPinPort));
  pv->defiPinPort::Init();
  this->pinPort_[this->numPorts_] = pv;
  this->numPorts_ += 1;
}

// 5.7
void defiPin::addPortLayer(const char* layer) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addLayer(layer);
}

// 5.7
void defiPin::addPortLayerSpacing(int minSpacing) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addLayerSpacing(minSpacing);
}

void defiPin::addPortLayerMask(int color) {
    int i = this->numPorts_ - 1;
    this->pinPort_[i]->defiPinPort::addLayerMask(color);
}

// 5.7
void defiPin::addPortLayerDesignRuleWidth(int effectiveWidth) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addLayerDesignRuleWidth(effectiveWidth);
}

// 5.7
void defiPin::addPortLayerPts(int xl, int yl, int xh, int yh) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addLayerPts(xl, yl, xh, yh);
}

// 5.7
void defiPin::addPortPolygon(const char* layerName) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addPolygon(layerName);
}

// 5.7
void defiPin::addPortPolySpacing(int minSpacing) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addPolySpacing(minSpacing);
}

void defiPin::addPortPolyMask(int color) {
    int i = this->numPorts_ - 1;
    this->pinPort_[i]->defiPinPort::addPolyMask(color);
}

// 5.7
void defiPin::addPortPolyDesignRuleWidth(int effectiveWidth) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addPolyDesignRuleWidth( effectiveWidth);
}

// 5.7
void defiPin::addPortPolygonPts(defiGeometries* geom) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addPolygonPts(geom);
}

// 5.7
void defiPin::addPortVia(const char* via, int viaX, int viaY, int color) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::addVia(via, viaX, viaY, color);
}

// 5.7
void defiPin::setPortPlacement(int typ, int x, int y, int orient) {
  int i = this->numPorts_ - 1;
  this->pinPort_[i]->defiPinPort::setPlacement(typ, x, y, orient);
}

// 5.7
int defiPin::hasPort() const {
  return this->numPorts_;
}

// 5.7
int defiPin::numPorts() const {
  return this->numPorts_;
}

// 5.7
defiPinPort* defiPin::pinPort(int index) const {
  if (index < 0 || index > this->numPorts_) {
    defiError(1, 0, "index out of bounds");
    return 0;
  }
  return this->pinPort_[index];
}

void defiPin::print(FILE* f) const {
  int xl, yl, xh,yh;
  int i;

  fprintf(f, "PINS '%s' on net '%s'\n", this->defiPin::pinName(),
	    this->defiPin::netName());
  if (this->defiPin::hasDirection())
    fprintf(f, "+ DIRECTION '%s'\n", this->defiPin::direction());
  if (this->defiPin::hasNetExpr())
    fprintf(f, "+ NETEXPR '%s'\n", this->defiPin::netExpr());
  if (this->defiPin::hasSupplySensitivity())
    fprintf(f, "+ SUPPLYSENSITIVITY '%s'\n",
               this->defiPin::supplySensitivity());
  if (this->defiPin::hasGroundSensitivity())
    fprintf(f, "+ GROUNDSENSITIVITY '%s'\n",
               this->defiPin::groundSensitivity());
  if (this->defiPin::hasUse())
    fprintf(f, "+ USE '%s'\n", this->defiPin::use());
  if (this->defiPin::hasLayer()) {
    for (i = 0; i < this->defiPin::numLayer(); i++) {
       this->defiPin::bounds(i, &xl, &yl, &xh, &yh);
       fprintf(f, "+ LAYER '%s' %d %d %d %d\n",
	          this->defiPin::layer(i), xl, yl, xh, yh);
    }
  }
  for (i = 0; i < this->defiPin::numPolygons(); i++) {
    fprintf(f, "+ POLYGON %s", this->defiPin::polygonName(i));
    if (this->defiPin::hasPolygonSpacing(i))
      fprintf(f, " SPACING %g", this->defiPin::polygonSpacing(i));
    if (this->defiPin::hasPolygonDesignRuleWidth(i))
      fprintf(f, " DESIGNRULEWIDTH %g",
              this->defiPin::polygonDesignRuleWidth(i));
  }
  for (i = 0; i < this->defiPin::numVias(); i++) {
    fprintf(f, "+ VIA %s %g %g\n", this->defiPin::viaName(i),
            this->defiPin::viaPtX(i), this->defiPin::viaPtY(i));
  }
  if (this->defiPin::hasPlacement())
    fprintf(f, "  PLACED %s%s%d %d\n",
		this->defiPin::isFixed() ? " FIXED" : "",
		this->defiPin::isCover() ? " COVER" : "",
		this->defiPin::placementX(),
		this->defiPin::placementY());
  if (this->defiPin::hasSpecial())
    fprintf(f, "+ SPECIAL\n");
}
