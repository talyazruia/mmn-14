#include "assembler.h"


FILE* macro_analysis(FILE* f1, command cmd[], command1 cmd1[], int argc, char *argv[], int i,macro*** macros_out, int* macro_count_out) 
{
	char row[MAX_LEN_OF_ROW+2];
    	char row_copy[MAX_LEN_OF_ROW+2];
    	char* token;
    	char* macro_name;
    	char* line_token;
    	macro** macros = NULL;
    	macro* new_macro;
    	int macro_count = 0;
    	int matched;
    	int is_command;
    	int j;
    	FILE* f2;
	char* temp_file_name;
	
	f2= end_file_name(0, argv, i,1);

    	if (f2 == NULL) 
	{
		fprintf(stderr, "Failed to create .am file for %s\n", argv[i]);
		fclose(f1);
        	return NULL;
        }
    	while (fgets(row, MAX_LEN_OF_ROW, f1) != NULL) 
	{
		sum_of_row++;
        	strcpy(row_copy, row);
        	token = strtok(row_copy, " \t\r\n");
        	matched = 0;

        	if (token != NULL && strcmp(token, "mcro") == 0) 
		{
            		macro_name = strtok(NULL, " \t\r\n");
            		if (macro_name == NULL || strtok(NULL, " \t\r\n") != NULL) 
			{
                		fprintf(stderr, "in row %d error, invalid macro definition\n" ,sum_of_row);
                		error = 1;
                		continue;
            		}

            		is_command = 0;
            		for (j = 0; j < 16; j++)
			{
                		if (strcmp(cmd[j].name, macro_name) == 0) 
					is_command = 1;
			}
            		for (j = 0; j < 5 && !is_command; j++)
			{
                		if (strcmp(cmd1[j].name, macro_name) == 0)
					is_command = 1;
            		}

            		if (is_command) 
			{
                		fprintf(stderr, "int line %d error: Invalid macro name (conflicts with command)\n" ,sum_of_row);
                		error = 1;
                		while (fgets(row, sizeof(row), f1)) 
				{
                    			strcpy(row_copy, row);
                    			line_token = strtok(row_copy, " \t\r\n");
                    			if (line_token && strcmp(line_token, "mcroend") == 0) 
						break;
                		}
                		continue;
            		}

            		new_macro = (macro*)malloc(sizeof(macro));
            		if (new_macro == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;
                		continue;
            		}

            		new_macro->name = (char*)malloc(strlen(macro_name) + 1);
            		if (new_macro->name == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;
                		free(new_macro);
                		continue;
            		}
            		strcpy(new_macro->name, macro_name);

            		new_macro->content = (char*)calloc(1, sizeof(char));
            		if (new_macro->content == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;
                		free(new_macro->name);
                		free(new_macro);
                		continue;
            		}

            		while (fgets(row, MAX_LEN_OF_ROW, f1) != NULL) 
			{
                		strcpy(row_copy, row);
                		token = strtok(row_copy, " \t\r\n");

                		if (token != NULL && strcmp(token, "mcroend") == 0) 
				{
                    			if (strtok(NULL, " \t\r\n") != NULL) 
					{
                        			fprintf(stderr, "in line %d error, extra text after 'mcroend'\n",sum_of_row);
                        			error = 1;
                    			}
                    			break;
                		}

                		new_macro->content = (char*)realloc(new_macro->content,
                    		strlen(new_macro->content) + strlen(row) + 1);
                		if (new_macro->content == NULL) 
				{
                    			fprintf(stderr, "error, memory allocation failed\n");
                    			error = 1;
                    			break;
                		}
                		strcat(new_macro->content, row);
            		}

            		macros = (macro**)realloc(macros, (macro_count + 1) * sizeof(macro*));
            		if (macros == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;
                		continue;
            		}
            		macros[macro_count] = new_macro;
            		macro_count++;
            		continue;
        	}
		
        	if (token != NULL) 
		{
            		for (j = 0; j < macro_count; j++) 
			{
                		if (strcmp(token, macros[j]->name) == 0) 
				{
                    			fprintf(f2, "%s", macros[j]->content);
                    			matched = 1;
                    			break;
                		}
            		}
        	}

        	if (!matched)
            		fprintf(f2, "%s", row);
        	}

    		fclose(f1);
		if (error == 0) 
		{
    			rewind(f2);
    			*macros_out = macros;
    			*macro_count_out = macro_count;
    			return f2;  
		} 
		else 
		{
    			fclose(f2);
    			temp_file_name = (char*)malloc(strlen(argv[i]) + strlen(END_FILE_NAME_AM) + 1);
    			if (temp_file_name != NULL) 
			{
        			strcpy(temp_file_name, argv[i]);
        			strcat(temp_file_name, END_FILE_NAME_AM);
        			remove(temp_file_name);  
        			free(temp_file_name);
    			}
    			return NULL;  
		
	}
}
