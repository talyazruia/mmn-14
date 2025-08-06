#include "encoding.h"
#include "second_row_analysis.h"
#include "row_analysis.h"
#include "validation.h"

void second_row_analysis(FILE * f , command cmd[]  ,SEMEL*** SEMELS, int* semel_count, binary_code ** array, binary_directive **struct_DC,extern_label** extern_labels,int * count_of_extern_labels)
{
	char row[MAX_LEN_OF_ROW];                       /* Buffer for reading each line from file */
	char row_copy[MAX_LEN_OF_ROW];                  /* Copy of row for tokenization (strtok modifies original) */
	char *token;                                    /* Pointer to current token from strtok */
	size_t len;                                     /* Length of current token for label detection */
	int i=0;                                        /* Loop counter for command array iterations */
	char *new_row;                                  /* Pointer to part of row after label (after colon) */
	char *colon;                                    /* Pointer to colon character in label definition */
	int found=0;
                                    /* Flag indicating if valid command was found (1=found, 0=not found) */
	
	/* Second pass: process each line from the input file for code generation */
	while (fgets(row, sizeof(row), f))
	{	sum_of_row++;                           /* Increment global line counter for error reporting */
		found=0;
		/* Check if line exceeds maximum allowed length */
		if (strchr(row, '\n') == NULL) 
		{
			error=1;                        /* Set global error flag */
			clear_row_arry();               /* Clear remaining input from buffer */
			continue;                       /* Skip to next line */
		}
		/* Skip comment lines (lines starting with semicolon) */
		if(row[0]==';')
			continue;
		/* Create copy for tokenization and extract first token */
		strcpy(row_copy, row);
        	token = strtok(row_copy, " \t\n\r");    /* Extract first token using whitespace delimiters */
        	/* Skip empty lines */
		if(token == NULL)
			continue;
		/* Check if first token is a label (ends with colon) */
		len = strlen(token);
        	if (len > 0 && token[len - 1] == ':')
        	{	/* Get next token after label to determine command type */
			token=strtok(NULL, " \t\n\r");
			if(token!=NULL)
			{	/* Extract part of line after colon for command processing */
				colon = strchr(row, ':');       /* Find colon in original line */
				if (colon != NULL) 
      					new_row = colon + 1;    /* Point to character after colon */
				else
					new_row=row;            /* Use entire row if no colon found */
					
				/* Check if token matches instruction commands */
				for(i=0; i<16; i++)             /* cmd array has 16 instruction commands */
				{
					if(strcmp(token, cmd[i].name)==0)
					{	/* Process instruction command and generate binary code */
						IC_command_analysis(new_row, *(SEMELS), semel_count, cmd, array, extern_labels, count_of_extern_labels);
						found=1;            /* Mark command as found */
						break;
					}
				}
				/* Process .string directive */
				if(strcmp(token, ".string")==0)
					string(row, struct_DC);     /* Generate binary data for string */
				/* Process .mat directive */
				else if(strcmp(token, ".mat")==0)
					mat(new_row , struct_DC);   /* Generate binary data for matrix */
				/* Process .data directive */
				else if(strcmp(token, ".data")==0)
					data(new_row , struct_DC);  /* Generate binary data for data values */
				/* Process .entry or .extern directives */
				else if(strcmp(token, ".entry")==0 ||	strcmp(token, ".extern")==0  )
					entry_extern( new_row );    /* Validate directive syntax */
				/* Handle unknown commands after valid label */
				else if(found==0)
				{
					error=1;                    /* Set global error flag */
					fprintf(stderr,"eror in line:%d unkown command",sum_of_row);
				}
			}
		}
		else
		{	/* Line doesn't start with label - process command directly */
			/* Check if token matches instruction commands */
			for(i=0; i<16; i++)                 /* cmd array has 16 instruction commands */
			{
				if(strcmp(token, cmd[i].name)==0)
				{	/* Process instruction command and generate binary code */
					IC_command_analysis(row, *(SEMELS),semel_count, cmd, array,extern_labels, count_of_extern_labels);
					found=1;                /* Mark command as found */
					break;
				}
			}
			/* Process .string directive */
			if(strcmp(token, ".string")==0)
				string(row, struct_DC);         /* Generate binary data for string */
			/* Process .mat directive */
			else if(strcmp(token, ".mat")==0)
				mat(row, struct_DC);            /* Generate binary data for matrix */
			/* Process .data directive */
			else if(strcmp(token, ".data")==0)
				data(row , struct_DC);          /* Generate binary data for data values */
			/* Process .entry or .extern directives */
			else if(strcmp(token, ".entry")==0 ||	strcmp(token, ".extern")==0  )
				entry_extern(row );             /* Validate directive syntax */
			/* Handle unknown commands */
			else if(found==0)
			{
					error=1;                /* Set global error flag */
					fprintf(stderr,"error in line:%d unkown command\n",sum_of_row);
			}
		}
	}
}

