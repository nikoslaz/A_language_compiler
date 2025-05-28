#include "target.h"
#include "quads.h"

unsigned magic_number = MAGIC_NUMBER;
incomplete_jump* ij_head = (incomplete_jump*) 0;
unsigned ij_total = 0;

generator_func_t generators[] = {
    generate_ASSIGN,
    generate_ADD, generate_SUB, generate_MUL, generate_DIV, generate_MOD,
    generate_UMINUS,
    generate_AND, generate_OR, generate_NOT,
    generate_IF_EQ, generate_IF_NOTEQ, generate_IF_LESSEQ, generate_IF_GREATEREQ, generate_IF_LESS, generate_IF_GREATER,
    generate_CALL, generate_PARAM, generate_RETURN, generate_GETRETVAL, generate_FUNCSTART, generate_FUNCEND,
    generate_NEWTABLE, generate_TABLEGETELEM, generate_TABLESETELEM,
    generate_JUMP,
    generate_NOP
};

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

static const char* vmopcode_to_string(vmopcode op) {
    switch (op) {
        case ASSIGN_V: return "ASSIGN";
        case ADD_V: return "ADD";
        case SUB_V: return "SUB";
        case MUL_V: return "MUL";
        case DIV_V: return "DIV";
        case MOD_V: return "MOD";
        case UMINUS_V: return "UMINUS";
        case AND_V: return "AND";
        case OR_V: return "OR";
        case NOT_V: return "NOT";
        case JEQ_V: return "JEQ";
        case JNE_V: return "JNE";
        case JLE_V: return "JLE";
        case JGE_V: return "JGE";
        case JLT_V: return "JLT";
        case JGT_V: return "JGT";
        case CALL_V: return "CALL";
        case PARAM_V: return "PARAM";
        case RETURN_V: return "RETURN";
        case GETRETVAL_V: return "GETRETVAL";
        case FUNCSTART_V: return "FUNCSTART";
        case FUNCEND_V: return "FUNCEND";
        case TABLECREATE_V: return "TABLECREATE";
        case TABLEGETELEM_V: return "TABLEGETELEM";
        case TABLESETELEM_V: return "TABLESETELEM";
        case JUMP_V: return "JUMP";
        case NOP_V: return "NOP";
        default: return "UNKNOWN_OP";
    }
}

// Helper function to check if an opcode is a jump instruction
static int is_jump_opcode(vmopcode op) {
    switch (op) {
        case JEQ_V:
        case JNE_V:
        case JLE_V:
        case JGE_V:
        case JLT_V:
        case JGT_V:
        case JUMP_V:
            return 1; // True
        default:
            return 0; // False
    }
}

// Helper function to print a vmarg operand
static void print_vmarg(FILE* fp, vmarg* arg, int is_jump_target) {
    if (!arg) {
        fprintf(fp, "(null vmarg)");
        return;
    }

    if (is_jump_target) {
        // For jump targets, arg->val is an instruction address (label)
        // The type might be 0 (GLOBAL_V) by default from calloc if not specifically set.
        // We can just print the label.
        fprintf(fp, "Label(%u)", arg->val);
        return;
    }
    
    // Print type as number for debugging, then human-readable form
    // fprintf(fp, "(%u)", arg->type); 

    switch (arg->type) {
        case GLOBAL_V:    fprintf(fp, "G[%u]", arg->val); break;
        case LOCAL_V:     fprintf(fp, "L[%u]", arg->val); break;
        case FORMAL_V:    fprintf(fp, "F[%u]", arg->val); break;
        case USERFUNC_V:
            // Assuming arg->val for USERFUNC_V is the starting instruction index of the function
            fprintf(fp, "UFuncAddr(%u)", arg->val);
            break;
        case LIBFUNC_V:
            if (arg->val < curr_libfunc_const && libfunc_const != NULL) {
                fprintf(fp, "LibFunc(\"%s\")", libfunc_const[arg->val]);
            } else {
                fprintf(fp, "LibFunc(INVALID_IDX:%u)", arg->val);
            }
            break;
        case TEMPORARY_V: fprintf(fp, "T[%u]", arg->val); break;
        case BOOL_V:      fprintf(fp, "%s", arg->val ? "true" : "false"); break;
        case STRING_V:
            if (arg->val < curr_str_const && string_const != NULL) {
                fprintf(fp, "\"%s\"", string_const[arg->val]);
            } else {
                fprintf(fp, "Str(INVALID_IDX:%u)", arg->val);
            }
            break;
        case NUMBER_V:
            if (arg->val < curr_num_const && number_const != NULL) {
                fprintf(fp, "%g", number_const[arg->val]);
            } else {
                fprintf(fp, "Num(INVALID_IDX:%u)", arg->val);
            }
            break;
        case NIL_V:       fprintf(fp, "nil"); break;
        // If you add a LABEL_V type for jump targets, handle it here:
        // case LABEL_V:     fprintf(fp, "Label(%u)", arg->val); break;
        default:          fprintf(fp, "UnknownType(%u):%u", arg->type, arg->val); break;
    }
}

