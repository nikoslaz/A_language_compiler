%{
    /* parser.y */
    /**
     * @authors nikos , nikoletta , mihalis
     */
    #include "table.h"
    #include "quads.h"
    #include "target.h"

    /* Globals */
    int inFunction = 0;
    int hasError = 0;
%}

%start program

%union {
	char*	stringZoumi;
	int		intZoumi;
	double	realZoumi;
    struct Symbol* symbolZoumi;
    struct expr_s* exprZoumi;  
    struct LoopContext* loopZoumi;
    unsigned int quadLabelZoumi;
}

%token <stringZoumi> ID
%token <stringZoumi> STRING
%token <intZoumi> INT
%token <realZoumi> REAL

%token IF
%token ELSE
%token WHILE
%token FOR
%token FUNCTION
%token RETURN
%token BREAK
%token CONTINUE
%token AND
%token OR
%token NOT
%token LOCAL
%token TRUE
%token FALSE
%token NIL
%token EQUALS
%token PLUS
%token MINUS
%token MULT
%token DIV
%token MOD
%token EQUALS_EQUALS
%token NOT_EQUALS
%token PLUS_PLUS
%token MINUS_MINUS
%token GREATER
%token LESS
%token GREATER_EQUAL
%token LESS_EQUAL
%token LEFT_BRACE
%token RIGHT_BRACE
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS
%token SEMICOLON
%token COMMA
%token COLON
%token COLON_COLON
%token PERIOD
%token PERIOD_PERIOD

%type <exprZoumi> funcdef

%type <exprZoumi> expr       
%type <exprZoumi> term       
%type <exprZoumi> assignexpr 
%type <exprZoumi> primary    
%type <exprZoumi> lvalue 
%type <exprZoumi> member 
%type <exprZoumi> objectdef
%type <exprZoumi> indexed
%type <exprZoumi> indexed_list
%type <exprZoumi> indexedelem
%type <exprZoumi> const
%type <exprZoumi> elist
%type <exprZoumi> elist_list

%type <exprZoumi> call
%type <exprZoumi> callsuffix
%type <exprZoumi> normcall
%type <exprZoumi> methodcall

/* For local count (offsett) */
%type <intZoumi> block

%type <quadLabelZoumi> M  /* Mark */
%type <quadLabelZoumi> MJ /* Mark and Jump */
%type <exprZoumi> F /* Function Name */
%type <loopZoumi> L /* Loop COntext */ 

%type <exprZoumi> ifcond
%type <exprZoumi> whilecond
%type <exprZoumi> forprefix
/* PROTERAIOTHTES KAI PROSETAIRISTIKOTHTA */

%right EQUALS
%left OR
%left AND

%nonassoc EQUALS_EQUALS NOT_EQUALS
%nonassoc GREATER GREATER_EQUAL LESS LESS_EQUAL

%left PLUS MINUS
%left MULT DIV MOD

%right NOT MINUS_MINUS PLUS_PLUS

%right UMINUS_CONFLICT
%nonassoc ELIST_INDEXED_CONFLICT
%nonassoc THEN_CONFLICT
%nonassoc ELSE

%left PERIOD PERIOD_PERIOD

%left RIGHT_BRACKET LEFT_BRACKET

%left LEFT_PARENTHESIS RIGHT_PARENTHESIS

%%

program:
    stmt_list
    ;

