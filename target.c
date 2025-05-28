#include "target.h"
#include "quads.h"

/* Globals */
char** string_const=(char**)0;
unsigned int total_str_const=0;
unsigned int curr_str_const=0;

double* number_const=(double*)0;
unsigned int total_num_const=0;
unsigned int curr_num_const=0;

char** libfunc_const=(char**)0;
unsigned int total_libfunc_const=0;
unsigned int curr_libfunc_const=0;

instruction* instructions=(instruction*)0;
unsigned int total_instruction=0;
unsigned int curr_instruction=0;

/*===============================================================================================*/
/* CONSTANTS */

unsigned consts_newstring(char* s) {
    if(curr_str_const == total_str_const) { 
        total_str_const += EXPAND_SIZE;
        string_const = realloc(string_const, total_str_const*sizeof(char*));
        if(!string_const) { MemoryFail(); }
    }
    for(int i = 0; i < curr_str_const; i++) {
        if(!strcmp(s, string_const[i])) { return i; }
    }
    char** new_string = string_const + curr_str_const++;
    *new_string = s;
    return curr_str_const - 1;
}

unsigned consts_newnumber(double n) {
    if(curr_num_const == total_num_const) {
        total_num_const += EXPAND_SIZE;
        number_const = realloc(number_const, total_str_const*sizeof(double));
        if(!number_const) { MemoryFail(); }
    }
    for(int i = 0; i < curr_num_const; i++) {
        if (number_const[i] == n) { return i; }
    }
    double* new_number = number_const + curr_num_const++;
    *new_number = n;
    return curr_num_const - 1;
}

unsigned consts_newlibfunc(char* s) {
    if(curr_libfunc_const == total_libfunc_const) {
        total_libfunc_const += EXPAND_SIZE;
        libfunc_const = realloc(libfunc_const, total_str_const*sizeof(char*));
        if(!libfunc_const) { MemoryFail(); }
    }
    for(int i = 0; i < curr_libfunc_const; i++) {
        if(!strcmp(s, libfunc_const[i])) { return i; }
    }
    char** new_libfunc = libfunc_const + curr_libfunc_const++;
    *new_libfunc = s;
    return curr_libfunc_const - 1;
}

/*===============================================================================================*/
/* Emit */

void emit_target(instruction* p) {
	if(curr_instruction==total_instruction) {
        total_instruction += EXPAND_SIZE;
        instructions = realloc(instructions, total_str_const*sizeof(instruction*));
        if(!instructions) { MemoryFail(); }
    }
	instruction* i = instructions+curr_instruction++;
	i->opcode 	= p->opcode;
	i->arg1 	= p->arg1;
	i->arg2 	= p->arg2;
	i->result 	= p->result;
	i->srcLine	= p->srcLine;
}

/*===============================================================================================*/
/* Translate to offset */

void make_operand(expr* e, vmarg* arg) {
    if (!e || !arg) return;
    switch (e->type) {
        /* All those below use a variable for storage */
        case EXP_VARIABLE:
        case EXP_TABLEITEM:
        case EXP_ARITH:
        case EXP_BOOL:
        case EXP_NEWTABLE: {
            if(e->symbol) { arg->val = e->symbol->offset; }
            else { perror("Error. NULL SYMBOL\n"); }
            switch (e->symbol->type) {
                case GLOBAL_T:    arg->type = GLOBAL_V; break;
                case LOCAL_T:     arg->type = LOCAL_V; break;
                case FORMAL_T:    arg->type = FORMAL_V; break;
                case TEMPORARY_T: arg->type = TEMPORARY_V; break;
                default:          assert(0);
            }
            break;
        }
        /* Constants */
        case EXP_CONSTBOOL: {
            arg->val = e->boolConst;
            arg->type = BOOL_V;
            break;
        }
        case EXP_CONSTSTRING: {
            arg->val = consts_newstring(e->stringConst);
            arg->type = STRING_V;
            break;
        }
        case EXP_CONSTNUMBER: {
            arg->val = consts_newnumber(e->numConst);
            arg->type = NUMBER_V;
            break;
        }
        case EXP_NIL: {
            arg->type = NIL_V;
            break;
        }
        /* Functions */
        case EXP_PROGRAMFUNC: {
            arg->val = e->symbol->quad_addr;
            arg->type = USERFUNC_V;
            break;
        }
        case EXP_LIBRARYFUNC: {
            arg->val = consts_newlibfunc(e->symbol->name);
            arg->type = LIBFUNC_V;
            break;
        }
        default:
            assert(0);
    }
}

/*===============================================================================================*/
/* Jumps */

void add_incomplete_jump(unsigned instrNo, unsigned iaddress) {
    incomplete_jump* inc = (incomplete_jump*)malloc(sizeof(incomplete_jump));
    if(!inc) { MemoryFail(); }
    inc->instrNo = instrNo;
    inc->iaddress = iaddress;
    if(!ij_head) {
        ij_head = inc;
        inc->next = NULL;
    } else {
        inc->next = ij_head;
        ij_head = inc;
    }
}

void patch_incomplete_jumps(void) {
    struct incomplete_jump* x = ij_head;
    while(x) {
        if(x->iaddress == currquad) {
            instructions[x->instrNo].result.val = curr_instruction; 
        } else {
            instructions[x->instrNo].result.val = quads[x->iaddress].target_addr;
        }
        x = x->next;
    }
}

