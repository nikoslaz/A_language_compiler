%{
    /**
     * @authors nikos , nikoletta , mihalis
     */
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

id                [a-zA-Z][a-zA-Z_0-9]*
integer           (0[xX][0-9A-Fa-f]+)|([0-9]+)
real              [0-9]*\.[0-9](e|E)?[0-9]*
space             [\r\n\t\v]
string            \"([^\"\\]|\\.)*\"?
newline           "\n"
if                "if"
else              "else"
while             "while"
for               "for"
function          "function"
return            "return"
break             "break"
continue          "continue"
and               "and"
or                "or"
not               "not"
local             "local"
true              "true"
false             "false"
nil               "nil"
equals            "="
plus              "+"
minus             "-"
mult              "*"
div               "/"
mod               "%"
equals_equals     "=="
not_equals        "!="
plus_plus         "++"
minus_minus       "--"
greater           ">"
less              "<"
greater_equals    ">="
less_equal        "=<"
left_brace        "{"
right_brace       "}"
left_bracket      "["
right_bracket     "]"
left_parenthesis  "("
right_parenthesis ")"
semicolon         ";"
comma             ","
colon             ":"
colon_colon       "::"
period            "."
period_period     ".."
line_comment      "//".*
python_comment    "#".*

%x COMMENT

%%

"/*" {
    assert(comment_top==-1 && comment_depth==0);
    comment_top++;
    comment_startlines[comment_top] = yylineno;
    comment_depth++;
    BEGIN(COMMENT);
}
<COMMENT>\n { }
<COMMENT>[^*]*;
<COMMENT>"*"[^*/]*;
<COMMENT>"/*" {
    comment_top++;
    if(comment_top>=MAX_NESTED_COMMENTS) {
        printf("Error. Maximum number of Nested Comments reached. Sorry :(\n");
        exit(1);
    }
    comment_startlines[comment_top] = yylineno;
    comment_depth++;
}
<COMMENT>"*/" {
    comment_depth--;
    assert(comment_depth >= 0);
    if(comment_depth > 0) {
        char range[MAX_COMMENT_RANGE];
        if(comment_startlines[comment_top] > MAX_FILE_LINES) {
            printf("Error. Maximum number of Lines reached. Sorry :(\n");
            exit(1);
        }
        sprintf(range, "%d - %d", comment_startlines[comment_top], yylineno);
        insertToken(&root, comment_startlines[comment_top], tokenCounter++, range, C_COMMENT, N_NESTED_COMMENT, S_ENUMERATED);
        comment_top--;
    }
    if(comment_depth == 0) {
        char range[20]; 
        sprintf(range, "%d - %d", comment_startlines[comment_top], yylineno);
        insertToken(&root, comment_startlines[comment_top], tokenCounter++, range, C_COMMENT, N_MULTILINE_COMMENT, S_ENUMERATED);
        comment_top--;
        assert(comment_top==-1);
        BEGIN(INITIAL);
    }
}

{line_comment}     { insertToken(&root, yylineno, tokenCounter++, "", C_COMMENT, N_LINE_COMMENT, S_ENUMERATED); }
{python_comment}   { insertToken(&root, yylineno, tokenCounter++, "", C_COMMENT, N_PYTHON_COMMENT, S_ENUMERATED); }

{integer}          { insertToken(&root, yylineno, tokenCounter++, yytext, C_CONST_INT, N_ERR, S_INTEGER); }
{real}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_CONST_REAL, N_ERR, S_REAL); } 

{string} {   
    insertToken(&root, yylineno, tokenCounter++, yytext, C_STRING, N_ERR, S_CHARSTAR);
}

{newline}           { }
{if}                { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_IF, S_ENUMERATED); }
{else}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_ELSE, S_ENUMERATED); }
{while}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_WHILE, S_ENUMERATED); }
{for}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_FOR, S_ENUMERATED); }
{function}          { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_FUNCTION, S_ENUMERATED); }
{return}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_RETURN, S_ENUMERATED); }    
{break}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_BREAK, S_ENUMERATED); }  
{continue}          { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_CONTINUE, S_ENUMERATED); }    
{and}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_AND, S_ENUMERATED); }  
{or}                { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_OR, S_ENUMERATED); }  
{not}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_NOT, S_ENUMERATED); }   
{local}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_LOCAL, S_ENUMERATED); }    
{true}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_TRUE, S_ENUMERATED); }    
{false}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_FALSE, S_ENUMERATED); }    
{nil}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_NIL, S_ENUMERATED); }

{equals}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_EQUALS, S_ENUMERATED); }    
{plus}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_PLUS, S_ENUMERATED); }    
{minus}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MINUS, S_ENUMERATED); }    
{mult}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MULT, S_ENUMERATED); }    
{div}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_DIV, S_ENUMERATED); }    
{mod}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MOD, S_ENUMERATED); }    
{equals_equals}     { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_EQUALS_EQUALS, S_ENUMERATED); }    
{not_equals}        { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_NOT_EQUALS, S_ENUMERATED); }    
{plus_plus}         { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_PLUS_PLUS, S_ENUMERATED); }    
{minus_minus}       { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MINUS_MINUS, S_ENUMERATED); }    
{greater}           { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_GREATER, S_ENUMERATED); }    
{less}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_LESS, S_ENUMERATED); }    
{greater_equals}    { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_GREATER_EQUAL, S_ENUMERATED); }    
{less_equal}        { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_LESS_EQUAL, S_ENUMERATED); }    

{left_brace}        { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_LEFT_BRACE, S_ENUMERATED); }    
{right_brace}       { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_RIGHT_BRACE, S_ENUMERATED); }    
{left_bracket}      { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_LEFT_BRACKET, S_ENUMERATED); }    
{right_bracket}     { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_RIGHT_BRACKET, S_ENUMERATED); }    
{left_parenthesis}  { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_LEFT_PARENTHESIS, S_ENUMERATED); } 
{right_parenthesis} { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_RIGHT_PARENTHESIS, S_ENUMERATED); } 
{semicolon}         { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_SEMICOLON, S_ENUMERATED); }    
{comma}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_COMMA, S_ENUMERATED); }    
{colon}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_COLON, S_ENUMERATED); }    
{colon_colon}       { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_COLON_COLON, S_ENUMERATED); }    
{period}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_PERIOD, S_ENUMERATED); }    
{period_period}     { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_PERIOD_PERIOD, S_ENUMERATED); }    

{id}                { insertToken(&root, yylineno, tokenCounter++, yytext, C_ID, N_ERR, S_CHARSTAR); }

%% 

int main (int argc, char** argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            fprintf(stderr, "Cannot read file: %s\n", argv[1]);
            return 1;
        }
    } else { yyin = stdin; }
    
    /* redirect output of lex */
    yyout = fopen("/dev/null", "w");  
    alpha_yylex(NULL);
    fclose(yyout);

    /* Comments not closed */
    if(comment_depth > 0){
        printf("Error! The multiline comments are not closed properly. Try again :)\n");
        exit(1);
    }

    /* Print Output */
    printf("   ======= Lexical Analysis =======\n\n");
    printTokens(root);
    
    /* Return Normally */
    freeTokenList(&root);
    return 0;
}