#pragma once
#include "buffer.h"
typedef char *cstring;
typedef struct s_Strings {
  cstring begin;
  cstring end;
} Strings;
Buffer cstring_toBuffer(cstring source);
cstring cstring_from(Strings source);
cstring cstring_concat(cstring str, ...);
cstring cstring_join(cstring args[]);
int Strings_is(Strings str, const cstring source);
int Strings_contains(Strings str, const cstring list[]);