TODO TODO
TODOTODOTODOTODO
TODOOOOOOO
DODODODODO

---------------------------------------------------------------------------------
// PHASE 4

Target code:

<!-- ARITH, SET, GET, UMINUS (2 args, result) - FULL ( TURH UMINUS TO FULL, USE CONST NUM -1)
RELATIONAL (2 args, index)         - RELATIONAL
FUNCEND, CALL, PARAM (arg1) - ARG1
GETRETVAL, RETURN - (result)        - RES

NOP
ASSIGN - (arg1, result)
JUMP (index)
FUNCSTART (2 args) -->

<!-- FIX RESOLVE RAW SYMBOL -->
<!-- GIA: LOCAL, GLOBALk, RAW -->
<!-- REMEMBER: ftiaxe na fainontai na locals sto FUNCSTART -->
<!-- REMEMBER: apo8hkeyoyme kapou posa program func yparxoun -->
<!-- REMEMBER: ftiaxnoyme ola ta ecalls apo twra ston pinaka -->
<!-- REMEMBER: bale pisw ta source line ston target (gia otan trwme error sthn avm) -->

<!-- FTIAXNOUME to target.c
Vazoume mesa ta emit target:
How to translate symbols to address: XRHSIMOPOIW MONO OFFSET? -->

<!-- First we make the final code dynamic array (similar to quads, but use generate_x) -->

<!-- Create more dynamic arrays for CONSTS (num, string etc) funcs -->

<!-- Write everything in a binary file in order ?? -->

---------------------------------------------------------------------------------
- AVM

/* Kanones */
STACK SIZE 8192
STACK TOP -> point to top element
STACK (DARTH) MAUL - > divider gia topikes - arguments metablhtes
STACK[0] einai to ret_val

|          |
| local2   | <- top
| local1   |
| local0   | <- maul
| old maul |
------------ << FUNCSTART
| ret addr |
| #args    |
------------ << CALL
| arg0     |
| arg1     |
| arg2     |
------------ << BEGIN PARAMS
| global1  |
| global0  | <- initial maul
------------
| retval   | <- 0

TRANSLATE:
Globals & Temporary: stack[1 + index]
Locals: stack[maul + index]
Formals: stack[maul - (4+index)] but check args pushed

/* Procedures */ 

Kanoume prwta push ta args

kanoume push ton ari8mo twn current args (current_args_pushed)
mhdenizoyme to current_args_pushed
mhdenizoume to ret_val

/* Ean einai usrfunc */
meta kanoume push to return address
kanoume branch sthn diey8ynsh ths synarthshs

H synarthsh apo8hkeuei to prohgoymeno stack MAUL
allazoume to stack_maul sto top+1
kanei push ta locals ths

otan bgainoume apo synarthsh??
pop ta locals
restore to stack MAUL
pop to return address
i must pop my own arguments (function that called be doesnt know when i return)
kane jump sto return address

EXECUTORS pali me table of functions kai enum
opws kai to to_bool, to_string, arithmetic_opcodes, relational_opcodes

---------------------------------------------------------------------------------
- HARD

SOMEHOW implement libfuncs
TABLES ??????????????????????????????????????


---------------------------------------------------------------------------------
- TESTS
01,02,03,06,12,13,14,15,16,17,18,19,20,21,22,23,24,25 : TABLES

---------------------------------------------------------------------------------
// PHASE 3

<!-- - Check operations between constants in compile time -->
<!-- - Create quad structs, dynamic array -->
<!-- - Implement Scope Spaces, scope stack, offset inside space, etc (MIXADRILL) GOAT  -->
<!-- - OFFSET MUST NOT RESET WHEN NOT IN FUNCTION (LOOK FAQ) (MIXADRILL) GOAT  -->
<!-- - Method to create, read, delete temp variables -> CREATE, READ  -->
<!-- - DUAL assign quads (for lists mostly) -->
<!-- - Fix double assignment (oxi arith) -->
<!-- - Fix plusplus, minusminus, etc etc -->

<!-- - Function start, end, argument -->
<!-- - Add expr boolean logic to elist -->
<!-- - Reset in loop in functions see FLOW CONTROL ERROR test -->
<!-- - Tables (REMEMBER:: ELIST CAN BE NULL) -->
<!-- - REMEMBER INDEXED AND MEMBER MUST HAVE EXPR BOOLEAN LOGIC -->
<!-- - PLUS_PLUS lvalue is CORRECT! TODO lvalue plus_plus, minus_minus etc etc -->
<!-- - Return must jump to funcend -->
<!-- - Jump before funcdef to after funcend -->

---------------------------------------------------------------------------------
<!-- // Require Backpatching, breaklist, continuelist, merge method -->

<!-- - if-else -->
<!-- - while -->
<!-- - for -->
<!-- - break-continue  -->
<!-- - Fix WHILE, FOR to use Merikh Apotimhsh -->
<!-- Optimize while, for if -->

<!-- Gia Merikh Apotimhsh: -->
<!-- - Fix relational == and !=  -->
<!-- - Expr relop Expr -->
<!-- - not expr -->
<!-- - const TRUE FALSE -->
<!-- - Fix priority of AND and OR -->
<!-- Gia ola auta: 8eloume na elegxoyme oti ta arguments einai valid:
kanoume truecheck etc etc.-->

-----------------------------------------------------------------------
// HARD

<!-- - Maybe Temporary variables Garbage collection ????? -->
<!-- - Almost done!! Needs testing!! -->

------------------------------------------------------------------------

TESTS:
<!-- BACKPATCH DONE -->
<!-- ASSIGNMENTS COMPLEX DONE -->
<!-- ASSIGNMENTS OBJECTS DONE -->
<!-- ASSIGNMENTS SIMPLE DONE -->
<!-- BASIC EXPR WE NEED TO CHECK PLUS PLUS WE GET SEGM!!!! -->
<!-- CALLS (NEEDS JUMPS FROM HARD ELSE DONE) -->
<!-- CONST MATHS DONE  -->
<!-- FLOW CONTROL DONE -->
<!-- FLOW CONTROL ERROR DONE -->
<!-- FUNCDECL DONE WITH CHANGES ON JUMP DONE -->
<!-- IF ELSE DONE  -->
<!-- OBJECT CREATION EXPR (NEEDS JUMPS FROM HARD ELSE DONE) -->
<!-- RELATIONAL DONE -->
<!-- VAR MATHS DONE -->
