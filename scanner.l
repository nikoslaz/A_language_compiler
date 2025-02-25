%{
    #if defined(WIN32)
    #define YY_NO_UNISTD_H
    static int isatty (int i) { return 0; }
    #elif defined(_WIN32_WCE)
    #define YY_NO_UNISTD_H
    static int isatty (void *i) { return 0; }
    #endif

    #include "list.h"

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
if            "if"
else          "else"
newline       "\n"

%x COMMENT INCLUDE

%%
"/*"               BEGIN(COMMENT);
<COMMENT>[^*]*   
<COMMENT>"*"[^*/]*  
<COMMENT>"*"+"/"   BEGIN(INITIAL); 

{newline}   { }
{if}        { insertToken(&root, yylineno, tokenCounter++, yytext, 1, CAT[4], 7); }
{else}      { insertToken(&root, yylineno, tokenCounter++, yytext, 1, CAT[5], 7); }
{integer}   { insertToken(&root, yylineno, tokenCounter++, yytext, 2, yytext, 9); }
{id}        { insertToken(&root, yylineno, tokenCounter++, yytext, 10, yytext, 8);}
{real}      { /*insertToken(&root, yylineno, tokenCounter++, yytext, 3);*/} 
{string}    { /*insertToken(&root, yylineno, tokenCounter++, yytext, 4);*/}
{comment1}  { /*insertToken(&root, yylineno, tokenCounter++, yytext, 5);*/}
{comment2}  { /*insertToken(&root, yylineno, tokenCounter++, yytext, 5);*/}

%%

int main (int argc, char** argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { yyin = stdin; }
    alpha_yylex(NULL);
    printTokens(root);
    freeTokenList(&root);
    return 0;
}