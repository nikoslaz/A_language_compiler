# 0=============0
# |   Makefile  |
# 0=============0
TARGET = calc
LIST = list
TABLE = table
QUADS = alpha_quads
PARSER = parser
SCANNER = scanner
CC = gcc
# Compile all
all: $(TARGET)
# Executable
$(TARGET): $(SCANNER).o $(PARSER).o $(LIST).o $(TABLE).o $(QUADS).o
	$(CC) -o $@ $^
# Object Files
%.o: %.c
	$(CC) -c -o $@ $^
# Source Codes
$(PARSER).c $(PARSER).h: $(PARSER).y $(TABLE).h $(QUADS).h
	bison --yacc -d -v -o $(PARSER).c $(PARSER).y
$(SCANNER).c: $(SCANNER).l $(LIST).h $(PARSER).h
	flex $(SCANNER).l
# Clean the Directory
clean:
	rm -f *.o $(TARGET) $(SCANNER).c $(PARSER).c $(PARSER).h $(PARSER).output
# End of Makefile
