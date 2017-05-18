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


#include "lefiDebug.hpp"
#include "lefrReader.hpp"
#include "lex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char lefDebug[100];

/*******************
 *  Debug flags:
 *  0 -
 *  1 - malloc debug
 *  2 - print each history size bump up.
 *  3 -
 *  4 -
 *  5 -
 *  6 -
 *  7 -
 *  8 -
 *  9 -
 * 10 -
 * 11 - lexer debug
 * 12 -
 * 13 - print each lex token as read in.
 *
 ******************************/

extern int totalMsgLimit;
extern int lefErrMsgPrinted;
extern int lefMsgLimit[3][4701];

/* Set flag */
void lefiSetDebug(int num, int value) {
  lefDebug[num] = value;
}



/* Read flag */
int lefiDebug(int num) {
  return lefDebug[num];
}

extern LEFI_LOG_FUNCTION lefiErrorLogFunction;

void lefiError(int check, int msgNum, const char* str) {

  /* check is 1 if the caller function has checked totalMsgLimit, etc. */

  if (!check) {
     if ((totalMsgLimit > 0) && (lefErrMsgPrinted >= totalMsgLimit))
        return;
     if (lefMsgLimit[0][msgNum] > 0) {
        if (lefMsgLimit[1][msgNum] >= lefMsgLimit[0][msgNum]) /*over the limit*/
            return;
        lefMsgLimit[1][msgNum] = lefMsgLimit[1][msgNum] + 1;
     }
     lefErrMsgPrinted++;
  }

  if (lefiErrorLogFunction)
    (*lefiErrorLogFunction)(str);
  else
    fprintf(stderr, str);
}


static char lefiShift [] = {
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


const char* lefUpperCase(const char* str) {
  static char* shiftBuf = 0;
  static int shiftBufLength = 0;
  char* place = (char*)str;
  char* to;
  int len = strlen(str) + 1;

  if (len > shiftBufLength) {
    if (shiftBuf == 0) {
      len = len < 64 ? 64 : len;
      shiftBuf = (char*)lefMalloc(len);
      shiftBufLength = len;
    } else {
      lefFree(shiftBuf);
      shiftBuf = (char*)malloc(len);
      shiftBufLength = len;
    }
  }

  to = shiftBuf;
  while (*place) {
    int i = (int)*place;
      place++;
    *to++ = lefiShift[i];
  }
  *to = '\0';

  return shiftBuf;
}


