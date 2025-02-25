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

newline       "\n"
if            "if"
else          "else"
id            [a-zA-Z][a-zA-Z_0-9]*
integer       (0[xX][0-9A-Fa-f]+)|([0-9]+)
real          [0-9]*\.[0-9](e|E)?[0-9]*
space         [\r\n\t\v]
string        \"[^\"]*\"

comment1      "//".*
comment2      "#".*
%x COMMENT

%%

"/*" {
    insertToken(&root, yylineno, tokenCounter++, "", 8, 44, 2);
    BEGIN(COMMENT);
}
<COMMENT>\n { }
<COMMENT>[^*]*;
<COMMENT>"*"[^*/]*; 
<COMMENT>"*/" {
    BEGIN(INITIAL);
}
{comment1} { insertToken(&root, yylineno, tokenCounter++, "", 8, 42, 2); }
{comment2} { insertToken(&root, yylineno, tokenCounter++, "", 8, 43, 2); }


{string} { 
    char tempyytext[100];
    strncpy(tempyytext, yytext + 1, yyleng - 2);  
    tempyytext[yyleng - 2] = '\0';  
    insertToken(&root, yylineno, tokenCounter++, tempyytext, 5, 47, 1);
}

{newline}   { }
{if}        { insertToken(&root, yylineno, tokenCounter++, yytext, 1, 1, 2); }
{else}      { insertToken(&root, yylineno, tokenCounter++, yytext, 1, 2, 2); }
{integer}   { insertToken(&root, yylineno, tokenCounter++, yytext, 3, 45, 3); }
{real}      { insertToken(&root, yylineno, tokenCounter++, yytext, 4, 46, 4); } 
{id}        { insertToken(&root, yylineno, tokenCounter++, yytext, 7, 48, 1); }



%%

int main (int argc, char** argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { yyin = stdin; }
    alpha_yylex(NULL);
    /* DO NOT TOUCH THIS PRINTF */
    printf("   ======= Lexical Analysis =======\n\n");
    printTokens(root);
    freeTokenList(&root);
    return 0;
}