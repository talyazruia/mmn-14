#include "assembler.h"

int main(int argc, char * argv[])
{
	command cmd[] = {
		{"mov", 2, 0},
		{"cmp", 2, 1},
		{"add", 2, 2},
		{"sub", 2, 3},
		{"not", 1, 4},
		{"clr", 1, 5},
		{"lea", 2, 6},
		{"inc", 1, 7},
		{"dec", 1, 8},
		{"jmp", 1, 9},
		{"bne", 1, 10},
		{"red", 1, 11},
		{"prn", 1, 12},
		{"jsr", 1, 13},
		{"rts", 0, 14},
		{"stop", 0, 15}}

	command1 cmd1[]={
		{".data", },
		{".string",1},
		{".mat", },
		{".entry", 1},
		{".extern", 1}}
		
	FILE *f; /* מערך של מצביעים למחרוזות, מוקצה סטטית על ה־stack*/
	FILE *f1;
	int i=1;/*אם יש שגיאה נדליק אותו וככה נדע לא להוציא קבצי פלט*/
	if(argc==1)
	{
		fprintf(stderr,"error, ther no input files.\n");
		error=1;
	}
	for(;i<argc; i++)
	{
		f=end_file_name_as( argc, argv , i);
		f1=macro_analysis(f,cmd);
		row_analysis(f);
	}
return 0;
}
