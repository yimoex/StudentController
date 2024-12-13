#pragma once
#define MEMPOOL_SIZE 8
#define MEMPOOL_BLOCK_SIZE 512
#include "../includes/public.h"

typedef struct MemBlock {
    uint id;
    uint size;
    void* ptr;
    struct MemBlock* next;
} MemBlock;

typedef struct {
    uint size;
    MemBlock* root;
    MemBlock* node;
} MemPool;

void* safe_malloc(size_t size);
void safe_free(void* ptr);
void* safe_realloc(void* ptr, size_t size);
void* mem_malloc(MemPool* pool, size_t memSize);
void* mem_calloc(MemPool* pool, size_t memSize);
MemBlock* mem_block_create(uint id, size_t memSize);
MemPool* mem_pool_create();
bool mem_free(MemPool* pool, uint id);
bool mem_pool_free(MemPool* pool);