%{
    /* parser.y */
    /**
     * @authors nikos , nikoletta , mihalis
     */
    #include "table.h"
    #include "alpha_quads.h"
    struct Symbol* tmp;

    /* Globals */
    //int inLoop = 0;
    int inFunction = 0;
%}

%start program

%union {
	char*	stringZoumi;
	int		intZoumi;
	double	realZoumi;
    struct Symbol* symbolZoumi;
    struct expr_s* exprZoumi;  
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
%type <exprZoumi> const
%type <exprZoumi> elist
%type <exprZoumi> elist_list

%type <symbolZoumi> call
%type <intZoumi> normcall

%type <quadLabelZoumi> M  /* Mark */
%type <quadLabelZoumi> MJ /* Mark and Jump */

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
    expr SEMICOLON {
        if($1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
        }
    }
    ;
    // MAYBE GARBAGE COLLECTION HERE ????????
    // The result expr* ($1) is calculated but not used further in this context.
    // If $1 represents a temporary result that's not otherwise used,
    // you might consider freeing it here, but be extremely careful.
    // For now, let's assume memory management is handled elsewhere or ignored.
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | BREAK SEMICOLON {
        if(loop_depth_counter == 0){ 
            yyerror("Use of 'break' outside loop"); 
        } else {
            add_to_breakList(nextquad());
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    }
    | CONTINUE SEMICOLON {
        if(loop_depth_counter == 0){
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

stmt_list:
    stmt stmt_list
    |
    ;

// MAYBE GARBAGE COLLECTION HERE ????????
// Free $1, $3 if they were temp const results
expr:
    assignexpr { $$ = $1; }
    | expr PLUS expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr PLUS expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_arith_expr();
            emit(OP_ADD, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr MINUS expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr MINUS expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_arith_expr();
            emit(OP_SUB, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr MULT expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr MULT expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_arith_expr();
            emit(OP_MUL, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr DIV expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr DIV expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_arith_expr();
            emit(OP_DIV, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }

    }
    | expr MOD expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr MOD expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_arith_expr();
            emit(OP_MOD, expr_temp, $1, $3, 0);
            $$ = expr_temp;
        }
    }
    | expr GREATER expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr GREATER expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFGREATER, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            $$ = expr_temp;
        }
    }
    | expr LESS expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr LESS expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFLESS, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            $$ = expr_temp;
        }
    }
    | expr GREATER_EQUAL expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr GREATER_EQUAL expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFGREATEREQ, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            $$ = expr_temp;
        }
    }
    | expr LESS_EQUAL expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr LESS_EQUAL expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_empty_bool_expr();
            expr_temp->truelist  = makelist(nextquad());
            expr_temp->falselist = makelist(nextquad() + 1);
            emit(OP_IFLESSEQ, NULL , $1 , $3 , -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
            $$ = expr_temp;
        }
    }
    | expr EQUALS_EQUALS expr {
        if($1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
        }
        if($3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
        }
        expr* expr_temp = create_empty_bool_expr();
        expr_temp->truelist  = makelist(nextquad());
        expr_temp->falselist = makelist(nextquad() + 1);
        emit(OP_IFEQ, NULL, $1, $3, -1);
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        $$ = expr_temp;
    }
    | expr NOT_EQUALS expr {
        if($1->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
            /* Truelist assigns TRUE to temp symbol */
            backpatch($1->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($1->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
        }
        if($3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
            /* Truelist assigns TRUE to temp symbol */
            backpatch($3->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($3->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
        }
        expr* expr_temp = create_empty_bool_expr();
        expr_temp->truelist  = makelist(nextquad());
        expr_temp->falselist = makelist(nextquad() + 1);
        emit(OP_IFNOTEQ, NULL, $1, $3, -1);
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        $$ = expr_temp;
    }
    | expr AND {
        if($1->type != EXP_BOOL) {
            $1->truelist = makelist(nextquad());
            $1->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $1, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    } M expr {
        if($5->type != EXP_BOOL) {
            $5->truelist = makelist(nextquad());
            $5->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $5, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        expr* expr_temp = create_empty_bool_expr();
        backpatch($1->truelist, $4);
        expr_temp->truelist  = $5->truelist;
        expr_temp->falselist = merge($1->falselist, $5->falselist);
        $$ = expr_temp;
    }
    | expr OR {
        if($1->type != EXP_BOOL) {
            $1->truelist = makelist(nextquad());
            $1->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $1, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
    } M expr {
        if($5->type != EXP_BOOL) {
            $5->truelist = makelist(nextquad());
            $5->falselist = makelist(nextquad() + 1);
            emit(OP_IFEQ, NULL, $5, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        expr* expr_temp = create_empty_bool_expr();
        backpatch($1->falselist, $4);
        expr_temp->truelist  = merge($1->truelist, $5->truelist);
        expr_temp->falselist = $5->falselist;
        $$ = expr_temp;
    }
    | term 
    ;

term:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS { $$ = $2; }
    | MINUS expr %prec UMINUS_CONFLICT {
        if(!$2 || !($2->type == EXP_VARIABLE || $2->type == EXP_ARITH || $2->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(MINUS expr)\n");
            $$ = NULL;
        } else {
            expr* expr_temp = create_arith_expr();
            emit(OP_UMINUS, expr_temp, $2, NULL, 0);
            $$ = expr_temp;
        }
    }
    | NOT expr %prec NOT { 
        expr* e2 = $2;
        if(e2->type != EXP_BOOL) {
            e2->truelist = makelist(nextquad());
            e2->falselist = makelist(nextquad()+1);
            emit(OP_IFEQ, NULL, e2, create_constbool_expr(1), -1);
            emit(OP_JUMP, NULL, NULL, NULL, -1);
        }
        expr* expr_temp = create_empty_bool_expr();
        expr_temp->truelist  = e2->falselist;
        expr_temp->falselist = e2->truelist;
        $$ = expr_temp;
     }
    | PLUS_PLUS lvalue {
        if($2 && ($2->symbol->type == USERFUNC_T || $2->symbol->type == LIBFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else {
            emit(OP_ADD, $2, $2, create_constnum_expr(1), 0);
            expr* expr_sym = create_var_expr(create_temp_symbol());
            emit(OP_ASSIGN, expr_sym, $2, NULL, 0);
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
            expr* expr_sym = create_var_expr(create_temp_symbol());
            emit(OP_ASSIGN, expr_sym, $1, NULL, 0);
            emit(OP_ADD, $1, $1, create_constnum_expr(1), 0);
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
            emit(OP_SUB, $2, $2, create_constnum_expr(1), 0);
            expr* expr_sym = create_var_expr(create_temp_symbol());
            emit(OP_ASSIGN, expr_sym, $2, NULL, 0);
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
            expr* expr_sym = create_var_expr(create_temp_symbol());
            emit(OP_ASSIGN, expr_sym, $1, NULL, 0);
            emit(OP_SUB, $1, $1, create_constnum_expr(1), 0);
            $$ = expr_sym;
        }
    }
    | primary { $$ = $1; }
    ;

assignexpr:
    lvalue EQUALS expr {
        if($1 && ($1->symbol->type == LIBFUNC_T || $1->symbol->type == LIBFUNC_T)) {
            char msg[1024];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else if ($1 && $3) {
            expr* rvalue;
            if($3->type == EXP_BOOL) {
                /* Create new temp bool expr */
                expr* mysym = create_bool_expr();
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
            expr* expr_result = create_var_expr(create_temp_symbol());
            // First assign
            emit(OP_ASSIGN, $1, $3, NULL, 0);
            // Second assign
            emit(OP_ASSIGN, expr_result, $1, NULL, 0);
            $$ = expr_result;
        } else {
            yyerror("Invalid assignment operation");
            $$ = NULL;
        }
    }
    ;

lvalue:
    ID {
        int inaccessible = 0;
        Symbol* sym = lookUp_All($1, &inaccessible);
        if(sym == NULL) {
            if(inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $1);
                yyerror(msg);
                $$ = NULL;
            } else {
                Symbol* symbol_temp = resolve_RawSymbol($1);
                expr* expr_temp = create_var_expr(symbol_temp);
                $$ = expr_temp;
            }
        } else {            
            expr* expr_temp = create_var_expr(sym);
            $$ = expr_temp;
        }
    }
    | LOCAL ID {
        Symbol* symbol_temp = resolve_LocalSymbol($2);
        expr* expr_temp = create_var_expr(symbol_temp);
        $$ = expr_temp;
    }
    | COLON_COLON ID {
        Symbol* symbol_temp = resolve_GlobalSymbol($2);
        expr* expr_temp = create_var_expr(symbol_temp);
        $$ = expr_temp;
    }
    | member
    ;

const:
    INT {
        expr* expr_temp = create_constnum_expr((double)$1);
        $$ = expr_temp;
    }
    | REAL {
        expr* expr_temp = create_constnum_expr($1);
        $$ = expr_temp;
    }
    | STRING {
        expr* expr_temp = create_conststring_expr(strdup($1));
        $$ = expr_temp;
    }
    | NIL {
        expr* expr_temp = create_nil_expr();
        $$ = expr_temp;
    }
    | TRUE {
        expr* expr_temp = create_constbool_expr(1);
        $$ = expr_temp;
    }
    | FALSE {
        expr* expr_temp = create_constbool_expr(0);
        $$ = expr_temp;
    }
    ;

ifcond:
    IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {
        if($3->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
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
        /* Store ELSE jump quad */
        $3->falselist = makelist(nextquad());
        /* ELSE jump */
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        $$ = $3;
    }
    ;

ifstmt:
    ifcond stmt %prec THEN_CONFLICT {
        /* Make ELSE jump to after THEN_stmt */
        backpatch($1->falselist, nextquad());
    }
    | ifcond stmt ELSE MJ stmt {
        /* Make ELSE jump to where ELSE_stmt begins */
        backpatch($1->falselist, $4+1);
        /* Make end of THEN_stmt jump to after ELSE_stmt ends */
        simplepatch($4, nextquad());
    }
    ;

whilecond:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {
        if($2->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
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
        /* Store ELSE jump quad */
        $2->falselist = makelist(nextquad());
        /* ELSE jump */
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        $$ = $2;
    }
    ;

whilestmt:
    WHILE M whilecond P stmt {
        /* Jump to cond */
        emit(OP_JUMP, NULL, NULL, NULL, $2);
        /* Make ELSE jump to after stmt ends */
        backpatch($3->falselist, nextquad());
        /* Breaks & Continues */
        if(loop_stack) {
            backpatch(loop_stack->break_list, nextquad());
            backpatch(loop_stack->continue_list, $2);
        }
        pop();
    }
    ;

forprefix:
    FOR LEFT_PARENTHESIS elist M SEMICOLON expr SEMICOLON {
        if($6->type == EXP_BOOL) {
            /* Create new temp bool expr */
            expr* mysym = create_bool_expr();
            /* Truelist assigns TRUE to temp symbol */
            backpatch($6->truelist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
            /* Then skips FALSE symbol */
            emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
            /* Falselist assigns FALSE to temp symbol */
            backpatch($6->falselist, nextquad());
            emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0); 
            $6 = mysym; 
        }
        /* THEN Jump */
        $6->truelist = makelist(nextquad());
        emit(OP_IFEQ, NULL, $6, create_constbool_expr(1), -1);
        /* ELSE jump */
        $6->falselist = makelist(nextquad());
        emit(OP_JUMP, NULL, NULL, NULL, -1);
        /* Store start of expr block in boolConst temporariliy */
        $6->boolConst = $4;
        $$ = $6;
    }

forstmt:
    forprefix M elist RIGHT_PARENTHESIS MJ P stmt MJ {
        /* Make THEN jump to start of stmt */
        backpatch($1->truelist, $5+1);
        /* Make ELSE jump to after stmt */
        backpatch($1->falselist, nextquad());
        /* Jump to begin of expr */
        simplepatch($5, $1->boolConst); 
        $1->boolConst=0;
        /* Jump to start of elist2 */
        simplepatch($8, $2);
        /* Breaks & Continues */
        if(loop_stack) {
            backpatch(loop_stack->break_list, nextquad());
            backpatch(loop_stack->continue_list, $2);
        }
        pop();
    }
    ;

primary:
    lvalue { $$ = $1; }
    | call { $$ = $1; }
    | objectdef 
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS { $$ = $2; }
    | const { $$ = $1; }
    ;

/* TODO: Implement TABLEGETELEM/TABLESETELEM logic here */
member: 
    lvalue PERIOD ID { $$ = $1; }
    | lvalue LEFT_BRACKET expr RIGHT_BRACKET { $$ = $1; }
    | call PERIOD ID { $$ = $1; }
    | call LEFT_BRACKET expr RIGHT_BRACKET { $$ = $1; }
    ;

/* TODO: functions */
call:
    call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS { $$ = $1; }
    | lvalue callsuffix {
        int inaccessible = 0;
        Symbol* sym;
        if(!$1) {
            sym = NULL;
        } else { sym = lookUp_All($1->symbol->name, &inaccessible); }
        $$ = NULL;
    }
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        Symbol* sym = handleAnonymousFuncCall($2->symbol);
        if(sym) {
            $$ = create_temp_symbol();
        } else {
            $$ = NULL;
        }
    }
    ;

callsuffix:
    normcall
    | methodcall
    ;

normcall:
    LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        //$$ = $2;
    }
    ;

methodcall:
    PERIOD_PERIOD ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;

elist: 
    expr elist_list { 
        if($1) {
            $1->next = $2;
            $$ = $1;
        }
        else { $$ = NULL; }
    }
    | { $$ = NULL; }
    ;

elist_list: 
    COMMA expr elist_list { 
        if($2) {
            $2->next = $3;
            $$ = $2;
        } else { $$ = NULL; } 
    }
    | { $$ = NULL; }
    ;

objectdef:
    LEFT_BRACKET elist RIGHT_BRACKET
    | LEFT_BRACKET indexed RIGHT_BRACKET %prec ELIST_INDEXED_CONFLICT
    ;

indexedelem:
    LEFT_BRACE expr COLON expr RIGHT_BRACE
    ;

indexed:
    indexedelem indexed_list
    { /* Indexed is only used inside objectdef, and its empty rule is already
    covered by the elist empty rule, thus we are able to remove it from here,
    which also removes the warning message Bison generates :D */ }
    ;

indexed_list:
    COMMA indexedelem indexed_list
    |
    ;

block:
     LEFT_BRACE {
        /* Functions make their own scopes */
        if(!fromFunct) { enter_Next_Scope(fromFunct); }
        fromFunct=0;
    } stmt_list RIGHT_BRACE {
        exit_Current_Scope();
    }
    ;

funcdef:
    FUNCTION ID LEFT_PARENTHESIS {
        Symbol* sym = resolve_FuncSymbol($2);
        emit(OP_FUNCSTART, create_prog_func_expr(sym), NULL, NULL, 0);
        tmp = sym;
        fromFunct = 1;
        inFunction++;
        enter_Next_Scope(fromFunct);
    } idlist RIGHT_PARENTHESIS block {
        emit(OP_FUNCEND, create_prog_func_expr(tmp), NULL, NULL, 0);
        fromFunct = 0;
        inFunction--;
        $$ = create_prog_func_expr(tmp);
    }
    | FUNCTION LEFT_PARENTHESIS {
        Symbol* sym = resolve_AnonymousFunc();
        emit(OP_FUNCSTART, create_prog_func_expr(sym), NULL, NULL, 0);
        tmp = sym;
        fromFunct = 1;
        inFunction++;
        enter_Next_Scope(fromFunct);
    } idlist RIGHT_PARENTHESIS block {
        emit(OP_FUNCEND, create_prog_func_expr(tmp), NULL, NULL, 0);
        fromFunct = 0;
        inFunction--;
        $$ = create_prog_func_expr(tmp);
    }
    ;

// EXPR
idlist:
    ID { resolve_FormalSymbol($1); } idlist_list
    |
    ;

// EXPR
idlist_list:
    COMMA ID { resolve_FormalSymbol($2); } idlist_list
    |
    ;

returnstmt:
    RETURN SEMICOLON {
        if(!inFunction) {
            yyerror("Use of 'return' outside a function");
        }
        else {
            emit(OP_RETURN, NULL, NULL, NULL, 0);
        }
    }
    | RETURN expr SEMICOLON {
        if (!inFunction) {
            yyerror("Use of 'return' outside a function");
        } else {
            if($2->type == EXP_BOOL) {
                /* Create new temp bool expr */
                expr* mysym = create_bool_expr();
                /* Truelist assigns TRUE to temp symbol */
                backpatch($2->truelist, nextquad());
                emit(OP_ASSIGN, mysym, create_constbool_expr(1), NULL, 0);
                /* Then skips FALSE symbol */
                emit(OP_JUMP, NULL, NULL, NULL, nextquad()+2);
                /* Falselist assigns FALSE to temp symbol */
                backpatch($2->falselist, nextquad());
                emit(OP_ASSIGN, mysym, create_constbool_expr(0), NULL, 0);
                emit(OP_RETURN, mysym, NULL, NULL, 0);
            }
            else {
                emit(OP_RETURN, $2, NULL, NULL, 0);
            }
        }
    }
    ;

M: { 
    $$ = nextquad(); 
};

MJ: {
    $$ = nextquad();
    emit(OP_JUMP, NULL, NULL, NULL, nextquad()); 
};

P: {
    push();
};

%%

int main(int argc, char** argv) {

    Initialize_HashTable();

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

    /* Print SymTable */
    // printf("\n           ======= Syntax Analysis =======\n");
    // print_SymTable();
    
    /* Print Quads */
    printf("\n           ======= Intermediate Code =======\n");
    printQuads();
    
    /* Return Normally */
    free_HashTable();
    freeTokenList(&root);

    return 0;
}

/* end of parser.y */