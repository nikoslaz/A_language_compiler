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

void helper_relational(instruction* inst) {
    unsigned int lv = 0;
    memcell* rv1 = (memcell*)malloc(sizeof(memcell));
    if(!rv1) { MemoryFail(); }
    memcell* rv2 = (memcell*)malloc(sizeof(memcell));
    if(!rv2) { MemoryFail(); }
    rv1 = translate_operand(&inst->arg1, rv1);
    if (!rv1) { runtimeError("Null RV1"); }
    rv2 = translate_operand(&inst->arg2, rv2);
    if (!rv2) { runtimeError("Null RV2"); }
    if(rv1->type != MEM_NUMBER) { runtimeError("RV1 is not a NUMBER in relational"); }
    if(rv1->type != MEM_NUMBER) { runtimeError("RV2 is not a NUMBER in relational"); } 
    relational_func_t op = relat_funcs[inst->opcode - OP_JLE];
    lv = (*op)(rv1->data.num_zoumi, rv2->data.num_zoumi);
    if (lv) {
        succ_branch = 1;
        branch_label = inst->result.val;
    }
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
void execute_AND(instruction* inst)    { printf("AND SHOULD NOT EXIST\n"); }
void execute_OR(instruction* inst)     { printf("OR SHOULD NOT EXIST\n"); }
void execute_NOT(instruction* inst)    { printf("NOT SHOULD NOT EXIST\n"); }

void execute_JUMP(instruction* inst) {
    succ_branch = 1;
    branch_label = inst->result.val;
}

void execute_JEQ(instruction* inst) {
    unsigned int lv = 0;
    memcell* rv1 = (memcell*)malloc(sizeof(memcell));
    if(!rv1) { MemoryFail(); }
    memcell* rv2 = (memcell*)malloc(sizeof(memcell));
    if(!rv2) { MemoryFail(); }
    rv1 = translate_operand(&inst->arg1, rv1);
    if (!rv1) { runtimeError("Null RV1"); }
    rv2 = translate_operand(&inst->arg2, rv2);
    if (!rv2) { runtimeError("Null RV2"); }
    
    if(rv1->type == MEM_UNDEF || rv2->type == MEM_UNDEF) {
        runtimeError("Undef in equality");
    }
    else if(rv1->type == MEM_NIL || rv2->type == MEM_NIL) {
        lv = rv1->type == MEM_NIL && rv2->type == MEM_NIL;
    }
    else if(rv1->type == MEM_BOOL || rv2->type == MEM_BOOL) {
        lv = bool_tobool(rv1) == bool_tobool(rv2);
    }
    else if(rv1->type != rv2->type) {
        /* maybe better print ?*/
        runtimeError("Illegal equality");
    }
    else {
        switch(rv1->type){
			case MEM_NUMBER:
			{
				lv = rv1->data.num_zoumi==rv2->data.num_zoumi;
				break;
			}	
			case MEM_STRING:
			{
				if(!strcmp(rv1->data.string_zoumi,rv2->data.string_zoumi)){
					lv = 1;
				}
				break;
			}
			case MEM_TABLE:
			{
				if(rv1->data.table_zoumi==rv2->data.table_zoumi){
					lv=1;
				}
				break;
			}
			case MEM_USERFUNC:
			{
				if(!strcmp(userfunc_tostring(rv1),userfunc_tostring(rv2))){
					lv = 1;
				}
				break;
			}
			case MEM_LIBFUNC:
			{
			
				if(!strcmp(libfunc_tostring(rv1),libfunc_tostring(rv2))){
					lv = 1;
				}
				break;
			}
			default: printf("Default in EQ\n");
		}
    }

    if(lv) {
        succ_branch = 1;
        branch_label = inst->result.val;
    }
}

void execute_JNE(instruction* inst) {
    unsigned int lv = 0;
    memcell* rv1 = (memcell*)malloc(sizeof(memcell));
    if(!rv1) { MemoryFail(); }
    memcell* rv2 = (memcell*)malloc(sizeof(memcell));
    if(!rv2) { MemoryFail(); }
    rv1 = translate_operand(&inst->arg1, rv1);
    if (!rv1) { runtimeError("Null RV1"); }
    rv2 = translate_operand(&inst->arg2, rv2);
    if (!rv2) { runtimeError("Null RV2"); }
    
    if(rv1->type == MEM_UNDEF || rv2->type == MEM_UNDEF) {
        runtimeError("Undef in no equality");
    }
    else if(rv1->type == MEM_NIL || rv2->type == MEM_NIL) {
        lv = rv1->type == MEM_NIL != rv2->type == MEM_NIL;
    }
    else if(rv1->type == MEM_BOOL || rv2->type == MEM_BOOL) {
        lv = bool_tobool(rv1) != bool_tobool(rv2);
    }
    else if(rv1->type != rv2->type) {
        /* maybe better print ?*/
        runtimeError("Illegal no equality");
    }
    else {
        switch(rv1->type){
			case MEM_NUMBER:
			{
				lv = rv1->data.num_zoumi!=rv2->data.num_zoumi;
				break;
			}	
			case MEM_STRING:
			{
				if(strcmp(rv1->data.string_zoumi,rv2->data.string_zoumi)){
					lv = 1;
				}
				break;
			}
			case MEM_TABLE:
			{
				if(rv1->data.table_zoumi!=rv2->data.table_zoumi){
					lv=1;
				}
				break;
			}
			case MEM_USERFUNC:
			{
				if(strcmp(userfunc_tostring(rv1),userfunc_tostring(rv2))){
					lv = 1;
				}
				break;
			}
			case MEM_LIBFUNC:
			{
			
				if(strcmp(libfunc_tostring(rv1),libfunc_tostring(rv2))){
					lv = 1;
				}
				break;
			}
			default: printf("Default in NEQ\n");
		}
    }

    if(lv) {
        succ_branch = 1;
        branch_label = inst->result.val;
    }
}

void execute_JLE(instruction* inst) { helper_relational(inst); }
void execute_JGE(instruction* inst) { helper_relational(inst); }
void execute_JLT(instruction* inst) { helper_relational(inst); }
void execute_JGT(instruction* inst) { helper_relational(inst); }

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
    clear_memcell(&stack[0]);
    stack[0].type = MEM_NIL;
    fprintf(avm_log, "Pushing current_args_pushed\n");
    push(totalargs);
    switch(func->type) {
        /* do j*b */
        case MEM_LIBFUNC:
            fprintf(avm_log, "Calling libfunc %d\n", func->data.libfunc_zoumi);
            (*libFuncs[func->data.libfunc_zoumi])();
            /* pop arguments */
            int totals = pop().data.stackval_zoumi;
            fprintf(avm_log, "Popping %d params\n", totals);
            for(int i=0; i<totals; i++) { pop(); }
            break;
        case MEM_USERFUNC:
            totalargs.data.stackval_zoumi = program_counter + 1;
            fprintf(avm_log, "Pushing Return Address %d\n", program_counter+2);
            push(totalargs);
            fprintf(avm_log, "Branching to userfunc at %d\n", func->data.usrfunc_zoumi+1);
            succ_branch = 1;
            branch_label = func->data.usrfunc_zoumi;
            break;
        default: runtimeError("Argument is not a function");
    }
}