int is_valid_number(char *str, int mode) 
{
	char *src, *dst, *endptr;
	long val;

	if (str == NULL)
		return 0;

	/* Trim leading whitespace */
	while (isspace((unsigned char)*str))
		str++;

	/* Move trimmed string to the beginning (overwrite original) */
	src = str;
	dst = str;
	while (*src != '\0')
		*dst++ = *src++;
	*dst = '\0';

	/* Now remove trailing whitespace */
	dst = str + strlen(str) - 1;
	while (dst >= str && isspace((unsigned char)*dst))
		*dst-- = '\0';

	/* Mode 0: check '#' at beginning, and no space after it */
	if (mode == 0) 
	{
		if (*str != '#')
			return 0;

		if (isspace((unsigned char)str[1])) /* space after # not allowed */
			return 0;

		/* Shift string left to remove '#' */
		memmove(str, str + 1, strlen(str));  /* including null terminator */
	}

	/* Handle optional sign */
	if (*str == '+' || *str == '-') 
	{
		if (mode == 2 && *str == '-')  /* unsigned can't be negative */
			return 0;
	}

	/* Validate all characters */
	src = str;
	while (*src && !isspace((unsigned char)*src)) 
	{
		if (*src == '+' || *src == '-') 
		{
			src++; /* allow sign */
			continue;
		}
		if (!isdigit((unsigned char)*src))
			return 0;
		src++;
	}

	/* Check for extra characters after number (excluding whitespace) */
	while (*src)
	{
		if (!isspace((unsigned char)*src))
			return 0;
		src++;
	}

	/* Final parse to check range */
	if (mode == 1 || mode == 2) 
	{
		val = strtol(str, &endptr, 10);
		if (str == endptr)
			return 0;

		/* Skip trailing whitespace */
		while (isspace((unsigned char)*endptr))
			endptr++;

		if (*endptr != '\0')
			return 0;

		if (val < INT_MIN || val > INT_MAX)
			return 0;
	}
	return 1;
}

/* Check if the given string represents a valid register (r0 to r7), ignoring surrounding spaces */
int reg(char* str)
{
	char* end;
	int len;

	if (str == NULL)
		return 0;
	/* Trim leading spaces */
	while (isspace(*str)) 
		str++;
	/* Trim trailing spaces */
	len = strlen(str);
	end = str + len - 1;
	while (end > str && isspace(*end)) 
	{
		*end = '\0';
		end--;
	}
	/* After trimming, check the register format */
	if (str[0] != 'r')
		return 0;
	if (str[1] < '0' || str[1] > '7')
		return 0;
	if (str[2] != '\0')
		return 0;
	return 1;
}

int* valid_matrix(char* str, SEMEL** SEMELS, int* semel_count)
{
	char array_name[31];                        /* Buffer for matrix name (max 30 chars + null) */
	char r1[31];                                /* Buffer for first register name */
	char r2[31];                                /* Buffer for second register name */
	int *result = NULL;                         /* Array to return: [symbol_index, reg1_num, reg2_num] */
	int i;                                      /* Symbol index in symbol table */

	result = (int*)malloc(3 * sizeof(int));/* Allocate memory for result array */
	if (result == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed\n");
		return NULL;
	}
	/* Parse matrix syntax: name[reg1][reg2] with flexible whitespace */
	sscanf(str, " %30[^[] [ %30[^] \t] ] [ %30[^] \t] ]", array_name, r1, r2);
	{	/* Validate that matrix name exists in symbol table */
		i = valid_SEMEL(array_name, SEMELS, semel_count);
		if (i != -1)
		{
			if (reg(r1) == 1 && reg(r2) == 1)/* Validate that both indices are registers */
			{
				result[0] = i;                  /* Symbol table index */
				result[1] = r1[1] - '0';        /* First register number (0-7) */
				result[2] = r2[1] - '0';        /* Second register number (0-7) */
				return result;                  /* Return valid matrix reference */
			}
		}
	}
	/* Invalid matrix syntax - clean up and return null */
	free(result);
	return NULL;
}

