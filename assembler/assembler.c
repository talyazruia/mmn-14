#include "assembler.h"

int IC =0;
int DC =0;
int error =0;
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
		{"stop", 0, 15}
				};

	command1 cmd1[]={
		{".data",1 },
		{".string",1},
		{".mat", 1},
		{".entry", 1},
		{".extern", 1}
				};
	FILE *f1;	
	FILE *f2; /* מערך של מצביעים למחרוזות, מוקצה סטטית על ה־stack*/
	int i=1;/*אם יש שגיאה נדליק אותו וככה נדע לא להוציא קבצי פלט*/
	if(argc==1)
	{
		fprintf(stderr,"error, ther no input files.\n");
		error=1;
		return 1;
	}
	for(;i<argc; i++)
	{
		f1=end_file_name_as( argc, argv , i);
		if(f1!=NULL)
		{
			f2=macro_analysis(f1,cmd, cmd1, argc, argv, i);
			printf("Finished macro analysis on file %s\n", argv[i]);
			fclose(f1);
			/*if(f2!=f1)
			row_analysis(f2);*/
			if(f2!=NULL&& f2!=f1)
				fclose(f2);
		}
		 else
        {
            printf("Could not open source file for %s\n", argv[i]);
        }
	}
return 0;
}
