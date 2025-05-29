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

// typedef struct incomplete_jump {
//     unsigned instrNo;     
//     unsigned iaddress;    
//     struct incomplete_jump* next; 
// } incomplete_jump;

// extern incomplete_jump* ij_head;
// extern unsigned ij_total;

// void add_incomplete_jump(unsigned instrNo, unsigned iaddress);
// void patch_incomplete_jumps(void);

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
void printTargetToFile();

#endif
/* end of list.h */
