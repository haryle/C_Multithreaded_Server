#include "../src/server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FIXTURESIZE 10
#define BOOKSIZE 20000
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int assertEqualsStr(char* first, char* second) {
    return strcmp(first, second) == 0 ? 0 : 1;
}

int assertEqualsInt(int first, int second) {
    return first == second ? 0 : 1;
}

server_t* server;

int total = 0;

int incorrect = 0;

int test_status = 0;

char* titles[FIXTURESIZE] = {
    "first_book.txt", "second_book.txt", "third_book.txt",   "fourth_book.txt",
    "fifth_book.txt", "sixth_book.txt",  "seventh_book.txt", "eighth_book.txt",
    "ninth_book.txt", "tenth_book.txt"};

void* thread_run(void* arg) {
    // Run
    Run_Server(server);
    return NULL;
}

void before_each(const char* test_name, char* pattern) {
    printf("Begin test: %s\n", test_name);
    test_status = 0;
    total += 1;
    // Initialise server
    server = (server_t*)malloc(sizeof(server_t));
    if (server == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    Init_Server(server, pattern, 8080);
    //Run server as background thread
    pthread_t thr;
    pthread_create(&thr, NULL, thread_run, NULL);
}

void after_each(const char* test_name) {
    if (test_status != 0) {
        printf("Fail test: %s\n", test_name);
        incorrect += 1;
    }
    Shutdown_Server(server);
}

void exec_cmd(char* arg) {
    int pid = fork();
    if (!pid) {
        execvp(arg[0], arg);
    }
    int status = 0;
    waitpid(pid, &status, 0);

    // Check if the child process terminated normally
    if (WIFEXITED(status)) {
        // Get the exit status of the child process
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0)
            test_status += 1;
    } else {
        // Child process did not terminate normally
        printf("Child process did not exit normally\n");
    }
}

void upload_file(char* filename) {
    char* arg[] = {"pymake", "run", "send_file", filename, NULL};
    exec_cmd(arg);
}

void compare() {
    char* arg[] = {"python3", "compare.py", NULL};
    exec_cmd(arg);
}

void upload_to_server(char* filename) {}

void test_single_file_upload(int i) {
    before_each(__func__, "S");
    upload_file(titles[i]);

    after_each(__func__);
}

int main() {
    test_single_file_upload();
    printf("Pass: %d, Fail: %d\n", total - incorrect, incorrect);
}