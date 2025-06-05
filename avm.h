/* avm.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef AVM_H
#define AVM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#endif
/* end of avm.h */
