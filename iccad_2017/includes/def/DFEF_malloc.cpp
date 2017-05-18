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
#include <malloc.h>

#include "lex.h"
#include "DFEF_malloc.h"
#include "DFEF_util.h"
#include "defiDebug.hpp"

extern char defDebug[100];


/**********************************************************************/
/**********************************************************************/
void *DFEF_calloc(int num_elem,int elem_size,const char *comment)
{
int n = num_elem*elem_size;
void *ptr;
if(defDebug[1])
	printf("%s calloc -> %d * %d = %d bytes"
		,comment,num_elem,elem_size,n);
if(num_elem<1) defrNerr(169);
if(elem_size<1) defrNerr(169);
ptr =  calloc(num_elem,elem_size);
if( ! ptr ) defNoMemory();
return ptr;
}
/**********************************************************************/
/**********************************************************************/
void *DFEF_malloc(int num_bytes,const char *comment)
{
void *ptr;
if(defDebug[1])
	printf("%s malloc -> %d bytes",comment,num_bytes);
if(num_bytes<1) defrNerr(115);
ptr =  defMalloc(num_bytes);
if( ! ptr ) defNoMemory();
return ptr;
}
/**********************************************************************/
void *DFEF_free_then_malloc(void *ptr,int num_bytes,const char *comment)
{
if(defDebug[1])
	printf("%s free, then malloc -> %d bytes",comment,num_bytes);
if(num_bytes<1) defrNerr(116);
if(ptr)
	{
	free((char*)ptr);
#	ifdef WIN32
	_heapmin();
#	endif
	}
ptr =  defMalloc(num_bytes);
if( ! ptr ) defNoMemory();
return ptr;
}
/**********************************************************************/
void *DFEF_realloc(void *ptr,int num_bytes,const char *comment)
{
if(defDebug[1])
	printf("%s realloc -> %d bytes",comment,num_bytes);
if(num_bytes<1) defrNerr(117);
if(ptr)
	ptr = defRealloc((char*)ptr,num_bytes);
else
	ptr = defMalloc(num_bytes);
if( ! ptr ) defNoMemory();
return ptr;
}
/**********************************************************************/
/**********************************************************************/
void *DFEF_relax(void *ptr,int num_bytes,const char *comment)
{
void *old_ptr;
void *new_ptr;
if(defDebug[1])
	printf("%s DFEF_relax(defRealloc) -> %d bytes",comment,num_bytes);
if(!ptr) defrNerr(168);
old_ptr = ptr;
new_ptr =  defRealloc((char*)ptr,num_bytes);
if(new_ptr != old_ptr)
   if(defDebug[0])
	defrNwarn(944);
return new_ptr;
}
/**********************************************************************/
/**********************************************************************/
void DFEF_free(void *ptr,const char *comment)
{
if(defDebug[1])
	printf("%s free",comment);
if(ptr)
	{
	free((char*)ptr);
#	ifdef WIN32
	_heapmin();
#	endif
	}
else  defrNerr(118);
}
/**********************************************************************/
/**********************************************************************/
void defNoMemory() {
  defiError(0, 6000, "ERROR (DEFPARS-6000): out of memory\n");
  exit(1);
}
/**********************************************************************/
/**********************************************************************/
/* Replacement for malloc() that keeps track of what's out, and can free it all*/
static int nout = 0;
static int array_size = 0;
static char **what;

char *defmemory_malloc(unsigned n)
{
if (array_size == 0) {  /* first time */
    array_size = 64;
    what = (char **)defMalloc(array_size * sizeof(char *));
    if (what == NULL)
	defNoMemory();
    }
nout++;
if (nout > array_size) {
    array_size <<= 1;
    what = (char **)defRealloc((char *)what, array_size*sizeof(char *));
    if (what == NULL)
	defNoMemory();
    }
what[nout-1] = (char *)defMalloc(n);
if (what[nout-1] == NULL)
    defNoMemory();
return what[nout-1];
}

void defmemory_free()
{
int i;
for(i=0; i<nout; i++)
    free(what[i]);
if(what != NULL)
    free((char *)what);
#ifdef WIN32
_heapmin();
#endif
}
