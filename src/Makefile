CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate assemble

assemble: assemble.o symbolmap.o tokenise.o
	$(CC) $^ -o $@

tokenise.o: tokenise.c
	$(CC) $(CFLAGS) $< -c -o $@

assemble.o: assemble.c
	$(CC) $(CFLAGS) $< -c -o $@

symbolmap.o: symbolmap.c
	$(CC) $(CFLAGS) $< -c -o $@

emulate: emulate.o tests.o
	$(CC) $^ -o $@

emulate.o: emulate.c
	$(CC) $(CFLAGS) $< -c -o $@

tests.o: tests.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(wildcard *.o)
	rm -r -f $(wildcard *.dSYM)
	rm -f assemble
	rm -f emulate
