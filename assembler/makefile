assembler: assembler.o chang_file_name.o macro_analysis.o row_analysis.o validation.o second_row_analysis.o encoding.o
	gcc -ansi -pedantic -Wall chang_file_name.o macro_analysis.o row_analysis.o assembler.o validation.o second_row_analysis.o encoding.o -o assembler

assembler.o: assembler.c assembler.h validation.h chang_file_name.h row_analysis.h second_row_analysis.h encoding.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

chang_file_name.o: chang_file_name.c chang_file_name.h assembler.h
	gcc -c -ansi -pedantic -Wall chang_file_name.c -o chang_file_name.o

macro_analysis.o: macro_analysis.c chang_file_name.h assembler.h
	gcc -c -ansi -pedantic -Wall macro_analysis.c -o macro_analysis.o

row_analysis.o: row_analysis.c row_analysis.h validation.h second_row_analysis.h assembler.h
	gcc -c -ansi -pedantic -Wall row_analysis.c -o row_analysis.o

validation.o: validation.c validation.h assembler.h
	gcc -c -ansi -pedantic -Wall validation.c -o validation.o

second_row_analysis.o: second_row_analysis.c second_row_analysis.h encoding.h validation.h row_analysis.h assembler.h
	gcc -c -ansi -pedantic -Wall second_row_analysis.c -o second_row_analysis.o

encoding.o: encoding.c encoding.h assembler.h
	gcc -c -ansi -pedantic -Wall encoding.c -o encoding.o

