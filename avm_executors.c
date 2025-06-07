/* avm_executors.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

void helper_assign(memcell* lv, memcell* rv) {
    if(lv == rv) { return; }
    /* check for tables */
    if(rv->type == MEM_UNDEF) { printf("Warning. Assignment of undef\n"); }
    clear_memcell(lv);
    memcpy(lv, rv, sizeof(memcell));
    /* again tables */
}

void helper_arith(instruction* inst) {
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE"); }
    memcell* arg1 = (memcell*)malloc(sizeof(memcell));
    if(!arg1) { MemoryFail(); }
    arg1 = translate_operand(&inst->arg1, arg1);
    if(!arg1) { runtimeError("Null ARG1"); }
    memcell* arg2 = (memcell*)malloc(sizeof(memcell));
    if(!arg2) { MemoryFail(); }
    arg2 = translate_operand(&inst->arg2, arg2);
    if(!arg2) { runtimeError("Null ARG2"); }
    if(arg1->type != MEM_NUMBER) { runtimeError("ARG1 is not a NUMBER"); }
    if(arg2->type != MEM_NUMBER) { runtimeError("ARG2 is not a NUMBER"); }
    memcell res;
    res.type = MEM_NUMBER;
    res.data.num_zoumi = (*arith_funcs[inst->opcode-OP_ADD])(arg1->data.num_zoumi, arg2->data.num_zoumi);
    helper_assign(lv, &res);
}

void execute_ASSIGN(instruction* inst) {
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE in assign"); }
    memcell* rv = (memcell*)malloc(sizeof(memcell));
    if(!rv) { MemoryFail(); }
    rv = translate_operand(&inst->arg1, rv);
    if(!rv) { runtimeError("Null RVALUE in assign"); }
    helper_assign(lv, rv);
}

void execute_ADD(instruction* inst) { helper_arith(inst); }
void execute_SUB(instruction* inst) { helper_arith(inst); }
void execute_MUL(instruction* inst) { helper_arith(inst); }
void execute_DIV(instruction* inst) { helper_arith(inst); }
void execute_MOD(instruction* inst) { helper_arith(inst); }

void execute_UMINUS(instruction* inst) { printf("UMINUS SHOULD NOT EXIST\n"); }
void execute_AND(instruction* inst) { printf("AND SHOULD NOT EXIST\n"); }
void execute_OR(instruction* inst) { printf("OR SHOULD NOT EXIST\n"); }
void execute_NOT(instruction* inst) { printf("NOT SHOULD NOT EXIST\n"); }

void execute_JUMP(instruction* inst) {

}

void execute_JEQ(instruction* inst) {

}

void execute_JNE(instruction* inst) {

}

void execute_JLE(instruction* inst) {

}

void execute_JGE(instruction* inst) {

}

void execute_JLT(instruction* inst) {

}

void execute_JGT(instruction* inst) {

}

void execute_CALL(instruction* inst) {
    memcell* func = (memcell*)malloc(sizeof(memcell));
    if(!func) { MemoryFail(); }
    func = translate_operand(&inst->arg1, func);
    if(!func) { runtimeError("Null memcell in call"); }
    /* Push total args */
    memcell totalargs;
    totalargs.type = MEM_STACKVAL;
    totalargs.data.stackval_zoumi = current_args_pushed;
    current_args_pushed=0;
    push(totalargs);
    switch(func->type) {
        /* do j*b */
        case MEM_LIBFUNC:
            printf("Calling libfunc %d\n", func->data.libfunc_zoumi);
            (*libFuncs[func->data.libfunc_zoumi])();
            break;
        default: printf("Hell naw bro\n");
    }
    /* kane kati */
}

void execute_PUSHARG(instruction* inst) {
    memcell* res = (memcell*)malloc(sizeof(memcell));
    if(!res) { MemoryFail(); }
    res = translate_operand(&inst->arg1, res);
    if(!res) { runtimeError("Null memcell in param"); }
    push(*res);
    current_args_pushed++;
}

void execute_FUNCENTER(instruction* inst) {

}

void execute_FUNCEXIT(instruction* inst) {

}

void execute_NEWTABLE(instruction* inst) {

}

void execute_TABLEGETELEM(instruction* inst) {

}

void execute_TABLESETELEM(instruction* inst) {

}

void execute_NOP(instruction* inst) {

}

void execute_RET(instruction* inst) {

}

void execute_GETRETVAL(instruction* inst) {
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null memcell in getretval"); }
    helper_assign(lv, &stack[0]);
}

/*===============================================================================================*/
/* ToStringFunc */

char* number_tostring(memcell* mem){
    char* str = (char*)malloc(16);
    sprintf(str, "%.3f", mem->data.num_zoumi);
    return str;
}

char* string_tostring(memcell* mem){
    return mem->data.string_zoumi;
}

char* bool_tostring(memcell* mem){
    if (mem->data.bool_zoumi) {
        return "true";
    }
    else {
        return "false";
    }
}

char* table_tostring(memcell* mem){
    /* j*b */
}

char* userfunc_tostring(memcell* mem){
    char* str = (char*)malloc(32);
    sprintf(str, "%u", mem->data.usrfunc_zoumi);
    return str;
}

char* libfunc_tostring(memcell* mem){
    /* make it a string bro */
    //return mem->data.libfunc_zoumi;
    return NULL;
}

char* nil_tostring(memcell* mem){
    return "nil";
}

char* stackval_tostring(memcell* mem){
    char* str = (char*)malloc(16);
    sprintf(str, "%u", mem->data.stackval_zoumi);
    return str;
}

char* undef_tostring(memcell* mem){
    return "undef";
}

/*===============================================================================================*/
/* Arith */

double add_arith(double x, double y) { return (double)(x+y); }
double sub_arith(double x, double y) { return (double)(x-y); }
double mul_arith(double x, double y) { return (double)(x*y); }
double div_arith(double x, double y) { return (double)(x/y); }
double mod_arith(double x, double y) { return (double)(((unsigned)x)%((unsigned)y)); }
double uminus_arith(double x, double y) { runtimeError("UMINUS SHOULD NOT EXIST"); return 0; }


/*===============================================================================================*/
/* To Bool */

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
/* Relational */

unsigned int jgt_rel(double x, double y) { return x > y; }
unsigned int jle_rel(double x, double y) { return x <= y; }
unsigned int jge_rel(double x, double y) { return x >= y; }
unsigned int jlt_rel(double x, double y) { return x < y; }

/*===============================================================================================*/

/* end of avm_executors.c */