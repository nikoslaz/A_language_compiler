/* list.h */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct alpha_token_t {
    unsigned int line;
    unsigned int num_token;
    
    char* zoumi;
    unsigned int category;
    unsigned int name;
    unsigned int superclass;
    
    struct alpha_token_t* next; 
} alpha_token_t;

extern char* CAT[];
extern char* NAM[];
extern char* SUP[];
extern int tokenCounter;
extern alpha_token_t* root;

alpha_token_t* createTokenNode(unsigned int line, unsigned int num_token, char* zoumi, unsigned int category, unsigned int name, unsigned int superclass);
alpha_token_t* searchToken(alpha_token_t* root, unsigned int num_token);

void insertToken(alpha_token_t** root, unsigned int line, unsigned int num_token, char* zoumi, unsigned int category, unsigned int name, unsigned int superclass);
void deleteToken(alpha_token_t** root, unsigned int num_token);
void printTokens(alpha_token_t* root);
void freeTokenList(alpha_token_t** root);

#endif
/* end of list.h */