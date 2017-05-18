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
#include "lefiPropType.hpp"
#include "lefiDebug.hpp"


lefiPropType::lefiPropType() {
  this->lefiPropType::Init();
}


void lefiPropType::Init() {
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  this->propNames_ = 0;
  this->propTypes_ = 0;
}


void lefiPropType::Clear() {
  int i;

  for (i = 0; i < this->numProperties_; i++) {
    lefFree(this->propNames_[i]);
  }
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
}


void lefiPropType::Destroy() {
  this->lefiPropType::Clear();
  if (this->propNames_)
     lefFree(this->propNames_);
  if (this->propTypes_)
     lefFree(this->propTypes_);
}


lefiPropType::~lefiPropType() {
  this->lefiPropType::Destroy();
}


void lefiPropType::setPropType(const char* name, const char type) {
  int len;

  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiPropType::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}


void lefiPropType::bumpProps() {
  int lim = this->propertiesAllocated_;
  int news ;
  char** newpn;
  char*   newt;
 
  news = lim ? lim + lim : 2;
 
  newpn = (char**)lefMalloc(sizeof(char*)*news);
  newt = (char*)lefMalloc(sizeof(char)*news);
 
  lim = this->propertiesAllocated_ = news;
 
  if (lim > 2) {
     int i;
     for (i = 0; i < this->numProperties_; i++) {
       newpn[i] = this->propNames_[i];
       newt[i] = this->propTypes_[i];
     }
     lefFree((char*)(this->propNames_));
     lefFree((char*)(this->propTypes_));
  }
  this->propNames_ = newpn;
  this->propTypes_ = newt;
}


const char lefiPropType::propType(char* name) const {
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
