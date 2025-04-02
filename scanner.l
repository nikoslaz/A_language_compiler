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
    #include "parser.h"
%}

%option outfile = "scanner.c"
%option noyywrap
%option yylineno

id                [a-zA-Z][a-zA-Z_0-9]*
integer           (0[xX][0-9A-Fa-f]+)|([0-9]+)
real              [0-9]*\.[0-9](e|E)?[0-9]*
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
less_equal        "<="
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

space             [\r\t\v]
illegal           .

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

{line_comment}      { insertToken(&root, yylineno, tokenCounter++, "", C_COMMENT, N_LINE_COMMENT, S_ENUMERATED); }

{integer}           { insertToken(&root, yylineno, tokenCounter++, yytext, C_CONST_INT, N_ERR, S_INTEGER);
                    yylval.intZoumi = atoi(yytext);
                    return INT; }
{real}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_CONST_REAL, N_ERR, S_REAL);
                    yylval.realZoumi = atof(yytext);
                    return REAL; } 

{string}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_STRING, N_ERR, S_CHARSTAR);
                    yylval.stringZoumi = strdup(yytext);
                    return STRING; }

{newline}           { }
{if}                { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_IF, S_ENUMERATED); return IF; }
{else}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_ELSE, S_ENUMERATED); return ELSE;}
{while}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_WHILE, S_ENUMERATED); return WHILE; }
{for}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_FOR, S_ENUMERATED); return FOR; }
{function}          { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_FUNCTION, S_ENUMERATED); return FUNCTION; }
{return}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_RETURN, S_ENUMERATED); return RETURN; }    
{break}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_BREAK, S_ENUMERATED); return BREAK; }  
{continue}          { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_CONTINUE, S_ENUMERATED); return CONTINUE; }    
{and}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_AND, S_ENUMERATED); return AND; }  
{or}                { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_OR, S_ENUMERATED); return OR; }  
{not}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_NOT, S_ENUMERATED); return NOT; }   
{local}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_LOCAL, S_ENUMERATED); return LOCAL; }    
{true}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_TRUE, S_ENUMERATED); return TRUE; }    
{false}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_FALSE, S_ENUMERATED); return FALSE; }    
{nil}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_KEYWORD, N_NIL, S_ENUMERATED); return NIL; }

{equals}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_EQUALS, S_ENUMERATED); return EQUALS; }    
{plus}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_PLUS, S_ENUMERATED); return PLUS; }    
{minus}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MINUS, S_ENUMERATED); return MINUS; }    
{mult}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MULT, S_ENUMERATED); return MULT; }    
{div}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_DIV, S_ENUMERATED); return DIV; }    
{mod}               { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MOD, S_ENUMERATED); return MOD; }    
{equals_equals}     { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_EQUALS_EQUALS, S_ENUMERATED); return EQUALS_EQUALS; }    
{not_equals}        { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_NOT_EQUALS, S_ENUMERATED); return NOT_EQUALS; }    
{plus_plus}         { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_PLUS_PLUS, S_ENUMERATED); return PLUS_PLUS; }    
{minus_minus}       { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_MINUS_MINUS, S_ENUMERATED); return MINUS_MINUS; }    
{greater}           { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_GREATER, S_ENUMERATED); return GREATER; }    
{less}              { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_LESS, S_ENUMERATED); return LESS; }    
{greater_equals}    { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_GREATER_EQUAL, S_ENUMERATED); return GREATER_EQUAL; }    
{less_equal}        { insertToken(&root, yylineno, tokenCounter++, yytext, C_OPERATOR, N_LESS_EQUAL, S_ENUMERATED); return LESS_EQUAL; }    

{left_brace}        { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_LEFT_BRACE, S_ENUMERATED); return LEFT_BRACE; }    
{right_brace}       { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_RIGHT_BRACE, S_ENUMERATED); return RIGHT_BRACE; }    
{left_bracket}      { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_LEFT_BRACKET, S_ENUMERATED); return LEFT_BRACKET; }    
{right_bracket}     { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_RIGHT_BRACKET, S_ENUMERATED); return RIGHT_BRACKET; }    
{left_parenthesis}  { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_LEFT_PARENTHESIS, S_ENUMERATED); return LEFT_PARENTHESIS; } 
{right_parenthesis} { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_RIGHT_PARENTHESIS, S_ENUMERATED); return RIGHT_PARENTHESIS;} 
{semicolon}         { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_SEMICOLON, S_ENUMERATED); return SEMICOLON; }    
{comma}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_COMMA, S_ENUMERATED); return COMMA; }    
{colon}             { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_COLON, S_ENUMERATED); return COLON; }    
{colon_colon}       { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_COLON_COLON, S_ENUMERATED); return COLON_COLON; }    
{period}            { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_PERIOD, S_ENUMERATED); return PERIOD; }    
{period_period}     { insertToken(&root, yylineno, tokenCounter++, yytext, C_PUNCTUATION, N_PERIOD_PERIOD, S_ENUMERATED); return PERIOD_PERIOD; }    

{id}                { insertToken(&root, yylineno, tokenCounter++, yytext, C_ID, N_ERR, S_CHARSTAR);
                    yylval.stringZoumi = strdup(yytext);
                    return ID; }

{space}             { }
" "                 { }
{illegal}           { printf("Error. Illegal Character %s in line %d.\n", yytext, yylineno); exit(1); }

%% 
