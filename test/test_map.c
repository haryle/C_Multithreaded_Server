#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../utils/map.h"
#define FIXTURESIZE 10
#define _GNU_SOURCE

typedef struct ___content_t {
    char* title;
    char* value;
} content_t;

content_t fixture[] = {
    {"COMPSCI1010", "Puzzle Based Learning"},
    {"COMPSCI1013", "Introduction to Computer Systems"},
    {"COMSCIP1102", "Object Oriented Programming"},
    {"COMPSCI2005", "Systems Programming"},
    {"COMPSCI2103", "Algorithm Design & Data Structures"},
    {"COMPSCI2203", "Problem Solving & Software Development"},
    {"COMPSCI2205UAC", "Software Engineering Workshop I"},
    {"COMPSCI2206", "Software Engineering Workshop II"},
    {"COMPSCI2207", "Web & Database Computing"},
    {"COMPSCI3007", "Artificial Intelligence"}};

typedef struct ___thread_insert_list_arg_t {
    linked_list_t* L;
    content_t* content;
} thread_insert_list_arg_t;

typedef struct ___thread_insert_map_arg_t {
    map_t* M;
    content_t* content;
} thread_insert_map_arg_t;

linked_list_t* BeforeEachList() {
    linked_list_t* LL = (linked_list_t*)malloc(sizeof(linked_list_t));
    List_Init(LL, "\n");
    return LL;
}

// Test cases for LinkedList
void AfterEachList(linked_list_t* LL) {
    List_Free(LL);
    free(LL);
}

int test_insert(int start, int end) {
    linked_list_t* LL = BeforeEachList();
    for (int i = start; i <= end; i++)
        List_Insert(LL, fixture[i].title, fixture[i].value);
    node_t* head = List_Head(LL);
    node_t* tail = List_Tail(LL);
    int result = 0;
    if (strcmp(head->title, fixture[start].title) != 0) {
        printf("Head Title:%s, %s\n", head->title, fixture[start].title);
        result = 1;
    }

    if (strcmp(head->value, fixture[start].value) != 0) {
        printf("Head Content:%s, %s\n", head->value, fixture[start].value);
        result = 1;
    }
    if (strcmp(tail->title, fixture[end].title) != 0) {
        printf("Tail Title:%s, %s\n", tail->title, fixture[end].title);
        result = 1;
    }
    if (strcmp(tail->value, fixture[end].value) != 0) {
        printf("Tail Content:%s, %s\n", tail->value, fixture[end].value);
        result = 1;
    }

    if (LL->size != (end - start + 1)) {
        printf("List size error: %d, %d", LL->size, end - start + 1);
        result = 1;
    }

    if (result == 1)
        printf("Fail: test_insert with start: %d, end: %d\n", start, end);
    AfterEachList(LL);
    return result;
}

int test_contain_empty_list_return_false() {
    linked_list_t* L = BeforeEachList();
    int success = 0;
    if (List_Contains(L, "COMPSCI2203",
                      "Problem Solving & Software Development"))
        success = 1;
    if (success == 1)
        printf("Failed: test_contain_empty_list_return_false");
    AfterEachList(L);
    return success;
}

int test_contain_non_empty_list_does_not_exist_return_false() {
    linked_list_t* L = BeforeEachList();
    List_Insert(L, fixture[0].title, fixture[0].value);
    int success = 0;
    if (List_Contains(L, "COMPSCI2203",
                      "Problem Solving & Software Development"))
        success = 1;
    if (success == 1)
        printf(
            "Failed: test_contain_non_empty_list_does_not_exist_return_false");
    AfterEachList(L);
    return success;
}

void* thread_insert_list(void* arg) {
    thread_insert_list_arg_t* fixture_ptr = (thread_insert_list_arg_t*)arg;
    List_Insert(fixture_ptr->L, fixture_ptr->content->title,
                fixture_ptr->content->value);
    return NULL;
}

int test_multithreaded_insert_list() {
    linked_list_t* L = BeforeEachList();
    pthread_t thr[FIXTURESIZE];
    thread_insert_list_arg_t args[FIXTURESIZE];
    // Prepare arguments
    for (int i = 0; i < FIXTURESIZE; i++) {
        args[i] = (thread_insert_list_arg_t){L, &fixture[i]};
    }
    // Fork
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_create(&thr[i], NULL, thread_insert_list, (void*)&args[i]);
    }
    // Join
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_join(thr[i], NULL);
    }
    // Test
    int success = 0;
    for (int i = 0; i < FIXTURESIZE; i++) {
        if (!List_Contains(L, fixture[i].title, fixture[i].value)) {
            printf("List does not contain title: %s\n", fixture[i].title);
            success = 1;
        }
    }
    if (success == 1)
        printf("Failed: test_multithreaded_insert_list\n");
    AfterEachList(L);
    return success;
}

// Test cases for map
map_t* before_each_map() {
    map_t* M = (map_t*)malloc(sizeof(map_t));
    Map_Init(M, "\n");
    return M;
}

void after_each_map(map_t* M) {
    Map_Free(M);
    free(M);
}

void* thread_insert_map(void* arg) {
    thread_insert_map_arg_t* fixture_ptr = (thread_insert_map_arg_t*)arg;
    Map_Insert(fixture_ptr->M, fixture_ptr->content->title,
               fixture_ptr->content->value);
    return NULL;
}

