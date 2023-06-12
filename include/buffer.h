#pragma once
#include <stdint.h>
#include <stdio.h>
typedef void *Buffer;
typedef Buffer (*Buffer_Alloc)(size_t size, const char *filename, int line);
typedef void (*Buffer_Free)(Buffer buffer);

/**
 * @brief init global alloc and free handle
 *
 * @param alloc allocator handle for alloc buffer with size
 * @param free free handle for free buffer with pointer
 */
void Buffer_init(Buffer_Alloc alloc, Buffer_Free free);
/**
 * @brief alloc buffer with size
 *
 * @param size size of buffer size
 * @return Buffer the pointer for alloc with size
 */
Buffer Buffer_alloc_(size_t size, const char *filename, int line);
#define Buffer_alloc(size) Buffer_alloc_(size, __FILE__, __LINE__)
/**
 * @brief clone buffer from pointer and size
 *
 * @param size size of the buffer
 * @param source pointer of source buffer
 * @return Buffer pointer of the new buffer
 */
Buffer Buffer_from_(size_t size, Buffer source, const char *filename, int line);
#define Buffer_from(size, source) Buffer_from_(size, source, __FILE__, __LINE__)
/**
 * @brief free buffer with pointer
 *
 * @param buf free buffer with pointer
 */
void Buffer_free(Buffer buf);

#define toBuffer(t) Buffer_from(sizeof(t), &(t))
#define fromBuffer(buf, type) (*(type *)(buf))
