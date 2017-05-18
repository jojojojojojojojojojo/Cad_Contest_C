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
#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "defiBlockage.hpp"
#include "defiDebug.hpp"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiBlockages
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

defiBlockage::defiBlockage() {
  this->defiBlockage::Init();
}


void defiBlockage::Init() {
  this->numPolys_ = 0;
  this->defiBlockage::clear();
  this->layerName_ = 0;
  this->componentName_ = 0;
  this->layerNameLength_ = 0;
  this->componentNameLength_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->rectsAllocated_ = 0;
  this->polysAllocated_ = 0;
  this->polygons_ = 0;
}


defiBlockage::~defiBlockage() {
  this->defiBlockage::Destroy();
}


void defiBlockage::clear() {
  this->hasLayer_ = 0;
  this->hasPlacement_ = 0;
  this->hasComponent_ = 0;
  this->hasSlots_ = 0;
  this->hasFills_ = 0;
  this->hasPushdown_ = 0;
  this->hasExceptpgnet_ = 0;
  this->hasSoft_ = 0;
  this->maxDensity_ = -1;
  this->minSpacing_ = -1;
  this->width_ = -1;
  this->numRectangles_ = 0;
  this->mask_ = 0;
}


void defiBlockage::clearPoly() {
  struct defiPoints* p;
  int i;

  for (i = 0; i < this->numPolys_; i++) {
    p = this->polygons_[i];
    defFree((char*)(p->x));
    defFree((char*)(p->y));
    defFree((char*)(this->polygons_[i]));
  }
  this->numPolys_ = 0;
}

void defiBlockage::Destroy() {
  if (this->layerName_) defFree(this->layerName_);
  if (this->componentName_) defFree(this->componentName_);
  this->layerName_ = 0;
  this->componentName_ = 0;
  if (this->rectsAllocated_) {
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    this->rectsAllocated_ = 0;
    this->xl_ = 0;
    this->yl_ = 0;
    this->xh_ = 0;
    this->yh_ = 0;
  }
  this->defiBlockage::clearPoly();
  defFree((char*)(this->polygons_));
  this->polygons_ = 0;
  this->defiBlockage::clear();
}


void defiBlockage::setLayer(const char* name) {
  int len = strlen(name) + 1;
  if (this->layerNameLength_ < len) {
    if (this->layerName_) defFree(this->layerName_);
    this->layerName_ = (char*)defMalloc(len);
    this->layerNameLength_ = len;
  }
  strcpy(this->layerName_, DEFCASE(name));
  this->hasLayer_ = 1;
}


void defiBlockage::setPlacement() {

  /* 10/29/2001 - Wanda da Rosa, new enhancement */
  this->hasPlacement_ = 1;
  return;
}

void defiBlockage::setComponent(const char* name) {
  int len;

  /* 10/29/2001 - Wanda da Rosa, component name is required */
  len = strlen(name) + 1;
  if (this->componentNameLength_ < len) {
    if (this->componentName_) defFree(this->componentName_);
    this->componentName_ = (char*)defMalloc(len);
    this->componentNameLength_ = len;
  }
  strcpy(this->componentName_, DEFCASE(name));
  this->hasComponent_ = 1;
}

void defiBlockage::setSlots() {
  this->hasSlots_ = 1;
}

void defiBlockage::setFills() {
  this->hasFills_ = 1;
}

void defiBlockage::setPushdown() {
  this->hasPushdown_ = 1;
}

// 5.7
void defiBlockage::setExceptpgnet() {
  this->hasExceptpgnet_ = 1;
}

// 5.7
void defiBlockage::setSoft() {
  this->hasSoft_ = 1;
}

// 5.7
void defiBlockage::setPartial(double maxDensity) {
  this->maxDensity_ = maxDensity;
}

void defiBlockage::setSpacing(int minSpacing) {
  this->minSpacing_ = minSpacing;
}

void defiBlockage::setDesignRuleWidth(int width) {
  this->width_ = width;
}

void defiBlockage::setMask(int colorMask) {
  this->mask_ = colorMask;
}

void defiBlockage::addRect(int xl, int yl, int xh, int yh) {
  if (this->numRectangles_ == this->rectsAllocated_) {
    int i;
    int max = this->rectsAllocated_ = (this->rectsAllocated_ == 0) ?
          2 : this->rectsAllocated_ * 2;
    int* newxl = (int*)defMalloc(sizeof(int)*max);
    int* newyl = (int*)defMalloc(sizeof(int)*max);
    int* newxh = (int*)defMalloc(sizeof(int)*max);
    int* newyh = (int*)defMalloc(sizeof(int)*max);
    for (i = 0; i < this->numRectangles_; i++) {
      newxl[i] = this->xl_[i];
      newyl[i] = this->yl_[i];
      newxh[i] = this->xh_[i];
      newyh[i] = this->yh_[i];
    }
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    this->xl_ = newxl;
    this->yl_ = newyl;
    this->xh_ = newxh;
    this->yh_ = newyh;
  }
  this->xl_[this->numRectangles_] = xl;
  this->yl_[this->numRectangles_] = yl;
  this->xh_[this->numRectangles_] = xh;
  this->yh_[this->numRectangles_] = yh;
  this->numRectangles_ += 1;
}


