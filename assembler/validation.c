#include "validation.h"

void  update_data_symbol_addresses(SEMEL** semels, int* semel_count) 
{
	int i;                                     /* Loop counter for symbol table iteration */
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

int check_commas(char *line)
{
	int i =0;
	int comma_count = 0;
	int error_found = 0;

	/* Skip leading whitespace */
	while (isspace((unsigned char)line[i]))
		i++;
	/* Skip the command name (until whitespace or null) */
	while (line[i] && !isspace((unsigned char)line[i]))
		i++;
	/* Skip spaces after command */
	while (isspace((unsigned char)line[i]))
		i++;
	/* If first non-space after command is a comma → error */
	if (line[i] == ',')
	{
		fprintf(stderr, "error in line:%d: Comma immediately after command.\n", sum_of_row);
		error = 1;
		return 0;
	}
	/* Main loop to check for multiple commas or trailing comma */
	while (line[i] && line[i] != '\n')
	{
		/* Multiple consecutive commas */
			if (line[i] == ',') 
			{
				comma_count = 1;
				i++;
				while (line[i] == ',')
				{
					comma_count++;
					i++;
				}
				if (comma_count > 1)
				{
					fprintf(stderr, "error in line:%d: Multiple consecutive commas.\n", sum_of_row);
					error = 1;
					error_found = 1;
				}
				/* Skip whitespace after comma */
				while (isspace((unsigned char)line[i]))
					i++;

				/* Comma at end of line */
				if (line[i] == '\0' || line[i] == '\n')
				{
					fprintf(stderr, "error in line:%d: Comma at end of line.\n", sum_of_row);
					error = 1;
					error_found = 1;
				}
				continue;
			}
		/* Skip operand token */
		while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i]))
			i++;
		/* Skip whitespace between operands */
		while (isspace((unsigned char)line[i]))
			i++;
	}
	return error_found ? 0 : 1;
}

