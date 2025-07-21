#ifndef MYMAT_H
#define MYMAT_H

#include <stdio.h>   /* Including the Standard I/O library */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define END_FILE_NAME_AS ".as"
#define END_FILE_NAME_AM ".am"
#define MAX_LEN_OF_ROW 80

typedef struct {
	char * name;
	int addres;
	int type;
	int ex;
}SEMEL;

typedef struct {
	char *name;
	int param;
	int op;
}command;

typedef struct {
	char *name;
	int param;
	int(*func)(char row[],SEMEL** SEMELS);
}command1;

extern int IC;
extern int DC;

typedef struct {
	char * name;
	char *content;
}macro;

typedef struct {
	char first;
	char second;
}binary_code;
typedef struct {
	char first;
	char second;
}binary_directive;
typedef struct {
	char * name;
	int addres
}entery;
typedef struct {
	char * name;
	int addres
}extern_;



extern int error;
extern int size_of_binary_struct;

int main(int argc, char * argv[]);
FILE *end_file_name_as(int argc, char * argv[],int i);
void row_analysis(FILE *f, int macro_count, macro **macros, command cmd[], command1 cmd1[], SEMEL ***SEMELS, int *semel_count);
void clear_row_arry();
FILE *end_file_name_am(int argc, char * argv[], int i); 
FILE * macro_analysis(FILE *f1, command cmd[], command1 cmd1[], int argc, char *argv[], int i,macro*** macros_out, int* macro_count_out);
void add_SEMEL(char* label, int type, int addres, SEMEL*** semels, int* semel_count);

int add(char row[],SEMEL** SEMELS, int SEMEL_count, command cmd[], binary_code* array);


void data(char row[],binary_directive **struct_DC);
void string(char row[],binary_directive **struct_DC);
void mat(char row[],binary_directive **struct_DC);
void entry(char row[],SEMEL** SEMELS, int semel_count,entery **enterys);
void extern_func(char row[],SEMEL** SEMELS,int semel_count,extern_ **ex);

void add_number(int num, binary_code* array);
void add_two_numbers(int num1, int num2, binary_code* array); 
void update_data_symbol_addresses(SEMEL** semels, int semel_count);
int has_two_square_bracket_pairs( char* str);
void ic_count_1_arg(char* row);
void ic_count_2_arg(char* row);
void dc_count_data(char* row);
void dc_count_string(char* row);
void dc_count_mat(char* row);
int reg( char * str);
int isHashNumber( char * str); 
int* valid_matrix(char* str, SEMEL** SEMELS,int SEMEL_count);
int valid_SEMEL(char* str, SEMEL** SEMELS, int SEMEL_count);
void to_binary(int opcode, int op1, int op2,binary_code* array);  
void fix_commas_in_place(char *line);

#endif
