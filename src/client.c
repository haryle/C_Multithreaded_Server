#include <arpa/inet.h>  // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // bzero()
#include <sys/socket.h>
#include <unistd.h>  // read(), write(), close()
#define MAX 80
#ifndef PORT
    #define PORT 8080
#endif
#define SA struct sockaddr

void parse_cli(int argc, char** argv, int* port, char** filename) {
    if (argc != 5) {
        printf("Usage: client -p <port_number> -f <file_name_in_resoures>\n");
        exit(EXIT_FAILURE);
    } else {
        if ((strcmp(argv[1], "-p") != 0) || (strcmp(argv[3], "-f") != 0)) {
            printf(
                "Usage: client -p <port_number> -f <file_name_in_resoures>\n");
            exit(EXIT_FAILURE);
        }
        *port = atoi(argv[2]);
        *filename = argv[4];
    }
}

void read_file(int sockfd, char* filename) {
    int count = 0;
    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;
    char dir[100];
    sprintf(dir, "resources/%s", filename);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL) {
        printf("File does not exist: %s", dir);
        return;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        write(sockfd, line, read);
    }
    fclose(fp);
    if (line)
        free(line);
}

int main(int argc, char* argv[]) {
    // Read CLI
    int port;
    char* filename;
    parse_cli(argc, argv, &port, &filename);

    // Create socket
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        // printf("socket creation failed...\n");
        exit(0);
    } else
        // printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        // printf("connection with the server failed...\n");
        exit(0);
    } else
        // printf("connected to the server..\n");

    // function for chat
    read_file(sockfd, filename);

    // close the socket
    close(sockfd);
}