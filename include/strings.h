#pragma once
#include "buffer.h"
typedef char *cstring;
typedef struct s_stirngs {
  cstring begin;
  cstring end;
} strings;
Buffer Buffer_fromString(cstring source);
int strings_is(strings str, const cstring source);