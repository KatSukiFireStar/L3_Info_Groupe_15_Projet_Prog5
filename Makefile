CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfFile.o

%.o: %.c %.h

clean:
	rm -f elfFile elfFile.o