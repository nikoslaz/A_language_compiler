/* avm_executors.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

arithmetic_func_t arith_funcs[] = {
    add_arith, sub_arith, mul_arith,
    div_arith, mod_arith, uminus_arith
};

double add_arith(double x, double y) { return (double)(x+y); }
double sub_arith(double x, double y) { return (double)(x-y); }
double mul_arith(double x, double y) { return (double)(x*y); }
double div_arith(double x, double y) { return (double)(x/y); }
double mod_arith(double x, double y) { return (double)(((unsigned)x)%((unsigned)y)); }
double uminus_arith(double x, double y) { runtimeError("UMINUS SHOULD NOT EXIST"); }

relational_func_t relat_funcs[] = {
	jle_rel, jge_rel, jlt_rel, jgt_rel
};

unsigned int jgt_rel(double x, double y) { return x > y; }
unsigned int jle_rel(double x, double y) { return x <= y; }
unsigned int jge_rel(double x, double y) { return x >= y; }
unsigned int jlt_rel(double x, double y) { return x < y; }

tobool_func_t to_bool_funcs[] = {
    number_tobool, string_tobool, bool_tobool,
    table_tobool, userfunc_tobool, libfunc_tobool,
    nil_tobool, stackval_tobool, undef_tobool
};

unsigned int number_tobool(memcell* mem)   { return mem->data.num_zoumi != 0; }
unsigned int string_tobool(memcell* mem)   { return mem->data.string_zoumi[0] != 0; }
unsigned int bool_tobool(memcell* mem)     { return mem->data.bool_zoumi; }
unsigned int table_tobool(memcell* mem)    { return 1; }
unsigned int userfunc_tobool(memcell* mem) { return 1; }
unsigned int libfunc_tobool(memcell* mem)  { return 1; }
unsigned int nil_tobool(memcell* mem)      { return 0; }
unsigned int stackval_tobool(memcell* mem) { return 1; }
unsigned int undef_tobool(memcell* mem)    { return 0; }

/*===============================================================================================*/
/* Helpers */

void helper_assign(memcell* lv, memcell* rv) {
    if(lv == rv) { return; }
    /* TODO check for tables */
    if(rv->type == MEM_UNDEF) { runtimeWarning("Assignment of undef"); }
    clear_memcell(lv);
    memcpy(lv, rv, sizeof(memcell));
    /* TODO again check tables */
}

void helper_arith(instruction* inst) {
    /* Translate LValue */
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE"); }
    /* Translate Arg1 */
    memcell* arg1 = (memcell*)malloc(sizeof(memcell));
    if(!arg1) { MemoryFail(); }
    arg1 = translate_operand(&inst->arg1, arg1);
    if(!arg1) { runtimeError("Null ARG1"); }
    /* Translate Arg2 */
    memcell* arg2 = (memcell*)malloc(sizeof(memcell));
    if(!arg2) { MemoryFail(); }
    arg2 = translate_operand(&inst->arg2, arg2);
    if(!arg2) { runtimeError("Null ARG2"); }
    /* Do j*b */
    if(arg1->type != MEM_NUMBER) { runtimeError("ARG1 is not a NUMBER"); }
    if(arg2->type != MEM_NUMBER) { runtimeError("ARG2 is not a NUMBER"); }
    memcell res;
    res.type = MEM_NUMBER;
    res.data.num_zoumi = (*arith_funcs[inst->opcode-OP_ADD])(arg1->data.num_zoumi, arg2->data.num_zoumi);
    helper_assign(lv, &res);
}