int valid_SEMEL(char* str, SEMEL** SEMELS, int* semel_count)
{
	int i = 0;
	int len;
	char* end;

	/* Trim leading spaces */
	while (isspace(*str))
		str++;
	/* Trim trailing spaces */
	len = strlen(str);
	end = str + len - 1;
	while (end > str && isspace(*end)) 
	{
		*end = '\0';
		end--;
	}
	/* Search through symbol table for matching name */
	while (i < *semel_count)
	{
		if (strcmp(SEMELS[i]->name, str) == 0)
		{
			if (SEMELS[i]->ex_en == 0)
			{
				error = 1; /* Entry labels can't be operands */
				return -1;
			}
			return i;
		}
		i++;
	}
	error = 1;
	return -1;
}

void add_to_extern_label(extern_label ** extern_labels,int* COUNT_OF_EXTERN_LABEL,char* str)
{
	if (*COUNT_OF_EXTERN_LABEL == 0)/* Check if this is the first external label */
	{	/* Initial allocation for first external label */
		*extern_labels = (extern_label*)malloc(sizeof(extern_label));
		if (*extern_labels == NULL)
    		{
			fprintf(stderr, "error, memory allocation failed\n");
			error = 1;              /* Set global error flag */
			return;
		}
	}
	else
	{	/* Expand existing array for additional external label */
		extern_label* temp = (extern_label*)realloc(*extern_labels, (*COUNT_OF_EXTERN_LABEL + 1) * sizeof(extern_label));
		if (temp == NULL)
		{
			fprintf(stderr, "error, memory allocation failed\n");
			error= 1;               /* Set global error flag */
			return;
		}
		*extern_labels = temp;          /* Update pointer to expanded array */
	}
	/* Set address where external label is used */
	(*extern_labels)[*COUNT_OF_EXTERN_LABEL].addres = IC;
	/* Allocate and copy external label name */
	(*extern_labels)[*COUNT_OF_EXTERN_LABEL].name = (char*)malloc(strlen(str) + 1);
	if ((*extern_labels)[*COUNT_OF_EXTERN_LABEL].name == NULL)
	{
		fprintf(stderr, "error, memory allocation failed\n");
		error = 1;                      /* Set global error flag */
		return;
	}
	strcpy((*extern_labels)[*COUNT_OF_EXTERN_LABEL].name, str);  /* Copy label name */
	(*COUNT_OF_EXTERN_LABEL)++;                 /* Increment external label counter */
}

