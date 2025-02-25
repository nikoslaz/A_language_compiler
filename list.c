/* list.c */

#include "list.h"

alpha_token_t* root = NULL;
int tokenCounter = 1;

char* CAT[] = {
    "ERR",         /* 0 */
    "KEYWORD",     /* 1 */
    "OPERATOR",    /* 2 */
    "CONST_INT",   /* 3 */
    "CONST_REAL",  /* 4 */
    "STRING",      /* 5 */
    "PUNCTUATION", /* 6 */
    "ID",          /* 7 */
    "COMMENT",     /* 8 */
};

char* NAM[] = {
    "ERR",      /* 0 */
    "IF",       /* 1 */
    "ELSE",     /* 2 */
    "WHILE",    /* 3 */
    "FOR",      /* 4 */
    "FUNCTION", /* 5 */
    "RETURN",   /* 6 */
    "BREAK",    /* 7 */
    "CONTINUE", /* 8 */
    "AND",      /* 9 */
    "OR",       /* 10 */
    "NOT",      /* 11 */
    "LOCAL",    /* 12 */
    "TRUE",     /* 13 */
    "FALSE",    /* 14 */
    "NIL",      /* 15 */

    "EQUALS",        /* 16 */
    "PLUS",          /* 17 */
    "MINUS",         /* 18 */
    "MULT",          /* 19 */
    "DIV",           /* 20 */
    "MOD",           /* 21 */
    "EQUALS_EQUALS", /* 22 */
    "NOT_EQUALS",    /* 23 */
    "PLUS_PLUS",     /* 24 */
    "MINUS_MINUS",   /* 25 */
    "GREATER",       /* 26 */
    "LESS",          /* 27 */
    "GREATER_EQUAL", /* 28 */
    "LESS_EQUAL",    /* 29 */

    "LEFT_BRACE",        /* 30 */
    "RIGHT_BRACE",       /* 31 */
    "LEFT_BRACKET",      /* 32 */
    "RIGHT_BRACKET",     /* 33 */
    "LEFT_PARENTHESIS",  /* 34 */
    "RIGHT_PARENTHESIS", /* 35 */
    "SEMICOLON",         /* 36 */
    "COMMA",             /* 37 */
    "COLON",             /* 38 */
    "COLON_COLON",       /* 39 */
    "PERIOD",            /* 40 */
    "PERIOD_PERIOD",     /* 41 */

    "LINE_COMMENT",      /* 42 */
    "PYTHON_COMMENT",    /* 43 */
    "MULTILINE_COMMENT", /* 44 */

    /* To print zoumi */
    "INTEGER", /* 45 */
    "REAL",    /* 46 */
    "STRING"   /* 47 */
    "ID",      /* 48 */
};

char* SUP[] = {
    "ERR",        /* 0 */
    "char*",      /* 1 */
    "enumerated", /* 2 */
    "integer",    /* 3 */
    "real"        /* 4 */
};

alpha_token_t* createTokenNode(unsigned int line, unsigned int num_token, char* zoumi, unsigned int category, unsigned int name, unsigned int superclass) {
    alpha_token_t* neoToken = (alpha_token_t*)malloc(sizeof(alpha_token_t));
    if(!neoToken) {
        printf("memory alloc problem!\n");
        return NULL;
    }

    neoToken->line = line;
    neoToken->num_token = num_token;
    neoToken->zoumi = strdup(zoumi);
    neoToken->category = category;
    neoToken->name = name;
    neoToken->superclass = superclass;
    neoToken->next = NULL;

    return neoToken;
}

void destroyToken(alpha_token_t* temp) {
    free(temp->zoumi);
    free(temp);
}

void insertToken(alpha_token_t** root, unsigned int line, unsigned int num_token, char* zoumi, unsigned int category, unsigned int name, unsigned int superclass) {
    alpha_token_t* neoToken = createTokenNode(line, num_token, zoumi, category, name, superclass);
    if(!neoToken) return;
    if(*root == NULL) {
        *root = neoToken;
        return;
    }
    alpha_token_t* temp = *root;
    while(temp->next) {
        temp = temp->next;
    }
    temp->next = neoToken;
}

void deleteToken(alpha_token_t** root, unsigned int num_token){
    if (*root == NULL) return;
    alpha_token_t* temp = *root;
    alpha_token_t* prev = NULL;
    //if the node is the head of the list
    if(temp && temp->num_token == num_token) {
        *root = temp->next;
        destroyToken(temp);
        return;
    }
    //search for the token
    while(temp && temp->num_token != num_token) {
        prev = temp;
        temp = temp->next;
    }
    //not found
    if(!temp) { return; }
    // remove from list
    prev->next = temp->next;
    destroyToken(temp);
}

alpha_token_t* searchToken(alpha_token_t* root, unsigned int num_token){
    alpha_token_t* temp = root;
    while(temp) {
        if(temp->num_token == num_token) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void printTokens(alpha_token_t* root){
    while(root != NULL) {
        printf("%u:  #%u  \"%s\"  %s  ", root->line, root->num_token, root->zoumi, CAT[root->category]);
        switch(root->name) {
            /* INTEGER - REAL */
            case 45:
            case 46:
                printf("%s", root->zoumi);
                break;
            /* STRING - ID */
            case 47:
            case 48:
                printf("\"%s\"", root->zoumi);
                break;
            default:
                printf("%s", NAM[root->name]);
                break;
        }
        printf("  <-%s\n", SUP[root->superclass]);
        root = root->next;
    }
}

void freeTokenList(alpha_token_t** root){
    alpha_token_t* temp;
    while(*root) {
        temp = *root;
        *root = (*root)->next;
        destroyToken(temp);
    }
}

/* end of list.c */