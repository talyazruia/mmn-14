#include "assembler.h"

int main(int argc, char * argv[])
{
	commands cmd[] = {
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
	FILE *f;
	FILE *f1;

	 /* מערך של מצביעים למחרוזות, מוקצה סטטית על ה־stack*/
	int i=1;
	int error=0;/*אם יש שגיאה נדליק אותו וככה נדע לא להוציא קבצי פלט*/
	if(argc==1)
	{
		fprintf(stderr,"error, ther no input files.\n");
		error=1;
	}
	for(;i<argc; i++)
	{
		f=end_file_name_as( argc, argv ,error, i);
		f1=macro_analysis(f,cmd[]);
		row_analysis(f1,error);
	}
return 0;
}
