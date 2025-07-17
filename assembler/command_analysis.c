#include "assembler.h"

int is_valid_integer(const char *str, int *value) {
    char *endptr;
    long val;

    if (str == NULL) return 0;

    /* דילוג על רווחים בתחילה */
    while (isspace((unsigned char)*str)) str++;

    val = strtol(str, &endptr, 10); // המרה לבסיס 10

    if (str == endptr) return 0; // לא התחיל במספר

    /* דילוג על רווחים אחרי המספר */
    while (isspace((unsigned char)*endptr)) endptr++;

    if (*endptr != '\0') return 0; // יש תווים נוספים – לא תקני

    if (val < INT_MIN || val > INT_MAX) return 0; // חורג מטווח int

    *value = (int)val;
    return 1;
}

void to_binary(int opcode, int op1, int op2,binary_code* array) {
    (void)opcode;
    (void)op1;
    (void)op2;
	binary_code* temp;
	int size=1;
if(array=NULL)
{
    array = ((binary_code*)malloc(sizeof(binary_code)));
    if (array == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        return 1;
    }
}
else{
    /* הוספת איבר ראשון */

    /* הגדלת המערך בכל הוספה נוספת */
    temp =( (binary_code*)realloc(array, (size + 1) * sizeof(binary_code)));
    if (temp == NULL) {
        fprintf(stderr, "Error: realloc failed.\n");
        free(array);
        return 1;
    }
    array = temp;
	size++;
}

    /* הוספת איבר נוסף */
    

    /* הדפסה לדוגמה */
   
    /* ניקוי זיכרון */
    /*free(array);*/
    return 0;
}



