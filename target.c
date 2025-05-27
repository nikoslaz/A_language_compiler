#include "target.h"

void resize_string(void) {
    char** p = (char**)malloc(NEW_SIZE);
    if (string_const) {
        memcpy(p, string_const, CURR_SIZE);
        free(string_const);
    }
    string_const = p;
    total_str_const += EXPAND_SIZE;
} 

unsigned consts_newstring(char* s) {
    int i;
    if (curr_str_const == total_str_const) {
        resize_string();
    }
    for (i = 0; i < curr_str_const; i++) {
        if (!strcmp(s, string_const[i])) {
            return i;
        }
    }
    char** new_string = string_const + curr_str_const++;
    *new_string = strdup(s);
    return curr_str_const - 1;
}

void resize_number(void) {
    double* p = (double*)malloc(NEW_SIZE);
    if (p) {
        memcpy(p, number_const, CURR_SIZE);
        free(number_const);
    }
    number_const = p;
    total_num_const += EXPAND_SIZE;
}

unsigned consts_newnumber(double n) {
    int i;
    if (curr_num_const == total_num_const) {
        resize_number();
    }
    for (i = 0; i < curr_num_const; i++) {
        if (number_const[i] == n) {
            return i;
        }
    }
    double* new_number = number_const + curr_num_const++;
    *new_number = n;
    return curr_num_const - 1;
}

void libfunc_resize(void) {
    char** p = (char**)malloc(NEW_SIZE);
    if (p) {
        memcpy(p, named_lib_funcs, CURR_SIZE);
        free(named_lib_funcs);
    }
    named_lib_funcs = p;
    total_libfunc_const += EXPAND_SIZE;
}

unsigned libfuncs_newused(char* s) {
    int i;
    if (curr_libfunc_const == total_libfunc_const) {
        libfunc_resize();
    }
    for (i = 0; i < curr_libfunc_const; i++) {
        if (!strcmp(s, libfunc_const[i])) {
            return i;
        }
    }
    char** new_libfunc = libfunc_const + curr_libfunc_const++;
    *new_libfunc = strdup(s);
    return curr_libfunc_const - 1;
}

void make_operand(expr* e, vmarg* arg) {
    if (!e || !arg) return;

    switch (e->type) {

        /* All those below use a variable for storage */
        case EXP_VARIABLE:
        case EXP_TABLEITEM:
        case EXP_ARITH:
        case EXP_BOOL:
        case EXP_NEWTABLE: {
            arg->val = e->symbol->offset;

            switch (e->symbol->type) {
                case GLOBAL_T:    arg->type = GLOBAL_V; break;
                case LOCAL_T:     arg->type = LOCAL_V; break;
                case FORMAL_T:    arg->type = FORMAL_V; break;
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
            arg->type = USERFUNC_V;
            arg->val = e->symbol->quad_addr;
            break;
        }

        case EXP_LIBRARYFUNC: {
            arg->type = LIBFUNC_V;
            arg->val = libfuncs_newused(e->symbol->name);
            break;
        }

        default:
            assert(0);
    }
}


void add_incomplete_jump(unsigned instrNo, unsigned iaddress) {
    incomplete_jump* inc = (incomplete_jump*)malloc(sizeof(incomplete_jump));

    inc->instrNo = instrNo;
    inc->iaddress = iaddress;

    if (!ij_head) {
        ij_head = inc;
        inc->next = NULL;
    } else {
        inc->next = ij_head;
        ij_head = inc;
    }
}

void patch_incomplete_jumps(void) {
    struct incomplete_jump* x = ij_head;

    while (x) {
        if (x->iaddress == currquad) {
            instructions[x->instrNo].result.val = curr_instruction; 
        } else {
            instructions[x->instrNo].result.val = quads[x->iaddress].quad_addr;
        }
        x = x->next;
    }
}

void generate(void) {
    for (unsigned i = 0; i < total; ++i)
        (*generators[quads[i].op])(quads + i);
}