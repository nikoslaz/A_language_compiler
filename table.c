/* table.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "table.h"

/* Globals */
HashTable* ht;
int fromFunct = 0;
/* Counter for naming anonymous functions */
int AnonymousCounter = 0;
/* Pointer to the current function being processed */
Symbol* currFunction;

/* Helper Function */
void MemoryFail(void) {
    fprintf(stderr, "Fatal Error. Memory Allocation failed\n");
    exit(1);
}

/* yyerror */
int yyerror(char* yaccProvidedMessage) {
    fprintf(stderr, "Error in Line %d: %s\n",
    yylineno, yaccProvidedMessage);
}

/*===============================================================================================*/
/* Scope Handling */

ScopeList* int_to_Scope(int index) {
    ScopeList* scope_list = ht->ScopesHead;
    while(scope_list && (scope_list->scope != index)) {
        scope_list = scope_list->next;
    }
    if(!scope_list) {
        printf("Error. Tried to access Non-existent scope\n");
        exit(1);
    }
    return scope_list;
}

void enter_Next_Scope(int fromFunct) {
    ScopeList* scope_list;
    /* First Time */
    if(ht->currentScope == -1) { scope_list=NULL; }
    else {
        scope_list = int_to_Scope(ht->currentScope);
    }
    /* Increment Index */
    ht->currentScope++;
    /* If not already in, insert in descending order */
    if(!scope_list || ((ht->currentScope) > (ht->ScopesHead->scope))) {
        /* Create new node */
        ScopeList* new_scope = (ScopeList*)malloc(sizeof(ScopeList));
        if(!new_scope) { MemoryFail(); }
        new_scope->head = NULL;
        new_scope->scope = ht->currentScope;
        new_scope->isFunc = fromFunct;
        new_scope->scopeOffset=0;
        /* Link at the Start */
        new_scope->next = ht->ScopesHead;
        ht->ScopesHead = new_scope;
    }
    int_to_Scope(ht->currentScope)->isFunc = fromFunct;
}

void exit_Current_Scope(void) {
    /* Find current ScopeList */
    ScopeList* scope_list = int_to_Scope(ht->currentScope);
    /* Set isActve to 0 */
    Symbol* curr = scope_list->head;
    while(curr) {
        curr->isActive = 0;
        curr = curr->next_in_scope;
    }
    /* Reset isFunc */
    scope_list->isFunc=0;
    scope_list->scopeOffset=0;
    /* Exit Scope */
    ht->currentScope--;
}

int isGlobalishScope(void) {
    ScopeList* scope_list = int_to_Scope(ht->currentScope);
    while(scope_list->scope != 0) {
        if(scope_list->isFunc) { return 0; }
        scope_list = scope_list->next;
    }
    return 1;
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
    if(type!=LIBFUNC_T) {
        if(isGlobalishScope()) {
            new->offset = int_to_Scope(0)->scopeOffset++;
        } else {
            new->offset = int_to_Scope(ht->currentScope)->scopeOffset++;
        }
    } else { new->offset = 0; }
    new->args = NULL;
    new->next_in_scope = NULL;
    if(type==USERFUNC_T) { currFunction = new; }
    /* Link with HashTable */
    int index = hash(name);
    new->next_in_bucket = ht->buckets[index];
    ht->buckets[index] = new;
    /* Link with ScopeList */
    ScopeList* scope_list = int_to_Scope(ht->currentScope);
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
    for(int i=0; i < HASH_SIZE; i++) { ht->buckets[i] = NULL; }
    /* Create Scope 0 */
    ht->ScopesHead = NULL;
    ht->currentScope=-1;
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
    /* Search my Scope for any active Symbol with the same name */
    Symbol* curr = ht->buckets[hash(name)];
    while(curr) {
        if(curr->scope==ht->currentScope && strcmp(name, curr->name)==0 && curr->isActive) {
            return curr;
        }
        curr = curr->next_in_bucket;
    }
    return NULL;
}

