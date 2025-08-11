#include "encoding.h"
#include "second_row_analysis.h"
#include "row_analysis.h"
#include "validation.h"

void second_row_analysis(FILE *f, command cmd[], SEMEL*** SEMELS, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label** extern_labels, int *count_of_extern_labels)
{
	char row[MAX_LEN_OF_ROW];			/* Input line buffer */
	char row_copy[MAX_LEN_OF_ROW];		/* Copy for tokenizing (strtok modifies the source) */
	char *token;						/* First token of the line */
	size_t len;							/* Length of the first token */
	int i = 0;							/* Loop index for command lookup */
	char *new_row;						/* Pointer to the line content after a label (after ':') */
	char *colon;						/* Pointer to ':' if a label exists */
	int found = 0;						/* 1 if a known command/directive was found, else 0 */

	/* Second pass: read each line and generate code/data */
	while (fgets(row, sizeof(row), f))
	{
		sum_of_row++;					/* Count lines for error messages */
		found = 0;

		/* Validate line length (must contain '\n') */
		if (strchr(row, '\n') == NULL)
		{
			error = 1;
			clear_row_arry();			/* Flush the rest of the long line */
			continue;
		}

		/* Skip comment lines */
		if (row[0] == ';')
			continue;

		/* Prepare tokenization on a safe copy */
		strcpy(row_copy, row);
		token = strtok(row_copy, " \t\n\r");
		if (token == NULL)				/* Skip empty/whitespace-only lines */
			continue;

		/* If the first token is a label (ends with ':') */
		len = strlen(token);
		if (len > 0 && token[len - 1] == ':')
		{
			/* Move to the token after the label to identify the directive/command */
			token = strtok(NULL, " \t\n\r");
			if (token != NULL)
			{
				/* new_row points to the substring following ':' in the original line */
				colon = strchr(row, ':');
				if (colon != NULL)
					new_row = colon + 1;
				else
					new_row = row;

				/* Try instruction mnemonics first */
				for (i = 0; i < NUM_OF_IC_COMMAND; i++)
				{
					if (strcmp(token, cmd[i].name) == 0)	/* cmd[] contains 16 instructions */
					{
						IC_command_analysis(new_row, *(SEMELS), semel_count, cmd, array, extern_labels, count_of_extern_labels);
						found = 1;
						break;
					}
				}

				/* Directives after a label */
				if (strcmp(token, ".string") == 0)
				{
					string(new_row, struct_DC);
				}
				else if (strcmp(token, ".mat") == 0)
				{
					mat(new_row, struct_DC);
				}
				else if (strcmp(token, ".data") == 0)
				{
					data(new_row, struct_DC);
				}
				else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)
				{
					entry_extern(new_row);
				}
				else if (found == 0)
				{
					error = 1;
					fprintf(stderr, "eror in line:%d unkown command", sum_of_row);
				}
			}
		}
		else
		{
			/* No label: treat the first token as a command/directive */
			for (i = 0; i < NUM_OF_IC_COMMAND; i++)
			{
				if (strcmp(token, cmd[i].name) == 0)
				{
					IC_command_analysis(row, *(SEMELS), semel_count, cmd, array, extern_labels, count_of_extern_labels);
					found = 1;
					break;
				}
			}

			if (strcmp(token, ".string") == 0)
			{
				string(row, struct_DC);
			}
			else if (strcmp(token, ".mat") == 0)
			{
				mat(row, struct_DC);
			}
			else if (strcmp(token, ".data") == 0)
			{
				data(row, struct_DC);
			}
			else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)
			{
				entry_extern(row);
			}
			else if (found == 0)
			{
				error = 1;
				fprintf(stderr, "error in line:%d unkown command\n", sum_of_row);
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

	/* Trim leading space */
	while (isspace((unsigned char)*str))
		str++;

	/* Compact string to start (remove leading spaces in-place) */
	src = str;
	dst = str;
	while (*src != '\0')
		*dst++ = *src++;
	*dst = '\0';

	/* Trim trailing space */
	dst = str + strlen(str) - 1;
	while (dst >= str && isspace((unsigned char)*dst))
		*dst-- = '\0';

	/* Mode 0: require leading '#' with no space after it; strip it */
	if (mode == 0)
	{
		if (*str != '#')
			return 0;
		if (isspace((unsigned char)str[1]))		/* No space allowed after '#' */
			return 0;
		memmove(str, str + 1, strlen(str));		/* Remove '#' */
	}

	/* Optional sign; mode 2 forbids negative numbers */
	if (*str == '+' || *str == '-')
	{
		if (mode == 2 && *str == '-')
			return 0;
	}

	/* Validate digits (no embedded spaces) */
	src = str;
	while (*src && !isspace((unsigned char)*src))
	{
		if (*src == '+' || *src == '-')
		{
			src++;								/* Allow single sign */
			continue;
		}
		if (!isdigit((unsigned char)*src))
			return 0;
		src++;
	}

	/* Only whitespace is allowed after the number */
	while (*src)
	{
		if (!isspace((unsigned char)*src))
			return 0;
		src++;
	}

	/* Range check (for modes 1 and 2) */
	if (mode == 1 || mode == 2)
	{
		val = strtol(str, &endptr, 10);
		if (str == endptr)
			return 0;

		/* Skip trailing space after parsed number */
		while (isspace((unsigned char)*endptr))
			endptr++;

		if (*endptr != '\0')
			return 0;

		if (val < INT_MIN || val > INT_MAX)
			return 0;
	}
	return 1;
}

/* Return 1 if str is a valid register token: r0..r7 (ignores surrounding spaces) */
int reg(char *str)
{
	char *end;
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

	/* Format: 'r' + digit 0..7 and nothing more */
	if (str[0] != 'r')
		return 0;
	if (str[1] < '0' || str[1] > '7')
		return 0;
	if (str[2] != '\0')
		return 0;

	return 1;
}

int* valid_matrix(char *str, SEMEL** SEMELS, int* semel_count)
{
	char array_name[31];				/* Matrix symbol name */
	char r1[31];						/* First index token (expected register) */
	char r2[31];						/* Second index token (expected register) */
	int *result = NULL;					/* [symbol_index, reg1_num, reg2_num] */
	int i;
	int matched;

	result = (int*)malloc(3 * sizeof(int));	/* Allocate result array */
	if (result == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed\n");
		return NULL;
	}

	/* Parse: name[reg1][reg2] with optional spaces */
	matched = sscanf(str, " %30[^[] [ %30[^] \t] ] [ %30[^] \t] ]", array_name, r1, r2);
	if (matched != 3)
	{
		free(result);
		return NULL;
	}

	/* Verify the matrix name exists and both indices are registers */
	i = valid_SEMEL(array_name, SEMELS, semel_count);
	if (i != -1)
	{
		if (reg(r1) == 1 && reg(r2) == 1)
		{
			result[0] = i;				/* Symbol index in table */
			result[1] = r1[1] - '0';	/* rN -> N */
			result[2] = r2[1] - '0';	/* rN -> N */
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
		{
			if (SEMELS[i]->ex_en == 0)
			{
				/* Entry labels cannot be operands */
				error = 1;
				fprintf(stderr, "in line %d: entry label%s cant be operand", sum_of_row, SEMELS[i]->name);
				return -1;
			}
			return i;
		}
		i++;
	}
	return -1;
}

void add_to_extern_label(extern_label **extern_labels, int* COUNT_OF_EXTERN_LABEL, char *str)
{
	if (*COUNT_OF_EXTERN_LABEL == 0)
	{
		/* First item: allocate the array */
		*extern_labels = (extern_label*)malloc(sizeof(extern_label));
		if (*extern_labels == NULL)
		{
			fprintf(stderr, "error, memory allocation failed\n");
			error = 1;
			return;
		}
	}
	else
	{
		/* Grow the array by one */
		extern_label *temp = (extern_label*)realloc(*extern_labels, (*COUNT_OF_EXTERN_LABEL + 1) * sizeof(extern_label));
		if (temp == NULL)
		{
			fprintf(stderr, "error, memory allocation failed\n");
			error = 1;
			return;
		}
		*extern_labels = temp;
	}

	/* Record IC where the extern is referenced */
	(*extern_labels)[*COUNT_OF_EXTERN_LABEL].addres = IC;

	/* Copy the label name */
	(*extern_labels)[*COUNT_OF_EXTERN_LABEL].name = (char*)malloc(strlen(str) + 1);
	if ((*extern_labels)[*COUNT_OF_EXTERN_LABEL].name == NULL)
	{
		fprintf(stderr, "error, memory allocation failed\n");
		error = 1;
		return;
	}
	strcpy((*extern_labels)[*COUNT_OF_EXTERN_LABEL].name, str);

	/* One more extern reference recorded */
	(*COUNT_OF_EXTERN_LABEL)++;
}

int IC_command_analysis(char row[], SEMEL** SEMELS, int* semel_count, command cmd[], binary_code** array, extern_label **extern_labels, int* COUNT_OF_EXTERN_LABEL)
{
	char *command;						/* Instruction mnemonic */
	char *op1 = NULL;					/* First operand (if any) */
	char *op2 = NULL;					/* Second operand (if any) */
	char *op3;							/* Extra token check */
	int opcode = -1;					/* Opcode from command table */
	int i = 0;							/* Loop index for cmd[] lookup */
	int op1_val = 0;					/* Numeric/immediate/reg value for op1 */
	int op2_val = 0;					/* Numeric/immediate/reg value for op2 */
	int op1_type = -1;					/* Addressing mode of op1 */
	int *op1_mat = NULL;				/* Matrix tuple for op1: [index, r1, r2] */
	int *op2_mat = NULL;				/* Matrix tuple for op2: [index, r1, r2] */
	int op2_type = -1;					/* Addressing mode of op2 */
	int flag1 = -1;						/* -1: none, 0: immediate/reg, 1: symbol, 2: matrix */
	int flag2 = -1;						/* -1: none, 0: immediate/reg, 1: symbol, 2: matrix */
	int semel_index = 0;				/* Symbol index for op1 */
	int op2_semel_index = -1;			/* Symbol index for op2 */
	int num_operands = 0;				/* Count of parsed operands */
	int required_operands = 0;			/* Required count from cmd table */
	int found_operand = 0;				/* Helper flag per operand */
	char *temp_semel;

	if (!(check_commas(row)))
		return 0;

	/* Instruction mnemonic */
	command = strtok(row, " \t\n\r");
	if (command == NULL)
	{
		fprintf(stderr, "error in line:%d Empty command\n", sum_of_row);
		error = 1;
		return 0;
	}

	/* Lookup opcode and operand count */
	while (i < NUM_OF_IC_COMMAND)
	{
		if (strcmp(cmd[i].name, command) == 0)
		{
			opcode = cmd[i].op;
			required_operands = cmd[i].param;
			break;
		}
		i++;
	}

	if (opcode == -1)
	{
		fprintf(stderr, "error in line:%d Unknown command %s\n", sum_of_row, command);
		error = 1;
		return 0;
	}

	/* Parse operand #1 if needed */
	if (required_operands >= 1)
	{
		if (required_operands == 2)
			op1 = strtok(NULL, ",");	/* Two-operand mnemonics use comma */
		else
			op1 = strtok(NULL, " \t\n");

		if (op1 == NULL)
		{
			fprintf(stderr, "error in line:%d Missing operand in command %s\n", sum_of_row, command);
			error = 1;
			return 0;
		}

		/* Trim leading spaces */
		while (*op1 == ' ' || *op1 == '\t')
			op1++;

		num_operands++;
	}

	/* Parse operand #2 if needed */
	if (required_operands >= 2)
	{
		op2 = strtok(NULL, " ");
		if (op2 == NULL)
		{
			fprintf(stderr, "error in line:%d Missing second operand in command %s\n", sum_of_row, command);
			error = 1;
			return 0;
		}

		while (*op2 == ' ' || *op2 == '\t')
			op2++;

		num_operands++;
	}

	/* Ensure there is no third operand/token */
	op3 = strtok(NULL, " \t\n");
	if (op3 != NULL && strlen(op3) > 0)
	{
		fprintf(stderr, "error in line:%d too many operands in command %s\n", sum_of_row, command);
		error = 1;
		return 0;
	}

	/* Operand count must match the command definition */
	if (num_operands != required_operands)
	{
		if (num_operands < required_operands)
			fprintf(stderr, "error in line:%d Missing operand in command %s\n", sum_of_row, command);
		else
			fprintf(stderr, "error in line:%d too many operands in command %s\n", sum_of_row, command);

		error = 1;
		return 0;
	}

	/* ---- Operand #1 decoding ---- */
	if (op1 != NULL)
	{
		/* Immediate */
		if (is_valid_number(op1, 0))
		{
			/* Immediate allowed for opcodes <= OP_OF_FIRST_HASH_NUM or OP_OF_FIRST_HASH_NUM1 */
			if (opcode <= OP_OF_FIRST_HASH_NUM || opcode == OP_OF_FIRST_HASH_NUM1)
			{
				op1_val = atoi(op1);
				op1_type = ADDRESSING_MODE_HASH_NUM;	/* 0 */
				flag1 = 0;
				found_operand = 1;
			}
			else
			{
				fprintf(stderr, "error in line:%d: Immediate operand not allowed for command %s\n", sum_of_row, command);
				error = 1;
				return 0;
			}
		}
		/* Register */
		else if (reg(op1))
		{
			if (opcode != OP_OF_LEA)
			{
				op1_val = atoi(op1 + 1);				/* rN -> N */
				op1_type = ADDRESSING_MODE_REG;			/* 3 */
				flag1 = 0;
				found_operand = 1;
			}
			else
			{
				fprintf(stderr, "error in line:%d: register operand not allowed for command %s\n", sum_of_row, command);
				error = 1;
				return 0;
			}
		}
		else
		{
			/* Matrix */
			op1_mat = valid_matrix(op1, SEMELS, semel_count);
			if (op1_mat != NULL)
			{
				op1_type = ADDRESSING_MODE_MATRIX;		/* 2 */
				flag1 = 2;
				found_operand = 1;
			}
			else
			{
				/* Symbol */
				semel_index = valid_SEMEL(op1, SEMELS, semel_count);
				if (semel_index != -1)
				{
					op1_val = SEMELS[semel_index]->addres;
					op1_type = ADDRESSING_MODE_SEMEL;	/* 1 */
					flag1 = 1;
					found_operand = 1;
				}
				else
				{
					fprintf(stderr, "error in line:%d Invalid operand: %s\n", sum_of_row, op1);
					error = 1;
					return 0;
				}
			}
		}

		if (found_operand == 0)
		{
			error = 1;
			fprintf(stderr, "error in line:%d unvalid operand", sum_of_row);
		}
		found_operand = 0;
	}

	/* ---- Operand #2 decoding ---- */
	if (op2 != NULL)
	{
		/* Immediate (destination immediate allowed only for cmp) */
		if (is_valid_number(op2, 0))
		{
			if (opcode == OP_OF_CMP)
			{
				op2_val = atoi(op2);
				op2_type = ADDRESSING_MODE_HASH_NUM;	/* 0 */
				flag2 = (op1_type == 0) ? 0 : 1;		/* Packing optimization if both regs */
				found_operand = 1;
			}
			else
			{
				fprintf(stderr, "error in line:%d Immediate operand not allowed as destination for command %s\n", sum_of_row, command);
				error = 1;
				return 0;
			}
		}
		/* Register */
		else if (reg(op2))
		{
			op2_val = atoi(op2 + 1);
			op2_type = ADDRESSING_MODE_REG;				/* 3 */
			flag2 = (op1_type == 3) ? 0 : 1;
			found_operand = 1;
		}
		else
		{
			/* Symbol */
			op2_semel_index = valid_SEMEL(op2, SEMELS, semel_count);
			if (op2_semel_index != -1)
			{
				op2_val = SEMELS[op2_semel_index]->addres;
				op2_type = ADDRESSING_MODE_SEMEL;		/* 1 */
				flag2 = 1;
				found_operand = 1;
			}
			else
			{
				/* Matrix */
				op2_mat = valid_matrix(op2, SEMELS, semel_count);
				if (op2_mat != NULL)
				{
					op2_type = ADDRESSING_MODE_SEMEL;	/* Matrix is encoded as symbol + indices */
					flag2 = 2;
					found_operand = 1;
				}
				else
				{
					fprintf(stderr, "error in line:%d Invalid operand: %s\n", sum_of_row, op2);
					error = 1;
					return 0;
				}
			}
		}

		if (found_operand == 0)
		{
			error = 1;
			fprintf(stderr, "error in line:%d unvalid operand", sum_of_row);
		}
	}

	/* First word: opcode + addressing modes */
	if (op1_type == -1 && op2_type == -1)
		to_binary(opcode, 0, 0, array);
	else if (op2_type == -1)
		to_binary(opcode, 0, op1_type, array);
	else
		to_binary(opcode, op1_type, op2_type, array);

	/* Extra words (according to operand kinds) */
	if (flag1 == -1 && flag2 == -1)
		return 1;
	else if (flag2 == -1)
	{
		/* Only op1 needs extra word(s) */
		if (op1_type == ADDRESSING_MODE_SEMEL)
		{
			if (SEMELS[semel_index]->ex_en == 1)
				add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op1);
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, SEMELS[semel_index]->ex_en);
		}
		else if (op1_type == ADDRESSING_MODE_HASH_NUM)
		{
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, 0);
		}
		else if (op1_type == ADDRESSING_MODE_REG)
		{
			add_two_numbers(0, op1_val, array);
		}
		else if (op1_type == ADDRESSING_MODE_MATRIX)
		{
			if (SEMELS[op1_mat[0]]->ex_en == 1)
			{
				temp_semel = strtok(op1, "[");
				add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, temp_semel);
			}
			add_number(SEMELS[op1_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op1_mat[0]]->ex_en);
			add_two_numbers(op1_mat[1], op1_mat[2], array);
			free(op1_mat);
			op1_mat = NULL;
		}
	}
	else if (flag1 + flag2 == 0)	/* Both regs -> pack in one word */
	{
		add_two_numbers(op1_val, op2_val, array);
	}
	else if (flag1 == 2)
	{
		/* op1 is matrix */
		if (SEMELS[op1_mat[0]]->ex_en == 1)
		{
			temp_semel = strtok(op1, "[");
			add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, temp_semel);
		}
		add_number(SEMELS[op1_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op1_mat[0]]->ex_en);
		add_two_numbers(op1_mat[1], op1_mat[2], array);
		free(op1_mat);
		op1_mat = NULL;
	}
	else if (flag1 == 1)
	{
		/* op1 is symbol */
		if (SEMELS[semel_index]->ex_en == 1)
			add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op1);
		add_number(op1_val, (void**)array, TYPE_INSTRUCTION, SEMELS[semel_index]->ex_en);
	}
	else
	{
		/* op1 immediate/reg */
		if (op1_type == ADDRESSING_MODE_HASH_NUM)
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, 0);
		if (op1_type == ADDRESSING_MODE_REG)
			add_two_numbers(op1_val, 0, array);
	}

	/* Extra words for op2 if present */
	if (flag2 == 2)
	{
		/* op2 is matrix */
		if (SEMELS[op2_mat[0]]->ex_en == 1)
		{
			temp_semel = strtok(op2, "[");
			add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, temp_semel);
		}
		add_number(SEMELS[op2_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op2_mat[0]]->ex_en);
		add_two_numbers(op2_mat[1], op2_mat[2], array);
		free(op2_mat);
		op2_mat = NULL;
	}
	else if (flag2 == 1)
	{
		/* op2 is symbol/immediate/reg */
		if (op2_type == ADDRESSING_MODE_SEMEL)
		{
			if (SEMELS[op2_semel_index]->ex_en == 1)
				add_to_extern_label(extern_labels, COUNT_OF_EXTERN_LABEL, op2);
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION, SEMELS[op2_semel_index]->ex_en);
		}
		else if (op2_type == ADDRESSING_MODE_HASH_NUM)
		{
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION, 0);
		}
		else
		{
			add_two_numbers(0, op2_val, array);
		}
	}

	return 0;
}

