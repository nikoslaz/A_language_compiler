/* SymbolTable.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define HASH_SIZE 211

/*ti sumvola mporoume na exoume */
typedef enum SymbolType {
    VAR,
    FUNC,
    LIBFUNC
} SymbolType;

typedef struct Symbol
{
    char* name;
    SymbolType type; //var, func, libfunc
    int scope;
    int line;

    struct Symbol* next_in_scope; // for scope list
    struct Symbol* next_in_bucket; // for collision list
} Symbol;

/*LISTA GIA TA SCOPES*/
typedef struct ScopeList
{
    Symbol* head; 
    struct ScopeList* next;
} ScopeList;

typedef struct HashTable
{
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
