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
#include "defiVia.hpp"
#include "defiDebug.hpp"


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiVia
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiVia::defiVia() {
  this->defiVia::Init();
}


void defiVia::Init() {
  this->name_ = 0;
  this->nameLength_ = 0;
  this->pattern_ = 0;
  this->patternLength_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->layersLength_ = 0;
  this->layers_ = 0;
  this->viaRule_ = 0;
  this->viaRuleLength_ = 0;
  this->xSize_ = 0;
  this->ySize_ = 0;
  this->botLayer_ = 0;
  this->cutLayer_ = 0;
  this->topLayer_ = 0;
  this->botLayerLength_ = 0;
  this->cutLayerLength_ = 0;
  this->topLayerLength_ = 0;
  this->xCutSpacing_ = 0;
  this->yCutSpacing_ = 0;
  this->xBotEnc_ = 0;
  this->yBotEnc_ = 0;
  this->xTopEnc_ = 0;
  this->yTopEnc_ = 0;
  this->cutPattern_ = 0;
  this->cutPatternLength_ = 0;
  this->numLayers_ = 0;
  this->numPolys_ = 0;
  this->polygons_ = 0;
  this->polysAllocated_ = 0;
  this->polygonNames_ = 0;
  this->rectMask_ = 0;
  this->polyMask_ = 0;
  this->defiVia::clear();
}

void defiVia::clear() {
  int i;

  this->hasPattern_ = 0;
  this->hasViaRule_ = 0;
  this->rows_ = 0;
  this->cols_ = 0;
  this->xOffset_ = 0;
  this->yOffset_ = 0;
  this->xBotOffset_ = 0;
  this->yBotOffset_ = 0;
  this->xTopOffset_ = 0;
  this->yTopOffset_ = 0;
  this->hasCutPattern_ = 0;
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
    defFree((char*)(this->polyMask_));

    this->polygonNames_ = 0;
    this->polygons_ = 0;
    this->polyMask_ = 0;
  }
  this->numPolys_ = 0;
  this->polysAllocated_ = 0;
}

defiVia::~defiVia() {
  this->defiVia::Destroy();
}


void defiVia::Destroy() {
  int i;

  if (this->name_) defFree(this->name_);
  if (this->pattern_) defFree(this->pattern_);

  if (this->layers_) {
    for (i = 0; i < this->numLayers_; i++)
      if (this->layers_[i]) defFree(this->layers_[i]);
    defFree((char*)(this->layers_));
    defFree((char*)(this->xl_));
    defFree((char*)(this->yl_));
    defFree((char*)(this->xh_));
    defFree((char*)(this->yh_));
    defFree((char*)(this->rectMask_));
    defFree((char*)(this->polyMask_));
  }
  if (this->viaRule_)
    defFree((char*)(this->viaRule_));
  if (this->botLayer_)
    defFree((char*)(this->botLayer_));
  if (this->cutLayer_)
    defFree((char*)(this->cutLayer_));
  if (this->topLayer_)
    defFree((char*)(this->topLayer_));
  if (this->cutPattern_)
    defFree((char*)(this->cutPattern_));
  this->defiVia::clear();
}


void defiVia::setup(const char* name) {
  int i;
  int len = strlen(name) + 1;
  if (len > this->nameLength_) {
    this->nameLength_ = len;
    this->name_ = (char*)defRealloc(this->name_, len);
  }
  strcpy(this->name_, DEFCASE(name));
  if (this->pattern_) *(this->pattern_) = 0;
  if (this->layers_) {
    for (i = 0; i < this->numLayers_; i++) {
      if (this->layers_[i]) defFree(this->layers_[i]);
    this->layers_[i] = 0;
    }
  }
  this->numLayers_ = 0;
}


void defiVia::addPattern(const char* pattern) {
  int len = strlen(pattern) + 1;
  if (len > this->patternLength_) {
    this->patternLength_ = len;
    this->pattern_ = (char*)defRealloc(this->pattern_, len);
  }
  strcpy(this->pattern_, DEFCASE(pattern));
  this->hasPattern_ = 1;
}


