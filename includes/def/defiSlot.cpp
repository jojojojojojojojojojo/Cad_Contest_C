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
#include "defiSlot.hpp"
#include "defiDebug.hpp"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiSlot
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

defiSlot::defiSlot() {
  this->defiSlot::Init();
}


void defiSlot::Init() {
  this->numPolys_ = 0;
  this->defiSlot::clear();
  this->layerNameLength_ = 0;
  this->xl_ = (int*)defMalloc(sizeof(int)*1);
  this->yl_ = (int*)defMalloc(sizeof(int)*1);
  this->xh_ = (int*)defMalloc(sizeof(int)*1);
  this->yh_ = (int*)defMalloc(sizeof(int)*1);
  this->rectsAllocated_ = 1;      // At least 1 rectangle will define
  this->polysAllocated_ = 0;
  this->polygons_ = 0;
  this->layerName_ = 0;
}

defiSlot::~defiSlot() {
  this->defiSlot::Destroy();
}

void defiSlot::clear() {
  this->hasLayer_ = 0;
  this->numRectangles_ = 0;
}

void defiSlot::clearPoly() {
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

void defiSlot::Destroy() {
  if (this->layerName_) defFree(this->layerName_);
  defFree((char*)(this->xl_));
  defFree((char*)(this->yl_));
  defFree((char*)(this->xh_));
  defFree((char*)(this->yh_));
  this->rectsAllocated_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->defiSlot::clearPoly();
  if (this->polygons_) defFree((char*)(this->polygons_));
  this->polygons_ = 0;
  this->defiSlot::clear();
}


void defiSlot::setLayer(const char* name) {
  int len = strlen(name) + 1;
  if (this->layerNameLength_ < len) {
    if (this->layerName_) defFree(this->layerName_);
    this->layerName_ = (char*)defMalloc(len);
    this->layerNameLength_ = len;
  }
  strcpy(this->layerName_, DEFCASE(name));
  this->hasLayer_ = 1;
}


void defiSlot::addRect(int xl, int yl, int xh, int yh) {
  if (this->numRectangles_ == this->rectsAllocated_) {
    int i;
    int max = this->rectsAllocated_ = this->rectsAllocated_ * 2;
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
void defiSlot::addPolygon(defiGeometries* geom) {
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

int defiSlot::hasLayer() const {
  return this->hasLayer_;
}


const char* defiSlot::layerName() const {
  return this->layerName_;
}


int defiSlot::numRectangles() const {
  return this->numRectangles_;
}


int defiSlot::xl(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numRectangles_) {
     sprintf (msg, "ERROR (DEFPARS-6160): The index number %d specified for the SLOT RECTANGLE is invalid.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, this->numRectangles_);
     defiError (0, 6160, msg);
     return 0;
  }
  return this->xl_[index];
}


int defiSlot::yl(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numRectangles_) {
     sprintf (msg, "ERROR (DEFPARS-6160): The index number %d specified for the SLOT RECTANGLE is invalid.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, this->numRectangles_);
     defiError (0, 6160, msg);
     return 0;
  }
  return this->yl_[index];
}


int defiSlot::xh(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numRectangles_) {
     sprintf (msg, "ERROR (DEFPARS-6160): The index number %d specified for the SLOT RECTANGLE is invalid.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, this->numRectangles_);
     defiError (0, 6160, msg);
     return 0;
  }
  return this->xh_[index];
}


int defiSlot::yh(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numRectangles_) {
     sprintf (msg, "ERROR (DEFPARS-6160): The index number %d specified for the SLOT RECTANGLE is invalid.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, this->numRectangles_);
     defiError (0, 6160, msg);
     return 0;
  }
  return this->yh_[index];
}

// 5.6
int defiSlot::numPolygons() const {
  return this->numPolys_;
}

// 5.6
struct defiPoints defiSlot::getPolygon(int index) const {
  return *(this->polygons_[index]);
}

void defiSlot::print(FILE* f) const {
  int i, j;
  struct defiPoints points;

  if (this->defiSlot::hasLayer())
    fprintf(f, "- LAYER %s\n", this->defiSlot::layerName());

  for (i = 0; i < this->defiSlot::numRectangles(); i++) {
    fprintf(f, "   RECT %d %d %d %d\n", this->defiSlot::xl(i),
            this->defiSlot::yl(i), this->defiSlot::xh(i),
            this->defiSlot::yh(i));
  }

  for (i = 0; i < this->defiSlot::numPolygons(); i++) {
    fprintf(f, "   POLYGON ");
    points = this->defiSlot::getPolygon(i);
    for (j = 0; j < points.numPoints; j++)
      fprintf(f, "%d %d ", points.x[j], points.y[j]);
    fprintf(f, "\n");
  }
  fprintf(f,"\n");
}