int IC_command_analysis(char row[], SEMEL** SEMELS, int* semel_count, command cmd[], binary_code** array, extern_label ** extern_labels, int* COUNT_OF_EXTERN_LABEL)
{
	char* command;                              /* Pointer to command name token */
	char* op1 = NULL;                           /* Pointer to first operand token */
	char* op2 = NULL;                           /* Pointer to second operand token */
	char* op3;                                  /* Pointer to check for extra operands */
	int opcode = -1;                            /* Command opcode number */
	int i = 0;                                  /* Loop counter for command lookup */
	int op1_val = 0;                            /* Numeric value of first operand */
	int op2_val = 0;                            /* Numeric value of second operand */
	int op1_type = -1;                          /* Addressing mode of first operand */
	int* op1_mat = NULL;                        /* Matrix info for first operand [index, reg1, reg2] */
	int* op2_mat = NULL;                        /* Matrix info for second operand [index, reg1, reg2] */
	int op2_type = -1;                          /* Addressing mode of second operand */
	int flag1 = -1;                             /* Processing flag for first operand */
	int flag2 = -1;                             /* Processing flag for second operand */
	int semel_index = 0;                        /* Symbol table index for first operand */
	int op2_semel_index = -1;                   /* Symbol table index for second operand */
	int num_operands = 0;                       /* Count of operands found */
	int required_operands = 0;                  /* Number of operands required by command */
	int found_operand = 0;                      /* Flag indicating if operand was successfully parsed */
	
	if(!(check_commas(row,0)))
		return 0;
	command = strtok(row, " \t\n\r");/* Extract command name from row */
	if (command == NULL) 
	{
		fprintf(stderr, "error in line:%d Empty command\n", sum_of_row);
		error = 1;                          /* Set global error flag */
		return 0;
	}
	while (i < 16) /* Look up command in command table to get opcode and parameter count */
	{ 
		if (strcmp(cmd[i].name, command) == 0)  /* cmd array has 16 instruction commands */
		{
			opcode = cmd[i].op;             /* Get command opcode */
			required_operands = cmd[i].param; /* Get number of required operands */
			break;
		}
		i++;
	}
	if (opcode == -1) /* Validate that command was found */
	{
		fprintf(stderr, "error in line:%d Unknown command @%s\n", sum_of_row, command);
		error = 1;                          /* Set global error flag */
		return 0;
	}
	if (required_operands >= 1) /* Extract first operand if required */
	{
		if (required_operands == 2)
			op1 = strtok(NULL, ",");    /* Use comma delimiter for 2-operand commands */
		else
			op1 = strtok(NULL, " \t\n"); /* Use whitespace delimiter for 1-operand commands */
		if (op1 == NULL) 
		{
			fprintf(stderr, "error in line:%d Missing operand in command %s\n", sum_of_row, command);
			error = 1;                  /* Set global error flag */
			return 0;
		}
		while (*op1 == ' ' || *op1 == '\t') /* Remove leading whitespace from operand */
			op1++;
		num_operands++;                     /* Increment operand counter */
	}
	if (required_operands >= 2) /* Extract second operand if required */
	{
		op2 = strtok(NULL, " ");            /* Get second operand */
		if (op2 == NULL) 
		{
			fprintf(stderr, "error in line:%d Missing second operand in command %s\n", sum_of_row, command);
			error = 1;                  /* Set global error flag */
			return 0;
		}
		while (*op2 == ' ' || *op2 == '\t')/* Remove leading whitespace from operand */ 
			op2++;
		num_operands++;                     /* Increment operand counter */
	}
	if (required_operands > 0) /* Check for extra operands */
	{
		op3 = strtok(NULL, " \t\n");        /* Check for additional tokens */
		if (op3 != NULL && strlen(op3) > 0) 
		{
			fprintf(stderr, "error in line:%d too many operands in command %s\n", sum_of_row, command);
			error = 1;                  /* Set global error flag */
			return 0;
		}
	}
	if (num_operands != required_operands) /* Validate operand count matches requirement */
	{
		if (num_operands < required_operands)
			fprintf(stderr, "error in line:%d Missing operand in command %s\n", sum_of_row, command);
		else
			fprintf(stderr, "error in line:%d too many operands in command %s\n", sum_of_row, command);
		error = 1;                          /* Set global error flag */
		return 0;
	}
	if (op1 != NULL) /* Process first operand */
	{	/* Check if operand is immediate value (starts with #) */
		if (is_valid_number(op1, 0)) 
		{	/* Validate that immediate addressing is allowed for this command */
			if (opcode == 1 || opcode == 12 || opcode == 13) 
			{
				op1_val = atoi(op1);    /* Extract number after # */
				op1_type = 0;               /* Addressing mode 0: immediate */
				flag1 = 0;                  /* Flag for immediate processing */
				found_operand = 1;          /* Mark operand as successfully parsed */
			} 
			else 
			{
				fprintf(stderr, "error in line:%d: Immediate operand not allowed for command %s\n", sum_of_row, command);
				error = 1;                  /* Set global error flag */
				return 0;
			}
		}
		else if (reg(op1))/* Check if operand is register */ 
		{
			op1_val = atoi(op1 + 1);        /* Extract register number */
			op1_type = 3;                   /* Addressing mode 3: register direct */
			flag1 = 0;                      /* Flag for register processing */
			found_operand = 1;              /* Mark operand as successfully parsed */
		} 
		else 
		{	/* Check if operand is matrix reference */
			op1_mat = valid_matrix(op1, SEMELS, semel_count);
			if (op1_mat != NULL) 
			{
				op1_type = 2;               /* Addressing mode 2: matrix */
				flag1 = 2;                  /* Flag for matrix processing */
				found_operand = 1;          /* Mark operand as successfully parsed */
			} 
			else 
			{	/* Check if operand is symbol/label */
				semel_index = valid_SEMEL(op1, SEMELS, semel_count);
				if (semel_index != -1) 
				{
					op1_val = SEMELS[semel_index]->addres; /* Get symbol address */
					op1_type = 1;           /* Addressing mode 1: direct */
					flag1 = 1;              /* Flag for symbol processing */
					found_operand = 1;      /* Mark operand as successfully parsed */
				} 
				else 
				{
					fprintf(stderr, "error in line:%d Invalid operand: %s\n", sum_of_row, op1);
					error = 1;              /* Set global error flag */
					return 0;
				}
			}
		}
		if (found_operand == 0) /* Check if operand was successfully parsed */
		{
			error = 1;                      /* Set global error flag */
			fprintf(stderr, "error in line:%d unvalid operand", sum_of_row);
		}
		found_operand = 0;                  /* Reset flag for next operand */
	}
	if (op2 != NULL) /* Process second operand */
	{	/* Check if operand is immediate value (starts with #) */
		if (is_valid_number(op2, 0)) 
		{	/* Validate that immediate addressing is allowed as destination */
			if (opcode == 1 || opcode == 13) 
			{
				op2_val = atoi(op2);    /* Extract number after # */
				op2_type = 0;               /* Addressing mode 0: immediate */
				/* Set flag based on first operand type for register optimization */
				flag2 = (op1_type == 0) ? 0 : 1;
				found_operand = 1;          /* Mark operand as successfully parsed */
			} 
			else 
			{
				fprintf(stderr, "error in line:%d Immediate operand not allowed as destination for command %s\n", sum_of_row, command);
				error = 1;                  /* Set global error flag */
				return 0;
			}
		}
		else if (reg(op2))/* Check if operand is register */ 
		{
			op2_val = atoi(op2 + 1);        /* Extract register number */
			op2_type = 3;                   /* Addressing mode 3: register direct */
			/* Set flag based on first operand type for register optimization */
			flag2 = (op1_type == 3) ? 0 : 1;
			found_operand = 1;              /* Mark operand as successfully parsed */
		} 
		else 
		{	/* Check if operand is symbol/label */
			op2_semel_index = valid_SEMEL(op2, SEMELS, semel_count);
			if (op2_semel_index != -1) 
			{
				op2_val = SEMELS[op2_semel_index]->addres; /* Get symbol address */
				op2_type = 1;               /* Addressing mode 1: direct */
				flag2 = 1;                  /* Flag for symbol processing */
				found_operand = 1;          /* Mark operand as successfully parsed */
			} 
			else 
			{	/* Check if operand is matrix reference */
				op2_mat = valid_matrix(op2, SEMELS, semel_count);
				if (op2_mat != NULL) 
				{
					op2_type = 2;           /* Addressing mode 2: matrix */
					flag2 = 2;              /* Flag for matrix processing */
					found_operand = 1;      /* Mark operand as successfully parsed */
				} 
				else 
				{
					fprintf(stderr, "error in line:%d Invalid operand: %s\n", sum_of_row, op2);
					error = 1;              /* Set global error flag */
					return 0;
				}
			}
		}
		if (found_operand == 0) /* Check if operand was successfully parsed */
		{
			error = 1;                      /* Set global error flag */
			fprintf(stderr, "error in line:%d unvalid operand", sum_of_row);
		}
	}
	/* Generate first instruction word with opcode and addressing modes */
	if (op1_type == -1 && op2_type == -1)
		to_binary(opcode, 0, 0, array);         /* No operands */
	else if (op2_type == -1)
		to_binary(opcode, 0, op1_type, array); /* One operand */
	else
		to_binary(opcode, op1_type, op2_type, array); /* Two operands */
	/* Generate additional words based on operand types */
	if (flag1 == -1 && flag2 == -1)
		return 1;                               /* No additional words needed */
	else if (flag2 == -1) 
	{	/* Only first operand needs additional word */
		if (op1_type == 1) 
		{	/* Symbol operand - check if external */
			if (SEMELS[semel_index]->ex_en == 1)
				add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op1);
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, SEMELS[semel_index]->ex_en);
		} 
		else if (op1_type == 0)/* Immediate operand */
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, 0);
		else if (op1_type ==3)/* Register operand */
			add_two_numbers(0, op1_val, array);
		else if(op1_type == 2)/*operand is matrix */
		{
			if (SEMELS[op1_mat[0]]->ex_en == 1)
				add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op1);
			add_number(SEMELS[op1_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op1_mat[0]]->ex_en);
			add_two_numbers(op1_mat[1], op1_mat[2], array); /* Matrix indices */
			free(op1_mat);                          /* Clean up matrix info */
			op1_mat = NULL;
		}
	} 
	else if (flag1 + flag2 == 0) /* Both operands are registers - can be packed in one word */
		add_two_numbers(op1_val, op2_val, array);
	else if (flag1 == 2) 
	{	/* First operand is matrix */
		if (SEMELS[op1_mat[0]]->ex_en == 1)
			add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op1);
		add_number(SEMELS[op1_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op1_mat[0]]->ex_en);
		add_two_numbers(op1_mat[1], op1_mat[2], array); /* Matrix indices */
		free(op1_mat);                          /* Clean up matrix info */
		op1_mat = NULL;
	} 
	else if (flag1 == 1) 
	{	/* First operand is symbol */
		if (SEMELS[semel_index]->ex_en == 1)
			add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op1);
		add_number(op1_val, (void**)array, TYPE_INSTRUCTION, SEMELS[semel_index]->ex_en);
	} 
	else 
	{	/* First operand is register or immediate */
		if (op1_type == 3)
			add_two_numbers(op1_val, 0, array); /* Register */
		else
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, 0); /* Immediate */
	}
	/* Generate additional words for second operand */
	if (flag2 == 2) 
	{	/* Second operand is matrix */
		if (SEMELS[op2_mat[0]]->ex_en == 1)
			add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op2);
		add_number(SEMELS[op2_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op2_mat[0]]->ex_en);
		add_two_numbers(op2_mat[1], op2_mat[2], array); /* Matrix indices */
		free(op2_mat);                          /* Clean up matrix info */
		op2_mat = NULL;
	} 
	else if (flag2 == 1) 
	{	/* Second operand is symbol or immediate */
		if (op2_type == 1) 
		{	/* Symbol operand */
			if (SEMELS[op2_semel_index]->ex_en == 1)
				add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op2);
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION, SEMELS[op2_semel_index]->ex_en);
		} 
		else 	/* Immediate operand */
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION, 0);
	}
	return 0;
}

