#include "util/buffer.h"
#include <stdlib.h>
#include <string.h>

static Buffer default_alloc(uint32_t size, const char *filename, int line) {
  return malloc(size);
}

static Buffer_Alloc g_alloc = default_alloc;
static Buffer_Free g_free = free;

void initBufferContext(Buffer_Alloc alloc, Buffer_Free free) {
  g_alloc = alloc;
  g_free = free;
}
Buffer Buffer_alloc_(uint32_t size, Buffer_Free dispose, const char *filename,
                     int line) {
  Buffer_Free *buf = g_alloc(size + sizeof(dispose), filename, line);
  *buf = dispose;
  return buf + 1;
}
Buffer Buffer_clone_(uint32_t size, Buffer source, const char *filename,
                     int line) {
  void *buf = Buffer_alloc_(size, NULL, filename, line);
  memcpy(buf, source, size);
  return buf;
}

void Buffer_free(Buffer buf) {
  Buffer_Free *ptr = (Buffer_Free *)buf;
  ptr--;
  if (*ptr) {
    (*ptr)(buf);
  }
  g_free(ptr);
}