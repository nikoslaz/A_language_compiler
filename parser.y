%{
    /**
     * @authors nikos , nikoletta , mihalis
     */
    #include "table.h"
    struct Symbol* tmp;
    int inLoop = 0;      // Flag to track if we're inside a loop
    int inFunction = 0;  // Flag to track if we're inside a function
%}

%start program
%union {
	char*	stringZoumi;
	int		intZoumi;
	double	realZoumi;
    struct Symbol* symbolZoumi;
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

%type <symbolZoumi> lvalue
%type <symbolZoumi> member
%type <symbolZoumi> funcdef
%type <symbolZoumi> call
%type <intZoumi> callsuffix
%type <intZoumi> normcall
%type <intZoumi> methodcall
%type <intZoumi> elist
%type <intZoumi> elist_list
%type <intZoumi> idlist
%type <intZoumi> idlist_list

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

%left DOT DOUBLE_DOT

%left RIGHT_BRACKET LEFT_BRACKET

%left LEFT_PARENTHESIS RIGHT_PARENTHESIS

%%

program:
    stmt_list
    ;

stmt:
    expr SEMICOLON
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | BREAK SEMICOLON {
        if (!inLoop) {
            yyerror("Use of 'break' while not in a loop");
        }
    }
    | CONTINUE SEMICOLON {
        if (!inLoop) {
            yyerror("Use of 'continue' while not in a loop");
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

expr:
    assignexpr
    | expr PLUS expr
    | expr MINUS expr
    | expr MULT expr
    | expr DIV expr
    | expr MOD expr
    | expr GREATER expr
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
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
    | MINUS expr %prec UMINUS_CONFLICT
    | NOT expr %prec NOT
    | PLUS_PLUS lvalue {
        if ($2 && ($2->type == USERFUNC_T || $2->type == LIBFUNC_T)) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($2, "increment");
        }
    }
    | lvalue PLUS_PLUS {
        if ($1 && ($1->type == USERFUNC_T || $1->type == LIBFUNC_T)) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($1, "increment");
        }
    }
    | MINUS_MINUS lvalue {
        if ($2 && ($2->type == USERFUNC_T || $2->type == LIBFUNC_T)) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $2->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($2, "decrement");
        }
    }
    | lvalue MINUS_MINUS {
        if ($1 && ($1->type == USERFUNC_T || $1->type == LIBFUNC_T)) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($1, "decrement");
        }
    }
    | primary
    ;

assignexpr:
    lvalue EQUALS expr {
        if ($1 && ($1->type == USERFUNC_T || $1->type == LIBFUNC_T)) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Using %s as an lvalue", $1->type == USERFUNC_T ? "ProgramFunc" : "LibFunc");
            yyerror(msg);
        } else {
            checkFunctionSymbol($1, "assign to");
        }
    }
    ;

primary:
    lvalue
    | call
    | objectdef
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
    | const 
    ;

lvalue:
    ID {
        int inaccessible = 0;
        Symbol* sym = lookUp_All($1, &inaccessible);
        if (sym == NULL) {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $1);
                yyerror(msg);
                $$ = NULL;
            } else {
                $$ = resolve_RawSymbol($1);
            }
        } else {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $1);
                yyerror(msg);
                $$ = NULL;
            } else {
                $$ = sym;
            }
        }
    }
    | LOCAL ID {
        $$ = resolve_LocalSymbol($2);
    }
    | COLON_COLON ID {
        $$ = resolve_GlobalSymbol($2);
    }
    | member
    ;

/*JUST A CHECK!!*/
member: lvalue PERIOD ID {
        $$ = $1;  // Pass lvalue’s Symbol*
      }
      | lvalue LEFT_BRACKET expr RIGHT_BRACKET {
        $$ = $1;
      }
      | call PERIOD ID {
        $$ = $1;  // Use the temporary symbol from the call
      }
      | call LEFT_BRACKET expr RIGHT_BRACKET {
        $$ = $1;
      }
      ;

call:
    call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        Symbol* sym = checkFunctionCall($1, "Invalid recursive function call");
        if (sym) {
            if (!sym->varArgs) {  // Skip validation for varArgs functions
                int formal_count = 0;
                argument_node* args = sym->args;
                while (args) {
                    formal_count++;
                    args = args->next;
                }
                if (formal_count != $3) {
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Function expects %d arguments, but %d were provided", formal_count, $3);
                    yyerror(msg);
                }
            }
            $$ = createTempSymbol();
        } else {
            $$ = NULL;
        }
    }
    | lvalue callsuffix {
        int inaccessible = 0;
        Symbol* sym = lookUp_All($1->name, &inaccessible);
        if (sym == NULL) {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $1->name);
                yyerror(msg);
                $$ = NULL;
            } else {
                char msg[100];
                snprintf(msg, sizeof(msg), "Undefined function '%s'", $1->name);
                yyerror(msg);
                $$ = NULL;
            }
        } else {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $1->name);
                yyerror(msg);
                $$ = NULL;
            } else if (sym->type != USERFUNC_T && sym->type != LIBFUNC_T) {
                char msg[100];
                snprintf(msg, sizeof(msg), "'%s' is not a function", $1->name);
                yyerror(msg);
                $$ = NULL;
            } else {
                sym = checkFunctionCall(sym, "Invalid function call");
                if (sym) {
                    if (!sym->varArgs) {  // Skip validation for varArgs functions
                        int formal_count = 0;
                        argument_node* args = sym->args;
                        while (args) {
                            formal_count++;
                            args = args->next;
                        }
                        if (formal_count != $2) {
                            char msg[100];
                            snprintf(msg, sizeof(msg), "Function '%s' expects %d arguments, but %d were provided", sym->name, formal_count, $2);
                            yyerror(msg);
                        }
                    }
                    $$ = createTempSymbol();
                } else {
                    $$ = NULL;
                }
            }
        }
    }
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        Symbol* sym = handleAnonymousFuncCall($2);
        if (sym) {
            if (!sym->varArgs) {  // Skip validation for varArgs functions
                int formal_count = 0;
                argument_node* args = sym->args;
                while (args) {
                    formal_count++;
                    args = args->next;
                }
                if (formal_count != $5) {
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Anonymous function expects %d arguments, but %d were provided", formal_count, $5);
                    yyerror(msg);
                }
            }
            $$ = createTempSymbol();
        } else {
            $$ = NULL;
        }
    }
    ;
    
