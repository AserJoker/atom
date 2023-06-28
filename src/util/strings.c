#include "util/Strings.h"
#include "util/buffer.h"
#include "util/list.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

Buffer cstring_toBuffer(cstring source) {
  return Buffer_clone(strlen(source) + 1, source);
}

int Strings_is(Strings str, const cstring source) {
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

int Strings_contains(Strings str, const cstring list[]) {
  for (int index = 0; list[index] != 0; index++) {
    if (Strings_is(str, list[index])) {
      return 1;
    }
  }
  return 0;
}

cstring cstring_from(Strings source) {
  uint32_t len = source.end - source.begin;
  cstring result = (cstring)Buffer_clone(len + 1, source.begin);
  result[len] = 0;
  return result;
}
cstring cstring_concat(cstring str, ...) {
  List_Option opt = {0, 0};
  List parts = List_create(opt);
  uint32_t len = strlen(str);
  va_list args;
  List_insert_tail(parts, str);
  va_start(args, str);
  for (;;) {
    cstring arg = va_arg(args, cstring);
    if (!arg) {
      va_end(args);
      break;
    }
    len += strlen(arg);
    List_insert_tail(parts, arg);
  }
  cstring result = (cstring)Buffer_alloc(len + 1, NULL);
  cstring selector = result;
  result[len] = 0;
  for (List_Node node = List_head(parts); node != List_tail(parts);
       node = List_next(node)) {
    cstring s = (cstring)List_get(node);
    while (*s) {
      *selector++ = *s++;
    }
  }
  Buffer_dispose(parts);
  return result;
}

cstring cstring_join(cstring args[]) {
  uint32_t len = 0;
  for (uint32_t i = 0; args[i] != 0; i++) {
    len += strlen(args[i]);
  }
  cstring result = (cstring)Buffer_alloc(len + 1, NULL);
  result[len] = 0;
  cstring s = result;
  for (uint32_t i = 0; args[i] != 0; i++) {
    cstring ss = args[i];
    while (*ss) {
      *s++ = *ss++;
    }
  }
  return result;
}