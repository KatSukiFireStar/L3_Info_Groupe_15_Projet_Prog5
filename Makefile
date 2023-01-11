CC=gcc
CFLAGS=-g -Wall -Werror -Wno-unknown-pragmas

all: elfFile

elfFile: elfStructure.o elfFile.o elfExtract.o elfDisplay.o elfFusion.o utils.o

docs:
	doxygen Doxyfile

openDoc:
	xdg-open Documentation.html

tests:
	TMP_LANG=$LANG
	LANG=en_US
	dotnet test test --verbosity normal
	LANG=$TMP_LANG

clean:
	rm -f elfFile elfFile.o elfStructure.o elfExtract.o elfDisplay.o elfFusion.o utils.o