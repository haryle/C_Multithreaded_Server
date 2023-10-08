#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/map.h"
#define FIXTURESIZE 6

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
    {"COMPSCI2203", "Problem Solving & Software Development"}};

typedef struct ___thread_insert_list_arg_t {
    linked_list_t* L;
    content_t* content;
} thread_insert_list_arg_t;

linked_list_t* BeforeEachList() {
    linked_list_t* LL = (linked_list_t*)malloc(sizeof(linked_list_t));
    List_Init(LL);
    return LL;
}

void AfterEachList(linked_list_t* LL) {
    List_Free(LL);
    free(LL);
}

int test_insert(int start, int end) {
    linked_list_t* LL = BeforeEachList();
    for (int i = start; i <= end; i++)
        List_Insert(LL, fixture[i].title, fixture[i].value);
    node_t* head = LL->head;
    node_t* tail = LL->tail;
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
    if (List_Contains(L, "COMPSCI2203"))
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
    if (List_Contains(L, "COMPSCI2203"))
        success = 1;
    if (success == 1)
        printf("Failed: test_contain_non_empty_list_does_not_exist_return_false");
    AfterEachList(L);
    return success;
}

void* thread_insert_list(void* arg) {
    thread_insert_list_arg_t* fixture_ptr = (thread_insert_list_arg_t*)arg;
    List_Insert(fixture_ptr->L, fixture_ptr->content->title,
                fixture_ptr->content->value);
    return NULL;
}

int test_multi_threaded_insert() {
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
        if (!List_Contains(L, fixture[i].title)) {
            printf("List does not contain title: %s\n", fixture[i].title);
            success = 1;
        }
    }
    if (success == 1)
        printf("Failed: test_multithreaded_insert\n");
    AfterEachList(L);
    return success;
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

    // Test multi-threaded insert
    run_tests += 1;
    failed_tests += test_multi_threaded_insert();

    printf("Failed tests: %d, Passed tests: %d\n", failed_tests,
           run_tests - failed_tests);
}