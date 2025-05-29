/* quads.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "quads.h"

quad* quads = NULL;
LoopContext* loop_stack = NULL;
ReturnContext* return_stack = NULL;
unsigned int totalquads = 0;
unsigned int currquad = 0;
unsigned int from_method = 0;

quad* emit(opcode op, expr* result, expr* arg1, expr* arg2, unsigned int label) {
    if(currquad == totalquads) {
        totalquads += EXPAND_SIZE;
        quads = realloc(quads, totalquads * sizeof(quad));
        if(!quads) { MemoryFail(); }
    }
    quad* new = (quad*)malloc(sizeof(quad));
    if(!new) { MemoryFail(); }
    new->op = op;
    new->arg1 = arg1;
    new->arg2 = arg2;
    new->result = result;
    new->label = label;
    new->line = yylineno;
    quads[currquad++] = *new;
    return new;
}

expr* emit_if_table_item_get(expr* e, expr* result) {
    if(e && e->type != EXP_TABLEITEM) {
        e->boolConst = 0;
        return e;
    } else {
        if(!result) { result = create_var_expr(get_temp_symbol()); }
        result->type = EXP_TABLEITEM;
        if(e) { emit(OP_TABLEGETELEM, result, e, e->index, 0); }
        else { printf("Error in emit if GET. NULL e given\n"); }
        result->boolConst = 1;
        return result; 
    }
}

void emit_if_table_item_set(expr* table, expr* arg2) {
    if(table && table->type == EXP_TABLEITEM) {
        emit(OP_TABLESETELEM, table, table->index, arg2, 0);
        table->boolConst = 1;
    } else {
        if(table) table->boolConst = 0;
        else { printf("Error in emit if SET. NULL table given\n"); }
    }
}

/*===============================================================================================*/
/* Expression Constructors */

expr* create_arith_expr(Symbol* sym) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_ARITH;
    temp->symbol=sym;
    temp->index=NULL;
    temp->numConst=0;
    temp->stringConst=NULL;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_bool_expr(Symbol* sym) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_BOOL;
    temp->symbol=sym;
    temp->index=NULL;
    temp->numConst=0;
    temp->stringConst=NULL;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_empty_bool_expr(void) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_BOOL;
    temp->symbol=NULL;
    temp->index=NULL;
    temp->numConst=0;
    temp->stringConst=NULL;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_var_expr(Symbol* symbol) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_VARIABLE;
    temp->symbol=symbol;
    temp->index=NULL;
    temp->numConst=0;
    temp->stringConst=0;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_prog_func_expr(Symbol* symbol) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_PROGRAMFUNC;
    temp->symbol=symbol;
    temp->index=NULL;
    temp->numConst=0;
    temp->stringConst=0;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_constnum_expr(double value) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_CONSTNUMBER;
    temp->symbol=NULL;
    temp->index=NULL;
    temp->numConst=value;
    temp->stringConst=0;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_conststring_expr(char* value) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_CONSTSTRING;
    temp->symbol=NULL;
    temp->numConst=0;
    temp->stringConst=value;
    temp->boolConst=0;
    temp->index=NULL;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_constbool_expr(unsigned int value) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_CONSTBOOL;
    temp->symbol=NULL;
    temp->numConst=0;
    temp->stringConst=NULL;
    temp->boolConst=value;
    temp->index=NULL;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_nil_expr(void) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_NIL;
    temp->symbol=NULL;
    temp->numConst=0;
    temp->stringConst=NULL;
    temp->boolConst=0;
    temp->index=NULL;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

expr* create_table_expr(Symbol* sym) {
    expr* temp=(expr*)malloc(sizeof(expr));
    if(!temp) { MemoryFail(); }
    temp->type=EXP_NEWTABLE;
    temp->symbol=sym;
    temp->index=NULL;
    temp->numConst=0;
    temp->stringConst=0;
    temp->boolConst=0;
    temp->next=NULL;
    temp->truelist=NULL;
    temp->falselist=NULL;
    return temp;
}

/*===============================================================================================*/
/* Backpatch Functions */

unsigned int nextquad(void) { return currquad; }

PatchList* makelist(unsigned int quad_index) {
    PatchList* new_node = (PatchList*)malloc(sizeof(PatchList));
    if(!new_node) { MemoryFail(); }
    new_node->quad_index = quad_index;
    new_node->next = NULL;
    return new_node;
}

PatchList* merge(PatchList* list1, PatchList* list2) {
    if(!list1) return list2;
    if(!list2) return list1;
    PatchList* iter = list1;
    while(iter->next != NULL) iter = iter->next;
    iter->next = list2;
    return list1;
}

