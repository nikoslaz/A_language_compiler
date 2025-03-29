/**
 * @authors nikos , nikoletta , mihalis
 */
#include "table.h"

/* Globals */
HashTable* ht;
int AnonymousCounter = 0;
int maxScope = -1;
int fromFunct = 0;
Symbol* currFunction;

/* Helper Function */
void MemoryFail(void) {
    fprintf(stderr, "Fatal Error. Memory Allocation failed\n");
    exit(1);
}

int yyerror(char* yaccProvidedMessage) {
    fprintf(stderr, "\nError in Line %d: %s\nINPUT NOT VALID\n",
    yylineno, yaccProvidedMessage);
    exit(1);
}

/*===============================================================================================*/
/* Scope Handling */

void pushNextScope(int which_scope) {
    scope_snake* new = (scope_snake*)malloc(sizeof(scope_snake));
    if(!new) { MemoryFail(); }
    new->scope = which_scope;
    new->next = ht->ScopeSnakeHead;
    ht->ScopeSnakeHead = new;
    return;
}

void enter_Next_Scope(int fromFunct) {
    if(maxScope!=-1) {
        pushNextScope(ht->currentScope);
        printf("Pushed scope %d into stack\n", ht->currentScope);
    }
    ScopeList* new_scope = (ScopeList*)malloc(sizeof(ScopeList));
    if(!new_scope) { MemoryFail(); }
    new_scope->head = NULL;
    new_scope->scope = ++maxScope;
    new_scope->next = ht->ScopesHead;
    ht->ScopesHead = new_scope;
    ht->currentScope = maxScope;
    new_scope->isFunc = fromFunct;
    printf("Entered scope %d in line %d\n", maxScope, yylineno);
    return;
}

int popNextScope(void) {
    if(!ht->ScopeSnakeHead) { assert(0); }
    int res = ht->ScopeSnakeHead->scope;
    scope_snake* tmp = ht->ScopeSnakeHead;
    ht->ScopeSnakeHead = ht->ScopeSnakeHead->next;
    free(tmp);
    return res;
}

void exit_Current_Scope(void) {
    printf("Exiting current scope %d at line %d\n", ht->currentScope, yylineno);
    /* Find current ScopeList */
    ScopeList* scope_list = ht->ScopesHead;
    while(scope_list && scope_list->scope != ht->currentScope) {
        scope_list = scope_list->next;
    }
    if(!scope_list) { assert(0); }
    
    /* Set isActve to 0 */
    Symbol* curr = scope_list->head;
    while(curr) {
        curr->isActive = 0;
        curr = curr->next_in_scope;
    }
    
    /* POP NEW SCOPE FROM SCOPESNAKE */
    ht->currentScope=popNextScope();
    printf("Popped Scope is now %d\n", ht->currentScope);
    return;
}

/*===============================================================================================*/
/* Insertion */

Symbol* insert_mikeSymbol(char* name, SymbolType type) {
    /* Create new Node */
    Symbol* new = (Symbol*)malloc(sizeof(Symbol));
    if(!new) { MemoryFail(); }
    new->isActive = 1;
    new->line = yylineno;
    new->name = name;
    new->type = type;
    new->scope = ht->currentScope;
    new->args = NULL;
    new->next_in_scope = NULL;
    if(type==USERFUNC_T) { currFunction = new; }
    /* Link with HashTable */
    int index = hash(name);
    new->next_in_bucket = ht->buckets[index];
    ht->buckets[index] = new;
    /* Link with ScopeList */
    ScopeList* scope_list = ht->ScopesHead;
    while(scope_list && (scope_list->scope != ht->currentScope)) {
        scope_list = scope_list->next;
    }
    if(!scope_list) { assert(0); }
    /* If im First */
    if(!scope_list->head) { scope_list->head = new; }
    /* Insert at the end */
    else {
        Symbol* prev = scope_list->head;
        while(prev->next_in_scope) { prev = prev->next_in_scope; }
        prev->next_in_scope = new;
    }
    /* Return */
    return new;
}

/*===============================================================================================*/
/* Hashtable */

unsigned int hash(const char* str){
    unsigned int hash = 0;
    while(*str) {
        hash = (hash * 31 + *str) % HASH_SIZE;
        str++;
    }
    return hash;
}

