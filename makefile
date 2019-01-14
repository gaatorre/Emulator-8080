CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -g
OBJECTS = emulator.o

.PHONY: all
all: prog

prog:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o emulator.exe

emulator.o: emulator.c
	 $(CC) $(CFLAGS) emulator.c -c

.Phony: clean
clean:
	rm -f $(OBJECTS) emulator.exe
