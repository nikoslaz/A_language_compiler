/* target.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "target.h"
#include "quads.h"
#include "table.h"

#define COL_WIDTH_OPCODE 12
#define COL_WIDTH_ARG    20
#define COL_WIDTH_LINE   5

unsigned magic_number = MAGIC_NUMBER;

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

unsigned int consts_newstring(char* s) {
    if(curr_str_const == total_str_const) { 
        total_str_const += EXPAND_SIZE;
        string_const = realloc(string_const, total_str_const*sizeof(char*));
        if(!string_const) { MemoryFail(); }
    }
    for(int i = 0; i < curr_str_const; i++) {
        if(!strcmp(s, string_const[i])) { return i; }
    }
    string_const[curr_str_const++] = s;
    return curr_str_const - 1;
}

unsigned int consts_newnumber(double n) {
    if(curr_num_const == total_num_const) {
        total_num_const += EXPAND_SIZE;
        number_const = realloc(number_const, total_num_const*sizeof(double));
        if(!number_const) { MemoryFail(); }
    }
    for(int i = 0; i < curr_num_const; i++) {
        if(number_const[i] == n) { return i; }
    }
    number_const[curr_num_const++] = n;
    return curr_num_const - 1;
}

unsigned int consts_newlibfunc(char* s) {
    if(curr_libfunc_const == total_libfunc_const) {
        total_libfunc_const += EXPAND_SIZE;
        libfunc_const = realloc(libfunc_const, total_libfunc_const*sizeof(char*));
        if(!libfunc_const) { MemoryFail(); }
    }
    for(int i = 0; i < curr_libfunc_const; i++) {
        if(!strcmp(s, libfunc_const[i])) { return i; }
    }
    libfunc_const[curr_libfunc_const++] = s;
    return curr_libfunc_const - 1;
}

/*===============================================================================================*/
/* Emit */

void emit_target(instruction p) {
	if(curr_instruction==total_instruction) {
        total_instruction += EXPAND_SIZE;
        instructions = realloc(instructions, total_instruction*sizeof(instruction));
        if(!instructions) { MemoryFail(); }
    }
    instructions[curr_instruction++] = p;
}

/*===============================================================================================*/
/* Translate to offset */

