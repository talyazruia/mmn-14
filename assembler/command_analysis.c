#include "assembler.h"

int is_valid_integer(const char *str) 
{
    char *endptr;
    long val;

    if (str == NULL) return 0;

    /* דילוג על רווחים בתחילה */
    while (isspace((unsigned char)*str)) str++;

    val = strtol(str, &endptr, 10); 

    if (str == endptr) return 0; 

    /* דילוג על רווחים אחרי המספר */
    while (isspace((unsigned char)*endptr)) endptr++;

    if (*endptr != '\0') return 0; 

    if (val < INT_MIN || val > INT_MAX) return 0; 

    return 1;
}

void to_binary(int opcode, int op1, int op2, binary_code** array) {
    binary_code* temp;

    if (*array == NULL) {
        *array = (binary_code*) malloc(sizeof(binary_code));
        if (*array == NULL) {
            fprintf(stderr, "Error: malloc failed.\n");
            return;
        }
    } else {
        temp = (binary_code*) realloc(*array, (current_size_instaction_struct + 1) * sizeof(binary_code));
        if (temp == NULL) {
            fprintf(stderr, "Error: realloc failed.\n");
            free(*array);
            *array = NULL;
            return;
        }
        *array = temp;
    }

    /* קידוד לפי סיביות:
       bits 0-3: opcode
       bits 4-5: op1
       bits 6-7: op2
    */
    (*array)[current_size_instaction_struct].first =
        ((op2 & 0x03) << 6) | ((op1 & 0x03) << 4) | (opcode & 0x0F);

    (*array)[current_size_instaction_struct].second = 0;

    current_size_instaction_struct++;
}

    /* הוספת איבר נוסף */
    

    /* הדפסה לדוגמה */
   
    /* ניקוי זיכרון */
    /*free(array);*/
    /*return ;
}

*/

int isHashNumber( char* str) {
	if (str == NULL || str[0] != '#') return 0; 

	str++; 

	if (*str == '\0') return 0; 

	if(*str=='-')
		str++;
	if (*str == '\0') return 0;
	
	while(*str!='\0')
	{
		if (!isdigit((unsigned char)*str)) 
			return 0;
		str++;
	} 
	return 1;
}

int reg(char* str) {
	if (str == NULL) return 0;

	/*בודק שהתוו הראשון הוא 'r'*/ 
	if (str[0] != 'r') return 0;

	/* בודק שהתוו השני קיים והוא ספרה בין '0' ל-'7'*/
	if (str[1] < '0' || str[1] > '7') return 0;

	/* בודק שאין עוד תווים אחרי הספרה*/
	if (str[2] != '\0') return 0;

	return 1;
}

int* valid_matrix(char* str,SEMEL** SEMELS, int* semel_count)
{
	char array_name[31];
	char r1[31];
	char r2[31];
	int *result = NULL;
	int i;

	result = (int*)malloc(3 * sizeof(int));
    	if (result == NULL) 
	{
        	fprintf(stderr, "Error: memory allocation failed\n");
        	return NULL;
    	}
	if (sscanf(str, "%30[^[][%30[^]]][%30[^]]]", array_name, r1, r2) == 3) 
	{
			i=valid_SEMEL(array_name,SEMELS,semel_count);
			if(i != -1) 
			{
				if (reg(r1) == 1) 
				{
					if (reg(r2) == 1)
					{
						result[0]= i;
						result[1] = r1[1]-'0';
				        	result[2] = r2[1]-'0';
						return result;
					}
				}
			}
					
	}

	free(result);
	return NULL;
}
int valid_SEMEL(char* str, SEMEL** SEMELS, int* semel_count)
{
	int i = 0;
	while (i < *(semel_count))
	{
		if (strcmp(SEMELS[i]->name, str) == 0)
		{
			if(SEMELS[i]->ex_en!=0)
				   return i;
			else
			{
				error=1;
				fprintf(stderr, "label oprand can not be entery");
				return -1;
			}
		}
		i++;
	}
	fprintf(stderr, "Error: undefined label: %s\n", str);
    	error = 1;
	return -1;

}  


