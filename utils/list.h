#include <pthread.h>
#include "map.h"

/*
Thread-safe list data structure 

Arguments: 
head: ptr to the head of the list 
tail: ptr to the tail of the list 
map: ptr to the map data 
tail_lock: lock
size: list size
*/
typedef struct __list_t {
    node_t* head;
    node_t* tail;
    map_t* map;
    pthread_mutex_t tail_lock;
    int size;
} list_t;

/*
Initialise the list data structure

Arguments:
L - pointer to the list data structure to init value
*/
void Concurrent_List_Init(list_t* L, char* pattern);

/*
Insert a new node to the list data structure 

Arguments: 
L - ptr to the list data structure 
title - string title of the book 
value - string content of the book 
*/
int Concurrent_List_Insert(list_t* L, char* title, char* value);

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
value - string book content 

Returns:
true if there is a node with those values
*/
bool Concurrent_List_Contains_Inefficient(list_t* L, char* title, char* value);

/*
Efficient List contains search

Arguments:
L - pointer to the list to search
title - string book title 
value - string book content 

Returns:
true if there is a node with those values
*/
bool Concurrent_List_Contains(list_t* L, char* title, char* value);

void Concurrent_List_Write_Book(list_t* L, char* title, int book_id);