void data(char row[],binary_directive** struct_DC) 
{
    	char* op1;                                  /* Pointer to current data value token */
	int value;                                  /* Parsed integer value */
	
    	op1 = strtok(row, " ");                     /* Skip .data command */
    	op1 = strtok(NULL, ",");                    /* Get first data value */
    	
    	/* Process each comma-separated data value */
	while (op1 != NULL) 
	{	/* Remove leading whitespace from current token */
		while (*op1 == ' ' || *op1 == '\t') op1++;
		/* Validate that token is a valid number */
		if (is_valid_number(op1,1)) 
		{
			value=atoi(op1);            /* Convert string to integer */
            		add_number(value, (void**)struct_DC, TYPE_DIRECTIVE,4); /* Add to data array */
        	} 
		else 
		{
            		fprintf(stderr, "in line %d error: Invalid operand: %s\n",sum_of_row, op1);
            		error = 1;              /* Set global error flag */
        	}
		op1 = strtok(NULL, ",");            /* Get next data value */
    	}
}

void string(char row[],binary_directive** struct_DC)
{
	char *start_quote, *end_quote;              /* Pointers to opening and closing quotes */
	int i;                                      /* Loop counter for character processing */

	/* Locate the first double quote in the row */
	start_quote = strchr(row, '"');
	if (start_quote == NULL)
        {
        	fprintf(stderr, "in line %d error,Missing opening quote for string\n",sum_of_row);
        	error = 1;                      /* Set global error flag */
        	return;
    	}	
	/* Locate the closing double quote */
	end_quote = strchr(start_quote + 1, '"');
	if (end_quote == NULL) 
	{
		fprintf(stderr, "in line %d error,Missing closing quote for string\n",sum_of_row);
		error = 1;                          /* Set global error flag */
		return;
	}
	/* Process each character between the quotes */
	if(end_quote != NULL&&start_quote != NULL)
	{
		for (i = 1; start_quote + i < end_quote; i++) 
		{
			char c = start_quote[i];    /* Get current character */
			/* Add character as integer value to data array */
			add_number((int)c,(void**)struct_DC, TYPE_DIRECTIVE,4);
		}
	}
	/* Add null terminator to end the string */
	add_number(0,(void**)struct_DC, TYPE_DIRECTIVE,4);
}

