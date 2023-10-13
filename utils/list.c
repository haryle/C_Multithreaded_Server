#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Concurrent_List_Init(list_t* L, char* pattern) {
    pthread_mutex_init(&L->tail_lock, NULL);
    pthread_mutex_init(&L->write_lock, NULL);
    L->size = 0;
    L->pattern = (char*)malloc(sizeof(char) * (strlen(pattern) + 1));
    strcpy(L->pattern, pattern);
    L->map = (map_t*)malloc(sizeof(map_t));
    Map_Init(L->map, &L->pattern);
    L->best_count = 0;
    L->best_title = NULL;
    L->head = NULL;
    L->tail = NULL;
}

int Concurrent_List_Insert(list_t* L, char* title, char* content) {
    int status = Map_Insert(L->map, title, content);
    if (status == 1)
        return status;
    // Only one thread updates list of the same title so lock not required
    linked_list_t* list = Map_Get(L->map, title);
    node_t* tail = list->tail;
    // Set tail is the newly added node
    pthread_mutex_lock(&L->tail_lock);
    L->size += 1;
    // Only happen when the list is full
    if (L->head == NULL) {
        L->head = tail;
    }
    if (L->tail == NULL) {
        L->tail = tail;
    } else {
        L->tail->next = tail;
        L->tail = tail;
    }
    pthread_mutex_unlock(&L->tail_lock);
    return 0;
}

void Concurrent_List_Free(list_t* L) {
    printf("Reach here\n");
    Map_Free(L->map);
    free(L->map);
    free(L->pattern);
}

linked_list_t* Concurrent_List_Get_Book(list_t* L, char* title) {
    return Map_Get(L->map, title);
}

bool Concurrent_List_Contains_Inefficient(list_t* L, char* title,
                                          char* content) {
    pthread_mutex_lock(&L->tail_lock);
    node_t* current = L->head;
    while (current != NULL) {
        if ((strcmp(current->content, content) == 0)) {
            pthread_mutex_unlock(&L->tail_lock);
            return true;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&L->tail_lock);
    return false;
}

bool Concurrent_List_Contains(list_t* L, char* title, char* content) {
    linked_list_t* list = Concurrent_List_Get_Book(L, title);
    if (list == NULL)
        return false;
    return List_Contains(list, content);
}

void Concurrent_List_Write_Book(list_t* L, char* title, int book_id) {
    linked_list_t* list = Concurrent_List_Get_Book(L, title);
    if (list == NULL)
        return;
    List_Write_Book(list, book_id);
}

typedef struct ___analysis_param_t {
    list_t* L;
    int* done;
    int* already_printed;
    pthread_cond_t* cond_var;
} analysis_param_t;

void Concurrent_List_Calculate(list_t* L, int* done, pthread_cond_t* cond_var) {
    pthread_mutex_lock(&L->write_lock);
    // Find best match pattern
    Map_Analyse(L->map, &L->best_title, &L->best_count);
    *done = 1;
    pthread_cond_broadcast(cond_var);
    pthread_mutex_unlock(&L->write_lock);
}

void Concurrent_List_Poll(list_t* L, int* already_printed, int* done,
                          pthread_cond_t* cond_var) {

    pthread_mutex_lock(&L->write_lock);
    while (*done == 0)
        // Wait for update
        pthread_cond_wait(cond_var, &L->write_lock);
    if (*already_printed == 0) {
        //Display to screen results if not yet printed
        if (L->best_title == NULL)
            printf("Title: NULL, count: 0\n");
        else
            printf("Title: %s, count: %d\n", L->best_title, L->best_count);
        *already_printed = 1;
    }
    // Release lock
    pthread_mutex_unlock(&L->write_lock);
}

void* thread_poll(void* analysis_param) {
    analysis_param_t* params = (analysis_param_t*)analysis_param;
    Concurrent_List_Poll(params->L, params->already_printed, params->done,
                         params->cond_var);
    return NULL;
}

void Concurrent_List_Analyse(list_t* L) {
    int done = 0;
    int already_printed = 0;
    pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
    analysis_param_t params =
        (analysis_param_t){L, &done, &already_printed, &cond_var};
    pthread_t poll_thr[5];
    for (int i = 0; i < 5; i++)
        pthread_create(&poll_thr[i], NULL, thread_poll, &params);
    Concurrent_List_Calculate(L, &done, &cond_var);
    for (int i = 0; i < 5; i++)
        pthread_join(poll_thr[i], NULL);
}

void* thread_analyse(void* List) {
    list_t* L = (list_t*)List;
    Concurrent_List_Analyse(L);
    return NULL;
}