#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/linked_list.h"
#define FIXTURESIZE 10
#define BOOKSIZE 100
#include <sys/types.h>
#include <unistd.h>

linked_list_t* L;

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

char* titles[FIXTURESIZE] = {
    "compsci_4416.txt", "compsci_4417.txt", "compsci_4807.txt",
    "compsci_4811.txt", "compsci_4812.txt", "compsci_7007.txt",
    "compsci_7039.txt", "compsci_7059.txt", "compsci_7064.txt",
    "compsci_7092.txt",
};

void before_each(const char* test_name, char** pattern, char** title) {
    test_status = 0;
    total += 1;
    L = (linked_list_t*)malloc(sizeof(linked_list_t));
    List_Init(L, pattern, title);
    printf("Begin test: %s\n", test_name);
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

void test_fixture_content_pattern_dona_counts() {
    char* pattern = "dona";
    int exp_count = 4;
    before_each(__func__, &pattern, &pattern);
    add_content_to_list(contents, 4);
    test_status += assertEqualsInt(L->pattern_count, exp_count);
    after_each(__func__);
}

void test_fixture_content_contains_lines() {
    char* pattern = "dona";
    before_each(__func__, &pattern, &pattern);
    add_content_to_list(contents, 4);
    for (int i = 0; i < 4; i++) {
        test_status += assertEqualsInt(List_Contains(L, contents[i]), 1);
    }
    after_each(__func__);
}

void test_fixture_content_next_pattern() {
    char* pattern = "dona";
    before_each(__func__, &pattern, &pattern);
    add_content_to_list(contents, 4);
    node_t* current = L->pattern_head;
    test_status += assertEqualsStr(current->content, contents[0]);
    current = current->next_frequent_search;
    test_status += assertEqualsStr(current->content, contents[1]);
    current = current->next_frequent_search;
    test_status += assertEqualsStr(current->content, contents[2]);
    current = current->next_frequent_search;
    assertEqualsInt(current == NULL, 1);
    after_each(__func__);
}

void test_fixture_content_pattern_does_not_exist() {
    char* pattern = "doesnotexist";
    before_each(__func__, &pattern, &pattern);
    add_content_to_list(contents, 4);
    assertEqualsInt(L->pattern_head == NULL, 1);
    assertEqualsInt(L->pattern_count, 0);
    after_each(__func__);
}

int main() {
    for (int id = 0; id < FIXTURESIZE; id++) {
        test_content_from_book_written_to_nodes(id);
    }
    test_fixture_content_pattern_dona_counts();
    test_fixture_content_contains_lines();
    test_fixture_content_next_pattern();
    test_fixture_content_pattern_does_not_exist();
    printf("Pass: %d, Fail: %d\n", total - incorrect, incorrect);
}