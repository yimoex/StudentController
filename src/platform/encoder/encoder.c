#pragma once
#include <stdio.h>
#include <string.h>
#include "../../../includes/public.h"
#include "../../../includes/mem.h"
#include "../../../includes/buffer.h" //兼容Buffer组件

void encoder_run(const char* raw, const char* key, char* dest, bool isEncode){
    uint length = strlen(raw), keyLength = strlen(key);
    char cur, subKey;
    for(uint i = 0;i < length;i++){
        cur = raw[i];
        subKey = key[i % keyLength];
        dest[i] = cur + (isEncode ? subKey : -subKey);
    }
    dest[length] = '\0';
}

Buffer* encoder_buffer_run(Buffer* buf, const char* key, bool isEncode, int bias){
    Buffer* res = buffer_create_size(buf -> size);
    uint keyLength = strlen(key);
    char* raw = buf -> ptr, *dest = res -> ptr;
    char cur, subKey;
    int t;
    for(uint i = 0;i < buf -> realsize;i++){
        t = i + bias;
        cur = raw[i];
        subKey = key[t % keyLength];
        dest[i] = cur + (isEncode ? subKey : -subKey);
    }
    res -> realsize = buf -> realsize;
    res -> ptr[res -> realsize] = '\0';
    return res;
}

