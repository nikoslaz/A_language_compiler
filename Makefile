# 0=============0
# |   Makefile  |
# 0=============0
TARGET = scanner
CC = gcc
# Compile all
all: $(TARGET)
# Clean the directory
clean:
	rm -f *.o $(TARGET) $(TARGET).c
# Generate Executable
$(TARGET): scanner.o  list.o
	$(CC) $^ -o $@
# Generate .o files
%.o: %.c list.h
	$(CC) -c $< -o $@
# Flex Scanner
$(TARGET).c: scanner.l list.h
	flex scanner.l
# Run Test
run:
	scanner test.txt
# end of Makefile