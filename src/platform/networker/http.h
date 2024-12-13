#pragma once
#include "../../../includes/buffer.h"

#define HTTP_MAX_HEADER_SIZE 128

typedef struct {
    Buffer* protocol;
    uint statusCode;
    Buffer* status;
    Buffer* header;
    Buffer* body;
    uint size;
    uint headerSize;
    uint bodySize;
} HttpResponse;

typedef enum {
    HTTP_VERSION_11 = 11,
    HTTP_VERSION_2 = 20,
} HttpProtocol;

typedef enum {
    HTTP_REQUEST_TYPE_GET = 1,
    HTTP_REQUEST_TYPE_POST = 2,
} HttpType;

typedef struct {
    Buffer* addr;
    Buffer* host;
    Buffer* path;
    uint port;
    uint ip;
    HttpType type; //请求类型
    HttpProtocol protocol; //协议族
    Buffer* header;
    Buffer* data;
} HttpRequest;

HttpRequest* request_create(const char* addr, uint port);
void request_free(HttpRequest* req);
void request_target_add(Buffer* buffer, const char* key, const char* value);
Buffer* request_build(HttpRequest* req);
//Buffer* response_require(const char* data, uint dataSize, uint* ptr, char cut)
HttpResponse* response_create(const char* data);
void response_free(HttpResponse* res);