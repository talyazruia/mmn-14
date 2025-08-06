#include "validation.h"

void  update_data_symbol_addresses(SEMEL** semels, int* semel_count) 
{
	int i;                                      /* Loop counter for symbol table iteration */
    	for (i = 0; i < *semel_count; i++) 
	{	/* Check if entry label is undefined (address == 3 indicates undefined entry) */
		if(semels[i]->ex_en==0 && semels[i]->addres==3)
		{
			error=1;                    /* Set global error flag */
			fprintf(stderr,"error in line :%d ,entry label has to be defined %s\n",sum_of_row,semels[i]->name);
		}
        	if (semels[i]->type == 1)           /* Type 1 indicates data symbol */
            		semels[i]->addres += IC;    /* Adjust data address by adding final IC value */
    	}
}

void fix_commas_in_place(char *line) 
{
    	char temp[MAX_LEN_OF_ROW + 1];              /* Temporary output line buffer, including '\0' */
    	int i = 0;                                  /* Index for input line */
    	int j = 0;                                  /* Index for output buffer */
    	int in_operand = 0;                         /* Flag indicating if currently inside operand (1=yes, 0=no) */
    
    	while (isspace((unsigned char)line[i]))     /* Skip leading whitespace */
        	i++;
        
    	while (line[i] && !isspace((unsigned char)line[i]) && line[i] != ',') /* Copy command name */
	{
        	if (j < MAX_LEN_OF_ROW) 
            		temp[j++] = line[i++];
        	else 
            		break;
    	}
    	if (j < MAX_LEN_OF_ROW)                    /* Add space after command name */
        	temp[j++] = ' ';
    	while (isspace((unsigned char)line[i]))   /* Skip whitespace after command name */
        	i++;
    	while (line[i]) 
	{
        	if (line[i] == ',')                   /* Process comma characters */
		{
            		if (!in_operand)                /* Error: comma without preceding operand */
			{
                		error = 1;
                		fprintf(stderr, "error in line:%d: Unexpected comma (e.g., at start or double commas).\n",sum_of_row);
                		while (line[i] == ',')        /* Skip consecutive commas */
                    			i++;
                		continue;
            		}
           	 	if (j + 2 < MAX_LEN_OF_ROW)       /* Add properly formatted comma with space */
			{
                		temp[j++] = ',';
                		temp[j++] = ' ';
            		}
            		in_operand = 0;                     /* Reset operand flag after comma */
            		i++;
            		while (isspace((unsigned char)line[i]))  /* Skip whitespace after comma */
                		i++;
            		continue;
        	}
        	if (isspace((unsigned char)line[i]))   /* Skip whitespace within operands */
		{
            		i++;
            		continue;
        	}
        	if (!in_operand)                      /* Mark start of new operand */
            		in_operand = 1;
        	while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i]))  /* Copy operand characters */
		{
            		if (j < MAX_LEN_OF_ROW) 
                		temp[j++] = line[i++];
            	else
                	break;
        	}
        	while (isspace((unsigned char)line[i]))  /* Skip whitespace after operand */
            		i++;
    	}
    
    	if (j >= 2 && temp[j - 2] == ',' && temp[j - 1] == ' ')  /* Remove trailing comma if present */
	{
        	j -= 2;
        	error = 1;
        	fprintf(stderr, "error in line:%d Comma at end of line.\n",sum_of_row);
    	}
    	temp[j] = '\0';                             /* Null-terminate the processed string */
    	strncpy(line, temp, MAX_LEN_OF_ROW);        /* Copy processed string back to original */
    	line[MAX_LEN_OF_ROW] = '\0';                /* Ensure proper termination */
}
