%{
    #include "table.h"
    #define YY_DECL int alpha_yylex (void* yylval)

    int yyerror(char* yaccProvidedMessage);
    int yylex(void);
    /*GLOBAL VARIABLES ARE INSIDE THE LIST.H*/
    HashTable* ht;
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

%right UMINUS

%left DOT DOUBLE_DOT

%left RIGHT_BRACKET LEFT_BRACKET

%left LEFT_PARENTHESIS RIGHT_PARENTHESIS

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
    ID {
        /*we should search only on current scope and scope 0!!!!!!! SOSOSOSOSOSO*/
        // Regular lookup: search from current scope upward to scope 0
        Symbol* sym = lookUp_Symbol(ht, $1, ht->currentScope, ht->currentScope-1);
        if (!sym) {
            // If not found, insert as a new variable
            if (ht->currentScope == 0) {
                insert_Symbol(ht, $1, GLOBAL, ht->currentScope, yylineno);
            } else {
                insert_Symbol(ht, $1, LOCAL_T, ht->currentScope, yylineno);
            }
        }
    }
    | LOCAL ID {
        // Check for redeclaration in the exact scope
        //change the lookup take only $1 etc
        Symbol* sym = lookUp_Symbol(//ht, $2, ht->currentScope, ht->);
        if (sym) {
            //return
        } else {
            insert_Symbol(ht, $2, LOCAL_T, ht->currentScope, yylineno);
        }
    }
    | COLON_COLON ID {
        // Global lookup: only search in scope 0
        Symbol* sym = lookUp_Symbol(ht, $2, 0, 1);
        if (!sym) {
            fprintf(stderr, "Error: Global variable %s not found at line %d\n", $2, yylineno);
        }
    }
    | member
    | call COLON_COLON ID {
        Symbol* sym = lookUp_Symbol(ht, $3, 0, 1);
        if (!sym) {
            fprintf(stderr, "Error: Global variable %s not found at line %d\n", $3, yylineno);
        }
    }
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

/*THIS IS NOT CORRECT YET I THINK. THE SCOPE CHANGES WITH ( and the arguments should also be kept.*/
funcdef:
    FUNCTION ID LEFT_PARENTHESIS {
        Symbol* sym = lookUp_Symbol(ht, $2, ht->currentScope, 1);
        if (sym) {
            fprintf(stderr, "Error: Function %s already defined in scope %d at line %d\n", $2, ht->currentScope, sym->line);
        } else {
            insert_Symbol(ht, $2, USERFUNC, ht->currentScope, yylineno);
        }
        enter_Scope(ht);
    } idlist RIGHT_PARENTHESIS block { exit_Scope(ht); }
    | FUNCTION LEFT_PARENTHESIS {
        char anon_name[32];
        snprintf(anon_name, sizeof(anon_name), "anon_func_%d", yylineno);
        insert_Symbol(ht, anon_name, USERFUNC, ht->currentScope, yylineno);
        enter_Scope(ht);
    } idlist RIGHT_PARENTHESIS block { exit_Scope(ht); }
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
        printf("Processing formal argument %s in scope %d\n", $1, ht->currentScope);
        Symbol* sym = lookUp_Symbol(ht, $1, ht->currentScope, 1);
        if (sym) {
            fprintf(stderr, "Error: Formal argument %s already defined in scope %d at line %d\n", $1, ht->currentScope, sym->line);
        } else {
            insert_Symbol(ht, $1, FORMAL, ht->currentScope, yylineno);
        }
    } idlist_list
    |
    ;

idlist_list:
    COMMA ID {
        // Check for redeclaration in the exact scope
        Symbol* sym = lookUp_Symbol(ht, $2, ht->currentScope, 1);
        if (sym) {
            fprintf(stderr, "Error: Formal argument %s already defined in scope %d at line %d\n", $2, ht->currentScope, sym->line);
        } else {
            insert_Symbol(ht, $2, FORMAL, ht->currentScope, yylineno);
        }
    } idlist_list
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
    ht = create_HashTable();

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
    // Symbol* test = insert_Symbol(ht, "hello", GLOBAL, 0, 0);
    // Symbol* test1 = insert_Symbol(ht, "hel", LOCAL_T, 0, 1);
    // Symbol* test2 = insert_Symbol(ht, "h", FORMAL, 0, 3);
    print_SymTable(ht);
    
    /* Return Normally */
    free_HashTable(ht);
    freeTokenList(&root);

    return 0;
}