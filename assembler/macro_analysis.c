#include "assembler.h"
#include "chang_file_name.h"

FILE* macro_analysis(FILE* f1, command cmd[], command1 cmd1[], int argc, char *argv[], int i,macro*** macros_out, int* macro_count_out) 
{
	char row[MAX_LEN_OF_ROW+2];         /* Buffer to store each line read from input file (+2 for safety margin) */
    	char row_copy[MAX_LEN_OF_ROW+2];    /* Copy of current line for tokenization (strtok modifies original) */
    	char* token;                        /* Pointer to current token extracted by strtok */
    	char* macro_name;                   /* Pointer to macro name extracted from mcro definition line */
    	char* line_token;                   /* Pointer to token used when skipping invalid macro content */
    	macro** macros = NULL;              /* Dynamic array of pointers to macro structures */
    	macro* new_macro;                   /* Pointer to newly allocated macro structure */
    	int macro_count = 0;                /* Counter tracking number of defined macros */
    	int matched;                        /* Flag indicating if current line matches a macro name (1=yes, 0=no) */
    	int is_command;                     /* Flag indicating if macro name conflicts with command name (1=conflict, 0=ok) */
    	int j;                              /* Loop counter for iterating through command arrays and macro list */
    	FILE* f2;                           /* Output file pointer for .am file (macro-expanded file) */
	char* temp_file_name;               /* Temporary string for constructing file name for deletion */
	
	/* Create output file with .am extension for macro-expanded assembly code */
	f2= end_file_name(0, argv, i,AM);

    	if (f2 == NULL) 
	{
		fprintf(stderr, "Failed to create .am file for %s\n", argv[i]);
		fclose(f1);  /* Close input file if output creation failed */
        	return NULL;
        }
        
    	/* Main loop: process each line from input file */
    	while (fgets(row, MAX_LEN_OF_ROW, f1) != NULL) 
	{
		sum_of_row++;                   /* Increment global line counter for error reporting */
        	strcpy(row_copy, row);          /* Create copy for tokenization (strtok destroys original) */
        	token = strtok(row_copy, " \t\r\n");  /* Extract first token, delimited by spaces, tabs, CR, LF */
        	matched = 0;                    /* Initialize macro match flag to false */

        	/* Check if current line starts a macro definition */
        	if (token != NULL && strcmp(token, "mcro") == 0) 
		{
            		/* Extract macro name from definition line */
            		macro_name = strtok(NULL, " \t\r\n");  /* Get next token after "mcro" */
            		
            		/* Validate macro definition syntax - should have exactly one name after "mcro" */
            		if (macro_name == NULL || strtok(NULL, " \t\r\n") != NULL) 
			{
                		fprintf(stderr, "in row %d error, invalid macro definition\n" ,sum_of_row);
                		error = 1;          /* Set global error flag */
                		continue;           /* Skip to next line */
            		}

            		/* Check if macro name conflicts with existing command names */
            		is_command = 0;         /* Initialize conflict flag to false */
            		
            		/* Check against instruction commands (cmd array has 16 elements) */
            		for (j = 0; j < NUM_OF_IC_COMMAND; j++)
			{
                		if (strcmp(cmd[j].name, macro_name) == 0) 
					is_command = 1;     /* Found conflict with instruction */
			}
			
            		/* Check against directive commands (cmd1 array has 5 elements) */
            		for (j = 0; j < NUM_OF_DC_COMMAND && !is_command; j++)
			{
                		if (strcmp(cmd1[j].name, macro_name) == 0)
					is_command = 1;     /* Found conflict with directive */
            		}

            		/* Handle macro name conflict with commands */
            		if (is_command) 
			{
                		fprintf(stderr, "int line %d error: Invalid macro name (conflicts with command)\n" ,sum_of_row);
                		error = 1;          /* Set global error flag */
                		
                		/* Skip all content until mcroend to avoid processing invalid macro */
                		while (fgets(row, sizeof(row), f1)) 
				{
                    			strcpy(row_copy, row);                      /* Copy for tokenization */
                    			line_token = strtok(row_copy, " \t\r\n");   /* Get first token */
                    			if (line_token && strcmp(line_token, "mcroend") == 0) 
						break;  /* Found end of invalid macro */
                		}
                		continue;           /* Skip to next line after mcroend */
            		}

            		/* Allocate memory for new macro structure */
            		new_macro = (macro*)malloc(sizeof(macro));
            		if (new_macro == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;          /* Set global error flag */
                		continue;           /* Skip macro creation */
            		}

            		/* Allocate and copy macro name */
            		new_macro->name = (char*)malloc(strlen(macro_name) + 1);  /* +1 for null terminator */
            		if (new_macro->name == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;          /* Set global error flag */
                		free(new_macro);    /* Clean up partially allocated macro */
                		continue;           /* Skip macro creation */
            		}
            		strcpy(new_macro->name, macro_name);  /* Copy macro name to allocated memory */

            		/* Initialize macro content as empty string */
            		new_macro->content = (char*)calloc(1, sizeof(char));  /* Allocate 1 byte initialized to 0 */
            		if (new_macro->content == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;          /* Set global error flag */
                		free(new_macro->name);  /* Clean up macro name memory */
                		free(new_macro);        /* Clean up macro structure */
                		continue;               /* Skip macro creation */
            		}

            		/* Read and accumulate macro content until mcroend */
            		while (fgets(row, MAX_LEN_OF_ROW, f1) != NULL) 
			{
                		strcpy(row_copy, row);                      /* Copy for tokenization */
                		token = strtok(row_copy, " \t\r\n");       /* Get first token */

                		/* Check for macro end marker */
                		if (token != NULL && strcmp(token, "mcroend") == 0) 
				{
                    			/* Validate mcroend syntax - should have no extra text */
                    			if (strtok(NULL, " \t\r\n") != NULL) 
					{
                        			fprintf(stderr, "in line %d error, extra text after 'mcroend'\n",sum_of_row);
                        			error = 1;  /* Set global error flag */
                    			}
                    			break;  /* Exit macro content reading loop */
                		}

                		/* Expand macro content buffer to accommodate new line */
                		new_macro->content = (char*)realloc(new_macro->content,
                    		strlen(new_macro->content) + strlen(row) + 1);  /* +1 for null terminator */
                		if (new_macro->content == NULL) 
				{
                    			fprintf(stderr, "error, memory allocation failed\n");
                    			error = 1;      /* Set global error flag */
                    			break;          /* Exit content reading loop */
                		}
                		strcat(new_macro->content, row);  /* Append current line to macro content */
            		}

            		/* Add completed macro to macros array */
            		macros = (macro**)realloc(macros, (macro_count + 1) * sizeof(macro*));  /* Expand array */
            		if (macros == NULL) 
			{
                		fprintf(stderr, "error, memory allocation failed\n");
                		error = 1;          /* Set global error flag */
                		continue;           /* Skip adding macro to array */
            		}
            		macros[macro_count] = new_macro;    /* Store pointer to new macro */
            		macro_count++;                      /* Increment macro counter */
            		continue;                           /* Move to next line */
        	}
		
        	/* Check if current line starts with a macro name (macro expansion) */
        	if (token != NULL) 
		{
            		/* Search through all defined macros for name match */
            		for (j = 0; j < macro_count; j++) 
			{
                		if (strcmp(token, macros[j]->name) == 0) 
				{
                    			/* Found matching macro - expand by writing its content to output */
                    			fprintf(f2, "%s", macros[j]->content);
                    			matched = 1;        /* Set match flag to true */
                    			break;              /* Exit search loop */
                		}
            		}
        	}

        	/* If no macro match found, write original line to output */
        	if (!matched)
            		fprintf(f2, "%s", row);         /* Copy line unchanged to output file */
        }

    	fclose(f1);     /* Close input file after processing all lines */
    		
	/* Handle function completion based on error status */
	if (error == 0) 
	{
    		rewind(f2);                     /* Reset file pointer to beginning of output file */
    		*macros_out = macros;           /* Return macro array through output parameter */
    		*macro_count_out = macro_count; /* Return macro count through output parameter */
    		return f2;                      /* Return file pointer for further processing */
	} 
	else 
	{
    		fclose(f2);     /* Close output file */
    			
    		/* Delete the created .am file since errors occurred */
    		temp_file_name = (char*)malloc(strlen(argv[i]) + strlen(END_FILE_NAME_AM) + 1);  /* +1 for null terminator */
    		if (temp_file_name != NULL) 
		{
        		strcpy(temp_file_name, argv[i]);        /* Copy base file name */
        		strcat(temp_file_name, END_FILE_NAME_AM); /* Append .am extension */
        		remove(temp_file_name);                 /* Delete the file from filesystem */
        		free(temp_file_name);                   /* Clean up temporary string memory */
    		}
    		return NULL;    /* Return null to indicate failure */
		}
}
