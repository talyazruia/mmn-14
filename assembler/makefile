assembler: assembler.o chang_file_name.o row_analysis.o
	gcc -ansi -pedantic -Wall chang_file_name.o row_analysis.o assembler.o -o assembler

assembler.o: assembler.c assembler.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

chang_file_name.o: chang_file_name.c assembler.h
	gcc -c -ansi -pedantic -Wall chang_file_name.c -o chang_file_name.o

row_analysis.o: row_analysis.c assembler.h
	gcc -c -ansi -pedantic -Wall row_analysis.c -o row_analysis.o
