/**
 * @authors nikos , nikoletta , mihalis
 */
#include "table.h"

/* Globals */
HashTable* ht;
int AnonymousCounter = 0; // Counter for naming anonymous functions
int maxScope = -1;  // Tracks the maximum (current deepest) scope
int fromFunct = 0;  // Flag to indicate if entering a function scope
Symbol* currFunction;   // Pointer to the current function being processed

/* Helper Function */
void MemoryFail(void) {
    fprintf(stderr, "Fatal Error. Memory Allocation failed\n");
    exit(1);
}

int yyerror(char* yaccProvidedMessage) {
    fprintf(stderr, "Error in Line %d: %s\n",
    yylineno, yaccProvidedMessage);
}

/*===============================================================================================*/
/* Scope Handling */

void pushNextScope(int which_scope) {
    scope_snake* new = (scope_snake*)malloc(sizeof(scope_snake));
    if(!new) { MemoryFail(); }
    new->scope = which_scope;
    new->next = ht->ScopeSnakeHead;
    ht->ScopeSnakeHead = new;
}

void enter_Next_Scope(int fromFunct) {
    if(maxScope!=-1) { pushNextScope(ht->currentScope); }
    ScopeList* new_scope = (ScopeList*)malloc(sizeof(ScopeList));
    if(!new_scope) { MemoryFail(); }
    new_scope->head = NULL;
    new_scope->scope = ++maxScope;
    new_scope->next = ht->ScopesHead;
    ht->ScopesHead = new_scope;
    ht->currentScope = maxScope;
    new_scope->isFunc = fromFunct;
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
}

/*===============================================================================================*/
/* Insertion */

Symbol* insert_Symbol(const char* name, SymbolType type) {
    /* Create new Node */
    Symbol* new = (Symbol*)malloc(sizeof(Symbol));
    if(!new) { MemoryFail(); }
    new->isActive = 1;
    new->line = yylineno;
    new->name = strdup(name);
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
    insert_Symbol("print", LIBFUNC_T);
    insert_Symbol("input", LIBFUNC_T);
    insert_Symbol("objectmemberkeys", LIBFUNC_T);
    insert_Symbol("objecttotalmembers", LIBFUNC_T);
    insert_Symbol("objectcopy", LIBFUNC_T);
    insert_Symbol("totalarguments", LIBFUNC_T);
    insert_Symbol("argument", LIBFUNC_T);
    insert_Symbol("typeof", LIBFUNC_T);
    insert_Symbol("strtonum", LIBFUNC_T);
    insert_Symbol("sqrt", LIBFUNC_T);
    insert_Symbol("cos", LIBFUNC_T);
    insert_Symbol("sin", LIBFUNC_T);
}

/*===============================================================================================*/
/* Functions */

Symbol* is_Lib_Func(const char* name) {
    Symbol* curr = ht->buckets[hash(name)];
    while(curr) {
        if(curr->type==LIBFUNC_T && strcmp(name, curr->name)==0) {
            return curr;
        }
        curr = curr->next_in_bucket;
    }
    return NULL;
}


Symbol* lookUp_CurrentScope(const char* name) {
    /* Search my Scope for any Symbol with the same name */
    Symbol* curr = ht->buckets[hash(name)];
    while(curr) {
        if(curr->scope==ht->currentScope && strcmp(name, curr->name)==0) {
            return curr;
        }
        curr = curr->next_in_bucket;
    }
    return NULL;
}

