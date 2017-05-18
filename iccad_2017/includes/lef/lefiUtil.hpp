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


#ifndef lefiUtil_h
#define lefiUtil_h

// Structure to return Macro data to FE
struct lefiPoints {
  double x;
  double y;
};

typedef struct lefiPoints lefiNum;

//int lefiValidTime();
int lefiValidUser();

char* lefiUser();

char* lefiOrientStr(int orient);

double convert_name2num(char *versionName);

bool  validateMaskNumber(int num);

#endif