/*int add(char row[],SEMEL** SEMELS, int* semel_count, command cmd[], binary_code** array)
{
	fprintf(stderr, "=== ENTERING ADD FUNCTION ===\n");
    fprintf(stderr, "Input row: '%.100s'\n", row);
    fprintf(stderr, "SEMELS=%p, semel_count=%p\n", (void*)SEMELS, (void*)semel_count);
    fprintf(stderr, "semel_count value=%d\n", *semel_count);
    fprintf(stderr, "array=%p\n", (void*)array);
	char* command;
	char* op1=NULL;
	char* op2=NULL;
	char* op3;
	int opcode=-1;
	int i = 0;
	int op1_val=0;
	int op2_val=0;
	int op1_type=0;
	int* op1_mat=NULL;
	int* op2_mat=NULL;
	int op2_type=0;
	int flag1=0;
	int flag2=0;
	int semel_index=0;
	int num_operands = 0;
    	int required_operands = 0;
	int temp_addres;
	int temp_ex_en;

	fprintf(stderr, "About to strtok for command\n");
	command = strtok(row, " ");
	fprintf(stderr, "command = %s\n", command ? command : "NULL");
	if (command == NULL) {
        fprintf(stderr, "Error: Empty command\n");
        error = 1;
        return 0;
    	}
	fprintf(stderr, "About to find opcode\n");
	while (i < 16)
	{
		if (strcmp(cmd[i].name, command) == 0)
		{
			opcode = cmd[i].op;
			fprintf(stderr, "Found opcode: %d\n", opcode);
			required_operands = cmd[i].param;
			break;
		}
		i++;
	}

	if (opcode == -1) {
        fprintf(stderr, "Error: Unknown command %s\n", command);
        error = 1;
        return 0;
    	}
	fprintf(stderr, "About to get operands\n");
	fprintf(stderr, "required_operands = %d\n", required_operands);
	if (required_operands >= 1) 
	{
		fprintf(stderr, "Getting first operand\n");
		if (required_operands == 2) 
		{
			fprintf(stderr, "Two operands expected, using comma delimiter\n");
			op1 = strtok(NULL, ",");
		} 
		else 
		{
			fprintf(stderr, "One operand expected, using space delimiter\n");
			op1 = strtok(NULL, " \t\n");
		}
		fprintf(stderr, "op1 = %s\n", op1 ? op1 : "NULL");
	if (op1 == NULL)
	{
		fprintf(stderr, "Error: Missing operand in command %s\n", command);
		error = 1;
		return 0;
	}
	fprintf(stderr, "About to trim op1\n");
	while (*op1 == ' ' || *op1 == '\t') op1++;
	fprintf(stderr, "op1 after trim = %s\n", op1);
	num_operands++;
	fprintf(stderr, "num_operands = %d\n", num_operands);
	}
	if(required_operands>=2)
	{
	  fprintf(stderr, "Getting second operand\n");
	op2 = strtok(NULL, " ");
		 fprintf(stderr, "op2 = %s\n", op2 ? op2 : "NULL");
	if (op2 == NULL)
	{
		fprintf(stderr, "Error: Missing second operand in command %s\n", command);
		error = 1;
           	return 0;
	}
	fprintf(stderr, "About to trim op2\n");
	while (*op2 == ' ' || *op2 == '\t') op2++;
	 fprintf(stderr, "op2 after trim = %s\n", op2);
	num_operands++;
	fprintf(stderr, "num_operands = %d\n", num_operands);
	}
	fprintf(stderr, "About to check for extra operands\n");
	if (required_operands > 0) 
	{
		fprintf(stderr, "Checking for third operand\n");
		op3 = strtok(NULL, " \t\n");
		fprintf(stderr, "op3 = %s\n", op3 ? op3 : "NULL");
		if(op3 != NULL && strlen(op3) > 0)
		{
		fprintf(stderr, "Error: too many operands in command %s\n", command);
		error=1;
		return 0;
		}
	}
	fprintf(stderr, "About to validate operand count\n");
	if (num_operands != required_operands) 
	{
		if (num_operands < required_operands) 
		{
			fprintf(stderr, "Error: Missing operand in command %s\n", command);
		} 
		else 
		{
			fprintf(stderr, "Error: too many operands in command %s\n", command);
		}
		error = 1;
		return 0;
	}
	fprintf(stderr, "About to process op1\n");
	if (op1 != NULL) 
	{
		fprintf(stderr, "Processing op1: %s\n", op1);
		if (isHashNumber(op1)) 
		{
			fprintf(stderr, "op1 is hash number\n");
			
			if (opcode == 1 || opcode == 12 || opcode == 13) 
			{ 
				op1_val = atoi(op1 + 1); 
				op1_type = 0;
				flag1 = 0;
			} 
			else 
			{
				fprintf(stderr, "Error: Immediate operand not allowed for command %s\n", command);
				error = 1;
				return 0;
			}
		}
		else if(reg(op1))
		{
			fprintf(stderr, "op1 is register\n");
			op1_val = atoi(op1 + 1);
			op1_type = 3;
			flag1 = 0;
		}
		else 
		{
			fprintf(stderr, "op1 is neither hash nor register, checking matrix\n");
			op1_mat = valid_matrix(op1, SEMELS, semel_count);
			fprintf(stderr, "valid_matrix returned: %p\n", (void*)op1_mat);
			if (op1_mat != NULL) 
			{
				fprintf(stderr, "op1 is valid matrix\n");
				op1_type = 2;
				flag1 = 2;
				fprintf(stderr, "Set op1_type=2, flag1=2\n");
			}
			else 
			{
				fprintf(stderr, "op1 is not matrix, checking SEMEL\n");
				semel_index = valid_SEMEL(op1, SEMELS, semel_count);
				fprintf(stderr, "valid_SEMEL returned: %d\n", semel_index);
				if(semel_index != -1)
				{
					fprintf(stderr, "op1 is valid SEMEL\n");
					op1_val = SEMELS[semel_index]->addres; 
					op1_type = 1;
					flag1 = 1;
					fprintf(stderr, "Set op1_val=%d, op1_type=1, flag1=1\n", op1_val);
				}
				else 
				{
					fprintf(stderr, "Error: Invalid operand: %s\n", op1);
					error = 1;
					return 0;
				}
			}
		}
	}
	fprintf(stderr, "Finished processing op1, about to process op2\n");
	if (op2 != NULL) 
	{
		fprintf(stderr, "Processing op2: %s\n", op2);
		if (isHashNumber(op2)) 
		{
			fprintf(stderr, "op2 is hash number\n");
			if (opcode == 1 || opcode == 13) 
			{
				op2_val = atoi(op2 + 1); 
				op2_type = 0;
				flag2 =(op1_type == 0) ? 0 : 1;
			} 
			else 
			{
				fprintf(stderr, "Error: Immediate operand not allowed as destination for command %s\n", command);
				error = 1;
				return 0;
			}
		}
		else if(reg(op2))
		{
			fprintf(stderr, "op2 is register\n");
			op2_val = atoi(op2 + 1);
			op2_type = 3;
			flag2 = (op1_type == 3) ? 0 : 1;
		}
		else
		{
			fprintf(stderr, "op2 is neither hash nor register, checking SEMEL\n");
			semel_index = valid_SEMEL(op2, SEMELS, semel_count);
			fprintf(stderr, "valid_SEMEL for op2 returned: %d\n", semel_index);
        		if (semel_index != -1)
        		{
				fprintf(stderr, "op2 is valid SEMEL, index=%d\n", semel_index);
    fprintf(stderr, "About to access SEMELS[%d]\n", semel_index);
				if (SEMELS == NULL) {
        fprintf(stderr, "ERROR: SEMELS is NULL!\n");
        return 0;
    }
    
    if (semel_index >= *semel_count) {
        fprintf(stderr, "ERROR: semel_index (%d) >= semel_count (%d)\n", semel_index, *semel_count);
        return 0;
    }
    
    if (SEMELS[semel_index] == NULL) {
        fprintf(stderr, "ERROR: SEMELS[%d] is NULL!\n", semel_index);
        return 0;
    }
    
    fprintf(stderr, "SEMELS[%d] looks valid, accessing addres\n", semel_index);
				op2_val = SEMELS[semel_index]->addres;
				fprintf(stderr, "op2_val = %d\n", op2_val); 
				op2_type = 1;
				flag2 = 1;
				fprintf(stderr, "Set op2_type=1, flag2=1\n");
			}
			else
        		{
				fprintf(stderr, "op2 matrix check\n");
            			op2_mat = valid_matrix(op2, SEMELS, semel_count);
				fprintf(stderr, "valid_matrix for op2 returned: %p\n", (void*)op2_mat);
            			if (op2_mat != NULL) 
            			{
                			op2_type = 2;
                			flag2 = 2;
            			}
				else {
					fprintf(stderr, "Error: Invalid operand: %s\n", op2);
					error = 1;
					return 0;
				}
        		}
    		}
	}
	fprintf(stderr, "About to call to_binary\n");
	to_binary(opcode, op1_type, op2_type, array);
	fprintf(stderr, "to_binary completed\n");

	fprintf(stderr, "Checking flags: flag1=%d, flag2=%d\n", flag1, flag2);
	
	if (flag1 + flag2 == 1)
	{
		fprintf(stderr, "Case: flag1 + flag2 == 1, calling add_two_numbers\n");
		add_two_numbers(op1_val,op2_val,array);
		fprintf(stderr, "add_two_numbers completed\n");
	}
	else if (flag1==2)
	{
		fprintf(stderr, "Case: flag1==2 (matrix)\n");
		if (op1_mat == NULL) {
        fprintf(stderr, "ERROR: op1_mat is NULL!\n");
        return 0;
    }
    
    fprintf(stderr, "About to access op1_mat[0]\n");
    fprintf(stderr, "op1_mat[0] = %d\n", op1_mat[0]);
    
    if (op1_mat[0] < 0 || op1_mat[0] >= *semel_count) {
        fprintf(stderr, "ERROR: op1_mat[0] (%d) out of bounds\n", op1_mat[0]);
        return 0;
    }
    fprintf(stderr, "Case: flag1==2 (matrix)\n");

if (op1_mat == NULL) {
    fprintf(stderr, "ERROR: op1_mat is NULL!\n");
    return 0;
}

fprintf(stderr, "About to access op1_mat[0]\n");
fprintf(stderr, "op1_mat[0] = %d\n", op1_mat[0]);

if (op1_mat[0] < 0 || op1_mat[0] >= *semel_count) {
    fprintf(stderr, "ERROR: op1_mat[0] (%d) out of bounds (semel_count=%d)\n", op1_mat[0], *semel_count);
    return 0;
}

fprintf(stderr, "About to check SEMELS[%d]\n", op1_mat[0]);
if (SEMELS[op1_mat[0]] == NULL) {
    fprintf(stderr, "ERROR: SEMELS[%d] is NULL!\n", op1_mat[0]);
    return 0;
}

fprintf(stderr, "SEMELS[%d]->addres = %d\n", op1_mat[0], SEMELS[op1_mat[0]]->addres);
fprintf(stderr, "SEMELS[%d]->ex_en = %d\n", op1_mat[0], SEMELS[op1_mat[0]]->ex_en);

fprintf(stderr, "About to check array parameter\n");
if (array == NULL) {
    fprintf(stderr, "ERROR: array parameter is NULL!\n");
    return 0;
}

 temp_addres = SEMELS[op1_mat[0]]->addres;
 temp_ex_en = SEMELS[op1_mat[0]]->ex_en;

fprintf(stderr, "Calling add_number with separated parameters:\n");
fprintf(stderr, "  temp_addres = %d\n", temp_addres);
fprintf(stderr, "  temp_ex_en = %d\n", temp_ex_en);
		/*add_number(SEMELS[op1_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION ,SEMELS[op2_mat[0]]->ex_en);
		add_number(temp_addres, (void**)array, TYPE_INSTRUCTION, temp_ex_en);
		fprintf(stderr, "add_number for matrix completed\n");
	fprintf(stderr, "About to call add_two_numbers for matrix indices\n");

		add_two_numbers(op1_mat[1],op1_mat[2] ,array);
		fprintf(stderr, "add_two_numbers for matrix completed\n");
    
    fprintf(stderr, "About to free op1_mat\n");
		free(op1_mat);
		fprintf(stderr, "op1_mat freed\n");
	}
	else if (flag1==1)
	{
		fprintf(stderr, "Case: flag1==1\n");
		if(op1_type==1)
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION,SEMELS[op2_mat[0]]->ex_en);
		else
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION,0);
	}
	fprintf(stderr, "About to check flag2 conditions\n");
	fprintf(stderr, "About to check flag2\n");
	if (flag2 == 2)
	{
		fprintf(stderr, "Case: flag2==2 (matrix)\n");
		if (op2_mat == NULL) {
        fprintf(stderr, "ERROR: op2_mat is NULL!\n");
        return 0;
    }
    
    fprintf(stderr, "About to access op2_mat[0]\n");
		add_number(SEMELS[op2_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION,SEMELS[op2_mat[0]]->ex_en);
		add_two_numbers(op2_mat[1], op2_mat[2] ,array);
		free(op2_mat);
	}
	else if(flag1+flag2!=1 && flag2 == 1)
	{
		fprintf(stderr, "Case: flag1+flag2!=1 && flag2==1\n");
    fprintf(stderr, "flag1=%d, flag2=%d, sum=%d\n", flag1, flag2, flag1+flag2);
		if(op2_type==1)
			fprintf(stderr, "op2_type==1, calling add_number\n");
			fprintf(stderr, "semel_index for op2 = %d\n", semel_index);
        
        if (semel_index < 0 || semel_index >= *semel_count) {
            fprintf(stderr, "ERROR: semel_index out of bounds\n");
            return 0;
        }
        
        if (SEMELS[semel_index] == NULL) {
            fprintf(stderr, "ERROR: SEMELS[semel_index] is NULL\n");
            return 0;
        }
        
        fprintf(stderr, "About to call add_number for op2\n");
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION,SEMELS[op2_mat[0]]->ex_en);
		fprintf(stderr, "add_number for op2 completed\n");}
		else{
			fprintf(stderr, "op2_type!=1, calling add_number with 0\n");
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION,0);
		fprintf(stderr, "add_number for op2 completed\n");
	}
	fprintf(stderr, "About to return from add function\n");
	return 0;
}*/

