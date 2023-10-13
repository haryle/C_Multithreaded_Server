#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/map.h"
#define FIXTURESIZE 10
#define BOOKSIZE 100
#include <sys/types.h>
#include <unistd.h>

typedef struct ___thread_arg_t {
    char** content;
    int size;
} thread_arg_t;

map_t* M;

int total = 0;

int incorrect = 0;

int test_status = 0;

char* contents[4] = {
    "Please check the Project Gutenberg web pages for current donation methods "
    "and addresses.\n",
    "Donations are accepted in a number of other ways including checks, online "
    "payments and credit card donations.\n",
    "To donate, please visit: www.gutenberg.org/donate.\n",
    "For forty years, he produced and distributed Project Gutenberg™ eBooks "
    "with only a loose network of volunteer support.\n",
};

char* titles[11] = {"compsci_4416.txt", "compsci_4417.txt", "compsci_4807.txt",
                    "compsci_4811.txt", "compsci_4812.txt", "compsci_7007.txt",
                    "compsci_7039.txt", "compsci_7059.txt", "compsci_7064.txt",
                    "compsci_7092.txt", "compsci_7076.txt"};

void before_each(const char* test_name, char** pattern) {
    test_status = 0;
    total += 1;
    M = (map_t*)malloc(sizeof(map_t));
    Map_Init(M, pattern);
    // printf("Begin test: %s\n", test_name);
}

void after_each(const char* test_name) {
    if (test_status != 0) {
        printf("Fail test: %s\n", test_name);
        incorrect += 1;
    }
    Map_Free(M);
    free(M);
}

int assertEqualsStr(char* first, char* second) {
    return strcmp(first, second) == 0 ? 0 : 1;
}

int assertEqualsInt(int first, int second) {
    return first == second ? 0 : 1;
}

/*
Read file into character contents

Arguments:
    id: fixture id 
    content: string array of book contents 
    size: size of content 

Note:
    content needs to be freed after use from 0 to 
    size - 1
*/
void read_file(int id, char* content[], int* size) {
    int count = 0;
    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;
    char* title = titles[id];
    char dir[100];
    sprintf(dir, "resources/%s", title);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return;
    while ((read = getline(&line, &len, fp)) != -1) {
        content[count] = (char*)malloc(read + 1);
        memmove(content[count], line, read + 1);
        count++;
    }
    fclose(fp);
    if (line)
        free(line);
    *size = count;
}

void add_content_to_map(char* content[], int size) {
    char* title = content[0];
    for (int i = 0; i < size; i++) {
        Map_Insert(M, title, content[i]);
    }
}

void free_book_content(char* content[], int size) {
    for (int i = 0; i < size; i++)
        free(content[i]);
}

void* thread_run(void* arg) {
    thread_arg_t* argv = (thread_arg_t*)arg;
    add_content_to_map(argv->content, argv->size);
    return NULL;
}

void test_content_from_book_written_to_nodes(int id) {
    char* content[BOOKSIZE];
    int size = 0;
    read_file(id, content, &size);
    before_each(__func__, &content[0]);
    add_content_to_map(content, size);
    node_t* current = Map_Get(M, content[0])->head;
    for (int i = 0; i < size; i++) {
        if (current == NULL) {
            test_status += 1;
            break;
        }
        test_status += assertEqualsStr(content[i], current->content);
        current = current->book_next;
    }
    after_each(__func__);
    free_book_content(content, size);
}

void test_content_from_books_parallel() {
    pthread_t thr[FIXTURESIZE];
    char* book_contents[FIXTURESIZE][BOOKSIZE];
    int sizes[FIXTURESIZE];
    thread_arg_t args[FIXTURESIZE];
    for (int i = 0; i < FIXTURESIZE; i++) {
        read_file(i, book_contents[i], &sizes[i]);
        args[i] = (thread_arg_t){book_contents[i], sizes[i]};
    }

    before_each(__func__, &contents[0]);
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_create(&thr[i], NULL, thread_run, &args[i]);
    }

    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_join(thr[i], NULL);
    }

    for (int i = 0; i < FIXTURESIZE; i++) {
        char* title = args->content[0];
        test_status += assertEqualsInt(Map_Contains(M, title), 1);
        char** content = args->content;
        int size = args->size;
        node_t* current = Map_Get(M, title)->head;
        for (int j = 0; j < size; j++) {
            if (current == NULL) {
                test_status += 1;
                break;
            }
            test_status += assertEqualsStr(content[j], current->content);

            current = current->book_next;
        }
    }

    after_each(__func__);

    for (int i = 0; i < FIXTURESIZE; i++)
        free_book_content(book_contents[i], sizes[i]);
}

int main() {
    for (int id = 0; id < FIXTURESIZE; id++) {
        test_content_from_book_written_to_nodes(id);
    }
    // Ensure nothing wrong happens with multi-threading
    for (int i = 0; i < 1000; i++) {
        test_content_from_books_parallel();
    }
    printf("Pass: %d, Fail: %d\n", total - incorrect, incorrect);
}