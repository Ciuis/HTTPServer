//
// Created by user on 09.10.2024.
//

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_LENGTH 1024 * 1024
#define BUFFER_SIZE 1024
#define HTTP_VERSION "HTTP/1.1"

typedef enum {
    OK = 200,
    NOT_FOUND = 404,
    ERROR_ = 500
} http_status;

static const char* HTTP_STATUS_STR[] = {
        [OK] = "OK",
        [NOT_FOUND] = "Not Found",
        [ERROR_] = "Internal Server Error"
};

static const short HTTP_STATUS_CODE[] = {
        [OK] = 200,
        [NOT_FOUND] = 404,
        [ERROR_] = 500
};

void sig_handler(int signo);
int make_server_socket();
void run_server(SOCKET server_socket);
void handle_request(SOCKET client_socket);

#endif