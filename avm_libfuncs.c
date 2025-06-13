/* avm_libfuncs.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

#define PI 3.14159265

unsigned libfuncs_total = 12;
char error_buffer[256];

const char* typeStrings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "stackval",
    "undef"
};

library_func_t libFuncs[] = {
    libfunc_print,
    libfunc_input,
    libfunc_objectmemberkeys,
    libfunc_objecttotalmembers,
    libfunc_objectcopy,
    libfunc_totalarguments,
    libfunc_argument,
    libfunc_typeof,
    libfunc_strtonum,
    libfunc_sqrt,
    libfunc_cos,
    libfunc_sin
};

void libfunc_print(void) {
    int totals = stack[stack_top].data.stackval_zoumi;
	for(int i = 0; i < totals; ++i) {
        memcell* tmp = &stack[stack_top - 1 - i];
		char * s = (*to_string_funcs[tmp->type])(tmp);
        if(!s) { runtimeError("Print returned null due to memory"); }
		printf("%s",s);
        if(tmp->type == MEM_TABLE || tmp->type == MEM_NUMBER
        || tmp->type == MEM_USERFUNC || tmp->type == MEM_LIBFUNC
        || tmp->type == MEM_STACKVAL) { /* free */ }
	}
    clear_memcell(&stack[0]);
	stack[0].type = MEM_NIL;
}

void libfunc_input(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 0) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'input' expects 0 arguments, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    char input_buffer[1024]; 
    if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
        clear_memcell(&stack[0]);
        stack[0].type = MEM_NIL;
        return;
    }
    input_buffer[strcspn(input_buffer, "\n")] = '\0';
    
    clear_memcell(&stack[0]);
    if(strcmp(input_buffer, "nil") == 0) {
        stack[0].type = MEM_NIL;
    } else if(strcmp(input_buffer, "true") == 0) {
        stack[0].type = MEM_BOOL;
        stack[0].data.bool_zoumi = 1;
    } else if(strcmp(input_buffer, "false") == 0) {
        stack[0].type = MEM_BOOL;
        stack[0].data.bool_zoumi = 0;
    } else {
        char* end = NULL;
        double num = strtod(input_buffer, &end);
        if(end != input_buffer && *end == '\0') {
            stack[0].type = MEM_NUMBER;
            stack[0].data.num_zoumi = num;
        } else {
            stack[0].type = MEM_STRING;
            stack[0].data.string_zoumi = strdup(input_buffer);
        }
    }
}

void libfunc_objectmemberkeys(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'objectmemberkeys' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    if(arg->type != MEM_TABLE) {
        runtimeError("Error: 'objectmemberkeys' argument must be a table.");
    }
    
    memcell result_table_cell;
    result_table_cell.type = MEM_TABLE;
    result_table_cell.data.table_zoumi = table_new();
    double array_index = 0.0;
    
    memcell new_key;
    table* input_table = arg->data.table_zoumi;
    for(unsigned int i = 0; i < HASHTABLE_SIZE; ++i) {
        for(table_bucket* p = input_table->hashtable[i]; p; p = p->next) {
            new_key.type = MEM_NUMBER;
            new_key.data.num_zoumi = array_index++;
            helper_table_SET(&result_table_cell, &new_key, &p->key);
        }
    }
    clear_memcell(&stack[0]);
    helper_assign(&stack[0], &result_table_cell);
}


void libfunc_objecttotalmembers() {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'objecttotalmembers' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    if(arg->type != MEM_TABLE) {
        runtimeError("Error: 'objecttotalmembers' argument must be a table.");
    }
    clear_memcell(&stack[0]);
    stack[0].type = MEM_NUMBER;
    stack[0].data.num_zoumi = arg->data.table_zoumi->total;
}

void libfunc_objectcopy() {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'objectcopy' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    if (arg->type != MEM_TABLE) {
        runtimeError("Error: 'objectcopy' argument must be a table.");
    }
    memcell new_table_cell;
    new_table_cell.type = MEM_TABLE;
    new_table_cell.data.table_zoumi = table_new();
    
    table* input_table = arg->data.table_zoumi;
    for (unsigned int i = 0; i < HASHTABLE_SIZE; ++i) {
        for (table_bucket* p = input_table->hashtable[i]; p; p = p->next) {
            helper_table_SET(&new_table_cell, &p->key, &p->value);
        }
    }
    clear_memcell(&stack[0]);
    helper_assign(&stack[0], &new_table_cell);
}

