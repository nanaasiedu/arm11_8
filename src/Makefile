CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.c
	$(CC) $(CFLAGS) $< -o $@

emulate: emulate.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(wildcard *.o)
	rm -r -f $(wildcard *.dSYM)
	rm -f assemble
	rm -f emulate
