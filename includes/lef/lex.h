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


void lefAddStringDefine(char *token, char *string);
void lefAddBooleanDefine(char *token, int val);
void lefAddNumDefine(char *token, double val);
void lefAddStringMessage(char *token, char *string);
void yyerror(const char *s);
void lefError(int msgNum, const char *s);
void lefWarning(int msgNum, const char *s);
void lefInfo(int msgNum, const char *s);
void* lefMalloc(int lef_size);
void* lefRealloc(void *name, int lef_size);
void lefFree(void *name);
void lefSetNonDefault(const char* name);
void lefUnsetNonDefault();
#ifdef __cplusplus
extern "C" int yylex();
extern "C" void lex_init();
extern "C" int lefyyparse();
extern "C" void lex_un_init();
#else
extern int yylex();
extern void lex_init();
extern int lefyyparse();
extern void lex_un_init();
#endif
int fake_ftell();
