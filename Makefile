CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfStructure.o elfFile.o

%.o: %.h %.c

clean:
	rm -f elfFile elfFile.o