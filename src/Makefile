CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate

emulate: emulateDefs.c
	$(CC) $(CFLAGS) emulateDefs.c -o emulateDefs

clean:
	rm -f $(wildcard *.o)
	rm -r -f $(wildcard *.dSYM)
	rm -f assemble
	rm -f emulate