void backpatch(PatchList* list, unsigned int target_quad_index) {
    PatchList* iter = list;
    while(iter) {
        if(iter->quad_index >= currquad) {
            fprintf(stderr, "Error: invalid index\n");
        } else {
            quads[iter->quad_index].label = target_quad_index;
        }
        iter = iter->next;
    }
}

void simplepatch(unsigned int quad, unsigned int index) {
    if(quad >= currquad || index > currquad) {
        fprintf(stderr, "Error: invalid index\n");
    } else {
        quads[quad].label = index;
    }
}

/*===============================================================================================*/
/* Elist Functions */

void handle_arguments(expr* arg) {
    if(arg) {
        handle_arguments(arg->next);
        emit(OP_PARAM, NULL, arg, NULL, 0);
    }
}

void push_return(void) {
    ReturnContext* new = (ReturnContext*)malloc(sizeof(ReturnContext));
    if(!new) { MemoryFail(); }
    new->return_list = NULL;
    new->next = return_stack;
    return_stack = new;
}

void pop_return(void) {
    if(return_stack) {
        ReturnContext* top = return_stack;
        return_stack = top->next;
        free(top);
    }
}

void add_to_return_list(unsigned int quad_to_patch) {
    if(return_stack) {
        return_stack->return_list = merge(return_stack->return_list, makelist(quad_to_patch));
    } else {
        yyerror("RETURN statement encountered outside of a function.");
    }
}

/*===============================================================================================*/
/* STACK FOR BREAK/CONTINUE LISTS */

void push_loop(void) {
    LoopContext* new = (LoopContext*)malloc(sizeof(LoopContext));
    if(!new) { MemoryFail(); }
    new->break_list = NULL;
    new->continue_list = NULL;
    new->next = loop_stack;
    loop_stack = new;
}

void pop_loop(void) {
    if(loop_stack) {
        LoopContext* top = loop_stack;
        loop_stack = top->next;
        free(top);
    }
}

void add_to_breakList(unsigned int quad_to_patch) {
    if(loop_stack) {
        loop_stack->break_list = merge(loop_stack->break_list, makelist(quad_to_patch));
    } else {
        yyerror("BREAK statement encountered outside of a loop stack.");
    }
}

void add_to_continueList(unsigned int quad_to_patch) {
    if(loop_stack) {
        loop_stack->continue_list = merge(loop_stack->continue_list, makelist(quad_to_patch));
    } else {
        yyerror("CONTINUE statement encountered outside of a loop stack.");
    }
}

/*===============================================================================================*/
/* Temporary Symbols */

unsigned int temp_counter = 0;
Symbol* temp_array[MAX_TEMPS];

Symbol* get_temp_symbol(void) {
    for(int index=0; index<MAX_TEMPS; index++) {
        if(temp_array[index]) {
            if(temp_array[index]->isActive) continue;
            else {
                temp_array[index]->isActive = 1;
                return temp_array[index];
            }
        } else {
            temp_array[index] = insert_Global_Temp_Symbol();
            return temp_array[index];
        }
    }
    printf("Outside of temp array!\n");
    return insert_Global_Temp_Symbol();
}

unsigned int hasTempSymbol(expr* ex) {
    if(ex && ex->symbol && ex->symbol->type == TEMPORARY_T) {
        return 1;
    } else { return 0; }
}

void freeIfTemp(expr* ex) {
    if(ex && ex->symbol && ex->symbol->type == TEMPORARY_T) {
        if(ex->symbol->isActive) ex->symbol->isActive = 0;
        else printf("Trying to free freed temp symbol [%s] :(\n", ex->symbol->name);
    }
}

void reset_temps(void) {
    for(int i=0; i<temp_counter; i++) {
        temp_array[i]->isActive = 0;
    }
}

void initialize_temp_array(void) {
    for(int i=0; i<MAX_TEMPS; i++) temp_array[i] = NULL;
}

