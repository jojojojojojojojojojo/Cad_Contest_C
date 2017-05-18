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


#ifndef lefiArray_h
#define lefiArray_h

#include <stdio.h>
#include "lefiKRDefs.hpp"
#include "lefiMisc.hpp"


class lefiArrayFloorPlan {
public:
  void Init(const char* name);
  void Destroy();
  void addSitePattern(const char* typ, lefiSitePattern* s);

  int numPatterns() const;
  lefiSitePattern* pattern(int index);
  char* typ(int index);
  const char* name();

protected:
  int numPatterns_;
  int patternsAllocated_;
  lefiSitePattern** patterns_;
  char** types_;
  char* name_;
};


class lefiArray {
public:
  lefiArray();
  void Init();

  void Destroy();
  ~lefiArray();

  void setName(const char* name);
  void addSitePattern(lefiSitePattern* s);
  void setTableSize(int tsize);
  void addDefaultCap(int minPins, double cap);
  void addCanPlace(lefiSitePattern* s);
  void addCannotOccupy(lefiSitePattern* s);
  void addTrack(lefiTrackPattern* t);
  void addGcell(lefiGcellPattern* g);
  void addFloorPlan(const char* name);
  void addSiteToFloorPlan(const char* typ, lefiSitePattern* p);
  void clear();
  void bump(void*** arr, int used, int* allocated);

  int numSitePattern();
  int numCanPlace();
  int numCannotOccupy();
  int numTrack();
  int numGcell();
  int hasDefaultCap();

  const char* name();
  lefiSitePattern* sitePattern(int index);
  lefiSitePattern* canPlace(int index);
  lefiSitePattern* cannotOccupy(int index);
  lefiTrackPattern* track(int index);
  lefiGcellPattern* gcell(int index);

  int tableSize();
  int numDefaultCaps();
  int defaultCapMinPins(int index);
  double defaultCap(int index);

  int numFloorPlans();
  const char* floorPlanName(int index);
  int numSites(int index);
  const char* siteType(int floorIndex, int siteIndex);
  lefiSitePattern* site(int floorIndex, int siteIndex);

  // Debug print
  void print(FILE* f);

protected:
  int nameSize_;
  char* name_;

  int patternsAllocated_;
  int numPatterns_;
  lefiSitePattern** pattern_;

  int canAllocated_;
  int numCan_;
  lefiSitePattern** canPlace_;

  int cannotAllocated_;
  int numCannot_;
  lefiSitePattern** cannotOccupy_;

  int tracksAllocated_;
  int numTracks_;
  lefiTrackPattern** track_;

  int gAllocated_;
  int numG_;
  lefiGcellPattern** gcell_;

  int hasDefault_;
  int tableSize_;
  int numDefault_;
  int defaultAllocated_;
  int* minPins_;
  double* caps_;

  int numFloorPlans_;
  int floorPlansAllocated_;
  lefiArrayFloorPlan** floors_;
};

#endif
