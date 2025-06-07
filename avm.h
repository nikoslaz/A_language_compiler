/* avm.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef AVM_H
#define AVM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avm_prints.h"

#define MAGIC_NUMBER 0xDEADBEEF
#define	AVM_STACKSIZE 8192 /* 2^13 */

typedef enum vmarg_t {
    ARG_GLOBAL,
    ARG_LOCAL,
    ARG_FORMAL,
    ARG_USERFUNC,
    ARG_LIBFUNC,
    ARG_TEMPORARY,
    ARG_BOOL,
    ARG_STRING,
    ARG_NUMBER,
    ARG_LABEL,
    ARG_NUMLOCALS,
    ARG_NIL,
    ARG_UNDEFINED
} vmarg_t;

typedef struct vmarg {
    vmarg_t type;
    unsigned val;
} vmarg;

typedef enum vmopcode_t {
    OP_ASSIGN,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_UMINUS,
    OP_AND, OP_OR, OP_NOT,    
    OP_JEQ, OP_JNE, OP_JLE, OP_JGE, OP_JLT, OP_JGT,      
    OP_CALL, OP_PARAM, OP_RETURN, OP_GETRETVAL, OP_FUNCSTART, OP_FUNCEND,
    OP_TABLECREATE, OP_TABLEGETELEM, OP_TABLESETELEM,
    OP_JUMP,
    OP_NOP
} vmopcode_t;

typedef struct instruction {
    vmopcode_t opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned int srcLine;
} instruction;

extern char** string_const;
extern unsigned int total_str_const;
extern double* number_const;
extern unsigned int total_num_const;
extern char** libfunc_const;
extern unsigned int total_libfunc_const;
extern instruction* instructions;
extern unsigned int total_instructions;
extern int totalprogvar;

/*===============================================================================================*/
/* AVM structs */

typedef enum memcell_type_t {
	MEM_NUMBER,
	MEM_STRING,	
	MEM_BOOL,
	MEM_TABLE,
	MEM_USERFUNC,
	MEM_LIBFUNC,
	MEM_NIL,
    MEM_STACKVAL,
	MEM_UNDEF
} memcell_type_t;

typedef struct memcell {
	memcell_type_t type;
	union{
		double num_zoumi;
		char* string_zoumi;
		unsigned char bool_zoumi;
		unsigned usrfunc_zoumi;
		unsigned libfunc_zoumi;
		struct AVM_table* table_zoumi;
        unsigned stackval_zoumi;
	} data;
} memcell;

/*===============================================================================================*/
/* Executor */

typedef void (*execute_func_t)(instruction*);
extern execute_func_t executors[];

extern void execute_ASSIGN(instruction*);
extern void execute_ADD(instruction*);
extern void execute_SUB(instruction*);
extern void execute_MUL(instruction*);
extern void execute_DIV(instruction*);
extern void execute_MOD(instruction*);
extern void execute_UMINUS(instruction*);
extern void execute_AND(instruction*);
extern void execute_OR(instruction*);
extern void execute_NOT(instruction*);
extern void execute_JUMP(instruction*);
extern void execute_JEQ(instruction*);
extern void execute_JNE(instruction*);
extern void execute_JLE(instruction*);
extern void execute_JGE(instruction*);
extern void execute_JLT(instruction*);
extern void execute_JGT(instruction*);
extern void execute_CALL(instruction*);
extern void execute_PUSHARG(instruction*);
extern void execute_FUNCENTER(instruction*);
extern void execute_FUNCEXIT(instruction*);
extern void execute_NEWTABLE(instruction*);
extern void execute_TABLEGETELEM(instruction*);
extern void execute_TABLESETELEM(instruction*);
extern void execute_NOP(instruction*);
extern void execute_RET(instruction*);
extern void execute_GETRETVAL(instruction*);

/*===============================================================================================*/
/* ToStringFunc */

typedef char* (*tostring_func_t)(memcell*);
extern tostring_func_t to_string_funcs[];

extern char* number_tostring(memcell*);
extern char* string_tostring(memcell*);
extern char* bool_tostring(memcell*);
extern char* table_tostring(memcell*);
extern char* userfunc_tostring(memcell*);
extern char* libfunc_tostring(memcell*);
extern char* nil_tostring(memcell*);
extern char* stackval_tostring(memcell*);
extern char* undef_tostring(memcell*);

/*===============================================================================================*/
/* ToBoolFunc */

typedef unsigned int (*tobool_func_t)(memcell*);
extern tobool_func_t to_bool_funcs[];

extern unsigned int number_tobool(memcell*);
extern unsigned int string_tobool(memcell*);
extern unsigned int bool_tobool(memcell*);
extern unsigned int table_tobool(memcell*);
extern unsigned int userfunc_tobool(memcell*);
extern unsigned int libfunc_tobool(memcell*);
extern unsigned int nil_tobool(memcell*);
extern unsigned int stackval_tobool(memcell*);
extern unsigned int undef_tobool(memcell*);

/*===============================================================================================*/
/* Arithmetic */

typedef double (*arithmetic_func_t)(double, double);
extern arithmetic_func_t arith_funcs[];

extern double add_arith(double, double);
extern double sub_arith(double, double);
extern double mul_arith(double, double);
extern double div_arith(double, double);
extern double mod_arith(double, double);
extern double uminus_arith(double, double);

/*===============================================================================================*/
/* Relational */

typedef unsigned int (*relational_func_t)(double, double);
extern relational_func_t relat_funcs[];

extern unsigned int jgt_rel(double, double);
extern unsigned int jle_rel(double, double);
extern unsigned int jge_rel(double, double);
extern unsigned int jlt_rel(double, double);

typedef void (*library_func_t)(void);
extern library_func_t libFuncs[];

extern void libfunc_print();
extern void libfunc_input();
extern void libfunc_objectmemberkeys();
extern void libfunc_objecttotalmembers();
extern void libfunc_objectcopy();
extern void libfunc_totalarguments();
extern void libfunc_argument();
extern void libfunc_typeof();
extern void libfunc_strtonum();
extern void libfunc_sqrt();
extern void libfunc_cos();
extern void libfunc_sin();

void clear_memcell(memcell* cell);
void push(memcell val);
memcell pop(void);
memcell* translate_operand(vmarg* arg, memcell* reg);
void stackError(char* input);
void runtimeError(char* input);
void MemoryFail(void);
extern unsigned int succ_branch;
extern unsigned int branch_label;
extern unsigned int current_args_pushed;
extern unsigned int execution_finished;
extern memcell stack[AVM_STACKSIZE];
extern unsigned int stack_top;
extern unsigned int stack_maul;

#endif
/* end of avm.h */
