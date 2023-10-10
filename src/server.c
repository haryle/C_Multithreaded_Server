#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../utils/list.h"
#include "../utils/string_vector.h"
#define LISTEN_BACKLOG 50
#define MAX 10
#define VECTORSIZE 1024
#define MAXTHREADCOUNT 50
#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#define handle_parse_error()                                            \
    {                                                                   \
        printf("Usage: server -l <port_number> -p <search_pattern>\n"); \
        exit(EXIT_FAILURE);                                             \
    }

int server_socket;  // Server socket file descriptor
list_t List;        // Concurrent List Data Structure

typedef struct ___runnable_param_t {
    int client_socket;
    int thread_id;
} runnable_param_t;

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

/*
Process buffer at every recv call.

Buffer is separated by the new line character. Every new line is added to the
concurrent list in a sequential order. The first read line is treated as the 
title 

Arguments:
    vector: string builder object 
    buffer: filled buffer
    size: size of buffer 
    title: book title ptr 
    num_parsed_line: ptr to number of lines that have been parsed
*/
void process_buffer(string_vector_t* vector, char* buffer, int size,
                    char** title, int* num_parsed_lines) {
    char* content;
    char temp[size];
    int temp_idx = 0;
    // Check for new line
    for (int i = 0; i < size; i++) {
        temp[temp_idx] = buffer[i];
        temp_idx++;
        if (buffer[i] == '\n') {
            //Complete the current line and append to vector
            temp[temp_idx] = '\0';
            Vector_Append(vector, temp);
            //Insert to concurrent list
            content = Vector_Flush(vector);
            // Display to stdout new node:
            printf("%s", content);
            if (*num_parsed_lines == 0)
                *title = content;
            Concurrent_List_Insert(&List, *title, content);
            // Free content if it is not title
            if (*num_parsed_lines != 0)
                free(content);
            *num_parsed_lines += 1;
            //Reset index:
            temp_idx = 0;
        }
    }
    // Add remaining temp buffer:
    temp[temp_idx] = '\0';
    Vector_Append(vector, temp);
}

/*
Connection runnable task to be handled by each thread
*/
void* connection_runnable(void* arg) {
    runnable_param_t* argv = (runnable_param_t*)arg;
    int connfd = argv->client_socket;
    char buff[MAX];
    int read_len, num_parsed_lines = 0;
    char* title = NULL;
    string_vector_t* vector = (string_vector_t*)malloc(sizeof(string_vector_t));
    Vector_Init(vector, VECTORSIZE);
    // Recv data loop
    while (true) {
        // Clear buffer
        bzero(buff, MAX);
        // read the message from client and copy it in buffer
        read_len = recv(connfd, buff, sizeof(buff), 0);
        // process the buffer
        process_buffer(vector, buff, read_len, &title, &num_parsed_lines);
        if (read_len == 0) {
            printf("Breaking Loop\n");
            break;
        }
    }
    // Write to file
    Concurrent_List_Write_Book(&List, title, argv->thread_id);
    // Collect Garbage
    printf("Closing client socket\n");
    close(connfd);
    Vector_Free(vector);
    free(vector);
    if (title != NULL)
        free(title);
    return NULL;
}

int main(int argc, char** argv) {
    // Parse CLI
    int port;
    char* pattern;
    if (argc != 5) {
        handle_parse_error();
    } else {
        if (strcmp(argv[1], "-l") != 0)
            handle_parse_error();
        if (strcmp(argv[3], "-p") != 0)
            handle_parse_error();
        port = atoi(argv[2]);
        pattern = argv[4];
    }
    // Initialise arguments
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
    server_addr.sin_port = htons(port);

    // Bind address
    if (bind(server_socket, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == -1)
        handle_error("bind");

    // Listen on port
    if (listen(server_socket, LISTEN_BACKLOG) == -1)
        handle_error("listen");
    printf("Server listening on port: %d\n", port);
    printf("Search pattern: %s\n", pattern);

    // Init list
    Concurrent_List_Init(&List, pattern);

    //=============================================================
    //                  WHILE LOOP
    //=============================================================

    // /* Now we can accept incoming connections one
    //           at a time using accept(2). */
    int thread_id = 0;
    while (true) {
        signal(SIGINT, handle_SIGINT);
        addr_size = sizeof(client_addr);
        client_socket =
            accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket == -1)
            handle_error("accept");
        runnable_param_t param = (runnable_param_t){client_socket, thread_id++};
        pthread_t thr;
        pthread_create(&thr, NULL, connection_runnable, (void*)&param);
    }
}