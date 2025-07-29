#include "assembler.h"

/*void second_row_analysis(FILE * f , command cmd[]  ,command1 cmd1[],SEMEL*** SEMELS, int* semel_count, binary_code ** array, binary_directive **struct_DC)
{
	fprintf(stderr, "Entering second_row_analysis\n");
    fprintf(stderr, "f=%p, SEMELS=%p, semel_count=%p\n", (void*)f, (void*)SEMELS, (void*)semel_count);
    fprintf(stderr, "array=%p, struct_DC=%p\n", (void*)array, (void*)struct_DC);
    
    if (SEMELS == NULL) {
        fprintf(stderr, "ERROR: SEMELS is NULL\n");
        return;
    }
    
    if (*SEMELS == NULL) {
        fprintf(stderr, "ERROR: *SEMELS is NULL\n");
        return;
    }
    
    fprintf(stderr, "semel_count value: %d\n", *semel_count);
	char row[MAX_LEN_OF_ROW];
	fprintf(stderr, "About to declare row_copy\n");
	char row_copy[MAX_LEN_OF_ROW];
	fprintf(stderr, "Variables declared successfully\n");
	char *token;
	fprintf(stderr, "token declared\n");
	size_t len;
	fprintf(stderr, "len declared\n");
	int i=0;
	fprintf(stderr, "i declared\n");
	char *new_row;
	char *colon;

	fprintf(stderr, "All variables declared\n");
    
    fprintf(stderr, "About to start while loop\n");
	while (fgets(row, sizeof(row), f))
	{
		fprintf(stderr, "Read line: %.50s\n", row);
		if (strchr(row, '\n') == NULL) 
		{
			fprintf(stderr, "Line too long, skipping\n");
			error=1;
			clear_row_arry();
			continue;
		}
		if(row[0]==';')
		{
			fprintf(stderr, "Comment line, skipping\n");
			continue;
		}
		fprintf(stderr, "About to copy row\n");
		strcpy(row_copy, row);
		fprintf(stderr, "About to strtok\n");
        	token = strtok(row_copy, " \t\n\r");
		fprintf(stderr, "token = %s\n", token ? token : "NULL");
		if(token == NULL)
		{
			fprintf(stderr, "Empty token, continuing\n");
			continue;
		}
		len = strlen(token);
		fprintf(stderr, "token length = %zu\n", len);
        	if (len > 0 && token[len - 1] == ':')
        	{
			fprintf(stderr, "Found label: %s\n", token);
        fprintf(stderr, "About to get next token\n");
			token=strtok(NULL, " \t\n\r");
			fprintf(stderr, "Next token = %s\n", token ? token : "NULL");
			if(token!=NULL)
			{
				fprintf(stderr, "About to find colon in original row\n");
				colon = strchr(row, ':');
				if (colon != NULL) 
      					new_row = colon + 1;
				else
					new_row=row;
				fprintf(stderr, "new_row = %.30s\n", new_row);
            
            fprintf(stderr, "About to check commands\n"); 
				for(i=0; i<16; i++)
				{
					if(strcmp(token, cmd[i].name)==0)
					{
						fprintf(stderr, "Found command: %s, calling add\n", token);
						add(new_row, *(SEMELS), semel_count, cmd, array);
						fprintf(stderr, "add function returned\n");
						break;
					}
				}
				fprintf(stderr, "Finished command check\n");
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
			fprintf(stderr, "No label found\n"); 
			for(i=0; i<16; i++)
			{
				if(strcmp(token, cmd[i].name)==0)
				{
					add(new_row, *(SEMELS),semel_count, cmd, array);
					break;
				}
			}
			if(strcmp(token, ".string")==0)
			{
				string(row, struct_DC);
			}
			else if(strcmp(token, ".mat")==0)
			{
				mat(row_copy , struct_DC);
			}
			else if(strcmp(token, ".data")==0)
			{
				data(row_copy , struct_DC);
			}
			else if(strcmp(token, ".entry")==0 ||	strcmp(token, ".extern")==0  )
			{
				entry_extern( row_copy );
			}
			else
				error=1;
		}
	}
}*/

void second_row_analysis(FILE * f , command cmd[]  ,command1 cmd1[],SEMEL*** SEMELS, int* semel_count, binary_code ** array, binary_directive **struct_DC)
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
						add(new_row, *(SEMELS), semel_count, cmd, array);
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
					add(row_copy, *(SEMELS),semel_count, cmd, array);
					break;
				}
			}
			if(strcmp(token, ".string")==0)
			{
				string(row, struct_DC);
			}
			else if(strcmp(token, ".mat")==0)
			{
				mat(row_copy , struct_DC);
			}
			else if(strcmp(token, ".data")==0)
			{
				data(row_copy , struct_DC);
			}
			else if(strcmp(token, ".entry")==0 ||	strcmp(token, ".extern")==0  )
			{
				entry_extern( row_copy );
			}
			else
				error=1;
		}
	}
}
			
			
