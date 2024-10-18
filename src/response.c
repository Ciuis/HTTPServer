//
// Created by user on 12.10.2024.
//

#include "response.h"
#include <fcntl.h>

response* new_response() {
    return (response*)calloc(1, sizeof(response));
}

void destroy_response(response* res) {
    free(res);
}

void build_response(request* req, response* res) {
    char relative_path[MAX_LENGTH];
    sprintf(relative_path, ".%s", req->target);

    int file = open(relative_path, O_RDONLY);
    if (file < 0) {
        res->status = NOT_FOUND;
    } else {
        res->status = OK;
        read(file, res->body, BUFFER_SIZE - 1);
        close(file);
    }
    short status_code = HTTP_STATUS_CODE[res->status];
    printf("[RESPONSE] %s %s %h\n", req->method, req->target, status_code);
}

void send_response(SOCKET client_socket, response* res) {
    char buffer[BUFFER_SIZE] = {0};
    short status_code = HTTP_STATUS_CODE[res->status];
    const char* status_str = HTTP_STATUS_STR[res->status];
    sprintf(buffer, "%s %hd %s\r\n\r\n%s", HTTP_VERSION, status_code, status_str, res->body);

    int n = send(client_socket, buffer, (int)strnlen(buffer, BUFFER_SIZE), 0);
    if (n < 0) {
        perror("Error writing socket");
    }
}