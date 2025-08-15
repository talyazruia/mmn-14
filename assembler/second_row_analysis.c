#include "encoding.h"
#include "second_row_analysis.h"
/*#include "row_analysis.h"*/
#include "validation.h"

void second_row_analysis(FILE *f, command cmd[], SEMEL*** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels, int macro_count, macro** macros)
{
	char row[MAX_LEN_OF_ROW];			/* Input line buffer - stores each line read from file */
	char row_copy[MAX_LEN_OF_ROW];		/* Copy for tokenizing (strtok modifies the source) */
	char *token;						/* First token of the line - could be label, instruction, or directive */
	size_t len;							/* Length of the first token */
	int i = 0;							/* Loop index for command lookup in cmd[] array */
	char *new_row;						/* Pointer to the line content after a label (after ':') */
	char *colon;						/* Pointer to ':' if a label exists on the line */
	int found = 0;						/* Flag: 1 if a known command/directive was found, else 0 */
	/* Second pass: read each line and generate code/data */
	/* This loop processes every line in the assembly file */
	while (fgets(row, sizeof(row), f))
	{
		sum_of_row++;	/* Global counter: track current line number for error messages */
		found = 0;
		/* Skip comment lines (lines starting with ';') */
		if (row[0] == ';')
			continue;
		/* Prepare tokenization on a safe copy (strtok modifies the string) */
		strcpy(row_copy, row);
		token = strtok(row_copy, " \t\n\r");	/* Get first token, splitting on whitespace */
		if (token == NULL)				/* Skip empty/whitespace-only lines */
			continue;
		/* Check if the first token is a label (ends with ':') */
		len = strlen(token);
		if (len > 0 && token[len - 1] == ':')
		{	/* CASE: Line starts with a label */
			/* Move to the token after the label to identify the directive/command */
			token = strtok(NULL, " \t\n\r");	/* Get the instruction/directive after the label */
			if (token != NULL)
			{	/* new_row points to the substring following ':' in the original line */
				/* This is needed because we want to process the rest of the line after the label */
				colon = strchr(row, ':');
				if (colon != NULL)
					new_row = colon + 1;		/* Point to content after the colon */
				else
					new_row = row;				/* Fallback to original line */
				/* Try to match against instruction mnemonics first */
				for (i = 0; i < NUM_OF_IC_COMMAND; i++)
				{
					if (strcmp(token, cmd[i].name) == 0)	/* cmd[] contains 16 instructions */
					{
						/* Found a matching instruction - analyze it and generate code */
						IC_command_analysis(new_row, *(semels), semel_count, cmd, array, extern_labels, count_of_extern_labels,struct_DC, macro_count, macros);
						found = 1;
						break;
					}
				}
				/* If not an instruction, check if it's a directive after a label */
				if (strcmp(token, ".string") == 0)
					string(new_row, struct_DC,macro_count, &macros,*semels,semel_count,array,extern_labels,count_of_extern_labels);	/* Process string directive */
				else if (strcmp(token, ".mat") == 0)
					mat(new_row, struct_DC,macro_count, &macros,*semels,semel_count,array,extern_labels,count_of_extern_labels);/* Process matrix directive */
				else if (strcmp(token, ".data") == 0)
					data(new_row, struct_DC,macro_count, &macros,*semels,semel_count,array,extern_labels,count_of_extern_labels);		/* Process data directive */
				else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)
					entry_extern(new_row);			/* Process entry/extern directives */
				else if (found == 0)
				{	/* Unknown command/directive found */
					error = 1;
					fprintf(stdout, "error in line: %d unkown command\n", sum_of_row);
				}
			}
		}
		else
		{	/* CASE: No label - treat the first token as a command/directive */
			/* Try to match against instruction mnemonics */
			for (i = 0; i < NUM_OF_IC_COMMAND; i++)
			{
				if (strcmp(token, cmd[i].name) == 0)
				{	/* Found matching instruction - process entire line */
					IC_command_analysis(row, *(semels), semel_count, cmd, array, extern_labels, count_of_extern_labels,struct_DC, macro_count, macros);
					found = 1;
					break;
				}
			}
			/* If not an instruction, check if it's a directive */
			if (strcmp(token, ".string") == 0)
				string(row,struct_DC,macro_count,&macros,*semels,semel_count,array,extern_labels,count_of_extern_labels);/* Process string directive */
			else if (strcmp(token, ".mat") == 0)
				mat(row, struct_DC,macro_count, &macros,*semels,semel_count,array,extern_labels,count_of_extern_labels);/* Process matrix directive */
			else if (strcmp(token, ".data") == 0)
				data(row, struct_DC,macro_count, &macros,*semels,semel_count,array,extern_labels,count_of_extern_labels);/* Process data directive */
			else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)
				entry_extern(row);					/* Process entry/extern directives */
			else if (found == 0)
			{	/* Unknown command/directive */
				error = 1;
				fprintf(stdout, "error in line: %d unkown command\n", sum_of_row);
			}
		}
	}
}