void helper_relational(instruction* inst) {
    /* Translate Arg1 */
    memcell* rv1 = (memcell*)malloc(sizeof(memcell));
    if(!rv1) { MemoryFail(); }
    rv1 = translate_operand(&inst->arg1, rv1);
    if(!rv1) { runtimeError("Null RV1"); }
    /* Translate Arg2 */
    memcell* rv2 = (memcell*)malloc(sizeof(memcell));
    if(!rv2) { MemoryFail(); }
    rv2 = translate_operand(&inst->arg2, rv2);
    if(!rv2) { runtimeError("Null RV2"); }
    /* Do j*b */
    if(rv1->type != MEM_NUMBER) { runtimeError("ARG1 is not a NUMBER"); }
    if(rv1->type != MEM_NUMBER) { runtimeError("ARG2 is not a NUMBER"); } 
    unsigned int res = 0;
    res = (*relat_funcs[inst->opcode - OP_JLE])(rv1->data.num_zoumi, rv2->data.num_zoumi);
    if(res) { branch_to(inst->result.val); }
}

void helper_equality(instruction* inst, int is_equal) {
    /* Translate Arg1 */
    memcell* rv1 = (memcell*)malloc(sizeof(memcell));
    if (!rv1) { MemoryFail(); }
    rv1 = translate_operand(&inst->arg1, rv1);
    if (!rv1) { runtimeError("Null RV1"); }

    /* Translate Arg2 */
    memcell* rv2 = (memcell*)malloc(sizeof(memcell));
    if (!rv2) { MemoryFail(); }
    rv2 = translate_operand(&inst->arg2, rv2);
    if (!rv2) { runtimeError("Null RV2"); }

    /* Perform equality/inequality check */
    unsigned int res = 0;
    if (rv1->type == MEM_UNDEF || rv2->type == MEM_UNDEF) {
        runtimeError("Undef in equality/inequality");
    } else if (rv1->type == MEM_NIL || rv2->type == MEM_NIL) {
        res = (rv1->type == MEM_NIL && rv2->type == MEM_NIL);
        if (!is_equal) { res = !res; }
    } else if (rv1->type == MEM_BOOL || rv2->type == MEM_BOOL) {
        unsigned int bool1 = (*to_bool_funcs[rv1->type])(rv1);
        unsigned int bool2 = (*to_bool_funcs[rv2->type])(rv2);
        res = (bool1 == bool2);
        if (!is_equal) { res = !res; }
    } else {
        res = equality_check(rv1, rv2);
        if (!is_equal) { res = !res; }
    }

    if (rv1->type != rv2->type) {
        res = is_equal ? 0 : 1;
    }

    if (res) { branch_to(inst->result.val); }
}

/*===============================================================================================*/
/* Executors */

execute_func_t executors[] = {
    execute_ASSIGN,
    execute_ADD, execute_SUB, execute_MUL, execute_DIV, execute_MOD,
    execute_UMINUS,
    execute_AND, execute_OR, execute_NOT,    
    execute_JEQ, execute_JNE, execute_JLE, execute_JGE, execute_JLT, execute_JGT,      
    execute_CALL, execute_PARAM, execute_RETURN, execute_GETRETVAL, execute_FUNCSTART, execute_FUNCEND,
    execute_NEWTABLE, execute_TABLEGETELEM, execute_TABLESETELEM,
    execute_JUMP,
    execute_NOP
};

void execute_UMINUS(instruction* inst) { printf("UMINUS SHOULD NOT EXIST\n"); }
void execute_AND(instruction* inst)    { printf("AND SHOULD NOT EXIST\n"); }
void execute_OR(instruction* inst)     { printf("OR SHOULD NOT EXIST\n"); }
void execute_NOT(instruction* inst)    { printf("NOT SHOULD NOT EXIST\n"); }
void execute_NOP(instruction* inst)    { printf("NOP SHOULD NOT EXIST\n"); }

/*===============================================================================================*/
/* Assigns */

void execute_ASSIGN(instruction* inst) {
    /* Translate LValue */
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE in assign"); }
    /* Translate RValue */
    memcell* rv = (memcell*)malloc(sizeof(memcell));
    if(!rv) { MemoryFail(); }
    rv = translate_operand(&inst->arg1, rv);
    if(!rv) { runtimeError("Null RVALUE in assign"); }
    /* do j*b */
    helper_assign(lv, rv);
}