Symbol* insert_Global_Temp_Symbol(void) {
    /* Create new Node */
    Symbol* new = (Symbol*)malloc(sizeof(Symbol));
    if(!new) { MemoryFail(); }
    /* Create new Name */
    if(temp_counter>MAX_TEMPS) {
        printf("Error. Maximum number of temporary variables reached. Sorry:(\n");
        exit(-1);
    }
    char temp_name[5];
    snprintf(temp_name, sizeof(temp_name), "_t%d", temp_counter++);
    /* Fields */
    ScopeList* scope_list = int_to_Scope(0);
    new->isActive = 1;
    new->line = yylineno;
    new->name = strdup(temp_name);
    new->type = TEMPORARY_T;
    new->scope = 0;
    new->offset = scope_list->scopeOffset++;
    new->args = NULL;
    new->num_args=0;
    new->num_locals=0;
    new->quad_addr=0;
    new->next_in_scope = NULL;
    /* Link with HashTable */
    int index = hash(temp_name);
    new->next_in_bucket = ht->buckets[index];
    ht->buckets[index] = new;
    /* Link with ScopeList */
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
/* Print */

const char* opcodeToStr(opcode op) {
    switch (op) {
        case OP_ASSIGN: return "ASSIGN";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_UMINUS: return "UMINUS";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_IFEQ: return "IFEQ";
        case OP_IFNOTEQ: return "IFNOTEQ";
        case OP_IFLESSEQ: return "IFLESSEQ";
        case OP_IFGREATEREQ: return "IFGREATEREQ";
        case OP_IFLESS: return "IFLESS";
        case OP_IFGREATER: return "IFGREATER";
        case OP_CALL: return "CALL";
        case OP_PARAM: return "PARAM";
        case OP_RETURN: return "RETURN";
        case OP_GETRETVAL: return "GETRETVAL";
        case OP_FUNCSTART: return "FUNCSTART";
        case OP_FUNCEND: return "FUNCEND";
        case OP_TABLECREATE: return "TABLECREATE";
        case OP_TABLEGETELEM: return "TABLEGETELEM";
        case OP_TABLESETELEM: return "TABLESETELEM";
        case OP_JUMP: return "JUMP";
        default: return "PRINT ERROR";
    }
}

const char* exprToStr(expr* e) {
    if(!e) return "-";
    char* buffer = (char*)malloc(1024*sizeof(char));
    switch (e->type) {
        case EXP_VARIABLE:
        case EXP_PROGRAMFUNC:
        case EXP_LIBRARYFUNC:
        case EXP_ARITH:
        case EXP_BOOL:
        case EXP_ASSIGN:
        case EXP_TABLEITEM:
        case EXP_NEWTABLE:
            return e->symbol ? e->symbol->name : "print unknown symbol";
        case EXP_CONSTNUMBER:
            snprintf(buffer, sizeof(buffer), "%.2f", e->numConst);
            return buffer;
        case EXP_CONSTSTRING:
            return e->stringConst ? e->stringConst : (char*)"\"\"";
        case EXP_CONSTBOOL:
            return e->boolConst ? (char*)"TRUE" : (char*)"FALSE";
        case EXP_NIL:
            return "NIL";
        default:
            return "PRINT ERROR";
    }
}

void printQuads(void) {
    printf("\n%-5s|   %-3s %-14s %-15s %-15s %-15s %-5s\n", "LINE", "#", "OP", "RESULT", "ARG1", "ARG2", "LABEL");
    printf("-----+----------------------------------------------------------------------------\n");
    for (unsigned int i = 0; i < currquad; ++i) {
        if(quads[i].op == OP_FUNCSTART) { printf("     |\n"); }
        printf(" %-4u|   %-3u %-14s %-15s %-15s %-15s",
        quads[i].line, i+1, opcodeToStr(quads[i].op),
        exprToStr(quads[i].result), exprToStr(quads[i].arg1), exprToStr(quads[i].arg2));
        if(quads[i].op==OP_JUMP || quads[i].op==OP_IFEQ || quads[i].op==OP_IFNOTEQ ||
        quads[i].op==OP_IFGREATER || quads[i].op==OP_IFGREATEREQ || quads[i].op==OP_IFLESS || quads[i].op==OP_IFLESSEQ)
        { printf(" %-5u\n", quads[i].label+1); }
        else { printf("\n"); }
        if(quads[i].op == OP_FUNCEND) { printf("     |\n"); }
    }
    printf("-----+----------------------------------------------------------------------------\n\n");
}

void printQuadsToFile(FILE* file) {
    fprintf(file, "%-4s|   %-3s %-14s %-15s %-15s %-15s %-5s\n", "LINE", "#", "OP", "RESULT", "ARG1", "ARG2", "LABEL");
    for(unsigned int i = 0; i < currquad; ++i) {
        if(quads[i].op == OP_FUNCSTART) { fprintf(file, "    |\n"); }
        fprintf(file, " %-3u|   %-3u %-14s %-15s %-15s %-15s",
        quads[i].line, i+1, opcodeToStr(quads[i].op),
        exprToStr(quads[i].result), exprToStr(quads[i].arg1), exprToStr(quads[i].arg2));
        if(quads[i].op==OP_JUMP || quads[i].op==OP_IFEQ || quads[i].op==OP_IFNOTEQ ||
        quads[i].op==OP_IFGREATER || quads[i].op==OP_IFGREATEREQ || quads[i].op==OP_IFLESS || quads[i].op==OP_IFLESSEQ)
        { fprintf(file, " %-5u\n", quads[i].label+1); }
        else { fprintf(file, "\n"); }
        if(quads[i].op == OP_FUNCEND) { fprintf(file, "    |\n"); }
    }
    fclose(file);
}

/* end of quads.c */
