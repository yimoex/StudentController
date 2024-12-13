#pragma once
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include <time.h>
#include "worker.h"
#include "http.h"
#include "../../../includes/mem.h"
#include "../../../includes/error.h"

void worker_init(){
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(result != 0) call_fatel("worker: 无法导入windows dll");
}

bool worker_isdomain(const char* data){
    uint max = strlen(data);
    for(uint i = 0;i < max;i++)
        if(('9' < data[i] || data[i] < '0') && data[i] != '.') return true;
    return false;
}

char* worker_domain2ip(const char* domain){
    int rec;
    struct hostent* info;
    struct in_addr* address;
    info = gethostbyname(domain);
    if(info == NULL) return NULL;
    address = (struct in_addr* )info -> h_addr_list[0];
    return inet_ntoa(*address);
}

Worker* worker_base_create(const char* addr, uint port){
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == INVALID_SOCKET){
        printf("Error: %d\n", WSAGetLastError());
        call_fatel("无法创建worker");
        return NULL;
    }
    Worker* worker = (Worker *)safe_malloc(sizeof(Worker));
    worker -> status = WORKER_STATUS_WAITTING;
    worker -> data = buffer_create_size(WORKER_READ_BUFFER_SIZE);
    worker -> cache = (char *)safe_malloc(WORKER_READ_BUFFER_SIZE * sizeof(char));
    worker -> onMessage = NULL;
    worker -> onClose = NULL;
    worker -> onConnect = NULL;
    worker -> onConnectError = NULL;
    worker -> sockid = sock;
    (worker -> local).sin_family = AF_INET;
    (worker -> local).sin_port = htons(port);
    strcpy(worker -> info.addr, addr);
    worker -> info.port = port;
    return worker;
}

Worker* worker_create(const char* addr, uint port, int* error){
    Worker* worker = worker_base_create(addr, port);
    if(!worker_isdomain(addr)){
        (worker -> local).sin_addr.s_addr = inet_addr(addr);
        strcpy(worker -> info.ip, addr);
        return worker;
    }
    char* new = worker_domain2ip(addr);
    if(new == NULL){
        *error = WORKER_ERROR_DOMAIN_PARSER;
        return NULL;
    }
    strcpy(worker -> info.ip, new);
    //call_fatel("worker无法解析域名");
    (worker -> local).sin_addr.s_addr = inet_addr(new);
    return worker;
}

Worker* worker_server(const char* addr, uint port){
    Worker* worker = worker_base_create(addr, port);
    (worker -> local).sin_addr.S_un.S_addr = INADDR_ANY;
    if(bind(worker -> sockid, (struct sockaddr*)&(worker -> local), sizeof((worker -> local))) == SOCKET_ERROR) call_fatel("worker无法绑定地址或端口");
    if(listen(worker -> sockid, 4) == SOCKET_ERROR) call_fatel("worker无法进行监听");
    return worker;
}

void worker_close(Worker* worker, int errorCode){
    if(worker -> status == WORKER_STATUS_CLOSE) return;
    worker -> status = WORKER_STATUS_CLOSE;
    if(worker -> onClose != NULL) (worker -> onClose)(worker, errorCode);
    closesocket(worker -> sockid);
    buffer_delete(worker -> data);
    safe_free(worker -> cache);
    safe_free(worker);
    worker = NULL;
}

int worker_send(Worker* worker, const char* data){
    if(worker -> status == WORKER_STATUS_CLOSE) return -1;
    uint length = strlen(data);
    return send(worker -> sockid, data, length, 0);
}

int worker_read(Worker* worker, SOCKET client){
    if(worker -> status == WORKER_STATUS_CLOSE) return -1;
    int rec = recv(client, worker -> cache, WORKER_READ_BUFFER_SIZE, 0);
    void (*callback)(Worker *) = worker -> onMessage;
    if(rec == 0){
        worker -> status = WORKER_STATUS_CLOSE;
        if(!buffer_empty(worker -> data)) (callback)(worker);
        worker_close(worker, WSAGetLastError());
        return -1;
    }else if(rec < 0){
        if(WSAGetLastError() == WSAEWOULDBLOCK) return 0;
        worker_close(worker, WSAGetLastError());
        return -1;
    }
    buffer_add(worker -> data, worker -> cache, rec);
    return rec;
}

bool worker_connect(Worker* worker, uint timeout){
    if(worker == NULL) return false;
    uint start = time(0), clock;
    SOCKET client = worker -> sockid;
    int rec, signal = 0;
    u_long mode = 1;
    void (*callback)(Worker *) = worker -> onMessage;
    rec = connect(client, (struct sockaddr*)&worker -> local, sizeof(worker -> local));
    if(rec == -1 || callback == NULL){
        int rec = WSAGetLastError();
        if(worker -> onConnectError != NULL) (worker -> onConnectError)(worker, rec);
        worker_close(worker, rec);
        return false;
    }
    worker -> status = WORKER_STATUS_CONNECTED;
    if(worker -> onConnect != NULL) (worker -> onConnect)(worker);
    ioctlsocket(client, FIONBIO, &mode);

    while(1){
        clock = time(0) - start;
        if(clock > timeout) return false;
        rec = worker_read(worker, client);
        if(rec == 0) continue;
        if(rec > 0) (callback)(worker);
        else break;
        if(worker == NULL || worker -> status == WORKER_STATUS_CLOSE){
            if(!buffer_empty(worker -> data)) (callback)(worker);
            break;
        }
        usleep(100);
    }
}

//该函数暂未维护
bool worker_server_run(Worker* worker){
    if(worker == NULL) return false;
    int length = sizeof(worker -> remote);
    SOCKET client;
    void (*callback)(Worker *) = worker -> onMessage;
    client = accept(worker -> sockid, (struct sockaddr*)&worker -> remote, &length);
    if(callback == NULL){
        worker_close(worker, -1);
        return true; //没有回调直接结束
    }
    int rec;
    while(1){
        rec = worker_read(worker, client);
        callback(worker);
        if(rec <= 0) return true;
        usleep(1000);
    }
}

void worker_quit(){
    WSACleanup();
}