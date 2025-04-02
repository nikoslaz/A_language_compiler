/* table.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define HASH_SIZE 211
#define MAX_ANONYMOUS_FUNCTIONS 9999

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Link with scanner.l */
#define YY_DECL int alpha_yylex (void* yylval)
int yyerror(char* yaccProvidedMessage);
int yylex(void);
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

/* Symbol Types */
typedef enum SymbolType {
    GLOBAL_T,
    LOCAL_T,
    FORMAL_T,
    USERFUNC_T,
    LIBFUNC_T
} SymbolType;

/* Used to store the Arguments of a Function */
struct Symbol;
typedef struct argument_t {
    struct Symbol* symbol;
    struct argument_t* next;
} argument_node;

/* symbol_t */
typedef struct Symbol {
    char* name;
    SymbolType type; // GLOBAL LOCAL FORMAL USERFUNC LIBFUNC
    int scope;
    int line;
    int isActive;
    int varArgs;
    argument_node* args;
    struct Symbol* next_in_scope; // for scope list
    struct Symbol* next_in_bucket; // for collision list
} Symbol;

/* Linked List of the same Scope */
typedef struct ScopeList {
    int scope;
    Symbol* head; 
    struct ScopeList* next;
    int isFunc;
} ScopeList;

/* Simple HashTable */
typedef struct HashTable {
    Symbol* buckets[HASH_SIZE];
    int currentScope;
    ScopeList* ScopesHead;
} HashTable;

extern HashTable* ht;
extern int AnonymousCounter;
extern Symbol* currFunction;

/* Functions */
unsigned int hash(const char* str);
void Initialize_HashTable(void);
void enter_Next_Scope(int fromFunct);
void exit_Current_Scope(void);
void print_SymTable(void);
void free_HashTable(void);

void checkFunctionSymbol(struct Symbol* sym, const char* operation);
Symbol* checkFunctionCall(Symbol* sym, const char* errorPrefix);
Symbol* handleAnonymousFuncCall(Symbol* funcdef);
Symbol* createTempSymbol(void);
Symbol* lookUp_All(const char* name, int* inaccessible);
Symbol* lookUp_CurrentScope(const char* name);

Symbol* resolve_FuncSymbol(const char* name);
Symbol* resolve_AnonymousFunc(void);
Symbol* resolve_FormalSymbol(const char* name);
Symbol* resolve_LocalSymbol(const char* name);
Symbol* resolve_GlobalSymbol(const char* name);
Symbol* resolve_RawSymbol(const char* name);


#endif
/* end of table.h */