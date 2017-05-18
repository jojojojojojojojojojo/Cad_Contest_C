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


#ifndef _DFEF_stringhash_h
#define _DFEF_stringhash_h

/* DFEF_stringhash.h
 *
 *TODO	The 'void *handle' arguments should really be 'HTABLE *'
 *	but this requires changing all uses.  For now make_stringtab
 *	will return an HTABLE *, which can be assigned to the 'void *'
 *	that is used in old code or a 'HTABLE *' pointer in new code.
 *	The 'void *' argument will accept both 'void *' and 'HTABLE *'
 *	arguments.
 */

typedef struct htable HTABLE;

/* Some constants for calling STR_make_stringtab.
 * These should be enumerated types, but its a pain to change now.
 */
enum table_alloc { TABLE_CALLER_ALLOCS, TABLE_ITSELF_ALLOCS};
enum table_case  { TABLE_IGNORE_CASE,   TABLE_USE_CASE};

extern HTABLE* STR_make_stringtab( enum table_alloc alloc,const char *comment,
			    enum table_case ignore_case /*= TABLE_USE_CASE*/ );

/*
 * STR_add_to_stringtab adds the string WITHOUT checking for duplication
 */
extern void STR_add_to_stringtab(void* handle,const char *str, void* value);
/*
 * STR_make_string_in_stringtab returns returns a matching string if present,
 * otherwise it adds it.  If the caller is allocating the string, then
 * the caller will probably have to free the incoming string if the return
 * value indicates the table is using an existing string.
 */
extern char *STR_make_string_in_stringtab(HTABLE* handle,const char *str,void *v);
extern int  STR_present_in_stringtab(void* handle,const char *strp,int **v);
extern void STR_destroy_stringtab(void* handle);
extern int  STR_active_hashtables(const char *indent);
extern void STR_apply_stringtab(void *handle,void (*fn)(const char*, void*, void *), void *a);
/*
 * STR_modify_stringtab modifies the value (an arbitrary int) associated
 * with this string and return TRUE if the string is present.
 * If the string isn't present, the function does nothing and returns FALSE.
 */
extern int STR_modify_stringtab(void *handle,const char *str,void *new_val);

#endif
