/*
Node data structure.

Fields:
    char* content;                // Line Content
    int pattern_count;            // Number of pattern occurences
    struct ___node_t* next;       // Link to the next element in the list
    struct ___node_t* book_next;  // Link to the next item in the same book
    struct ___node_t*
        next_frequent_search;  // Link to the next item containing search pattern
*/
typedef struct ___node_t {
    char* content;                // Line Content
    int pattern_count;            // Number of pattern occurences
    struct ___node_t* next;       // Link to the next element in the list
    struct ___node_t* book_next;  // Link to the next item in the same book
    struct ___node_t*
        next_frequent_search;  // Link to the next item containing search pattern
} node_t;

/*
Init new node 

Set: 
    content to input content. Allocate memory to contain a copy of input content 
    pattern_count to the number of occurence of pattern in content 
    next to NULL
    book_next to NULL
    next_frequent_search to NULL

Arguments:
    content: node content 
    pattern: pointer to string pattern 
    dst: ptr to created node ptr. User should free created node by calling Node_Free

Return:
    0 if node is created successfully 
    -1 failure
*/
int Node_Init(char* content, char** pattern, node_t** dst);

/*
Free Node Resources

Free content

Args:
    N: node pointer to free 
*/
void Node_Free(node_t* N);