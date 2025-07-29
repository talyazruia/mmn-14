#ifndef MYMAT_H
#define MYMAT_H

#include <stdio.h>   /* Including the Standard I/O library */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define END_FILE_NAME_AS ".as"
#define END_FILE_NAME_AM ".am"
#define END_FILE_NAME_OB ".ob"
#define END_FILE_NAME_ENT ".ent"
#define END_FILE_NAME_EXT ".ext"
#define MAX_LEN_OF_ROW 82
#define MAX_LEN_OF_LABEL 31
enum FileType { AM = 1, OB, EXT, ENT };


typedef struct {
	char * name;
	int addres;
	int type;
	int ex_en;
}SEMEL;

typedef struct {
	char *name;
	int param;
	int op;
}command;

typedef struct {
	char *name;
	int param;

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
	int addres;
}entery;
typedef struct {
	char * name;
	int addres;
}extern_;
typedef enum {
    TYPE_INSTRUCTION,
    TYPE_DIRECTIVE
} array_type;



extern int error;
extern int current_size_instaction_struct;
extern int current_size_directive_struct;

int main(int argc, char * argv[]);
FILE *end_file_name_as(int argc, char * argv[],int i);
void row_analysis(FILE *f, int macro_count, macro **macros, command cmd[], command1 cmd1[], SEMEL ***SEMELS, int *semel_count);
void clear_row_arry();
FILE *end_file_name(int argc, char * argv[], int i,int type); 
FILE * macro_analysis(FILE *f1, command cmd[], command1 cmd1[], int argc, char *argv[], int i,macro*** macros_out, int* macro_count_out);
void add_SEMEL(char* label, int type, int addres, SEMEL*** semels, int* semel_count,int ex_en);
void second_row_analysis(FILE * f , command cmd[]  ,command1 cmd1[],SEMEL*** SEMELS, int* semel_count, binary_code ** array, binary_directive **struct_DC);

int add(char row[],SEMEL** SEMELS, int* semel_count, command cmd[], binary_code** array);
void data(char row[],binary_directive **struct_DC);
void string(char row[],binary_directive **struct_DC);
void mat(char row[], binary_directive** struct_DC);
void entry_extern(char row[]);


void add_number(int num, void** array, array_type type,int flag);
void add_two_numbers(int num1, int num2, binary_code** array); 
void update_data_symbol_addresses(SEMEL** semels, int semel_count);
int has_two_square_bracket_pairs( char* str);
int is_valid_integer(const char *str);
void ic_count_1_arg(char* row);
void ic_count_2_arg(char* row);
void dc_count_data(char* row);
void dc_count_string(char* row);
void dc_count_mat(char* row);
int reg( char * str);
int isHashNumber( char * str); 
int* valid_matrix(char* str,SEMEL** SEMELS, int* semel_count);
int valid_SEMEL(char* str, SEMEL** SEMELS, int* semel_count);
void to_binary(int opcode, int op1, int op2, binary_code** array);  
void fix_commas_in_place(char *line);
void encode_10bit_to_chars(int num, char *a, char *b);
void BinaryToBase4(void** array, int argc, char *argv[], int i, FILE* f2, int struct_type, int* semel_count);
char *base4_convert(char a, char b, char* result);
int is_valid_label_format(const char* label);
#endif
