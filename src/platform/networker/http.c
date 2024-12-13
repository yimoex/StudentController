#pragma once
#include <string.h>
#include "../../../includes/buffer.h"
#include "../../../includes/mem.h"
#include "../../../includes/convert.h"
#include "http.h"

HttpRequest* request_create(const char* addr, uint port){
    Buffer* header = buffer_create();
    Buffer* data = buffer_create();
    Buffer* host = buffer_create();
    Buffer* addrBuf = buffer_create();
    Buffer* path = buffer_create();
    HttpRequest* request = (HttpRequest *)safe_malloc(sizeof(HttpRequest));
    request -> header = header;
    request -> data = data;
    request -> host = host;
    request -> addr = addrBuf;
    request -> path = path;

    buffer_add(addrBuf, addr, strlen(addr));
    return request;
}

void request_free(HttpRequest* req){
    buffer_delete(req -> header);
    buffer_delete(req -> data);
    buffer_delete(req -> addr);
    buffer_delete(req -> host);
    buffer_delete(req -> path);
    safe_free(req);
}

void request_target_add(Buffer* buffer, const char* key, const char* value){
    buffer_add(buffer, key, strlen(key));
    buffer_add(buffer, ": ", 2);
    buffer_add(buffer, value, strlen(value));
    buffer_add(buffer, "\r\n", 2);
}

Buffer* request_build(HttpRequest* req){
    Buffer* res = buffer_create();
    if(req -> type == HTTP_REQUEST_TYPE_POST) buffer_add(res, "POST", 4);
    else buffer_add(res, "GET", 3);
    buffer_add(res, " ", 1);
    buffer_add(res, "/", 1);
    if(!buffer_empty(req -> data)) buffer_add_buffer(res, req -> path); //添加路径
    buffer_add(res, " ", 1);
    buffer_add(res, "HTTP/1.1\r\n", 10); //默认1.1

    //主体部分
    request_target_add(res, "Host", req -> host -> ptr);
    request_target_add(res, "Connection", "close");
    if(!buffer_empty(req -> header)) buffer_add_buffer(res, req -> header);
    buffer_add(res, "\r\n", 2);

    //数据体部分
    if(req -> type == HTTP_REQUEST_TYPE_POST && !buffer_empty(req -> data)){
        buffer_add_buffer(res, req -> data);
    }

    
    return res;
}

Buffer* response_require(const char* data, uint dataSize, uint* ptr, char cut){
    char cur, next;
    Buffer* buf = buffer_create();
    while(1){
        if(*ptr == dataSize) return NULL;
        cur = data[*ptr];
        if(cur == cut){
            buf -> ptr[buf -> realsize] = '\0';
            (*ptr)++;
            return buf;
        }
        buffer_add_char(buf, cur);
        (*ptr)++;
    }
}

HttpResponse* response_create(const char* data){
    uint ptr = 0, dataSize = strlen(data);
    HttpResponse* res = (HttpResponse *)safe_malloc(sizeof(HttpResponse));
    Buffer* protocol = response_require(data, dataSize, &ptr, ' '); //procotol
    Buffer* statusCodeRaw = response_require(data, dataSize, &ptr, ' '); //statusCode
    Buffer* status = response_require(data, dataSize, &ptr, '\r'); //status
    if(protocol == NULL || statusCodeRaw == NULL || status == NULL) return NULL;
    uint statusCode = string2uint(statusCodeRaw -> ptr);
    const char* newData = data + ptr + 1; // +1 = \n
    char* bodyString = strstr(newData, "\r\n\r\n");
    uint pos = bodyString - newData - 4; //\r\n

    Buffer *header, *body;
    header = buffer_create();
    body = buffer_create();
    buffer_add(header, newData, pos);
    buffer_add(body, bodyString + 4, strlen(bodyString));
    res -> statusCode = statusCode;
    res -> protocol = protocol;
    res -> status = status;
    res -> body = body;
    res -> header = header;
    
    return res;
}

void response_free(HttpResponse* res){
    buffer_delete(res -> header);
    buffer_delete(res -> body);
    buffer_delete(res -> status);
    buffer_delete(res -> protocol);
    safe_free(res);
}