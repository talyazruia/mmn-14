#ifndef MYMAT_H
#define MYMAT_H

#include <stdio.h>   /* Including the Standard I/O library */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
	int(*func)(char row[],SEMEL** SEMELS);
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

extern int error;

int main(int argc, char * argv[]);
FILE *end_file_name_as(int argc, char * argv[],int i);
void row_analysis(FILE * f, int macro_count,macro**macros, command cmd[], command1 cmd1[]);
void clear_row_arry();
FILE *end_file_name_am(int argc, char * argv[], int i); 
void macro_analysis(FILE *f1, command cmd[], command1 cmd1[], int argc, char *argv[], int i);
SEMEL** add_SEMEL(char *label , int type,int addres);
int add(char row[],SEMEL** SEMELS);
int mov(char row[],SEMEL** SEMELS);
int cmp(char row[],SEMEL** SEMELS);
int sub(char row[],SEMEL** SEMELS);
int not_func(char row[],SEMEL** SEMELS);
int clr(char row[],SEMEL** SEMELS);
int lea(char row[],SEMEL** SEMELS);
int inc(char row[],SEMEL** SEMELS);
int dec(char row[],SEMEL** SEMELS);
int jmp(char row[],SEMEL** SEMELS);
int bne(char row[],SEMEL** SEMELS);
int red(char row[],SEMEL** SEMELS);
int prn(char row[],SEMEL** SEMELS);
int jsr(char row[],SEMEL** SEMELS);
int rts(char row[],SEMEL** SEMELS);
int stop(char row[],SEMEL** SEMELS);
int data(char row[],SEMEL** SEMELS);
int string(char row[],SEMEL** SEMELS);
int mat(char row[],SEMEL** SEMELS);
int entry(char row[],SEMEL** SEMELS);
int extern_func(char row[],SEMEL** SEMELS);

#endif
