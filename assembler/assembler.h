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
	char *name;
	int param;
	int op;
}command;

int main(int argc, char * argv[]);
FILE *end_file_name_as(int argc, char * argv[], int error,int i);
void row_analysis(FILE * f, int error);
void clear_row_arry();
FILE *end_file_name_am(int argc, char * argv[], int error, int i); 

#endif
