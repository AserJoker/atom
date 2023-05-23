#include "source.h"
#include "list.h"
#include "strings.h"
#include <stdio.h>

SourceLine SourceLine_create() {
  SourceLine sl = (SourceLine)Buffer_alloc(sizeof(struct s_SourceLine));
  sl->_begin = 0;
  sl->_end = 0;
  sl->_raw.begin = 0;
  sl->_raw.end = 0;
  return sl;
}

void SourceLine_dispose(SourceLine sl) { Buffer_free(sl); }

SourceFile SourceFile_read(cstring filename) {
  SourceFile sf = (SourceFile)Buffer_alloc(sizeof(struct s_SourceFile));
  FILE *fp = NULL;
  cstring source = NULL;
  fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  uint32_t len = ftell(fp);
  source = (cstring)Buffer_alloc(len + 1);
  source[len] = 0;
  fseek(fp, 0, SEEK_SET);
  fread(source, 1, len, fp);
  fclose(fp);
  sf->_source = source;
  sf->_filename = filename;
  List_Option opt = {1, (Buffer_Free)SourceLine_dispose};
  sf->_lines = List_create(opt);
  cstring linestart = source;
  for (;;) {
    if (*source == '\n' || *source == '\0') {
      source++;
      if (*source == '\r') {
        source++;
      }
      SourceLine sl = (SourceLine)Buffer_alloc(sizeof(struct s_SourceLine));
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

void SourceFile_dispose(SourceFile sf) {
  List_dispose(sf->_lines);
  Buffer_free(sf->_source);
  Buffer_free(sf);
}

Location getLocation(SourceFile file, cstring source) {
  Location loc;
  loc._filename = file->_filename;
  uint32_t index = 0;
  if (!*source) {
    loc._position._line = List_size(file->_lines);
    List_Node last = List_last(List_tail(file->_lines));
    SourceLine line = (SourceLine)List_get(last);
    loc._position._column = line->_end - line->_begin;
    return loc;
  }
  for (List_Node it = List_head(file->_lines); it != List_tail(file->_lines);
       it = List_next(it)) {
    SourceLine line = List_get(it);
    if (source >= line->_raw.begin && source < line->_raw.end) {
      loc._position._line = index;
      loc._position._column = source - line->_raw.begin;
      while (loc._position._column == '\r') {
        loc._position._column--;
      }
      break;
    }
    index++;
  }
  return loc;
}