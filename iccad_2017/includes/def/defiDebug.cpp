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
#include <string.h>
#include "lex.h"
#include "defiDebug.hpp"


char defDebug[100];

typedef void (*DEFI_LOG_FUNC)(const char* str) ;
DEFI_LOG_FUNC defiErrorLogFunction = 0;

typedef void (*DEFI_WARNING_LOG_FUNC)(const char* str) ;
DEFI_WARNING_LOG_FUNC defiWarningLogFunction = 0;

typedef void (*DEFI_LINE_NUMBER_FUNC)(int);
DEFI_LINE_NUMBER_FUNC defiLineNumberFunction = 0;
int defiDeltaNumberLines = 10000;

typedef void (*DEFI_MAGIC_COMMENT_FOUND_FUNC)(void);
DEFI_MAGIC_COMMENT_FOUND_FUNC defiMagicCommentFoundFunction = 0;
char *defiMagicCommentString = NULL;

/*******************
 *  Debug flags:
 *  0 -
 *  1 - malloc debug
 *  2 - print each history size bump up.
 *  3 - print each call to CatchAll
 *  4 -
 *  5 -
 *  6 -
 *  7 -
 *  8 -
 *  9 -
 * 10 -
 * 11 - lexer debug
 *
 ******************************/

extern int totalDefMsgLimit;
extern int defMsgPrinted;
extern int defMsgLimit[2][4013];

/* Set flag */
void defiSetDebug(int num, int value) {
  defDebug[num] = value;
}



/* Read flag */
int defiDebug(int num) {
  return defDebug[num];
}



void defiError(int check, int msgNum, const char* mess) {
  /* check is 1 if the caller function has checked totalMsgLimit, etc. */

  if (!check) {
     if ((totalDefMsgLimit > 0) && (defMsgPrinted >= totalDefMsgLimit))
        return;
     if (defMsgLimit[0][msgNum-5000] > 0) {
        if (defMsgLimit[1][msgNum-5000] >= defMsgLimit[0][msgNum-5000])
           return;    /*over the limit*/
        defMsgLimit[1][msgNum-5000] = defMsgLimit[1][msgNum-5000] + 1;
     }
     defMsgPrinted++;
  }

  if (defiErrorLogFunction) {
    (*defiErrorLogFunction)(mess);
  } else {
    fprintf(stderr, mess);
  }
}



static char defiShift [] = {
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  ' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'', 
  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', 
  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7', 
  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', 
  '@',  'A',  'B',  'C',  'D',  'E',  'F',  'G', 
  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', 
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W', 
  'X',  'Y',  'Z',  '[', '\\',  ']',  '^',  '_', 
  '`',  'A',  'B',  'C',  'D',  'E',  'F',  'G', 
  'H',  'I',  'J',  'K',  'l',  'M',  'N',  'O', 
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W', 
  'X',  'Y',  'Z',  '{',  '|',  '}',  '~', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};


const char* upperCase(const char* str) {
  static char* shiftBuf = 0;
  static int shiftBufLength = 0;
  char* place = (char*)str;
  char* to;
  int len = strlen(str) + 1;

  if (len > shiftBufLength) {
    if (shiftBuf == 0) {
      len = len < 64 ? 64 : len;
      shiftBuf = (char*)defMalloc(len);
      shiftBufLength = len;
    } else {
      defFree(shiftBuf);
      shiftBuf = (char*)defMalloc(len);
      shiftBufLength = len;
    }
  }

  to = shiftBuf;
  while (*place) {
      int i = (int)*place;
      place++;
    *to++ = defiShift[i];
  }
  *to = '\0';

  return shiftBuf;
}


