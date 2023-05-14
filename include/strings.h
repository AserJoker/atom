#pragma once
#include "buffer.h"
typedef char *cstring;
typedef struct s_stirngs {
  cstring begin;
  cstring end;
} strings;
Buffer Buffer_fromString(cstring source);
cstring cstring_from(strings source);
cstring cstring_concat(cstring str, ...);
cstring cstring_join(cstring args[]);
int strings_is(strings str, const cstring source);