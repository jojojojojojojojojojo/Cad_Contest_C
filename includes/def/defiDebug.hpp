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


#ifndef defiDebug_h
#define defiDebug_h 1

#include "defiKRDefs.hpp"

/* Set flag */
EXTERN void defiSetDebug PROTO_PARAMS((int num, int value)) ;

/* Read flag */
EXTERN int defiDebug PROTO_PARAMS((int num)) ;



/* Error loggin function */
extern void defiError(int check, int msgNum, const char* message);

/* for auto upshifting names in case insensitive files */
extern int names_case_sensitive;
extern const char* upperCase(const char* c);
#define DEFCASE(x) (names_case_sensitive ? x : upperCase(x)) 

#endif
