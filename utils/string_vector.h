#include <string.h>

/*
 String vector data structure.
 Fields:
    message pointer
    current message size
    maximum size allocated
    title of vector
    line marsing indicator
*/
typedef struct ____string_vector_t {
    char* message;      // Message pointer
    size_t size;        // Current message size
    size_t max_size;    // Maximum allocated
    char* title;        // Title associated with the vector
    int line_parsed;    // Indicator for line parsing
} string_vector_t;

/*
 Initialize a string vector.
 Args: 
    V: Pointer to the string vector to initialise
    size: Initial size of the vector
*/
void Vector_Init(string_vector_t* V, int size);

/*
 Resize the string vector if needed.
 Args:
    V: Pointer to the string vector to resize.
 Returns: 
    1 on success and 0 if an error occurred.
*/
int Vector_Resize(string_vector_t* V);

/*
 Append a message to the string vector.
 Args:
    V: Pointer to the string vector to append to.
    message: Pounter to the char being appended onto the end of the vector.
 Returns:
    1 on success and 0 if an error occurred.
*/
int Vector_Append(string_vector_t* V, char* message);

/*
 Flush and retrieve the contents of the string vector.
 Args:
    V: Pointer to the string vector to flush/empty.
 Returns:
    A pointer to the current content of the vector / the flushed content
*/
char* Vector_Flush(string_vector_t* V);

/*
 * Free the memory associated with the string vector.
 Args:
    V: Pointer to the string vector to free.
*/
void Vector_Free(string_vector_t* V);
