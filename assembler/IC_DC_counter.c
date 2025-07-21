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
    	command = strtok(row, " \t");
    	op1 = strtok(NULL, " \t\n\r");
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

    command = strtok(row, " \t");
    op1 = strtok(NULL, ",");
    op2 = strtok(NULL, ",\n\r");

    if (op1 == NULL || op2 == NULL) 
    {
        error = 1;
        fprintf(stderr, "error, missing operand for 2-arg command\n");
        IC += 1;
        return;
    }
	
   /* הסרת רווחים מיותרים*/
    while (*op1 == ' ' || *op1 == '\t') op1++;
    while (*op2 == ' ' || *op2 == '\t') op2++;

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
    	command = strtok(row, " \t");
    	op1 = strtok(NULL, ",\n\r");
    	while (op1 != NULL) 
	{
		while (*op1 == ' ' || *op1 == '\t') op1++;/*הסרת רווחים מיותרים */
		if(*op1!='\0')
        		DC++;
        	op1 = strtok(NULL, " ,");
    	}
}

void dc_count_string(char* row) 
{
     	 char* command;
    	char* op1;
    	char* start_string;
    	char* end_string;
    
    	command = strtok(row, " \t");
	if(command==NULL)
		return;
    	op1 = strtok(NULL, "\n\r");  /* כל השאר של השורה*/
    
    	if (op1 == NULL) return;
    
   /* הסרת רווחים מיותרים*/
    	while (*op1 == ' ' || *op1 == '\t') op1++;
    
  /* מציאת המחרוזת בין הגרשיים*/
    	start_string = strchr(op1, '"');
    	if (start_string != NULL) 
    	{
        	end_string = strchr(start_string + 1, '"');
        	if (end_string != NULL) 
        	{
            		DC += (end_string - start_string - 1) + 1;  /* אורך המחרוזת + '\0'*/
        	}
    	}
}


void dc_count_mat(char* row) 
{
	char rows;
	char col;
    	char* token1;
	char* token2;
	int i=0;
	char row_str[225];
	char col_str[225];
	char* data;
	int data_count=0;

	token1 = strchr(row, '[');
	if(token1==NULL) 
		return;
    	token2 = strchr(token1, ']');
	if(token2==NULL) 
		return;
	token1++;
	while(token1<token2 && *token1>= '0' && *token1 <= '9')
	{
		row_str[i++]= *token1;
		token1++;
	}
	row_str[i]='\0';
	rows= atoi(row_str);
	token1 = strchr(token2, '[');
	if(token1==NULL) 
		return;
	token2 = strchr(token1, ']');
	if(token2==NULL) 
		return;
	i=0;
	token1++;
	while(token1<token2 && *token1>= '0' && *token1 <= '9')
	{
		col_str[i++]= *token1;
		token1++;
	}
	col_str[i]='\0';
	col= atoi(row_str);
	data=token2+1;
	/*while(*data == ' ' || *data == '\t')
		data++;
	if(*data == '\0' || *data == '\n')
	{
		DC+=rows*col;
	}
	else
	{
    		while(*data !='\0' && *data != '\n')
		{
			while(*data ==' ' || *data == '\t')
				data++;
			if(*data== '\0' || *data == '\n')
				break;*/
	data = strtok(data, " , ");
	if(data==NULL)
	{
		DC+=col*rows;
	}
	else
	{
		while(data!=NULL)
		{
			data = strtok(NULL, " , ");
			DC++;
		}
	}	
}











