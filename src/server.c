#include <arpa/inet.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../utils/list.h"
#ifndef PORT
    #define PORT 8080
#endif
#define LISTEN_BACKLOG 50
#define MAX 10
#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int server_socket;
list_t List;

// Release the socket
void shutdown_server() {
    printf("Closing Server\n");
    close(server_socket);
}

// Close server socket, free up resources
void handle_SIGINT(int sig) {
    shutdown_server();
    exit(0);
}

void func(int connfd) {
    char buff[MAX];
    int read_len;
    // infinite loop for chat
    while (true) {
        signal(SIGINT, handle_SIGINT);
        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read_len = recv(connfd, buff, sizeof(buff), 0);
        // print buffer which contains the client contents
        printf("Received: %s\n", buff);
        printf("Read Length: %d\n", read_len);
        if (read_len == 0) {
            printf("Breaking Loop\n");
            break;
        }
    }
    printf("Closing client socket\n");
    close(connfd);
}

int main(void) {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    // Allow socket reuse
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option,
               sizeof(option));
    if (server_socket == -1)
        handle_error("socket");

    // Create address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind address
    if (bind(server_socket, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == -1)
        handle_error("bind");

    // Listen on port
    if (listen(server_socket, LISTEN_BACKLOG) == -1)
        handle_error("listen");
    printf("Server listening on port: %d\n", (int)PORT);

    // Init list
    Concurrent_List_Init(&List);

    //=============================================================
    //                  WHILE LOOP
    //=============================================================

    // /* Now we can accept incoming connections one
    //           at a time using accept(2). */
    addr_size = sizeof(client_addr);
    client_socket =
        accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    if (client_socket == -1)
        handle_error("accept");

    func(client_socket);
    shutdown_server();
}