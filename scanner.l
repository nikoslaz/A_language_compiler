%{
    #if defined(WIN32)
    #define YY_NO_UNISTD_H
    static int isatty (int i) { return 0; }
    #elif defined(_WIN32_WCE)
    #define YY_NO_UNISTD_H
    static int isatty (void *i) { return 0; }
    #endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "list.h"

    enum CAT {ID, STRING, COMMENT, CONST_INT, OPERATOR, KEYWORD, PUNCTUATION};

    alpha_token_t* root = NULL;

    int tokenCounter = 0; //gia na exoume global to num_token

    #define YY_DECL int alpha_yylex (void* yylval)
%}

%option outfile = "scanner.c"
%option noyywrap
%option yylineno

id            [a-zA-Z][a-zA-Z_0-9]*
integer       (0[xX][0-9A-Fa-f]+)|([0-9]+)
real          [0-9]*\.[0-9](e|E)?[0-9]*
space         [\r\n\t\v]
string        "\"[^\n\"]*\""
comment1      "//".*
comment2      "#".*

%x COMMENT INCLUDE

%%
"/*"               BEGIN(COMMENT);
<COMMENT>[^*]*   
<COMMENT>"*"[^*/]*  
<COMMENT>"*"+"/"   BEGIN(INITIAL); 

{integer}   { fprintf(stderr, "Recognized integer with value: %s\n", yytext); insertToken(&root, yylineno, tokenCounter++, yytext, 1); }
{id}        { fprintf(stderr, "Recognized id with value: %s\n", yytext); insertToken(&root, yylineno, tokenCounter++, yytext, 2);}
{real}      { fprintf(stderr, "Recognized real with value: %s\n", yytext); insertToken(&root, yylineno, tokenCounter++, yytext, 3);} 
{string}    { fprintf(stderr, "Recognized string with value: %s\n", yytext); insertToken(&root, yylineno, tokenCounter++, yytext, 4);}
{comment1}  { fprintf(stderr, "Recognized comment with value: %s\n", yytext); insertToken(&root, yylineno, tokenCounter++, yytext, 5);}
{comment2}  { fprintf(stderr, "Recognized comment with value: %s\n", yytext); insertToken(&root, yylineno, tokenCounter++, yytext, 5);}
%%

int main (int argc, char** argv) {
    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else
    {
        yyin = stdin;
    }

    alpha_yylex(NULL);
    printf("\nFinal Token List:\n");
    printTokens(root);
    freeTokenList(&root);
    return 0;
}