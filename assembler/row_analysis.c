#include "row_analysis.h"
#include "second_row_analysis.h"
#include "validation.h"

void row_analysis(FILE * f , int macro_count, macro** macros, command cmd[],command1 cmd1[],SEMEL*** SEMELS, int* semel_count)
{
	char row[MAX_LEN_OF_ROW+2];                     /* Buffer for reading each line from file (+1 for \n, +1 for \0) */
	char row_copy[MAX_LEN_OF_ROW+2];                /* Copy of row for tokenization (strtok modifies original) */
	char label[31];                                 /* Buffer to store extracted label name (max 30 chars + null) */
	char *token;                                    /* Pointer to current token from strtok */
	char *token2;                                   /* Pointer to second token (used for .entry/.extern arguments) */
	size_t len;                                     /* Length of current token for label detection */
	size_t i;                                       /* Loop counter for command array iterations */
	int valid_label;                                /* Flag indicating if label format is valid (1=valid, 0=invalid) */
	int j;                                          /* Loop counter for macro and command iterations */
	int type;                                       /* Symbol type: 0=instruction, 1=data, 2=extern */
	int addres;                                     /* Address value for symbol (IC for instructions, DC for data) */
	char *new_row;                                  /* Pointer to part of row after label (after colon) */
	 char *colon;                                   /* Pointer to colon character in label definition */
	int found_command;                              /* Flag indicating if valid command was found (1=found, 0=not found) */
	char* cmd_2_arg[] = {"mov","cmp","add", "sub","lea"};      /* Array of commands that take 2 arguments */
	char * cmd_1_arg[]={"not","clr","inc","dec","jmp","bne","red","prn","jsr"};  /* Array of commands that take 1 argument */
	char * cmd_0_arg[]={"rts","stop"};              /* Array of commands that take 0 arguments */
	

	/* Main loop: process each line from the input file */
	while (fgets(row, sizeof(row), f))
	{
		sum_of_row++;                               /* Increment global line counter for error reporting */
		/* Check if line exceeds maximum allowed length */
		if (strchr(row, '\n') == NULL) 
		{
            		fprintf(stderr,"error in line %d: line too long (more than %d characters)\n",sum_of_row, MAX_LEN_OF_ROW);
			error=1;                            /* Set global error flag */
			clear_row_arry();                   /* Clear remaining input from buffer */
			continue;                           /* Skip to next line */
		}
		/* Skip comment lines (lines starting with semicolon) */
		if(row[0]==';')
			continue;
		/* Create copy for tokenization and extract first token */
		strcpy(row_copy, row);
        	token = strtok(row_copy, " \t\n\r");        /* Extract first token using whitespace delimiters */
        	/* Skip empty lines */
		if(token == NULL)
			continue;
		valid_label=1;                              /* Initialize label validity flag to true */
        	/* Check if first token is a label (ends with colon) */
        	len = strlen(token);
        	if (len > 0 && token[len - 1] == ':')
        	{
			token[len-1]='\0';                  /* Remove colon from token to get label name */
			strcpy(label,token);                /* Copy label name to buffer */
			/* Validate label format */
			if (!is_valid_label_format(label)) 
			{
    				error = 1;                  /* Set global error flag */
    				valid_label = 0;            /* Mark label as invalid */
			}
			/* Check if label conflicts with macro names */
			if(valid_label==1)
			{
				for (j = 0; j < macro_count; j++) 
				{
					if (strcmp(label, macros[j]->name) == 0) 
					{
						fprintf(stderr, "error in line:%d, label cant be macro name: %s\n",sum_of_row, label);
						error=1;            /* Set global error flag */
						valid_label=0;      /* Mark label as invalid */
            					break;
					}
				}
			}	
			/* Check if label conflicts with instruction command names */
			if(valid_label==1)
			{
				for(i=0; i<NUM_OF_IC_COMMAND; i++)             /* cmd array has 16 instruction commands */
				{
					if(strcmp(label, cmd[i].name)==0)
					{
						valid_label=0;      /* Mark label as invalid */
						error=1;            /* Set global error flag */
						fprintf(stderr,"error in line:%d, label cannot be the name of a command: %s\n",sum_of_row, label);
						break;
					}
				}
			}
			/* Process valid labels */
			if(valid_label==1)
			{	/* Get next token after label to determine command type */
				token=strtok(NULL, " \t\n\r");
				if(token!=NULL)
				{	/* Check if label precedes .entry or .extern (which is invalid) */
					if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0) {
						fprintf(stderr, "error in line:%d, label cannot precede .entry or .extern: %s\n",sum_of_row, label);
						error = 1;          /* Set global error flag */
						continue;           /* Skip this line */
					}
					found_command=0;                /* Initialize command found flag */
					type=0;                         /* Default type: instruction */
					addres=IC;                      /* Default address: current instruction counter */
					/* Check if token matches instruction commands */
					for(i=0; i<NUM_OF_IC_COMMAND; i++)             /* cmd array has 16 instruction commands */
					{
						if(strcmp(token, cmd[i].name)==0)
						{
							type=0;             /* Type 0: instruction */
							addres=IC;          /* Address: instruction counter */
							found_command=1;    /* Mark command as found */
							break;
						}
					}
					/* Check if token matches directive commands */
				 	for (j = 0; j <NUM_OF_DC_COMMAND; j++)         /* cmd1 array has 5 directive commands */
					{
						if (strcmp(token, cmd1[j].name) == 0) 
						{
							type=1;             /* Type 1: data/directive */
							addres=DC;          /* Address: data counter */
							found_command=1;    /* Mark command as found */
							break;	
						}
					}
					/* If valid command found, add label to symbol table and process command */
					if(found_command)
					{	/* Add label to symbol table: name, type, address, ex_en=2 (regular label) */
						add_SEMEL(label, type, addres, SEMELS, semel_count, 2);
						if(SEMELS!=NULL)
						{	/* Extract part of line after colon for command processing */
				 			colon = strchr(row, ':');   /* Find colon in original line */
							if (colon != NULL) 
      								new_row = colon + 1;    /* Point to character after colon */
							else
								new_row=row;            /* Use entire row if no colon found */
							
							/* Process 2-argument commands */
							for(i=0; i<_2_ARG_COMMAND_IC; i++)              /* cmd_2_arg array has 5 commands */
							{
								if(strcmp(token, cmd_2_arg[i])==0)
								{	/* Create safe copy for processing */
									ic_count_2_arg(new_row);   /* Count instruction words needed */
									break;
								}
							}
							/* Process 1-argument commands */
				 			for (j = 0; j < _1_ARG_COMMAND_IC; j++)         /* cmd_1_arg array has 9 commands */
							{
								if (strcmp(token, cmd_1_arg[j]) == 0) 
								{	/* Create safe copy for processing */
									ic_count_1_arg(new_row);   /* Count instruction words needed */
									break;
								}
							}
							/* Process 0-argument commands */
							for (j = 0; j < _0_ARG_COMMAND_IC; j++)         /* cmd_0_arg array has 2 commands */
							{
								if (strcmp(token, cmd_0_arg[j]) == 0) 
								{
									IC++;               /* Increment instruction counter by 1 */
									break;
								}
							}
							/* Process .data directive */
							if(strcmp(token, ".data") == 0) 
							{	/* Create safe copy for processing */
								dc_count_data(new_row);    /* Count data words needed */
							}
							/* Process .string directive */
							if(strcmp(token, ".string") == 0) 
							{	/* Create safe copy for processing */
								dc_count_string(new_row);  /* Count string characters needed */
							}
							/* Process .mat directive */
							if(strcmp(token, ".mat") == 0) 
							{	/* Create safe copy for processing */
								dc_count_mat(new_row);     /* Count matrix elements needed */
							}
							/* Process .extern directive */
							if(strcmp(token, ".extern")==0)
							{
								token2 = strtok(NULL, " \t\n\r");  /* Get label name after .extern */
								if(token2==NULL)
								{
									error=1;        /* Set global error flag */
									fprintf(stderr, "missing label in func extern\n");
									continue;       /* Skip this line */
								}
								/* Add external label to symbol table */
								if (is_valid_label_format(token2))
									add_SEMEL(token2,2/*type for extern*/,0, SEMELS, semel_count, 1);
								else
								{
									fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
									error=1;        /* Set global error flag */
								}
							}
							/* Process .entry directive */
							if(strcmp(token, ".entry")==0)
					    		{
								token2 = strtok(NULL, " \t\n\r");  /* Get label name after .entry */
								if(token2==NULL)
								{
									error=1;        /* Set global error flag */
									fprintf(stderr, "missing label in func entry\n");
									continue;       /* Skip this line */
								}
								found_command = 1;  /* Mark as valid command */
								/* Add entry label to symbol table */
								if (is_valid_label_format(token2))
									add_SEMEL(token2,1/*type for entry*/,1, SEMELS, semel_count, 0);
								else
								{
									fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
									error=1;        /* Set global error flag */
								}
							}
						}
					}
					else
					{	/* No command found after valid label */
						fprintf(stderr,"error in line:%d, unknown command after label: %s\n", sum_of_row,token);
						error=1;                    /* Set global error flag */
					}
				}
			}
		}
		else 
		{		/* Line doesn't start with label - process command directly */
				found_command=0;                    /* Initialize command found flag */
				
				/* Check for 2-argument commands */
				for(i=0; i<_2_ARG_COMMAND_IC; i++)                  /* cmd_2_arg array has 5 commands */
				{
					if(strcmp(token, cmd_2_arg[i])==0)
					{
						found_command=1;        /* Mark command as found */
						ic_count_2_arg(row); /* Count instruction words needed */
						break;
					}
				}
				/* Check for 1-argument commands if not already found */
				if(!found_command) 
				{
					for (j = 0; j < _1_ARG_COMMAND_IC; j++)             /* cmd_1_arg array has 9 commands */
					{
						if (strcmp(token, cmd_1_arg[j]) == 0) 
						{
							found_command=1;        /* Mark command as found */
							ic_count_1_arg(row);    /* Count instruction words needed */
							break;
						}
					}
				}
				/* Check for 0-argument commands if not already found */
				if(!found_command) 
				{
					for (j = 0; j < _0_ARG_COMMAND_IC; j++)             /* cmd_0_arg array has 2 commands */
					{
						if (strcmp(token, cmd_0_arg[j]) == 0) 
						{
							found_command=1;        /* Mark command as found */
							IC++;                   /* Increment instruction counter by 1 */
							break;
						}
					}
				}
				/* Check for .data directive */
				if(strcmp(token, ".data") == 0) 
				{
					dc_count_data(row);             /* Count data words needed */
					found_command = 1;              /* Mark command as found */
				}
				/* Check for .string directive */
				if(strcmp(token, ".string") == 0) 
				{
					dc_count_string(row);          /* Count string characters needed */
					found_command = 1;              /* Mark command as found */
				}
				/* Check for .mat directive */
				if(strcmp(token, ".mat") == 0) 
				{
					dc_count_mat(row);              /* Count matrix elements needed */
					found_command = 1;              /* Mark command as found */
				}
				/* Process .extern directive */
				if(strcmp(token, ".extern")==0)
				{
					token2 = strtok(NULL, " \t\n\r");   /* Get label name after .extern */
					if(token2==NULL)
					{
						error=1;                /* Set global error flag */
						fprintf(stderr, "missing label in func extern\n");
						continue;               /* Skip this line */
					}
					found_command = 1;              /* Mark command as found */
					/* Add external label to symbol table */
					if (is_valid_label_format(token2))
						add_SEMEL(token2,2,0, SEMELS, semel_count, 1);
					else
					{
						fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
						error=1;                /* Set global error flag */
					}
				}
				/* Process .entry directive */
				if(strcmp(token, ".entry")==0)
				{
					token2 = strtok(NULL, " \t\n\r");   /* Get label name after .entry */
					if(token2==NULL)
					{
						error=1;                /* Set global error flag */
						fprintf(stderr, "error in line:%d missing label in func entry\n",sum_of_row);
						continue;               /* Skip this line */
					}
					found_command = 1;              /* Mark command as found */
					/* Add entry label to symbol table */
					if (is_valid_label_format(token2))
						add_SEMEL(token2,1,1, SEMELS, semel_count, 0);
					else
					{
						fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
						error=1;                /* Set global error flag */
					}
				}
				/* If no valid command was found, set error flag */
				if(!found_command)
					error=1;                    /* Set global error flag */
			}	
		}
 		/* Clean up macro memory after processing all lines */
		for (j = 0; j < macro_count; j++) 
		{
			if(macros[j])                           /* Check if macro pointer is not null */
			{
        			free(macros[j]->name);          /* Free macro name string */
        			free(macros[j]->content);       /* Free macro content string */
        			free(macros[j]);                /* Free macro structure */
			}
    		}
    		free(macros);                               /* Free macro array */
	}

