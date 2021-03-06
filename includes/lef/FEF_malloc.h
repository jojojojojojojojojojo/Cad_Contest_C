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


#ifndef FEF_mallocs
#define FEF_mallocs

/*****************************************************************
 * FEF_malloc.h:  memory management basic routines
 ******************************************************************
 */

void *FEF_malloc(int num_bytes,const char *comment);
void *FEF_calloc(int num_elem,int elem_size,const char *comment);
void *FEF_realloc(void *ptr,int num_bytes,const char *comment);
void *FEF_free_then_malloc(void *ptr,int num_bytes,const char *comment);
void *FEF_relax(void *ptr,int num_bytes,const char *comment);
void  FEF_free(void *ptr,const char *comment);
void NoMemory();
char *memory_malloc(unsigned n);
void memory_free();

#endif
