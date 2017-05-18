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
#include "defiFPC.hpp"
#include "defiDebug.hpp"


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiFPC
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiFPC::defiFPC() {
  this->defiFPC::Init();
}


void defiFPC::Init() {
  this->name_ = 0;
  this->nameLength_ = 0;
  this->namesAllocated_ = 0;
  this->namesUsed_ = 0;
  this->names_ = 0;
  this->rowOrComp_ = 0;
  this->defiFPC::clear();
}


defiFPC::~defiFPC() {
  this->defiFPC::Destroy();
}


void defiFPC::Destroy() {

  this->defiFPC::clear();

  if (this->name_) defFree(this->name_);
  this->name_ = 0;
  this->nameLength_ = 0;

  defFree((char*)(this->names_));
  defFree((char*)(this->rowOrComp_));
  this->namesAllocated_ = 0;
}


void defiFPC::clear() {
  int i;

  this->direction_ = 0;
  this->hasAlign_ = 0;
  this->hasMin_ = 0;
  this->hasMax_ = 0;
  this->hasEqual_ = 0;
  this->corner_ = 0;

  for (i = 0; i < this->namesUsed_; i++) {
    if (this->names_[i]) defFree (this->names_[i]) ;
  }
  this->namesUsed_ = 0;
}


void defiFPC::setName(const char* name, const char* direction) {
  int len = strlen(name) + 1;

  this->defiFPC::clear();

  if (len > this->nameLength_) {
    if (this->name_) defFree(this->name_);
    this->nameLength_ = len;
    this->name_ = (char*)defMalloc(len);
  }
  strcpy(this->name_, DEFCASE(name));

  if (*direction == 'H')
    this->direction_ = 'H';
  else if (*direction == 'V')
    this->direction_ = 'V';
  else
    defiError(0, 6030, "ERROR (DEFPARS-6030): Invalid direction specified with FPC name. The valid direction is either 'H' or 'V'. Specify a valid value and then try again.");

}


void defiFPC::print(FILE* f) const {
  fprintf(f, "FPC '%s'\n", this->name_);
}


const char* defiFPC::name() const {
  return this->name_;
}


int defiFPC::isVertical() const {
  return this->direction_ == 'V' ? 1 : 0 ;
}


int defiFPC::isHorizontal() const {
  return this->direction_ == 'H' ? 1 : 0 ;
}


int defiFPC::hasAlign() const {
  return (int)(this->hasAlign_);
}


int defiFPC::hasMax() const {
  return (int)(this->hasMax_);
}


int defiFPC::hasMin() const {
  return (int)(this->hasMin_);
}


int defiFPC::hasEqual() const {
  return (int)(this->hasEqual_);
}


double defiFPC::alignMin() const {
  return this->minMaxEqual_;
}


double defiFPC::alignMax() const {
  return this->minMaxEqual_;
}


double defiFPC::equal() const {
  return this->minMaxEqual_;
}


int defiFPC::numParts() const {
  return this->namesUsed_;
}


void defiFPC::setAlign() {
  this->hasAlign_ = 0;
}


void defiFPC::setMin(double num) {
  this->minMaxEqual_ = num;
}


void defiFPC::setMax(double num) {
  this->minMaxEqual_ = num;
}


void defiFPC::setEqual(double num) {
  this->minMaxEqual_ = num;
}


void defiFPC::setDoingBottomLeft() {
  this->corner_ = 'B';
}


void defiFPC::setDoingTopRight() {
  this->corner_ = 'T';
}


void defiFPC::getPart(int index, int* corner, int* typ, char** name) const {
  if (index >= 0 && index <= this->namesUsed_) {
    // 4 for bottom left  0 for topright
    // 2 for row   0 for comps
    if (corner) *corner = (int)((this->rowOrComp_[index] & 4) ? 'B' : 'T') ;
    if (typ) *typ = (int)((this->rowOrComp_[index] & 2) ? 'R' : 'C') ;
    if (name) *name = this->names_[index];
  }
}


void defiFPC::addRow(const char* name) {
  this->defiFPC::addItem('R', DEFCASE(name));
}


void defiFPC::addComps(const char* name) {
  this->defiFPC::addItem('C', DEFCASE(name));
}


void defiFPC::addItem(char item, const char* name) {
  int len = strlen(name) + 1;

  if (this->namesUsed_ >= this->namesAllocated_) {
    char* newR;
    char** newN;
    int i;
    this->namesAllocated_ =
	this->namesAllocated_ ? this->namesAllocated_ * 2 : 8 ;
    newN = (char**) defMalloc(sizeof(char*) * this->namesAllocated_);
    newR = (char*) defMalloc(sizeof(char) * this->namesAllocated_);
    for (i = 0; i < this->namesUsed_; i++) {
      newN[i] = this->names_[i];
      newR[i] = this->rowOrComp_[i];
    }
    if (this->names_) defFree((char*)(this->names_));
    if (this->rowOrComp_) defFree(this->rowOrComp_);
    this->names_ = newN;
    this->rowOrComp_ = newR;
  }

  this->names_[this->namesUsed_] = (char*)defMalloc(len);
  strcpy(this->names_[this->namesUsed_], name);

  // 4 for bottomleft
  // 2 for row
  this->rowOrComp_[this->namesUsed_] = 
         (char)(((this->corner_ == 'B') ? 4 : 0) |
	 (item == 'R' ? 2 : 0));

  this->namesUsed_ += 1;
}


