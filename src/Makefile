CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate assemble

assemble: assemble.o symbolmap.o tokenise.o parse.o generate.o table.o
	$(CC) $^ -o $@

tokenise.o: tokenise.c
	$(CC) $(CFLAGS) $< -c -o $@

assemble.o: assemble.c
	$(CC) $(CFLAGS) $< -c -o $@

symbolmap.o: symbolmap.c
	$(CC) $(CFLAGS) $< -c -o $@

parse.o: parse.c
	$(CC) $(CFLAGS) $< -c -o $@

table.o: table.c
	$(CC) $(CFLAGS) $< -c -o $@

generate.o: generate.c
	$(CC) $(CFLAGS) $< -c -o $@

emulate: emulate.o
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
