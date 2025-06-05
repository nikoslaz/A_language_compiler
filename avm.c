/* avm.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

execute_func_t executors[] = {
    execute_ASSIGN,
    execute_ADD, execute_SUB, execute_MUL, execute_DIV, execute_MOD,
    execute_UMINUS,
    execute_AND, execute_OR, execute_NOT,    
    execute_JEQ, execute_JNE, execute_JLE, execute_JGE, execute_JLT, execute_JGT,      
    execute_CALL, execute_PUSHARG, execute_RET, execute_GETRETVAL, execute_FUNCENTER, execute_FUNCEXIT,
    execute_NEWTABLE, execute_TABLEGETELEM, execute_TABLESETELEM,
    execute_JUMP,
    execute_NOP
};

tostring_func_t to_string_func[] = {
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring,
    invalid_tostring
};

tobool_func_t to_bool_func[] = {
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool,
    invalid_tobool
};

arithmetic_func_t arith_func[] = {
    add_arith,
    sub_arith,
    mul_arith,
    div_arith,
    mod_arith,
    uminus_arith
};

relational_func_t relat_func[] = {
	jle_rel,
	jge_rel,
	jlt_rel,
	jgt_rel
};

/* Globals */
char** string_const=(char**)0;
unsigned int total_str_const;
double* number_const=(double*)0;
unsigned int total_num_const;
char** libfunc_const=(char**)0;
unsigned int total_libfunc_const;
instruction* instructions=(instruction*)0;
unsigned int total_instructions;
int totalprogvar;

/* AVM Globals */
unsigned int succ_branch = 0;
unsigned int branch_label = 0;
unsigned int current_args_pushed = 0;
unsigned int program_counter = 0;
unsigned int stack_top = 0;
unsigned int stack_maul = 0;

/*===============================================================================================*/
/* Read Binary */

void read_binary(FILE *fd) {
	int length;
    unsigned magic_number;

	fread(&magic_number, sizeof(unsigned), 1, fd);
	if(magic_number != MAGIC_NUMBER) {
        printf("Error. Unrecognized binary file. Exiting...\n");
        exit(-1);
    }

	fread(&total_str_const, sizeof(unsigned), 1, fd);
	if(total_str_const) { string_const = (char** )malloc(total_str_const*sizeof(char*)); }
	for(int i=0; i<total_str_const; i++) {
		fread(&length, sizeof(unsigned), 1, fd);
		string_const[i] = (char*)malloc(length*sizeof(char));
		fread(string_const[i], length*sizeof(char) ,sizeof(char), fd);
	}

	fread(&total_num_const, sizeof(unsigned), 1, fd);
	if(total_num_const) { number_const = (double*)malloc(total_num_const*sizeof(double)); }
	for(int i=0; i<total_num_const; i++) {
		fread(&number_const[i], sizeof(double), 1, fd);
	}

	fread(&total_libfunc_const, sizeof(unsigned), 1, fd);
	if(total_libfunc_const) { libfunc_const = (char**)malloc(total_libfunc_const*sizeof(char*)); }
	for(int i=0; i<total_libfunc_const; i++) {
		fread(&length, sizeof(unsigned), 1, fd);
		libfunc_const[i] = (char*)malloc(length*sizeof(char));
		fread(libfunc_const[i], length*sizeof(char), 1, fd);
	}

	fread(&total_instructions, sizeof(unsigned), sizeof(unsigned), fd);
	if(total_instructions) { instructions = (instruction*)malloc(total_instructions*sizeof(instruction)); }
	for(int i=0; i<total_instructions; i++) {
		fread(&instructions[i], sizeof(instruction), 1, fd);
	}

    fread(&totalprogvar, sizeof(int), 1, fd);

	fclose(fd);
}

/*===============================================================================================*/
/* AVM translate */

// avm_memcell * avm_translate_operand(vmarg *  arg , avm_memcell * reg){

// 	switch (arg->type) {
// 		case ARG_GLOBAL: 
// 		{
// 			return &stack [ AVM_STACKSIZE - 1 - arg->val];
// 		}
// 		case ARG_LOCAL:
// 		{
// 			return &stack [ topsp - arg->val];
// 		}
// 		case ARG_FORMAL:
// 		{
// 			return &stack [ topsp + AVM_STACKENV_SIZE + 1 + arg->val];
// 		}
// 		case RETVAL_A:
// 		{
// 			return &retval;
// 		}

// 		case ARG_NUMBER: 
// 		{
// 			reg->type = NUMBER_M;
// 			reg->data.numVal = consts_getnumber(arg->val);	
// 			return reg;
// 		}

// 		case ARG_STRING: 
// 		{
// 			reg->type = STRING_M;
// 			reg->data.strVal = strdup(consts_getstring(arg->val));	
// 			return reg;
// 		}

// 		case ARG_BOOL: 
// 		{

// 			reg->type = BOOL_M;
// 			reg->data.boolVal = arg->val;	
// 			return reg;	
// 		}

// 		case ARG_NIL: 
// 		{
// 			reg->type = NIL_M; 
// 			return reg;
// 		}

// 		case ARG_USERFUNC: 
// 		{
// 			reg->type = USERFUNC_M;
// 			reg->data.funcVal = user_funcs[arg->val].address;
// 			return reg;
// 		}

// 		case ARG_LIBFUNC: 
// 		{

// 			reg->type = LIBFUNC_M;
// 			reg->data.libfuncVal = libfuncs_getused(arg->val);
// 			return reg;
// 		}

// 		case ARG_UNDEFINED:
// 		{
// 			reg=(avm_memcell *)0;
// 			return reg;
// 		}

// 		default: {
// 			assert(0);
// 		}
// 	}
// }

// void avm_memcell_clear(avm_memcell* memcell){

