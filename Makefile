CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfStructure.o elfFile.o elfExtract.o elfDisplay.c utils.o

%.o: %.h %.c

clean:
	rm -f elfFile elfFile.o elfStructure.o elfExtract.o