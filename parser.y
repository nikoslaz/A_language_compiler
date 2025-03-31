%{
    /**
     * @authors nikos , nikoletta , mihalis
     */
    #include "table.h"
    struct Symbol* tmp;
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
/* 
*/
%type <symbolZoumi> funcdef

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
    | BREAK SEMICOLON
    | CONTINUE SEMICOLON
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
        checkFunctionSymbol($2, "increment");
    }
    | lvalue PLUS_PLUS {
        checkFunctionSymbol($1, "increment");
    }
    | MINUS_MINUS lvalue {
        checkFunctionSymbol($2, "decrement");
    }
    | lvalue MINUS_MINUS {
        checkFunctionSymbol($1, "decrement");
    }
    | primary
    ;

assignexpr:
    lvalue EQUALS expr {
        checkFunctionSymbol($1, "assign to");
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
        $$ = resolve_RawSymbol($1);
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
        $$ = $1;  // simplified: pass lvalue’s Symbol*
      }
      | lvalue LEFT_BRACKET expr RIGHT_BRACKET {
        $$ = $1;
      }
      | call PERIOD ID {
        $$ = NULL;  // Placeholder !!!
      }
      | call LEFT_BRACKET expr RIGHT_BRACKET {
        $$ = NULL;  // Placeholder !!!
      }
      ;

call:
    call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    | lvalue callsuffix
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;

callsuffix:
    normcall
    | methodcall
    ;

normcall:
    LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;

methodcall:
    PERIOD_PERIOD ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;

elist:
    expr elist_list
    |
    ;

elist_list:
    COMMA expr elist_list
    |
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
        /*if is is not from function enter next scope*/
        if(!fromFunct) { enter_Next_Scope(0); }
        fromFunct=0;
    } stmt_list RIGHT_BRACE {
        exit_Current_Scope();
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
    } idlist_list
    |
    ;

idlist_list:
    COMMA ID {
        resolve_FormalSymbol($2);
    } idlist_list
    |
    ;

ifstmt:
    IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt %prec THEN_CONFLICT
    | IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt
    ;

whilestmt:
    WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
    ;

forstmt:
    FOR LEFT_PARENTHESIS elist SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS stmt
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