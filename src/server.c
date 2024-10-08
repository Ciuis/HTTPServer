//
// Created by ciuis on 08.10.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 4096

void handle_request(int client_socket) {
    char request[MAX_REQUEST_SIZE];
    char response[BUFFER_SIZE];
    char* file_path = NULL;
    int file_fd;
    ssize_t bytes_read;

    // Читаем запрос от клиента
    bytes_read = recv(client_socket, request, MAX_REQUEST_SIZE - 1, 0);
    if (bytes_read < 0) {
        int error = WSAGetLastError();
        printf("Read error...: %d\n", error);
        closesocket(client_socket);
        return;
    }
    request[bytes_read] = '\0';

    // Парсим запрос
    if (strncmp(request, "GET ", 4) == 0) {
        file_path = request + 4;
        char* end_of_path = strchr(file_path, ' ');
        if (end_of_path) *end_of_path = '\0';
    }

    // File path logging
    if (file_path && *file_path) {
        printf("Requested file: %s\n", file_path);
    } else {
        printf("Invalid request\n");
    }

    // Открытие файла и отправка содержимого клиенту
    if (file_path && *file_path) {
        snprintf(file_path, MAX_REQUEST_SIZE, "resources%s", file_path);

        file_fd = open(file_path, O_RDONLY);
        if (file_fd < 0) {
            snprintf(response, BUFFER_SIZE, "HTTP/1.1 404 Not Found\r\n\r\n");
            send(client_socket, response, strlen(response), 0);
        } else {
            snprintf(response, BUFFER_SIZE, "HTTP/1.1 200 OK\r\n\r\n");
            send(client_socket, response, strlen(response), 0);
            while ((bytes_read = read(file_fd, response, BUFFER_SIZE - 1)) > 0) {
                send(client_socket, response, bytes_read, 0);
            }
            close(file_fd);
        }
    } else {
        snprintf(response, BUFFER_SIZE, "HTTP/1.1 400 Bad Request\r\n\r\n");
        send(client_socket, response, strlen(response), 0);
    }
    closesocket(client_socket);
}

int main(void) {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Инициализация Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Создаем сокет
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        int error = WSAGetLastError();  // Получаем код ошибки
        printf("Cannot create socket...: %d\n", error);
        exit(EXIT_FAILURE);
    }

    // Адрес сервера
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Привязываем сокет к адресу
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        int error = WSAGetLastError();
        printf("Cannot bind socket...: %d\n", error);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Слушаем сокет
    if (listen(server_socket, 5) < 0) {
        int error = WSAGetLastError();
        printf("Cannot listen...: %d\n", error);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Принимаем входящие соединения
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket < 0) {
            int error = WSAGetLastError();
            printf("Cannot accept connection...: %d\n", error);
            continue;
        }

        handle_request(client_socket);
    }

    close(server_socket);
    WSACleanup();
    return 0;
}