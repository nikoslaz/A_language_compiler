%{
    /* parser.y */
    /**
     * @authors nikos , nikoletta , mihalis
     */
    #include "table.h"
    #include "alpha_quads.h"
    struct Symbol* tmp;

    /* Globals */
    int inLoop = 0;
    int inFunction = 0;
%}

%start program

%union {
	char*	stringZoumi;
	int		intZoumi;
	double	realZoumi;
    struct Symbol* symbolZoumi;
    struct expr_s* exprZoumi;  
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

%type <symbolZoumi> funcdef

%type <exprZoumi> expr       
%type <exprZoumi> term       
%type <exprZoumi> assignexpr 
%type <exprZoumi> primary    
%type <exprZoumi> lvalue 
%type <exprZoumi> member 
%type <exprZoumi> const      

%type <symbolZoumi> call
%type <intZoumi> normcall
%type <intZoumi> elist
%type <intZoumi> elist_list

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
    expr SEMICOLON 
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
        if(!inLoop) { yyerror("Use of 'break' outside loop"); }
    }
    | CONTINUE SEMICOLON {
        if(!inLoop) { yyerror("Use of 'continue' outside loop"); }
    }
    | block
    | funcdef
    | SEMICOLON
    ;

stmt_list:
    stmt stmt_list
    |
    ;

expr:
    assignexpr { $$ = $1; }
    | expr PLUS expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr PLUS expr)\n");
            $$ = NULL;
        } 
        else {
            expr* expr_temp = create_arith_expr();
            emit(OP_ADD, expr_temp /*result*/, $1 /*arg1*/, $3 /*arg2*/, 0 /*label*/);
            $$ = expr_temp;
        }
    }
    | expr MINUS expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr MINUS expr)\n");
            $$ = NULL;
        }
        else {
            expr* expr_temp = create_arith_expr();
            emit(OP_SUB, expr_temp /*result*/, $1 /*arg1*/, $3 /*arg2*/, 0 /*label*/);
            $$ = expr_temp;
        }
    }
    | expr MULT expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr MULT expr)\n");
            $$ = NULL;
        }
        else {
            expr* expr_temp = create_arith_expr();
            emit(OP_MUL, expr_temp /*result*/, $1 /*arg1*/, $3 /*arg2*/, 0 /*label*/);
            $$ = expr_temp;
        }
    }
    | expr DIV expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr DIV expr)\n");
            $$ = NULL;
        }
        else {
            expr* expr_temp = create_arith_expr();
            emit(OP_DIV, expr_temp /*result*/, $1 /*arg1*/, $3 /*arg2*/, 0 /*label*/);
            $$ = expr_temp;
        }

    }
    | expr MOD expr {
        if (!$1 || !$3 
            || !($1->type == EXP_VARIABLE || $1->type == EXP_ARITH || $1->type == EXP_CONSTNUMBER)
            || !($3->type == EXP_VARIABLE || $3->type == EXP_ARITH || $3->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(expr MOD expr)\n");
            $$ = NULL;
        }
        else {
            expr* expr_temp = create_arith_expr();
            emit(OP_MOD, expr_temp /*result*/, $1 /*arg1*/, $3 /*arg2*/, 0 /*label*/);
            $$ = expr_temp;
        }
    }
    /* TODO: Backpatching needed */
    | expr GREATER expr {}
    | expr LESS expr
    | expr GREATER_EQUAL expr
    | expr LESS_EQUAL expr
    | expr EQUALS_EQUALS expr
    | expr NOT_EQUALS expr
    | expr AND expr
    | expr OR expr
    | term 
    ;

term:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS { $$ = $2; }
    | MINUS expr %prec UMINUS_CONFLICT {
        if (!$2 || !($2->type == EXP_VARIABLE || $2->type == EXP_ARITH || $2->type == EXP_CONSTNUMBER)) {
            yyerror("Error: Invalid arguments(MINUS expr)\n");
            $$ = NULL;
        }
        else {
            expr* expr_temp = create_arith_expr();
            emit(OP_UMINUS, expr_temp /*result*/, $2 /*arg1*/, NULL /*arg2*/, 0 /*label*/);
            $$ = expr_temp;
        }
    }
    | NOT expr %prec NOT { $$ = $2; }
    | PLUS_PLUS lvalue {
        if($2 && ($2->type == USERFUNC_T || $2->type == LIBFUNC_T)) {
            char msg[34];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($2->symbol, "increment");
        }
    }
    | lvalue PLUS_PLUS {
        if($1 && ($1->type == USERFUNC_T || $1->type == LIBFUNC_T)) {
            char msg[34];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($1->symbol, "increment");
        }
    }
    | MINUS_MINUS lvalue {
        if($2 && ($2->type == USERFUNC_T || $2->type == LIBFUNC_T)) {
            char msg[34];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($2->symbol, "decrement");
        }
    }
    | lvalue MINUS_MINUS {
        if($1 && ($1->type == USERFUNC_T || $1->type == LIBFUNC_T)) {
            char msg[34];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($1->symbol, "decrement");
        }
    }
    | primary { $$ = $1; }
    ;

assignexpr:
    lvalue EQUALS expr {
        if($1 && ($1->type == EXP_PROGRAMFUNC || $1->type == EXP_LIBRARYFUNC)) {
            // MORE CHECKING
            char msg[34];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
            $$ = NULL;
        } else if ($1 && $3) { // Check if both sides are valid expr*
            // First assign
            emit(OP_ASSIGN, $1 /*result(lvalue)*/, $3 /*arg1(rvalue)*/, NULL /*arg2*/, 0 /*label*/);
            expr* expr_result = create_arith_expr();
            // Second assign
            emit(OP_ASSIGN, expr_result /*result(lvalue)*/, $1 /*arg1(rvalue)*/, NULL /*arg2*/, 0 /*label*/);
            $$ = expr_result;
        } 
        else {
            yyerror("Invalid assignment operation");
            $$ = NULL;
        }
    }
    ;

primary:
    lvalue { $$ = $1; }
    | call
    | objectdef
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
    | const { $$ = $1; }
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
        Symbol* symbol_temp = resolve_RawSymbol($2);
        expr* expr_temp = create_var_expr(symbol_temp);
        $$ = expr_temp;
    }
    | COLON_COLON ID {
        Symbol* symbol_temp = resolve_RawSymbol($2);
        expr* expr_temp = create_var_expr(symbol_temp);
        $$ = expr_temp;
    }
    | member
    ;