void execute_RETURN(instruction* inst) {
    /* Translate Result */
    memcell* res = (memcell*)malloc(sizeof(memcell));
    if(!res) { MemoryFail(); }
    res = translate_operand(&inst->result, res);
    if(!res) { runtimeError("Null RVALUE in return"); }
    /* do j*b */
    helper_assign(&stack[0], res);
}

void execute_GETRETVAL(instruction* inst) {
    /* Pop total params */
    int totals = pop().data.stackval_zoumi;
    /* Pop that many params */
    for(int i=0; i<totals; i++) { pop(); }
    /* Translate LValue */
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE in getretval"); }
    /* to j*b */
    helper_assign(lv, &stack[0]);
}

/*===============================================================================================*/
/* Arith */

void execute_ADD(instruction* inst) { helper_arith(inst); }
void execute_SUB(instruction* inst) { helper_arith(inst); }
void execute_MUL(instruction* inst) { helper_arith(inst); }
void execute_DIV(instruction* inst) { helper_arith(inst); }
void execute_MOD(instruction* inst) { helper_arith(inst); }

/*===============================================================================================*/
/* Jumps */

void execute_JUMP(instruction* inst) { branch_to(inst->result.val); }

void execute_JEQ(instruction* inst) { helper_equality(inst,1); }
void execute_JNE(instruction* inst) { helper_equality(inst,0); }

void execute_JLE(instruction* inst) { helper_relational(inst); }
void execute_JGE(instruction* inst) { helper_relational(inst); }
void execute_JLT(instruction* inst) { helper_relational(inst); }
void execute_JGT(instruction* inst) { helper_relational(inst); }

/*===============================================================================================*/
/* Functions */

void execute_PARAM(instruction* inst) {
    /* Translate Result */
    memcell* res = (memcell*)malloc(sizeof(memcell));
    if(!res) { MemoryFail(); }
    res = translate_operand(&inst->arg1, res);
    if(!res) { runtimeError("Null memcell in param"); }
    /* do j*b */
    push(*res);
    current_args_pushed++;
}

void execute_CALL(instruction* inst) {
    /* Translate Function Address */
    memcell* func = (memcell*)malloc(sizeof(memcell));
    if(!func) { MemoryFail(); }
    func = translate_operand(&inst->arg1, func);
    if(!func) { runtimeError("Null memcell in call"); }
    /* Push total args */
    memcell totalargs;
    clear_memcell(&totalargs);
    totalargs.type = MEM_STACKVAL;
    totalargs.data.stackval_zoumi = current_args_pushed;
    push(totalargs);
    current_args_pushed=0;
    /* Prepare retval */
    clear_memcell(&stack[0]);
    stack[0].type = MEM_NIL;
    /* do j*b */
    switch(func->type) {
        case MEM_LIBFUNC:
            fprintf(avm_log, "Calling libfunc %d\n", func->data.libfunc_zoumi);
            (*libFuncs[func->data.libfunc_zoumi])();
            break;
        case MEM_USERFUNC:
            /* Push Return Address */
            totalargs.data.stackval_zoumi = program_counter + 1;
            push(totalargs);
            /* Branch to Function Address */
            branch_to(func->data.usrfunc_zoumi);
            break;
        default: runtimeError("Argument is not a function");
    }
}

void execute_FUNCSTART(instruction* inst) {
    /* Push Old DARTH MAUL */
    memcell oldmaul;
    oldmaul.type = AVM_STACKSIZE;
    oldmaul.data.stackval_zoumi = stack_maul;
    push(oldmaul);
    /* Create new local environment */
    stack_maul = stack_top+1;
    int locals = inst->arg2.val;
    memcell tmp_local;
    clear_memcell(&tmp_local);
    for(int i=0; i<locals; i++) { push(tmp_local); }
}

