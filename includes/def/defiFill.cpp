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
#include "defiFill.hpp"
#include "defiDebug.hpp"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiFill
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

defiFill::defiFill() {
  this->defiFill::Init();
}


void defiFill::Init() {
  this->numPolys_ = 0;
  this->numPts_ = 0;
  this->defiFill::clear();
  this->layerNameLength_ = 0;
  this->xl_ = (int*)malloc(sizeof(int)*1);
  this->yl_ = (int*)malloc(sizeof(int)*1);
  this->xh_ = (int*)malloc(sizeof(int)*1);
  this->yh_ = (int*)malloc(sizeof(int)*1);
  this->rectsAllocated_ = 1;      // At least 1 rectangle will define
  this->polysAllocated_ = 0;
  this->polygons_ = 0;
  this->layerName_ = 0;
  this->viaName_ = 0;
  this->viaNameLength_ = 0;
  this->viaPts_ = 0;
  this->ptsAllocated_ = 0;
  this->viaPts_ = 0;

}

defiFill::~defiFill() {
  this->defiFill::Destroy();
}

void defiFill::clear() {
  this->hasLayer_ = 0;
  this->layerOpc_ = 0;
  this->numRectangles_ = 0;
  this->hasVia_ = 0;
  this->viaOpc_ = 0;
  this->mask_ = 0;
}

