#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

HashTable* create_HashTable(){
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if(!ht){
        printf("memory allocation failed for hashtable\n");
        exit(1);
    }
    /*initialize buckets to null*/
    for(int i=0; i < HASH_SIZE; i++){
        ht->buckets[i] = NULL;
    }

    ht->scopes = NULL;
    ht->currentScope = 0; //global scope

    /*create first scope GLOBAL*/
    enter_Scope(ht);

    return ht;
}

unsigned int hash(const char* str){
    unsigned int hash = 0;
    while(*str){
        hash = (hash * 31 + *str) % HASH_SIZE;
        str++;
    }

    return hash;
}

Symbol* insert_Symbol(HashTable* ht, const char* name, SymbolType type, int scope, int line){
    /*uparxei?*/
    Symbol* exists = lookUp_Symbol(ht, name, scope);
    if(exists){
        printf("Symbol %s already exists in defined scope %d at line %d\n", name, scope, line);
        return NULL;
    }
    
    /*since it does not exist alredy make a new one*/
    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if(!newSymbol){
        printf("memory alloc failed for symbol\n");
        exit(1);
    }
    newSymbol->name = name;
    newSymbol->type = type;
    newSymbol->scope = scope;
    newSymbol->line = line;
    newSymbol->next_in_bucket = NULL;
    newSymbol->next_in_scope = NULL;

    /*insert into bucket (collision list)*/
    unsigned int  index = hash(name);
    newSymbol->next_in_bucket = ht->buckets[index];
    ht->buckets[index] = newSymbol;

    /*insert into scope list*/
    ScopeList* scope_list = ht->scopes;
    while (scope_list && scope_list->head && scope_list->head->scope != scope) {
        scope_list = scope_list->next;
    }
    if (!scope_list) {
        printf("Scope %d not found\n", scope);
        free(newSymbol->name);
        free(newSymbol);
        return NULL;
    }
    newSymbol->next_in_scope = scope_list->head;
    scope_list->head = newSymbol;

    return newSymbol;
}

void enter_scope(HashTable* ht) {
    ScopeList* new_scope = (ScopeList*)malloc(sizeof(ScopeList));
    if (!new_scope) {
        fprintf(stderr, "Memory allocation failed for scope list\n");
        exit(1);
    }
    new_scope->head = NULL;
    new_scope->next = ht->scopes;
    ht->scopes = new_scope;
    ht->currentScope++;
}

Symbol* lookup_symbol(HashTable* ht, const char* name, int scope) {
    unsigned int index = hash(name);
    Symbol* current = ht->buckets[index];

    /*check collision list*/
    while (current) {
        if (strcmp(current->name, name) == 0) {
            //if scope is -1, we don't care about scope (global lookup)
            if (scope == -1 || current->scope == scope) {
                return current;
            }
            //check if the symbol is in a parent scope (for nested lookups)
            if (scope >= 0 && current->scope <= scope) {
                return current;
            }
        }
        current = current->next_in_bucket;
    }
    return NULL;
}

void exit_scope(HashTable* ht) {
    if (!ht->scopes) {
        fprintf(stderr, "No scope to exit\n");
        return;
    }
    ht->scopes = ht->scopes->next;
    ht->currentScope--;
}


void free_hash_table(HashTable* ht) {
    /*free all symbols*/
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol* current = ht->buckets[i];
        while (current) {
            Symbol* temp = current;
            current = current->next_in_bucket;
            free(temp->name);
            free(temp);
        }
    }

    /*free scope lists*/
    ScopeList* scope = ht->scopes;
    while (scope) {
        ScopeList* temp = scope;
        scope = scope->next;
        free(temp);
    }

    free(ht);
}
