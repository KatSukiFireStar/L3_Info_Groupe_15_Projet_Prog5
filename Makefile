CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfStructure.o elfFile.o elfExtract.o elfDisplay.o elfFusion.o utils.o

docs:
	doxygen Doxyfile

tests:
	dotnet test test --verbosity normal

clean:
	rm -f elfFile elfFile.o elfStructure.o elfExtract.o elfDisplay.o elfFusion.o utils.o