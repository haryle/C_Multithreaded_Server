#include <pthread.h>
#include "map.h"

/*
Thread-safe list data structure 

Arguments: 
    node_t* head;                   // Head
    node_t* tail;                   // Tail
    map_t* map;                     // Map
    pthread_mutex_t tail_lock;      // Lock
    pthread_mutex_t write_lock;     // Analysis lock
    int size;                       // Size
    char* pattern;                  // Pattern
    char** best_title;           // Best Title
    int best_count;              // Best count
*/
typedef struct __list_t {
    node_t* head;                // Head
    node_t* tail;                // Tail
    map_t* map;                  // Map
    pthread_mutex_t tail_lock;   // Lock to add to tail
    pthread_mutex_t write_lock;  // Analysis lock
    int size;                    // Size
    char* pattern;               // Pattern
    char** best_title;           // Best Title
    int best_count;              // Best count
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
bool Concurrent_List_Contains_Inefficient(list_t* L, char* title,
                                          char* content);

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

/*
Calculate best title and best count 

Acquire current snapshot of the book title with highest number of 
matched pattern. 

Will acquire a lock using cond_var

*/
void Concurrent_List_Calculate(list_t* L, int* done, pthread_cond_t* cond_var);
/*
Poll for result.

While result is not done, sleep using conditional variable. 
When result is done and the thread is woken up, acquire 
write lock and try to print to the stdout. If already_printed
is true, then do nothing and return. If already_printed is false,
print to stdout and set already_printed to true then return.

Arguments:
    L: linked list, containing pointers to best_title, best_count 
    already_printed: only one competing thread can print to screen 
    done: whether best_title and best_count have been computed 
    cond_var: conditional variables
*/
void Concurrent_List_Poll(list_t* L, int* already_printed, int* done,
                          pthread_cond_t* cond_var);