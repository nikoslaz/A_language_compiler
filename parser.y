%{
    #include <stdio.h>
    #include "list.h"
    #define YY_DECL int alpha_yylex (void* yylval)
    
    int yyerror (char* yaccProvidedMessage);
    int yylex (void);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
%}

%start program

/*edw mallon prepei na mpoune ola ta tokens? APO TON YACC isws oxi ola mallon mono chars?*/
%token id integer

/*Ορισμός προτεραιοτήτων
και προσεταιριστικότητας APO KATW PROS TA PANW!!*/
%right '='
%left ','

%left '+' '-'
%left '*' '/'

%nonassoc UMINUS

%left '(' ')'

%%

program:
    assignments expressions 
    | /* empty */
    ;

expression:
    integer
    | id
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
    id '=' expression '\n'
    ;

assignments:
    assignments assignment
    | /* empty */
    ;

%%

int yyerror (char* yaccProvidedMessage)
{
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    fprintf(stderr, "INPUT NOT VALID\n");
}

/******************************/

int main(int argc, char** argv)
{
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else
        yyin = stdin;

    yyparse();

    return 0;
}

