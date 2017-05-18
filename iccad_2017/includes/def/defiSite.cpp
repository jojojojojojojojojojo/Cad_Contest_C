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
#include "defiSite.hpp"
#include "defiDebug.hpp"
#include "defiUtil.hpp"


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//     defiSite
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiSite::defiSite() {
  this->defiSite::Init();
}


defiSite::~defiSite() {
  this->defiSite::Destroy();
}


void defiSite::Init() {
  this->siteName_ = (char*)defMalloc(32);
  this->nameSize_ = 32;
  this->defiSite::clear();
}


void defiSite::Destroy() {
  defFree(this->siteName_);
}


void defiSite::clear() {
  if (this->siteName_)
     *this->siteName_ = '\0';
  this->x_num_ = 0.0;
  this->y_num_ = 0.0;
  this->x_step_ = 0.0;
  this->y_step_ = 0.0;
  this->orient_ = 0;
}


void defiSite::setName(const char* name) {
  int len = 1;
  char* from = (char*)name;
  this->defiSite::clear();
  while (*from++) len++;
  if (this->nameSize_ < len) this->defiSite::bumpName(len);
  strcpy(this->siteName_, DEFCASE(name));
}


void defiSite::setLocation(double xorg, double yorg) {
  this->x_orig_ = xorg;
  this->y_orig_ = yorg;
}


void defiSite::setOrient(int orient) {
  this->orient_ = orient;
}


void defiSite::setDo(double x_num, double y_num, double x_step, double y_step) {
  this->x_num_ = x_num;
  this->y_num_ = y_num;
  this->x_step_ = x_step;
  this->y_step_ = y_step;
}


double defiSite::x_num() const {
  return this->x_num_;
}


double defiSite::y_num() const {
  return this->y_num_;
}


double defiSite::x_step() const {
  return this->x_step_;
}


double defiSite::y_step() const {
  return this->y_step_;
}


double defiSite::x_orig() const {
  return this->x_orig_;
}


double defiSite::y_orig() const {
  return this->y_orig_;
}


int defiSite::orient() const {
  return this->orient_;
}


const char* defiSite::orientStr() const {
  return (defiOrientStr(this->orient_));
}


const char* defiSite::name() const {
  return this->siteName_;
}


void defiSite::bumpName(int size) {
  defFree(this->siteName_);
  this->siteName_ = (char*)defMalloc(size);
  this->nameSize_ = size;
  *this->siteName_ = '\0';
}


void defiSite::print(FILE* f) const {
  fprintf(f, "Site '%p' %s\n", this->defiSite::name(),
     this->defiSite::orientStr());
  fprintf(f, "  DO X %g %g BY %g\n",
     this->defiSite::x_orig(),
     this->defiSite::x_num(),
     this->defiSite::x_step());
  fprintf(f, "  DO Y %g %g BY %g\n",
     this->defiSite::y_orig(),
     this->defiSite::y_num(),
     this->defiSite::y_step());

}


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//     defiBox
//
//////////////////////////////////////////////
//////////////////////////////////////////////

defiBox::defiBox() {
  this->defiBox::Init();
}

void defiBox::Init() {
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->points_ = 0;
}

defiBox::~defiBox() {
  this->defiBox::Destroy();
}

void defiBox::Destroy() {
  struct defiPoints* p;

  p = this->points_;
  if (p) {
    defFree((char*)(p->x));
    defFree((char*)(p->y));
    defFree((char*)(this->points_));
  }
}

void defiBox::addPoint(defiGeometries* geom) {
  struct defiPoints* p;
  struct defiPoints* tp;
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
    // for backward compatibility assign the first 2 points to xl, yl, xh & yh
    if (i == 0) {
      this->xl_ = x;
      this->yl_ = y;
    } else if (i == 1) {
      this->xh_ = x;
      this->yh_ = y;
    }
  }
  if (this->points_) {
     tp = this->points_;
     defFree((char*)(tp->x));
     defFree((char*)(tp->y));
     defFree((char*)(tp));
  }
  this->points_ = p;
}

int defiBox::xl() const {
  return this->xl_;
}


int defiBox::yl() const {
  return this->yl_;
}


int defiBox::xh() const {
  return this->xh_;
}


int defiBox::yh() const {
  return this->yh_;
}


struct defiPoints defiBox::getPoint() const {
  return *(this->points_);
}

void defiBox::print(FILE* f) const {
  fprintf(f, "Box %d,%d %d %d\n",
    this->defiBox::xl(),
    this->defiBox::yl(),
    this->defiBox::xh(),
    this->defiBox::yh());
}


