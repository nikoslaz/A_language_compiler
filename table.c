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
    ht->scopeListHead = NULL;
    ht->currentScope = 0; //global scope

    /*inserting all the library functions*/
    insert_Symbol(ht, "print", LIBFUNC, 0, 0);
    insert_Symbol(ht, "input", LIBFUNC, 0, 0);
    insert_Symbol(ht, "objectmemberkeys", LIBFUNC, 0, 0);
    insert_Symbol(ht, "objecttotalmembers", LIBFUNC, 0, 0);
    insert_Symbol(ht, "objectcopy", LIBFUNC, 0, 0);
    insert_Symbol(ht, "totalarguments", LIBFUNC, 0, 0);
    insert_Symbol(ht, "argument", LIBFUNC, 0, 0);
    insert_Symbol(ht, "typeof", LIBFUNC, 0, 0);
    insert_Symbol(ht, "strtonum", LIBFUNC, 0, 0);
    insert_Symbol(ht, "sqrt", LIBFUNC, 0, 0);
    insert_Symbol(ht, "cos", LIBFUNC, 0, 0);
    insert_Symbol(ht, "sin", LIBFUNC, 0, 0);

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

/*i think we should do d*/


Symbol* insert_Symbol(HashTable* ht, const char* name, SymbolType type, int scope, int line) {
    Symbol* exists = lookUp_Symbol(ht, name, scope, 1);
    if (exists) {
        if (exists->type == LIBFUNC && (type == USERFUNC || type == LOCAL_T)) {
            ScopeList* scope_list = ht->scopeListHead;
            while (scope_list && scope_list->scope != exists->scope) {
                scope_list = scope_list->next_full;
            }
            if (scope_list) {
                Symbol* current = scope_list->head;
                Symbol* prev = NULL;
                while (current && strcmp(current->name, name) != 0) {
                    prev = current;
                    current = current->next_in_scope;
                }
                if (current) {
                    if (prev) {
                        prev->next_in_scope = current->next_in_scope;
                    } else {
                        scope_list->head = current->next_in_scope;
                    }
                }
            }
            unsigned int index = hash(name);
            Symbol* bucket_current = ht->buckets[index];
            Symbol* bucket_prev = NULL;
            while (bucket_current && strcmp(bucket_current->name, name) != 0) {
                bucket_prev = bucket_current;
                bucket_current = bucket_current->next_in_bucket;
            }
            if (bucket_current) {
                if (bucket_prev) {
                    bucket_prev->next_in_bucket = bucket_current->next_in_bucket;
                } else {
                    ht->buckets[index] = bucket_current->next_in_bucket;
                }
                free(bucket_current->name);
                free(bucket_current);
            }
        } else {
            printf("Symbol %s already exists in defined scope %d at line %d\n", name, scope, line);
            return NULL;
        }
    }

    Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
    if (!newSymbol) {
        printf("memory alloc failed for symbol\n");
        exit(1);
    }
    newSymbol->name = strdup(name);
    newSymbol->type = type;
    newSymbol->scope = scope;
    newSymbol->line = line;
    newSymbol->next_in_bucket = NULL;
    newSymbol->next_in_scope = NULL;

    unsigned int index = hash(name);
    newSymbol->next_in_bucket = ht->buckets[index];
    ht->buckets[index] = newSymbol;

    ScopeList* scope_list = ht->scopeListHead;
    while (scope_list && scope_list->scope != scope) {
        scope_list = scope_list->next_full;
    }
    if (!scope_list) {
        scope_list = (ScopeList*)malloc(sizeof(ScopeList));
        if (!scope_list) {
            fprintf(stderr, "Memory allocation failed for scope list\n");
            exit(1);
        }
        scope_list->head = NULL;
        scope_list->scope = scope;
        scope_list->next = NULL;
        scope_list->next_full = ht->scopeListHead;
        ht->scopeListHead = scope_list;
    }
    if (!scope_list->head) {
        scope_list->head = newSymbol;
    } else {
        Symbol* current = scope_list->head;
        while (current->next_in_scope) {
            current = current->next_in_scope;
        }
        current->next_in_scope = newSymbol;
    }

    return newSymbol;
}

void enter_Scope(HashTable* ht) {
    ScopeList* new_scope = (ScopeList*)malloc(sizeof(ScopeList));
    if (!new_scope) {
        fprintf(stderr, "Memory allocation failed for scope list\n");
        exit(1);
    }
    new_scope->head = NULL;
    new_scope->next = ht->scopes;
    new_scope->scope = ht->currentScope;
    ht->scopes = new_scope;
    new_scope->next_full = ht->scopeListHead; 
    printf("Entering scope %d\n", ht->currentScope);
    ht->currentScope++;
    printf("New current scope: %d\n", ht->currentScope);
}

/*I THINK THIS SHOULD BE CHANGED*/
/*modified based on alpha rules: variables in the current scope should be found first
if not found search in parent scopes up to scope 0
for :: only scope 0 should be checked.*/
Symbol* lookUp_Symbol(HashTable* ht, const char* name, int scope, int exact_scope) {
    if (exact_scope) {
        // Find the ScopeList node for the given scope
        ScopeList* scope_list = ht->scopeListHead;
        while (scope_list && scope_list->scope != scope) {
            scope_list = scope_list->next;
        }
        if (!scope_list) {
            return NULL;  // Scope not found
        }

        // Search only the symbols in this scope
        Symbol* current = scope_list->head;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                return current;
            }
            current = current->next_in_scope;
        }
        return NULL;
    } else {
        // Regular lookup: search all buckets, considering parent scopes
        unsigned int index = hash(name);
        Symbol* current = ht->buckets[index];

        while (current) {
            if (strcmp(current->name, name) == 0) {
                if (current->scope == scope || (scope >= 0 && current->scope <= scope)) {
                    return current;
                }
            }
            current = current->next_in_bucket;
        }
        return NULL;
    }
}