void Initialize_HashTable(void){
    ht = (HashTable*)malloc(sizeof(HashTable));
    if(!ht) { MemoryFail(); }
    /* Initialize all buckets to NULL */
    for(int i=0; i < HASH_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
    /* Create Scope 0 */
    ht->ScopesHead = NULL;
    ht->ScopeSnakeHead = NULL;
    enter_Next_Scope(0);
    /* Insert all Library Functions */    
    insert_mikeSymbol("print", LIBFUNC_T);
    insert_mikeSymbol("input", LIBFUNC_T);
    insert_mikeSymbol("objectmemberkeys", LIBFUNC_T);
    insert_mikeSymbol("objecttotalmembers", LIBFUNC_T);
    insert_mikeSymbol("objectcopy", LIBFUNC_T);
    insert_mikeSymbol("totalarguments", LIBFUNC_T);
    insert_mikeSymbol("argument", LIBFUNC_T);
    insert_mikeSymbol("typeof", LIBFUNC_T);
    insert_mikeSymbol("strtonum", LIBFUNC_T);
    insert_mikeSymbol("sqrt", LIBFUNC_T);
    insert_mikeSymbol("cos", LIBFUNC_T);
    insert_mikeSymbol("sin", LIBFUNC_T);
    return;
}

/*===============================================================================================*/

/*NIKOLETTA*/
Symbol* lookUp_GlobalSymbol(const char* name){
    //search only global scope if found return it else  ERROR
}

Symbol* insert_GlobalSymbol(const char* name){
    //save as global 
}

/*===============================================================================================*/

/*LET IT BE!!!!*/

Symbol* resolve_RawSymbol(const char* name){
    // VARIABLES:search all the previous scopes including current
    // until you find variable with the same name that can reach current scope
    // without bool isFunc getting in the way
    // PIIPAPAAAAA
    // FUNCTIONS: all the previous scopes
    // globals always visible
}

/*===============================================================================================*/
/* Functions */

int is_Lib_Func(const char* name) {
    Symbol* curr = ht->buckets[hash(name)];
    while(curr) {
        if(curr->type==LIBFUNC_T && strcmp(name, curr->name)==0) {
            return 1;
        }
        curr = curr->next_in_bucket;
    }
    return 0;
}

int lookUp_CurrentScope(const char* name) {
    /* Search my Scope for any Symbol with the same name */
    Symbol* curr = ht->buckets[hash(name)];
    while(curr) {
        if(curr->scope==ht->currentScope && strcmp(name, curr->name)==0) {
            return 1;
        }
        curr = curr->next_in_bucket;
    }
    return 0;
}

/*===============================================================================================*/
/* Resolves */

Symbol* resolve_FuncSymbol(char* name) {
    printf("Resolving %s in line %d\n", name, yylineno);
    if(is_Lib_Func(name)) { yyerror("Trying to redefine Library Function"); }
    else if(lookUp_CurrentScope(name)) { yyerror("Symbol already defined"); }
    else { return insert_mikeSymbol(name, USERFUNC_T); }
    assert(0);
}

Symbol* resolve_AnonymousFunc(void) {
    printf("Resolving anonymous in line %d\n", yylineno);
    if(AnonymousCounter>MAX_ANONYMOUS_FUNCTIONS) {
        printf("Error. Maximum number of Anonymous Functions reached. Sorry :(\n");
        exit(1);
    }
    char* anon_name = (char*)malloc(10*sizeof(char));
    if(!anon_name) { MemoryFail(); }
    sprintf(anon_name, "$func%d", AnonymousCounter++);
    return insert_mikeSymbol(anon_name, USERFUNC_T);
}

Symbol* resolve_FormalSymbol(char* name) {
    printf("Resolving %s in line %d\n", name, yylineno);
    if(is_Lib_Func(name)) { yyerror("Trying to redefine Library Function"); }
    else if(lookUp_CurrentScope(name)) { yyerror("Formal Argument Redefinition"); }
    else {
        /* Insert Symbol */
        Symbol* new = insert_mikeSymbol(name, FORMAL_T);
        /* Create Argument node */
        argument_node* newarg = (argument_node*)malloc(sizeof(argument_node));
        if(!newarg) { MemoryFail(); }
        newarg->symbol = new;
        newarg->next = NULL;
        /* Link at the End */
        if(!currFunction->args) { currFunction->args = newarg; }
        else {
            argument_node* currnode = currFunction->args;
            while(currnode->next) { currnode = currnode->next; }
            currnode->next = newarg;
        }
        /* Return */
        return new;
    }
    assert(0);
}

Symbol* resolve_LocalSymbol(char* name) {
    if(is_Lib_Func(name)) { yyerror("Trying to redefine Library Function"); }
    else if(lookUp_CurrentScope(name)) { return NULL; }
    else {
        if(ht->currentScope==0) {
            return insert_mikeSymbol(name, GLOBAL_T);
        } else {
            return insert_mikeSymbol(name, LOCAL_T);
        }
    }
    assert(0);
}

/*===============================================================================================*/
/* Final Steps */

void free_HashTable(void) {
    /* TODO */
}

const char* symbolTypeToString(SymbolType type) {
    switch(type) {
        case GLOBAL_T: return "global variable";
        case LOCAL_T: return "local variable";
        case FORMAL_T: return "formal argument";
        case USERFUNC_T: return "user function";
        case LIBFUNC_T: return "library function";
        default: return "unknown";
    }
    assert(0);
}

/* Recursive Print */
void printScopes(const ScopeList* scopelist) {
    /* Base Case */
    if(!scopelist) { return; }
    /* Print in reverse order Recursively */
    printScopes(scopelist->next);
    /* My job */
    printf("---------- Scope %d ----------\n", scopelist->scope);
    Symbol* symbol = scopelist->head;
    while(symbol) {
        /* Do not print Library Functions :D */
        if(symbol->type!=LIBFUNC_T) {
            printf("\"%s\" [%s] (line %d) (scope %d)\n",
            symbol->name, symbolTypeToString(symbol->type),
            symbol->line, symbol->scope);
        }
        symbol = symbol->next_in_scope;
    }
    return;
}

/* API */
void print_SymTable(void) {
    const ScopeList* scopeNode = ht->ScopesHead;
    printScopes(scopeNode);
    return;
}

/* end of table.c */
