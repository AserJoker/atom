#define _CRT_SECURE_NO_WARNINGS
#include "util/source.h"
#include "util/list.h"
#include "util/strings.h"
#include <stdio.h>

SourceLine SourceLine_create() {
  SourceLine sl = (SourceLine)Buffer_alloc(sizeof(struct s_SourceLine), NULL);
  sl->_begin = 0;
  sl->_end = 0;
  sl->_raw.begin = 0;
  sl->_raw.end = 0;
  return sl;
}

void SourceFile_dispose(SourceFile sf) {
  Buffer_dispose(sf->_lines);
  Buffer_dispose(sf->_source);
}
SourceFile SourceFile_read(cstring filename) {
  SourceFile sf =
      (SourceFile)Buffer_alloc(sizeof(struct s_SourceFile), SourceFile_dispose);
  FILE *fp = NULL;
  cstring source = NULL;
  fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  uint32_t len = ftell(fp);
  source = (cstring)Buffer_alloc(len + 1, NULL);
  source[len] = 0;
  fseek(fp, 0, SEEK_SET);
  fread(source, 1, len, fp);
  fclose(fp);
  sf->_source = source;
  sf->_filename = filename;
  sf->_lines = List_create(True);
  cstring linestart = source;
  for (;;) {
    if (*source == '\n' || *source == '\0') {
      source++;
      if (*source == '\r') {
        source++;
      }
      SourceLine sl =
          (SourceLine)Buffer_alloc(sizeof(struct s_SourceLine), NULL);
      sl->_raw.begin = linestart;
      sl->_raw.end = source;
      linestart = source;
      sl->_begin = sl->_raw.begin - sf->_source;
      sl->_end = sl->_raw.end - sf->_source;
      List_insert_tail(sf->_lines, sl);
      if (!*source) {
        break;
      }
    } else {
      source++;
    }
  }

  return sf;
}

Location SourceFile_getLocation(SourceFile file, cstring source) {
  Location loc;
  cstring selector = source;
  while (selector > file->_source &&
         (*selector == '\n' || *selector == '\r' || *selector == '\0')) {
    selector--;
  }
  loc._filename = file->_filename;
  uint32_t index = 0;
  for (List_Node it = List_head(file->_lines); it != List_tail(file->_lines);
       it = List_next(it)) {
    SourceLine line = List_get(it);
    if (selector >= line->_raw.begin && selector < line->_raw.end) {
      loc._position._line = index + 1;
      int column = 0;
      cstring c_selector = line->_raw.begin;
      int isUTF8 = 0;
      while (c_selector != selector) {
        c_selector++;
        column++;
        if (c_selector > line->_raw.begin) {
          if ((*c_selector < 0 && *(c_selector - 1) < 0)) {
            column--;
            isUTF8 = 1;
          }
        }
      }
      if (isUTF8) {
        column++;
      }
      loc._position._column = column + 1;
      if (!*source || *source == '\n' || *source == '\r') {
        loc._position._column++;
      }
      break;
    }
    index++;
  }
  return loc;
}