#include <pthread.h>
#include <stdbool.h>

#ifndef CAPACITY
    #define CAPACITY 500000
#endif

/*
Node data structure.

Fields:
    char* title - book title
    char* value - book content
    node_t* next - ptr to next node in the list 
    node_t* book_next - ptr to the next item in the same book 
*/
typedef struct ___node_t {
    char* title;
    char* value;
    int pattern_count;            // Number of pattern occurences
    struct ___node_t* next;       // Link to the next element in the list
    struct ___node_t* book_next;  // Link to the next item in the same book
    struct ___node_t*
        next_frequent_search;  // Link to the next item containing search pattern
} node_t;

/*
Internal data structure used in hash map 

linked_list contains nodes of the same book title, arranged in by the order or 
read. New node is added to the tail end of the list. 

Fields: 
    node_t * head - head node. Used for iterating from head 
    node_t * tail - tail node. Used for adding new item
    pthread_mutex_t lock - list lock 
*/
typedef struct ___linked_list_t {
    node_t* pattern_head;  // ptr to the head of the pattern search
    node_t* head;          // ptr to the head of the list
    node_t* tail;          // ptr to the tail of the list
    pthread_mutex_t lock;  // list lock
    int size;              // list size
    char* pattern;         // search pattern string
    int pattern_count;     // pattern count
} linked_list_t;

/* 
Get the head node of the list
*/
node_t* List_Head(linked_list_t* L);

/*
Get the tail node of the list 
*/
node_t* List_Tail(linked_list_t* L);

/*
Initialise linked list 
*/
void List_Init(linked_list_t* LL, char* pattern);

/*
Insert new node to the list

Arguments: 
LL - pointer to the linked list 
title - string book title 
value - string book content 

Returns:
0 - if the operation was sucessful 
1 - if the operation fails 
*/
int List_Insert(linked_list_t* LL, char* title, char* value);

/*
Check if list contains a title

Arguments: 
LL - pointer to the linked list 
title - string book title to search 
value - string book content 

Returns: 
true - if the list contains any node with the same title 
false - otherwise 
*/
bool List_Contains(linked_list_t* LL, char* title, char* value);

/*
Free up memory of the linked list 

Argument: 
LL - pointer to the linked list to free memory of 
*/
void List_Free(linked_list_t* LL);

/*
Write List Content to text file

Argument:
LL - pointer to the linked list with content 
book_id: name of book
*/
void List_Write_Book(linked_list_t* LL, int book_id);

/*
Create a hash of the input string 

Arguments:
str - string to create a hash out of 
*/
int hash(char* str);

/*
Thread-safe hashmap data structure 

Fields: 
lists[CAPACITY] - internal linked list 
*/
typedef struct ___map_t {
    linked_list_t lists[CAPACITY];
} map_t;

/* Initialise the map */
void Map_Init(map_t* M, char* pattern);

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

/*
Get LinkedList corresponding to title 
*/
linked_list_t* Map_Get(map_t* M, char* title);

int count_occurence(char* string, char* pattern);