int is_valid_label_format(const char* label) 
{
	size_t i;                                   /* Loop counter for character validation */
    	size_t len = strlen(label);                 /* Length of label string */
    
    	/* Check if label exceeds maximum allowed length */
    	if (len >MAX_LEN_OF_LABEL ) 
	{
        	fprintf(stderr, "error in line %d: label too long (max 30 characters): %s\n", sum_of_row,label);
		error=1;                            /* Set global error flag */
        	return 0;                           /* Return invalid */
    	}
    	/* Check if label starts with alphabetic character */
    	if (!isalpha(label[0])) 
	{
        	fprintf(stderr, "error in line:%d, label must start with a letter: %s\n",sum_of_row, label);
		error=1;                            /* Set global error flag */
        	return 0;                           /* Return invalid */
    	}
    	/* Check if all remaining characters are alphanumeric */
    	for(i = 1; i < len; i++) 
	{
        	if (!isalnum(label[i])) 
		{
            		fprintf(stderr, "error in line:%d, label can contain only letters and digits: %s\n", sum_of_row,label);
			error=1;                    /* Set global error flag */
            		return 0;               /* Return invalid */
        	}
    	}
    return 1;                                   /* Return valid */
}

void clear_row_arry() 
{
	char c;                                     /* Character buffer for input clearing */
	
	/* Clear remaining characters from input buffer until newline or EOF */
    	while ((c = getchar()) != '\n' && c != EOF);
}

