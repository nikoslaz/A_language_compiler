/* alpha_quads.c */
/**
 * @authors nikos , nikoletta , mihalis
 */
#include "alpha_quads.h"

quad* quads = NULL;
unsigned int totalquads = 0;
unsigned int currquad = 0;
unsigned int temp_counter = 0;

Symbol* create_temp_symbol(void) {
    if(temp_counter>MAX_TEMPS) {
        printf("Error. Maximum number of temporary variables reached. Sorry.\n");
        return NULL;
    }
    char temp_name[12];
    snprintf(temp_name, sizeof(temp_name), "_t%d", temp_counter++);
    return insert_Symbol(temp_name, TEMPORARY_T);
}

quad* emit(opcode op, expr* result, expr* arg1, expr* arg2, unsigned int label, unsigned int line) {
    if (currquad == totalquads) {
        totalquads += EXPAND_SIZE;
        quads = realloc(quads, totalquads * sizeof(quad));
        if (!quads) { MemoryFail(); }
    }
    quad* new = (quad*)malloc(sizeof(quad));
    if(!new) { MemoryFail(); }
    new->op = op;
    new->arg1 = arg1;
    new->arg2 = arg2;
    new->result = result;
    new->label = label;
    new->line = line;
    quads[currquad++] = *new;
    return new;
}

/*===============================================================================================*/

const char* opcodeToStr(opcode op) {
    switch (op) {
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
        case OP_IFEQ: return "IFEQ";
        case OP_IFNOTEQ: return "IFNOTEQ";
        case OP_IFLESSEQ: return "IFLESSEQ";
        case OP_IFGREATEREQ: return "IFGREATEREQ";
        case OP_IFLESS: return "IFLESS";
        case OP_IFGREATER: return "IFGREATER";
        case OP_CALL: return "CALL";
        case OP_PARAM: return "PARAM";
        case OP_RET: return "RET";
        case OP_GETRETVAL: return "GETRETVAL";
        case OP_FUNCSTART: return "FUNCSTART";
        case OP_FUNCEND: return "FUNCEND";
        case OP_TABLECREATE: return "TABLECREATE";
        case OP_TABLEGETELEM: return "TABLEGETELEM";
        case OP_TABLESETELEM: return "TABLESETELEM";
        default: return "ERROR";
    }
}

const char* exprToStr(expr* e) {
    char* buffer = (char*)malloc(1024*sizeof(char));
    if (!e) return "NULL";
    switch (e->type) {
        case EXP_VARIABLE:
        case EXP_PROGRAMFUNC:
        case EXP_LIBRARYFUNC:
        case EXP_ARITH:
        case EXP_BOOL:
        case EXP_ASSIGN:
            return e->symbol ? e->symbol->name : "unknown_symbol";
        case EXP_CONSTNUMBER:
            snprintf(buffer, sizeof(buffer), "%.2f", e->numConst);
            return buffer;
        case EXP_CONSTSTRING:
            return e->stringConst ? e->stringConst : (char*)"\"\"";
        case EXP_CONSTBOOL:
            return e->boolConst ? (char*)"true" : (char*)"false";
        case EXP_NIL:
            return "NIL";
        case EXP_TABLEITEM:
        case EXP_NEWTABLE:
        default:
            return "ERROR";
    }
}

void printQuads(void) {
    printf("\n%-4s %-14s %-15s %-15s %-15s %-5s\n", "#", "OP", "RESULT", "ARG1", "ARG2", "LABEL");
    printf("-------------------------------------------------------------------------------\n");

    for (unsigned int i = 0; i < currquad; ++i) {
        printf("%-4u %-14s %-15s %-15s %-15s",
        i, opcodeToStr(quads[i].op),
        exprToStr(quads[i].result), exprToStr(quads[i].arg1), exprToStr(quads[i].arg2));
        if(quads[i].op == OP_IFEQ || quads[i].op == OP_IFNOTEQ || quads[i].op == OP_IFLESS ||
        quads[i].op == OP_IFGREATER || quads[i].op == OP_IFLESSEQ || quads[i].op == OP_IFGREATEREQ)
        { printf(" %-5u\n", quads[i].label); }
        else { printf("\n"); }
    }

    printf("-------------------------------------------------------------------------------\n\n");
}

/* end of alpha_quads.c */
