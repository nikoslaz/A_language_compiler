TODO TODO
TODOTODOTODOTODO
TODOOOOOOO
DODODODODO

---------------------------------------------------------------------------------

- Check operations between constants in compile time
- Create quad structs, dynamic array
- Create opcode enum
- Create vartype enum (program, local, formal etc)
- Create Variable enum (FunctionAddress, Table, etc)
- Implement Scope Spaces, scope stack, offset inside space, etc (MIXADRILL)
- OFFSET MUST NOT RESET WHEN NOT IN FUNCTION (LOOK FAQ)
- Method to create, read, delete temp variables

---------------------------------------------------------------------------------
// Require Backpatching, breaklist, continuelist, merge method

- Function start, end, argument
- if-else
- while
- for
- break-continue 

---------------------------------------------------------------------------------
// Hard

Table variables
Merikh Apotimhsh

/* WHAT IT SHOULD LOOK LIKE */

expr: expr PLUS expr {
    // Create the temporary result for the operation
    Symbol* temp = create_temp_symbol();
    // Generate a quad for the addition operation
    generate_quad("+", $1, $3, temp);
    // Assign the result to $$ (the expression result)
    $$ = create_arith_expr(temp);
}