void add_SEMEL(char* label, int type, int addres, SEMEL*** SEMELS, int* semel_count, int ex_en)
{
	SEMEL* new_SEMEL;                           /* Pointer to new symbol structure */
	SEMEL** temp;                               /* Temporary pointer for reallocation */
	int j;                                      /* Loop counter for symbol search */

	/* Check if symbol already exists in symbol table */
	if(SEMELS != NULL && *SEMELS != NULL)
	{
		for (j = 0; j<*semel_count; j++) 
		{
			if (strcmp(label, (*SEMELS)[j]->name) == 0) 
			{	/* Handle conflicts between regular and extern labels */
				if(((*SEMELS)[j]->ex_en==1 && ex_en==2 )||((*SEMELS)[j]->ex_en==2 && ex_en==1))
				{	
					fprintf(stderr, "error in line:%d a regular label cannot be an extern label and vice versa.\n",sum_of_row);
					error=1;            /* Set global error flag */
					return;
				}
				/* Handle conflicts between extern and entry labels */
				else if(((*SEMELS)[j]->ex_en==1 && ex_en==0) || ((*SEMELS)[j]->ex_en==0 && ex_en==1))
				{
					fprintf(stderr, "error in line:%d, a label cannot be both extern and entry.\n",sum_of_row);
					error=1;            /* Set global error flag */
					return;
				}
				/* Update extern label to entry */
				else if((*SEMELS)[j]->ex_en==2 &&  ex_en==0)
				{
					(*SEMELS)[j]->ex_en=ex_en;  /* Change extern to entry */      
					return;
				}
				/* Update regular label with address information */
				else if((*SEMELS)[j]->ex_en==0 && ex_en==2)
				{
					if(addres==IC)              /* If address is instruction counter */
						(*SEMELS)[j]->type=0;   /* Set type to instruction */
					(*SEMELS)[j]->addres=addres; /* Update address */
					return;
				}
				/* Handle duplicate regular label definitions */
				else if((*SEMELS)[j]->ex_en==2 && ex_en==2)
				{
					fprintf(stderr, "error in line:%d, label already defined: %s\n",sum_of_row, label);
					error = 1;          /* Set global error flag */
					return;
				}
			}
		}
	}
	/* Create new symbol structure */
	new_SEMEL = (SEMEL*)malloc(sizeof(SEMEL));
	if (new_SEMEL == NULL) 
	{
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;                      /* Set global error flag */
		return;
	}
	/* Allocate and copy label name */
        new_SEMEL->name = (char*)malloc(strlen(label) + 1);  /* +1 for null terminator */
        if (new_SEMEL->name == NULL) 
	{
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;                      /* Set global error flag */
                free(new_SEMEL);                /* Clean up partial allocation */
		return;
	}
        strcpy(new_SEMEL->name, label);         /* Copy label name to allocated memory */
	/* Set symbol properties */
        new_SEMEL->type = type;                 /* Symbol type: 0=instruction, 1=data, 2=extern */
	new_SEMEL->addres = addres;             /* Symbol address */
	new_SEMEL->ex_en = ex_en;               /* External/entry flag: 0=entry, 1=extern, 2=regular */
	/* Expand symbol table array */
	temp = realloc(*SEMELS, (*semel_count + 1) * sizeof(SEMEL*));
	if (!temp) 
	{
    		fprintf(stderr, "error, memory allocation failed\n");
    		error = 1;                      /* Set global error flag */
		free(new_SEMEL->name);          /* Clean up name allocation */
        	free(new_SEMEL);                /* Clean up structure allocation */
    		return;
	}
	*SEMELS = temp;                         /* Update symbol table pointer */
        (*SEMELS)[*semel_count] = new_SEMEL;    /* Add new symbol to table */
    	(*semel_count)++;                       /* Increment symbol count */
}

