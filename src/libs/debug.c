#include <stdio.h>
#include "../includes/mem.h"

void mem_print_all(MemPool* pool){
    MemBlock* block = pool -> root, *next;
    if(block -> next == NULL){
        printf("Root[%d]: %d\n", block -> id, block -> size);
    }
    do{
        printf("Mem[%d]: %d\n", block -> id, block -> size);
    }while((block = block -> next) != NULL);
}

