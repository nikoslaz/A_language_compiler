/* avm.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

execute_func_t executors[] = {
    // execute_ASSIGN,
    // execute_ADD, execute_SUB, execute_MUL, execute_DIV, execute_MOD,
    // execute_UMINUS,
    // execute_AND, execute_OR, execute_NOT,    
    // execute_JEQ, execute_JNE, execute_JLE, execute_JGE, execute_JLT, execute_JGT,      
    // execute_CALL, execute_PUSHARG, execute_RET, execute_GETRETVAL, execute_FUNCENTER, execute_FUNCEXIT,
    // execute_NEWTABLE, execute_TABLEGETELEM, execute_TABLESETELEM,
    // execute_JUMP,
    // execute_NOP
};

tostring_func_t to_string_func[] = {
    // number_tostring, string_tostring, bool_tostring,
    // table_tostring, userfunc_tostring, libfunc_tostring,
    // nil_tostring, stackval_tostring, undef_tostring
};

tobool_func_t to_bool_func[] = {
    // number_tobool, string_tobool, bool_tobool,
    // table_tobool, userfunc_tobool, libfunc_tobool,
    // nil_tobool, stackval_tobool, undef_tobool
};

arithmetic_func_t arith_func[] = {
    // add_arith, sub_arith, mul_arith,
    // div_arith, mod_arith, uminus_arith
};

relational_func_t relat_func[] = {
	// jle_rel, jge_rel, jlt_rel, jgt_rel
};

/* Consts Globals */
char** string_const = NULL;
unsigned int total_str_const;
double* number_const = NULL;
unsigned int total_num_const;
char** libfunc_const = NULL;
unsigned int total_libfunc_const;
instruction* instructions = NULL;
unsigned int total_instructions;
int totalprogvar;

/* AVM Globals */
unsigned int succ_branch;
unsigned int branch_label;
unsigned int current_args_pushed;
unsigned int program_counter;

memcell stack[AVM_STACKSIZE];
unsigned int stack_top;  /* Points to top non-empty element */
unsigned int stack_maul; /* Splits the activations in half */

/*===============================================================================================*/
/* Read Binary */

void read_binary(FILE* fd) {
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
/* Stack */

void stackError(char* input) {
	printf("Fatal Stack Error. %s.\n");
	exit(-1);
}

void initilize_stack(void) {
    for(int i=0; i<AVM_STACKSIZE; i++){
        memset(&stack[i], 0, sizeof(memcell));
        stack[i].type = MEM_UNDEF;
    }
    stack_top = -1;
}

void clear_memcell(memcell* cell) {
    if(!cell) return;
    if(cell->type == MEM_STRING && cell->data.string_zoumi) {
        /* Points to const string entry in table */
        cell->data.string_zoumi = NULL;
    } else if (cell->type == MEM_TABLE && cell->data.table_zoumi) {
        /* Decrement reference counter?? */
        cell->data.table_zoumi = NULL; 
    }
    memset(cell, 0, sizeof(memcell));
    cell->type = MEM_UNDEF;
}

void push(memcell val) {
    if(stack_top >= AVM_STACKSIZE) { stackError("Stack Overflow"); }
    stack_top++;
    stack[stack_top] = val;
}

memcell pop(void) {
    if(stack_top < 0) { stackError("Stack Underflow"); }
    memcell popped_cell = stack[stack_top];
    clear_memcell(&stack[stack_top]);
    stack_top--;
    return popped_cell;
}

/*===============================================================================================*/
/* AVM translate */

/* NEEDS WORK */
memcell* avm_translate_operand(vmarg* arg, memcell* reg) {
	if(!arg) { printf("Error. Null vmarg in translate.\n"); return NULL; }
	switch(arg->type) {
		case ARG_GLOBAL:
			unsigned int index = 2 + arg->val;
			if(arg->val >= stack[1].data.stackval_zoumi || index>stack_top) {
				stackError("Invalid Global Index");
			}
			return &stack[index];
		case ARG_TEMPORARY:
			unsigned int index = 2 + arg->val;
			if(arg->val >= stack[1].data.stackval_zoumi || index>stack_top) {
				stackError("Invalid Temporary Index");
			}
			return &stack[index];
		case ARG_LOCAL:
			unsigned int index = stack_maul + 1 + arg->val;
			if(arg->val >= stack[stack_maul].data.stackval_zoumi || index>stack_top) {
				stackError("Invalid Local Index");
			}
			return &stack[index];
		case ARG_FORMAL:
			unsigned int index = stack_maul - (4 + arg->val);
			if(arg->val >= stack[stack_maul-3].data.stackval_zoumi || index>stack_top) {
				stackError("Invalid Formal Index");
			}
			return &stack[index];
		case ARG_NUMBER:
            reg->type = MEM_NUMBER;
			reg->data.num_zoumi = number_const[arg->val];	
			return reg;
		case ARG_STRING:
			reg->type = MEM_STRING;
			reg->data.string_zoumi = string_const[arg->val];	
			return reg;
		case ARG_BOOL: 
			reg->type = MEM_BOOL;
			reg->data.bool_zoumi = arg->val;	
			return reg;	
		case ARG_NIL: 
			reg->type = MEM_NIL; 
			return reg;
		case ARG_USERFUNC:
			reg->type = MEM_USERFUNC;
			reg->data.usrfunc_zoumi = arg->val;
			return reg;
		case ARG_LIBFUNC:
			reg->type = MEM_LIBFUNC;
			reg->data.libfunc_zoumi = arg->val;
			return reg;
		case ARG_UNDEFINED:
			reg->type = MEM_UNDEF;
			return reg;
		default:
			reg->type = MEM_UNDEF;
			return reg;
	}
}

/*===============================================================================================*/
/* Main */

void avm_initialize(void) {
    initilize_stack();
	memcell cell;
	
	/* Push Ret Val */
	memset(&cell, 0, sizeof(memcell));
	cell.type = MEM_UNDEF;
	push(cell);
	
	/* Push number of program variables */
	cell.type = MEM_STACKVAL;
	cell.data.stackval_zoumi = totalprogvar;
	push(cell);
	stack_maul = 1;
	
	/* Push program variables */
	memset(&cell, 0, sizeof(memcell));
	cell.type = MEM_UNDEF;
	for(int i=0; i<totalprogvar; i++) { push(cell); }
	
	/* Controls */
    succ_branch = 0;
    current_args_pushed = 0;
    program_counter = 0;

	/* Start execution here */

}

int main(int argc, char** argv) {
    FILE* fin;
    if(argc > 1) {
        if(!(fin = fopen(argv[1], "rb"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { printf("Error. File not supplied\n"); return 1; }
    read_binary(fin);
    printReadTargetToFile();
    avm_initialize();
    return 0;
}

/* end of avm.c */