int has_two_square_bracket_pairs( char* str) 
{
    	int i = 0;                              /* Character position counter */
   	int bracket_count = 0;                  /* Counter for opening brackets found */
    	int inside = 0;                         /* Flag indicating if currently inside brackets (1=inside, 0=outside) */
    	
	/* Scan through entire string character by character */
	while (str[i] != '\0') 
	{
        	if (str[i] == '[') 
		{
            		if (inside) return 0;       /* Error: nested brackets not allowed */
            		inside = 1;             /* Mark as inside brackets */
            		bracket_count++;        /* Count this opening bracket */
        	}
        	else if (str[i] == ']') 
		{
            		if (!inside) return 0;      /* Error: closing bracket without opening */
            		inside = 0;             /* Mark as outside brackets */
        	}
        	i++;                            /* Move to next character */
    	}
    	/* Return true if exactly 2 bracket pairs found and all brackets are properly closed */
	return (bracket_count == 2) && (inside == 0);
}

void ic_count_1_arg(char* row) 
{
	char* op1;                              /* Pointer to first operand token */
    	
    	op1 = strtok(row, " \t");               /* Skip command name */
    	op1 = strtok(NULL, " \t\n\r");         /* Get first operand */
    	
    	/* Check if operand is missing */
    	if (op1 == NULL) 
	{
        	error = 1;                      /* Set global error flag */
        	IC += 1;                        /* Add 1 word for incomplete instruction */
		return;
    	}
	/* Check if operand uses matrix addressing (two bracket pairs) */
    	if (has_two_square_bracket_pairs(op1))
        	IC += 3;                        /* Matrix addressing needs 3 words total */
    	else
        	IC += 2;                        /* Regular addressing needs 2 words total */
}

