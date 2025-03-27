/* table.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define HASH_SIZE 211

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;

#define MAX_NESTED_COMMENTS 100
extern int comment_startlines[MAX_NESTED_COMMENTS];
extern int comment_top;   
extern int comment_depth;
typedef struct alpha_token_t alpha_token_t;
extern alpha_token_t* root;
void freeTokenList(alpha_token_t** root);

/*ti sumvola mporoume na exoume */
typedef enum SymbolType {
    GLOBAL,
    LOCAL_T,
    FORMAL,
    USERFUNC,
    LIBFUNC
} SymbolType;

typedef struct Symbol {
    char* name;
    SymbolType type; // GLOBAL LOCAL FORMAL USERFUNC LIBFUNC
    int scope;
    int line;

    struct Symbol* next_in_scope; // for scope list
    struct Symbol* next_in_bucket; // for collision list
} Symbol;

/*LISTA GIA TA SCOPES*/
typedef struct ScopeList {
    Symbol* head; 
    struct ScopeList* next;
} ScopeList;

typedef struct HashTable {
    Symbol* buckets[HASH_SIZE]; //array of buckets
    ScopeList* scopes; //linked list of scope lists
    int currentScope;
} HashTable;

/*FUNCTIONS FOR SYMBOLTABLE*/
HashTable* create_HashTable();
unsigned int hash(const char* str);
Symbol* insert_Symbol(HashTable* ht, const char* name, SymbolType type, int scope, int line);
Symbol* lookUp_Symbol(HashTable* ht, const char* name, int scope);
void enter_Scope(HashTable* ht);
void exit_Scope(HashTable* ht);
void print_SymTable(HashTable* ht);
void free_HashTable(HashTable* ht);

#endif
/* end of table.h */
