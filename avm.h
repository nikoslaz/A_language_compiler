/* avm.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef AVM_H
#define AVM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAGIC_NUMBER 0xDEADBEEF

typedef enum VmargType {
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

typedef enum vmopcode {
    OP_ASSIGN,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_UMINUS,
    OP_AND, OP_OR, OP_NOT,    
    OP_JEQ, OP_JNE, OP_JLE, OP_JGE, OP_JLT, OP_JGT,      
    OP_CALL, OP_PARAM, OP_RETURN, OP_GETRETVAL, OP_FUNCSTART, OP_FUNCEND,
    OP_TABLECREATE, OP_TABLEGETELEM, OP_TABLESETELEM,
    OP_JUMP,
    OP_NOP
} vmopcode;

typedef struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned int srcLine;
} instruction;

typedef enum avm_memcell{
	MEM_NUMBER,
	MEM_STRING,	
	MEM_BOOL,
	MEM_TABLE,
	MEM_USERFUN,
	MEM_LIBFUN,
	MEM_NIL,
	MEM_UNDEF,
	MEM_INVALID
} avm_memcell_t;

typedef struct AVM_memcell {
	avm_memcell_t type;

	union{
		double num_zoumi;
		char* string_zoumi;
		unsigned char bool_zoumi;
		struct AVM_table* table_zoumi;
		unsigned func_zoumi;
		char* libfunc_zoumi;
	} data;

} avm_memcell;

/*===============================================================================================*/
/* Executor */

typedef void (*execute_func_t)(instruction *);
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

typedef char *(*tostring_func_t)(avm_memcell *);
extern tostring_func_t to_string_func[];

extern char* number_tostring(avm_memcell*);
extern char* string_tostring(avm_memcell*);
extern char* bool_tostring(avm_memcell*);
extern char* table_tostring(avm_memcell*);
extern char* userfunc_tostring(avm_memcell*);
extern char* libfunc_tostring(avm_memcell*);
extern char* nil_tostring(avm_memcell*);
extern char* undef_tostring(avm_memcell*);
extern char* invalid_tostring(avm_memcell*);

/*===============================================================================================*/
/* ToBoolFunc */

typedef unsigned char (*tobool_func_t)(avm_memcell *);
extern tobool_func_t to_bool_func[];

extern unsigned char number_tobool(avm_memcell*);
extern unsigned char string_tobool(avm_memcell*);
extern unsigned char bool_tobool(avm_memcell*);
extern unsigned char table_tobool(avm_memcell*);
extern unsigned char userfunc_tobool(avm_memcell*);
extern unsigned char libfunc_tobool(avm_memcell*);
extern unsigned char nil_tobool(avm_memcell*);
extern unsigned char undef_tobool(avm_memcell*);
extern unsigned char invalid_tobool(avm_memcell*);

/*===============================================================================================*/
/* Arithmetic */

typedef double (*arithmetic_func_t) (double, double);
extern arithmetic_func_t arith_func[];

extern double add_arith(double, double);
extern double sub_arith(double, double);
extern double mul_arith(double, double);
extern double div_arith(double, double);
extern double mod_arith(double, double);
extern double uminus_arith(double, double);

/*===============================================================================================*/
/* Relational */

typedef unsigned char (*relational_func_t) (double, double);
extern relational_func_t relat_func;

extern unsigned char jgt_rel(double, double);
extern unsigned char jle_rel(double, double);
extern unsigned char jge_rel(double, double);
extern unsigned char jlt_rel(double, double);

#endif
/* end of avm.h */