/*NIKOS*/
Symbol* lookUp_LocalSymbol(const char* name){
    //search only current scope if found return it else null also check lib functions!!
    
}

Symbol* insert_LocalSymbol(const char* name){
    //search only current scope if found return it else null
    //if scope 0 make it global, call insert global symbol, else insert local
}

Symbol* define_LocalSymbol(const char* name){
    //calls the lookup uf found return else insert local symbol
}




/*NIKOLETTA*/
Symbol* lookUp_GlobalSymbol(const char* name){
    //search only global scope if found return it else  ERROR
}


Symbol* insert_GlobalSymbol(const char* name){
    //save as global 
}

/*DEFINE GLOBAL HERE*/



/*LET IT BE!!!!*/

Symbol* lookUp_Symbol(const char* name){
    //VARIABLES:search all the previous scopes including current until you find variable with the same name that can reach current scope without bool isFUnc getting in the way
    //PIIPAPAAAAA
    //FUNCTIONS: all the previous scopes
    //global always visible
    
}

Symbol* insert_Symbol(const char* name){
    //check the scope: if 0 make it global else error
}



/*MIXALIS*/
Symbol* lookUp_FuncSymbol(const char* name){
    //only current scope and libfunc if found error else call insert
    
}

Symbol* insert_FuncSymbol(const char* name){
    //self explanatory
}


Symbol* lookUp_FormalSymbol(const char* name){
    //only current scope and libfunc if found error else call insert
    
}

Symbol* insert_FormalSymbol(const char* name){
    //same as local! but as formal arg
}



void exit_Scope(HashTable* ht) {
    if (!ht->scopes) {
        fprintf(stderr, "No scope to exit\n");
        return;
    }
    ht->scopes = ht->scopes->next;
    ht->currentScope--;
    if (ht->currentScope < 0) {
        fprintf(stderr, "Error: Attempted to exit below global scope\n");
        ht->currentScope = 0;
    }
}


void free_HashTable(HashTable* ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol* current = ht->buckets[i];
        while (current) {
            Symbol* temp = current;
            current = current->next_in_bucket;
            free(temp->name);
            free(temp);
        }
        ht->buckets[i] = NULL;
    }

    ScopeList* scope = ht->scopeListHead;
    while (scope) {
        ScopeList* temp = scope;
        scope = scope->next_full;
        free(temp);
    }
    ht->scopes = NULL;
    ht->scopeListHead = NULL;

    free(ht);
}

const char* symbolTypeToString(SymbolType type) {
    if (type == GLOBAL) {
        return "global variable";
    }
    else if (type == LOCAL_T) {
        return "local variable";
    }
    else if (type == FORMAL) {
        return "formal argument"; 
    }
    else if (type == USERFUNC) {
        return "user function";
    }
    else if (type == LIBFUNC) {
        return "library function";
    }
    else {
        return "unknown";
    }
}


void print_SymTable(HashTable* ht) {
    if (!ht) return;

    const ScopeList* scopeNode = ht->scopeListHead;
    if (!scopeNode) {
        printf("No scopes to print\n");
        return;
    }

    // Find the maximum scope number
    int max_scope = -1;
    const ScopeList* temp = scopeNode;
    while (temp) {
        if (temp->scope > max_scope) {
            max_scope = temp->scope;
        }
        temp = temp->next_full;
    }

    // Print symbols for each scope from 0 to max_scope
    for (int scope = 0; scope <= max_scope; scope++) {
        int scope_found = 0;
        scopeNode = ht->scopeListHead;
        while (scopeNode) {
            if (scopeNode->scope == scope) {
                if (!scope_found) {
                    printf("Scope #%d\n", scope);
                    printf("----------\n");
                    scope_found = 1;
                }

                const Symbol* symbol = scopeNode->head;
                while (symbol) {
                    const char* typeStr = symbolTypeToString(symbol->type);
                    printf("\"%s\" [%s] (line %d) (scope %d)\n", symbol->name, typeStr, symbol->line, symbol->scope);
                    symbol = symbol->next_in_scope;
                }
            }
            scopeNode = scopeNode->next_full;
        }
        if (scope_found) {
            printf("\n");  // Add a newline between scopes
        }
    }
}


// void print_SymTable(HashTable* ht) {
//     if (!ht) return;

//     const ScopeList* scopeNode = ht->scopes;
//     int scopeIndex = 0;

//     while (scopeNode) {
//         printf("---------- Scope %d ----------\n", scopeIndex);

//         const Symbol* symbol = scopeNode->head;
//         while (symbol) {
//             const char* typeStr = symbolTypeToString(symbol->type);

//             printf("\"%s\" [%s] (line %d) (scope %d)\n", symbol->name, typeStr, symbol->line, symbol->scope);

//             symbol = symbol->next_in_scope;
//         }

//         scopeNode = scopeNode->next;
//         scopeIndex++;
//     }
    
// }
