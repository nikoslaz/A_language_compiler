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

void generate(void) {
    for (unsigned i = 0; i < totalquads; ++i) {
        /* REMEBER GENERATORS MUST ALSO FILL target_addr field of their quad */
        (*generators[quads[i].op])(quads + i);
    }
    patch_incomplete_jumps();
}

/* end of target.c */