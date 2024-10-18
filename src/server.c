//
// Created by ciuis on 08.10.2024.
//

#include "server.h"
#include "request.h"
#include "response.h"

#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t stop = false;

void sig_handler(int signo) {
    if (signo == SIGINT) stop = true;
}

int build_server_socket() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        perror("WSAStartup failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Socket opening failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    result = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if (result == SOCKET_ERROR) {
        perror("Socket options setup failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    result = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (result == SOCKET_ERROR) {
        perror("Bind to port failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    result = listen(server_socket, 5);
    if (result == SOCKET_ERROR) {
        perror("Listen socket failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

void handle_request(SOCKET client_socket) {
    char* buffer = calloc(1, MAX_LENGTH);
    if (buffer == NULL) {
        perror("Cannot allocate buffer");
        exit(EXIT_FAILURE);
    }

    int bytes_read = recv(client_socket, buffer, MAX_LENGTH - 1, 0);
    if (bytes_read < 0) {
        perror("Read from buffer failed");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_read] = 0;

    request* req = new_request();
    parse_request(buffer, req);
    printf("[REQUEST] %s %s\n", req->method, req->target);
    free(buffer);

    response* res = new_response();
    build_response(req, res);
    send_response(client_socket, res);

    closesocket(client_socket);
    destroy_request(req);
    destroy_response(res);
}

void run_server(SOCKET server_socket) {
    struct sockaddr_in client_address;
    int client_address_len = sizeof(client_address);

    while (!stop) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(server_socket, &fds);

        int result = select((int)server_socket + 1, &fds, NULL, NULL, NULL);
        if (result == SOCKET_ERROR) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        if (stop) break;
        if (!result) continue;

        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);

        handle_request(client_socket);
    }
}

int main(void) {
    SOCKET server_socket;
    signal(SIGINT, sig_handler);
    server_socket = build_server_socket();
    run_server(server_socket);
    closesocket(server_socket);
    WSACleanup();

    return EXIT_SUCCESS;
}