#include "validation.h"

void update_data_symbol_addresses(SEMEL** semels, int* semel_count) 
{
	int i;                                     /* Loop counter for symbol table iteration */
    	for (i = 0; i < *semel_count; i++) 
	{	
		/* Check if entry label is undefined (address == 3 indicates undefined entry) */
		/* Entry labels must be defined in the current file to be exported */
		if(semels[i]->ex_en==ex_en_en_label)
		{
			count_of_entry_labels++;
			if(semels[i]->addres==type_en)
			{
				error=1;                    /* Set global error flag */
				fprintf(stdout,"error in line :%d ,entry label has to be defined %s\n",sum_of_row,semels[i]->name);
			}
		}
        	/* Update data symbol addresses */
        	if (semels[i]->type == 1)           /* Type 1 indicates data symbol */
        	{	
			/* Adjust data address by adding final IC value */
        		/* This places data symbols after all instructions in memory */
            		semels[i]->addres += IC;    /* IC = final instruction counter value */
        	}
    	}
}

int check_commas(char *line)
{
	int i =0;					/* Current position index in the line */
	int comma_count = 0;			/* Counter for consecutive commas */
	int error_found = 0;			/* Flag to track if any errors were detected */
	
	/* Skip leading whitespace at start of line */
	while (isspace((unsigned char)line[i]))
		i++;
	
	/* Skip the command name (until whitespace or null terminator) */
	/* This moves past the instruction/directive name */
	while (line[i] && !isspace((unsigned char)line[i]))
		i++;
	
	/* Skip spaces after command name */
	while (isspace((unsigned char)line[i]))
		i++;
	
	/* RULE 1: If first non-space after command is a comma → syntax error */
	/* Example error: "mov ,r1,r2" or "add ,#5" */
	if (line[i] == ',')
	{
		fprintf(stdout, "error in line:%d: Comma immediately after command.\n", sum_of_row);
		error = 1;				/* Set global error flag */
		return 0;				/* Return immediately - this is a critical error */
	}
	
	/* Main validation loop: scan through operands and commas */
	while (line[i] && line[i] != '\n')
	{	
		/* RULE 2: Check for multiple consecutive commas */
		if (line[i] == ',') 
		{
			comma_count = 1;		/* Found first comma */
			i++;					/* Move past first comma */
			
			/* Count additional consecutive commas */
			while (line[i] == ',')
			{
				comma_count++;		/* Count each additional comma */
				i++;				/* Move to next character */
			}
			
			/* Multiple consecutive commas is a syntax error */
			/* Example errors: "mov r1,,r2" or "add #5,,,r3" */
			if (comma_count > 1)
			{
				fprintf(stdout, "error in line:%d: Multiple consecutive commas.\n", sum_of_row);
				error = 1;
				error_found = 1;	/* Mark error but continue checking */
			}
			
			/* Skip whitespace after comma(s) */
			while (isspace((unsigned char)line[i]))
				i++;
			
			/* RULE 3: Comma at end of line is syntax error */
			/* Example errors: "mov r1,r2," or "data 5,10," */
			if (line[i] == '\0' || line[i] == '\n')
			{
				fprintf(stdout, "error in line:%d: Comma at end of line.\n", sum_of_row);
				error = 1;
				error_found = 1;
			}
			continue;				/* Continue to next iteration */
		}
		
		/* Skip over operand token (non-comma, non-whitespace characters) */
		/* This moves past register names, numbers, symbols */
		while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i]))
			i++;
		
		/* Skip whitespace between operands */
		while (isspace((unsigned char)line[i]))
			i++;
	}
	
	/* Return validation result */
	return error_found ? 0 : 1;		/* 0 if errors found, 1 if syntax is valid */
}

void error_allocation(int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels)
{
    	int j;                                      /* Loop counter for array iterations */

    	/* Free SEMELs array: array of pointers */
    	if (semels && semel_count && *semel_count > 0) 
	{
       		for (j = 0; j < *semel_count; j++) 
		{
            		if (semels[j]) 
			{
                		free(semels[j]->name);      /* Free symbol name string */
                		free(semels[j]);            /* Free symbol structure */
            		}
        	}
        	free(semels);                           /* Free symbol array */
    	}
    	
    	/* Free extern labels: array of pointers */
    	if (extern_labels && count_of_extern_labels && *count_of_extern_labels > 0) 
	{
        	for (j = 0; j < *count_of_extern_labels; j++) 
		{
            		if (extern_labels[j]) 
			{
                		free(extern_labels[j]->name);   /* Free external label name string */
                		free(extern_labels[j]);         /* Free external label structure */
            		}
        	}
        	free(extern_labels);                    /* Free external labels array */
    	}
    	
	/* Free instruction buffer (single block) */
    	if (array && *array) 
        	free(*array);
    	
    	/* Free data buffer (single block) */
    	if (struct_DC && *struct_DC) 
        	free(*struct_DC);
    	
    	/* Free macros: array of pointers */
    	if (macros && *macros && macro_count)
	{
        	for (j = 0; j < macro_count; j++) 
		{
            		if ((*macros)[j]) 
			{
                		free((*macros)[j]->name);       /* Free macro name string */
                		free((*macros)[j]->content);    /* Free macro content string */
                		free((*macros)[j]);             /* Free macro structure */
            		}
        	}
        	free(*macros);                          /* Free macros array */
    	}
    	
    	/* Terminate program execution after cleanup */
    	/* fprintf(stderr, "Fatal: allocation error, resources freed.\n"); */
    	exit(1);
}