void mat(char row[], binary_directive** struct_DC)
{
	char  *open1, *close1, *open2, *close2, *after_brackets; /* Pointers for bracket parsing */
	int row_val, col_val;                       /* Matrix dimensions */
	char temp[31];                              /* Buffer for extracting dimension values */
	char *token;                                /* Pointer to data tokens */

	/* Fix comma formatting in the input row */
	if(!check_commas(row,1))
		return;
	/* Find first opening bracket for row dimension */
	open1 = strchr(row, '[');
	if (!open1) 
	{
		fprintf(stderr, "in line %d error,Missing first '['\n",sum_of_row);
		error = 1;                          /* Set global error flag */
	}
	/* Find first closing bracket */
	close1 = strchr(open1, ']');
	if (!close1 || close1 < open1) 
	{
		fprintf(stderr, "in line %d error, Missing first ']'\n",sum_of_row);
		error = 1;                          /* Set global error flag */
	}
	/* Find second opening bracket for column dimension */
	open2 = strchr(close1, '[');
	if (!open2) 
	{
		fprintf(stderr, "in line %d error, Missing second '['\n",sum_of_row);
		error = 1;                          /* Set global error flag */
	}
	/* Find second closing bracket */
	close2 = strchr(open2, ']');
	if (!close2 || close2 < open2) 
	{
		fprintf(stderr, "in line %d error, Missing second ']'\n",sum_of_row);
		error = 1;                          /* Set global error flag */
	}
	/* Extract the row dimension value between first brackets */
	strncpy(temp, open1 + 1, close1 - open1 - 1); /* Copy characters between brackets */
	temp[close1 - open1 - 1] = '\0';        /* Null-terminate the string */
	if (!is_valid_number(temp,2))               /* Validate as positive number */
	{
		fprintf(stderr, "in line %d error, Invalid row number\n",sum_of_row);
		error = 1;                          /* Set global error flag */
	}
	else
		row_val = atoi(temp);                       /* Convert to integer */
	/* Extract the column dimension value between second brackets */
	strncpy(temp, open2 + 1, close2 - open2 - 1); /* Copy characters between brackets */
	temp[close2 - open2 - 1] = '\0';        /* Null-terminate the string */
	if (!is_valid_number(temp,2))               /* Validate as positive number */
	{
		fprintf(stderr, "in line %d error, Invalid col number\n",sum_of_row);
		error=1;                            /* Set global error flag */
	}
	else
	col_val = atoi(temp);                       /* Convert to integer */
	/* Point to data section after the second closing bracket */
	after_brackets = close2 + 1;
	/* Check if there are initialization values after the brackets */
	if (*after_brackets == '\0') 
	{	/* No initialization values - fill matrix with zeros */
		int i;
		for (i = 0; i < row_val * col_val; i++) 
			add_number(0,(void**)struct_DC, TYPE_DIRECTIVE,4); /* Add zero for each matrix element */
	} 
	else 
	{	/* Initialization values provided - process each value */
		token = strtok(after_brackets, " ,");   /* Get first data token */
		while (token != NULL) 
		{	/* Validate each data value */
			if (!is_valid_number(token,1)) 
			{
				fprintf(stderr, "in line %d error, Invalid number: %s\n", sum_of_row, token);
				error = 1;              /* Set global error flag */
			} 
			else 
			{	/* Add valid number to data array */
				add_number(atoi(token),(void**)struct_DC, TYPE_DIRECTIVE,4);
				token = strtok(NULL, " ,"); /* Get next data token */
			}
		}
	}
}

void entry_extern(char row[])
{
    	char* command;                              /* Pointer to directive name (.entry or .extern) */
    	char* extra;                                /* Pointer to check for extra parameters */

    	command = strtok(row, " \t\n\r");          /* Get directive name */
    	extra = strtok(NULL, " \t\n\r");           /* Skip label name */
    	extra = strtok(NULL, " \t\n\r");           /* Check for extra parameters */

    	/* Validate that no extra parameters are provided */
    	if (extra != NULL) 
	{
        	fprintf(stderr, "in line %d error, too many parameters in func %s\n",sum_of_row, command);
        	error = 1;                          /* Set global error flag */
    	}
}
