//
// Created by user on 12.10.2024.
//

#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>

typedef struct {
    char method[16];
    char target[256];
} request;

request* new_request();
void destroy_request(request* req);
void parse_request(const char* buffer, request* req);


#endif
