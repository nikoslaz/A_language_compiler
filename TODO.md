TODO TODO
TODOTODOTODOTODO
TODOOOOOOO
DODODODODO

---------------------------------------------------------------------------------
// PHASE 4

ARITH, SET, GET, UMINUS (2 args, result) - FULL ( TURH UMINUS TO FULL, USE CONST NUM -1)
RELATIONAL (2 args, index)         - RELATIONAL
FUNCSTART, FUNCEND, CALL, PARAM (arg1) - ARG1
GETRETVAL, RETURN - (result)        - RES


ASSIGN - (arg1, result)  - ASSIGN
JUMP (index)             - JUMP
NOP                      - NOP

Target code:

FTIAXNOUME to target.c

Vazoume mesa ta emit target:

How to translate symbols to address: XRHSIMOPOIW MONO OFFSET?

1 extra field: pointer to translated quad
SKATA
REMEMBER: modify function address to translated_address (after translation)
ALSO: for each quad, store the address of the translated_quad
and at the end, after we have finished the whole translated array, make
every jump point to the destination quad translated address

First we make the final code dynamic array (similar to quads, but use generate_x)
patch JUMPS

Create more dynamic arrays for CONSTS (num, string etc) funcs

Write everything in a binary file in order ??

Keep up with target.h && target.c!


AVM

(when do we make functstart and funcend push_pop the activation stack?)














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