void execute_FUNCEND(instruction* inst) {
    /* Pop my locals */
    int locals = (stack_top < stack_maul) ? 0 : (stack_top - stack_maul + 1);
    for(int i=0; i<locals; i++) { pop(); }
    /* Pop and Restore old DARTH MAUL */
    stack_maul = pop().data.stackval_zoumi;
    /* Pop return address */
    int retaddr = pop().data.stackval_zoumi;
    /* Branch to return address */
    branch_to(retaddr);
}

/*===============================================================================================*/
/* Tables */

unsigned int hash(memcell* t) { return (*hashes[t->type])(t); }

hash_t hashes[] = {
    number_hash, string_hash, bool_hash,
    table_hash, userfunc_hash, libfunc_hash,
    nil_hash, stackval_hash, undef_hash
};

unsigned int number_hash(memcell* key)   { return ((unsigned)key->data.num_zoumi) % HASHTABLE_SIZE; }
unsigned int string_hash(memcell* key)   { return ((unsigned)key->data.string_zoumi) % HASHTABLE_SIZE; }
unsigned int bool_hash(memcell* key)     { return ((unsigned)key->data.bool_zoumi) % HASHTABLE_SIZE; }
unsigned int table_hash(memcell* key)    { return ((unsigned)key->data.table_zoumi) % HASHTABLE_SIZE; }
unsigned int userfunc_hash(memcell* key) { return ((unsigned)key->data.usrfunc_zoumi) % HASHTABLE_SIZE; }
unsigned int libfunc_hash(memcell* key)  { return ((unsigned)key->data.libfunc_zoumi) % HASHTABLE_SIZE; }
unsigned int stackval_hash(memcell* key) { return ((unsigned)key->data.stackval_zoumi) % HASHTABLE_SIZE; }
unsigned int nil_hash(memcell* key)      { return 0; }
unsigned int undef_hash(memcell* key)    { return 210; }

unsigned int equality_check(memcell* v1, memcell* v2) { return (*equalities[v1->type])(v1, v2); }

are_equals_t equalities[] = {
    number_equality, string_equality, bool_equality,
    table_equality, userfunc_equality, libfunc_equality,
    nil_equality, stackval_equality, undef_equality
};

unsigned int number_equality(memcell* v1, memcell* v2)   { return v1->data.num_zoumi == v2->data.num_zoumi; }
unsigned int string_equality(memcell* v1, memcell* v2)   { return v1->data.string_zoumi == v2->data.string_zoumi; }
unsigned int bool_equality(memcell* v1, memcell* v2)     { return v1->data.bool_zoumi == v2->data.bool_zoumi; }
unsigned int table_equality(memcell* v1, memcell* v2)    { return v1->data.table_zoumi == v2->data.table_zoumi; }
unsigned int userfunc_equality(memcell* v1, memcell* v2) { return v1->data.usrfunc_zoumi == v2->data.usrfunc_zoumi; }
unsigned int libfunc_equality(memcell* v1, memcell* v2)  { return v1->data.libfunc_zoumi == v2->data.libfunc_zoumi; }
unsigned int stackval_equality(memcell* v1, memcell* v2) { return v1->data.stackval_zoumi == v2->data.stackval_zoumi; }
unsigned int nil_equality(memcell* v1, memcell* v2)      { return 1; }
unsigned int undef_equality(memcell* v1, memcell* v2)    { return 1; }

table* table_new(void){
    table* t = (table*) malloc(sizeof(table));
    if(!t) {
        runtimeError("Fatal: Could not allocate memory for new table.");
    }
    clear_memcell(&t);
    t->ref_count = 0;
    t->total = 0;
    table_bucketsinit(t->hashtable);
    return t;
}

void table_destroy(table* t){
    table_bucketsdestroy(t->hashtable);
    free(t);
}

