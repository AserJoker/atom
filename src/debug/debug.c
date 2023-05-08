#include "debug.h"
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
static size_t g_memory_alloced = 0;
Buffer Debug_alloc(size_t size) {
  void *data = malloc(size + sizeof(size_t));
  printf("[DEBUG] : alloc memory with size: %lld, address is 0x%llx\n", size,
         (uintptr_t)((size_t *)data + 1));
  *(size_t *)data = size;
  g_memory_alloced += size;
  return (size_t *)data + 1;
}
void Debug_free(Buffer buf) {
  size_t size = *((size_t *)buf - 1);
  fprintf(stderr, "[DEBUG] : free memory with address: 0x%llx, size is:%lld\n",
          (uintptr_t)buf, size);
  free((size_t *)buf - 1);
  g_memory_alloced -= size;
}
void Debug_check() {
  if (g_memory_alloced != 0) {
    printf("[DEBUG] : !!memory leak!! size: %lld", g_memory_alloced);
  }
}
void Debug_init() {
  Buffer_init(Debug_alloc, Debug_free);
  atexit(&Debug_check);
}