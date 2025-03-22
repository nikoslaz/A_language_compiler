all: parser

parser: scanner.c parser.c list.c
	@gcc $^ -o $@
	@echo "Syntax analyzer is ready to syntactically analyze :D"

parser.c: parser.y
	@bison -d -v $^

scanner.c: scanner.l
	@flex $^

clean:
	@rm -f al parser scanner.c parser.c parser.h
