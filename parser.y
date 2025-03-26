%{
    #include "table.h"
    #define YY_DECL int alpha_yylex (void* yylval)

    int yyerror(char* yaccProvidedMessage);
    int yylex(void);
    /*GLOBAL VARIABLES ARE INSIDE THE LIST.H*/
%}

%start program

%union {
	char*	stringZoumi;
	int		intZoumi;
	double	realZoumi;
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

/* PROTERAIOTHTES KAI PROSETAIRISTIKOTHTA */

%right EQUALS
%left OR
%left AND

%nonassoc EQUALS_EQUALS NOT_EQUALS
%nonassoc GREATER GREATER_EQUAL LESS LESS_EQUAL

%left PLUS MINUS
%left MULT DIV MOD

%right NOT MINUS_MINUS PLUS_PLUS

%nonassoc UMINUS

%left DOT DOUBLE_DOT

%left RIGHT_BRACKET LEFT_BRACKET

%left LEFT_PARENTHESIS RIGHT_PARENTHESIS

%left RIGHT_BRACE LEFT_BRACE

%%
/* DEFINED GRAMMAR RULES  */

program:
    stmt_list
    ;

stmt:
    expr SEMICOLON
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | funcdef
    | block
    | BREAK SEMICOLON
    | CONTINUE SEMICOLON
    | SEMICOLON
    ;

stmt_list:
    stmt stmt_list
    |
    ;

expr:
    assignexpr
    | expr op expr
    | term
    ;

op:
    PLUS | MINUS | MULT | DIV | MOD
    | GREATER | LESS | GREATER_EQUAL | LESS_EQUAL
    | EQUALS | NOT_EQUALS
    | AND | OR
    ;

term:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
    | MINUS expr %prec UMINUS
    | NOT expr %prec NOT
    | PLUS_PLUS lvalue
    | lvalue PLUS_PLUS
    | MINUS_MINUS lvalue
    | lvalue MINUS_MINUS
    | primary
    ;

assignexpr:
    lvalue EQUALS expr
    ;

primary:
    lvalue
    | call
    | objectdef
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
    | const 
    ;

lvalue:
    ID
    | LOCAL ID
    | COLON_COLON ID
    | member
    ;

member:
    lvalue PERIOD ID
    | lvalue LEFT_BRACKET expr RIGHT_BRACKET
    | call PERIOD ID
    | call LEFT_BRACKET expr RIGHT_BRACKET
    ;

call:
    call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    | lvalue callsuffix
    |LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
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
    | LEFT_BRACKET indexed RIGHT_BRACKET
    |
    ;

indexed:
    indexedelem indexedelem_list
    |
    ;

indexedelem:
    LEFT_BRACE expr COLON expr RIGHT_BRACE
    ;

indexedelem_list:
    COMMA indexedelem indexedelem_list
    |
    ;

block:
    LEFT_BRACE stmt_list RIGHT_BRACE
    | LEFT_BRACE RIGHT_BRACE
    ;

funcdef:
    FUNCTION ID LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
    | FUNCTION LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
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
    ID idlist_list
    |
    ;

idlist_list:
    COMMA ID idlist_list
    |
    ;

ifstmt:
    IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
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

int yyerror(char* yaccProvidedMessage) {
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
}

/******************************/

int main(int argc, char** argv) {

    /*EDW ARXIKOPOIOUME TO HASHTABLE KAI VAZOUME MESA TA LIB FUNCTIONS*/
    
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { yyin = stdin; }
 
    /* redirect output of bison */
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
    /*EDW PREPEI NA UPARXEI MIA PRINT()*/
    
    /* Return Normally */
    // void free_HashTable(HashTable* ht);
    freeTokenList(&root);

    return 0;
}