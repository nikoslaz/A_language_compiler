# Alpha Language Compiler & Virtual Machine - Phases 4 & 5

This project is a complete toolchain for the Alpha programming language, consisting of a compiler that produces binary bytecode and a virtual machine (AVM) that executes it.

## Components:

### Compiler (generates 'out.abc')
*   `scanner.l` (Lexer)
*   `parser.y` (Parser & Intermediate Code Generator)
*   `list.h`, `list.c` (Token Management)
*   `table.h`, `table.c` (Symbol Table Management)
*   `quads.h`, `quads.c` (Quadruple Generation & Management)
*   `target.h`, `target.c` (Target Code File Generation)

### Virtual Machine (executes 'out.abc')
*   `avm.h`, `avm.c` (Core AVM definitions, stack, and execution cycle)
*   `avm_executors.c` (Implementations for each VM instruction executor)
*   `avm_libfuncs.c` (Implementations for the standard library functions)

## Core Functionality:

### Compiler
*   Tokenization of Alpha source code.
*   Parsing according to Alpha's defined grammar.
*   Symbol table management for scopes and identifiers.
*   **Generation of a binary file** containing intermediate code (quadruples) for all language features, including expressions, control flow, functions, and tables.

### Alpha Virtual Machine (AVM)
*   Loading and validation of the binary instruction file.
*   A stack-based execution model with a frame pointer for function environments.
*   Execution of all opcodes produced by the compiler.
*   A complete standard library with support for I/O, math, type introspection, and argument handling.
*   Automatic memory management for tables via reference counting.

## Usage:

The project is run as a two-step process: compile the source code, then execute the resulting binary file. After compiling the project with the supplied Makefile, run the following commands:

#### 1. Compile the Alpha source file:
./calc input_file

This creates a binary file named `chief.alpha` in your directory.

#### 2. Execute the binary file with the AVM:
./alpha_avm chief.alpha

NOTE: This program was run and tested inside Debian PC "Kasos" 

Authors:
csd4844 - Nikoletta Arvaniti
csd4922 - Nikolaos Lazaridis
csd5087 - Kritsotakis Mihail
