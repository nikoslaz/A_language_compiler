/* target.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef TARGET_H
#define TARGET_H

#include "quads.h"

#define CURR_STR_SIZE (total_str_const*sizeof(char*))
#define CURR_NUM_SIZE (total_str_const*sizeof(double))
#define CURR_LIBFUNC_SIZE (total_str_const*sizeof(char*))
#define CURR_INSTRUCTION_SIZE (total_instruction*sizeof(instruction*))

#define MAGIC_NUMBER 0xDEADBEEF
extern unsigned magic_number;

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
    OPC_ASSIGN,
    OPC_ADD, OPC_SUB, OPC_MUL, OPC_DIV, OPC_MOD,
    OPC_UMINUS,
    OPC_AND, OPC_OR, OPC_NOT,    
    OPC_JEQ, OPC_JNE, OPC_JLE, OPC_JGE, OPC_JLT, OPC_JGT,      
    OPC_CALL, OPC_PARAM, OPC_RETURN, OPC_GETRETVAL, OPC_FUNCSTART, OPC_FUNCEND,
    OPC_TABLECREATE, OPC_TABLEGETELEM, OPC_TABLESETELEM,
    OPC_JUMP,
    OPC_NOP
} vmopcode;

typedef struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned int srcLine;
} instruction;

/*===============================================================================================*/
/* Globals */

extern char** string_const;
extern unsigned int total_str_const;
extern unsigned int curr_str_const;
extern double* number_const;
extern unsigned int total_num_const;
extern unsigned int curr_num_const;
extern char** libfunc_const;
extern unsigned int total_libfunc_const;
extern unsigned int curr_libfunc_const;
extern instruction* instructions;
extern unsigned int total_instruction;
extern unsigned int curr_instruction;

/*===============================================================================================*/
/* Generators */

typedef void (*generator_func_t)(quad*);
extern generator_func_t generators[];

unsigned int consts_newstring(char* s);
unsigned int consts_newnumber(double n);
unsigned int consts_newlibfunc(char* s);
void make_operand(expr* e, vmarg* arg);

void generateTarget(void);

extern void generate_ASSIGN(quad*);
extern void generate_ADD(quad*); 
extern void generate_SUB(quad*); 
extern void generate_MUL(quad*);
extern void generate_DIV(quad*); 
extern void generate_MOD(quad*);
extern void generate_UMINUS(quad*);
extern void generate_AND(quad*);
extern void generate_OR(quad*);
extern void generate_NOT(quad*);
extern void generate_IF_EQ(quad*);
extern void generate_IF_NOTEQ(quad*);
extern void generate_IF_LESSEQ(quad*);
extern void generate_IF_GREATEREQ(quad*);
extern void generate_IF_LESS(quad*);
extern void generate_IF_GREATER(quad*);
extern void generate_CALL(quad*);
extern void generate_PARAM(quad*);
extern void generate_RETURN(quad*);
extern void generate_GETRETVAL(quad*);
extern void generate_FUNCSTART(quad*);
extern void generate_FUNCEND(quad*);
extern void generate_NEWTABLE(quad*);
extern void generate_TABLEGETELEM(quad*);
extern void generate_TABLESETELEM(quad*);
extern void generate_JUMP(quad*);
extern void generate_NOP(quad*);

void helper_generate_full(vmopcode op, quad* q);
void helper_generate_relational(vmopcode op, quad* q);
void helper_generate_arg1(vmopcode op, quad* q);
void helper_generate_res(vmopcode op, quad* q);
void printTargetToFile(void);
void write_binary(void);

#endif
/* end of target.h */