int is_valid_number(char *str, int mode)
{
	char *src, *dst, *endptr;		/* Pointers for string manipulation and parsing */
	long val;						/* Parsed numeric value for range checking */

	if (str == NULL)
		return 0;
	/* Trim leading whitespace */
	while (isspace((unsigned char)*str))
		str++;
	/* Compact string to start (remove leading spaces in-place) */
	/* This loop removes all spaces from the string */
	src = str;
	dst = str;
	while (*src != '\0')
		*dst++ = *src++;
	*dst = '\0';
	/* Trim trailing whitespace */
	dst = str + strlen(str) - 1;
	while (dst >= str && isspace((unsigned char)*dst))
		*dst-- = '\0';
	/* Mode 0: Immediate mode - require leading '#' with no space after it */
	if (mode == Immediate_mode)
	{
		if (*str != '#')
			return 0;					/* Must start with '#' */
		if (isspace((unsigned char)str[1]))		/* No space allowed after '#' */
			return 0;
		memmove(str, str + 1, strlen(str));		/* Remove '#' from string */
	}
	/* Handle optional sign; mode 2 forbids negative numbers */
	if (*str == '+' || *str == '-')
	{
		if (mode == positive_num && *str == '-')
			return 0;					/* Mode 2: unsigned numbers only */
	}
	/* Validate that all characters are digits (no embedded spaces allowed) */
	src = str;
	while (*src && !isspace((unsigned char)*src))
	{
		if (*src == '+' || *src == '-')
		{
			src++;					/* Allow single sign character */
			continue;
		}
		if (!isdigit((unsigned char)*src))
			return 0;				/* Non-digit character found */
		src++;
	}
	/* Only whitespace is allowed after the number */
	while (*src)
	{
		if (!isspace((unsigned char)*src))
			return 0;				/* Non-whitespace after number */
		src++;
	}
	/* Range check for modes 1 and 2 (ensure number fits in int range) */
	if (mode == valid_num || mode == positive_num)
	{
		val = strtol(str, &endptr, 10);		/* Parse string to long */
		if (str == endptr)
			return 0;						/* No digits were parsed */
		/* Skip trailing whitespace after parsed number */
		while (isspace((unsigned char)*endptr))
			endptr++;
		if (*endptr != '\0')
			return 0;						/* Extra characters after number */
		/* Check if value fits in int range */
		if (val < INT_MIN || val > INT_MAX)
			return 0;
	}
	return 1;
}

int reg(char *str)
{
	char *end;
	int len;

	if (str == NULL)
		return 0;
	/* Trim leading spaces */
	while (isspace(*str))
		str++;
	/* Trim trailing spaces by null-terminating */
	len = strlen(str);
	end = str + len - 1;
	while (end > str && isspace(*end))
	{
		*end = '\0';
		end--;
	}
	/* Valid format: 'r' + digit 0..7 and nothing more */
	if (str[0] != 'r')
		return 0;						/* Must start with 'r' */
	if (str[1] < '0' || str[1] > '7')
		return 0;						/* Must be followed by digit 0-7 */
	if (str[2] != '\0')
		return 0;						/* Must be exactly 2 characters */
	return 1;
}

int* valid_matrix(char *str, SEMEL** semels, int* semel_count, extern_label **extern_labels, int* count_of_extern_labels, binary_code** array, binary_directive **struct_DC,int macro_count, macro*** macros)
{
	char array_name[MAX_LEN_OF_ROW];				/* Matrix symbol name (max 30 chars + null) */
	char r1[MAX_LEN_OF_ROW];						/* First index token (expected to be register) */
	char r2[MAX_LEN_OF_ROW];						/* Second index token (expected to be register) */
	int *result = NULL;					/* Return array: [symbol_index, reg1_num, reg2_num] */
	int i;
	int matched;
	char*p1;
	char* p2;

	/* Allocate result array for 3 integers */
	result = (int*)malloc(3 * sizeof(int));
	if (result == NULL)
	{
		fprintf(stdout, "error: memory allocation failed\n");
		error_allocation(macro_count, macros, semels, semel_count, array, struct_DC, extern_labels, count_of_extern_labels);
	}
	/* Parse matrix syntax: name[reg1][reg2] with optional spaces */
	/* Format: up to 30 chars for name, then [register][register] */
	matched = sscanf(str," %31[^ [][%30[^]]][%30[^]]]",array_name,r1,r2);
	if (matched != 3 )
	{	/* Parsing failed - not valid matrix syntax */
		free(result);
		return NULL;
	}
	/* Verify the matrix name exists in symbol table and both indices are valid registers */
	i = valid_SEMEL(array_name, semels, semel_count);
	if (i != -1)							/* Matrix name found in symbol table */
	{
		if (reg(r1) == 1 && reg(r2) == 1)	/* Both indices are valid registers */
		{
			p1=r1;
			p2=r2;
			while (*p1 == ' ' || *p1 == '\t') 
				p1++;
			while (*p2 == ' ' || *p2 == '\t') 
				p2++;
			result[0] = i;					/* Symbol table index */
			result[1] = atoi(p1 + 1);		/* Convert 'rN' to N for first register */
			result[2] = atoi(p2 + 1);		/* Convert 'rN' to N for second register */
			return result;
		}
	}
	/* Not a valid matrix reference */
	free(result);
	return NULL;
}

