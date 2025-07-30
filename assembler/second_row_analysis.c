#include "assembler.h"


void second_row_analysis(FILE * f , command cmd[]  ,command1 cmd1[],SEMEL*** SEMELS, int* semel_count, binary_code ** array, binary_directive **struct_DC,extern_label** extern_labels,int * count_of_extern_labels)
{
	char row[MAX_LEN_OF_ROW];
	char row_copy[MAX_LEN_OF_ROW];
	char *token;
	size_t len;
	int i=0;
	char *new_row;
	char *colon;
	
	while (fgets(row, sizeof(row), f))
	{
		if (strchr(row, '\n') == NULL) 
		{
			error=1;
			clear_row_arry();
			continue;
		}
		if(row[0]==';')
		{
			continue;
		}
		strcpy(row_copy, row);
        	token = strtok(row_copy, " \t\n\r");
		if(token == NULL)
		{
			continue;
		}
		len = strlen(token);
        	if (len > 0 && token[len - 1] == ':')
        	{
			token=strtok(NULL, " \t\n\r");
			if(token!=NULL)
			{
				colon = strchr(row, ':');
				if (colon != NULL) 
      					new_row = colon + 1;
				else
					new_row=row; 
				for(i=0; i<16; i++)
				{
					if(strcmp(token, cmd[i].name)==0)
					{
						add(new_row, *(SEMELS), semel_count, cmd, array, extern_labels, count_of_extern_labels);
						break;
					}
				}
				if(strcmp(token, ".string")==0)
				{
					string(row, struct_DC);
				}
				else if(strcmp(token, ".mat")==0)
				{
					mat(new_row , struct_DC);
				}
				else if(strcmp(token, ".data")==0)
				{
					data(new_row , struct_DC);
				}
				else if(strcmp(token, ".entry")==0 ||	strcmp(token, ".extern")==0  )
				{
					entry_extern( new_row );
				}
				else
					error=1;
			}
		}
		else
		{
			for(i=0; i<16; i++)
			{
				if(strcmp(token, cmd[i].name)==0)
				{
					add(row, *(SEMELS),semel_count, cmd, array,extern_labels, count_of_extern_labels);
					break;
				}
			}
			if(strcmp(token, ".string")==0)
			{
				string(row, struct_DC);
			}
			else if(strcmp(token, ".mat")==0)
			{
				mat(row, struct_DC);
			}
			else if(strcmp(token, ".data")==0)
			{
				data(row , struct_DC);
			}
			else if(strcmp(token, ".entry")==0 ||	strcmp(token, ".extern")==0  )
			{
				entry_extern(row );
			}
			else
				error=1;
		}
	}
}
			
			
