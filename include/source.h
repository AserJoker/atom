#pragma once
#include "list.h"
#include "strings.h"

typedef struct s_SourceLine {
  strings _raw;
  size_t _begin;
  size_t _end;
} *SourceLine;

typedef struct s_SourceFile {
  List _lines;
  cstring _source;
  cstring _filename;
} *SourceFile;

struct s_Position {
  size_t _line;
  size_t _column;
};
typedef struct s_Position Position;

struct s_Location {
  cstring _filename;
  Position _position;
};
typedef struct s_Location Location;

SourceLine SourceLine_create();
void SourceLine_dispose(SourceLine sl);
SourceFile SourceFile_read(cstring filename);
void SourceFile_dispose(SourceFile sf);
Location getLocation(SourceFile file, cstring source);