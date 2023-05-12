#include "buffer.h"
#include <stdlib.h>
#include <string.h>

static Buffer_Alloc g_alloc = malloc;
static Buffer_Free g_free = free;

void Buffer_init(Buffer_Alloc alloc, Buffer_Free free) {
  g_alloc = alloc;
  g_free = free;
}
Buffer Buffer_alloc(uint32_t size) { return g_alloc(size); }
Buffer Buffer_from(uint32_t size, Buffer source) {
  void *buf = g_alloc(size);
  memcpy(buf, source, size);
  return buf;
}

void Buffer_free(Buffer buf) { g_free(buf); }