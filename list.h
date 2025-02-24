#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//enum CAT {ID, STRING, COMMENT, CONST_INT, OPERATOR, KEYWORD, PUNCTUATION};

typedef struct alpha_token_t {
    unsigned int line;
    unsigned int num_token;

    char* zoumi;
    unsigned int category;
    
    struct alpha_token_t* next; 
}alpha_token_t;




alpha_token_t* createTokenNode(unsigned int line, unsigned int num_token, char* zoumi, unsigned int category);
void insertToken(alpha_token_t** root, unsigned int line, unsigned int num_token, char* zoumi, unsigned int category);
void deleteToken(alpha_token_t** root, unsigned int num_token);
alpha_token_t* searchToken(alpha_token_t* root, unsigned int num_token);
void printTokens(alpha_token_t* root);
void freeTokenList(alpha_token_t** root);

#endif 