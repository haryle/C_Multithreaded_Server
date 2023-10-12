#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Concurrent_List_Init(list_t* L, char* pattern) {
    L->head = L->tail = NULL;
    pthread_mutex_init(&L->tail_lock, NULL);
    pthread_mutex_init(&L->write_lock, NULL);
    L->size = 0;
    L->pattern = (char*)malloc(sizeof(char) * (strlen(pattern) + 1));
    strcpy(L->pattern, pattern);
    L->map = (map_t*)malloc(sizeof(map_t));
    Map_Init(L->map, &L->pattern);
    L->best_count = 0;
    L->best_title = NULL;
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

void Concurrent_List_Calculate(list_t* L, int* done, pthread_cond_t* cond_var);

void Concurrent_List_Poll(list_t* L, int* already_printed, int* done,
                          pthread_cond_t* cond_var) {

    pthread_mutex_lock(&L->write_lock);
    while (done == 0)
        // Wait for update
        pthread_cond_wait(cond_var, &L->write_lock);
    if (*already_printed == 0) {
        //Display to screen results:
    }
    // Release lock
    pthread_mutex_unlock(&L->write_lock);
}