// Implements methods and structures in map.h
// Author: Hoang Son Le a1691819
// Date: 8/10/2023

#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SEED 0x12345678

void Map_Init(map_t* M, char** pattern) {
    M->pattern = pattern;
    pthread_mutex_init(&M->lock, NULL);
    M->size = 0;
}

linked_list_t* Map_Get(map_t* M, char* title) {
    if (Map_Contains(M, title))
        return M->lists[hash(title)];
    return NULL;
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

int Map_Insert(map_t* M, char* title, char* content) {
    if (Map_Contains(M, title) == false) {
        if (M->size < CAPACITY) {
            // Add new key to list
            pthread_mutex_lock(&M->lock);

            // Allocate new list:
            M->lists[hash(title)] =
                (linked_list_t*)malloc(sizeof(linked_list_t));
            if (M->lists[hash(title)] == NULL) {
                pthread_mutex_unlock(&M->lock);
                return -1;
            }
            // Add title to keys
            M->keys[M->size] =
                (char*)malloc(sizeof(char) * (strlen(title) + 1));
            if (M->keys[M->size] == NULL) {
                pthread_mutex_unlock(&M->lock);
                free(M->lists[hash(title)]);
                return -1;
            }
            strcpy(M->keys[M->size], title);
            // Initialise list
            List_Init(M->lists[hash(title)], M->pattern, &M->keys[M->size]);
            M->size++;
            pthread_mutex_unlock(&M->lock);
        }
    }
    int status = List_Insert(Map_Get(M, title), content);
    return status;
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

void Map_Free(map_t* M) {
    for (int i = 0; i < M->size; i++) {
        if (Map_Get(M, M->keys[i]) != NULL) {
            List_Free(Map_Get(M, M->keys[i]));
            free(Map_Get(M, M->keys[i]));
        }
        if (M->keys[i] != NULL) {
            free(M->keys[i]);
        }
    }
    M->size = 0;
}
