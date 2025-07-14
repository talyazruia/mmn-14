#include "assembler.h"
int isHashNumber(char * str) {
	if (str == NULL || str[0] != '#') return 0; 

	str++; 

	if (*str == '\0') return 0; 

	while (*str) {
		if (!isdigit((unsigned char)*str)) {
			return 0; 
		}
		str++;
	}

	return 1;
}
int regisrer(char * str)
{
	if (str == NULL || str[0] != 'r') return 0;
	str++;
	if (*str == '\0') return 0;
	while (*str) {
		if (!isdigit((unsigned char)*str)) {
			return 0;
		}
		str++;
	}
	return 1;
}
/*

int add(char row[],SEMEL** SEMELS)
{
	char* command;
	char* op1;
	char* op2;
	int op1_val;
	int op2_val;
	int op1_type;
	int op2_type;
	command = strtok(row, " ");
	op1 = strtok(NULL, ",");
	op2 = strtok(NULL, " ");
	if (op1 == NULL)
	{
		fprintf(stderr, "Error: Missing operand in command %s\n", command);
		error = 1;
		return 0;
	}
	if (op2 == NULL)
	{
		fprintf(stderr, "Error: Missing second operand in command %s\n", command);
		error = 1;
           return 0;
	}
	if (isHashNumber(op1)) {
		op1_val = atoi(op1 + 1); 
		op1_type = 0;
	
	return 0;
}

int mov(char row[],SEMEL** SEMELS)
{
fprintf(stderr,"hhhhhh\n");
return 0;
}

int cmp(char row[],SEMEL** SEMELS)
{return 0;
}

int sub(char row[],SEMEL** SEMELS)
{return 0;
}

int not_func(char row[],SEMEL** SEMELS)
{return 0;
}

int clr(char row[],SEMEL** SEMELS)
{return 0;
}

int lea(char row[],SEMEL** SEMELS)
{return 0;
}

int inc(char row[],SEMEL** SEMELS)
{return 0;
}

int dec(char row[],SEMEL** SEMELS)
{return 0;
}

int jmp(char row[],SEMEL** SEMELS)
{return 0;
}

int bne(char row[],SEMEL** SEMELS)
{return 0;
}

int red(char row[],SEMEL** SEMELS)
{return 0;
}

int prn(char row[],SEMEL** SEMELS)
{return 0;
}

int jsr(char row[],SEMEL** SEMELS)
{return 0;
}

int rts(char row[],SEMEL** SEMELS)
{return 0;
}

int stop(char row[],SEMEL** SEMELS)
{return 0;
}

int data(char row[],SEMEL** SEMELS)
{return 0;
}

int string(char row[],SEMEL** SEMELS)
{return 0;
}

int mat(char row[],SEMEL** SEMELS)
{return 0;
}

int entry(char row[],SEMEL** SEMELS)
{ return 0;
}

int extern_func(char row[],SEMEL** SEMELS)
{
return 0;
}
}*/
