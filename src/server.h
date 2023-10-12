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
#define MAX 100
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

typedef struct ___server_t {
    int server_socket;
    list_t* L;
    int status;
    int sequence_id;
    pthread_mutex_t lock;
} server_t;

typedef struct ___runnable_params_t {
    server_t* server;
    int client_socket;
} runnable_params_t;

void Init_Server(server_t* server, char* pattern, int port);

void Shutdown_Server(server_t* server);

void* connection_runnable(void* arg);

void Run_Server(server_t* server);

void parse_cli(int argc, char** argv, int* port, char** pattern);