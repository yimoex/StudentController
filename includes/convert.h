#pragma once
#include "public.h"
#include "mem.h"

#define BUFFER_MAX_SIZE 1024

char* uint2string(MemPool* pool, uint num, uint* length);
uint string2uint(const char* string);