memcell* table_GET(memcell* table, memcell* key) {
    int index = hash(key);
    table_bucket* bucket = table->data.table_zoumi->hashtable[index];
    table_bucket* node = NULL;
    while(bucket) {
        if(bucket->key.type == key->type) {
            if(equality_check(&bucket->key, key)) {
                node = bucket;
                break;
            }
        }
        bucket = bucket->next;
    }
    if(node) { return &node->value; }
    else {
        /* Return NULL */
        memcell* tmp = (memcell*)malloc(sizeof(memcell));
        if(!tmp) { MemoryFail(); }
        clear_memcell(&tmp);
        tmp->type = MEM_NIL;
        return tmp;
    }
}

void table_SET(memcell* table, memcell* key, memcell* value) {
    int index = hash(key);
    table_bucket* bucket = table->data.table_zoumi->hashtable[index];
    table_bucket* node = NULL;
    while(bucket) {
        if(bucket->key.type == key->type && equality_check(&bucket->key, key)) {
            node = bucket;
            break;
        }
        bucket = bucket->next;
    }
    if(node) {
        /* Entry exists */
        if(node->value.type!=MEM_NIL && value->type==MEM_NIL) { table->data.table_zoumi->total--; }
        node->value = *value;
    } else {
        /* New entry, place first onto bucket */
        table_bucket* tmp = (table_bucket*)malloc(sizeof(table_bucket));
        if(!tmp) { MemoryFail(); }
        tmp->key = *key;
        tmp->value = *value;
        tmp->next = bucket;
        table->data.table_zoumi->hashtable[index] = tmp;
        if(value->type != MEM_NIL) { table->data.table_zoumi->total++; }
    }
}

void table_bucketsdestroy(table_bucket** hash){
    for (unsigned int i = 0; i < HASHTABLE_SIZE; ++i, ++hash) {
        for (table_bucket* b = *hash; b != NULL; ) {
            table_bucket* del = b;
            b = b->next;
            clear_memcell(&del->key);
            clear_memcell(&del->value);
            
            free(del);
        }
        hash[i] = (table_bucket *)0;
    }
}

void table_bucketsinit(table_bucket** hash){
    for (unsigned int i = 0; i < HASHTABLE_SIZE; ++i) {
        hash[i] = (table_bucket *)0;    
    }
}

void table_decrementcounter(table* t){
    // assert(t->ref_count > 0);
    
    if (!--t->ref_count) {
        table_destroy(t);
    }
}


void execute_NEWTABLE(instruction* inst) {
    runtimeError("NEWTABLE NOT IMPLEMENTED YET");
}

void execute_TABLEGETELEM(instruction* inst) {
    runtimeError("TABLEGET NOT IMPLEMENTED YET");
}

void execute_TABLESETELEM(instruction* inst) {
    runtimeError("TABLESET NOT IMPLEMENTED YET");
}

/*===============================================================================================*/
/* ToString */

tostring_func_t to_string_funcs[] = {
    number_tostring, string_tostring, bool_tostring,
    table_tostring, userfunc_tostring, libfunc_tostring,
    nil_tostring, stackval_tostring, undef_tostring
};

char* number_tostring(memcell* mem) {
    char* str = (char*)malloc(1024);
    sprintf(str, "%.3f", mem->data.num_zoumi);
    return str;
}

char* string_tostring(memcell* mem) { return mem->data.string_zoumi; }

char* bool_tostring(memcell* mem) {
    if(mem->data.bool_zoumi) { return "true"; }
    else { return "false"; }
}

char* table_tostring(memcell* mem) { /* j*b */ }

char* userfunc_tostring(memcell* mem){
    char* str = (char*)malloc(32);
    sprintf(str, "%u", mem->data.usrfunc_zoumi);
    return str;
}

char* libfunc_tostring(memcell* mem) {
    char* str = (char*)malloc(1024);
    sprintf(str, "%u", mem->data.libfunc_zoumi);
    return str;
}

char* nil_tostring(memcell* mem){ return "nil"; }

char* stackval_tostring(memcell* mem){
    char* str = (char*)malloc(1024);
    sprintf(str, "%u", mem->data.stackval_zoumi);
    return str;
}

char* undef_tostring(memcell* mem){ return "undef"; }

/* end of avm_executors.c */