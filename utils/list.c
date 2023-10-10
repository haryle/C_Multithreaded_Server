#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Concurrent_List_Init(list_t* L, char* pattern) {
    L->map = (map_t*)malloc(sizeof(map_t));
    Map_Init(L->map, pattern);
    L->head = L->tail = NULL;
    L->size = 0;
    pthread_mutex_init(&L->tail_lock, NULL);
}

int Concurrent_List_Insert(list_t* L, char* title, char* value) {
    int status = Map_Insert(L->map, title, value);
    if (status == 1)
        return status;
    linked_list_t* list = Map_Get(L->map, title);
    node_t* tail = List_Tail(list);
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
}

linked_list_t* Concurrent_List_Get_Book(list_t* L, char* title) {
    return Map_Get(L->map, title);
}

bool Concurrent_List_Contains_Inefficient(list_t* L, char* title, char* value) {
    pthread_mutex_lock(&L->tail_lock);
    node_t* current = L->head;
    while (current != NULL) {
        if ((strcmp(current->title, title) == 0) &&
            (strcmp(current->value, value) == 0)) {
            pthread_mutex_unlock(&L->tail_lock);
            return true;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&L->tail_lock);
    return false;
}

bool Concurrent_List_Contains(list_t* L, char* title, char* value) {
    linked_list_t* list = Map_Get(L->map, title);
    return List_Contains(list, title, value);
}

void Concurrent_List_Write_Book(list_t* L, char* title, int book_id) {
    linked_list_t* list = Concurrent_List_Get_Book(L, title);
    List_Write_Book(list, book_id);
}
