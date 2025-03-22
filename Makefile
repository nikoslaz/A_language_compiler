all: parser

parser: scanner.c parser.c list.c
	@gcc $^ -o $@
	@echo "syntax analyzer ready :D"

parser.c parser.h: parser.y
	@bison -d -v -o parser.c $^

scanner.c: scanner.l
	@flex $^

clean:
	@rm -f parser scanner.c parser.c parser.h parser.output