int isHashNumber( char* str) {
	if (str == NULL || str[0] != '#') return 0; 

	str++; 

	if (*str == '\0') return 0; 

	
		if (!isdigit((unsigned char)*str)) 
			return 0; 
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

int* valid_matrix(char* str, SEMEL** SEMELS,int semel_count)
{
	char array_name[31];
	char r1[31];
	char r2[31];
	int i = 0;
	int*result = NULL;

	if (sscanf(str, "%30[^[][%30[^]]][%30[^]]]", array_name, r1, r2) == 3) {
		while (i < semel_count)
		{
			if (strcmp(SEMELS[i]->name, array_name) == 0) {
				if (reg(r1) == 0) {
					if (reg(r2) == 0)
					{
						result[0]= i;
						result[1] = r1[1];
				        result[2] = r2[1];
						return result;
						break;
					}
					else
						fprintf(stderr, "Invalid format for matrix access.\n");
				}
				else
					fprintf(stderr, "Invalid format for matrix access.\n");
			}
			else
				fprintf(stderr, "Invalid format for matrix access.\n");

			i++;
		}
	}
	else {
		fprintf(stderr,"Invalid format for matrix access.\n");
	}

	return NULL;
}
int valid_SEMEL(char* str, SEMEL** SEMELS, int semel_count)
{
	int i = 0;
	while (i < semel_count)
	{
		if (strcmp(SEMELS[i]->name, str) == 0)
			return i;
		i++;
	}
	return -1;

}


int add(char row[],SEMEL** SEMELS, int semel_count, command cmd[], binary_code* array)/*func* מבנה שמקבל 2 צארים והופך את לבינארי*/
{
	char* command;
	char* op1;
	char* op2;
	char* op3;
	int opcode=0;
	int i = 0;
	int op1_val;
	int op2_val;
	int op1_type;
	int* op1_mat;
	int* op2_mat;
	int op2_type;
	int flag1;
	int flag2;
	command = strtok(row, " ");/*כאן להוסיף משתנה שיכיל את ה opcode*/
	op1 = strtok(NULL, ",");
	op2 = strtok(NULL, " ");
	op3 = strtok(NULL, " ");
	if(op3!=NULL)
	{
		fprintf(stderr, "Error: too many operands in command %s\n", command);
	}
	while (i < 16){
		if (strcmp(cmd[i].name, command) == 0){
			opcode = cmd[i].op;
			break;
		}
		i++;
	}

	if (op1 == NULL)
	{
		fprintf(stderr, "Error: Missing operand in command %s\n", command);
		error = 1;
		return 0;
	}
	if (op2 == NULL)
	{
		fprintf(stderr, "Error: Missing second operand in command %s\n", command);
		error = 1;
           return 0;
	
	if (opcode>=0&&opcode<=3&&isHashNumber(op1)) {
		op1_val = atoi(op1 + 1); 
		op1_type = 0;
		flag1 =1;
	}
	else if(opcode >= 0 && opcode <= 3&&reg(op1))
	{
		op1_val = atoi(op1 + 1);
		op1_type = 3;
		flag1 =1;
	}
	else if(opcode >= 0 && opcode <= 4 ){
		op1_mat=(valid_matrix(op1, SEMELS, semel_count));
			if (op1_mat != NULL) {
				op1_type = 2;
				flag1 = 2;
			}

	}
	else if (opcode >= 0 && opcode <= 3&&(valid_SEMEL(op1, SEMELS, semel_count) != -1))
	{
		op1_val = SEMELS[i]->addres;
		op1_type = 1;
		flag1=1;
	}
	if ((opcode != 14) && (opcode != 15) && isHashNumber(op2) && ((opcode == 1) || (opcode == 13))) {
		op2_val = atoi(op2 + 1); 
		op2_type = 0;
		if (flag1 == 1)
			flag2 = 0;
		else
			flag2 = 1;
	}
	else if ((opcode != 14) && (opcode != 15)&&reg(op2))
	{
		op2_val = atoi(op2 + 1);
		op2_type = 3;
		if (op1_type == 3)
			flag2 = 0;
		else
			flag2 = 1;

	}
	else if ((opcode != 14) && (opcode != 15)&&(valid_SEMEL(op2, SEMELS, semel_count) != -1))
	{
		op2_val = SEMELS[i]->addres;
		op2_type = 1;
		flag2=1;
	}

	else if((opcode != 14) && (opcode != 15))
	{
		op2_mat = (valid_matrix(op2, SEMELS, semel_count));
			if (op2_mat != NULL) {
				op2_type = 2;
				flag2 = 2;

			}

	}
	to_binary(opcode, op1_type, op2_type, array);
	if (flag1 + flag2 == 1)
	{
			add_two_numbers(op1_val,op2_val, array);
	}
		else if (flag1==2)
		{
			add_number(SEMELS[op1_mat[0]]->addres, array);
			add_two_numbers(op1_mat[1],op1_mat[2] ,array);
		}
		else
			add_number(op1_val, array);
		if (flag2 == 2)
		{

			add_number(SEMELS[op2_mat[0]]->addres, array);
			add_two_numbers(op2_mat[1], op2_mat[2] ,array);
		}
		else if(flag1+flag2!=1){
			add_number(op2_val, array);
		}
}
return 0;
}





	


	

int mov(char row[],SEMEL** SEMELS)
{
fprintf(stderr,"hhhhhh\n");
return 0;
}

int cmp(char row[],SEMEL** SEMELS)
{return 0;
}

int sub(char row[],SEMEL** SEMELS)
{return 0;
}

int not_func(char row[],SEMEL** SEMELS)
{return 0;
}

int clr(char row[],SEMEL** SEMELS)
{return 0;
}

int lea(char row[],SEMEL** SEMELS)
{return 0;
}

int inc(char row[],SEMEL** SEMELS)
{return 0;
}

int dec(char row[],SEMEL** SEMELS)
{return 0;
}

int jmp(char row[],SEMEL** SEMELS)
{return 0;
}

int bne(char row[],SEMEL** SEMELS)
{return 0;
}

int red(char row[],SEMEL** SEMELS)
{return 0;
}

int prn(char row[],SEMEL** SEMELS)
{return 0;
}

int jsr(char row[],SEMEL** SEMELS)
{return 0;
}

int rts(char row[],SEMEL** SEMELS)
{return 0;
}

int stop(char row[],SEMEL** SEMELS)
{return 0;
}

int data(char row[],SEMEL** SEMELS)
{
void data(char row[], binary_code* array) {
    char* command;
    char* op1;
    command = strtok(row, " ");
    op1 = strtok(NULL, ",");

    if (op1 == NULL) {
        fprintf(stderr, "Error: Missing operand in command %s\n", command);
        error = 1;
        return;
    }

    while (op1 != NULL) {
        int val;

        if (is_valid_integer(op1, &val)) {
            add_number(val, array);
        } else {
            fprintf(stderr, "Error: Invalid operand: '%s'\n", op1);
            error = 1;
        }

        op1 = strtok(NULL, ",");
    }
}

return 0;
}

int string(char row[],SEMEL** SEMELS)
{return 0;
}

int mat(char row[],SEMEL** SEMELS)
{return 0;
}

int entry(char row[],SEMEL** SEMELS)
{ return 0;
}

int extern_func(char row[],SEMEL** SEMELS)
{
return 0;
}
