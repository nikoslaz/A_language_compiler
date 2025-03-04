# 0=============0
# |   Makefile  |
# 0=============0
TARGET = al
SOURCE = scanner
CC = gcc
#compile all
all: $(TARGET)
#clean the directory
clean:
	rm -f *.o $(TARGET) $(TARGET).c
#executable
$(TARGET): al.o  list.o
	$(CC) $^ -o $@
#.o files
%.o: %.c list.h
	$(CC) -c $< -o $@
#al.c
$(TARGET).c: $(SOURCE).l list.h
	flex $(SOURCE).l
#end of Makefile