//     if (memcell->type != MEM_UNDEF && memcell->type < 8) {

//     }
// }

/*===============================================================================================*/
/* Print */

#define COL_WIDTH_OPCODE 12
#define COL_WIDTH_ARG    20
#define COL_WIDTH_LINE   5

static const char* vmopcode_to_string(vmopcode op) {
    switch(op) {
        case OP_ASSIGN: return "ASSIGN";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_UMINUS: return "UMINUS";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_JEQ: return "JEQ";
        case OP_JNE: return "JNE";
        case OP_JLE: return "JLE";
        case OP_JGE: return "JGE";
        case OP_JLT: return "JLT";
        case OP_JGT: return "JGT";
        case OP_CALL: return "CALL";
        case OP_PARAM: return "PARAM";
        case OP_RETURN: return "RETURN";
        case OP_GETRETVAL: return "GETRETVAL";
        case OP_FUNCSTART: return "FUNCSTART";
        case OP_FUNCEND: return "FUNCEND";
        case OP_TABLECREATE: return "TABLECREATE";
        case OP_TABLEGETELEM: return "TABLEGETELEM";
        case OP_TABLESETELEM: return "TABLESETELEM";
        case OP_JUMP: return "JUMP";
        case OP_NOP: return "NOP";
        default: return "UNKNOWN_OP";
    }
}

static int is_jump_opcode(vmopcode op) {
    switch(op) {
        case OP_JEQ:
        case OP_JNE:
        case OP_JLE:
        case OP_JGE:
        case OP_JLT:
        case OP_JGT:
        case OP_JUMP:
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
            case ARG_GLOBAL:    snprintf(buffer, sizeof(buffer), "G[%u]",       arg->val); break;
            case ARG_LOCAL:     snprintf(buffer, sizeof(buffer), "L[%u]",       arg->val); break;
            case ARG_FORMAL:    snprintf(buffer, sizeof(buffer), "F[%u]",       arg->val); break;
            case ARG_USERFUNC:  snprintf(buffer, sizeof(buffer), "UsrFunc(%u)", arg->val + 1); break;
            case ARG_LIBFUNC:   snprintf(buffer, sizeof(buffer), "LibFunc(%u)", arg->val); break;
            case ARG_TEMPORARY: snprintf(buffer, sizeof(buffer), "T[%u]",       arg->val); break;
            case ARG_BOOL:      snprintf(buffer, sizeof(buffer), "%s",          arg->val ? "TRUE" : "FALSE"); break;
            case ARG_STRING:    snprintf(buffer, sizeof(buffer), "Str(%u)",     arg->val); break;
            case ARG_NUMBER:    snprintf(buffer, sizeof(buffer), "Num(%u)",     arg->val); break;
            case ARG_LABEL:     snprintf(buffer, sizeof(buffer), "Label(%u)",   arg->val + 1); break;
            case ARG_NUMLOCALS: snprintf(buffer, sizeof(buffer), "Locals(%u)",  arg->val); break;
            case ARG_NIL:       snprintf(buffer, sizeof(buffer), "NIL");        break;
            case ARG_UNDEFINED: snprintf(buffer, sizeof(buffer), "Undef");      break;
            default:          snprintf(buffer, sizeof(buffer), "Unknown");    break;
        }
    }
    fprintf(fp, "%-*s", COL_WIDTH_ARG, buffer);
}

void printTargetToFile(void) {
    FILE* fp = fopen("readtarget.output", "w");
    if(!fp) { perror("Error opening readtarget.output for writing\n"); return; }
    fprintf(fp, "Magic_number: 0x%X (%u)\n\n", MAGIC_NUMBER, MAGIC_NUMBER);

    fprintf(fp, "--- String Constants (%u total) ---\n", total_str_const);
    for (unsigned i = 0; i < total_str_const; ++i)
        fprintf(fp, "%-4u: \"%s\"\n", i, string_const[i]);
    fprintf(fp, "\n");

    fprintf(fp, "--- Number Constants (%u total) ---\n", total_num_const);
    for (unsigned i = 0; i < total_num_const; ++i)
        fprintf(fp, "%-4u: %g\n", i, number_const[i]);
    fprintf(fp, "\n");

    fprintf(fp, "--- Library Functions (%u total) ---\n", total_libfunc_const);
    for (unsigned i = 0; i < total_libfunc_const; ++i)
        fprintf(fp, "%-4u: \"%s\"\n", i, libfunc_const[i]);
    fprintf(fp, "\n");

    fprintf(fp, "--- Instructions (%u total) ---\n", total_instructions);
    fprintf(fp, "#   Opcode       Result              Arg1                Arg2               Line\n");
    fprintf(fp, "---------------------------------------------------------------------------------\n");

    for(unsigned i = 0; i < total_instructions; ++i) {
        instruction* instr = &instructions[i];

        fprintf(fp, "%-3u %-*s ", i + 1, COL_WIDTH_OPCODE, vmopcode_to_string(instr->opcode));

        print_vmarg_aligned(fp, &instr->result, is_jump_opcode(instr->opcode));
        print_vmarg_aligned(fp, &instr->arg1, 0);
        print_vmarg_aligned(fp, &instr->arg2, 0);

        fprintf(fp, "%-*u\n", COL_WIDTH_LINE, instr->srcLine);
    }

    fprintf(fp, "\nTotal Program Variables: (%d)\n", totalprogvar);

    fclose(fp);
    printf("Read Target code written to readtarget.output\n");
}

/*===============================================================================================*/
/* Main */

int main(int argc, char** argv) {
    FILE* fin;
    if(argc > 1) {
        if(!(fin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { printf("Error. File not supplied\n"); return 1; }
    read_binary(fin);
    printTargetToFile();
    return 0;
}

/* end of avm.c */
