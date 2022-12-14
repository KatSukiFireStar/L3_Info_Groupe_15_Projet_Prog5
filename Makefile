CC=clang
CFLAGS=-g -Wall -Werror

all: elfFile

elfFile: elfFile.o

%.o: %.c %.h

clean:
	rm -f elfFile *.o