void ic_count_2_arg(char* row) 
{
    	char* op1;                                  /* Pointer to first operand token */
    	char* op2;                                  /* Pointer to second operand token */

    	int ic_extra = 0;                           /* Counter for additional instruction words needed */
    
	/*if(!check_commas(row,0)) */                      /* Fix comma formatting in the row */
		/*return 0;*/
    	op1 = strtok(row, " \t");                   /* Skip command name */
    	op1 = strtok(NULL, ",");                    /* Get first operand (before comma) */
    	op2 = strtok(NULL, ",\n\r");               /* Get second operand (after comma) */

    	/* Check if either operand is missing */
    	if (op1 == NULL || op2 == NULL) 
    	{
        	error = 1;                              /* Set global error flag */
        	IC += 1;                                /* Add 1 word for incomplete instruction */
        	return;
    	}
	/* Remove leading whitespace from operands */
    	while (*op1 == ' ' || *op1 == '\t') 
		op1++;
    	while (*op2 == ' ' || *op2 == '\t') 
		op2++;
	IC += 1;  /* Add 1 word for the instruction itself (opcode + addressing modes) */
	/* Count words needed for first operand */
    	if (has_two_square_bracket_pairs(op1))
        	ic_extra += 2;                          /* Matrix addressing needs 2 additional words */
    	else
        	ic_extra += 1;                          /* Regular addressing needs 1 additional word */
	/* Count words needed for second operand */
    	if (has_two_square_bracket_pairs(op2))
        	ic_extra += 2;                          /* Matrix addressing needs 2 additional words */
    	else
        	ic_extra += 1;                          /* Regular addressing needs 1 additional word */
	/* Optimization: if both operands are registers, they can share one word */
    	if (reg(op1) && reg(op2))
        	ic_extra -= 1;                          /* Save 1 word when both are registers */
	IC += ic_extra;                             /* Add all additional words to instruction counter */
}