void data(char row[], binary_directive** struct_DC)
{
	char *op1;							/* Current token after .data */
	int value;							/* Parsed integer value */

	if (!(check_commas(row)))
		return;
	/* Skip the directive token itself */
	op1 = strtok(row, " ");
	op1 = strtok(NULL, ",");
	/* Parse comma-separated integers */
	while (op1 != NULL)
	{
		/* Trim leading spaces on the token */
		while (*op1 == ' ' || *op1 == '\t')
			op1++;
		/* Validate integer token */
		if (is_valid_number(op1, 1))
		{
			value = atoi(op1);
			add_number(value, (void**)struct_DC, TYPE_DIRECTIVE, 4);
		}
		else
		{
			fprintf(stderr, "in line %d error: Invalid operand: %s\n", sum_of_row, op1);
			error = 1;
		}

		op1 = strtok(NULL, ",");
	}
}

void string(char row[], binary_directive** struct_DC)
{
	char *start_quote;					/* Pointer to opening quote */
	char *end_quote;					/* Pointer to closing quote */
	int i;								/* Loop index over string characters */

	if (!(string_commas_check(row)))
		return;
	/* Find the first '"' */
	start_quote = strchr(row, '"');
	/* Find the matching '"' */
	end_quote = strchr(start_quote + 1, '"');
	/* Emit characters between the quotes as bytes */
	if (end_quote != NULL && start_quote != NULL)
	{
		for (i = 1; start_quote + i < end_quote; i++)
		{
			char c = start_quote[i];
			add_number((int)c, (void**)struct_DC, TYPE_DIRECTIVE, 4);
		}
	}
	/* Terminate string with '\0' */
	add_number(0, (void**)struct_DC, TYPE_DIRECTIVE, 4);
}