void make_operand(expr* e, vmarg* arg) {
    if(!e || !arg) return;
    switch(e->type) {
        /* All those below use a variable for storage */
        case EXP_VARIABLE:
        case EXP_TABLEITEM:
        case EXP_ARITH:
        case EXP_BOOL:
        case EXP_NEWTABLE: {
            if(e->symbol) { arg->val = e->symbol->offset; }
            else { printf("Error. NULL SYMBOL in make_operand\n"); }
            switch(e->symbol->type) {
                case GLOBAL_T:    arg->type = GLOBAL_V; break;
                case LOCAL_T:     arg->type = LOCAL_V; break;
                case FORMAL_T:    arg->type = FORMAL_V; break;
                case TEMPORARY_T: arg->type = TEMPORARY_V; break;
                default: printf("Error. Unknown Symbol Type in make_operand\n");
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
        /* Nil */
        case EXP_NIL: {
            arg->type = NIL_V;
            break;
        }
        default: printf("Error. Unknown Expression Type in make_operand\n");
    }
}

/*===============================================================================================*/
/* Generate */

void generate_ASSIGN(quad* q) {
    instruction t;
    t.opcode = OPC_ASSIGN;
    make_operand(q->arg1, &(t.arg1));   
    t.arg2.type = UNDEFINED_V;     
    make_operand(q->result, &(t.result)); 
    emit_target(t);
}

void generate_ADD(quad* q) { helper_generate_full(OPC_ADD, q); }
void generate_SUB(quad* q) { helper_generate_full(OPC_SUB, q); }
void generate_MUL(quad* q) { helper_generate_full(OPC_MUL, q); }
void generate_DIV(quad* q) { helper_generate_full(OPC_DIV, q); }
void generate_MOD(quad* q) { helper_generate_full(OPC_MOD, q); }
void generate_UMINUS(quad* q) {
    q->arg2 = create_constnum_expr(-1);
    helper_generate_full(OPC_MUL, q);
}

void generate_AND(quad* q) { printf("Error. AND Should not exist\n"); }
void generate_OR(quad* q) { printf("Error. OR Should not exist\n"); }
void generate_NOT(quad* q) { printf("Error. NOT Should not exist\n"); }

void generate_IF_EQ(quad* q) { helper_generate_relational(OPC_JEQ, q); }
void generate_IF_NOTEQ(quad* q) { helper_generate_relational(OPC_JNE, q); }
void generate_IF_LESSEQ(quad* q) { helper_generate_relational(OPC_JLE, q); }
void generate_IF_GREATEREQ(quad* q) { helper_generate_relational(OPC_JGE, q); }
void generate_IF_LESS(quad* q) { helper_generate_relational(OPC_JLT, q); }
void generate_IF_GREATER(quad* q) { helper_generate_relational(OPC_JGT, q); }

void generate_CALL(quad* q) { helper_generate_arg1(OPC_CALL, q); }
void generate_PARAM(quad* q) { helper_generate_arg1(OPC_PARAM, q); }

void generate_RETURN(quad* q) { helper_generate_res(OPC_RETURN, q); }
void generate_GETRETVAL(quad* q) { helper_generate_res(OPC_GETRETVAL, q); }

void generate_FUNCSTART(quad* q) {
    instruction t;
    t.opcode = OPC_FUNCSTART;
    make_operand(q->arg1, &(t.arg1));   
    t.arg2.type = NUMLOCALS_V;
    if(q->arg1->symbol) {
        t.arg2.val = q->arg1->symbol->num_locals;
    } else { printf("Error. Calling a Null symbol\n"); }
    t.result.type = UNDEFINED_V; 
    t.srcLine = q->line;
    emit_target(t);
}
void generate_FUNCEND(quad* q) { helper_generate_arg1(OPC_FUNCEND, q); }

void generate_NEWTABLE(quad* q) { helper_generate_res(OPC_TABLECREATE, q); }

void generate_TABLEGETELEM(quad* q) { helper_generate_full(OPC_TABLEGETELEM, q); }
void generate_TABLESETELEM(quad* q) { helper_generate_full(OPC_TABLESETELEM, q); }

void generate_JUMP(quad* q) {
    instruction t;
    t.opcode = OPC_JUMP;
    t.arg1.type = UNDEFINED_V; 
    t.arg2.type = UNDEFINED_V; 
    t.result.type = LABEL_V;
    t.result.val = q->label;
    t.srcLine = q->line;
    emit_target(t);
}

void generate_NOP(quad* q) { printf("ERROR. NOP Should not exist\n"); }

/* Helpers */

void helper_generate_full(vmopcode op, quad* q) {
    instruction t;
    t.opcode = op;
    make_operand(q->arg1, &(t.arg1));
    make_operand(q->arg2, &(t.arg2));
    make_operand(q->result, &(t.result));
    t.srcLine = q->line;
    emit_target(t);
}

void helper_generate_relational(vmopcode op, quad* q) {
    instruction t;
    t.opcode = op;
    make_operand(q->arg1, &(t.arg1));
    make_operand(q->arg2, &(t.arg2));
    t.result.type = LABEL_V;
    t.result.val = q->label;
    t.srcLine = q->line;
    emit_target(t);
}

void helper_generate_arg1(vmopcode op, quad* q) {
    instruction t;
    t.opcode = op;
    make_operand(q->arg1, &(t.arg1));
    t.arg2.type = UNDEFINED_V; 
    t.result.type = UNDEFINED_V;
    t.srcLine = q->line;
    emit_target(t);
}

void helper_generate_res(vmopcode op, quad* q) {
    instruction t;
    t.opcode = op;
    t.arg1.type = UNDEFINED_V; 
    t.arg2.type = UNDEFINED_V; 
    make_operand(q->result, &(t.result));
    t.srcLine = q->line;
    emit_target(t);
}

void generateTarget(void) {
    char* library_names[12] = {
        "print", "input", "objectmemberkeys", "objecttotalmembers", "objectcopy",
        "totalarguments", "argument", "typeof", "strtonum", "sqrt", "cos", "sin"
    };
    for(int i=0; i<12; i++) {
        char* tmp;
        consts_newlibfunc(is_Lib_Func(strdup(library_names[i]))->name);
    }
    for(unsigned i = 0; i < currquad; ++i) {
        // printf("Generating quad %d\n", i+1);
        (*generators[quads[i].op])(quads + i);
    }
}


/*===============================================================================================*/
/* Write binary */

void write_binary(void) {
	FILE *fd = fopen("chief.alpha","wb");
    if(!fd) { printf("Error opening chief.alpha for writing.\n"); return; }
	int length;

	fwrite(&magic_number, sizeof(unsigned), 1, fd);

	fwrite(&curr_str_const,sizeof(unsigned),1,fd);
	for(int i=0; i<curr_str_const; i++) {
		length = strlen(string_const[i]);
		fwrite(&length, sizeof(unsigned), 1, fd);
		fwrite(string_const[i], sizeof(char), length, fd);
	}

	fwrite(&curr_num_const, sizeof(unsigned), 1, fd);
	for(int i=0; i<curr_num_const; i++) {
		fwrite(&number_const[i], sizeof(double), 1, fd);
	} 

	fwrite(&curr_libfunc_const, sizeof(unsigned), 1, fd);
	for(int i=0; i<curr_libfunc_const; i++) {
		length = strlen(libfunc_const[i]);
		fwrite(&length, sizeof(unsigned), 1, fd);
		fwrite(libfunc_const[i], sizeof(char), length, fd);
	}

	fwrite(&curr_instruction, sizeof(unsigned), sizeof(unsigned), fd);
	for(int i=0; i<curr_instruction; i++) {
		fwrite(&instructions[i], sizeof(instruction), 1, fd);
	}

    fwrite(&int_to_Scope(0)->scopeOffset, sizeof(int), 1, fd);

	fclose(fd);
}


/*===============================================================================================*/
/* Print */

static const char* vmopcode_to_string(vmopcode op) {
    switch(op) {
        case OPC_ASSIGN: return "ASSIGN";
        case OPC_ADD: return "ADD";
        case OPC_SUB: return "SUB";
        case OPC_MUL: return "MUL";
        case OPC_DIV: return "DIV";
        case OPC_MOD: return "MOD";
        case OPC_UMINUS: return "UMINUS";
        case OPC_AND: return "AND";
        case OPC_OR: return "OR";
        case OPC_NOT: return "NOT";
        case OPC_JEQ: return "JEQ";
        case OPC_JNE: return "JNE";
        case OPC_JLE: return "JLE";
        case OPC_JGE: return "JGE";
        case OPC_JLT: return "JLT";
        case OPC_JGT: return "JGT";
        case OPC_CALL: return "CALL";
        case OPC_PARAM: return "PARAM";
        case OPC_RETURN: return "RETURN";
        case OPC_GETRETVAL: return "GETRETVAL";
        case OPC_FUNCSTART: return "FUNCSTART";
        case OPC_FUNCEND: return "FUNCEND";
        case OPC_TABLECREATE: return "TABLECREATE";
        case OPC_TABLEGETELEM: return "TABLEGETELEM";
        case OPC_TABLESETELEM: return "TABLESETELEM";
        case OPC_JUMP: return "JUMP";
        case OPC_NOP: return "NOP";
        default: return "UNKNOWN_OP";
    }
}

static int is_jump_opcode(vmopcode op) {
    switch(op) {
        case OPC_JEQ:
        case OPC_JNE:
        case OPC_JLE:
        case OPC_JGE:
        case OPC_JLT:
        case OPC_JGT:
        case OPC_JUMP:
            return 1;
        default:
            return 0;
    }
}

static void print_vmarg_aligned(FILE* fp, vmarg* arg, int is_jump_target) {
    char buffer[64] = {0};
    if(!arg) { snprintf(buffer, sizeof(buffer), "null vmarg)"); }
    else if(is_jump_target) { snprintf(buffer, sizeof(buffer), "Label(%u)", arg->val + 1); }
    else {
        switch(arg->type) {
            case GLOBAL_V:    snprintf(buffer, sizeof(buffer), "G[%u]",       arg->val); break;
            case LOCAL_V:     snprintf(buffer, sizeof(buffer), "L[%u]",       arg->val); break;
            case FORMAL_V:    snprintf(buffer, sizeof(buffer), "F[%u]",       arg->val); break;
            case USERFUNC_V:  snprintf(buffer, sizeof(buffer), "UsrFunc(%u)", arg->val + 1); break;
            case LIBFUNC_V:   snprintf(buffer, sizeof(buffer), "LibFunc(%u)", arg->val); break;
            case TEMPORARY_V: snprintf(buffer, sizeof(buffer), "T[%u]",       arg->val); break;
            case BOOL_V:      snprintf(buffer, sizeof(buffer), "%s",          arg->val ? "TRUE" : "FALSE"); break;
            case STRING_V:    snprintf(buffer, sizeof(buffer), "Str(%u)",     arg->val); break;
            case NUMBER_V:    snprintf(buffer, sizeof(buffer), "Num(%u)",     arg->val); break;
            case LABEL_V:     snprintf(buffer, sizeof(buffer), "Label(%u)",   arg->val + 1); break;
            case NUMLOCALS_V: snprintf(buffer, sizeof(buffer), "Locals(%u)",  arg->val); break;
            case NIL_V:       snprintf(buffer, sizeof(buffer), "NIL");        break;
            case UNDEFINED_V: snprintf(buffer, sizeof(buffer), "Undef");      break;
            default:          snprintf(buffer, sizeof(buffer), "Unknown");    break;
        }
    }
    fprintf(fp, "%-*s", COL_WIDTH_ARG, buffer);
}

void printTargetToFile(void) {
    FILE* fp = fopen("target.output", "w");
    if(!fp) { perror("Error opening target.output for writing\n"); return; }

    fprintf(fp, "Magic_number: 0x%X (%u)\n\n", magic_number, magic_number);

    fprintf(fp, "--- String Constants (%u total) ---\n", curr_str_const);
    for(unsigned i = 0; i < curr_str_const; ++i)
        fprintf(fp, "%-4u: \"%s\"\n", i, string_const[i]);
    fprintf(fp, "\n");

    fprintf(fp, "--- Number Constants (%u total) ---\n", curr_num_const);
    for(unsigned i = 0; i < curr_num_const; ++i)
        fprintf(fp, "%-4u: %g\n", i, number_const[i]);
    fprintf(fp, "\n");

    fprintf(fp, "--- Library Functions (%u total) ---\n", curr_libfunc_const);
    for(unsigned i = 0; i < curr_libfunc_const; ++i)
        fprintf(fp, "%-4u: \"%s\"\n", i, libfunc_const[i]);
    fprintf(fp, "\n");

    fprintf(fp, "--- Instructions (%u total) ---\n", curr_instruction);
    fprintf(fp, "#   Opcode       Result              Arg1                Arg2               Line\n");
    fprintf(fp, "---------------------------------------------------------------------------------\n");

    for(unsigned i = 0; i < curr_instruction; ++i) {
        instruction* instr = &instructions[i];

        fprintf(fp, "%-3u %-*s ", i + 1, COL_WIDTH_OPCODE, vmopcode_to_string(instr->opcode));

        print_vmarg_aligned(fp, &instr->result, is_jump_opcode(instr->opcode));
        print_vmarg_aligned(fp, &instr->arg1, 0);
        print_vmarg_aligned(fp, &instr->arg2, 0);

        fprintf(fp, "%-*u\n", COL_WIDTH_LINE, instr->srcLine);
    }

    fprintf(fp, "\nTotal Program Variables: (%u)\n", int_to_Scope(0)->scopeOffset);
    
    fclose(fp);
    printf("Target code written to target.output\n");
}


/* end of target.c */
