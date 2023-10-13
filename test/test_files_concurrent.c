#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/list.h"
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

list_t* L;

int total = 0;

int incorrect = 0;

int test_status = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int global_id = 0;

char* contents[FIXTURESIZE][BOOKSIZE];
int sizes[FIXTURESIZE] = {0};
char* titles[FIXTURESIZE] = {
    "first_book.txt", "second_book.txt", "third_book.txt",   "fourth_book.txt",
    "fifth_book.txt", "sixth_book.txt",  "seventh_book.txt", "eighth_book.txt",
    "ninth_book.txt", "tenth_book.txt"};

typedef struct ___thread_arg_t {
    char* title;
    char** content;
    int size;
} thread_arg_t;

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
        content[count] = (char*)malloc(read + 1);
        if (content[count] == NULL) {
            perror("malloc");
        }
        memmove(content[count], line, read + 1);
        count++;
    }
    fclose(fp);
    if (line)
        free(line);
    *size = count;
}

void* thr_insert(void* arg) {
    thread_arg_t* args = (thread_arg_t*)arg;
    for (int i = 0; i < args->size; i++) {
        Concurrent_List_Insert(L, args->title, args->content[i]);
    }
    return NULL;
}

void* thr_insert_write(void* arg) {
    thread_arg_t* args = (thread_arg_t*)arg;
    for (int i = 0; i < args->size; i++) {
        Concurrent_List_Insert(L, args->title, args->content[i]);
    }
    int myid;
    pthread_mutex_lock(&lock);
    myid = global_id;
    global_id++;
    pthread_mutex_unlock(&lock);
    Concurrent_List_Write_Book(L, args->title, myid);
    return NULL;
}

void free_book_content(char* content[], int size) {
    for (int i = 0; i < size; i++)
        free(content[i]);
}

void after_each(const char* test_name) {
    if (test_status != 0) {
        printf("Fail test: %s\n", test_name);
        incorrect += 1;
    }
    Concurrent_List_Free(L);
    free(L);
    for (int i = 0; i < FIXTURESIZE; i++) {
        free_book_content(contents[i], sizes[i]);
        char bookname[30];
        sprintf(bookname, "book_0%d.txt", i);
        remove(bookname);
    }
    global_id = 0;
}

void before_each(const char* test_name, char* pattern, int serial, int write) {
    global_id = 0;
    // printf("Begin test: %s\n", test_name);
    test_status = 0;
    total += 1;
    thread_arg_t args[FIXTURESIZE];
    for (int i = 0; i < FIXTURESIZE; i++) {
        read_file(i, contents[i], &sizes[i]);
        args[i] = (thread_arg_t){titles[i], contents[i], sizes[i]};
    }
    L = (list_t*)malloc(sizeof(list_t));
    Concurrent_List_Init(L, pattern);
    pthread_t thr[FIXTURESIZE];
    if (serial) {
        for (int i = 0; i < FIXTURESIZE; i++) {
            if (write)
                pthread_create(&thr[i], NULL, thr_insert_write, &args[i]);
            else
                pthread_create(&thr[i], NULL, thr_insert, &args[i]);
            pthread_join(thr[i], NULL);
        }
    } else {
        for (int i = 0; i < FIXTURESIZE; i++) {
            if (write)
                pthread_create(&thr[i], NULL, thr_insert_write, &args[i]);
            else
                pthread_create(&thr[i], NULL, thr_insert, &args[i]);
        }
        for (int i = 0; i < FIXTURESIZE; i++) {
            pthread_join(thr[i], NULL);
        }
    }
}

void test_all_nodes_created(int serial, int write) {
    before_each(__func__, "S", serial, write);
    for (int i = 0; i < FIXTURESIZE; i++) {
        linked_list_t* list = Map_Get(L->map, titles[i]);
        node_t* current = list->head;
        for (int j = 0; j < sizes[i]; j++) {
            if (assertEqualsStr(current->content, contents[i][j]) != 0) {
                test_status += 1;
                printf("%s %s\n", current->content, contents[i][j]);
            }
            current = current->book_next;
        }
    }
    after_each(__func__);
}

void compare_results() {
    int pid = fork();
    if (!pid) {
        char* arg[] = {"python3", "compare.py", NULL};
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

void test_files_written() {
    before_each(__func__, "S", 0, 0);
    for (int i = 0; i < FIXTURESIZE; i++) {
        Concurrent_List_Write_Book(L, titles[i], i);
    }
    compare_results();
    after_each(__func__);
}

void test_files_written_while_pthread() {
    before_each(__func__, "S", false, true);
    compare_results();
    after_each(__func__);
}

void test_analyse_pattern(char* pattern, char* expected_title,
                          int expected_count) {
    before_each(__func__, pattern, false, false);
    char* best_title;
    int best_count = 0;
    Map_Analyse(L->map, &best_title, &best_count);
    assertEqualsStr(best_title, expected_title);
    assertEqualsInt(best_count, expected_count);
    after_each(__func__);
}

void test_print_to_screen(char* pattern) {
    before_each(__func__, pattern, false, false);
    Concurrent_List_Analyse(L);
    after_each(__func__);
}

int main() {
    test_all_nodes_created(true, false);
    for (int i = 0; i < 5; i++)
        test_all_nodes_created(false, false);
    test_files_written();
    for (int i = 0; i < 5; i++)
        test_files_written_while_pthread();
    test_analyse_pattern("splendid", "eighth_book.txt", 11);
    test_analyse_pattern("Splendid", "fourth_book.txt", 1);
    test_analyse_pattern("splendid fellows", "eighth_book.txt", 1);
    test_print_to_screen("Saying");
    printf("Pass: %d, Fail: %d\n", total - incorrect, incorrect);
}