/*===============================================================================================*/
/* Generate */

void generate_ASSIGN(quad* q) {
    instruction* t = (instruction*)malloc(sizeof(instruction));
    if (!t) { perror("Failed to allocate instruction"); exit(EXIT_FAILURE); }
    t->opcode =ASSIGN_V;
    make_operand(q->arg1, &(t->arg1));   
    make_operand(NULL, &(t->arg2));      
    make_operand(q->result, &(t->result)); 
    q->target_addr = curr_instruction;
    emit_target(t);
}

void generate_ADD(quad* q) { helper_generate_full(ADD_V, q); } 
void generate_SUB(quad* q) { helper_generate_full(SUB_V, q); } 
void generate_MUL(quad* q) { helper_generate_full(MUL_V, q); }
void generate_DIV(quad* q) { helper_generate_full(DIV_V, q); } 
void generate_MOD(quad* q) { helper_generate_full(MOD_V, q); }
void generate_UMINUS(quad* q) {
    q->arg2 = create_constbool_expr(-1);
    helper_generate_full(MUL_V, q);
}

void generate_AND(quad* q) { printf("AND Should not exist\n"); }
void generate_OR(quad* q) { printf("OR Should not exist\n"); }
void generate_NOT(quad* q) { printf("NOT Should not exist\n"); }

void generate_IF_EQ(quad* q) { helper_generate_relational(JEQ_V, q); }
void generate_IF_NOTEQ(quad* q) { helper_generate_relational(JNE_V, q); }
void generate_IF_LESSEQ(quad* q) { helper_generate_relational(JLE_V, q); }
void generate_IF_GREATEREQ(quad* q) { helper_generate_relational(JGE_V, q); }
void generate_IF_LESS(quad* q) { helper_generate_relational(JLT_V, q); }
void generate_IF_GREATER(quad* q) { helper_generate_relational(JGT_V, q); }

void generate_CALL(quad* q) { helper_generate_arg1(CALL_V, q); }
void generate_PARAM(quad* q) { helper_generate_arg1(PARAM_V, q); }

void generate_RETURN(quad* q) { helper_generate_res(RETURN_V, q); }
void generate_GETRETVAL(quad* q) { helper_generate_res(GETRETVAL_V, q); }

void generate_FUNCSTART(quad* q) { helper_generate_arg1(FUNCSTART_V, q); }
void generate_FUNCEND(quad* q) { helper_generate_arg1(FUNCEND_V, q); }

void generate_NEWTABLE(quad* q) { helper_generate_res(TABLECREATE_V, q); }

void generate_TABLEGETELEM(quad* q) { helper_generate_full(TABLEGETELEM_V, q); }
void generate_TABLESETELEM(quad* q) { helper_generate_full(TABLEGETELEM_V, q); }

void generate_JUMP(quad* q) {
    instruction* t = (instruction*)malloc(sizeof(instruction));
    if (!t) { perror("Failed to allocate instruction"); exit(EXIT_FAILURE); }
    t->opcode = JUMP_V;
    make_operand(NULL, &(t->arg1)); 
    make_operand(NULL, &(t->arg2)); 
    t->result.type = LABEL_V;
    t->result.val = q->label;
    q->target_addr = curr_instruction;
    emit_target(t);
}

void generate_NOP(quad* q) { printf("NOP Should not exist\n"); }

void helper_generate_full(vmopcode op, quad* q){
    instruction* t = (instruction*)malloc(sizeof(instruction));
    if(!t) { MemoryFail(); }
    t->opcode = op;
    make_operand(q->arg1, &(t->arg1));
    make_operand(q->arg2, &(t->arg2));
    make_operand(q->result, &(t->result));
    q->target_addr = curr_instruction;
    emit_target(t);
}

void helper_generate_relational(vmopcode op, quad* q){
    instruction* t = (instruction*)malloc(sizeof(instruction));
    if(!t) { MemoryFail(); }
    t->opcode = op;
    make_operand(q->arg1, &(t->arg1));
    make_operand(q->arg2, &(t->arg2));
    t->result.type = LABEL_V;
    t->result.val = q->label;
    q->target_addr = curr_instruction;
    emit_target(t);
}

void helper_generate_arg1(vmopcode op, quad* q){
    instruction* t = (instruction*)malloc(sizeof(instruction));
    if(!t) { MemoryFail(); }
    t->opcode = op;
    make_operand(q->arg1, &(t->arg1));
    make_operand(NULL, &(t->arg2));
    make_operand(q->result, &(t->result));
    q->target_addr = curr_instruction;
    emit_target(t);
}

void helper_generate_res(vmopcode op, quad* q){
    instruction* t = (instruction*)malloc(sizeof(instruction));
    if(!t) { MemoryFail(); }
    t->opcode = op;
    make_operand(NULL, &(t->arg1));
    make_operand(NULL, &(t->arg2)); 
    make_operand(q->result, &(t->result));
    q->target_addr = curr_instruction;
    emit_target(t);
}



void generate(void) {
    for(unsigned i = 0; i < totalquads; ++i) {
        /* REMEBER GENERATORS MUST ALSO FILL target_addr field of their quad */
        (*generators[quads[i].op])(quads + i);
    }
    patch_incomplete_jumps();
}

/* end of target.c */