#include <stdio.h>
#include <stdlib.h>
#include "../utils/string_vector.h"

string_vector_t* before_each() {
    string_vector_t* vector = (string_vector_t*)malloc(sizeof(string_vector_t));
    Vector_Init(vector, 5);
    return vector;
}

void after_each(string_vector_t* V) {
    Vector_Free(V);
}

int test_append() {
    string_vector_t* vector = before_each();
    char* fixture =
        "This is quite a long message, definitely longer than 5 characters";
    Vector_Append(vector, fixture);
    int status = 0;
    if (strcmp(vector->message, fixture) != 0) {
        printf("Fail: string value comparison\n");
        status = 1;
    }
    if (vector->max_size != 80) {
        printf("Fail: maxsize: %ld, %d\n", vector->max_size, 80);
        status = 1;
    }
    if (vector->size != strlen(fixture)) {
        printf("Fail: size: %ld, %ld\n", vector->size, strlen(fixture));
        status = 1;
    }
    if (status == 1) {
        printf("Fail test append\n");
    }
    after_each(vector);
    return status;
}

int test_flush_then_reappend() {
    string_vector_t* vector = before_each();
    char* fixture =
        "This is quite a long message, definitely longer than 5 characters";
    Vector_Append(vector, fixture);
    int status = 0;

    char* flushed_msg = Vector_Flush(vector);
    if (strcmp(flushed_msg, fixture) != 0) {
        printf("Fail: string value comparison\n");
        status = 1;
    }
    if (vector->size != strlen(vector->message)) {
        printf("Message size after flushing: %ld, %ld\n", vector->size,
               strlen(vector->message));
        status = 1;
    }
    if (vector->max_size != 80) {
        printf("Fail: maxsize: %ld, %d\n", vector->max_size, 80);
        status = 1;
    }
    // Append New message:
    Vector_Append(vector, fixture);
    if (strcmp(vector->message, fixture) != 0) {
        printf("Fail: string value comparison\n");
        status = 1;
    }
    if (vector->max_size != 80) {
        printf("Fail: maxsize: %ld, %d\n", vector->max_size, 80);
        status = 1;
    }
    if (status == 1) {
        printf("Fail test_flush_then_reappend\n");
    }
    after_each(vector);
    return status;
}

int test_append_multiple_strings() {
    string_vector_t* vector = before_each();
    char* fixture1 = "This is the first sentence";
    char* fixture2 = "This is the second sentence";
    char fixture[1024];
    strcat(strcat(fixture, fixture1), fixture2);
    Vector_Append(vector, fixture1);
    Vector_Append(vector, fixture2);
    int status = 0;
    if (strcmp(vector->message, fixture) != 0) {
        printf("Fail: string value comparison\n");
        status = 1;
    }
    if (vector->max_size != 80) {
        printf("Fail: maxsize: %ld, %d\n", vector->max_size, 80);
        status = 1;
    }
    if (vector->size != strlen(fixture)) {
        printf("Fail: size: %ld, %ld\n", vector->size, strlen(fixture));
        status = 1;
    }
    if (status == 1) {
        printf("test_append_multiple_strings\n");
    }
    after_each(vector);
    return status;
}

int main() {
    int failed_tests = 0;
    int run_tests = 0;

    run_tests += 1;
    failed_tests += test_append();

    run_tests += 1;
    failed_tests += test_flush_then_reappend();

    run_tests += 1;
    failed_tests += test_append_multiple_strings();

    printf("Passed tests: %d, Failed tests: %d\n", run_tests - failed_tests,
           failed_tests);
}