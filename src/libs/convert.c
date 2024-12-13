#include <string.h>
#include "../includes/public.h"
#include "../includes/mem.h"
#include "../includes/error.h"
#include "../includes/convert.h"

char* uint2string(MemPool* pool, uint num, uint* length){
    MemBlock* mem = mem_malloc(pool, sizeof(char) * BUFFER_MAX_SIZE);
    char* buffer = mem -> ptr;
    uint rawLen = 0, i;
    char number[BUFFER_MAX_SIZE] = {0};
    while(num > 0){
        number[rawLen] = num % 10 + '0';
        rawLen++;
        num /= 10;
    }
    for(i = 0;i < rawLen;i++){
        buffer[i] = number[rawLen - i - 1];
    }
    buffer[i] = '\0';
    *length = rawLen;
    return buffer;
}

uint string2uint(const char* string){
    uint rec = 0, i;
    uint length = strlen(string);
    char tmp;
    for(i = 0;i < length;i++){
        tmp = string[i];
        if(tmp < '0' || tmp > '9') call_fatel("convert: 无法读入非数字");
        rec = rec * 10 + (tmp - '0');
    }
    return rec;
}
