#pragma once
#include <stdint.h>
#include <stdio.h>
typedef void *Buffer;
typedef Buffer (*Buffer_Alloc)(uint32_t size);
typedef void (*Buffer_Free)(Buffer buffer);
void Buffer_init(Buffer_Alloc alloc, Buffer_Free free);
Buffer Buffer_alloc(uint32_t size);
Buffer Buffer_from(uint32_t size, Buffer source);
void Buffer_free(Buffer buf);

#define toBuffer(t) Buffer_from(sizeof(t), &(t))
#define fromBuffer(buf, type) (*(type *)(buf))