int add(char row[],SEMEL** SEMELS, int* semel_count, command cmd[], binary_code** array)
{
	char* command;
	char* op1=NULL;
	char* op2=NULL;
	char* op3;
	int opcode=-1;
	int i = 0;
	int op1_val=0;
	int op2_val=0;
	int op1_type=0;
	int* op1_mat=NULL;
	int* op2_mat=NULL;
	int op2_type=0;
	int flag1=0;
	int flag2=0;
	int semel_index=0;
	int op2_semel_index = -1;  /* שמירת אינדקס עבור op2 */
	int num_operands = 0;
	int required_operands = 0;

	command = strtok(row, " ");
	if (command == NULL) {
		fprintf(stderr, "Error: Empty command\n");
		error = 1;
		return 0;
	}

	while (i < 16)
	{
		if (strcmp(cmd[i].name, command) == 0)
		{
			opcode = cmd[i].op;
			required_operands = cmd[i].param;
			break;
		}
		i++;
	}

	if (opcode == -1) {
		fprintf(stderr, "Error: Unknown command %s\n", command);
		error = 1;
		return 0;
	}

	if (required_operands >= 1) 
	{
		if (required_operands == 2) 
		{
			op1 = strtok(NULL, ",");
		} 
		else 
		{
			op1 = strtok(NULL, " \t\n");
		}
		if (op1 == NULL)
		{
			fprintf(stderr, "Error: Missing operand in command %s\n", command);
			error = 1;
			return 0;
		}
		while (*op1 == ' ' || *op1 == '\t') op1++;
		num_operands++;
	}
	
	if(required_operands>=2)
	{
		op2 = strtok(NULL, " ");
		if (op2 == NULL)
		{
			fprintf(stderr, "Error: Missing second operand in command %s\n", command);
			error = 1;
			return 0;
		}
		while (*op2 == ' ' || *op2 == '\t') op2++;
		num_operands++;
	}
	
	if (required_operands > 0) 
	{
		op3 = strtok(NULL, " \t\n");
		if(op3 != NULL && strlen(op3) > 0)
		{
			fprintf(stderr, "Error: too many operands in command %s\n", command);
			error=1;
			return 0;
		}
	}
	
	if (num_operands != required_operands) 
	{
		if (num_operands < required_operands) 
		{
			fprintf(stderr, "Error: Missing operand in command %s\n", command);
		} 
		else 
		{
			fprintf(stderr, "Error: too many operands in command %s\n", command);
		}
		error = 1;
		return 0;
	}

	/* עיבוד op1 */
	if (op1 != NULL) 
	{
		if (isHashNumber(op1)) 
		{
			if (opcode == 1 || opcode == 12 || opcode == 13) 
			{ 
				op1_val = atoi(op1 + 1); 
				op1_type = 0;
				flag1 = 0;
			} 
			else 
			{
				fprintf(stderr, "Error: Immediate operand not allowed for command %s\n", command);
				error = 1;
				return 0;
			}
		}
		else if(reg(op1))
		{
			op1_val = atoi(op1 + 1);
			op1_type = 3;
			flag1 = 0;
		}
		else 
		{
			op1_mat = valid_matrix(op1, SEMELS, semel_count);
			if (op1_mat != NULL) 
			{
				op1_type = 2;
				flag1 = 2;
			}
			else 
			{
				semel_index = valid_SEMEL(op1, SEMELS, semel_count);
				if(semel_index != -1)
				{
					op1_val = SEMELS[semel_index]->addres; 
					op1_type = 1;
					flag1 = 1;
				}
				else 
				{
					fprintf(stderr, "Error: Invalid operand: %s\n", op1);
					error = 1;
					return 0;
				}
			}
		}
	}

	/* עיבוד op2 */
	if (op2 != NULL) 
	{
		if (isHashNumber(op2)) 
		{
			if (opcode == 1 || opcode == 13) 
			{
				op2_val = atoi(op2 + 1); 
				op2_type = 0;
				flag2 = (op1_type == 0) ? 0 : 1;
			} 
			else 
			{
				fprintf(stderr, "Error: Immediate operand not allowed as destination for command %s\n", command);
				error = 1;
				return 0;
			}
		}
		else if(reg(op2))
		{
			op2_val = atoi(op2 + 1);
			op2_type = 3;
			flag2 = (op1_type == 3) ? 0 : 1;
		}
		else
		{
			op2_semel_index = valid_SEMEL(op2, SEMELS, semel_count);
			if (op2_semel_index != -1)
			{
				op2_val = SEMELS[op2_semel_index]->addres; 
				op2_type = 1;
				flag2 = 1;
			}
			else
			{
				op2_mat = valid_matrix(op2, SEMELS, semel_count);
				if (op2_mat != NULL) 
				{
					op2_type = 2;
					flag2 = 2;
				}
				else 
				{
					fprintf(stderr, "Error: Invalid operand: %s\n", op2);
					error = 1;
					return 0;
				}
			}
		}
	}
	
	to_binary(opcode, op1_type, op2_type, array);
	
	if (flag1 + flag2 == 1)
	{
		add_two_numbers(op1_val, op2_val, array);
	}
	else if (flag1 == 2)
	{
		add_number(SEMELS[op1_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op1_mat[0]]->ex_en);
		add_two_numbers(op1_mat[1], op1_mat[2], array);
		free(op1_mat);
		op1_mat = NULL;
	}
	else if (flag1 == 1)
	{
		if(op1_type == 1)
		{
			/* מצא את האינדקס של op1 שוב */
			int op1_semel_index = valid_SEMEL(op1, SEMELS, semel_count);
			if (op1_semel_index != -1) {
				add_number(op1_val, (void**)array, TYPE_INSTRUCTION, SEMELS[op1_semel_index]->ex_en);
			} else {
				add_number(op1_val, (void**)array, TYPE_INSTRUCTION, 0);
			}
		}
		else
			add_number(op1_val, (void**)array, TYPE_INSTRUCTION, 0);
	}
	
	if (flag2 == 2)
	{
		add_number(SEMELS[op2_mat[0]]->addres, (void**)array, TYPE_INSTRUCTION, SEMELS[op2_mat[0]]->ex_en);
		add_two_numbers(op2_mat[1], op2_mat[2], array);
		free(op2_mat);
		op2_mat = NULL;
	}
	else if(flag1 + flag2 != 1 && flag2 == 1)
	{
		if(op2_type == 1)
		{
			if (op2_semel_index != -1) {
				add_number(op2_val, (void**)array, TYPE_INSTRUCTION, SEMELS[op2_semel_index]->ex_en);
			} else {
				add_number(op2_val, (void**)array, TYPE_INSTRUCTION, 0);
			}
		}
		else
		{
			add_number(op2_val, (void**)array, TYPE_INSTRUCTION, 0);
		}
	}
	
	return 0;
}

