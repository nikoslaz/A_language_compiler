/* avm_readbin.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef AVMREAD_H
#define AVMREAD_H

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
    ASSIGN_V,
    ADD_V, SUB_V, MUL_V, DIV_V, MOD_V,
    UMINUS_V,
    AND_V, OR_V, NOT_V,    
    JEQ_V, JNE_V, JLE_V, JGE_V, JLT_V, JGT_V,      
    CALL_V, PARAM_V, RETURN_V, GETRETVAL_V, FUNCSTART_V, FUNCEND_V,
    TABLECREATE_V, TABLEGETELEM_V, TABLESETELEM_V,
    JUMP_V,
    NOP_V
} vmopcode;

typedef struct instruction {
    vmopcode opcode;
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
extern unsigned int total_instruction;

extern unsigned int totalprogvar;

void read_binary(void);

#endif
/* end of avm_readbin.h */