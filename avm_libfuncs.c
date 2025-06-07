/* avm_libfuncs.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm.h"

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

void libfunc_print() {
    int totals = stack[stack_top].data.stackval_zoumi;
	for(int i = 0; i < totals; ++i) {
        fprintf(avm_log, "Printing arg %d\n", i);
        memcell* tmp = &stack[stack_top - 1 - i];
		char * s = strdup((*to_string_funcs[tmp->type])(tmp));
		printf("%s",s );
		free(s);
	}
	stack[0].type = MEM_NIL;
}

void libfunc_input() {

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
void libfunc_typeof() {

}
void libfunc_strtonum() {

}
void libfunc_sqrt() {

}
void libfunc_cos() {

}
void libfunc_sin() {

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