#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 50

int seq_num = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* client_socket) {
    int socket_fd = *((int*)client_socket);
    char buffer[1024];
    int bytes_received;

    // Create a unique filename based on connection ID
    char filename[20];
    pthread_mutex_lock(&lock);
    snprintf(filename, sizeof(filename), "book_%d.txt", seq_num);
    seq_num++;
    pthread_mutex_unlock(&lock);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        close(socket_fd);
        free(client_socket);
        pthread_exit(NULL);
    }

    while ((bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0)) > 0) {
        // Write received data to the file
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    close(socket_fd);
    free(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket, addr_len;
    struct sockaddr_in server_addr, client_addr;
    pthread_t thread_id;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        addr_len = sizeof(struct sockaddr_in);
        // Accept a new client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr,
                               (socklen_t*)&addr_len);
        if (client_socket == -1) {
            perror("Accepting client failed");
            continue;
        }

        // Create a new thread to handle the client
        int* new_socket = malloc(1);
        *new_socket = client_socket;
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_socket) <
            0) {
            perror("Thread creation failed");
            free(new_socket);
            continue;
        }

        printf("New client connected, assigned thread id: %ld\n", thread_id);
    }

    close(server_socket);
    return 0;
}
