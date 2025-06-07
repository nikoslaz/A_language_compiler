/* avm_executors.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

void helper_assign(memcell* lv, memcell* rv) {
    if(lv == rv) { return; }
    /* check for tables */
    if(rv->type == MEM_UNDEF) { printf("Warning. Assignment of undef\n"); }
    memcpy(lv, rv, sizeof(memcell));
    /* again tables */
}

void execute_ASSIGN(instruction* inst) {
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE in assign"); }
    memcell* rv;
    rv = translate_operand(&inst->arg1, rv);
    if(!rv) { runtimeError("Null RVALUE in assign"); }
    helper_assign(lv, rv);
}

void execute_ADD(instruction* inst) {
    memcell* lv = translate_operand(&inst->result, NULL);
    if(!lv) { runtimeError("Null LVALUE in ADD"); }
    memcell* arg1;
    arg1 = translate_operand(&inst->arg1, arg1);
    if(!arg1) { runtimeError("Null ARG1 in ADD"); }
    memcell* arg2;
    arg2 = translate_operand(&inst->arg2, arg2);
    if(!arg2) { runtimeError("Null ARG2 in ADD"); }
    if(arg1->type != MEM_NUMBER) { runtimeError("ARG1 is not a NUMBER"); }
    if(arg2->type != MEM_NUMBER) { runtimeError("ARG2 is not a NUMBER"); }
    clear_memcell(lv);
    lv->type = MEM_NUMBER;
    lv->data.num_zoumi = (*arith_funcs[inst->opcode-OP_ADD])(arg1->data.num_zoumi, arg2->data.num_zoumi);
}

void execute_SUB(instruction* inst) {

}

void execute_MUL(instruction* inst) {

}

void execute_DIV(instruction* inst) {

}

void execute_MOD(instruction* inst) {

}

void execute_UMINUS(instruction* inst) {

}

void execute_AND(instruction* inst) {

}

void execute_OR(instruction* inst) {

}

void execute_NOT(instruction* inst) {

}

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
    memcell* func;
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
    }
    /* kane kati */
}

void execute_PUSHARG(instruction* inst) {
    memcell* res;
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
    sprintf(str, "%.3f", mem->data.usrfunc_zoumi);
    return str;
}

char* libfunc_tostring(memcell* mem){
    return mem->data.libfunc_zoumi;
}

char* nil_tostring(memcell* mem){
    return "nil";
}

char* stackval_tostring(memcell* mem){
    char* str = (char*)malloc(16);
    sprintf(str, "%.3f", mem->data.stackval_zoumi);
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

unsigned int number_tobool(memcell* mem) {
    return mem->data.stackval_zoumi != 0;
}

unsigned int string_tobool(memcell* mem) {
    return mem->data.string_zoumi[0] != 0;
}

unsigned int bool_tobool(memcell* mem) {
    return mem->data.bool_zoumi;
}

unsigned int table_tobool(memcell* mem) {
    return 1;
}

unsigned int userfunc_tobool(memcell* mem) {
    return 1;
}

unsigned int libfunc_tobool(memcell* mem) {
    return 1;
}

unsigned int nil_tobool(memcell* mem) {
    return 0;
}

unsigned int stackval_tobool(memcell* mem) {
    return 1;
}

unsigned int undef_tobool(memcell* mem) {
    return 0;
}

/*===============================================================================================*/
/* Relational */

unsigned int jgt_rel(double x, double y) { return x > y; }
unsigned int jle_rel(double x, double y) { return x <= y; }
unsigned int jge_rel(double x, double y) { return x >= y; }
unsigned int jlt_rel(double x, double y) { return x < y; }

/*===============================================================================================*/

/* end of avm_executors.c */