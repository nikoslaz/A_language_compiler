%{
    #include <stdio.h>
    #include "list.h"
    #define YY_DECL int alpha_yylex (void* yylval)

    int yyerror(char* yaccProvidedMessage);
    int yylex(void);
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

/* Ορισμός προτεραιοτήτων και προσεταιριστικότητας APO KATW PROS TA PANW!! */

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

/*DEFINE GRAMMAR RULES  */
program:
    stmt_list
    ;

stmt_list:
    stmt
    | stmt_list stmt
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

expr:
    assignexpr
    | expr op expr
    | term
    ;

assignexpr:
    ID EQUALS expr
    ;

op:
    PLUS | MINUS | MULT | DIV | MOD
    | GREATER | LESS | GREATER_EQUAL | LESS_EQUAL
    | EQUALS | NOT_EQUALS
    | AND | OR
    ;

term:
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
    | '-' expr %prec UMINUS
    | '!' expr %prec NOT
    | primary
    ;


primary:
    ID
    | INT
    | REAL
    | STRING
    | TRUE
    | FALSE
    | NIL
    ;

ifstmt:
    IF '(' expr ')' stmt
    | IF '(' expr ')' stmt ELSE stmt
    ;

whilestmt:
    WHILE '(' expr ')' stmt
    ;

forstmt:
    FOR '(' elist ';' expr ';' elist ')' stmt
    ;

returnstmt:
    RETURN [ expr ] ';'
    ;

block:
    '{' stmt_list '}'
    ;

funcdef:
    FUNCTION ID '(' idlist ')' block
    ;

idlist:
    /* empty */
    | ID
    | ID ',' idlist
    ;

elist:
    /* empty */
    | expr
    | expr ',' elist
    ;
 /*mexri edw einai auta pou mas exei dwsei o savvidhs, apo dw kai pera einai tou frontisthriou*/
expression:
    INT
    | ID
    | expression '+' expression
    | expression '-' expression
    | expression '*' expression
    | expression '/' expression
    | '(' expression ')'
    | '-' expression %prec UMINUS
    ;

expr:
    expression '\n'

expressions:
    expressions expr
    | expr
    ;

assignment:
    ID '=' expression '\n'
    ;

assignments:
    assignments assignment
    | /* empty */
    ;

%%

int yyerror(char* yaccProvidedMessage) {
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
}

/******************************/

int main(int argc, char** argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { yyin = stdin; }
    yyparse();
    /* Comments not closed */
    if(comment_depth > 0){
        printf("Error! The comment in line %d is not closed properly.\n", comment_startlines[comment_top]);
        exit(1);
    }
    return 0;
}