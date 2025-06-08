/* avm_prints.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "avm_prints.h"

#define COL_WIDTH_OPCODE 12
#define COL_WIDTH_ARG    20
#define COL_WIDTH_LINE   5

static const char* vmopcode_to_string(vmopcode_t op) {
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

static int is_jump_opcode(vmopcode_t op) {
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

void printReadTargetToFile(void) {
    FILE* fp = fopen("2_readtarget.output", "w");
    if(!fp) { perror("Error opening 2_readtarget.output for writing\n"); return; }
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
}

/* end of avm_prints.c */
