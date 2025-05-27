# Alpha Language Compiler - Phase 3

This program is a lexical analyzer, syntax analyzer, and **intermediate code generator** for a custom programming language named Alpha. It now produces quadruples as an intermediate representation.

## Components:

*   `scanner.l` (Lexer)
*   `parser.y` (Parser & Intermediate Code Generator)
*   `table.h`, `table.c` (Symbol Table Management)
*   `quads.h`, `quads.c` (Quadruple Generation & Management)
*   `list.h`, `list.c` (Utility Lists)

## Core Functionality:

*   Tokenization of Alpha source code.
*   Parsing according to Alpha's defined grammar.
*   Symbol table management for scopes and identifiers.
*   **Generation of intermediate code (quadruples)** for:
    *   Expressions (arithmetic, relational, logical with short-circuiting).
    *   Control flow statements (`if`, `while`, `for`, `break`, `continue`).
    *   Function definitions and calls.
    *   Table (object) operations.
    *   Assignments.

Usage: After compiling using the supplied Makefile, run the following command:

./calc input_file

Where input_file is the path to the source code file. If no file is provided,
the analyzer will read from standard input.

NOTE: This program was run and tested inside Debian PC "Kasos" 

Authors:
csd4844 - Nikoletta Arvaniti
csd4922 - Nikolaos Lazaridis
csd5087 - Kritsotakis Mihail
