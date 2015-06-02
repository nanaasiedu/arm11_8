CC      = gcc
CFLAGS  = -ferror-limit=200 -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate assemble

assemble: assemble.o symbolmap.o tokenise.o parse.o generate.o table.o intArray.o bitUtils.o
	$(CC) $^ -o $@

tokenise.o: tokenise.c
	$(CC) $(CFLAGS) $< -c -o $@

assemble.o: assemble.c
	$(CC) $(CFLAGS) $< -c -o $@

symbolmap.o: helpers/symbolmap.c
	$(CC) $(CFLAGS) $< -c -o $@

parse.o: parse.c
	$(CC) $(CFLAGS) $< -c -o $@

table.o: helpers/table.c
	$(CC) $(CFLAGS) $< -c -o $@

generate.o: generate.c
	$(CC) $(CFLAGS) $< -c -o $@

intArray.o: helpers/intArray.c
	$(CC) $(CFLAGS) $< -c -o $@

emulate: emulate.o bitUtils.o
	$(CC) $^ -o $@

emulate.o: emulate.c
	$(CC) $(CFLAGS) $< -c -o $@

tests.o: tests.c
	$(CC) $(CFLAGS) $< -c -o $@

bitUtils.o: helpers/bitUtils.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(wildcard *.o)
	rm -r -f $(wildcard *.dSYM)
	rm -f assemble
	rm -f emulate