int string_commas_check(char *row)
{
	char *colon;
	/* Move to the character after the 'g' of ".string" (per original parsing logic) */
	colon = strchr(row, 'g') + 1;
	/* Skip spaces until newline or non-space */
	while ((isspace(*colon)) && (*colon != '\n'))
		colon++;
	/* The next non-space must be the opening quote */
	if (*colon != '"')
	{
		error = 1;
		fprintf(stderr, "in line:%d unexpected char\n", sum_of_row);
		return 0;
	}
	else
	{
		colon += 1;
		colon = strchr(colon, '"') + 1;		/* Move past the closing quote */
		if (colon == NULL)
		{
			error = 1;
			fprintf(stderr, "in line:%d missing last quotes\n", sum_of_row);
			return 0;
		}
		/* Only whitespace is allowed after the closing quote up to newline */
		while (isspace(*colon) && *colon != '\n')
			colon++;

		if (*colon != '\n')
		{
			error = 1;
			fprintf(stderr, "in line:%d unexpected char\n", sum_of_row);
			return 0;
		}
	}
	return 1;
}

void mat(char row[], binary_directive** struct_DC)
{
	char *open1, *close1, *open2, *close2, *after_brackets;	/* Bracket pointers */
	int row_val, col_val;										/* Matrix dimensions */
	char temp[31];												/* Temp buffer for bracket content */
	char *token;												/* Token for initialization list */
	/* Normalize commas/spacing first */
	if (!check_commas(row))
		return;
	/* First pair of brackets: rows */
	open1 = strchr(row, '[');
	if (!open1)
	{
		fprintf(stderr, "in line %d error,Missing first '['\n", sum_of_row);
		error = 1;
	}
	close1 = strchr(open1, ']');
	if (!close1 || close1 < open1)
	{
		fprintf(stderr, "in line %d error, Missing first ']'\n", sum_of_row);
		error = 1;
	}
	/* Second pair of brackets: columns */
	open2 = strchr(close1, '[');
	if (!open2)
	{
		fprintf(stderr, "in line %d error, Missing second '['\n", sum_of_row);
		error = 1;
	}
	close2 = strchr(open2, ']');
	if (!close2 || close2 < open2)
	{
		fprintf(stderr, "in line %d error, Missing second ']'\n", sum_of_row);
		error = 1;
	}
	/* Extract row dimension */
	strncpy(temp, open1 + 1, close1 - open1 - 1);
	temp[close1 - open1 - 1] = '\0';
	if (!is_valid_number(temp, 2))
	{
		fprintf(stderr, "in line %d error, Invalid row number\n", sum_of_row);
		error = 1;
	}
	else
	{
		row_val = atoi(temp);
	}
	/* Extract column dimension */
	strncpy(temp, open2 + 1, close2 - open2 - 1);
	temp[close2 - open2 - 1] = '\0';
	if (!is_valid_number(temp, 2))
	{
		fprintf(stderr, "in line %d error, Invalid col number\n", sum_of_row);
		error = 1;
	}
	else
	{
		col_val = atoi(temp);
	}
	/* Initialization values come after the second ']' */
	after_brackets = close2 + 1;
	/* No initialization list: fill with zeros */
	if (*after_brackets == '\0')
	{
		int i;
		for (i = 0; i < row_val * col_val; i++)
			add_number(0, (void**)struct_DC, TYPE_DIRECTIVE, 4);
	}
	else
	{
		/* Parse comma-separated initial values */
		token = strtok(after_brackets, ",");
		while (token != NULL)
		{
			if (!is_valid_number(token, 1))
			{
				fprintf(stderr, "in line %d error, Invalid number: %s\n", sum_of_row, token);
				error = 1;
			}
			else
			{
				add_number(atoi(token), (void**)struct_DC, TYPE_DIRECTIVE, 4);
			}
			token = strtok(NULL, ",");
		}
	}
}

void entry_extern(char row[])
{
	char *command;						/* Directive name: .entry / .extern */
	char *extra;						/* Used to detect extra params */

	command = strtok(row, " \t\n\r");	/* .entry / .extern */
	extra = strtok(NULL, " \t\n\r");	/* Skip the label name */
	extra = strtok(NULL, " \t\n\r");	/* Anything else is an error */

	if (!(check_commas(row)))
		return;
	/* No extra parameters are allowed */
	if (extra != NULL)
	{
		fprintf(stderr, "in line %d error, too many parameters in func %s\n", sum_of_row, command);
		error = 1;
	}
}