void data(char row[],binary_directive** struct_DC) {
    	char* command;
    	char* op1;
	int value;
    	command = strtok(row, " ");
    	op1 = strtok(NULL, ",");

    	if (op1 == NULL) 
	{
        	fprintf(stderr, "Error: Missing operand in command %s\n", command);
        	error = 1;
        	return;
    	}
	while (op1 != NULL) 
	{
		while (*op1 == ' ' || *op1 == '\t') op1++;
		if (is_valid_integer(op1)) 
		{
			value=atoi(op1);
            		add_number(value, (void**)struct_DC, TYPE_DIRECTIVE,4);
        	} 
		else 
		{
            		fprintf(stderr, "Error: Invalid operand: '%s'\n", op1);
            		error = 1;
        	}
		op1 = strtok(NULL, ",");
    	}
}


void string(char row[],binary_directive** struct_DC)
{
	char *start_quote, *end_quote;
	int i;

	/* locate the first quote */
	start_quote = strchr(row, '"');
	if (start_quote == NULL)
        {
        fprintf(stderr, "Missing opening quote for string\n");
        error = 1;
        return;
    }	
	

	/* locate the ending quote */
	end_quote = strchr(start_quote + 1, '"');
	if (end_quote == NULL) {
		fprintf(stderr, "Missing closing quote for string\n");
		error = 1;
		
	}

	if(end_quote != NULL&&start_quote != NULL)/* validate and send each character between the quotes */
	for (i = 1; start_quote + i < end_quote; i++) {
		char c = start_quote[i];
		add_number((int)c,(void**)struct_DC, TYPE_DIRECTIVE,4);
	}

	/* add null terminator */
	add_number(0,(void**)struct_DC, TYPE_DIRECTIVE,4);
}


