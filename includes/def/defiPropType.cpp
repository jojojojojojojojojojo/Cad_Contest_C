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
#include <string.h>
#include "lex.h"
#include "defiPropType.hpp"
#include "defiDebug.hpp"


defiPropType::defiPropType() {
  this->defiPropType::Init();
}


void defiPropType::Init() {
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  this->propNames_ = 0;
  this->propTypes_ = 0;
}


void defiPropType::Clear() {
  int i;

  for (i = 0; i < this->numProperties_; i++) {
    defFree(this->propNames_[i]);
  }
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
}


void defiPropType::Destroy() {
  this->defiPropType::Clear();
  if (this->propNames_)
     defFree(this->propNames_);
  if (this->propTypes_)
     defFree(this->propTypes_);
}


defiPropType::~defiPropType() {
  this->defiPropType::Destroy();
}


void defiPropType::setPropType(const char* name, const char type) {
  int len;

  if (this->numProperties_ == this->propertiesAllocated_)
    this->defiPropType::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)defMalloc(len);
  strcpy(this->propNames_[this->numProperties_], DEFCASE(name));
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}


void defiPropType::bumpProps() {
  int lim = this->propertiesAllocated_;
  int news ;
  char** newpn;
  char*   newt;
 
  news = lim ? lim + lim : 2;
 
  newpn = (char**)defMalloc(sizeof(char*)*news);
  newt = (char*)defMalloc(sizeof(char)*news);
 
  lim = this->propertiesAllocated_ = news;
 
  if (lim > 2) {
     int i;
     for (i = 0; i < this->numProperties_; i++) {
       newpn[i] = this->propNames_[i];
       newt[i] = this->propTypes_[i];
     }
     defFree((char*)(this->propNames_));
     defFree((char*)(this->propTypes_));
  }
  this->propNames_ = newpn;
  this->propTypes_ = newt;
}


const char defiPropType::propType(char* name) const {
  int i;

  // Name is NULL, error
  if (!name)
     return('N');

  for (i = 0; i < this->numProperties_; i++) {
     if (strcmp(name, this->propNames_[i]) == 0)
        return(this->propTypes_[i]);  // found the prop name
  }
  return('N'); // Can't found the name
}
