CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfStructure.o elfFile.o elfExtract.o elfDisplay.o elfFusion.o utils.o

docs:
	doxygen Doxyfile

openDoc:
	xdg-open Documentation.html

tests:
	LANG=en_US
	dotnet test test --verbosity normal

clean:
	rm -f elfFile elfFile.o elfStructure.o elfExtract.o elfDisplay.o elfFusion.o utils.o