void execute_PARAM(instruction* inst) {
    memcell* res = (memcell*)malloc(sizeof(memcell));
    if(!res) { MemoryFail(); }
    res = translate_operand(&inst->arg1, res);
    if(!res) { runtimeError("Null memcell in param"); }
    push(*res);
    current_args_pushed++;
}

void execute_FUNCSTART(instruction* inst) {
    memcell oldmaul;
    oldmaul.type = AVM_STACKSIZE;
    oldmaul.data.stackval_zoumi = stack_maul;
    fprintf(avm_log, "Pushing old DARTH MAUL\n");
    push(oldmaul);
    stack_maul = stack_top+1;
    int locals = inst->arg2.val;
    memcell tmp_local;
    clear_memcell(&tmp_local);
    fprintf(avm_log, "Pushing %d locals\n", locals);
    for(int i=0; i<locals; i++) { push(tmp_local); }
    fprintf(avm_log, "Begin Function Execution\n");
}

void execute_FUNCEND(instruction* inst) {
    int locals = stack[stack_maul-3].data.stackval_zoumi;
    fprintf(avm_log, "Popping %d locals\n", locals);
    for(int i=0; i<locals; i++) { pop(); }
    fprintf(avm_log, "Restore old DARTH MAUL\n");
    stack_maul = pop().data.stackval_zoumi;
    fprintf(avm_log, "Pop return address\n");
    int retaddr = pop().data.stackval_zoumi;
    fprintf(avm_log, "Pop total params\n");
    int params = pop().data.stackval_zoumi;
    fprintf(avm_log, "Popping %d params\n", params);
    for(int i=0; i<params; i++) { pop(); }
    fprintf(avm_log, "Branch to return address %d\n", retaddr+1);
    succ_branch = 1;
    branch_label = retaddr;
}

void execute_NEWTABLE(instruction* inst) {

}

void execute_TABLEGETELEM(instruction* inst) {

}

void execute_TABLESETELEM(instruction* inst) {

}

void execute_NOP(instruction* inst) {

}

void execute_RETURN(instruction* inst) {

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
    char* str = (char*)malloc(16);
    sprintf(str, "%u", mem->data.libfunc_zoumi);
    return str;
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