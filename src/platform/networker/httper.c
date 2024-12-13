#pragma once
#include "../../../includes/buffer.h"
#include "worker.h"
#include "http.h"
#include "httper.h"

void http_url_parser(const char* url, HttpRequest* req){
    Buffer* real = buffer_create();
    char* first = strstr(url, "://");
    char* second = strstr(first + 3, "/"); //常数3是 "://" 的长度
    if(second == NULL){ //无后缀
        buffer_add(real, first + 3, strlen(url) - 3);
        buffer_add(req -> path, "/", 1);
    }else{
        int start = first - url, end = second - url;
        int length = end - start - 3;
        buffer_add(real, first + 3, length);
        buffer_add(req -> path, second, strlen(second));
    }
    buffer_add_buffer(req -> host, real); //填充host
    buffer_delete(real);
}

void http_packet_init(Worker* worker){
    HttpPacket* packet = (HttpPacket *)worker -> context;
    HttpRequest* req = (HttpRequest *)packet -> request;
    Buffer* res = request_build(req);
    
    worker_send(worker, res -> ptr); //发送请求包
    buffer_delete(res);
}

void http_packet_process(Worker* worker){
    if(worker -> status != WORKER_STATUS_CLOSE) return; //确保数据被完全接收
    HttpPacket* packet = (HttpPacket *)worker -> context;
    HttpResponse* res = response_create(worker -> data -> ptr);
    (packet -> hook)(worker, res);
    buffer_clear(worker -> data);
    request_free(packet -> request);
    response_free(res);
    safe_free(packet);
}

Worker* httper_create(const char* addr, uint port, void (*callback)(Worker *, HttpResponse *)){
    HttpPacket* packet = (HttpPacket *)safe_malloc(sizeof(HttpPacket));
    HttpRequest* request = packet -> request = request_create(addr, port);
    http_url_parser(addr, request);
    packet -> hook = callback;
    int error;
    Worker* worker = worker_create(request -> host -> ptr, port, &error);
    if(error == WORKER_ERROR_DOMAIN_PARSER) return NULL;

    worker -> context = (void *)packet;
    worker -> onConnect = http_packet_init;
    worker -> onMessage = http_packet_process;
}
