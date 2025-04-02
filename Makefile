# 0=============0
# |   Makefile  |
# 0=============0
TARGET = calc
LIST = list
TABLE = table
PARSER = parser
SCANNER = scanner
CC = gcc
# Compile all
all: $(TARGET)
# Executable
$(TARGET): $(SCANNER).o $(PARSER).o $(LIST).o $(TABLE).o
	$(CC) -o $@ $^
# Object Files
%.o: %.c
	$(CC) -c -o $@ $^
# Source Codes
$(PARSER).c $(PARSER).h: $(PARSER).y $(TABLE).h
	bison --yacc -d -v -o $(PARSER).c $(PARSER).y
$(SCANNER).c: $(SCANNER).l $(LIST).h $(PARSER).h
	flex $(SCANNER).l
# Clean the Directory
clean:
	rm -f *.o $(TARGET) $(SCANNER).c $(PARSER).c $(PARSER).h $(PARSER).output
# End of Makefile
runError:
	./calc phase2/Errors/Error0.asc
	./calc phase2/Errors/Error1.asc
	./calc phase2/Errors/Error2.asc
	./calc phase2/Errors/Error3.asc
	./calc phase2/Errors/Error4.asc
	./calc phase2/Errors/Error5.asc
	./calc phase2/Errors/Error6.asc
	./calc phase2/Errors/Error7.asc
	./calc phase2/Errors/Error8.asc
	./calc phase2/Errors/Error9.asc
	./calc phase2/Errors/Error10.asc
	./calc phase2/Errors/Error11.asc
	./calc phase2/Errors/Error12.asc

runWorking:
	./calc phase2/Working/Anonymous.asc
	./calc phase2/Working/Block.asc
	./calc phase2/Working/Circle.asc
	./calc phase2/Working/GlobalAndLocal.asc
	./calc phase2/Working/Grammar.asc
	./calc phase2/Working/Random.asc
	./calc phase2/Working/ShadowedNameOffunctions.asc
	./calc phase2/Working/Simple.asc
	./calc phase2/Working/Tree.asc
