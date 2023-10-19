#include <stdio.h>
#include <string.h>
#include "node.h"

node_t *first, *second;
char* first_line = "This is a line\n";
char* second_line = "This is also a line\n";
int total = 0;
int incorrect = 0;
int test_status = 0;

void before_each(const char* test_name, char** pattern) {
    test_status = 0;
    total += 1;
    // printf("Begin test: %s\n", test_name);
    Node_Init(first_line, pattern, &first);
    Node_Init(second_line, pattern, &second);
}

void after_each(const char* test_name) {
    if (test_status != 0) {
        printf("Fail test: %s\n", test_name);
        incorrect += 1;
    }
    Node_Free(first);
    Node_Free(second);
}

int assertEqualsStr(char* first, char* second) {
    return strcmp(first, second) == 0 ? 0 : 1;
}

int assertEqualsInt(int first, int second) {
    return first == second ? 0 : 1;
}

void test_content_created_correctly() {
    char* pattern = "s";
    before_each(__func__, &pattern);
    test_status += assertEqualsStr(first->content, first_line);
    test_status += assertEqualsStr(second->content, second_line);
    after_each(__func__);
}

void test_pattern_s_counted_correctly() {
    char* pattern = "s";
    before_each(__func__, &pattern);
    assertEqualsInt(first->pattern_count, 1);
    assertEqualsInt(second->pattern_count, 3);
    after_each(__func__);
}

void test_pattern_new_line_counted_correctly() {
    char* pattern = "\n";
    before_each(__func__, &pattern);
    assertEqualsInt(first->pattern_count, 1);
    assertEqualsInt(second->pattern_count, 1);
    after_each(__func__);
}

void test_pattern_T_counted_correctly() {
    char* pattern = "T";
    before_each(__func__, &pattern);
    assertEqualsInt(first->pattern_count, 1);
    assertEqualsInt(second->pattern_count, 1);
    after_each(__func__);
}

int main(void) {
    test_content_created_correctly();
    test_pattern_s_counted_correctly();
    test_pattern_new_line_counted_correctly();
    test_pattern_T_counted_correctly();

    printf("Pass: %d, Fail: %d\n", total - incorrect, incorrect);
}