void mat(char row[], binary_directive** struct_DC)
{
	char  *open1, *close1, *open2, *close2, *after_brackets;
	int row_val, col_val;
	char temp[31];
	char *token;

	fix_commas_in_place(row);
	open1 = strchr(row, '[');
	if (!open1) {
		/* missing first '[' */
		fprintf(stderr, "Missing first '['\n");
		error = 1;
	}
	close1 = strchr(open1, ']');
	if (!close1 || close1 < open1) {
		/* missing first ']' */
		fprintf(stderr, "Missing first ']'\n");
		error = 1;
	}

	open2 = strchr(close1, '[');
	if (!open2) {
		/* missing second '[' */
		fprintf(stderr, "Missing second '['\n");
		error = 1;
	}
	close2 = strchr(open2, ']');
	if (!close2 || close2 < open2) {
		/* missing second ']' */
		fprintf(stderr, "Missing second ']'\n");
		error = 1;
	}

	/* extract the value between the first brackets */
	strncpy(temp, open1 + 1, close1 - open1 - 1);
	temp[close1 - open1 - 1] = '\0';
	if (!is_valid_integer(temp)) {
		fprintf(stderr, "Invalid row number\n");
		error = 1;
	}
	else

	row_val = atoi(temp);

	/* extract the value between the second brackets */
	strncpy(temp, open2 + 1, close2 - open2 - 1);
	temp[close2 - open2 - 1] = '\0';
	if (!is_valid_integer(temp)) {
		fprintf(stderr, "Invalid col number\n");
		error=1;
	}
	else
	col_val = atoi(temp);

	/* check if there are more numbers after the brackets */
	after_brackets = close2 + 1;

	if (*after_brackets == '\0' && error == 0) {
		/* no additional numbers */
		int i;
		for (i = 0; i < row_val * col_val; i++) {
			add_number(0,(void**)struct_DC, TYPE_DIRECTIVE,4);
		}
	} else if(error==0){
		/* additional numbers exist – send each one separately */
		token = strtok(after_brackets, " ,");
		while (token != NULL) {
			if (!is_valid_integer(token)) {
				fprintf(stderr, "Invalid number: %s\n", token);
				error = 1;
			} else {
				add_number(atoi(token),(void**)struct_DC, TYPE_DIRECTIVE,4);
				token = strtok(NULL, " ,");
			}
		}
	}

}


void entry_extern(char row[])
{
	char* command;
	char* param;
	command=strtok(row," ");
	param=strtok(NULL," ");
	param=strtok(NULL," ");
	if(param!=NULL)
	{
		error=1;
		fprintf(stderr, "error, too many paramters in func %s\n",command);
	}
}