int valid_SEMEL(char *str, SEMEL** SEMELS, int* semel_count)
{
	int i = 0;
	int len;
	char *end;

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
	/* Search by name in the symbol table */
	while (i < *semel_count)
	{
		if (strcmp(SEMELS[i]->name, str) == 0)
		{	/* Found the symbol - check if it can be used as operand */
			if (SEMELS[i]->ex_en == 0)
			{	/* Entry labels cannot be used as operands (they're for export only) */
				error = 1;
				fprintf(stdout, "in line %d: entry label %s cant be operand\n", sum_of_row, SEMELS[i]->name);
				return -1;
			}
			return i;						/* Return symbol table index */
		}
		i++;
	}
	return -1;								/* Symbol not found */
}

void add_to_extern_label(extern_label **extern_labels, int* count_of_extern_labels, char *str, SEMEL** semels, int* semel_count , binary_code** array, binary_directive **struct_DC,int macro_count, macro*** macros)
{
	if (*count_of_extern_labels == 0)
	{	/* First external reference - allocate initial array */
		*extern_labels = (extern_label*)malloc(sizeof(extern_label));
		if (*extern_labels == NULL)
		{
			fprintf(stdout, "error, memory allocation failed\n");
			error_allocation(macro_count, macros, semels, semel_count, array, struct_DC, extern_labels, count_of_extern_labels);
		}
	}
	else
	{	/* Grow the array to accommodate one more external reference */
		extern_label *temp = (extern_label*)realloc(*extern_labels, (*count_of_extern_labels + 1) * sizeof(extern_label));
		if (temp == NULL)
		{
			fprintf(stdout, "error, memory allocation failed\n");
			error_allocation(macro_count, macros, semels, semel_count, array, struct_DC, extern_labels, count_of_extern_labels);
		}
		*extern_labels = temp;
	}
	/* Record the current instruction counter (IC) where the external symbol is referenced */
	(*extern_labels)[*count_of_extern_labels].addres = IC;
	/* Allocate memory and copy the label name */
	(*extern_labels)[*count_of_extern_labels].name = (char*)malloc(strlen(str) + 1);
	if ((*extern_labels)[*count_of_extern_labels].name == NULL)
	{
		fprintf(stdout, "error, memory allocation failed\n");
		error_allocation(macro_count, macros, semels, semel_count, array, struct_DC, extern_labels, count_of_extern_labels);
	}
	strcpy((*extern_labels)[*count_of_extern_labels].name, str);
	/* Increment the count of external references */
	(*count_of_extern_labels)++;
}