void defiVia::addLayer(const char* layer, int xl, int yl, int xh, int yh, int colorMask) {
  char* l;
  int len;

  if (this->numLayers_ >= this->layersLength_) {
    int i;
    char** newl;
    int* ints;
    this->layersLength_ = this->layersLength_ ? 2 * this->layersLength_ : 8;

    newl = (char**)defMalloc(this->layersLength_ * sizeof(char*));
    for (i = 0; i < this->numLayers_; i++)
      newl[i] = this->layers_[i];
    if (this->layers_) defFree((char*)(this->layers_));
    this->layers_ = newl;

    ints = (int*)defMalloc(this->layersLength_ * sizeof(int));
    for (i = 0; i < this->numLayers_; i++)
      ints[i] = this->xl_[i];
    if (this->xl_) defFree((char*)(this->xl_));
    this->xl_ = ints;

    ints = (int*)defMalloc(this->layersLength_ * sizeof(int));
    for (i = 0; i < this->numLayers_; i++)
      ints[i] = this->yl_[i];
    if (this->yl_) defFree((char*)(this->yl_));
    this->yl_ = ints;

    ints = (int*)defMalloc(this->layersLength_ * sizeof(int));
    for (i = 0; i < this->numLayers_; i++)
      ints[i] = this->xh_[i];
    if (this->xh_) defFree((char*)(this->xh_));
    this->xh_ = ints;

    ints = (int*)defMalloc(this->layersLength_ * sizeof(int));
    for (i = 0; i < this->numLayers_; i++)
      ints[i] = this->yh_[i];
    if (this->yh_) defFree((char*)(this->yh_));
    this->yh_ = ints;

    ints = (int*)defMalloc(this->layersLength_ * sizeof(int));
    for (i = 0; i < this->numLayers_; i++)
        ints[i] = this->rectMask_[i];
    if (this->rectMask_) defFree((char*)(this->rectMask_));
    this->rectMask_ = ints;
  }

  len = strlen(layer) + 1;
  l = (char*)defMalloc(len);
  strcpy(l, DEFCASE(layer));
  this->layers_[this->numLayers_] = l;
  this->xl_[this->numLayers_] = xl;
  this->yl_[this->numLayers_] = yl;
  this->xh_[this->numLayers_] = xh;
  this->yh_[this->numLayers_] = yh;
  this->rectMask_[this->numLayers_] = colorMask;
  this->numLayers_++;
}

// 5.6
void defiVia::addPolygon(const char* layer, defiGeometries* geom, int colorMask) {
  struct defiPoints* p;
  int x, y;
  int i;

  if (this->numPolys_ == this->polysAllocated_) {
    char** newn;
    int* masks;
    struct defiPoints** poly;
    this->polysAllocated_ = (this->polysAllocated_ == 0) ?
          2 : this->polysAllocated_ * 2;
    newn = (char**)defMalloc(sizeof(char*) * this->polysAllocated_);
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            this->polysAllocated_);
    masks = (int*)defMalloc(this->polysAllocated_ * sizeof(int));
    for (i = 0; i < this->numPolys_; i++) {
      newn[i] = this->polygonNames_[i];
      poly[i] = this->polygons_[i];
      masks[i] = this->polyMask_[i];
    }
    if (this->polygons_)
      defFree((char*)(this->polygons_));
    if (this->polygonNames_)
      defFree((char*)(this->polygonNames_));
    if (this->polyMask_) 
      defFree((char*)(this->polyMask_));
    this->polygonNames_ = newn;
    this->polygons_ = poly;
    this->polyMask_ = masks;
  }
  this->polygonNames_[this->numPolys_] = strdup(layer);
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
  this->polyMask_[this->numPolys_] = colorMask;
  this->numPolys_ += 1;
}