Symbol* lookUp_GlobalScope(const char* name){
    ScopeList* scope_list = ht->ScopesHead;
    while (scope_list && scope_list->scope != 0 ) {
        scope_list = scope_list->next;
    }
    if (!scope_list) {
        printf("Scope 0 not found?\n");
        return NULL;
    }
    //search only the symbols in this scope (0)
    Symbol* current = scope_list->head;
    while(current) {
        if(strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next_in_scope;
    }
    return NULL;
}

void createArgumentNode(Symbol* mySymbol) {
    argument_node* newarg = (argument_node*)malloc(sizeof(argument_node));
    if(!newarg) { MemoryFail(); }
    newarg->symbol = mySymbol;
    newarg->next = NULL;
    /* Link at the end of args list */
    if(!currFunction->args) { currFunction->args = newarg; }
    else {
        argument_node* currnode = currFunction->args;
        while(currnode->next) { currnode = currnode->next; }
        currnode->next = newarg;
    }
}

const char* createNewArgumentName(void) {
    char* anon_name = (char*)malloc(10*sizeof(char));
    if(!anon_name) { MemoryFail(); }
    sprintf(anon_name, "$func%d", AnonymousCounter++);
    return anon_name;
}

void checkFunctionSymbol(Symbol* sym, const char* operation) {
    if (sym && (sym->type == USERFUNC_T || sym->type == LIBFUNC_T)) {
        char msg[100];
        snprintf(msg, sizeof(msg), "Cannot %s function", operation);
        yyerror(msg);
    }
}

/*===============================================================================================*/
/* Resolves */

Symbol* resolve_FuncSymbol(const char* name) {
    Symbol* res = NULL;
    if(res = is_Lib_Func(name)) {
        yyerror("Trying to redefine Library Function");
    } else if(res = lookUp_CurrentScope(name)) {
        yyerror("Symbol already defined");
    } else { res = insert_Symbol(name, USERFUNC_T); }
    return res;
}

Symbol* resolve_AnonymousFunc(void) {
    Symbol* res = NULL;
    if(AnonymousCounter>MAX_ANONYMOUS_FUNCTIONS) {
        yyerror("Maximum number of Anonymous Functions reached.");
        return res;
    }
    /* Anonymous are always added with a unique name */
    const char* anon_name = createNewArgumentName();
    res = insert_Symbol(anon_name, USERFUNC_T);
    return res;
}

Symbol* resolve_FormalSymbol(const char* name) {
    Symbol* res = NULL;
    if(res = is_Lib_Func(name)) { yyerror("Symbol is a Library Function"); }
    else if(res = lookUp_CurrentScope(name)) { yyerror("Formal Argument Redefinition"); }
    else {
        /* Create Symbol and link with Function Arguments */
        res = insert_Symbol(name, FORMAL_T);
        createArgumentNode(res);
    }
    return res;
}

Symbol* resolve_LocalSymbol(const char* name) {
    Symbol* res = NULL;
    if(res = is_Lib_Func(name)) { yyerror("Trying to redefine Library Function"); }
    else if(res = lookUp_CurrentScope(name)) { return res; }
    else {
        if(ht->currentScope==0) { res = insert_Symbol(name, GLOBAL_T); }
        else { res = insert_Symbol(name, LOCAL_T); }
    }
    return res;
}

Symbol* resolve_GlobalSymbol(const char* name) {
    Symbol* res = lookUp_GlobalScope(name);
    if(!res) { yyerror("Global Variable not found"); }
    return res;
}

/*===============================================================================================*/
/* Raw Symbol */

Symbol* resolve_RawSymbol(const char* name) {
    // VARIABLES:search all the previous scopes including current
    // until you find variable with the same name that can reach current scope
    // without bool isFunc getting in the way
    printf("Line %d symbol type NOT IMPLEMENTED YET!!\n", yylineno);
    // FUNCTIONS: all the previous scopes
    // globals always visible
    return NULL;
}

/*===============================================================================================*/
/* Final Steps */

void free_HashTable(void) {
    if (!ht) {
        return;
    }

    /* Free all symbols and their argument nodes via ScopeList */
    ScopeList* scope_curr = ht->ScopesHead;
    while (scope_curr) {
        Symbol* sym_curr = scope_curr->head;
        while (sym_curr) {
            argument_node* arg_curr = sym_curr->args;
            while (arg_curr) {
                argument_node* arg_next = arg_curr->next;
                free(arg_curr); 
                arg_curr = arg_next;
            }
            
            Symbol* sym_next = sym_curr->next_in_scope;
            free(sym_curr->name); 
            free(sym_curr);
            sym_curr = sym_next;
        }
        
        ScopeList* scope_next = scope_curr->next;
        free(scope_curr);
        scope_curr = scope_next;
    }

    /* Free the scope_snake stack */
    scope_snake* snake_curr = ht->ScopeSnakeHead;
    while (snake_curr) {
        scope_snake* snake_next = snake_curr->next;
        free(snake_curr);
        snake_curr = snake_next;
    }

    /* FINALLY: free the hash table */
    free(ht);
    ht = NULL; 
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

void printArgs(argument_node* node) {
    if(!node) { return; }
    else { printf(", %s", node->symbol->name); }
    printArgs(node->next);
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
            printf("\"%s\" [%s] (line %d) (scope %d)",
            symbol->name, symbolTypeToString(symbol->type),
            symbol->line, symbol->scope);
            if(symbol->type == USERFUNC_T) {
                if(!symbol->args) {
                    printf(" with no args");
                } else {
                    printf(" with args: %s", symbol->args->symbol->name);
                    printArgs(symbol->args->next);
                }
            }
            printf("\n");
        }
        symbol = symbol->next_in_scope;
    }
}

/* API */
void print_SymTable(void) {
    const ScopeList* scopeNode = ht->ScopesHead;
    printScopes(scopeNode);
}

/* end of table.c */
