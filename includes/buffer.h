#pragma once
#include <stdio.h>
#include "public.h"

#define BUFFER_BLOCK_SIZE 2048
#define BUFFER_BLOCK_EXT_SIZE 8

typedef struct {
    char* ptr;
    uint realsize;
    uint size;
} Buffer;

Buffer* buffer_create();
Buffer* buffer_create_size(uint size);
Buffer* buffer_add(Buffer* buf, const char* data, uint length);
Buffer* buffer_add_char(Buffer* buf, char ch);
Buffer* buffer_write(Buffer* buf, char* data);
bool buffer_to_file(Buffer* buf, FILE* fp);
bool buffer_fread(Buffer* buf, uint size, uint count, FILE* fp);
bool buffer_fread_add(Buffer* buf, uint size, uint count, FILE* fp);
Buffer* buffer_add_buffer(Buffer* dest, Buffer* src);
Buffer* buffer_add_char(Buffer* buf, char ch);
bool buffer_empty(Buffer* buf);
bool buffer_is(Buffer* buf, char* ptr);
Buffer* buffer_clear(Buffer* buf);
Buffer* buffer_copy(Buffer* src);
void buffer_check(Buffer* buf);
void buffer_delete(Buffer* buf);