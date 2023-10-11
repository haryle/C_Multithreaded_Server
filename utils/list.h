#include <pthread.h>
#include "map.h"

/*
Thread-safe list data structure 

Arguments: 
    node_t* head;               // Head
    node_t* tail;               // Tail
    map_t* map;                 // Map
    pthread_mutex_t tail_lock;  // Lock
    int size;                   // Size
*/
typedef struct __list_t {
    node_t* head;               // Head
    node_t* tail;               // Tail
    map_t* map;                 // Map
    pthread_mutex_t tail_lock;  // Lock
    int size;                   // Size
    char* pattern;              // Pattern
} list_t;

/*
Initialise the list data structure

Arguments:
L - pointer to the list data structure to init content
*/
void Concurrent_List_Init(list_t* L, char* pattern);

/*
Insert a new node to the list data structure 

Arguments: 
L - ptr to the list data structure 
title - string title of the book 
content - string content of the book 
*/
int Concurrent_List_Insert(list_t* L, char* title, char* content);

/*
Destroy the given list
*/
void Concurrent_List_Free(list_t* L);

/*
Get book based on title 
*/
linked_list_t* Concurrent_List_Get_Book(list_t* L, char* title);

/*
Traverse the list via next. Very inefficient and is only used for testing 
For practical use, try Concurrent_List_Contains method instead. 

Arguments:
L - pointer to the list to search
title - string book title 
content - string book content 

Returns:
true if there is a node with those contents
*/
bool Concurrent_List_Contains_Inefficient(list_t* L, char* title, char* content);

/*
Efficient List contains search

Arguments:
L - pointer to the list to search
title - string book title 
content - string book content 

Returns:
true if there is a node with those contents
*/
bool Concurrent_List_Contains(list_t* L, char* title, char* content);

void Concurrent_List_Write_Book(list_t* L, char* title, int book_id);