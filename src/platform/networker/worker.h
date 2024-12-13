#pragma once
#include "../../../includes/buffer.h"
#include <winsock2.h>
#include <windows.h>

#define WORKER_READ_BUFFER_SIZE 8192

#define WORKER_STATUS_WAITTING 0
#define WORKER_STATUS_CLOSE 1
#define WORKER_STATUS_CONNECTED 0

#define WORKER_ERROR_DOMAIN_PARSER 1 //Worker无法解析域名

typedef struct {
    char ip[16];
    char addr[512];
    uint port;
} WorkerInfo;

typedef struct Worker {
    SOCKET sockid;
    int status;
    Buffer* data;
    char* cache;
    WorkerInfo info;
    void (*onClose)(struct Worker *, int);
    void (*onConnectError)(struct Worker *, int);
    void (*onConnect)(struct Worker *);
    void (*onMessage)(struct Worker *);
    struct sockaddr_in local;
    struct sockaddr_in remote;
    void* context; //绑定上下文
} Worker;

void worker_init();
bool worker_isdomain(const char* data);
char* worker_domain2ip(const char* domain);
//Worker* worker_base_create(const char* addr, uint port);
Worker* worker_create(const char* addr, uint port, int* error);
Worker* worker_server(const char* addr, uint port);
void worker_close(Worker* worker, int errorCode);
int worker_send(Worker* worker, const char* data);
int worker_read(Worker* worker, SOCKET client);
bool worker_connect(Worker* worker, uint timeout);
bool worker_server_run(Worker* worker);
void worker_quit();