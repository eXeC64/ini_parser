/* Copyright (c) 2015 Harry Jeffery

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "ini_parser.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

enum parse_state {
  START,
  READING_KEY,
  SEEKING_EQ,
  SEEKING_VALUE,
  READING_VALUE,
  READING_SECTION_KEY,
  SEEKING_SECTION_VALUE,
  SEEKING_END_SECTION,
  READING_SECTION_VALUE,
  ACCEPT,
  REJECT
};

int parse_ini_file(FILE* f, char *out_key, size_t key_size, char *out_value, size_t value_size)
{
  char buf[512];
  while(fgets(&buf[0], sizeof(buf), f)) {
    int type = parse_ini_str(&buf[0], out_key, key_size, out_value, value_size);
    if(type != 0) {
      return type;
    }
  }
  return 0;
}

int parse_ini_str(const char* str, char *out_key, size_t key_size, char *out_value, size_t value_size)
{
  /* ignore comments */
  if(*str == ';') {
    out_key[0] = 0;
    out_value[0] = 0;
    return INI_UNKNOWN;
  }

  const char *key_start = 0;
  size_t key_len = 0;
  const char *value_start = 0;
  size_t value_len = 0;

  int type = INI_UNKNOWN;
  enum parse_state state = START;
  char quote = 0;
  char c = 0;

  do {
    c = *str;
    switch(state) {
      case START:
        if(c == '[') {
          type = INI_SECTION;
          state = READING_SECTION_KEY;
        } else if(c != '=') {
          type = INI_VALUE;
          state = READING_KEY;
          key_start = str;
          ++key_len;
        } else {
          state = REJECT;
        }
        break;
      case READING_KEY:
        if(isspace(c)) {
          state = SEEKING_EQ;
        } else if(c == '=') {
          state = SEEKING_VALUE;
        } else {
          ++key_len;
        }
        break;
      case SEEKING_EQ:
        if(c == '=') {
          state = SEEKING_VALUE;
        } else if(!isspace(c)) {
          state = REJECT;
        }
        break;
      case SEEKING_VALUE:
        if(c == '"' || c == '\'') {
          if(!value_start) {
            quote = c;
            state = READING_VALUE;
          } else {
            state = REJECT;
          }
        } else if(!isspace(c)) {
          value_start = str;
          ++value_len;
          state = READING_VALUE;
        }
        break;
      case READING_VALUE:
        if(quote == 0 && isspace(c)) {
          state = ACCEPT;
        } else if(quote && c == quote) {
          state = ACCEPT;
        } else if(c == 0) {
          state = ACCEPT;
        } else if(c) {
          if(!value_start) {
            value_start = str;
          }
          ++value_len;
        } else {
          state = REJECT;
        }
        break;
      case READING_SECTION_KEY:
        if(isspace(c)) {
          state = SEEKING_SECTION_VALUE;
        } else if(c == ']') {
          state = ACCEPT;
        } else {
          if(!key_start) {
            key_start = str;
          }
          ++key_len;
        }
        break;
      case SEEKING_SECTION_VALUE:
        if(c == '"' || c == '\'') {
          quote = c;
          state = READING_SECTION_VALUE;
        } else if(c == ']') {
          state = ACCEPT;
        } else if(!isspace(c)) {
          state = REJECT;
        }
        break;
      case READING_SECTION_VALUE:
        if(quote == 0 && isspace(c)) {
          state = SEEKING_END_SECTION;
        } else if(quote && c == quote) {
          state = SEEKING_END_SECTION;
        } else if(c == ']') {
          state = ACCEPT;
        } else if(c) {
          if(!value_start) {
            value_start = str;
          }
          ++value_len;
        } else {
          state = REJECT;
        }
        break;
      case SEEKING_END_SECTION:
        if(c == ']') {
          state = ACCEPT;
        } else if(!isspace(c)) {
          state = REJECT;
        }
        break;
      case ACCEPT:
        if(c != 0 && !isspace(c)) {
          state = REJECT;
        }
        break;
      case REJECT:
        return 0;
        break;
    }
    ++str;
  } while(c != 0 && state != REJECT);

  if(state == ACCEPT) {
    if(key_len >= key_size) {
      key_len = key_size - 1;
    }
    if(value_len >= value_size) {
      value_len = value_size - 1;
    }
    memcpy(out_key, key_start, key_len);
    memcpy(out_value, value_start, value_len);
    out_key[key_len] = 0;
    out_value[value_len] = 0;
    return type;
  } else {
    out_key[0] = 0;
    out_value[0] = 0;
  }

  return INI_UNKNOWN;
}
