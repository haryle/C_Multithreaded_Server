#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void List_Init(linked_list_t* LL, char** pattern, char** title) {
    LL->pattern_head = NULL;
    LL->pattern_tail = NULL;
    LL->head = NULL;
    LL->tail = NULL;
    pthread_mutex_init(&LL->lock, NULL);
    LL->size = 0;
    LL->pattern_count = 0;

    LL->pattern = pattern;
    LL->title = title;
}

int List_Insert(linked_list_t* LL, char* content) {
    node_t* tmp;
    int status = Node_Init(content, LL->pattern, &tmp);
    // Halts if cannot create node
    if (status != 0)
        return status;
    // Add new node to list
    pthread_mutex_lock(&LL->lock);
    LL->size += 1;
    if (LL->head == NULL)
        LL->head = tmp;
    if (LL->tail == NULL)
        LL->tail = tmp;
    else {
        LL->tail->book_next = tmp;
        LL->tail = tmp;
    }
    // Add currrent node to list of search patterns if contains pattern
    if (tmp->pattern_count != 0) {
        LL->pattern_count += tmp->pattern_count;
        if (LL->pattern_head == NULL)
            LL->pattern_head = tmp;
        if (LL->pattern_tail == NULL)
            LL->pattern_tail = tmp;
        else {
            LL->pattern_tail->next_frequent_search = tmp;
            LL->pattern_tail = tmp;
        }
    }
    pthread_mutex_unlock(&LL->lock);
    return 0;
}

bool List_Contains(linked_list_t* LL, char* content) {
    pthread_mutex_lock(&LL->lock);
    node_t* current_node = LL->head;
    while (current_node != NULL) {
        if ((strcmp(content, current_node->content) == 0)) {
            pthread_mutex_unlock(&LL->lock);
            return true;
        }
        current_node = current_node->book_next;
    }
    pthread_mutex_unlock(&LL->lock);
    return false;
}

void List_Free(linked_list_t* LL) {
    pthread_mutex_lock(&LL->lock);
    node_t* current_node = LL->head;
    while (current_node != NULL) {
        node_t* new_node = current_node->book_next;
        Node_Free(current_node);
        free(current_node);
        current_node = new_node;
    }
    LL->head = LL->tail = NULL;
    LL->pattern_head = LL->pattern_tail = NULL;
    pthread_mutex_unlock(&LL->lock);
}

// void List_Write_Book(linked_list_t* L, int book_id) {
//     pthread_mutex_lock(&L->lock);
//     char file_name[SAVENAMEMAX];
//     if (book_id < 10)
//         sprintf(file_name, "book_0%d.txt", book_id);
//     else
//         sprintf(file_name, "book_%d.txt", book_id);
//     FILE* fp = fopen(file_name, "wb");
//     if (fp == NULL) {
//         printf("Cannot open file_name: %s\n", file_name);
//         pthread_mutex_unlock(&L->lock);
//         return;
//     }
//     node_t* current = L->head;
//     while (current != NULL) {
//         fputs(current->content, fp);
//         current = current->book_next;
//     }
//     fclose(fp);
//     pthread_mutex_unlock(&L->lock);
// }