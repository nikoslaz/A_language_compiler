/* avm_libfuncs.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"
#include <math.h>

#define PI 3.14159265

static char error_buffer[256];
unsigned libfuncs_total = 12;

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

static const char* typeStrings[] = {
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


void libfunc_print() {
    int totals = stack[stack_top].data.stackval_zoumi;
	for(int i = 0; i < totals; ++i) {
        fprintf(avm_log, "Printing arg %d\n", i);
        memcell* tmp = &stack[stack_top - 1 - i];
		char * s = strdup((*to_string_funcs[tmp->type])(tmp));
		printf("%s",s );
		free(s);
	}
    clear_memcell(&stack[0]);
	stack[0].type = MEM_NIL;
}

void libfunc_input(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if (num_args != 0) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'input' expects 0 arguments, but received %u.", num_args);
        runtimeError(error_buffer);
        clear_memcell(&stack[0]);
        stack[0].type = MEM_NIL;
        return;
    }
    char input_buffer[1024]; 
    if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
        clear_memcell(&stack[0]);
        stack[0].type = MEM_NIL;
        return;
    }
    input_buffer[strcspn(input_buffer, "\n")] = '\0';
    clear_memcell(&stack[0]);

    if (strcmp(input_buffer, "nil") == 0) {
        stack[0].type = MEM_NIL;
    } else if (strcmp(input_buffer, "true") == 0) {
        stack[0].type = MEM_BOOL;
        stack[0].data.bool_zoumi = 1;
    } else if (strcmp(input_buffer, "false") == 0) {
        stack[0].type = MEM_BOOL;
        stack[0].data.bool_zoumi = 0;
    } else {
        char* end = NULL;
        double num = strtod(input_buffer, &end);
        if (end != input_buffer && *end == '\0') {
            stack[0].type = MEM_NUMBER;
            stack[0].data.num_zoumi = num;
        } else {
            stack[0].type = MEM_STRING;
            stack[0].data.string_zoumi = strdup(input_buffer);
        }
    }
}

void libfunc_objectmemberkeys() {

}

void libfunc_objecttotalmembers() {

}

void libfunc_objectcopy() {

}
void libfunc_totalarguments() {

}
void libfunc_argument() {

}

void libfunc_typeof(void) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;
    if (num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: 'typeof' expects 1 argument, but received %u.", num_args);
        runtimeError(error_buffer);
        clear_memcell(&stack[0]);
        stack[0].type = MEM_NIL;
        return;
    }
    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);
    stack[0].type = MEM_STRING;
    stack[0].data.string_zoumi = strdup(typeStrings[arg->type]);
    if (!stack[0].data.string_zoumi) {
        runtimeError("Internal error: memory allocation failed in typeof.");
        stack[0].type = MEM_NIL;
    }
}


void libfunc_strtonum() {

}
void libfunc_sqrt() {

}

static void single_arg_math_handler(const char* func_name, double (*math_op)(double)) {
    unsigned int num_args = stack[stack_top].data.stackval_zoumi;

    if (num_args != 1) {
        snprintf(error_buffer, sizeof(error_buffer), "Error: '%s' expects 1 argument, but received %u.", func_name, num_args);
        runtimeError(error_buffer);
        clear_memcell(&stack[0]);
        stack[0].type = MEM_NIL;
        return;
    }

    memcell* arg = &stack[stack_top - 1];
    clear_memcell(&stack[0]);

    if (arg->type == MEM_NUMBER) {
        double degrees = arg->data.num_zoumi;
        double radians = degrees * (PI / 180.0);
        
        stack[0].type = MEM_NUMBER;
        stack[0].data.num_zoumi = (*math_op)(radians); //cos or sin
    } else {
        snprintf(error_buffer, sizeof(error_buffer), "Error: '%s' argument must be a number, not a %s.", func_name, typeStrings[arg->type]);
        runtimeError(error_buffer);
        stack[0].type = MEM_NIL;
    }
}

void libfunc_cos(void) {
    single_arg_math_handler("cos", cos);
}

void libfunc_sin() {
    single_arg_math_handler("sin", sin);
}



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
	return NULL;
}



/* end of avm_libfuncs.c */