#include "buffer.h"
#include <stdlib.h>
#include <string.h>

static Buffer default_alloc(size_t size, const char *filename, int line) {
  return malloc(size);
}

static Buffer_Alloc g_alloc = default_alloc;
static Buffer_Free g_free = free;

void Buffer_init(Buffer_Alloc alloc, Buffer_Free free) {
  g_alloc = alloc;
  g_free = free;
}
Buffer Buffer_alloc_(size_t size, const char *filename, int line) {
  return g_alloc(size, filename, line);
}
Buffer Buffer_from_(size_t size, Buffer source, const char *filename, int line) {
  void *buf = g_alloc(size, filename, line);
  memcpy(buf, source, size);
  return buf;
}

void Buffer_free(Buffer buf) { g_free(buf); }