void defiVia::addViaRule(char* viaRuleName, int xSize, int ySize,
                         char* botLayer, char* cutLayer, char* topLayer,
                         int xSpacing, int ySpacing, int xBotEnc, int yBotEnc,
                         int xTopEnc, int yTopEnc) {
  int len;

  len = strlen(viaRuleName) + 1;
  if (len > this->viaRuleLength_) {
    if (this->viaRule_)
      defFree(this->viaRule_);
    this->viaRule_ = (char*)defMalloc(strlen(viaRuleName)+1);
  }
  strcpy(this->viaRule_, DEFCASE(viaRuleName));
  this->xSize_ = xSize;
  this->ySize_ = ySize;
  len = strlen(botLayer) + 1;
  if (len > this->botLayerLength_) {
    if (this->botLayer_)
      defFree(this->botLayer_);
    this->botLayer_ = (char*)defMalloc(strlen(botLayer)+1);
    this->botLayerLength_ = len;
  }
  strcpy(this->botLayer_, DEFCASE(botLayer));
  len = strlen(cutLayer) + 1;
  if (len > this->cutLayerLength_) {
    if (this->cutLayer_)
      defFree(this->cutLayer_);
    this->cutLayer_ = (char*)defMalloc(strlen(cutLayer)+1);
    this->cutLayerLength_ = len;
  }
  strcpy(this->cutLayer_, DEFCASE(cutLayer));
  len = strlen(topLayer) + 1;
  if (len > this->topLayerLength_) {
    if (this->topLayer_)
      defFree(this->topLayer_);
    this->topLayer_ = (char*)defMalloc(strlen(topLayer)+1);
    this->topLayerLength_ = len;
  }
  strcpy(this->topLayer_, DEFCASE(topLayer));
  this->xCutSpacing_ = xSpacing;
  this->yCutSpacing_ = ySpacing;
  this->xBotEnc_ = xBotEnc;
  this->yBotEnc_ = yBotEnc;
  this->xTopEnc_ = xTopEnc;
  this->yTopEnc_ = yTopEnc;
  this->hasViaRule_ = 1;
}

void defiVia::addRowCol(int numCutRows, int numCutCols) {
  this->rows_ = numCutRows;
  this->cols_ = numCutCols;
}

void defiVia::addOrigin(int xOffset, int yOffset) {
  this->xOffset_ = xOffset;
  this->yOffset_ = yOffset;
}

void defiVia::addOffset(int xBotOs, int yBotOs, int xTopOs, int yTopOs) {
  this->xBotOffset_ = xBotOs;
  this->yBotOffset_ = yBotOs;
  this->xTopOffset_ = xTopOs;
  this->yTopOffset_ = yTopOs;
}

void defiVia::addCutPattern(char* cutPattern) {
  int len;

  len = strlen(cutPattern) + 1;
  if (len > this->cutPatternLength_) {
    if (this->cutPattern_)
      defFree(this->cutPattern_);
    this->cutPattern_ = (char*)defMalloc(strlen(cutPattern)+1);
    this->cutPatternLength_ = len;
  }
  strcpy(this->cutPattern_, DEFCASE(cutPattern));
  this->hasCutPattern_ = 1;
}

int defiVia::hasPattern() const {
  return this->hasPattern_;
}


const char* defiVia::pattern() const {
  return this->pattern_;
}


const char* defiVia::name() const {
  return this->name_;
}


int defiVia::numLayers() const {
  return this->numLayers_;
}

int defiVia::rectMask(int index) const {
    if (index >= 0 && index < this->numLayers_) {
        return this->rectMask_[index];
    }

    return 0;
}

int defiVia::polyMask(int index) const {
    if (index >= 0 && index < this->numLayers_) {
        return this->polyMask_[index];
    }

    return 0;
}

void defiVia::layer(int index, char** layer, int* xl, int* yl, 
      int* xh, int* yh) const {
  if (index >= 0 && index < this->numLayers_) {
    if (layer) *layer = this->layers_[index];
    if (xl) *xl = this->xl_[index];
    if (yl) *yl = this->yl_[index];
    if (xh) *xh = this->xh_[index];
    if (yh) *yh = this->yh_[index];
  }
}

// The following code is for 5.6

int defiVia::numPolygons() const {
  return this->numPolys_;
}

const char* defiVia::polygonName(int index) const {
  char msg[160];
  if (index < 0 || index > this->numPolys_) {
     sprintf (msg, "ERROR (DEFPARS-6180): The index number %d specified for the VIA POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again",
              index, this->numPolys_);
     defiError (0, 6180, msg);
     return 0;
  }
  return this->polygonNames_[index];
}

struct defiPoints defiVia::getPolygon(int index) const {
  return *(this->polygons_[index]);
}

int defiVia::hasViaRule() const {
  return this->hasViaRule_;
}

