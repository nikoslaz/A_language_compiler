Lexical Analyzer for language Alpha

This program is a lexical analyzer for a custom programming language named Alpha.
It processes an input source code file and breaks it down into a sequence of tokens
such as keywords, operators, constants, identifiers, and comments.
After processing the input, the program prints the list of tokens, showing the line number,
token number, token itself, category, name, and superclass of each token.
The lexer also handles nested comments and ensures they are properly closed.
This program is designed to be extensible, allowing new keywords, operators, and other tokens 
to be added easily.

Components: list.h, list.c, scanner.l

Usage: After compiling using the supplied Makefile, run the following command:

./al input_file

Where input_file is the path to the source code file. If no file is provided,
the lexer will read from standard input.

Authors:
csd4844 - Nikoletta Arvaniti
csd4922 - Nikolaos Lazaridis
csd5087 - Kritsotakis Mihail
