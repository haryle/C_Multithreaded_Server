#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "list.h"
#include "string_vector.h"

#define LISTEN_BACKLOG 50
#define MAX 100
#define VECTORSIZE 1024
#define MAXTHREADCOUNT 50
#define FREQUENCY 5
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

/*
 Structure to represent the server
 Fields:
    server's socket
    pointer to a list data structure
    server's status
    sequence identifies
    mutex for thread synchronisation
        -> A mutex is like a lock that threads use to prevent them from messing up shared information. 
*/
typedef struct ___server_t {
    int server_socket;      // Server's socket
    list_t* L;              // Pointer to a list data structure
    int status;             // Server's status
    int sequence_id;        // Sequence identifier
    pthread_mutex_t lock;   // Mutex for thread synchronisation
} server_t;

/*
 Structure to hold parameters for a runnable task
 Fields:
    pointer to a server
    pointer to a client socket
*/
typedef struct ___runnable_params_t {
    server_t* server;       // Pointer to a server
    int* client_socket;     // Pointer to a client socket
} runnable_params_t;

/*
 Initialize the server with the provided search pattern and port.
 Args: 
    server: Pointer to the server structure to initialize.
    pattern: The search pattern to use.
    port: The port number to listen on.
*/
void Init_Server(server_t* server, char* pattern, int port);

/*
 Shutdown the server, releasing allocated resources.
 Args:
    server: Pointer to the server structure to shutdown.
*/
void Shutdown_Server(server_t* server);

/*
 Runnable function for handling client connections.
 Args: 
    arg: Pointer to the runnable parameters.
*/
void* connection_runnable(void* arg);

/*
 Start the server and listen for incoming connections.
 Args: 
    server: Pointer to the server to run.
*/
void Run_Server(server_t* server);

/*
 Parse command-line arguments to set the port and search pattern.
 Args:
    argc: Number of command-line arguments.
    argv: Array of command-line argument strings.
    port: Pointer to store the port number.
    pattern: Pointer to store the search pattern.
*/
void parse_cli(int argc, char** argv, int* port, char** pattern);

/*
 Runnable function for analyzing the server's list.
 Args:
    server: Pointer to the server to analyze.
*/
void* thread_analyse_server_list(void* server);
