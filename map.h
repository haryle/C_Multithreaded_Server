#include <pthread.h>
#include <stdbool.h>
#include "linked_list.h"
#ifndef CAPACITY
    #define CAPACITY 500000
#endif

/*
Thread-safe hashmap data structure 

Fields:
    char** pattern;                  // Search pattern
    int size;                        // Map size
    pthread_mutex_t lock;            // Lock
    char* keys[CAPACITY];            // Inserted keys
    linked_list_t* lists[CAPACITY];  // List pointers
*/
typedef struct ___map_t {
    char** pattern;                  // Search pattern
    int size;                        // Map size
    pthread_mutex_t lock;            // Lock
    char* keys[CAPACITY];            // Inserted keys
    linked_list_t* lists[CAPACITY];  // List pointers
} map_t;

/* Initialise the map 

Set:
    pattern to pattern
    initialise lock
    size to 0 
*/
void Map_Init(map_t* M, char** pattern);

/*
Get LinkedList corresponding to title 

Return
Pointer to linked list or NULL if map does not 
contain title 
*/
linked_list_t* Map_Get(map_t* M, char* title);

/*
Create a hash of the input string 

Arguments:
str - string to create a hash out of 
*/
int hash(char* str);

/*
Check if map contains title
*/
bool Map_Contains(map_t* M, char* title);

/*
Insert new node to the map

Arguments: 
M - pointer to the map
title - string book title 
value - string book content 

Returns:
0 - if the operation was sucessful 
1 - if the operation fails 
*/
int Map_Insert(map_t* M, char* title, char* value);

/* Free map memory */
void Map_Free(map_t* M);

void Map_Analyse(map_t* M, char** best_title, int* best_count);