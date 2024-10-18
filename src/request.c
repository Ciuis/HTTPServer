//
// Created by user on 12.10.2024.
//

#include "request.h"
#include "server.h"

request* new_request() {
    return (request*)calloc(1, sizeof(request));
}

void destroy_request(request* req) {
    free(req);
}

void parse_request(const char* buffer, request* req) {
    sscanf(buffer, "%s %s", req->method, req->target);
}