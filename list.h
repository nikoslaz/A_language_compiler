/* list.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_FILE_LINES 999999999
#define MAX_NESTED_COMMENTS 100
#define MAX_COMMENT_RANGE 24

typedef enum ALPHA_CATEGORY {
    C_ERR         = 0,
    C_KEYWORD     = 1,
    C_OPERATOR    = 2,
    C_CONST_INT   = 3,
    C_CONST_REAL  = 4,
    C_STRING      = 5,
    C_PUNCTUATION = 6,
    C_ID          = 7,
    C_COMMENT     = 8
} ALPHA_CATEGORY;

typedef enum ALPHA_NAME {
    /* Keywords */
    N_ERR      = 0,
    N_IF       = 1,
    N_ELSE     = 2,
    N_WHILE    = 3,
    N_FOR      = 4,
    N_FUNCTION = 5,
    N_RETURN   = 6,
    N_BREAK    = 7,
    N_CONTINUE = 8,
    N_AND      = 9,
    N_OR       = 10,
    N_NOT      = 11,
    N_LOCAL    = 12,
    N_TRUE     = 13,
    N_FALSE    = 14,
    N_NIL      = 15,
    /* Operators */
    N_EQUALS        = 16,
    N_PLUS          = 17,
    N_MINUS         = 18,
    N_MULT          = 19,
    N_DIV           = 20,
    N_MOD           = 21,
    N_EQUALS_EQUALS = 22,
    N_NOT_EQUALS    = 23,
    N_PLUS_PLUS     = 24,
    N_MINUS_MINUS   = 25,
    N_GREATER       = 26,
    N_LESS          = 27,
    N_GREATER_EQUAL = 28,
    N_LESS_EQUAL    = 29,
    /* Punctuation */
    N_LEFT_BRACE        = 30,
    N_RIGHT_BRACE       = 31,
    N_LEFT_BRACKET      = 32,
    N_RIGHT_BRACKET     = 33,
    N_LEFT_PARENTHESIS  = 34,
    N_RIGHT_PARENTHESIS = 35,
    N_SEMICOLON         = 36,
    N_COMMA             = 37,
    N_COLON             = 38,
    N_COLON_COLON       = 39,
    N_PERIOD            = 40,
    N_PERIOD_PERIOD     = 41,
    /* Comments */
    N_LINE_COMMENT      = 42,
    N_PYTHON_COMMENT    = 43,
    N_MULTILINE_COMMENT = 44,
    N_NESTED_COMMENT    = 45
} ALPHA_NAME;

typedef enum ALPHA_SUPERCLASS {
    S_ERR        =0,
    S_CHARSTAR   =1,
    S_ENUMERATED =2,
    S_INTEGER    =3,
    S_REAL       =4
} ALPHA_SUPERCLASS;

/* AlphaToken Struct */
typedef struct alpha_token_t {
    unsigned int line;
    unsigned int num_token;
    
    char* zoumi;
    ALPHA_CATEGORY category;
    ALPHA_NAME name;
    ALPHA_SUPERCLASS superclass;
    
    struct alpha_token_t* next; 
} alpha_token_t;

/* Global Variables */
extern alpha_token_t* root;
extern int tokenCounter;
extern int comment_depth;
extern int comment_top;   
extern int comment_startlines[MAX_NESTED_COMMENTS];  

/* Implement List */
void trim_escape(char** destination, char* source);
alpha_token_t* createTokenNode(unsigned int line, unsigned int num_token, char* zoumi, ALPHA_CATEGORY category, ALPHA_NAME name, ALPHA_SUPERCLASS superclass);
void insertToken(alpha_token_t** root, unsigned int line, unsigned int num_token, char* zoumi, ALPHA_CATEGORY category, ALPHA_NAME name, ALPHA_SUPERCLASS superclass);
void printTokens(alpha_token_t* root);
alpha_token_t* searchToken(alpha_token_t* root, unsigned int num_token);
void destroyToken(alpha_token_t** temp);
void deleteToken(alpha_token_t** root, unsigned int num_token);
void freeTokenList(alpha_token_t** root);

#endif
/* end of list.h */