/* TODO: Implement TABLEGETELEM/TABLESETELEM logic here */
member: 
    lvalue PERIOD ID { $$ = $1; }
    | lvalue LEFT_BRACKET expr RIGHT_BRACKET { $$ = $1; }
    | call PERIOD ID { $$ = $1; }
    | call LEFT_BRACKET expr RIGHT_BRACKET { $$ = $1; }
    ;

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
        Symbol* sym = handleAnonymousFuncCall($2);
        if(sym) {
            $$ = createTempSymbol();
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
        $$ = $2;
    }
    ;

methodcall:
    PERIOD_PERIOD ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;

elist: 
    expr elist_list { $$ = 1 + $2; }
    | { $$ = 0; }
    ;

elist_list: 
    COMMA expr elist_list { $$ = 1 + $3; }
    | { $$ = 0; }
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
        inLoop++;
    } stmt_list RIGHT_BRACE {
        exit_Current_Scope();
        inLoop--;
    }
    ;

funcdef:
    FUNCTION ID LEFT_PARENTHESIS {
        Symbol* sym = resolve_FuncSymbol($2);
        tmp = sym;
        fromFunct = 1;
        inFunction++;
        enter_Next_Scope(fromFunct);
    } idlist RIGHT_PARENTHESIS block {
        fromFunct = 0;
        inFunction--;
        $$ = tmp;
    }
    | FUNCTION LEFT_PARENTHESIS {
        Symbol* sym = resolve_AnonymousFunc();
        tmp = sym;
        fromFunct = 1;
        inFunction++;
        enter_Next_Scope(fromFunct);
    } idlist RIGHT_PARENTHESIS block {
        fromFunct = 0;
        inFunction--;
        $$ = tmp;
    }
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

ifstmt:
    IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt %prec THEN_CONFLICT
    | IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt
    ;

whilestmt:
    WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS {
        inLoop++;
    } stmt {
        inLoop--;
    }
    ;

forstmt:
    FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS {
        inLoop++;
    } stmt {
        inLoop--;
    }
    ;

returnstmt:
    RETURN SEMICOLON {
        if (!inFunction) {
            yyerror("Use of 'return' outside a function");
        }
    }
    | RETURN expr SEMICOLON {
        if (!inFunction) {
            yyerror("Use of 'return' outside a function");
        }
    }
    ;

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

    /* Print Output */
    printf("\n           ======= Syntax Analysis =======\n");
    print_SymTable();
    printQuads();
    
    /* Return Normally */
    free_HashTable();
    freeTokenList(&root);

    return 0;
}

/* end of parser.y */