callsuffix:
    normcall { $$ = $1; }
    | methodcall { $$ = $1; }
    ;

normcall:
    LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        $$ = $2;
    }
    ;

methodcall:
    PERIOD_PERIOD ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        int inaccessible = 0;
        Symbol* sym = lookUp_All($2, &inaccessible);
        if (sym == NULL) {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $2);
                yyerror(msg);
            } else {
                char msg[100];
                snprintf(msg, sizeof(msg), "Undefined method '%s'", $2);
                yyerror(msg);
            }
            $$ = 0;
        } else {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $2);
                yyerror(msg);
                $$ = 0;
            } else if (sym->type != USERFUNC_T && sym->type != LIBFUNC_T) {
                char msg[100];
                snprintf(msg, sizeof(msg), "'%s' is not a function", $2);
                yyerror(msg);
                $$ = 0;
            } else {
                if (!sym->varArgs) {  // Skip validation for varArgs functions
                    int formal_count = 0;
                    argument_node* args = sym->args;
                    while (args) {
                        formal_count++;
                        args = args->next;
                    }
                    if (formal_count != $4) {
                        char msg[100];
                        snprintf(msg, sizeof(msg), "Method '%s' expects %d arguments, but %d were provided", $2, formal_count, $4);
                        yyerror(msg);
                    }
                }
                $$ = $4;
            }
        }
    }
    ;

call:
    PERIOD_PERIOD ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
        // Treat as a regular function call for now
        int inaccessible = 0;
        Symbol* sym = lookUp_All($2, &inaccessible);
        if (sym == NULL) {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $2);
                yyerror(msg);
            } else {
                char msg[100];
                snprintf(msg, sizeof(msg), "Undefined method '%s'", $2);
                yyerror(msg);
            }
            $$ = 0;
        } else {
            if (inaccessible && inFunction) {
                char msg[100];
                snprintf(msg, sizeof(msg), "Cannot access '%s' inside function", $2);
                yyerror(msg);
                $$ = 0;
            } else if (sym->type != USERFUNC_T && sym->type != LIBFUNC_T) {
                char msg[100];
                snprintf(msg, sizeof(msg), "'%s' is not a function", $2);
                yyerror(msg);
                $$ = 0;
            } else {
                int formal_count = 0;
                argument_node* args = sym->args;
                while (args) {
                    formal_count++;
                    args = args->next;
                }
                if (formal_count != $4) {
                    char msg[100];
                    snprintf(msg, sizeof(msg), "Method '%s' expects %d arguments, but %d were provided", $2, formal_count, $4);
                    yyerror(msg);
                }
                $$ = $4;
            }
        }
    }
    ;

elist: expr elist_list {
        $$ = 1 + $2; 
    }
    | { $$ = 0; }
    ;

elist_list: COMMA expr elist_list {
        $$ = 1 + $3;
    }
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
        if (!fromFunct) {
            enter_Next_Scope(0);
        } else {
            inFunction = 1;  // Entering a function body
        }
        fromFunct = 0;
    } stmt_list RIGHT_BRACE {
        exit_Current_Scope();
        inFunction = 0;  // Exiting a function body
    }
    ;

funcdef:
    FUNCTION ID LEFT_PARENTHESIS {
        Symbol* sym = resolve_FuncSymbol($2);
        if (sym && sym->type == LIBFUNC_T) {
            yyerror("Cannot redefine library function");
        }
        tmp = sym;
        fromFunct = 1;
        enter_Next_Scope(1);
    } idlist RIGHT_PARENTHESIS block {
        $$ = tmp;
    }
    | FUNCTION LEFT_PARENTHESIS {
        Symbol* sym = resolve_AnonymousFunc();
        tmp = sym;
        fromFunct = 1;
        enter_Next_Scope(1);
    } idlist RIGHT_PARENTHESIS block {
        $$ = tmp;
    }
    ;

const:
    INT
    | REAL
    | STRING
    | NIL
    | TRUE
    | FALSE
    ;

idlist:
    ID {
        resolve_FormalSymbol($1);
        $$ = 1;
    } idlist_list {
        $$ = 1 + $3;
    }
    | {
        $$ = 0;
    }
    ;

idlist_list:
    COMMA ID {
        resolve_FormalSymbol($2);
        $$ = 1;
    } idlist_list {
        $$ = 1 + $3;  // Use $3 (idlist_list) instead of $2 (ID)
    }
    | {
        $$ = 0;
    }
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
    RETURN SEMICOLON 
    | RETURN expr SEMICOLON 
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

    /* Comments not closed */
    if(comment_depth > 0){
        printf("Error! The comment in line %d is not closed properly.\n", comment_startlines[comment_top]);
        exit(1);
    }

    /* Print Output */
    printf("\n   ======= Syntax Analysis =======\n\n");
    print_SymTable();
    
    /* Return Normally */
    free_HashTable();
    freeTokenList(&root);

    return 0;
}