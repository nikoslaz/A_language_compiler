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
#define COL_WIDTH_OPCODE 12
#define COL_WIDTH_ARG    20
#define COL_WIDTH_LINE   5

typedef enum VmargType {
    GLOBAL_V,
    LOCAL_V,
    FORMAL_V,
    USERFUNC_V,
    LIBFUNC_V,
    TEMPORARY_V,
    BOOL_V,
    STRING_V,
    NUMBER_V,
    LABEL_V,
    NUMLOCALS_V,
    NIL_V,
    UNDEFINED_V
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

/* Globals */
extern char** string_const;
extern unsigned int total_str_const;
extern double* number_const;
extern unsigned int total_num_const;
extern char** libfunc_const;
extern unsigned int total_libfunc_const;
extern instruction* instructions;
extern unsigned int total_instructions;
extern int totalprogvar;

#endif
/* end of avm.h */
