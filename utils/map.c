// Implements methods and structures in map.h
// Author: Hoang Son Le a1691819
// Date: 8/10/2023

#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SEED 0x12345678

int Node_Init(char* content, char** pattern, node_t** dst) {
    *dst = (node_t*)malloc(sizeof(node_t));
    if (*dst == NULL) {
        perror("malloc");
        return 1;
    }
    (*dst)->content = (char*)malloc(sizeof(char) * (strlen(content) + 1));
    if ((*dst)->content == NULL) {
        perror("malloc");
        return 1;
    }
    (*dst)->next = NULL;
    (*dst)->book_next = NULL;
    (*dst)->next_frequent_search = NULL;
    (*dst)->pattern_count = 0;
    strcpy((*dst)->content, content);
    (*dst)->pattern_count = count_occurence((*dst)->content, *pattern);
    return 0;
}

void Node_Free(node_t* N) {
    free(N->content);
}

void List_Init(linked_list_t* LL, char** pattern, char** title) {
    LL->pattern_head = NULL;
    LL->pattern_tail = NULL;
    LL->head = NULL;
    LL->tail = NULL;
    pthread_mutex_init(&LL->lock, NULL);
    LL->size = 0;
    LL->pattern = pattern;
    LL->title = title;
    LL->pattern_count = 0;
}

int List_Insert(linked_list_t* LL, char* content) {
    node_t* tmp;
    int status = Node_Init(content, LL->pattern, &tmp);
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
        Node_Destroy(current_node);
        free(current_node);
        current_node = new_node;
    }
    LL->head = LL->tail = NULL;
    pthread_mutex_unlock(&LL->lock);
}

int hash(char* str) {
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    unsigned long h = (int)SEED;
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h % CAPACITY;
}

void Map_Init(map_t* M, char* pattern) {
    pthread_mutex_init(&M->lock, NULL);
    M->size = 0;
    for (int i = 0; i < CAPACITY; i++)
        List_Init(&M->lists[i], pattern);
}

bool Map_Contains(map_t* M, char* title) {
    pthread_mutex_lock(&M->lock);
    if (M->size == 0) {
        pthread_mutex_unlock(&M->lock);
        return false;
    }
    bool contains = false;
    for (int i = 0; i < M->size; i++) {
        if (strcmp(M->keys[i], title) == 0) {
            contains = true;
            break;
        }
    }
    pthread_mutex_unlock(&M->lock);
    return contains;
}

int Map_Insert(map_t* M, char* title, char* value) {
    if (Map_Contains(M, title) == false) {
        if (M->size < CAPACITY) {
            // Add new key to list
            pthread_mutex_lock(&M->lock);
            M->keys[M->size] = title;
            M->size++;
            pthread_mutex_unlock(&M->lock);
        }
    }
    int index = hash(title);
    int status = List_Insert(&M->lists[index], title, value);
    return status;
}

void Map_Free(map_t* M) {
    for (int i = 0; i < CAPACITY; i++)
        List_Free(&M->lists[i]);
}

linked_list_t* Map_Get(map_t* M, char* title) {
    return &M->lists[hash(title)];
}

node_t* List_Head(linked_list_t* L) {
    pthread_mutex_lock(&L->lock);
    node_t* head = L->head;
    pthread_mutex_unlock(&L->lock);
    return head;
}

node_t* List_Tail(linked_list_t* L) {
    pthread_mutex_lock(&L->lock);
    node_t* tail = L->tail;
    pthread_mutex_unlock(&L->lock);
    return tail;
}

void List_Write_Book(linked_list_t* L, int book_id) {
    pthread_mutex_lock(&L->lock);
    char file_name[128];
    if (book_id < 10)
        sprintf(file_name, "book_0%d.txt", book_id);
    else
        sprintf(file_name, "book_%d.txt", book_id);
    FILE* fp = fopen(file_name, "wb");
    if (fp == NULL) {
        printf("Cannot open file_name: %s\n", file_name);
        pthread_mutex_unlock(&L->lock);
        return;
    }
    node_t* current = L->head;
    while (current != NULL) {
        fputs(current->value, fp);
        current = current->book_next;
    }
    fclose(fp);
    pthread_mutex_unlock(&L->lock);
}

int count_occurence(char* string, char* pattern) {
    int result = 0;
    char* tmp = (char*)malloc(sizeof(char) * (strlen(string) + 1));
    if (tmp == NULL) {
        perror("malloc");
        return -1;
    }
    char* substr = strstr(string, pattern);
    while (substr != NULL) {
        result++;
        if (strlen(substr) == strlen(pattern))
            break;
        char* src = &substr[strlen(pattern) + 1];
        int size = strlen(substr + strlen(pattern) + 1);
        memmove(tmp, src, size + 1);

        substr = strstr(tmp, pattern);
    }
    free(tmp);
    return result;
}