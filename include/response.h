//
// Created by user on 12.10.2024.
//

#ifndef RESPONSE_H
#define RESPONSE_H

#include "server.h"
#include "request.h"

typedef struct {
    http_status status;
    char body[MAX_LENGTH];
} response;

response* new_response();
void destroy_response(response* res);
void build_response(request* req, response* res);
void send_response(SOCKET client_socket, response* res);

#endif
