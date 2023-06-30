#pragma once
#include "list.h"
#include "strings.h"


typedef struct s_SourceLine {
  Strings _raw;
  uint32_t _begin;
  uint32_t _end;
} *SourceLine;

typedef struct s_SourceFile {
  List _lines;
  cstring _source;
  cstring _filename;
} *SourceFile;

struct s_Position {
  uint32_t _line;
  uint32_t _column;
};
typedef struct s_Position Position;

struct s_Location {
  cstring _filename;
  Position _position;
};
typedef struct s_Location Location;

SourceLine SourceLine_create();
SourceFile SourceFile_read(cstring filename);
void SourceFile_dispose(SourceFile sf);
Location SourceFile_getLocation(SourceFile file, cstring source);