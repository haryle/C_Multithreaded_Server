#include "node.h"
#include <stdio.h>
#include <stdlib.h>

int Node_Init(char* content, char** pattern, node_t** dst) {
    *dst = (node_t*)malloc(sizeof(node_t));
    if (*dst == NULL) {
        perror("malloc");
        return 1;
    }
    (*dst)->content = (char*)malloc(sizeof(char) * (strlen(content) + 1));
    if ((*dst)->content == NULL) {
        perror("malloc");
        return 1;
    }
    (*dst)->next = NULL;
    (*dst)->book_next = NULL;
    (*dst)->next_frequent_search = NULL;
    (*dst)->pattern_count = 0;
    strcpy((*dst)->content, content);
    (*dst)->pattern_count = count_occurence((*dst)->content, *pattern);
    return 0;
}

void Node_Free(node_t* N) {
    free(N->content);
}