stmt:
    // Simple Garbage Collection
    // The result expr*($1) is calculated but not used further in this context.
    expr SEMICOLON {
        if($1 && $1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            freeIfTemp(mysym);
        }
        freeIfTemp($1);
    }
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | BREAK SEMICOLON {
        if(!loop_stack) { 
            yyerror("Use of 'break' outside loop"); 
        } else {
            add_to_breakList(nextquad());
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    }
    | CONTINUE SEMICOLON {
        if(!loop_stack) {
            yyerror("Use of 'continue' outside loop"); 
        } else {
            add_to_continueList(nextquad());
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    }
    | block
    | funcdef
    | SEMICOLON
    ;

// Simple Garbage Collection
// Reset all temporary vars after each complete statement
stmt_list:
    stmt { reset_temps(); } stmt_list
    |
    ;

// Simple Garbage Collection
// Use $1 or $3 if already temp, free $3 if not needed
expr:
    assignexpr { $$ = $1; }
    | expr PLUS expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr PLUS expr)");
            $$ = NULL;
        } else {
            Symbol* res_sym;
            if(hasTempSymbol($1)) {
                res_sym = $1->symbol;
                freeIfTemp($3);
            } else if(hasTempSymbol($3)) {
                res_sym = $3->symbol;
            } else {
                res_sym = get_temp_symbol();
            }
            expr* expr_temp = create_arith_expr(res_sym);
            emit(OP_ADD, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr MINUS expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr MINUS expr)");
            $$ = NULL;
        } else {
            Symbol* res_sym;
            if(hasTempSymbol($1)) {
                res_sym = $1->symbol;
                freeIfTemp($3);
            } else if(hasTempSymbol($3)) {
                res_sym = $3->symbol;
            } else {
                res_sym = get_temp_symbol();
            }
            expr* expr_temp = create_arith_expr(res_sym);
            emit(OP_SUB, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr MULT expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr MULT expr)");
            $$ = NULL;
        } else {
            Symbol* res_sym;
            if(hasTempSymbol($1)) {
                res_sym = $1->symbol;
                freeIfTemp($3);
            } else if(hasTempSymbol($3)) {
                res_sym = $3->symbol;
            } else {
                res_sym = get_temp_symbol();
            }
            expr* expr_temp = create_arith_expr(res_sym);
            emit(OP_MUL, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr DIV expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr DIV expr)");
            $$ = NULL;
        } else {
            Symbol* res_sym;
            if(hasTempSymbol($1)) {
                res_sym = $1->symbol;
                freeIfTemp($3);
            } else if(hasTempSymbol($3)) {
                res_sym = $3->symbol;
            } else {
                res_sym = get_temp_symbol();
            }
            expr* expr_temp = create_arith_expr(res_sym);
            emit(OP_DIV, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }

    }
    | expr MOD expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr MOD expr)");
            $$ = NULL;
        } else {
            Symbol* res_sym;
            if(hasTempSymbol($1)) {
                res_sym = $1->symbol;
                freeIfTemp($3);
            } else if(hasTempSymbol($3)) {
                res_sym = $3->symbol;
            } else {
                res_sym = get_temp_symbol();
            }
            expr* expr_temp = create_arith_expr(res_sym);
            emit(OP_MOD, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr GREATER expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr GREATER expr)");
            $$ = NULL;
        } else {
            /* Return an empty bool expr with appropriate true/false lists */
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFGREATER, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            freeIfTemp($1);
            freeIfTemp($3);
            $$ = expr_temp;
        }
    }
    | expr LESS expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr LESS expr)");
            $$ = NULL;
        } else {
            /* Return an empty bool expr with appropriate true/false lists */
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFLESS, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            freeIfTemp($1);
            freeIfTemp($3);
            $$ = expr_temp;
        }
    }
    | expr GREATER_EQUAL expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr GREATER_EQUAL expr)");
            $$ = NULL;
        } else {
            /* Return an empty bool expr with appropriate true/false lists */
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFGREATEREQ, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            freeIfTemp($1);
            freeIfTemp($3);
            $$ = expr_temp;
        }
    }
    | expr LESS_EQUAL expr {
        if(!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER || $1->type == EXP_TABLEITEM)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER || $3->type == EXP_TABLEITEM)) {
            yyerror("Error: Invalid arguments(expr LESS_EQUAL expr)");
            $$ = NULL;
        } else {
            /* Return an empty bool expr with appropriate true/false lists */
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFLESSEQ, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            freeIfTemp($1);
            freeIfTemp($3);
            $$ = expr_temp;
        }
    }
    | expr EQUALS_EQUALS expr {
        if($1 && $1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $1 = mysym;
        }
        if($3 && $3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $3 = mysym;
        }
        /* Return an empty bool expr with appropriate true/false lists */
        expr* expr_temp = create_empty_bool_expr();
        expr_temp->truelist  = makelist(nextquad());
        expr_temp->falselist = makelist(nextquad() + 1);
        emit(OP_IFEQ, NULL, $1, $3, -1);
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        freeIfTemp($1);
        freeIfTemp($3);
        $$ = expr_temp;
    }
    | expr NOT_EQUALS expr {
        if($1 && $1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $1 = mysym;
        }
        if($3 && $3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $3 = mysym;
        }
        /* Return an empty bool expr with appropriate true/false lists */
        expr* expr_temp = create_empty_bool_expr();
        expr_temp->truelist  = makelist(nextquad());
        expr_temp->falselist = makelist(nextquad() + 1);
        emit(OP_IFNOTEQ, NULL, $1, $3, -1);
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        freeIfTemp($1);
        freeIfTemp($3);
        $$ = expr_temp;
    }
    | expr AND {
        /* If expr is not already a boolean, run an equality check */
        if($1 && $1->type != EXP_BOOL) {
            $1->truelist = makelist(nextquad());
            $1->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $1, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    } M expr {
        /* If expr is not already a boolean, run an equality check */
        if($5 && $5->type != EXP_BOOL) {
            $5->truelist = makelist(nextquad());
            $5->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $5, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        /* Return an empty bool expr with appropriate true/false lists */
        expr* expr_temp = create_empty_bool_expr();
        if($1 && $5) {
            /* First TRUE expr must also check the second */
            backpatch($1->truelist, $4);
            expr_temp->truelist  = $5->truelist;
            /* Any FALSE expr leads to the whole result being FALSE */
            expr_temp->falselist = merge($1->falselist, $5->falselist);
        } 
        $$ = expr_temp;
    }
    | expr OR {
        /* If expr is not already a boolean, run an equality check */
        if($1 && $1->type != EXP_BOOL) {
            $1->truelist = makelist(nextquad());
            $1->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $1, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    } M expr {
        /* If expr is not already a boolean, run an equality check */
        if($5 && $5->type != EXP_BOOL) {
            $5->truelist = makelist(nextquad());
            $5->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $5, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        /* Return an empty bool expr with appropriate true/false lists */
        expr* expr_temp = create_empty_bool_expr();
        if($1 && $5) {
            /* First FALSE expr must check the second */
            backpatch($1->falselist, $4);
            expr_temp->truelist  = merge($1->truelist, $5->truelist);
            /* Any TRUE expr leads to the whole result being TRUE */
            expr_temp->falselist = $5->falselist;
        } 
        $$ = expr_temp;
    }
    | term { $$ = $1; }
    ;

// Simple Garbage Collection
// If already temp, use the same symbol
term:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS { $$ = $2; }
    | MINUS expr %prec UMINUS_CONFLICT {
        if(!$2 || !($2->type == EXP_VARIABLE || $2->type == EXP_ARITH || $2->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(MINUS expr)");
            $$ = NULL;
        } else {
            Symbol* res_sym;
            if(hasTempSymbol($2)) { res_sym = $2->symbol; }
            else { res_sym = get_temp_symbol(); }
            expr* expr_temp = create_arith_expr(res_sym);
            emit(OP_UMINUS, expr_temp, $2, NULL, 0);
            $$ = expr_temp;
        }
    }
    | NOT expr %prec NOT { 
        /* If expr is not already a boolean, run an equality check */
        if($2 && $2->type != EXP_BOOL) {
            $2->truelist = makelist(nextquad());
            $2->falselist = makelist(nextquad()+1);
            emit(OP_IFEQ, NULL, $2, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        /* Return an empty bool expr with flipped true/false lists */
        expr* expr_temp = create_empty_bool_expr();
        if($2) {
            expr_temp->truelist  = $2->falselist;
            expr_temp->falselist = $2->truelist;
        }
        $$ = expr_temp;
    }
    | PLUS_PLUS lvalue {
        if($2 && ($2->symbol->type == USERFUNC_T || $2->symbol->type == LIBFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else {
            expr* prevlval = $2;
            if($2) { $2 = emit_if_table_item_get($2, NULL); }
            emit(OP_ADD, $2, $2, create_constnum_expr(1), 0);
            expr* expr_sym;
            if($2 && $2->boolConst == 1) {
                /* boolConst is 1 if previous emit_if_table was true */
                emit_if_table_item_set(prevlval, $2);
                $2->boolConst = 0;
                expr_sym = $2;
            } else {
                expr_sym = create_var_expr(get_temp_symbol());
                emit(OP_ASSIGN, expr_sym, $2, NULL, 0);
            }
            $$ = expr_sym;
        }
    }
    | lvalue PLUS_PLUS {
        if($1 && ($1->symbol->type == USERFUNC_T || $1->symbol->type == LIBFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else {
            expr* prevlval = $1;
            expr* expr_sym = create_var_expr(get_temp_symbol());
            if($1) { $1 = emit_if_table_item_get($1, NULL); }
            emit(OP_ASSIGN, expr_sym, $1, NULL, 0);
            emit(OP_ADD, $1, $1, create_constnum_expr(1), 0);
            /* boolConst is 1 if previous emit_if_table was true */
            if($1 && $1->boolConst == 1) {
                emit_if_table_item_set(prevlval, $1);
                $1->boolConst = 0;
            }
            $$ = expr_sym;
        }
    }
    | MINUS_MINUS lvalue {
        if($2 && ($2->symbol->type == USERFUNC_T || $2->symbol->type == LIBFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else {
            expr* prevlval = $2;
            if($2) { $2 = emit_if_table_item_get($2, NULL); }
            emit(OP_SUB, $2, $2, create_constnum_expr(1), 0);
            expr* expr_sym;
            /* boolConst is 1 if previous emit_if_table was true */
            if($2 && $2->boolConst == 1) {
                emit_if_table_item_set(prevlval, $2);
                $2->boolConst = 0;
                expr_sym = $2;
            } else {
                expr_sym = create_var_expr(get_temp_symbol());
                emit(OP_ASSIGN, expr_sym, $2, NULL, 0);
            }
            $$ = expr_sym;
        }
    }
    | lvalue MINUS_MINUS {
        if($1 && ($1->symbol->type == USERFUNC_T || $1->symbol->type == LIBFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else {
            expr* prevlval = $1;
            expr* expr_sym = create_var_expr(get_temp_symbol());
            if($1) { $1 = emit_if_table_item_get($1, NULL); }
            emit(OP_ASSIGN, expr_sym, $1, NULL, 0);
            emit(OP_SUB, $1, $1, create_constnum_expr(1), 0);
            /* boolConst is 1 if previous emit_if_table was true */
            if($1 && $1->boolConst == 1) {
                emit_if_table_item_set(prevlval, $1);
                $1->boolConst = 0;
            }
            $$ = expr_sym;
        }
    }
    | primary { $$ = $1; }
    ;

assignexpr:
    lvalue EQUALS expr {
        if($1 && ($1->symbol->type == LIBFUNC_T || $1->symbol->type == USERFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else if ($1 && $3) {
            expr* rvalue;
            if($3->type == EXP_BOOL) {
                /* Create new temp bool expr */
                expr* mysym = create_bool_expr(get_temp_symbol());
                /* Truelist assigns TRUE to temp symbol */
                backpatch($3->truelist, nextquad());
                emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
                /* Then skips FALSE symbol */
                emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
                /* Falselist assigns FALSE to temp symbol */
                backpatch($3->falselist, nextquad());
                emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
                rvalue = mysym;
            } else { rvalue = $3; }
            emit_if_table_item_set($1, rvalue);
            expr* expr_result = create_var_expr(get_temp_symbol());
            /* boolConst is 1 if previous emit_if_table was true */
            if($1->boolConst == 1) {
                $1 = emit_if_table_item_get($1, expr_result);
                $1->boolConst = 0;
            } else {
                // First assign
                emit(OP_ASSIGN, $1, rvalue, NULL, 0);
                // Second assign
                emit(OP_ASSIGN, expr_result, $1, NULL, 0);
            }
            $$ = expr_result;
        } else {
            yyerror("Invalid assignment operation");
            $$ = NULL;
        }
    }
    ;

lvalue:
    ID { 
        Symbol* sym = resolve_RawSymbol($1);
        if(sym) {
            if(sym->type == USERFUNC_T) {
                $$ = create_prog_func_expr(sym);
            } else if(sym->type == LIBFUNC_T) {
                $$ = create_lib_func_expr(sym);
            } else {
                $$ = create_var_expr(sym);
            }
        } else { $$ = NULL; }
    }
    | LOCAL ID {
        Symbol* sym = resolve_LocalSymbol($2);
        if(sym) {
            if(sym->type == USERFUNC_T) {
                $$ = create_prog_func_expr(sym);
            } else if(sym->type == LIBFUNC_T) {
                $$ = create_lib_func_expr(sym);
            } else {
                $$ = create_var_expr(sym);
            }
        } else { $$ = NULL; }
    }
    | COLON_COLON ID {
        Symbol* sym = resolve_GlobalSymbol($2);
        if(sym) {
            if(sym->type == USERFUNC_T) {
                $$ = create_prog_func_expr(sym);
            } else if(sym->type == LIBFUNC_T) {
                $$ = create_lib_func_expr(sym);
            } else {
                $$ = create_var_expr(sym);
            }
        } else { $$ = NULL; }
    }
    | member { $$ = $1; }
    ;

const:
    INT { $$ = create_constnum_expr((double)$1); }
    | REAL { $$ = create_constnum_expr($1); }
    | STRING { $$ = create_conststring_expr(strdup($1)); }
    | NIL { $$ = create_nil_expr(); }
    | TRUE { $$ = create_constbool_expr(1); }
    | FALSE { $$ = create_constbool_expr(0); }
    ;

ifcond:
    IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {
        if($3 && $3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $3 = mysym;
        }
        /* THEN jump */
        emit(OP_IFEQ, NULL, $3, create_constbool_expr(1), nextquad()+2);
        if($3) {
            /* ELSE jump */
            $3->falselist = makelist(nextquad()); 
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        $$ = $3;
    }
    ;

ifstmt:
    ifcond stmt %prec THEN_CONFLICT {
        /* Make ELSE jump jump to after THEN_stmt */
        if($1) { backpatch($1->falselist, nextquad()); }
    }
    | ifcond stmt ELSE MJ stmt {
        /* Make ELSE jump jump to where ELSE_stmt begins */
        if($1) { backpatch($1->falselist, $4+1); }
        /* Make end of THEN_stmt jump to after ELSE_stmt ends */
        simplepatch($4, nextquad());
    }
    ;

whilecond:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {
        if($2 && $2->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($2->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($2->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);  
            $2 = mysym;
        }
        /* THEN jump */
        emit(OP_IFEQ, NULL, $2, create_constbool_expr(1), nextquad() + 2);
        if($2) {
            /* ELSE jump */
            $2->falselist = makelist(nextquad()); 
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        $$ = $2;
    }
    ;

whilestmt:
    WHILE M whilecond P stmt {
        /* Jump to cond */
        emit(OP_JUMP, NULL, NULL, NULL, $2);
        /* Make ELSE jump jump to after stmt ends */
        if($3) { backpatch($3->falselist, nextquad()); }
        /* Breaks & Continues */
        if(loop_stack) {
            backpatch(loop_stack->break_list, nextquad());
            backpatch(loop_stack->continue_list, $2);
        }
        pop_loop();
    }
    ;

forprefix:
    FOR LEFT_PARENTHESIS elist {
        // Simple Garbage Collection
        // Elist isn't used in this context after it has been evaluated
        while($3) {
            freeIfTemp($3);
            $3 = $3->next;
        }
    }
    M SEMICOLON expr SEMICOLON {
        if($7 && $7->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($7->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($7->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0); 
            $7 = mysym; 
        }
        if($7) {
            /* THEN Jump */
            $7->truelist = makelist(nextquad());
            emit(OP_IFEQ, NULL, $7, create_constbool_expr(1), -1);
            /* ELSE jump */
            $7->falselist = makelist(nextquad());
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            /* Store start of expr block in boolConst temporarily */
            $7->boolConst = $5;
        }
        $$ = $7;
    }
    ;

forstmt:
    forprefix M elist RIGHT_PARENTHESIS MJ P stmt MJ {
        if($1) {
            /* Make THEN jump jump to start of stmt */
            backpatch($1->truelist, $5+1);
            /* Make ELSE jump jump to after stmt */
            backpatch($1->falselist, nextquad());
            /* Jump to begin of expr (use marker from boolConst) */
            simplepatch($5, $1->boolConst); 
            $1->boolConst=0;
        }
        /* Jump to start of elist2 */
        simplepatch($8, $2);
        /* Breaks & Continues */
        if(loop_stack) {
            backpatch(loop_stack->break_list, nextquad());
            backpatch(loop_stack->continue_list, $2);
        }
        pop_loop();
    }
    ;

primary:
    lvalue { $$ = emit_if_table_item_get($1, NULL); }
    | call { $$ = $1; }
    | objectdef { $$ = $1; }
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS { $$ = $2; }
    | const { $$ = $1; }
    ;

call:
    call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS { 
        expr* result = create_var_expr(get_temp_symbol());
        if($1) {
            /* Only Functions or Dynamic Symbols allowed */
            if($1->type == EXP_PROGRAMFUNC ||  $1->type == EXP_LIBRARYFUNC || $1->type == EXP_VARIABLE || $1->type == EXP_TABLEITEM) {
                /* Arguments from right to left */
                handle_arguments($3);
                emit(OP_CALL, NULL, $1, NULL, 0);
                emit(OP_GETRETVAL, result, NULL, NULL, 0);
            } else {
                yyerror("Error. Symbol is NOT a function");
                result = NULL;
            }
        }
        $$ = result;
    }
    | lvalue callsuffix {
        expr* result = NULL;
        if($1) {
            $1 = emit_if_table_item_get($1, NULL);
            /* Only Functions or Dynamic Symbols allowed */
            if($1->type == EXP_PROGRAMFUNC ||  $1->type == EXP_LIBRARYFUNC || $1->type == EXP_VARIABLE || $1->type == EXP_TABLEITEM) {
                expr* table;
                if($2 && $2->boolConst == 1) {
                    $2->boolConst = 0;
                    /* boolConst indigates from method (table function) */
                    table = create_var_expr(get_temp_symbol());
                    char msg[1024];
                    /* Add "" to name of function, placed inside stringConst */
                    snprintf(msg, sizeof(msg), "\"%s\"", $2->stringConst);
                    $2->stringConst = msg;
                    emit(OP_TABLEGETELEM, table, $1, $2, 0);
                    /* Arguments from right to left */
                    handle_arguments($2->next);
                    emit(OP_PARAM, NULL, $1, NULL, 0);
                } else { table = $1; }
                emit(OP_CALL, NULL, table, NULL, 0);
                result = create_var_expr(get_temp_symbol());
                emit(OP_GETRETVAL, result, NULL, NULL, 0);
            } else {
                yyerror("Error. Symbol is NOT a function");
                result = NULL;
            }
        }
        $$ = result;
    }
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        expr* result;
        if($2) {
            /* Only Functions or Dynamic Symbols allowed */
            if($2->type == EXP_PROGRAMFUNC ||  $2->type == EXP_LIBRARYFUNC || $2->type == EXP_VARIABLE || $2->type == EXP_TABLEITEM) {
                /* Arguments from right to left */
                handle_arguments($5);
                emit(OP_CALL, NULL, $2, NULL, 0);
                result = create_var_expr(get_temp_symbol());
                emit(OP_GETRETVAL, result, NULL, NULL, 0);
            } else {
                yyerror("Error. Symbol is NOT a function");
                result = NULL;
            }
        }
        $$ = result;
    }
    ;

/* Use boolConst temporarily to differentiate between call options */ 
callsuffix:
    normcall { $$ = NULL; }
    | methodcall { 
        if($1) {
            $1->boolConst = 1;
            $$ = $1;
        } else {
            $$ = NULL;
        }
    }
    ;

normcall:
    LEFT_PARENTHESIS elist RIGHT_PARENTHESIS { handle_arguments($2); }
    ;

elist: 
    expr {
        if($1 && $1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $1 = mysym;
        }
    } elist_list { 
        if($1) {
            $1->next = $3;
            $$ = $1;
        } else { $$ = NULL; }
    }
    | { $$ = NULL; }
    ;

elist_list: 
    COMMA expr {
        if($2 && $2->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($2->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($2->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $2 = mysym;
        }
    } elist_list { 
        if($2) {
            $2->next = $4;
            $$ = $2;
        } else { $$ = NULL; } 
    }
    | { $$ = NULL; }
    ;

funcdef:
    FUNCTION ID F MJ LEFT_PARENTHESIS {
        expr* res = create_prog_func_expr(resolve_FuncSymbol($2));
        /* Save Address */
        if(res && res->symbol) { res->symbol->quad_addr = nextquad(); }
        emit(OP_FUNCSTART, NULL, res, NULL, 0);
        /* Store function symbol */
        if($3) { $3->symbol = res->symbol; }
        fromFunct = 1;
        inFunction++;
        /* Return List */
        push_return();
        enter_Next_Scope(1);
    } idlist { int_to_Scope(ht->currentScope)->scopeOffset = 0; } RIGHT_PARENTHESIS block {
        /* Patch return list */
        if(return_stack) { backpatch(return_stack->return_list, nextquad()); }
        /* Calculate function total offset */
        if($3 && $3->symbol) { $3->symbol->num_locals = $10; }
        emit(OP_FUNCEND, NULL, $3, NULL, 0);
        fromFunct = 0;
        inFunction--;
        /* Patch jump to skip funcdef */
        simplepatch($4, nextquad());
        pop_return();
        $$ = $3;
    }
    | FUNCTION F MJ LEFT_PARENTHESIS {
        expr* sym = create_prog_func_expr(resolve_AnonymousFunc());
        /* Save Address */
        if(sym && sym->symbol) { sym->symbol->quad_addr = nextquad(); }
        emit(OP_FUNCSTART, NULL, sym, NULL, 0);
        /* Store function symbol */
        if($2) { $2->symbol = sym->symbol; }
        fromFunct = 1;
        inFunction++;
        /* Return List */
        push_return();
        enter_Next_Scope(1);
    } idlist { int_to_Scope(ht->currentScope)->scopeOffset = 0; } RIGHT_PARENTHESIS block {
        /* Patch return list */
        if(return_stack) { backpatch(return_stack->return_list, nextquad()); }
        /* Calculate function total offset */
        if($2 && $2->symbol) { $2->symbol->num_locals = $9; }
        emit(OP_FUNCEND, NULL, $2, NULL, 0);
        fromFunct = 0;
        inFunction--;
        /* Patch jump to skip funcdef */
        simplepatch($3, nextquad());
        pop_return();
        $$ = $2;
    }
    ;

idlist:
    ID { resolve_FormalSymbol($1); } idlist_list
    |
    ;

idlist_list:
    COMMA ID { resolve_FormalSymbol($2); } idlist_list
    |
    ;

returnstmt:
    RETURN SEMICOLON {
        if(!inFunction) { yyerror("Use of 'return' outside a function"); }
        else {
            /* Return Jump */
            emit(OP_RETURN, NULL, NULL, NULL, 0);
            add_to_return_list(nextquad());
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    }
    | RETURN expr SEMICOLON {
        if(!inFunction) {
            yyerror("Use of 'return' outside a function");
        } else {
            if($2 && $2->type == EXP_BOOL) {
                /* Create new temp bool expr */
                expr* mysym = create_bool_expr(get_temp_symbol());
                /* Truelist assigns TRUE to temp symbol */
                backpatch($2->truelist, nextquad());
                emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
                /* Then skips FALSE symbol */
                emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
                /* Falselist assigns FALSE to temp symbol */
                backpatch($2->falselist, nextquad());
                emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
                /* Return Jump */
                emit(OP_RETURN, mysym, NULL, NULL, 0);
                add_to_return_list(nextquad());
                emit(OP_JUMP, NULL, NULL, NULL, -1);
            } else {
                /* Return Jump */
                emit(OP_RETURN, $2, NULL, NULL, 0);
                add_to_return_list(nextquad());
                emit(OP_JUMP, NULL, NULL, NULL, -1);
            }
        }
    }
    ;

block:
    LEFT_BRACE L {
        /* If I am a function block, reset loop_stack */
        if(!fromFunct) { enter_Next_Scope(0); }
        else { loop_stack = NULL; }
        fromFunct=0;
    } stmt_list RIGHT_BRACE {
        int offset = int_to_Scope(ht->currentScope)->scopeOffset;
        exit_Current_Scope();
        /* Restore loop_stack */
        loop_stack = $2;
        $$ = offset;
    }
    ;

objectdef:
    LEFT_BRACKET elist RIGHT_BRACKET {
        expr* new_table = create_table_expr(get_temp_symbol());
        emit(OP_TABLECREATE, new_table, NULL, NULL, 0);
        expr* elist_temp = $2;
        /* Create integer indices */
        unsigned int i = 0;
        while(elist_temp) {
            emit(OP_TABLESETELEM, new_table, create_constnum_expr((double)i++), elist_temp, 0);
            elist_temp = elist_temp->next;
        }
        $$ = new_table;
    }
    | LEFT_BRACKET indexed RIGHT_BRACKET %prec ELIST_INDEXED_CONFLICT {
        expr* new_table = create_table_expr(get_temp_symbol());
        emit(OP_TABLECREATE, new_table, NULL, NULL, 0);
        expr* elist_temp = $2;
        while(elist_temp) {
            emit(OP_TABLESETELEM, new_table, elist_temp->index, elist_temp, 0);
            elist_temp = elist_temp->next;
        }
        $$ = new_table;
    }
    ;

indexedelem:
    LEFT_BRACE expr COLON expr RIGHT_BRACE {
        if($2 && $2->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($2->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($2->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $2 = mysym;
        }
        if($4 && $4->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($4->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($4->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $4 = mysym;
        }
        $$ = $4;
        if($$) {
            $$->index = $2;
            $$->next = NULL;
        }
    }
    ;

indexed:
    indexedelem indexed_list
    { /* Indexed is only used inside objectdef, and its empty rule is already
    covered by the elist empty rule, thus we are able to remove it from here,
    which also removes the warning message Bison generates :D */
        $$ = $1;
        if($$) { $$->next = $2; }
    }
    ;

indexed_list:
    COMMA indexedelem indexed_list {
        $$ = $2;
        if($$) { $$->next = $3; }
    }
    | { $$ = NULL; }
    ;

methodcall:
    PERIOD_PERIOD ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        $$ = create_conststring_expr($2);
        if($$) { $$->next = $4; }
    }
    ;

member: 
    lvalue PERIOD ID { 
        $1 = emit_if_table_item_get($1, NULL);
        if($1) {
            $1->index = create_conststring_expr($3);
            $1->type = EXP_TABLEITEM;
            char* msg = (char*)malloc(128);
            /* Add "" to index and store it in stringConst temporarily */
            sprintf(msg, "\"%s\"", $1->index->stringConst);
            $1->index->stringConst = msg;  
        } else {
            yyerror("Error. NULL lvalue in MEMBER");
        }
        $$ = $1;
    }
    | lvalue LEFT_BRACKET expr RIGHT_BRACKET { 
        $1 = emit_if_table_item_get($1, NULL);
        if($3 && $3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $3 = mysym;
        }
        if($1) {
            $1->type = EXP_TABLEITEM;
            $1->index = $3;
        } else {
            yyerror("Error. NULL lvalue in MEMBER");
        }
        $$ = $1;
    }
    | call PERIOD ID {
        $1 = emit_if_table_item_get($1, NULL);
        if($1) {
            $1->index = create_conststring_expr($3);
            $1->type = EXP_TABLEITEM;
            char* msg = (char*)malloc(128);
            /* Add "" to index and store it in stringConst temporarily */
            sprintf(msg, "\"%s\"", $1->index->stringConst);
            $1->index->stringConst = msg;  
        } else {
            yyerror("Error. NULL lvalue in MEMBER");
        }
        $$ = $1;
    }
    | call LEFT_BRACKET expr RIGHT_BRACKET {
        $1 = emit_if_table_item_get($1, NULL);
        if($3 && $3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr(get_temp_symbol());
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
            $3 = mysym;
        }
        if($1 && $3 && $3->symbol) {
            $1->index = create_conststring_expr($3->symbol->name);
            $1->type = EXP_TABLEITEM;
            char* msg = (char*)malloc(128);
            /* Add "" to index and store it in stringConst temporarily */
            sprintf(msg, "\"%s\"", $1->index->stringConst);
            $1->index->stringConst = msg;  
        } else {
            yyerror("Error. NULL lvalue in MEMBER");
        }
        $$ = $1;
    }
    ;

/* Mark */
M: {  $$ = nextquad(); }

/* Mark and Jump */
MJ: { $$ = nextquad(); emit(OP_JUMP, NULL, NULL, NULL, nextquad()); }

/* Function Name */
F: { $$ = create_prog_func_expr(NULL); }

/* Loop Context */ 
L: { $$ = loop_stack; }
P: { push_loop(); }

%%

int main(int argc, char** argv) {

    Initialize_HashTable();
    initialize_temp_array();

    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { yyin = stdin; }
 
    /* Redirect output of Bison */
    yyout = fopen("/dev/null", "w");  
    yyparse();
    fclose(yyout);

    /* Comments not closed properly */
    if(comment_depth > 0){
        printf("Error! The comment in line %d is not closed properly.\n", comment_startlines[comment_top]);
        exit(1);
    }

    if(!hasError) {

        /* Print SymTable */
        printf("\n           ======= Syntax Analysis =======\n");
        print_SymTable();
    
        /* Print Quads */
        printf("\n           ======= Intermediate Code =======\n");
        printQuads();
        /* FILE* fd;
        if(!(fd = fopen("quads.output", "w"))) {
            fprintf(stderr, "Cannot create quads.output file\n");
            return 1;
        }
        printQuadsToFile(fd); */
        
        generateTarget();
        printTargetToFile();
        write_binary();

    } else { printf("\nTarget code generation failed:(\n"); }

    /* Return Normally */
    free_HashTable();
    freeTokenList(&root);

    return 0;
}

/* end of parser.y */