void dc_count_data(char* row) 
{
	char copy_row[256];		/* Local buffer for a safe copy of the row */
	char* p;				/* Pointer for iterating over the copied row */

	strcpy(copy_row, row);	/* Copy the input line to avoid modifying the original */

	p = copy_row;			/* Initialize pointer to the beginning of the copied row */
	while (*p != '\n' && *p != '\0') 
	{
		if (*p == ',') 
		{
			DC++;			/* Count each comma as a separator between values */
		}
		p++;				/* Move to the next character */
	}

	DC++;					/* Account for the last (or only) value after the last comma */
}


void dc_count_string(char* row) 
{
	char* command;                              /* Pointer to command token */
   	char* op1;                                  /* Pointer to string operand */
    	char* start_string;                         /* Pointer to opening quote */
    	char* end_string;                           /* Pointer to closing quote */
    	int string_length;                          /* Length of string content */
    
	/*if(!check_commas(row,0))*/                       /* Fix comma formatting in the row */
		/*return 0;*/
    	command = strtok(row, " \t");               /* Skip .string command */
    	if(command == NULL)
        	return;
    	op1 = strtok(NULL, "\n\r");                /* Get rest of line as string operand */
    	if (op1 == NULL) 
        	return;
    	/* Remove leading whitespace */
    	while (*op1 == ' ' || *op1 == '\t') 
		op1++;
    	/* Find string content between regular double quotes only */
    	start_string = strchr(op1, '"');           /* Find first double quote */
    	if (start_string == NULL) 
        	return;
    	end_string = strchr(start_string + 1, '"'); /* Find closing double quote */
    	if (end_string == NULL) 
        	return;
    	/* Calculate string length between quotes */
    	string_length = (end_string - start_string - 1);
    	DC += string_length + 1;  /* Add string length + 1 for null terminator '\0' */
}

void dc_count_mat(char* row) 
{
	int rows;                                   /* Number of matrix rows */
	int col;                                    /* Number of matrix columns */
    	char* token1;                               /* Pointer for parsing brackets */
	char* token2;                               /* Pointer for bracket end position */
	int i=0;                                    /* Character position counter */
	char row_str[225];                          /* Buffer for row dimension string */
	char col_str[225];                          /* Buffer for column dimension string */
	char* data;                                 /* Pointer to matrix data values */
	int data_count=0;                           /* Counter for actual data values provided */

	/* Find first opening bracket for row dimension */
	token1 = strchr(row, '[');
	if(token1==NULL) 
		return;                             /* Exit if no opening bracket found */
	/* Find corresponding closing bracket */
    	token2 = strchr(token1, ']');
	if(token2==NULL) 
		return;                             /* Exit if no closing bracket found */
	token1++;                               /* Move past opening bracket */
	/* Extract row dimension digits between brackets */
	while(token1<token2 && *token1>= '0' && *token1 <= '9')
	{
		row_str[i++]= *token1;              /* Copy digit to row string buffer */
		token1++;
	}
	row_str[i]='\0';                        /* Null-terminate the row string */
	rows= atoi(row_str);                    /* Convert string to integer */
	/* Find second opening bracket for column dimension */
	token1 = strchr(token2, '[');
	if(token1==NULL) 
		return;                             /* Exit if second opening bracket not found */
	/* Find second closing bracket */
	token2 = strchr(token1, ']');
	if(token2==NULL) 
		return;                             /* Exit if second closing bracket not found */
	i=0;                                    /* Reset counter for column parsing */
	token1++;                               /* Move past second opening bracket */
	/* Extract column dimension digits between brackets */
	while(token1<token2 && *token1>= '0' && *token1 <= '9')
	{
		col_str[i++]= *token1;              /* Copy digit to column string buffer */
		token1++;
	}
	col_str[i]='\0';                        /* Null-terminate the column string */
	col= atoi(col_str);                     /* Convert string to integer */
	/* Point to data section after second closing bracket */
	data=token2+1;
	/* Tokenize data values using comma and whitespace delimiters */
	data = strtok(data, " ,\t\n\r");
	if(data==NULL)	/* No data provided - allocate space for full matrix */
		DC+=col*rows;                       /* Add total matrix size to data counter */
	else
	{	/* Count actual data values provided */
		while(data!=NULL)
		{
			data_count++;               /* Increment data value counter */
			data = strtok(NULL, " ,\t\n\r");  /* Get next data token */
		}
	DC+= data_count;                        /* Add actual data count to data counter */
	}
}
