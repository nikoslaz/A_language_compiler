# 0=============0
# |   Makefile  |
# 0=============0

TARGET = calc
LIST = list
TABLE = table
QUADS = quads
TCODE = target
PARSER = parser
SCANNER = scanner

AVMTARGET = avm
AVM = avm

CC = gcc
# Compile all
all: $(TARGET) $(AVMTARGET)

# Compiler Executable
$(TARGET): $(SCANNER).o $(PARSER).o $(LIST).o $(TABLE).o $(QUADS).o $(TCODE).o
	$(CC) -o $@ $^

# AVM Executable
$(AVMTARGET): $(AVM).o
	$(CC) -o $@ $^

# Object Files
%.o: %.c
	$(CC) -c -o $@ $^

# Source Codes
$(PARSER).c $(PARSER).h: $(PARSER).y
	bison --yacc -d -v -o $(PARSER).c $(PARSER).y
$(SCANNER).c: $(SCANNER).l $(PARSER).h
	flex $(SCANNER).l

# Clean the Directory
clean:
	rm -f *.o $(TARGET) $(AVMTARGET) $(SCANNER).c $(PARSER).c $(PARSER).h *.output chief.alpha

# End of Makefile
