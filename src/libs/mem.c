#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../includes/public.h"
#include "../includes/error.h"
#include "../includes/mem.h"

size_t mem_all_size = 0;

void* safe_malloc(size_t size){
    void* ptr = (void *)malloc(size);
    if(ptr == NULL) call_fatel("分配内存失败!\n");
    mem_all_size += size;
    return ptr;
}

void safe_free(void* ptr){
    if(ptr == NULL) call_fatel("正在尝试释放空指针!\n");
    free(ptr);
}

void* safe_realloc(void* ptr, size_t size){
    void* newPtr = (void *)realloc(ptr, size);
    if(newPtr == NULL) call_fatel("realloc: 分配内存失败!\n");
    return newPtr;
}

MemBlock* mem_block_create(uint id, size_t memSize){
    MemBlock* block = (MemBlock *)safe_malloc(sizeof(MemBlock));
    block -> ptr = (void *)safe_malloc(memSize);
    block -> size = memSize;
    block -> id = id;
    block -> next = NULL;
    return block;
}

MemPool* mem_pool_create(){
    MemPool* pool = (MemPool *)safe_malloc(sizeof(MemPool));
    pool -> size = 0;
    pool -> root = (MemBlock *)safe_malloc(sizeof(MemBlock));
    pool -> root -> id = -1;
    pool -> root -> next = NULL;
    pool -> root -> ptr = NULL;
    pool -> node = pool -> root;
    return pool;
}

void* mem_malloc(MemPool* pool, size_t memSize){
    pool -> node -> next = mem_block_create(pool -> size++, memSize);
    pool -> node = pool -> node -> next;
    return pool -> node;
}

void* mem_calloc(MemPool* pool, size_t memSize){
    pool -> node -> next = mem_block_create(pool -> size++, memSize);
    pool -> node = pool -> node -> next;
    memset(pool -> node -> ptr, 0, memSize);
    return pool -> node;
}

bool mem_free(MemPool* pool, uint id){
    MemBlock* block = pool -> root, *last = pool -> root;
    while((block = block -> next) != NULL){
        if(block -> id == id){
            last -> next = block -> next;
            free(block -> ptr);
            free(block);
            return true;
        }
        last = block;
    }
    return false;
}

bool mem_pool_free(MemPool* pool){
    MemBlock* block = pool -> root;
    if(block -> next == NULL){
        free(pool -> root);
        free(pool);
        return true;
    }
    while((block = block -> next) != NULL){
        free(block -> ptr);
        block -> ptr = NULL;
        free(block);
    }
    free(pool -> root);
    free(pool);
    return true;
}

void* mem_pool_hook( void* (*hooker)(MemPool*, void*), void* data){
    MemPool* pool = mem_pool_create();
    void* back = hooker(pool, data);
    mem_pool_free(pool);
    return back;
}

void* mem_hook(MemPool* pool, void* (*hooker)(MemPool*, void*), void* data){
    return hooker(pool, data);
}

