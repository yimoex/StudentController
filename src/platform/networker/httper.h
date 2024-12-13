#pragma once
#include "worker.h"
#include "http.h"

typedef struct {
    HttpRequest* request;
    void (*hook)(Worker* worker, HttpResponse* data);
} HttpPacket;

void http_url_parser(const char* url, HttpRequest* req);
void http_packet_init(Worker* worker);
void http_packet_process(Worker* worker);
Worker* httper_create(const char* addr, uint port, void (*callback)(Worker *, HttpResponse *));