int IC_command_analysis(char row[], SEMEL** semels, int* semel_count, command cmd[], binary_code** array, extern_label **extern_labels, int* count_of_extern_label,binary_directive **struct_DC,int macro_count, macro** macros)
{
	char *command;						/* Instruction mnemonic (e.g., "mov", "add") */
	char *op1 = NULL;					/* First operand (source, if any) */
	char *op2 = NULL;					/* Second operand (destination, if any) */
	char *op3;							/* Used to detect extra operands (error condition) */
	int opcode = -1;					/* Numeric opcode from command table */
	int i = 0;							/* Loop index for cmd[] lookup */
	int op1_val = 0;					/* Numeric/register value for op1 */
	int op2_val = 0;					/* Numeric/register value for op2 */
	int op1_type = -1;					/* Addressing mode of op1 (0=immediate, 1=direct, 2=matrix, 3=register) */
	int *op1_mat = NULL;				/* Matrix access info for op1: [symbol_index, reg1, reg2] */
	int *op2_mat = NULL;				/* Matrix access info for op2: [symbol_index, reg1, reg2] */
	int op2_type = -1;					/* Addressing mode of op2 */
	int flag1 = -1;						/* Operand type flag: -1=none, 0=immediate/reg, 1=symbol, 2=matrix */
	int flag2 = -1;						/* Operand type flag for op2 */
	int semel_index = 0;				/* Symbol table index for op1 */
	int op2_semel_index = -1;			/* Symbol table index for op2 */
	int num_operands = 0;				/* Count of operands found */
	int required_operands = 0;			/* Required operand count from command definition */
	int found_operand = 0;				/* Helper flag for operand validation */
	char *temp_semel;					/* Temporary pointer for symbol name extraction */
	int op1_mat1;
	int op1_mat2;
	int op2_mat1;
	int op2_mat2;
	int op_mat1_semel;
	int op_mat2_semel;
	int op_mat1_semel_ex_en;
	int op_mat2_semel_ex_en;

	/* Validate comma usage in the instruction line */
	if (!(check_commas(row)))
		return 0;
	/* Extract the instruction mnemonic (first token) */
	command = strtok(row, " \t\n\r");
	if (command == NULL)
	{
		fprintf(stdout, "error in line:%d empty command\n", sum_of_row);
		error = 1;
		return 0;
	}
	/* Look up the instruction in the command table to get opcode and operand count */
	while (i < NUM_OF_IC_COMMAND)
	{
		if (strcmp(cmd[i].name, command) == 0)
		{
			opcode = cmd[i].op;				/* Get numeric opcode */
			required_operands = cmd[i].param;	/* Get required operand count */
			break;
		}
		i++;
	}
	if (opcode == -1)
	{
		fprintf(stdout, "error in line:%d Unknown command %s\n", sum_of_row, command);
		error = 1;
		return 0;
	}
	/* Parse operand #1 if the instruction requires operands */
	if (required_operands >= 1)
	{
		if (required_operands == 2)
			op1 = strtok(NULL, ",");		/* Two-operand instructions: operands separated by comma */
		else
			op1 = strtok(NULL, " \t\n");	/* Single-operand instructions: separated by whitespace */
		if (op1 == NULL)
		{
			fprintf(stdout, "error in line:%d Missing operand in command %s\n", sum_of_row, command);
			error = 1;
			return 0;
		}
		/* Trim leading whitespace from operand */
		while (*op1 == ' ' || *op1 == '\t')
			op1++;
		num_operands++;
	}
	/* Parse operand #2 if the instruction requires two operands */
	if (required_operands >= 2)
	{
		op2 = strtok(NULL, " ");
		if (op2 == NULL)
		{
			fprintf(stdout, "error in line:%d Missing second operand in command %s\n", sum_of_row, command);
			error = 1;
			return 0;
		}
		/* Trim leading whitespace from operand */
		while (*op2 == ' ' || *op2 == '\t')
			op2++;
		num_operands++;
	}
	/* Check for extra operands (error condition) */
	op3 = strtok(NULL, " \t\n");
	if (op3 != NULL && strlen(op3) > 0)
	{
		fprintf(stdout, "error in line:%d too many operands in command %s\n", sum_of_row, command);
		error = 1;
		return 0;
	}
	/* Verify operand count matches command requirements */
	if (num_operands != required_operands)
	{
		if (num_operands < required_operands)
			fprintf(stdout, "error in line:%d Missing operand in command %s\n", sum_of_row, command);
		else
			fprintf(stdout, "error in line:%d too many operands in command %s\n", sum_of_row, command);
		error = 1;
		return 0;
	}
	/* ---- OPERAND #1 ANALYSIS AND CLASSIFICATION ---- */
	if (op1 != NULL)
	{	/* Check if operand is an immediate value (starts with #) */
		if (is_valid_number(op1, Immediate_mode))
		{	/* Immediate values are only allowed for certain instructions */
			if (opcode <= OP_OF_FIRST_HASH_NUM || opcode == OP_OF_FIRST_HASH_NUM1)
			{	/* Convert to integer */
				fprintf(stderr, "%d\n",op1_val);
				if(op2_val>MAX_NUM_IN_8_BIT|| op2_val<MIN_NUM_IN_8_BIT)
				{
					error=1;
					fprintf(stdout,"in line %d error: number to encode not in bit range\n", sum_of_row);
					return 0;
				}
				op1_type = ADDRESSING_MODE_HASH_NUM;	/* Addressing mode: immediate (0) */
				flag1 = 3;							/* Flag: immediate/register type */
				found_operand = 1;
			}
			else
			{
				fprintf(stdout, "error in line:%d: Immediate operand not allowed for command %s\n", sum_of_row, command);
				error = 1;
				return 0;
			}
		}
		/* Check if operand is a register (r0-r7) */
		else if (reg(op1))
		{	/* Registers not allowed as source operand for LEA instruction */
			if (opcode != OP_OF_LEA)
			{
				op1_val = atoi(op1 + 1);			/* Extract register number (rN -> N) */
				op1_type = ADDRESSING_MODE_REG;		/* Addressing mode: register (3) */
				flag1 = 0;							/* Flag: immediate/register type */
				found_operand = 1;
			}
			else
			{
				fprintf(stdout, "error in line:%d: register operand not allowed for command %s\n", sum_of_row, command);
				error = 1;
				return 0;
			}
		}
		else
		{	/* Check if operand is matrix access: name[reg][reg] */
			op1_mat = valid_matrix(op1, semels, semel_count, extern_labels, count_of_extern_label, array, struct_DC, macro_count, &macros);
			if (op1_mat != NULL)
			{
				op1_mat1=op1_mat[1];
				op2_mat1=op1_mat[2];
				op_mat1_semel=semels[op1_mat[0]]->addres;
				op_mat1_semel_ex_en=semels[op1_mat[0]]->ex_en;
				free(op1_mat);
				op1_type = ADDRESSING_MODE_MATRIX;	/* Addressing mode: matrix (2) */
				flag1 = 2;							/* Flag: matrix type */
				found_operand = 1;
			}
			else
			{	/* Must be a symbol (label) reference */
				semel_index = valid_SEMEL(op1, semels, semel_count);
				if (semel_index != -1)
				{
					op1_val = semels[semel_index]->addres;	/* Get symbol address */
					op1_type = ADDRESSING_MODE_SEMEL;		/* Addressing mode: direct/symbol (1) */
					flag1 = 1;								/* Flag: symbol type */
					found_operand = 1;
				}
				else
				{
					fprintf(stdout, "error in line:%d Invalid operand: %s\n", sum_of_row, op1);
					error = 1;
					return 0;
				}
			}
		}
		if (found_operand == 0)
		{
			error = 1;
			fprintf(stdout, "error in line:%d unvalid operand\n", sum_of_row);
		}
		found_operand = 0;
	}
	/* ---- OPERAND #2 ANALYSIS AND CLASSIFICATION ---- */
	if (op2 != NULL)
	{	/* Check if destination operand is immediate (only allowed for CMP instruction) */
		if (is_valid_number(op2, Immediate_mode))
		{
			if (opcode == OP_OF_CMP)
			{
				op2_val = atoi(op2);
				if(op2_val>MAX_NUM_IN_8_BIT|| op2_val<MIN_NUM_IN_8_BIT)
				{
					error=1;
					fprintf(stdout,"in line %d error: number to encode not in bit range\n", sum_of_row);
					return 0;
				}		
				op2_type = ADDRESSING_MODE_HASH_NUM;	/* Addressing mode: immediate (0) */
				flag2 = 3;		/* Optimization: pack if both immediate */
				found_operand = 1;
			}
			else
			{
				fprintf(stdout, "error in line:%d Immediate operand not allowed as destination for command %s\n", sum_of_row, command);
				error = 1;
				return 0;
			}
		}
		/* Check if destination operand is a register */
		else if (reg(op2))
		{
			op2_val = atoi(op2 + 1);				/* Extract register number */
			op2_type = ADDRESSING_MODE_REG;			/* Addressing mode: register (3) */
			flag2 = (op1_type == 3) ? 0 : 1;		/* Optimization: pack if both registers */
			found_operand = 1;
		}
		else
		{	/* Check if destination operand is a symbol */
			op2_semel_index = valid_SEMEL(op2, semels, semel_count);
			if (op2_semel_index != -1)
			{
				op2_val = semels[op2_semel_index]->addres;
				op2_type = ADDRESSING_MODE_SEMEL;	/* Addressing mode: direct/symbol (1) */
				flag2 = 1;							/* Flag: symbol type */
				found_operand = 1;
			}
			else
			{	/* Check if destination operand is matrix access */
				op2_mat = valid_matrix(op2, semels, semel_count, extern_labels, count_of_extern_label, array, struct_DC, macro_count, &macros);
				if (op2_mat != NULL)
				{
					op1_mat2=op2_mat[1];
					op2_mat2=op2_mat[2];
					op_mat2_semel=semels[op2_mat[0]]->addres;
					op_mat2_semel_ex_en=semels[op2_mat[0]]->ex_en;
					free(op2_mat);
					op2_type = ADDRESSING_MODE_SEMEL;	/* Matrix encoded as symbol + indices */
					flag2 = 2;							/* Flag: matrix type */
					found_operand = 1;
				}
				else
				{
					fprintf(stdout, "error in line:%d Invalid operand: %s\n", sum_of_row, op2);
					error = 1;
					return 0;
				}
			}
		}
		if (found_operand == 0)
		{
			error = 1;
			fprintf(stdout, "error in line:%d invalid operand\n", sum_of_row);
		}
	}
	/* ---- MACHINE CODE GENERATION ---- */
	/* Generate the first word: opcode + addressing modes */
	if (op1_type == -1 && op2_type == -1)
		to_binary(opcode, 0, 0, array,macro_count, &macros, semels, semel_count,struct_DC, extern_labels,count_of_extern_label);	/* No operands */
	else if (op2_type == -1)
		to_binary(opcode, 0, op1_type, array,macro_count, &macros, semels, semel_count,struct_DC, extern_labels,count_of_extern_label);	/* One operand */
	else
		to_binary(opcode, op1_type, op2_type, array,macro_count, &macros, semels, semel_count,struct_DC, extern_labels,count_of_extern_label);/* Two operands */
	/* Generate additional words for operands */
	if (flag1 == -1 && flag2 == -1)
		return 1;								/* No additional words needed */
	else if (flag2 == -1)
	{	/* Only operand 1 needs additional words */
		/* Symbol reference - check if it's external and add to extern list */
		if (op1_type == ADDRESSING_MODE_SEMEL)
		{
			if (semels[semel_index]->ex_en == 1)
				add_to_extern_label(extern_labels, count_of_extern_label,op1, semels, semel_count , array, struct_DC, macro_count, &macros);
			add_number(op1_val, TYPE_INSTRUCTION, semels[semel_index]->ex_en,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
		}
		/* Immediate value */
		else if (op1_type == ADDRESSING_MODE_HASH_NUM)
			add_number(op1_val, TYPE_INSTRUCTION, 0,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
		/* Register - pack source and destination register in one word */
		else if (op1_type == ADDRESSING_MODE_REG)
			add_two_numbers(0, op1_val, array ,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
		/* Matrix access - requires symbol address + register indices */
		else if (op1_type == ADDRESSING_MODE_MATRIX)
		{
			/* Check if matrix symbol is external */
			if (op_mat1_semel_ex_en == 1)
			{
				temp_semel = strtok(op1, "[");		/* Extract symbol name before '[' */
				add_to_extern_label(extern_labels, count_of_extern_label,temp_semel, semels, semel_count , array, struct_DC, macro_count, &macros);
			}
			/* Add matrix base address */
			add_number(op_mat1_semel, TYPE_INSTRUCTION, op_mat1_semel_ex_en,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
			/* Add register indices for matrix access */
			add_two_numbers(op1_mat1, op2_mat1, array ,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
			op1_mat = NULL;
		}
	}
	/* Special case: both operands are registers - pack them in one word */
	else if (flag1 + flag2 == 0)	/* Both flags are 0 (register) */
		add_two_numbers(op1_val, op2_val, array,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
	else if (flag1 == 2)
	{	/* Operand 1 is matrix - generate matrix access code */
		if (op_mat1_semel_ex_en  == 1)
		{
			temp_semel = strtok(op1, "[");
			add_to_extern_label(extern_labels, count_of_extern_label,temp_semel, semels, semel_count , array, struct_DC, macro_count, &macros);
		}
		add_number(op_mat1_semel , TYPE_INSTRUCTION, op_mat1_semel_ex_en,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
		add_two_numbers(op1_mat1, op2_mat1, array ,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
		op1_mat = NULL;
	}
	else if (flag1 == 1)
	{	/* Operand 1 is symbol */
		if (semels[semel_index]->ex_en == 1)
			add_to_extern_label(extern_labels, count_of_extern_label,op1, semels, semel_count , array, struct_DC, macro_count, &macros);
		add_number(op1_val, TYPE_INSTRUCTION, semels[semel_index]->ex_en,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
	}
	else
	{	/* Operand 1 is immediate or register */
		if (op1_type == ADDRESSING_MODE_HASH_NUM)
			add_number(op1_val, TYPE_INSTRUCTION, 0,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
		if (op1_type == ADDRESSING_MODE_REG)
			add_two_numbers(op1_val, 0, array ,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
	}
	/* Generate additional words for operand 2 if present */
	if (flag2 == 2)
	{	/* Operand 2 is matrix */
		if (op_mat2_semel_ex_en == 1)
		{
			temp_semel = strtok(op2, "[");
			add_to_extern_label(extern_labels, count_of_extern_label,temp_semel, semels, semel_count , array, struct_DC, macro_count, &macros);
		}
		add_number (op_mat2_semel, TYPE_INSTRUCTION, op_mat2_semel_ex_en,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
		add_two_numbers(op1_mat2, op2_mat2, array ,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
		op2_mat = NULL;
	}
	else if (flag2 == 1)
	{	/* Operand 2 is symbol, immediate, or register */
		if (op2_type == ADDRESSING_MODE_SEMEL)/*symbol*/
		{
			if (semels[op2_semel_index]->ex_en == 1)
				add_to_extern_label(extern_labels, count_of_extern_label,op2, semels, semel_count , array, struct_DC, macro_count, &macros);
			add_number(op2_val, TYPE_INSTRUCTION, semels[op2_semel_index]->ex_en,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
		}
		else    /*register*/
			add_two_numbers(0, op2_val, array ,macro_count, &macros, semels, semel_count, struct_DC, extern_labels, count_of_extern_label);
	}
	else if (op2_type == ADDRESSING_MODE_HASH_NUM)/*immediate*/
			add_number(op2_val, TYPE_INSTRUCTION, 0,macro_count, &macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_label);
	return 0;
}

void data(char row[], binary_directive** struct_DC,int macro_count,macro*** macros,SEMEL** semels,int* semel_count,binary_code **array, extern_label**extern_labels, int *count_of_extern_labels)
{
	char *op1;							/* Current token after .data */
	int value;							/* Parsed integer value */

	/* Validate comma usage in the directive */
	if (!(check_commas(row)))
		return;
	/* Skip the directive token itself (.data) */
	op1 = strtok(row, " ");
	op1 = strtok(NULL, ",");			/* Get first value (comma-separated) */
	/* Parse comma-separated integers and add them to data section */
	if(op1!=NULL)
	{
		while (*op1 == ' ' || *op1 == '\t')  
        		op1++;
	}
	if (op1 == NULL || *op1 == '\0') 
	{
    		error = 1;
    		fprintf(stdout, "in line %d error: .data must contain at least one number\n", sum_of_row);
    		return;
	}
	while (op1 != NULL)
	{	/* Trim leading whitespace from the current token */
		while (*op1 == ' ' || *op1 == '\t')
			op1++;
		/* Validate that the token is a valid integer */
		if (is_valid_number(op1, valid_num))
		{
			value = atoi(op1);				/* Convert string to integer */
			/* Add the integer to data section with type 4 (regular data) */
			add_number(value, TYPE_DIRECTIVE, 4,macro_count, macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_labels);
		}
		else
		{
			fprintf(stdout, "in line %d error: Invalid operand: %s\n", sum_of_row, op1);
			error = 1;
			return;
		}
		op1 = strtok(NULL, ",");		/* Get next comma-separated value */
	}
}

void string(char row[], binary_directive** struct_DC,int macro_count,macro*** macros,SEMEL** semels,int* semel_count,binary_code **array, extern_label**extern_labels, int *count_of_extern_labels)
{
	char *start_quote=NULL;					/* Pointer to opening quote character */
	char *end_quote=NULL;					/* Pointer to closing quote character */
	int i;
	
	/* Validate string syntax */
	if (!(string_commas_check(row)))
		return;
	/* Find the first '"' character in the line */
	start_quote = strchr(row, '"');
	/* Find the matching closing '"' character */
	end_quote = strrchr(start_quote + 1, '"');
    	if (!end_quote || end_quote <= start_quote)
        	return;
	/* Iterate through characters between quotes (excluding the quotes themselves) */
	for (i = 1; start_quote + i < end_quote; i++)
	{
		unsigned char c = start_quote[i];		/* Get current character */
		/* Add character as integer value to data section */
		add_number((int)c, TYPE_DIRECTIVE, 4,macro_count, macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_labels);
	}
	/* Add null terminator '\0' to end the string */
	add_number(0, TYPE_DIRECTIVE, 4,macro_count, macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_labels);
}

int string_commas_check(char *row)
{
	char *colon;
	char *open;
	char *close;
	/* Move to the character after the 'g' of ".string" */
	/* This finds the end of the directive name */
	colon = strstr(row, ".string");
	colon += strlen(".string");
	/* Skip whitespace until newline or non-space character */
	while ((isspace(*colon)) && (*colon != '\n') && (*colon != '\0'))
		colon++;
	/* The next non-space character must be the opening quote */
	if (*colon != '"')
	{	
		error = 1;
		fprintf(stdout, "in line:%d unexpected char\n", sum_of_row);
		return 0;
	}	
	open = colon;		
	close = strrchr(open + 1, '"');
	if (!close) 
	{
        	error = 1;
        	fprintf(stdout, "in line:%d missing last quotes\n", sum_of_row);
        	return 0;
    	}
	colon = close + 1;
    	while (isspace((unsigned char)*colon) && *colon != '\n' && *colon != '\0')
        	colon++;
    	if (*colon != '\n' && *colon != '\0') 
	{
        	error = 1;
        	fprintf(stdout, "in line:%d unexpected char\n", sum_of_row);
        	return 0;
    	}
    	return 1;
}

void mat(char row[], binary_directive** struct_DC,int macro_count,macro*** macros,SEMEL** semels,int* semel_count,binary_code **array, extern_label**extern_labels, int *count_of_extern_labels)
{
	char *open1, *close1, *open2, *close2, *after_brackets;	/* Bracket position pointers */
	int row_val, col_val;										/* Matrix dimensions (rows, columns) */
	char temp[MAX_LEN_OF_ROW];											/* Temporary buffer for extracting bracket content */
	char *token;												/* Token for parsing initialization values */
	int i;
	
	/* Validate comma usage in the directive */
	if (!check_commas(row))
		return;
	/* Parse matrix dimensions from brackets: .mat [rows][cols] */
	/* Find first pair of brackets for row dimension */
	open1 = strchr(row, '[');
	if (!open1)
	{
		fprintf(stderr, "in line %d error,Missing first '['\n", sum_of_row);
		error = 1;
		return;
	}
	close1 = strchr(open1, ']');
	if (!close1 || close1 < open1)
	{
		fprintf(stderr, "in line %d error, Missing first ']'\n", sum_of_row);
		error = 1;
		return;
	}
	/* Find second pair of brackets for column dimension */
	open2 = strchr(close1, '[');
	if (!open2)
	{
		fprintf(stderr, "in line %d error, Missing second '['\n", sum_of_row);
		error = 1;
		return;
	}
	close2 = strchr(open2, ']');
	if (!close2 || close2 < open2)
	{
		fprintf(stderr, "in line %d error, Missing second ']'\n", sum_of_row);
		error = 1;
		return;
	}
	/* Extract and validate row dimension */
	strncpy(temp, open1 + 1, close1 - open1 - 1);		/* Copy content between first [ ] */
	temp[close1 - open1 - 1] = '\0';					/* Null terminate */
	if (!is_valid_number(temp, positive_num))						/* Mode 2: unsigned integers only */
	{
		fprintf(stderr, "in line %d error, Invalid row number\n", sum_of_row);
		error = 1;
		return;
	}
	else
		row_val = atoi(temp);
	if(row_val==0)
	{
		fprintf(stderr, "in line %d error, Invalid row number\n", sum_of_row);
		error=1;
		return;
	}
	/* Extract and validate column dimension */
	strncpy(temp, open2 + 1, close2 - open2 - 1);		/* Copy content between second [ ] */
	temp[close2 - open2 - 1] = '\0';					/* Null terminate */
	if (!is_valid_number(temp, positive_num))						/* Mode 2: unsigned integers only */
	{
		fprintf(stderr, "in line %d error, Invalid col number\n", sum_of_row);
		error = 1;
		return;
	}
	else
		col_val = atoi(temp);
	if(col_val==0)
	{
		fprintf(stderr, "in line %d error, Invalid row number\n", sum_of_row);
		error=1;
		return;
	}
	/* Process initialization values (if any) after the brackets */
	after_brackets = close2 + 1;
	while (*after_brackets == ' ' || *after_brackets == '\t')
		after_brackets++;
	/* Check if initialization values are provided */
	if (*after_brackets == '\0')
	{	/* No initialization list provided - fill matrix with zeros */
		for (i = 0; i < row_val * col_val; i++)
			add_number(0, TYPE_DIRECTIVE, 4,macro_count, macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_labels);
	}
	else
	{	/* Parse comma-separated initialization values */
		i=0;
		token = strtok(after_brackets, ",");
		while (token != NULL)
		{	/* Validate each initialization value */
			if (!is_valid_number(token, valid_num))				/* Mode 1: signed integers */
			{
				fprintf(stderr, "in line %d error, Invalid number: %s\n", sum_of_row, token);
				error = 1;
				return;
			}
			else 
			{
				if(i>row_val * col_val) /*There are more numbers than the matrix can hold.*/
				{
					error=1;
					fprintf(stderr, "in line %d error, There are more values than fit in the matrix.\n", sum_of_row);
					return;
				}
				add_number(atoi(token), TYPE_DIRECTIVE, 4,macro_count, macros, semels, semel_count, array,struct_DC, extern_labels,count_of_extern_labels);
				i++;  /*Count the number of values*/
			}
		token = strtok(NULL, ",");					/* Get next value */
		}
		while(i<row_val * col_val)
		{
			add_number(0, TYPE_DIRECTIVE, 4,macro_count, macros, semels, semel_count,array, struct_DC, extern_labels, count_of_extern_labels);
			i++;
		}
	}
}

void entry_extern(char row[])
{
	char *command;						/* Directive name: .entry or .extern */
	char *extra;						/* Used to detect extra parameters */

	command = strtok(row, " \t\n\r");	/* Extract directive name (.entry/.extern) */
	extra = strtok(NULL, " \t\n\r");	/* Skip the label name parameter */
	extra = strtok(NULL, " \t\n\r");	/* Check for extra parameters (should be NULL) */

	/* Validate comma usage */
	if (!(check_commas(row)))
		return;
	/* These directives should have exactly one parameter (the label name) */
	if (extra != NULL)
	{
		fprintf(stderr, "in line %d error, too many parameters in func %s\n", sum_of_row, command);
		error = 1;
	}
}
