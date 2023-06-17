#include "debug.h"
#include "buffer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct s_DebugFrame *DebugFrame;

uint32_t allocated = 0;
struct s_DebugFrame {
  void *handler;
  uint32_t size;
  const char *filename;
  int line;
  DebugFrame next;
};

static DebugFrame frame = NULL;

Buffer Debug_alloc(uint32_t size, const char *filename, int line) {
  void *data = malloc(size);
  DebugFrame f = (DebugFrame)malloc(sizeof(struct s_DebugFrame));
  f->filename = filename;
  f->line = line;
  f->handler = data;
  f->size = size;
  f->next = NULL;
  if (!frame) {
    frame = f;
  } else {
    DebugFrame tmp = frame;
    while (tmp->next) {
      tmp = tmp->next;
    }
    tmp->next = f;
  }
  allocated += size;
  return data;
}
void Debug_free(Buffer buf) {
  if (frame->handler == buf) {
    DebugFrame tmp = frame;
    frame = frame->next;
    allocated -= tmp->size;
    free(tmp);
  } else {
    DebugFrame f = frame;
    while (f->next) {
      if (f->next->handler == buf) {
        break;
      }
      f = f->next;
    }
    DebugFrame tmp = f->next;
    f->next = f->next->next;
    allocated -= tmp->size;
    free(tmp);
  }
  free(buf);
}
void Debug_check() {
  while (frame) {
    DebugFrame f = frame;
    frame = frame->next;
    printf("[Debug] memory leak: addr is 0x%llx,size is %u,define at %s:%d\n",
           (uint64_t)(ptrdiff_t)f->handler, f->size, f->filename, f->line);
    free(f->handler);
    free(f);
  }
}
void Debug_init() {
  Buffer_init(Debug_alloc, Debug_free);
  atexit(&Debug_check);
}