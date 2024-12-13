#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "../includes/buffer.h"

Buffer* buffer_create(){
    Buffer* buf = (Buffer *)safe_malloc(sizeof(Buffer));
    buf -> size = BUFFER_BLOCK_SIZE;
    buf -> realsize = 0;
    buf -> ptr = (char *)safe_malloc(sizeof(char) * BUFFER_BLOCK_SIZE);
    return buf;
}

Buffer* buffer_create_size(uint size){
    Buffer* buf = (Buffer *)safe_malloc(sizeof(Buffer));
    buf -> size = size;
    buf -> realsize = 0;
    buf -> ptr = (char *)safe_malloc(sizeof(char) * size);
    return buf;
}

void buffer_check(Buffer* buf){
    if(buf -> realsize < buf -> size) return;
    uint newSize = ((buf -> realsize / BUFFER_BLOCK_SIZE) + 1) * BUFFER_BLOCK_SIZE;
    buf -> ptr = (char *)safe_realloc(buf -> ptr, newSize);
    buf -> size = newSize;
}

Buffer* buffer_add_char(Buffer* buf, char ch){
    buffer_check(buf);
    buf -> ptr[buf -> realsize++] = ch;
    return buf;
}

Buffer* buffer_add(Buffer* buf, const char* data, uint length){
    uint dataSize = length == 0 ? strlen(data) : length;
    uint newAllSize = dataSize + buf -> realsize;
    if(newAllSize > buf -> size){
        uint newSize = ((newAllSize / BUFFER_BLOCK_SIZE) + 1) * BUFFER_BLOCK_SIZE;
        buf -> ptr = (char *)safe_realloc(buf -> ptr, newSize);
        buf -> size = newSize;
    }
    char* ptr = buf -> ptr;
    for(uint i = 0;i < dataSize;i++){
        ptr[i + buf -> realsize] = data[i];
    }
    buf -> realsize += dataSize;
    ptr[buf -> realsize] = '\0'; //这里是对网络库的优化
    return buf;
}

bool buffer_empty(Buffer* buf){
    return buf -> realsize == 0 || buf -> ptr[0] == '\0';
}

Buffer* buffer_add_buffer(Buffer* dest, Buffer* src){
    buffer_add(dest, src -> ptr, src -> realsize);
    return dest;
}

Buffer* buffer_copy(Buffer* src){
    Buffer* new = buffer_create_size(src -> size);
    new -> realsize = src -> realsize;
    strcpy(new -> ptr, src -> ptr);
    return new;
}

Buffer* buffer_write(Buffer* buf, char* data){
    return buffer_add(buf, data, 0);
}

bool buffer_fread_add(Buffer* buf, uint size, uint count, FILE* fp){
    char* tmp = (char *)safe_malloc(size);
    int rec = fread(tmp, size, count, fp);
    uint max = size * count;
    tmp[max] = '\0';
    buffer_add(buf, tmp, max);
    safe_free(tmp);
    return rec == count;
}

bool buffer_fread(Buffer* buf, uint size, uint count, FILE* fp){
    buffer_clear(buf);
    return buffer_fread_add(buf, size, count, fp);
}

bool buffer_is(Buffer* buf, char* ptr){
    return strcmp(buf -> ptr, ptr) == 0;
}

bool buffer_to_file(Buffer* buf, FILE* fp){
    return fwrite(buf -> ptr, buf -> realsize, 1, fp) == 1;
}

Buffer* buffer_clear(Buffer* buf){
    memset(buf -> ptr, '\0', buf -> size);
    buf -> realsize = 0;
    return buf;
}

void buffer_delete(Buffer* buf){
    safe_free(buf -> ptr);
    safe_free(buf);
}
