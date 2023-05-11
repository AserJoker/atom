#include "strings.h"
#include <string.h>
Buffer Buffer_fromString(cstring source) {
  return Buffer_from(strlen(source) + 1, source);
}

int strings_is(strings str, const cstring source) {
  cstring ss = (cstring)source;
  cstring s = str.begin;
  while (s != str.end) {
    if (*s != *ss) {
      return 0;
    }
    s++;
    ss++;
  }
  return *ss == 0;
}