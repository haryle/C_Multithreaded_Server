#include "src/server.h"

int main(int argc, char** argv) {
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

    // Run
    Run_Server(server);
}