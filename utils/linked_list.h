#include <pthread.h>
#include <stdbool.h>
#include "node.h"
#define SAVENAMEMAX 128  // Max size of filenames

/*
Internal data structure used in hash map 

linked_list contains nodes of the same book title, arranged in by the order or 
read. New node is added to the tail end of the list. 

Fields: 
    node_t* pattern_head;  // ptr to the head of the pattern search
    node_t* pattern_tail;  // ptr to the tail of the pattern search
    node_t* head;          // ptr to the head of the list
    node_t* tail;          // ptr to the tail of the list
    pthread_mutex_t lock;  // list lock
    int size;              // list size
    char** pattern;        // search pattern string
    char** title;          // title
    int pattern_count;     // pattern count
*/
typedef struct ___linked_list_t {
    node_t* pattern_head;  // ptr to the head of the pattern search
    node_t* pattern_tail;  // ptr to the tail of the pattern search
    node_t* head;          // ptr to the head of the list
    node_t* tail;          // ptr to the tail of the list
    pthread_mutex_t lock;  // list lock
    int size;              // list size
    char** pattern;        // search pattern string
    char** title;          // title
    int pattern_count;     // pattern count
} linked_list_t;

/*
Initialise linked list 

Set:
    pattern_head to NULL
    pattern_tail to NULL 
    head to NULL 
    tail to NULL 
    init lock 
    size to 0 
    pattern_count to 0 
    title to title 
    pattern to pattern 

Arguments:
LL - pointer ot linked list
pattern - pointer to pattern string 
title - pointer to title string 
*/
void List_Init(linked_list_t* LL, char** pattern, char** title);

/*
Insert new node to the list
- Increment size 
- Create new_node containing content 
- If head is NULL, set head to new_node (When list is empty)
- If tail is NULL, set tail to new_node (When list is empty)
- If list is not empty, set new_node to book_next of tail 
- If content has pattern:
    - Increase pattern_count 
    - If pattern_head and pattern_tail is NULL, set to new_node
    - Otherwise set new_node to next_frequent_search of pattern_tail

Arguments: 
LL - pointer to the linked list 
content - string book content 

Returns:
0 - if the operation was successful 
1 - if the operation fails 
*/
int List_Insert(linked_list_t* LL, char* content);

/*
Check if list contains content

Arguments: 
LL - pointer to the linked list 
content - string book content 

Returns: 
true - if the list contains any node with the same title 
false - otherwise 
*/
bool List_Contains(linked_list_t* LL, char* content);

/*
Write List Content to text file

Argument:
LL - pointer to the linked list with content 
book_id: name of book
*/
void List_Write_Book(linked_list_t* LL, int book_id);

/*
Free up memory of the linked list 

Iteratively free the nodes in the list 

Argument: 
LL - pointer to the linked list to free memory of 
*/
void List_Free(linked_list_t* LL);