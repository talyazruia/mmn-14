assembler: assembler.o chang_file_name.o macro_analysis.o row_analysis.o command_analysis.o IC_DC_counter.o add_func.o
	gcc -ansi -pedantic -Wall chang_file_name.o  macro_analysis.o row_analysis.o command_analysis.o assembler.o IC_DC_counter.o add_func.o -o assembler

assembler.o: assembler.c assembler.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

chang_file_name.o: chang_file_name.c assembler.h
	gcc -c -ansi -pedantic -Wall chang_file_name.c -o chang_file_name.o

macro_analysis.o:macro_analysis.c assembler.h
	gcc -c -ansi -pedantic -Wall macro_analysis.c -o macro_analysis.o

row_analysis.o:row_analysis.c assembler.h
	gcc -c -ansi -pedantic -Wall row_analysis.c -o row_analysis.o

command_analysis.o:command_analysis.c assembler.h
	gcc -c -ansi -pedantic -Wall command_analysis.c -o command_analysis.o

IC_DC_counter.o:IC_DC_counter.c assembler.h
	gcc -c -ansi -pedantic -Wall IC_DC_counter.c -o IC_DC_counter.o

add_func.o:add_func.c assembler.h
	gcc -c -ansi -pedantic -Wall add_func.c -o add_func.o
