#include "assembler.h"

int IC = 100;
int DC = 0;
int error = 0;
int current_size_instaction_struct = 0;
int current_size_directive_struct = 0;

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
		{".data",1},
		{".string",1},
		{".mat", 1},
		{".entry", 1},
		{".extern", 1}
				};
	FILE *f1;
	FILE *f_used;
    	macro** macros;
    	int macro_count;
	SEMEL** SEMELS = NULL;
	int semel_count = 0;
	binary_code * array=NULL;
	binary_directive *struct_DC=NULL;
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
		IC =100;
		DC =0;
		error =0;
		current_size_instaction_struct=0;
		current_size_directive_struct=0;

		if (SEMELS != NULL) 
		{
            		for (j = 0; j < semel_count; j++) 
			{
                		if (SEMELS[j]) 
				{
                    			free(SEMELS[j]->name);
                    			free(SEMELS[j]);
                		}
            		}
            		free(SEMELS);
            		SEMELS = NULL;
            		semel_count = 0;
        	}
        
        	if (array != NULL) 
		{
            		free(array);
            		array = NULL;
        	}
        
        	if (struct_DC != NULL) 
		{
            		free(struct_DC);
            		struct_DC = NULL;
        	}
		printf("Processing file: %s\n", argv[i]);
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
    				fclose(f_used);
				if (error != 0) 
				{
                    			char* temp_file_name = (char*)malloc(strlen(argv[i]) + strlen(".am") + 1);
                   	 		if (temp_file_name != NULL) 
					{
                        			strcpy(temp_file_name, argv[i]);
                        			strcat(temp_file_name, ".am");
                        			remove(temp_file_name);
                        			free(temp_file_name);
                    			}
                		}
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
	if (SEMELS != NULL) {
        for (j = 0; j < semel_count; j++) {
            if (SEMELS[j]) {
                free(SEMELS[j]->name);
                free(SEMELS[j]);
            }
        }
        free(SEMELS);
    }
    
    if (array != NULL) {
        free(array);
    }
    
    if (struct_DC != NULL) {
        free(struct_DC);
    }
    
    return 0;
}

