#include "assembler.h"

int has_two_square_bracket_pairs( char* str) 
{
    	int i = 0;
   	int bracket_count = 0;
    	int inside = 0; 
	while (str[i] != '\0') 
	{
        	if (str[i] == '[') 
		{
            		if (inside) return 0; 
            		inside = 1;
            		bracket_count++;
        	}
        	else if (str[i] == ']') 
		{
            		if (!inside) return 0; 
            		inside = 0;
        	}
        	i++;
    	}
	return (bracket_count == 2) && (inside == 0);
}

void ic_count_1_arg(char* row) 
{
	char* command;
    	char* op1;
    	command = strtok(row, " ");
    	op1 = strtok(NULL, " ");
    	if (op1 == NULL) 
	{
        	error = 1;
		fprintf(stderr, "error, missing operand for 1-arg command\n");
        	IC += 1;
		return;
    	}

    	if (has_two_square_bracket_pairs(op1))
        	IC += 3;
    	else
        	IC += 2;
}
void ic_count_2_arg(char* row) 
{
    fix_commas_in_place(row);

    char* command;
    char* op1;
    char* op2;

    int ic_extra = 0;

    command = strtok(row, " ");
    op1 = strtok(NULL, ",");
    op2 = strtok(NULL, ",");

    if (op1 == NULL || op2 == NULL) 
    {
        error = 1;
        fprintf(stderr, "error, missing operand for 2-arg command\n");
        IC += 1;
        return;
    }

    IC += 1;  /* עבור הפקודה עצמה (opcode + addressing modes) */

    /* אופראנד ראשון */
    if (has_two_square_bracket_pairs(op1))
        ic_extra += 2;
    else
        ic_extra += 1;

    /* אופראנד שני */
    if (has_two_square_bracket_pairs(op2))
        ic_extra += 2;
    else
        ic_extra += 1;

    /* אם שניהם רגיסטרים → חסכון של 1 */
    if (reg(op1) && reg(op2))
        ic_extra -= 1;

    IC += ic_extra;
}

/*void ic_count_2_arg(char* row) 
{
	fix_commas_in_place(row);
	const char* command;
    	const char* op1;
    	const char* op2;
    	command = strtok(row, " ");
    	op1 = strtok(NULL, ",");
    	op2 = strtok(NULL, ",");		
    	if (op1 == NULL || op2==NULL) 
	{
        	error = 1;
		fprintf(stderr, "error, missing operand for 2-arg command\n");
        	IC += 1;
		return;
    	}
    	if (has_two_square_bracket_pairs(op1))
        	IC += 3;
    	else
        	IC += 2;
    	if (has_two_square_bracket_pairs(op2))
        	IC += 2;
    	else
        	IC += 1;
    	if (isHashNumber(op1) && isHashNumber(op2))
        	IC--;
    	if (reg(op1) && reg(op2))
        	IC--;
}*/

void dc_count_data(char* row) 
{
	fix_commas_in_place(row);
    	char* command;
    	char* op1;
    	command = strtok(row, " ");
    	op1 = strtok(NULL, ",");
    	while (op1 != NULL) 
	{
        	DC++;
        	op1 = strtok(NULL, " ,");
    	}
}

void dc_count_string(char* row) 
{
     	char* command;
     	char* op1;
    	command = strtok(row, " ");
    	op1 = strtok(NULL, " ");
    	DC += strlen(op1) + 1;
}

void dc_count_mat(char* row) 
{
	char row1;
	char col;
    	char* command;
    	command = strchr(row, ']');
	row1=*(command - 1);
    	command = strchr(command + 1, ']');
	col = *(command - 1);

    	command++;
    	command = strtok(command, ",");
	if(command==NULL)
	{
		DC+=(int)row1*(int)col;
	}
	else{
    	while (command != NULL) 
	{
        	DC++;
        	command = strtok(NULL, " ,");
    	}
	}
}