void defiVia::viaRule(char** viaRuleName, int* xSize, int* ySize,
                      char** botLayer, char** cutLayer, char** topLayer,
                      int* xCutSpacing, int* yCutSpacing, int* xBotEnc,
                      int* yBotEnc, int* xTopEnc, int* yTopEnc) const {
  *viaRuleName = this->viaRule_;
  *xSize = this->xSize_;
  *ySize = this->ySize_;
  *botLayer = this->botLayer_;
  *cutLayer = this->cutLayer_;
  *topLayer = this->topLayer_;
  *xCutSpacing = this->xCutSpacing_;
  *yCutSpacing = this->yCutSpacing_;
  *xBotEnc = this->xBotEnc_;
  *yBotEnc = this->yBotEnc_;
  *xTopEnc = this->xTopEnc_;
  *yTopEnc = this->yTopEnc_;
}

int defiVia::hasRowCol() const {
  if (this->rows_)
    return this->rows_; 
  else
    return this->cols_; 
}

void defiVia::rowCol(int* numCutRows, int* numCutCols) const {
  *numCutRows = this->rows_;
  *numCutCols = this->cols_;
}

int defiVia::hasOrigin() const {
  if (this->xOffset_)
    return this->xOffset_;
  else
    return this->yOffset_;
}

void defiVia::origin(int* xOffset, int* yOffset) const {
  *xOffset = this->xOffset_;
  *yOffset = this->yOffset_;
}

int defiVia::hasOffset() const {
  if (this->xBotOffset_)
    return this->xBotOffset_;
  else if (this->yBotOffset_)
    return this->yBotOffset_;
  else if (this->xTopOffset_)
    return this->xTopOffset_;
  else 
    return this->yTopOffset_;
}

void defiVia::offset(int* xBotOffset, int* yBotOffset, int* xTopOffset,
                     int* yTopOffset) const {
  *xBotOffset = this->xBotOffset_;
  *yBotOffset = this->yBotOffset_;
  *xTopOffset = this->xTopOffset_;
  *yTopOffset = this->yTopOffset_;
}

int defiVia::hasCutPattern() const {
  return this->hasCutPattern_;
}

int defiVia::hasRectMask(int index) const {
    if (index > 0 || index < this->numLayers_) {
        return this->rectMask_[index];
    }

    return 0;
}

int defiVia::hasPolyMask(int index) const {
    if (index > 0 || index < this->numPolys_) {
        return this->polyMask_[index];
    }
    
    return 0;
}

const char* defiVia::cutPattern() const {
  return this->cutPattern_;
}

void defiVia::print(FILE* f) const {
  int i;
  int xl, yl, xh, yh;
  char* c;
  char* vrn;
  char *bl, *cl, *tl;
  int xs, ys, xcs, ycs, xbe, ybe, xte, yte;
  int cr, cc, xo, yo, xbo, ybo, xto, yto;

  fprintf(f, "via '%s'\n", this->defiVia::name());

  if (this->defiVia::hasPattern())
    fprintf(f, "  pattern '%s'\n", this->defiVia::pattern());

  for (i = 0; i < this->defiVia::numLayers(); i++) {
    this->defiVia::layer(i, &c, &xl, &yl, &xh, &yh);
    fprintf(f, "  layer '%s' %d,%d %d,%d\n", c, xl, yl, xh, yh);
  }

  if (this->defiVia::hasViaRule()) {
    this->defiVia::viaRule(&vrn, &xs, &ys, &bl, &cl, &tl, &xcs, &ycs,
                                 &xbe, &ybe, &xte, &yte);
    fprintf(f, "  viarule '%s'\n", vrn);
    fprintf(f, "    cutsize %d %d\n", xs, ys);
    fprintf(f, "    layers %d %d %d\n", bl, cl, tl);
    fprintf(f, "    cutspacing %d %d\n", xcs, ycs);
    fprintf(f, "    enclosure %d %d %d %d\n", xbe, ybe, xte, yte);
    if (this->defiVia::hasRowCol()) {
       this->defiVia::rowCol(&cr, &cc);
       fprintf(f, "    rowcol %d %d\n", cr, cc);
    }
    if (this->defiVia::hasOrigin()) {
       this->defiVia::origin(&xo, &yo);
       fprintf(f, "    origin %d %d\n", xo, yo);
    }
    if (this->defiVia::hasOffset()) {
       this->defiVia::offset(&xbo, &ybo, &xto, &yto);
       fprintf(f, "    offset %d %d %d %d\n", xbo, ybo, xto, yto);
    }
    if (this->defiVia::hasCutPattern())
       fprintf(f, "    pattern '%s'\n", this->defiVia::cutPattern());
  }
}