// 5.6
void defiBlockage::addPolygon(defiGeometries* geom) {
  struct defiPoints* p;
  int x, y;
  int i;

  if (this->numPolys_ == this->polysAllocated_) {
    struct defiPoints** poly;
    this->polysAllocated_ = (this->polysAllocated_ == 0) ?
          2 : this->polysAllocated_ * 2;
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->polysAllocated_);
    for (i = 0; i < this->numPolys_; i++)
      poly[i] = this->polygons_[i];
    if (this->polygons_)
      defFree((char*)(this->polygons_));
    this->polygons_ = poly;
  }
  p = (struct defiPoints*)defMalloc(sizeof(struct defiPoints));
  p->numPoints = geom->defiGeometries::numPoints();
  p->x = (int*)defMalloc(sizeof(int)*p->numPoints);
  p->y = (int*)defMalloc(sizeof(int)*p->numPoints);
  for (i = 0; i < p->numPoints; i++) {
    geom->defiGeometries::points(i, &x, &y);
    p->x[i] = x;
    p->y[i] = y;
  }
  this->polygons_[this->numPolys_] = p;
  this->numPolys_ += 1;
}


int defiBlockage::hasLayer() const {
  return this->hasLayer_;
}


int defiBlockage::hasPlacement() const {
  return this->hasPlacement_;
}


int defiBlockage::hasComponent() const {
  return this->hasComponent_;
}


int defiBlockage::hasSlots() const {
  return this->hasSlots_;
}


int defiBlockage::hasFills() const {
  return this->hasFills_;
}


int defiBlockage::hasPushdown() const {
  return this->hasPushdown_;
}

// 5.7
int defiBlockage::hasExceptpgnet() const {
  return this->hasExceptpgnet_;
}

// 5.7
int defiBlockage::hasSoft() const {
  return this->hasSoft_;
}

// 5.7
int defiBlockage::hasPartial() const {
  if (this->maxDensity_ == -1)
    return 0;
  return 1;
}

// 5.7
double defiBlockage::placementMaxDensity() const {
  return this->maxDensity_;
}

int defiBlockage::hasSpacing() const {
  if (this->minSpacing_ == -1)
    return 0;
  return 1;
}


int defiBlockage::hasDesignRuleWidth() const {
  if (this->width_ == -1)
    return 0;
  return 1;
}

int defiBlockage::hasMask() const {
    return this->mask_;
}

int defiBlockage::minSpacing() const {
  return this->minSpacing_;
}


int defiBlockage::designRuleWidth() const {
  return this->width_;
}

int defiBlockage::mask() const {
  return this->mask_;
}

const char* defiBlockage::layerName() const {
  return this->layerName_;
}


const char* defiBlockage::layerComponentName() const {
  return this->componentName_;
}


const char* defiBlockage::placementComponentName() const {
  return this->componentName_;
}


int defiBlockage::numRectangles() const {
  return this->numRectangles_;
}


int defiBlockage::xl(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for blockage xl");
    return 0;
  }
  return this->xl_[index];
}


int defiBlockage::yl(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for blockage yl");
    return 0;
  }
  return this->yl_[index];
}


int defiBlockage::xh(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for blockage xh");
    return 0;
  }
  return this->xh_[index];
}


int defiBlockage::yh(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for blockage yh");
    return 0;
  }
  return this->yh_[index];
}


// 5.6
int defiBlockage::numPolygons() const {
  return this->numPolys_;
}


// 5.6
struct defiPoints defiBlockage::getPolygon(int index) const {
  return *(this->polygons_[index]);
}


void defiBlockage::print(FILE* f) const {
  int i, j;
  struct defiPoints points;

  if (this->defiBlockage::hasLayer()) {
    fprintf(f, "- LAYER %s", this->defiBlockage::layerName());
    if (this->defiBlockage::hasComponent())
      fprintf(f, " + COMPONENT %s", this->defiBlockage::layerComponentName());
    if (this->defiBlockage::hasSlots())
      fprintf(f, " + SLOTS");
    if (this->defiBlockage::hasFills())
      fprintf(f, " + FILLS");
    if (this->defiBlockage::hasPushdown())
      fprintf(f, " + PUSHDOWN");
    if (this->defiBlockage::hasExceptpgnet())
      fprintf(f, " + EXCEPTPGNET");
    fprintf(f, "\n");
  }
  if (this->defiBlockage::hasPlacement()) {
    fprintf(f, "- PLACEMENT");
    if (this->defiBlockage::hasComponent())
      fprintf(f, " + COMPONENT %s", this->defiBlockage::layerComponentName());
    if (this->defiBlockage::hasPushdown())
      fprintf(f, " + PUSHDOWN");
    if (this->defiBlockage::hasSoft())
      fprintf(f, " + SOFT");
    if (this->defiBlockage::hasPartial())
      fprintf(f, " + PARTIAL", this->defiBlockage::placementMaxDensity());
    fprintf(f, "\n");
  }

  for (i = 0; i < this->defiBlockage::numRectangles(); i++) {
    fprintf(f, "   RECT %d %d %d %d\n", this->defiBlockage::xl(i),
            this->defiBlockage::yl(i), this->defiBlockage::xh(i),
            this->defiBlockage::yh(i));
  }

  for (i = 0; i < this->defiBlockage::numPolygons(); i++) {
    fprintf(f, "   POLYGON ");
    points = this->defiBlockage::getPolygon(i);
    for (j = 0; j < points.numPoints; j++)
      fprintf(f, "%d %d ", points.x[j], points.y[j]);
    fprintf(f,"\n");
  }
  fprintf(f,"\n");
}
