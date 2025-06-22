#include "assembler.h"

int IC =0;
int DC =0;
int error =0;
int main(int argc, char * argv[])
{
	command cmd[] = {
		{"mov", 2, 0,mov},
		{"cmp", 2, 1,cmp},
		{"add", 2, 2,add},
		{"sub", 2, 3,sub},
		{"not", 1, 4,not_func},
		{"clr", 1, 5,clr},
		{"lea", 2, 6,lea},
		{"inc", 1, 7,inc},
		{"dec", 1, 8,dec},
		{"jmp", 1, 9,jmp},
		{"bne", 1, 10,bne},
		{"red", 1, 11,red},
		{"prn", 1, 12,prn},
		{"jsr", 1, 13,jsr},
		{"rts", 0, 14,rts},
		{"stop", 0, 15,stop}
				};

	command1 cmd1[]={
		{".data",1,data },
		{".string",1,string},
		{".mat", 1,mat},
		{".entry", 1,entry},
		{".extern", 1,extern_func}
				};
	FILE *f1;
		
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
			macro_analysis(f1,cmd, cmd1, argc, argv, i);
			/*printf("Finished macro analysis on file %s\n", argv[i]);
			fclose(f1);
			if(f2!=f1)
			row_analysis(
			if(f2!=NULL&& f2!=f1)
				fclose(f2);*/
		}
		 else
        {
            printf("Could not open source file for %s\n", argv[i]);
        }
	}
return 0;
}
