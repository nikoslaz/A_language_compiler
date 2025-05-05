/* alpha_quads.h */
/**
 * @authors nikos , nikoletta , mihalis
 */
#ifndef ALPHAQUADS_H
#define ALPHAQUADS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "table.h"

#define EXPAND_SIZE 1024
#define CURR_SIZE (totalquads*sizeof(quad))
#define NEW_SIZE (CURR_SIZE+EXPAND_SIZE*sizeof(quad))
#define MAX_TEMPS 9999999999

typedef enum iopcode_e {
    OP_ASSIGN,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_UMINUS,
    OP_AND, OP_OR, OP_NOT,
    OP_IFEQ, OP_IFNOTEQ, OP_IFLESSEQ, OP_IFGREATEREQ, OP_IFLESS, OP_IFGREATER,
    OP_CALL, OP_PARAM, OP_RET, OP_GETRETVAL, OP_FUNCSTART, OP_FUNCEND,
    OP_TABLECREATE, OP_TABLEGETELEM, OP_TABLESETELEM
} opcode;

typedef enum expr_type_e {
    EXP_VARIABLE,
    EXP_PROGRAMFUNC, EXP_LIBRARYFUNC,
    EXP_ARITH, EXP_BOOL, EXP_ASSIGN,
    EXP_NEWTABLE, EXP_TABLEITEM,
    EXP_CONSTNUMBER, EXP_CONSTSTRING, EXP_CONSTBOOL,
    EXP_NIL,
} expr_type;

typedef struct expr_s {
    expr_type type;
    Symbol* symbol;
    struct expr_s* index;   /* for tables */
    double numConst;
    char* stringConst;
    unsigned int boolConst;
    struct expr_s* next;    /* For lists */
} expr;

typedef struct quad_s {
    opcode op;
    expr* result;
    expr* arg1;
    expr* arg2;
    unsigned int label;
    unsigned int line;
} quad;

extern quad* quads;
extern unsigned int totalquads;
extern unsigned int currquad;
extern unsigned int temp_counter;

Symbol* create_temp_symbol(void);
quad* emit(opcode op, expr* result, expr* arg1, expr* arg2, unsigned int label);
void printQuads(void);

#endif
/* end of alpha_quads.h */