Symbol* lookUp_GlobalScope(const char* name) {
    /* Search global Scope for any Symbol with the same name */
    ScopeList* scope_list = int_to_Scope(0);
    Symbol* current = scope_list->head;
    while(current) {
        if(strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next_in_scope;
    }
    return NULL;
}

/* Used only for functions */
void createArgumentNode(Symbol* mySymbol) {
    argument_node* newarg = (argument_node*)malloc(sizeof(argument_node));
    if(!newarg) { MemoryFail(); }
    newarg->symbol = mySymbol;
    newarg->next = NULL;
    /* Link at the end of args list */
    if(!currFunction) { return; }
    if(!currFunction->args) { currFunction->args = newarg; }
    else {
        argument_node* currnode = currFunction->args;
        while(currnode->next) { currnode = currnode->next; }
        currnode->next = newarg;
    }
}

const char* createNewFunctionName(void) {
    char* anon_name = (char*)malloc(10*sizeof(char));
    if(!anon_name) { MemoryFail(); }
    sprintf(anon_name, "$func%d", AnonymousCounter++);
    return anon_name;
}

void checkFunctionSymbol(struct Symbol* sym, const char* operation) {
    if (sym && (sym->type == USERFUNC_T || sym->type == LIBFUNC_T)) {
        char msg[30];
        sprintf(msg, "Cannot %s a function", operation);
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
    }
    /* Not already defined, insert it as a user function */ 
    else { res = insert_Symbol(name, USERFUNC_T); }
    return res;
}

Symbol* resolve_AnonymousFunc(void) {
    Symbol* res = NULL;
    if(AnonymousCounter>MAX_ANONYMOUS_FUNCTIONS) {
        yyerror("Maximum number of Anonymous Functions reached.");
        return res;
    }
    /* Anonymous are always added with a unique name */
    const char* anon_name = createNewFunctionName();
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
        /* Not already defined, insert it as a local or global variable */ 
        if(ht->currentScope==0) { res = insert_Symbol(name, GLOBAL_T); }
        else { res = insert_Symbol(name, LOCAL_T); }
    }
    return res;
}

Symbol* resolve_GlobalSymbol(const char* name) {
    /* Only check if it exists */
    Symbol* res = lookUp_GlobalScope(name);
    if(!res) { yyerror("Global Variable not found"); }
    return res;
}

/*===============================================================================================*/
/* Raw Symbol */

Symbol* is_User_Func(const char* name) {
    Symbol* curr = ht->buckets[hash(name)];
    while(curr) {
        if(curr->type==USERFUNC_T && strcmp(name, curr->name)==0 && curr->isActive) {
            return curr;
        }
        curr = curr->next_in_bucket;
    }
    return NULL;
}

Symbol* lookUp_All(const char* name, int* inaccessible) {
    /* Boolean to check if the symbol we find is accessible or not */
    *inaccessible = 0;
    int current_scope = ht->currentScope;
    ScopeList* scope = ht->ScopesHead;
    int hit_function_scope = 0;
    /* Work towards outer scopes */
    while (current_scope >= 0) {
        scope = int_to_Scope(current_scope);
        Symbol* sym = scope->head;
        while (sym) {
            if (strcmp(sym->name, name) == 0 && sym->isActive) {
                if (sym->type == USERFUNC_T || sym->type == LIBFUNC_T) {
                    return sym;
                } else {
                    /* Global always accessible */
                    if (sym->scope == 0) { return sym; }
                    if (hit_function_scope) {
                        /* found but inaccessible */
                        *inaccessible = 1;
                        return NULL;
                    }
                    return sym;
                }
            }
            sym = sym->next_in_scope;
        }
        /* Encounter a function, so i cant go further */
        if (scope->isFunc) { hit_function_scope = 1; }
        /* Move to previous scope */
        current_scope--;
    }
    /* Not found */
    return NULL;
}

