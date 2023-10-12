#include "server.h"

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

    // Get seq num
    pthread_mutex_lock(&lock);
    int file_name = seq_num;
    seq_num++;
    pthread_mutex_unlock(&lock);

    //Make file name
    if (file_name < 10)
        snprintf(filename, sizeof(filename), "book_0%d.txt", seq_num);
    else
        snprintf(filename, sizeof(filename), "book_%d.txt", seq_num);

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

int main(int argc, char* argv[]) {
    // Parse CLI
    int port;
    char* pattern;
    parse_cli(argc, argv, &port, &pattern);

    // Initialise server
    server_t* server = (server_t*)malloc(sizeof(server_t));
    if (server == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    Init_Server(server, pattern, port);

    int client_socket, addr_len;
    struct sockaddr_in client_addr;
    pthread_t thread_id;

    while (1) {
        addr_len = sizeof(struct sockaddr_in);
        // Accept a new client connection
        client_socket =
            accept(server->server_socket, (struct sockaddr*)&client_addr,
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

    close(server->server_socket);
    return 0;
}