void libfunc_totalarguments(void) {
    clear_memcell(&stack[0]);
    if(stack_maul == 1) {
        stack[0].type = MEM_NIL;
        return;
    } else {
        memcell* num_args_cell = &stack[stack_maul - 3];
        stack[0].type = MEM_NUMBER;
        stack[0].data.num_zoumi = num_args_cell->data.stackval_zoumi;   
    }
}

void libfunc_argument(void) {
    clear_memcell(&stack[0]);
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'argument' expects 1 argument (the index), but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* index_arg = &stack[stack_top - 1];
    if(index_arg->type != MEM_NUMBER) {
        runtimeError("Error: 'argument' expects a number as its argument.");
    }
    unsigned int i = (unsigned int)index_arg->data.num_zoumi;

    if(stack_maul == 1) {
        stack[0].type = MEM_NIL;
        return;
    }
    unsigned int total_caller_args = stack[stack_maul -3].data.stackval_zoumi;
    
    if(i >= total_caller_args) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: index %u out of bounds for 'argument'. Caller has %u arguments.", i, total_caller_args);
        runtimeError(error_buffer);
    }
    memcell* target_arg = &stack[stack_maul -4 - i];
    helper_assign(&stack[0], target_arg);
}

void libfunc_typeof(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'typeof' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);
    stack[0].type = MEM_STRING;
    stack[0].data.string_zoumi = strdup(typeStrings[arg->type]);
    if(!stack[0].data.string_zoumi) {
        runtimeError("Internal error: memory allocation failed in typeof.");
    }
}

void libfunc_strtonum(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'strtonum' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);
    if(arg->type == MEM_STRING) {
        const char* input_string = arg->data.string_zoumi;
        char* end = NULL;
        double num = strtod(input_string, &end);
        if(end != input_string && *end == '\0') {
            // Success! The whole string was a valid number.
            stack[0].type = MEM_NUMBER;
            stack[0].data.num_zoumi = num;
        } else {
            stack[0].type = MEM_NIL;
        }
    } else {
        stack[0].type = MEM_NIL;
    }
}

void libfunc_sqrt(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'sqrt' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);
    if(arg->type == MEM_NUMBER) {
        double value = arg->data.num_zoumi;
        if(value < 0) {
            snprintf(error_buffer, sizeof(error_buffer), "sqrt received negative number %f. Returning NULL", value);
            runtimeWarning(error_buffer);
            stack[0].type = MEM_NIL;
        } else {
            stack[0].type = MEM_NUMBER;
            stack[0].data.num_zoumi = sqrt(value);
        }   
    } else {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'sqrt' argument must be a number, not a %s.", typeStrings[arg->type]);
        runtimeError(error_buffer);
    }
}

void libfunc_cos(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'cos' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);
    if(arg->type == MEM_NUMBER) {
        double degrees = arg->data.num_zoumi;
        double radians = degrees * (PI / 180.0);
        stack[0].type = MEM_NUMBER;
        stack[0].data.num_zoumi = cos(radians);
    } else {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'cos' argument must be a number, not a %s.", typeStrings[arg->type]);
        runtimeError(error_buffer);
    }
}

void libfunc_sin(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if(num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'sin' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
    }
    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);
    if(arg->type == MEM_NUMBER) {
        double degrees = arg->data.num_zoumi;
        double radians = degrees * (PI / 180.0);
        stack[0].type = MEM_NUMBER;
        stack[0].data.num_zoumi = sin(radians);
    } else {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'sin' argument must be a number, not a %s.", typeStrings[arg->type]);
        runtimeError(error_buffer);
    }
}

/*===============================================================================================*/
/* Calling the LIBS */

char * libfunc_names[] = {
    "print",
    "input",
    "objectmemberkeys",
    "objecttotalmembers",
    "objectcopy",
    "totalarguments",
    "argument",
    "typeof",
    "strtonum",
    "sqrt",
    "cos",
    "sin"
};

library_func_t avm_get_libfunc(char * id){
	for(int i = 0; i < libfuncs_total; i++){
		if(!strcmp(libfunc_names[i],id)){
			return libFuncs[i];
		}
	}
    runtimeError("Unsupported LibFunc");
}

/* end of avm_libfuncs.c */