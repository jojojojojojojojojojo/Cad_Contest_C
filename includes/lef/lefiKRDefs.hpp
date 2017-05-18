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


/* Definitions header file for K&R related defines */

#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32)

#ifndef PROTO_PARAMS
#define PROTO_PARAMS(params) params
#define DEFINED_PROTO_PARAMS
#endif

#ifndef VAR_ARG_LIST
#define VAR_ARG_LIST ...
#define DEFINED_VAR_ARG_LIST
#endif

#ifndef EXTERN
#ifdef  LEFI_CPP_ANSI
#define EXTERN extern "C"
#else
#define EXTERN
#endif
#define DEFINED_EXTERN
#endif

#else

#ifndef PROTO_PARAMS
#define PROTO_PARAMS(params) (/* nothing */)
#define DEFINED_PROTO_PARAMS
#endif

#ifndef VAR_ARG_LIST
#define VAR_ARG_LIST va_tdcl(char *)
#define DEFINED_VAR_ARG_LIST
#endif

#ifndef EXTERN
#define EXTERN extern
#define DEFINED_EXTERN
#ifndef DEFINED_CONST
#define const /* nothing */
#define DEFINED_CONST
#endif
#endif

#endif /* __STDC__ */

