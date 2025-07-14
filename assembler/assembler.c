#include "assembler.h"

int IC =100;
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
	FILE *f_used;
    	macro** macros;
    	int macro_count;
	/*func* instructions = NULL;*/
	int i=1;/*אם יש שגיאה נדליק אותו וככה נדע לא להוציא קבצי פלט*/
	if(argc==1)
	{
		fprintf(stderr,"error, ther no input files.\n");
		error=1;
		return 1;
	}
	for(;i<argc; i++)
	{
		error=0;
		f1=end_file_name_as( argc, argv , i);
		if(f1!=NULL)
		{
			macros = NULL;
            		macro_count = 0;
			f_used =macro_analysis(f1,cmd, cmd1, argc, argv, i, &macros, &macro_count);
			if (f_used != NULL) 
			{;
    				row_analysis(f_used, macro_count, macros, cmd, cmd1);
				rewind(f_used);
    				fclose(f_used);
			} 			
			else 
			{
    				printf("Error processing macros for %s\n", argv[i]);
				continue;
		
			}
		}
		 else
        	{
            		printf("Could not open source file for %s\n", argv[i]);
        	}
	}
return 0;
}
