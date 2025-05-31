#include "avm_readbin.h"

char** string_const;
unsigned int total_str_const;
double* number_const;
unsigned int total_num_const;
char** libfunc_const;
unsigned int total_libfunc_const;
instruction* instructions;
unsigned int total_instruction;
int totalprogvar;

void read_binary(void) {

	FILE *fd;
	int i;
	int length;
    unsigned magic_number;

	fd = fopen("chief.alpha","rb");
    

	fread(&magic_number,sizeof(unsigned),1,fd);

	assert(magic_number==MAGIC_NUMBER);

	fread(&total_str_const,sizeof(unsigned),1,fd);

	if(total_str_const)
		string_const = (char ** ) malloc ( total_str_const * sizeof(char *));

	for(i=0;i<total_str_const;++i){

		fread(&length,sizeof(unsigned),1,fd);

		string_const[i] = (char *) malloc (length * sizeof(char));

		fread(string_const[i], length * sizeof(char) ,sizeof(char),fd);

	}

	fread(&total_num_const,sizeof(unsigned),1,fd);

	if(total_num_const)
		number_const = (double * ) malloc ( total_num_const * sizeof(double ));

	

	for(i=0;i<total_num_const;++i){

		fread(&number_const[i],sizeof(double),1,fd);
	}

	fread(&total_libfunc_const,sizeof(unsigned),1,fd);

	if(total_libfunc_const)
		libfunc_const = (char ** ) malloc ( total_libfunc_const * sizeof(char *));

	for(i=0;i<total_libfunc_const;++i){

		fread(&length,sizeof(unsigned),1,fd);

		libfunc_const[i] = (char *)malloc (length * sizeof(char));

		fread(libfunc_const[i],length * sizeof(char),1,fd);

	}

	fread(&total_instruction,sizeof(unsigned),sizeof(unsigned),fd);

	if(total_instruction)
		instructions = (instruction *)malloc (total_instruction * sizeof(instruction));


	for (i = 0; i < total_instruction; i++) {

		fread(&instructions[i],sizeof(instruction),1,fd);

	}
	fread(&totalprogvar,sizeof(unsigned),1,fd);

	fclose(fd);

}

/*===============================================================================================*/
/* Main */

static const char* vmopcode_to_string(vmopcode op) {
    switch(op) {
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

static int is_jump_opcode(vmopcode op) {
    switch(op) {
        case JEQ_V:
        case JNE_V:
        case JLE_V:
        case JGE_V:
        case JLT_V:
        case JGT_V:
        case JUMP_V:
            return 1;
        default:
            return 0;
    }
}

static void print_vmarg_aligned(FILE* fp, vmarg* arg, int is_jump_target) {
    char buffer[64] = {0};
    if (!arg) { snprintf(buffer, sizeof(buffer), "null vmarg)"); }
    else if (is_jump_target) { snprintf(buffer, sizeof(buffer), "Label(%u)", arg->val + 1); }
    else {
        switch (arg->type) {
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
    FILE* fp = fopen("readtarget.output", "w");
    if (!fp) {
        perror("Error opening readtarget.output for writing");
        return;
    }
    fprintf(fp, "Magic_number: 0x%X (%u)\n", MAGIC_NUMBER, MAGIC_NUMBER);
    fprintf(fp, "Total Program Variables: (%u)\n\n", totalprogvar);

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

    fprintf(fp, "--- Instructions (%u total) ---\n", total_instruction);
    fprintf(fp, "#   Opcode       Result             Arg1               Arg2               Line\n");
    fprintf(fp, "-------------------------------------------------------------------------------\n");

    for (unsigned i = 0; i < total_instruction; ++i) {
        instruction* instr = &instructions[i];

        fprintf(fp, "%-3u %-*s ", i + 1, COL_WIDTH_OPCODE, vmopcode_to_string(instr->opcode));

        print_vmarg_aligned(fp, &instr->result, is_jump_opcode(instr->opcode));
        print_vmarg_aligned(fp, &instr->arg1, 0);
        print_vmarg_aligned(fp, &instr->arg2, 0);

        fprintf(fp, "%-*u\n", COL_WIDTH_LINE, instr->srcLine);
    }
    fclose(fp);
    printf("Read Target code written to readtarget.output\n");
}

int main(void) {
    read_binary();
    printTargetToFile();
    return 0;
}