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

/* end of avm_executors.c */