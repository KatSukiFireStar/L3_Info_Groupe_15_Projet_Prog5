CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfFile.o elfStructure.h

%.o: %.h %.c

clean:
	rm -f elfFile elfFile.o