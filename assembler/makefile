assembler: assembler.o chang_file_name.o macro_analysis.o
	gcc -ansi -pedantic -Wall chang_file_name.o  macro_analysis.o assembler.o -o assembler

assembler.o: assembler.c assembler.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

chang_file_name.o: chang_file_name.c assembler.h
	gcc -c -ansi -pedantic -Wall chang_file_name.c -o chang_file_name.o

macro_analysis.o:macro_analysis.c assembler.h
	gcc -c -ansi -pedantic -Wall macro_analysis.c -o macro_analysis.o
