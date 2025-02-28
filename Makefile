# 0=============0
# |   Makefile  |
# 0=============0
TARGET = scanner
CC = gcc
#compile all
all: $(TARGET)
#clean the directory
clean:
	rm -f *.o $(TARGET) $(TARGET).c
#executable
$(TARGET): scanner.o  list.o
	$(CC) $^ -o $@
#.o files
%.o: %.c list.h
	$(CC) -c $< -o $@
#scanner.c
$(TARGET).c: scanner.l list.h
	flex scanner.l
#end of Makefile