// Main function to print the generated AVM code to a file
void printFile() {
    FILE* fp = fopen("avm_output.txt", "w");
    if (!fp) {
        perror("Error opening avm_output.txt for writing");
        return;
    }

    fprintf(fp, "magic_number: 0x%X (%u)\n\n", magic_number, magic_number);

    fprintf(fp, "--- String Constants (%u total) ---\n", curr_str_const);
    if (string_const) {
        for (unsigned i = 0; i < curr_str_const; ++i) {
            fprintf(fp, "%u: \"%s\"\n", i, string_const[i]);
        }
    } else {
        fprintf(fp, "(No string constants defined or array is null)\n");
    }
    fprintf(fp, "\n");

    fprintf(fp, "--- Number Constants (%u total) ---\n", curr_num_const);
    if (number_const) {
        for (unsigned i = 0; i < curr_num_const; ++i) {
            fprintf(fp, "%u: %g\n", i, number_const[i]);
        }
    } else {
        fprintf(fp, "(No number constants defined or array is null)\n");
    }
    fprintf(fp, "\n");

    fprintf(fp, "--- Library Functions Used (%u total) ---\n", curr_libfunc_const);
    if (libfunc_const) {
        for (unsigned i = 0; i < curr_libfunc_const; ++i) {
            fprintf(fp, "%u: \"%s\"\n", i, libfunc_const[i]);
        }
    } else {
        fprintf(fp, "(No library functions used or array is null)\n");
    }
    fprintf(fp, "\n");

    // Print Instructions
    fprintf(fp, "--- Instructions (%u total) ---\n", curr_instruction);
    fprintf(fp, "Instr#\tOpcode\t\tResult\t\tArg1\t\tArg2\t\tSrcLine\n");
    fprintf(fp, "------------------------------------------------------------------------------------\n");
    if (instructions) {
        for (unsigned i = 0; i < curr_instruction; ++i) {
            instruction* instr = &instructions[i];
            fprintf(fp, "%u:\t%-10s\t", i, vmopcode_to_string(instr->opcode));

            int is_jump_instr = is_jump_opcode(instr->opcode);
            print_vmarg(fp, &instr->result, is_jump_instr);
            fprintf(fp, "\t\t");

            print_vmarg(fp, &instr->arg1, 0); 
            fprintf(fp, "\t\t");

            print_vmarg(fp, &instr->arg2, 0); 
            fprintf(fp, "\t\t");

            fprintf(fp, "%u\n", instr->srcLine);
        }
    } else {
        fprintf(fp, "(No instructions generated or array is null)\n");
    }

    fclose(fp);
    printf("AVM code written to avm_output.txt\n");
}

/* end of target.c */