void defiFill::clearPoly() {
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

void defiFill::clearPts() {
  struct defiPoints* p;
  int i;

  for (i = 0; i < this->numPts_; i++) {
    p = this->viaPts_[i];
    defFree((char*)(p->x));
    defFree((char*)(p->y));
    defFree((char*)(this->viaPts_[i]));
  }
  this->numPts_ = 0;
}

void defiFill::Destroy() {
  if (this->layerName_) defFree(this->layerName_);
  if (this->viaName_) defFree(this->viaName_);
  defFree((char*)(this->xl_));
  defFree((char*)(this->yl_));
  defFree((char*)(this->xh_));
  defFree((char*)(this->yh_));
  this->rectsAllocated_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->defiFill::clearPoly();
  if (this->polygons_) defFree((char*)(this->polygons_));
  this->polygons_ = 0;
  this->defiFill::clearPts();
  if (this->viaPts_) defFree((char*)(this->viaPts_));
  this->viaPts_ = 0;
  this->defiFill::clear();
}


void defiFill::setLayer(const char* name) {
  int len = strlen(name) + 1;
  if (this->layerNameLength_ < len) {
    if (this->layerName_) defFree(this->layerName_);
    this->layerName_ = (char*)malloc(len);
    this->layerNameLength_ = len;
  }
  strcpy(this->layerName_, DEFCASE(name));
  this->hasLayer_ = 1;
}

// 5.7
void defiFill::setLayerOpc() {
  this->layerOpc_ = 1;
}

void defiFill::addRect(int xl, int yl, int xh, int yh) {
  if (this->numRectangles_ == this->rectsAllocated_) {
    int i;
    int max = this->rectsAllocated_ = this->rectsAllocated_ * 2;
    int* newxl = (int*)malloc(sizeof(int)*max);
    int* newyl = (int*)malloc(sizeof(int)*max);
    int* newxh = (int*)malloc(sizeof(int)*max);
    int* newyh = (int*)malloc(sizeof(int)*max);
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
void defiFill::addPolygon(defiGeometries* geom) {
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

int defiFill::hasLayer() const {
  return this->hasLayer_;
}

const char* defiFill::layerName() const {
  return this->layerName_;
}

// 5.7
int defiFill::hasLayerOpc() const {
  return this->layerOpc_;
}

int defiFill::numRectangles() const {
  return this->numRectangles_;
}


int defiFill::xl(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for Fill xl");
    return 0;
  }
  return this->xl_[index];
}


int defiFill::yl(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for Fill yl");
    return 0;
  }
  return this->yl_[index];
}


int defiFill::xh(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for Fill xh");
    return 0;
  }
  return this->xh_[index];
}


int defiFill::yh(int index) const {
  if (index < 0 || index >= this->numRectangles_) {
    defiError(1, 0, "bad index for Fill yh");
    return 0;
  }
  return this->yh_[index];
}

// 5.6
int defiFill::numPolygons() const {
  return this->numPolys_;
}


// 5.6
struct defiPoints defiFill::getPolygon(int index) const {
  return *(this->polygons_[index]);
}

// 5.7
void defiFill::setVia(const char* name) {
  int len = strlen(name) + 1;
  if (this->viaNameLength_ < len) {
    if (this->viaName_) defFree(this->viaName_);
    this->viaName_ = (char*)malloc(len);
    this->viaNameLength_ = len;
  }
  strcpy(this->viaName_, DEFCASE(name));
  this->hasVia_ = 1;
}

// 5.7
void defiFill::setViaOpc() {
  this->viaOpc_ = 1;
}

// 5.8
void defiFill::setMask(int colorMask) {
    this->mask_ = colorMask;
}


// 5.7
void defiFill::addPts(defiGeometries* geom) {
  struct defiPoints* p;
  int x, y;
  int i;

  if (this->numPts_ == this->ptsAllocated_) {
    struct defiPoints** pts;
    this->ptsAllocated_ = (this->ptsAllocated_ == 0) ?
          2 : this->ptsAllocated_ * 2;
    pts= (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->ptsAllocated_);
    for (i = 0; i < this->numPts_; i++)
      pts[i] = this->viaPts_[i];
    if (this->viaPts_)
      defFree((char*)(this->viaPts_));
    this->viaPts_ = pts;
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
  this->viaPts_[this->numPts_] = p;
  this->numPts_ += 1;
}

// 5.7
int defiFill::hasVia() const {
  return this->hasVia_;
}

// 5.7
const char* defiFill::viaName() const {
  return this->viaName_;
}

// 5.7
int defiFill::hasViaOpc() const {
  return this->viaOpc_;
}

// 5.7
int defiFill::numViaPts() const {
  return this->numPts_;
}

// 5.8
int defiFill::layerMask() const {
    return this->mask_;
}

// 5.8
int defiFill::viaTopMask() const {
    return this->mask_ / 100;
}

// 5.8
int defiFill::viaCutMask() const {
    return this->mask_ / 10 % 10;
}

// 5.8
int defiFill::viaBottomMask() const {
    return this->mask_ % 10;
}

// 5.7
struct defiPoints defiFill::getViaPts(int index) const {
  return *(this->viaPts_[index]);
}

void defiFill::print(FILE* f) const {
  int i, j;
  struct defiPoints points;

  if (this->defiFill::hasLayer())
    fprintf(f, "- LAYER %s", this->defiFill::layerName());

  if (this->defiFill::layerMask())
      fprintf(f, " + Mask %d", this->defiFill::layerMask());

  if (this->defiFill::hasLayerOpc())
    fprintf(f, " + OPC");
  fprintf(f, "\n");

  for (i = 0; i < this->defiFill::numRectangles(); i++) {
    fprintf(f, "   RECT %d %d %d %d\n", this->defiFill::xl(i),
            this->defiFill::yl(i), this->defiFill::xh(i),
            this->defiFill::yh(i));
  }

  for (i = 0; i < this->defiFill::numPolygons(); i++) {
    fprintf(f, "   POLYGON ");
    points = this->defiFill::getPolygon(i);
    for (j = 0; j < points.numPoints; j++)
      fprintf(f, "%d %d ", points.x[j], points.y[j]);
    fprintf(f, "\n");
  }
  fprintf(f,"\n");

  if (this->defiFill::hasVia())
    fprintf(f, "- VIA %s", this->defiFill::viaName());

  if (this->defiFill::viaTopMask() || this->defiFill::viaCutMask() 
      || this->defiFill::viaBottomMask()) {
      fprintf(f, " + MASK %d", this->defiFill::viaTopMask(),
          this->defiFill::viaCutMask(),
          this->defiFill::viaBottomMask());
  }

  if (this->defiFill::hasViaOpc())
    fprintf(f, " + OPC");
  fprintf(f, "\n");

  for (i = 0; i < this->defiFill::numViaPts(); i++) {
    fprintf(f, "   ");
    points = this->defiFill::getViaPts(i);
    for (j = 0; j < points.numPoints; j++)
      fprintf(f, "%d %d ", points.x[j], points.y[j]);
    fprintf(f, "\n");
  }
  fprintf(f,"\n");
}
