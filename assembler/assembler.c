
#include "assembler.h"
#include "validation.h"
#include "chang_file_name.h"
#include "row_analysis.h"
#include "second_row_analysis.h"
#include "encoding.h"

int IC = 100;
int DC = 0;
int error = 0;
int current_size_instaction_struct = 0;
int current_size_directive_struct = 0;
int sum_of_row=0;


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
	FILE* f2_ob;
	FILE* f3_ex;
	FILE* f4_en;
	FILE *f_used;
	macro** macros;
	int macro_count;
	SEMEL** SEMELS = NULL;
	int semel_count = 0;
	binary_code * array=NULL;
	binary_directive *struct_DC=NULL;
	extern_label* extern_labels = NULL;
	int count_of_extern_labels = 0;

	int i=1;
	int j=0;
	int k=0;
	int ic;
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
		sum_of_row=0;

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

		f1=end_file_name_as( argc, argv , i);
		if(f1!=NULL)
		{
			macros = NULL;
			macro_count = 0;
			f_used =macro_analysis(f1,cmd, cmd1, argc, argv, i, &macros, &macro_count);
			if (f_used != NULL) 
			{
				sum_of_row=0;
				row_analysis(f_used, macro_count, macros, cmd, cmd1, &SEMELS, &semel_count);
				update_data_symbol_addresses( SEMELS, &semel_count);
			/*semel print*/	for(k=0; k<semel_count; k++)
				{
    					fprintf(stderr, "%s %d %d\n", SEMELS[k]->name, SEMELS[k]->addres, SEMELS[k]->ex_en);
				}
			/*IC DCprint*/fprintf(stderr, "%d %d\n", IC, DC); 
				rewind(f_used);
				ic=IC;
				IC=100;
				sum_of_row=0;
				second_row_analysis(f_used , cmd  , &SEMELS, &semel_count,  &array, &struct_DC,&extern_labels , &count_of_extern_labels);
				print_binary_code_array(array, current_size_instaction_struct);
			/*extern print*/	for(k = 0; k < count_of_extern_labels; k++)
				{
					fprintf(stderr, "%s %d \n", extern_labels[k].name, extern_labels[k].addres);
				}

				if(error==0&&ic+DC<256)
				{
				f2_ob=end_file_name( argc,argv,i, 2);
				if(f2_ob==NULL)
				{
					error=1;
					return 0;
				}
				BinaryToBase4((void**)&array,argc, argv, i, f2_ob, 5,&semel_count,ic-100);
				fprintf(f2_ob, "\n");
				BinaryToBase4((void**)&array,argc, argv, i, f2_ob, 1,&semel_count,0);
				BinaryToBase4((void**)&struct_DC,argc, argv, i, f2_ob, 2,&semel_count,0);
				fclose(f2_ob);

				f3_ex=end_file_name( argc,argv,i, 3);
				if(f3_ex==NULL)
				{
					error=1;
					return 0;
				}
				BinaryToBase4((void**)&extern_labels,argc, argv, i, f3_ex, 3,&count_of_extern_labels,0);
				fclose(f3_ex);

				f4_en=end_file_name( argc,argv,i, 4);
				if(f4_en==NULL)
				{
					error=1;
					return 0;
				}
				BinaryToBase4((void**)SEMELS,argc, argv, i, f4_en, 4,&semel_count,0);
				fclose(f4_en);

				fclose(f_used);
				}
				else
				{
					fprintf(stderr ,"errors found - skipping file generation for %s\n", argv[i]);
				}
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
				continue;
		}
		else
		{
			fprintf(stderr,"error Could not open source file for %s\n", argv[i]);
		}
	}

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
	}
	
	if (extern_labels != NULL) 
	{
		for (j = 0; j < count_of_extern_labels; j++) 
			free(extern_labels[j].name); 
	
		free(extern_labels); 

	}

	if (array != NULL) 
	{
		free(array);
	}

	if (struct_DC != NULL) 
	{
		free(struct_DC);
	}

	return 0;
}

	void print_binary_code_array(binary_code* array, int size) {
    int k, bit;
    for (k = 0; k < size; k++) {
        /* הדפסת first */
        for (bit = 7; bit >= 0; bit--) {
            fprintf(stderr, "%d", (array[k].first >> bit) & 1);
        }
        fprintf(stderr, " ");

        /* הדפסת second */
        for (bit = 7; bit >= 0; bit--) {
            fprintf(stderr, "%d", (array[k].second >> bit) & 1);
        }
        fprintf(stderr, "\n");
    }
}
			
