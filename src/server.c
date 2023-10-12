#include "server.h"

void parse_cli(int argc, char** argv, int* port, char** pattern) {
    if (argc != 5) {
        handle_parse_error();
    } else {
        if (strcmp(argv[1], "-l") != 0)
            handle_parse_error();
        if (strcmp(argv[3], "-p") != 0)
            handle_parse_error();
        *port = atoi(argv[2]);
        *pattern = argv[4];
    }
}

void Init_Server(server_t* server, char* pattern, int port) {
    server->status = 0;
    server->sequence_id = 0;
    pthread_mutex_init(&server->lock, NULL);
    // Init list
    server->L = (list_t*)malloc(sizeof(list_t));
    if (server->L == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    Concurrent_List_Init(server->L, pattern);

    // Setup server socket
    struct sockaddr_in server_addr;
    // Create server socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    // Allow socket reuse
    setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &option,
               sizeof(option));
    if (server->server_socket == -1)
        handle_error("socket");

    // Create address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind address
    if (bind(server->server_socket, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == -1)
        handle_error("bind");

    // Listen on port
    if (listen(server->server_socket, MAXTHREADCOUNT) == -1)
        handle_error("listen");
    printf("Server listening on port: %d\n", port);
    printf("Search pattern: %s\n", pattern);
}

int Get_Sequence_Id_Server(server_t* server) {
    pthread_mutex_lock(&server->lock);
    int retval = server->sequence_id;
    server->sequence_id++;
    pthread_mutex_unlock(&server->lock);
    return retval;
}

// Release the socket
void Shutdown_Server(server_t* server) {
    printf("Closing Server\n");
    close(server->server_socket);
    Concurrent_List_Free(server->L);
    free(server->L);
    server->status = 1;
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
                    int* num_parsed_lines, list_t* L) {
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
            // printf("%s", content);

            Concurrent_List_Insert(L, vector->title, content);
            // Free content if it is not title
            if (content != vector->title)
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

void handle_io(int client_socket, list_t* L, server_t* server) {
    char buff[MAX];
    int read_len = 0;
    int num_parsed_lines = 0;
    string_vector_t* vector = (string_vector_t*)malloc(sizeof(string_vector_t));
    Vector_Init(vector, VECTORSIZE);

    // Recv data loop
    while (true) {
        // Clear buffer
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read_len = recv(client_socket, buff, sizeof(buff), 0);
        process_buffer(vector, buff, read_len, &num_parsed_lines, L);
        if (read_len == 0) {
            //Flush remaining buffer to file:
            if (vector->size != 0) {
                char* content = Vector_Flush(vector);
                Concurrent_List_Insert(L, vector->title, content);
                if (vector->title != content)
                    free(content);
            }
            break;
        }
    }
    // Write to file
    int thread_id = Get_Sequence_Id_Server(server);
    printf("Writing book_%d.txt\n", thread_id);
    Concurrent_List_Write_Book(L, vector->title, thread_id);
    // Collect Garbage
    close(client_socket);
    Vector_Free(vector);
    free(vector);
}

/*
Connection runnable task to be handled by each thread
*/
void* connection_runnable(void* arg) {
    runnable_params_t* params = (runnable_params_t*)arg;
    handle_io(*(params->client_socket), params->server->L, params->server);
    free(params->client_socket);
    free(params);
    return NULL;
}

void Run_Server(server_t* server) {
    int client_socket;
    socklen_t addr_size;
    struct sockaddr_in client_addr;

    pthread_t thr;

    while (true) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server->server_socket,
                               (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket == -1)
            handle_error("accept");
        int* new_socket = (int*)malloc(sizeof(int));
        *new_socket = client_socket;
        runnable_params_t* params =
            (runnable_params_t*)malloc(sizeof(runnable_params_t));
        params->server = server;
        params->client_socket = new_socket;

        pthread_create(&thr, NULL, connection_runnable, (void*)params);
        if (server->status == 1)
            break;
    }
}