int test_multithreaded_insert_map() {
    map_t* M = before_each_map();
    pthread_t thr[FIXTURESIZE];
    thread_insert_map_arg_t args[FIXTURESIZE];
    // Prepare arguments
    for (int i = 0; i < FIXTURESIZE; i++) {
        args[i] = (thread_insert_map_arg_t){M, &fixture[i]};
    }

    // Fork
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_create(&thr[i], NULL, thread_insert_map, (void*)&args[i]);
    }
    // Join
    for (int i = 0; i < FIXTURESIZE; i++) {
        pthread_join(thr[i], NULL);
    }

    // Test
    int success = 0;
    for (int i = 0; i < FIXTURESIZE; i++) {
        linked_list_t* list = Map_Get(M, fixture[i].title);
        node_t* tail = List_Tail(list);
        if (list->size != 1) {
            printf("Size is not one for key: %s\n", fixture[i].title);
            success = 1;
        }
        if (strcmp(tail->title, fixture[i].title) != 0) {
            printf("Title does not match: %s, %s\n", tail->title,
                   fixture[i].title);
            success = 1;
        }
        if (strcmp(tail->value, fixture[i].value) != 0) {
            printf("Value does not match: %s, %s\n", tail->value,
                   fixture[i].value);
            success = 1;
        }
        if (tail->next != NULL) {
            printf("Next node after tail is not NULL\n");
            success = 1;
        }
        if (tail->book_next != NULL) {
            printf("Book Next node after tail is not NULL\n");
            success = 1;
        }
    }
    if (success == 1)
        printf("Failed: test_multithreaded_insert_map\n");
    after_each_map(M);
    return success;
}

// Test readline from file

int test_read_from_file() {
    map_t* M = before_each_map();
    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;
    char* title = "test_input.txt";
    char dir[100];
    sprintf(dir, "resources/%s", title);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return 1;
    while ((read = getline(&line, &len, fp)) != -1) {
        Map_Insert(M, title, line);
    }
    fclose(fp);

    // Second pass - compare line by line
    int success = 0;
    fp = fopen(dir, "r");
    if (fp == NULL)
        return 1;

    linked_list_t* list = Map_Get(M, title);

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

    // Free resources
    if (line)
        free(line);

    after_each_map(M);
    return success;
}

int test_write_to_file() {
    map_t* M = before_each_map();
    char* line = NULL;
    FILE* fp;
    size_t len = 0;
    ssize_t read;
    char* title = "test_input.txt";
    char dir[100];
    sprintf(dir, "resources/%s", title);

    // First pass - read data
    fp = fopen(dir, "r");
    if (fp == NULL)
        return 1;
    while ((read = getline(&line, &len, fp)) != -1) {
        Map_Insert(M, title, line);
    }
    fclose(fp);

    // Second pass - write to file and compare file content
    int success = 0;
    List_Write_Book(Map_Get(M, title), 0);
    // Check if file exists
    char book_name[] = "book_00.txt";
    if (access(book_name, F_OK) != 0) {
        success = 1;
        printf("File is not created\n");
    } else {
        remove("book_00.txt");
    }

    if (success != 0) {
        printf("Fail test_write_to_file\n");
    }

    after_each_map(M);
    return success;
}

int test_count_occurrence(char* string, char* pattern, int count) {
    int actual = count_occurence(string, pattern);
    if (actual != count) {
        printf("Fail test_count_occurence for input: %s, %s, %d, %d\n", string,
               pattern, count, actual);
        return 1;
    }
    return 0;
}

int main() {
    int failed_tests = 0;
    int run_tests = 0;
    for (int i = 0; i < FIXTURESIZE; i++) {
        run_tests += 1;
        failed_tests += test_insert(0, i);
    }
    // Test contains:
    run_tests += 1;
    failed_tests += test_contain_empty_list_return_false();

    run_tests += 1;
    failed_tests += test_contain_non_empty_list_does_not_exist_return_false();

    // Test multi-threaded insert list
    run_tests += 1;
    failed_tests += test_multithreaded_insert_list();

    // Test multi-threaded insert map
    run_tests += 1;
    failed_tests += test_multithreaded_insert_map();

    // Test read from file:
    run_tests += 1;
    failed_tests += test_read_from_file();

    // Test write to file:
    run_tests += 1;
    failed_tests += test_write_to_file();

    // Test count occurences:
    run_tests += 1;
    failed_tests +=
        test_count_occurrence("hello 1 hello 2 hello 3", "hello", 3);

    run_tests += 1;
    failed_tests += test_count_occurrence(
        "internal, external, international, rational, annal", "al", 5);

    run_tests += 1;
    failed_tests += test_count_occurrence(
        "internal, external, international, rational, annal", "er", 3);

    run_tests += 1;
    failed_tests += test_count_occurrence(
        "algebra, alabama, alquaeda, allegedly", "al", 4);
    
    run_tests += 1;
    failed_tests += test_count_occurrence(
        "algebra, alabama, alquaeda, allegedly", "a", 10);


    printf("Passed tests: %d, Failed tests: %d\n", run_tests - failed_tests,
           failed_tests);
}