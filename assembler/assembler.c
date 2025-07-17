#include "assembler.h"

int IC =100;
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
	SEMEL** SEMELS = NULL;
	int semel_count = 0;
	binary_code * array=NULL;
char* row="mov #5 r15";
	int i=1;/*אם יש שגיאה נדליק אותו וככה נדע לא להוציא קבצי פלט*/
	int j=0;

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
			{
    				row_analysis(f_used, macro_count, macros, cmd, cmd1, &SEMELS, &semel_count);
				update_data_symbol_addresses( SEMELS, semel_count); 
				fprintf(stderr,"%d %d", IC, DC);
				rewind(f_used);
				add(row,SEMELS, semel_count,  cmd,  array);
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
	for (j = 0; j < semel_count; j++) {
        if (SEMELS[j]) {
            free(SEMELS[j]->name);
            free(SEMELS[j]);
     	   }
  	  }
   free(SEMELS);
return 0;
}

