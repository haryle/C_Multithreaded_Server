#include <string.h>

typedef struct ____string_vector_t {
    char* message;    // Message pointer
    size_t size;      // Current message size
    size_t max_size;  // Maximum allocated
} string_vector_t;

void Vector_Init(string_vector_t* V, int size);

int Vector_Resize(string_vector_t* V);

int Vector_Append(string_vector_t* V, char* message);

char* Vector_Flush(string_vector_t* V);

void Vector_Free(string_vector_t* V);