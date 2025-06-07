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

tostring_func_t to_string_funcs[] = {
    // number_tostring, string_tostring, bool_tostring,
    // table_tostring, userfunc_tostring, libfunc_tostring,
    // nil_tostring, stackval_tostring, undef_tostring
};

tobool_func_t to_bool_funcs[] = {
    // number_tobool, string_tobool, bool_tobool,
    // table_tobool, userfunc_tobool, libfunc_tobool,
    // nil_tobool, stackval_tobool, undef_tobool
};

arithmetic_func_t arith_funcs[] = {
    add_arith, sub_arith, mul_arith,
    div_arith, mod_arith, uminus_arith
};

relational_func_t relat_funcs[] = {
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
unsigned int execution_finished;
unsigned int curr_line;

memcell stack[AVM_STACKSIZE];
unsigned int stack_top;  /* Points to top non-empty element */
unsigned int stack_maul; /* Splits the activations in half */

/*===============================================================================================*/
/* Read Binary */

void MemoryFail(void) {
    fprintf(stderr, "Fatal Error. Memory Allocation failed\n");
    exit(1);
}

void read_binary(FILE* fd) {
	int length;
    unsigned magic_number;

	fread(&magic_number, sizeof(unsigned), 1, fd);
	if(magic_number != MAGIC_NUMBER) {
        printf("Error. Unrecognized binary file. Exiting...\n");
        exit(-1);
    }

	fread(&total_str_const, sizeof(unsigned), 1, fd);
	if(total_str_const) {
		string_const = (char** )malloc(total_str_const*sizeof(char*));
		if(!string_const) { MemoryFail(); }
	}
	for(int i=0; i<total_str_const; i++) {
		fread(&length, sizeof(unsigned), 1, fd);
		string_const[i] = (char*)malloc(length*sizeof(char));
		if(!(string_const[i])) { MemoryFail(); }
		fread(string_const[i], length*sizeof(char) ,sizeof(char), fd);
	}

	fread(&total_num_const, sizeof(unsigned), 1, fd);
	if(total_num_const) {
		number_const = (double*)malloc(total_num_const*sizeof(double));
		if(!number_const) { MemoryFail(); }
	}
	for(int i=0; i<total_num_const; i++) {
		fread(&number_const[i], sizeof(double), 1, fd);
	}

	fread(&total_libfunc_const, sizeof(unsigned), 1, fd);
	if(total_libfunc_const) {
		libfunc_const = (char**)malloc(total_libfunc_const*sizeof(char*));
		if(!libfunc_const) { MemoryFail(); }
	}
	for(int i=0; i<total_libfunc_const; i++) {
		fread(&length, sizeof(unsigned), 1, fd);
		libfunc_const[i] = (char*)malloc(length*sizeof(char));
		if(!(libfunc_const[i])) { MemoryFail(); }
		fread(libfunc_const[i], length*sizeof(char), 1, fd);
	}

	fread(&total_instructions, sizeof(unsigned), sizeof(unsigned), fd);
	if(total_instructions) {
		instructions = (instruction*)malloc(total_instructions*sizeof(instruction));
		if(!instructions) { MemoryFail(); }
	}
	for(int i=0; i<total_instructions; i++) {
		fread(&instructions[i], sizeof(instruction), 1, fd);
	}

    fread(&totalprogvar, sizeof(int), 1, fd);

	fclose(fd);
}

/*===============================================================================================*/
/* Stack */

void stackError(char* input) {
	printf("Fatal Stack Error. %s.\n", input);
	exit(-1);
}

void runtimeError(char* input) {
	printf("Runtime Error in line %d. %s.\n", curr_line, input);
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
	if(++stack_top >= AVM_STACKSIZE) { stackError("Stack Overflow"); }
    stack[stack_top] = val;
}

memcell pop(void) {
    if(stack_top < 0) { stackError("Stack Underflow"); }
    memcell popped_cell = stack[stack_top];
    clear_memcell(&stack[stack_top--]);
    return popped_cell;
}

/*===============================================================================================*/
/* AVM translate */

/* NEEDS WORK */
memcell* translate_operand(vmarg* arg, memcell* reg) {
	if(!arg) { printf("Error. Null vmarg in translate.\n"); return NULL; }
	unsigned int index;
	switch(arg->type) {
		
		/* Variables */
		case ARG_GLOBAL:
			index = 1 + arg->val;
			if(index > stack_top) { stackError("Invalid Global Index"); }
			return &stack[index];
		case ARG_TEMPORARY:
			index = 1 + arg->val;
			if(index > stack_top) { stackError("Invalid Temporary Index"); }
			return &stack[index];
		case ARG_LOCAL:
			index = stack_maul + arg->val;
			if(index > stack_top) { stackError("Invalid Local Index"); }
			return &stack[index];
		case ARG_FORMAL:
			/* CHECK IF IT EXCEEDS GIVEN ARGUMENTS */
			if(stack_maul-3 > stack_top) { stackError("Illegal Stack State"); }
			if(arg->val >= stack[stack_maul-3].data.stackval_zoumi) {
				runtimeError("Not enough arguments were supplied for this operation");
			}
			index = stack_maul - (4 + arg->val);
			if(index>stack_top) { stackError("Invalid Formal Index"); }
			return &stack[index];
		
			/* Consts */
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
/* Execution */

void execute_cycle(void) {
	if(execution_finished) { return; }
	if(program_counter == total_instructions) {
		execution_finished = 1;
		return;
	}
	instruction* instr = &instructions[program_counter];
	curr_line = instr->srcLine;
	// (*executors[instr->opcode])(instr);
	if(succ_branch) {
		succ_branch = 0;
		program_counter = branch_label;
	} else { program_counter++; }
	return;
}

void begin_execution(void) {
	while(1) {
		execute_cycle();
		if(execution_finished) { break; }
	}
	printf("Execution Finished\n");
}

void avm_initialize(void) {
    
	initilize_stack();
	
	memcell cell;
	memset(&cell, 0, sizeof(memcell));
	cell.type = MEM_UNDEF;
	
	/* Push Ret Val */
	push(cell);
	/* Push Locals */
	for(int i=0; i<totalprogvar; i++) { push(cell); }
	stack_maul = 1;
	
	/* Controls */
    current_args_pushed = 0;
	execution_finished = 0;
    succ_branch = 0;
    
	/* Execute Instructions */
	program_counter = 0;
	begin_execution();
}

/*===============================================================================================*/
/* Main */

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
