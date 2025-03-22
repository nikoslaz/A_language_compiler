# 0=============0
# |   Makefile  |
# 0=============0
TARGET = calc
LIST = list
PARSER = parser
SCANNER = scanner
CC = gcc
# Compile all
all: $(TARGET)
# Executable
$(TARGET): $(SCANNER).o $(PARSER).o $(LIST).o
	$(CC) -o $@ $^
# Object Files
%.o: %.c
	$(CC) -c -o $@ $^
# Source Codes
$(PARSER).c $(PARSER).h: $(PARSER).y
	bison --yacc -d -v -o $(PARSER).c $^
	touch $(PARSER).c
$(SCANNER).c: $(SCANNER).l $(LIST).h $(PARSER).h
	flex $(SCANNER).l
	touch $(SCANNER).c
# Clean the Directory
clean:
	rm -f *.o $(TARGET) $(SCANNER).c $(PARSER).c $(PARSER).h $(PARSER).output
	clear
# End of Makefile
