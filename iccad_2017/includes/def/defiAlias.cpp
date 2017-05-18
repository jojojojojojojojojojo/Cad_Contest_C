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


#include <stdio.h>
#include <stdlib.h>
#include "defiAlias.hpp"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiAlias_itr
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

/* From DFEF_stringhash.cpp */
extern const char* defAliasKey(int num);
extern const char* defAliasData(int num);
extern int defAliasMarked(int num);
extern int defAliasLimit();


defiAlias_itr::defiAlias_itr() {
  this->defiAlias_itr::Init();
}


void defiAlias_itr::Init() {
  this->num_ = -1;
}
 

void defiAlias_itr::Destroy() {
}


defiAlias_itr::~defiAlias_itr() {
  this->defiAlias_itr::Destroy();
}

 
int defiAlias_itr::Next() {
  while (1) {
    this->num_ += 1;
    if (this->num_ <= -1 || this->num_ >= defAliasLimit()) return 0;
    if (defAliasKey(this->num_)) break;
  }
  return 1;
}


const char* defiAlias_itr::Key() {
  if (this->num_ <= -1 || this->num_ >= defAliasLimit()) return 0;
  return defAliasKey(this->num_);
}


const char* defiAlias_itr::Data() {
  if (this->num_ <= -1 || this->num_ >= defAliasLimit()) return 0;
  return defAliasData(this->num_);
}

 
int defiAlias_itr::Marked() {
  if (this->num_ <= -1 || this->num_ >= defAliasLimit()) return 0;
  return defAliasMarked(this->num_);
}
