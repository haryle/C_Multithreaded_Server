#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "../utils/list.h"
#define FIXTURESIZE 10
#define _GNU_SOURCE

typedef struct ___thread_arg_t {
    list_t* list;
    char* title;
} thread_arg_t;

char* titles[FIXTURESIZE] = {
    "compsci_4416.txt", "compsci_4417.txt", "compsci_4807.txt",
    "compsci_4811.txt", "compsci_4812.txt", "compsci_7007.txt",
    "compsci_7039.txt", "compsci_7059.txt", "compsci_7064.txt",
    "compsci_7092.txt",
};

void* thread_insert(void* arg) {
    thread_arg_t* argv = (thread_arg_t*)arg;

    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;

    char dir[100];
    sprintf(dir, "resources/%s", argv->title);
    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        Concurrent_List_Insert(argv->list, argv->title, line);
    }
    fclose(fp);
    if (line)
        free(line);
    return NULL;
}

list_t* BeforeEach() {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    Concurrent_List_Init(list);
    // Add data
    thread_arg_t args[FIXTURESIZE];
    pthread_t thr[FIXTURESIZE];
    for (int i = 0; i < FIXTURESIZE; i++) {
        args[i] = (thread_arg_t){list, titles[i]};
    }
    // Fork
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_create(&thr[i], NULL, thread_insert, (void*)&args[i]);
    }
    // Join
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_join(thr[i], NULL);
    }
    return list;
}

void AfterEach(list_t* L) {
    Concurrent_List_Free(L);
    free(L);
}

int test_list_get_book(char* title) {
    int success = 0;
    list_t* L = BeforeEach();

    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;

    char dir[100];
    sprintf(dir, "resources/%s", title);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return 1;

    linked_list_t* list = Concurrent_List_Get_Book(L, title);

    node_t* current = List_Head(list);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (current == NULL) {
            printf("Current node is NULL. Not supposed to be NULL\n");
            success = 1;
            break;
        }
        if (strcmp(current->value, line) != 0) {
            printf("Different: %s, %s\n", current->value, line);
            success = 1;
            break;
        }
        current = current->book_next;
    }
    if (current != NULL) {
        printf("Current node is NOT NULL. Supposed to be NULL\n");
        success = 1;
    }

    fclose(fp);
    if (line)
        free(line);

    AfterEach(L);
    if (success == 1)
        printf("Fail: test_list_get_book with title: %s\n", title);
    return success;
}

int test_list_contains_node_inefficient(char* title) {
    int success = 0;
    list_t* L = BeforeEach();

    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;

    char dir[100];
    sprintf(dir, "resources/%s", title);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return 1;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (!Concurrent_List_Contains_Inefficient(L, title, line)) {
            printf("List does not contain line: %s from title: %s\n", line,
                   title);
            success = 1;
            break;
        }
    }
    fclose(fp);
    if (line)
        free(line);

    AfterEach(L);
    if (success == 1)
        printf("Fail: test_list_contains_node_inefficient with title: %s\n",
               title);
    return success;
}

int test_list_contains_node_efficient(char* title) {
    int success = 0;
    list_t* L = BeforeEach();

    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;

    char dir[100];
    sprintf(dir, "resources/%s", title);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return 1;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (!Concurrent_List_Contains(L, title, line)) {
            printf("List does not contain line: %s from title: %s\n", line,
                   title);
            success = 1;
            break;
        }
    }
    fclose(fp);
    if (line)
        free(line);

    AfterEach(L);
    if (success == 1)
        printf("Fail: test_list_contains_node_efficient with title: %s\n",
               title);
    return success;
}

int main() {
    int failed_tests = 0;
    int run_tests = 0;
    for (int i = 0; i < FIXTURESIZE; i++) {
        run_tests += 1;
        failed_tests += test_list_get_book(titles[i]);
    }

    for (int i = 0; i < FIXTURESIZE; i++) {
        run_tests += 1;
        failed_tests += test_list_contains_node_efficient(titles[i]);
    }

    for (int i = 0; i < FIXTURESIZE; i++) {
        run_tests += 1;
        failed_tests += test_list_contains_node_inefficient(titles[i]);
    }

    printf("Passed tests: %d, Failed tests: %d\n", run_tests - failed_tests,
           failed_tests);
}