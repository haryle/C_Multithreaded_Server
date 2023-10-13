#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/linked_list.h"
#define FIXTURESIZE 10
#define BOOKSIZE 20000
#include <sys/types.h>
#include <unistd.h>

linked_list_t* L;

int total = 0;

int incorrect = 0;

int test_status = 0;

char* titles[FIXTURESIZE] = {
    "first_book.txt", "second_book.txt", "third_book.txt",   "fourth_book.txt",
    "fifth_book.txt", "sixth_book.txt",  "seventh_book.txt", "eighth_book.txt",
    "ninth_book.txt", "tenth_book.txt"};

void before_each(const char* test_name, char** pattern, char** title) {
    test_status = 0;
    total += 1;
    L = (linked_list_t*)malloc(sizeof(linked_list_t));
    List_Init(L, pattern, title);
    // printf("Begin test: %s\n", test_name);
}

void after_each(const char* test_name) {
    if (test_status != 0) {
        printf("Fail test: %s\n", test_name);
        incorrect += 1;
    }
    List_Free(L);
    free(L);
}

int assertEqualsStr(char* first, char* second) {
    return strcmp(first, second) == 0 ? 0 : 1;
}

int assertEqualsInt(int first, int second) {
    return first == second ? 0 : 1;
}

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
    if (fp == NULL) {
        printf("File does not exist: %s", dir);
        return;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("%s", line);
        content[count] = (char*)malloc(read + 1);
        memmove(content[count], line, read + 1);
        count++;
    }
    fclose(fp);
    if (line)
        free(line);
    *size = count;
}

void add_content_to_list(char* content[], int size) {
    for (int i = 0; i < size; i++)
        List_Insert(L, content[i]);
}

void free_book_content(char* content[], int size) {
    for (int i = 0; i < size; i++)
        free(content[i]);
}

void test_content_from_book_written_to_nodes(int id) {
    char* content[BOOKSIZE];
    int size = 0;
    read_file(id, content, &size);
    before_each(__func__, &content[0], &content[0]);
    add_content_to_list(content, size);
    node_t* current = L->head;
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

int main() {
    for (int id = 0; id < FIXTURESIZE; id++) {
        test_content_from_book_written_to_nodes(id);
    }
    printf("Pass: %d, Fail: %d\n", total - incorrect, incorrect);
}