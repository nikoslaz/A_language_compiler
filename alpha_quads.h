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
#define MAX_TEMPS 100

typedef enum iopcode_e {
    OP_ASSIGN,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_UMINUS,
    OP_AND, OP_OR, OP_NOT,
    OP_IFEQ, OP_IFNOTEQ, OP_IFLESSEQ, OP_IFGREATEREQ, OP_IFLESS, OP_IFGREATER,
    OP_CALL, OP_PARAM, OP_RETURN, OP_GETRETVAL, OP_FUNCSTART, OP_FUNCEND,
    OP_TABLECREATE, OP_TABLEGETELEM, OP_TABLESETELEM,
    OP_JUMP
} opcode;

typedef enum expr_type_e {
    EXP_VARIABLE,
    EXP_PROGRAMFUNC, EXP_LIBRARYFUNC,
    EXP_ARITH, EXP_BOOL, EXP_ASSIGN,
    EXP_NEWTABLE, EXP_TABLEITEM,
    EXP_CONSTNUMBER, EXP_CONSTSTRING, EXP_CONSTBOOL,
    EXP_NIL,
} expr_type;

typedef struct PatchList {
    unsigned int quad_index;
    struct PatchList* next;
} PatchList;

/* STACK FOR RETURN LIST */
typedef struct ReturnContext {
    PatchList* return_list;
    struct ReturnContext* next;
} ReturnContext;

/* STACK FOR BREAK/CONTINUE LISTS */
typedef struct LoopContext {
    PatchList* break_list;
    PatchList* continue_list;
    struct LoopContext* next;
} LoopContext;

/* Expression Struct */
typedef struct expr_s {
    expr_type type;
    Symbol* symbol;
    struct expr_s* index;   /* for tables */
    double numConst;
    char* stringConst;
    unsigned int boolConst;
    struct expr_s* next;    /* for lists */
    /* --- Backpatching --- */
    PatchList* truelist;
    PatchList* falselist;
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
extern LoopContext* loop_stack;
extern ReturnContext* return_stack;
extern unsigned int totalquads;
extern unsigned int currquad;
extern unsigned int from_method;

/* Emits */
quad* emit(opcode op, expr* result, expr* arg1, expr* arg2, unsigned int label);
expr* emit_if_table_item_get(expr* e, expr* result);
expr* emit_if_table_item_set(expr* table, expr* arg2);

/* Temporary Symbols */
Symbol* get_temp_symbol(void);
unsigned int hasTempSymbol(expr* ex);
void freeIfTemp(expr* ex);
void reset_temp_array(void);
Symbol* insert_Global_Temp_Symbol(void);

/* Expression Constructors */
expr* create_arith_expr(Symbol* sym);
expr* create_bool_expr(Symbol* sym);
expr* create_empty_bool_expr(void);
expr* create_var_expr(Symbol* symbol);
expr* create_prog_func_expr(Symbol* symbol);
expr* create_constnum_expr(double value);
expr* create_conststring_expr(char* value);
expr* create_constbool_expr(unsigned int value);
expr* create_nil_expr(void);
expr* create_table_expr(Symbol* sym);

/* Backpatch Functions */
unsigned int nextquad(void);
PatchList* makelist(unsigned int quad_index);
PatchList* merge(PatchList* list1, PatchList* list2);
void backpatch(PatchList* list, unsigned int target_quad_index);
void simplepatch(unsigned int quad, unsigned int index);

/* Elist Functions */
void handle_arguments(expr* arg);
void push_return(void);
void pop_return(void);
void add_to_return_list(unsigned int quad_to_patch);

/* Stack Functions */
void push_loop(void);
void pop_loop(void);
void add_to_breakList(unsigned int quad_to_patch);
void add_to_continueList(unsigned int quad_to_patch);

/* Final Print */
void printQuads(void);

#endif
/* end of alpha_quads.h */
