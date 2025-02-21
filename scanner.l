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

    enum CAT {ID, STRING, COMMENT, CONST_INT, OPERATOR, KEYWORD, PUNCTUATION};

    typedef struct alpha_token_t {
        unsigned int line;
        unsigned int num_token;

        char* zoumi;
        unsigned int category;
        
        struct alpha_token_t* next; 
    }alpha_token_t;

    alpha_token_t* root;

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

{id}        { fprintf(stderr, "Recognized id with value: %s\n", yytext); }
{string}    { fprintf(stderr, "Recognized string with value: %s\n", yytext); }
{comment1}  { fprintf(stderr, "Recognized comment with value: %s\n", yytext); }
{comment2}  { fprintf(stderr, "Recognized comment with value: %s\n", yytext); }
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
    return 0;
}