Symbol* resolve_RawSymbol(const char* name) {
    // VARIABLES: search all the previous scopes including current
    // until you find variable with the same name that can reach current scope
    // without bool inaccessible getting in the way
    // FUNCTIONS: all the previous scopes
    // globals always visible   

    Symbol* res = NULL;
    int inaccessible = 0;

    if((res = is_Lib_Func(name))) {
        yyerror("Trying to redefine Library Function");
        return res;
    }
    if((res = is_User_Func(name))) {
        yyerror("Trying to redefine User Function");
        return res;
    }

    res = lookUp_All(name, &inaccessible);
    if(res) {
        return res;  /* Found and accessible! :D */
    } else if(inaccessible) {
        /* Inaccessible */
        int size = 28+strlen(name);
        char* msg = (char*)malloc(size*sizeof(char));
        if(!msg) { MemoryFail(); }
        sprintf(msg, "Symbol '%s' is not accessible", name);
        yyerror(msg);
        return NULL;
    } else {
        /* Not found, insert it in the Hashtable */
        SymbolType type = (ht->currentScope == 0) ? GLOBAL_T : LOCAL_T;
        return insert_Symbol(name, type);
    }
}

/*===============================================================================================*/
/* Function Calls */

Symbol* checkFunctionCall(Symbol* sym, const char* errorPrefix) {
    if (sym == NULL) {
        char msg[100];
        snprintf(msg, sizeof(msg), "%s: base symbol is NULL", errorPrefix);
        yyerror(msg);
        return NULL;
    }
    return sym;
}

Symbol* handleAnonymousFuncCall(Symbol* funcdef) {
    if(funcdef == NULL) {
        yyerror("Invalid anonymous function call: funcdef is NULL");
        return NULL;
    }
    return funcdef;
}

Symbol* createTempSymbol(void) {
    /* Returns a symbol but does not insert it in Symbol Table (function) */
    static int temp_counter = 0;
    char temp_name[32];
    snprintf(temp_name, sizeof(temp_name), "__temp%d", temp_counter++);

    Symbol* new = (Symbol*)malloc(sizeof(Symbol));
    if (!new) {
        fprintf(stderr, "Fatal Error. Memory Allocation failed\n");
        exit(1);
    }

    new->name = strdup(temp_name);
    new->type = LOCAL_T;  
    new->scope = ht->currentScope;  
    new->line = yylineno;  
    new->isActive = 1;  
    new->args = NULL;  
    new->next_in_scope = NULL;  
    new->next_in_bucket = NULL;  

    return new;
}

/*===============================================================================================*/
/* Final Steps */

void free_HashTable(void) {
    if (!ht) { return; }

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
        case TEMPORARY_T: return "temp variable";
        default: return "unknown";
    }
    assert(0);
}

void printArgs(argument_node* node) {
    if(!node) { return; }
    else { printf(", %s", node->symbol->name); }
    printArgs(node->next);
}

void printScopes(const ScopeList* scopelist) {
    if (!scopelist) { return; }
    
    printScopes(scopelist->next);
    
    printf("\n-----------------------  Scope %-2d  ------------------------------\n", scopelist->scope);
    printf("| %-15s | %-15s | %-6s | %-6s | %-7s |\n", "Name", "Type", "Line", "Scope", "Offset");
    printf("|-----------------|-----------------|--------|--------|---------|\n");
    
    Symbol* symbol = scopelist->head;
    while (symbol) {
        if (symbol->type != LIBFUNC_T) {
            printf("| %-15s | %-15s | %-6d | %-6d | %-7d |",
                   symbol->name,
                   symbolTypeToString(symbol->type),
                   symbol->line,
                   symbol->scope,
                   symbol->offset);
            if (symbol->type == USERFUNC_T) {
                if (!symbol->args) {
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
    printf("-----------------------------------------------------------------\n");
}

/* API */
void print_SymTable(void) {
    const ScopeList* scopeNode = ht->ScopesHead;
    printScopes(scopeNode);
    printf("\n\n");
}

/* end of table.c */
