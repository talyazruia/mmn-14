#include "assembler.h"

void add_number(int num, binary_code** array) 
{
	binary_code* temp;
    /* הקצאה מחדש עם מקום לעוד מבנה אחד */
    	temp = (binary_code*) realloc(*array, size_of_binary_struct+1 * sizeof(binary_code));
    	if (temp == NULL) 
	{
  /* במקרה של כישלון הקצאה */
        free(array);
	*arry=NULL;
        return;
    	}
	*array=temp;
    (*array)[*current_size].first = (char)(num & 0xFF);
    (*array)[*current_size].second = (char)((num >> 8) & 0xFF);
    (*current_size)++;

    	
}
void add_two_numbers(int num1, int num2, binary_code** array) 
{
    	binary_code* temp;

    	temp = (binary_code*) realloc(array, size_of_binary_struct+1 * sizeof(binary_code));
    	if (temp == NULL) 
	{
        	free(array);
		*array =NULL;
        	return;
    	}

    	 *array = temp;
    (*array)[*current_size].first = (char)(((num1 & 0x0F) << 4) | (num2 & 0x0F));
    (*array)[*current_size].second = 0;
    (*current_size)++;

}
void  update_data_symbol_addresses(SEMEL** semels, int semel_count) 
{
	int i;
    	for (i = 0; i < semel_count; i++) 
	{
        	if (semels[i]->type == 1) 
		{
            		semels[i]->addres += IC;
        	}
    	}
}


void fix_commas_in_place(char *line) {
    char temp[MAX_LEN_OF_ROW + 1]; /* שורת פלט זמנית, כולל '\0' */
    int i = 0;  /* אינדקס לקלט */
    int j = 0;  /* אינדקס לפלט */
    int in_operand = 0;

    error = 0;

    /* דילוג על רווחים בתחילת השורה */
    while (isspace((unsigned char)line[i])) {
        i++;
    }

    /* העתקת שם הפקודה */
    while (line[i] && !isspace((unsigned char)line[i]) && line[i] != ',') {
        if (j < MAX_LEN_OF_ROW) {
            temp[j++] = line[i++];
        } else {
            break;
        }
    }

    /* הוספת רווח אחרי שם הפקודה */
    if (j < MAX_LEN_OF_ROW) {
        temp[j++] = ' ';
    }

    /* דילוג על רווחים אחרי שם הפקודה */
    while (isspace((unsigned char)line[i])) {
        i++;
    }

    while (line[i]) {
        /* פסיקים */
        if (line[i] == ',') {
            if (!in_operand) {
                error = 1;
                fprintf(stderr, "Error: Unexpected comma (e.g., at start or double commas).\n");
                while (line[i] == ',') {
                    i++;
                }
                continue;
            }

            if (j + 2 < MAX_LEN_OF_ROW) {
                temp[j++] = ',';
                temp[j++] = ' ';
            }
            in_operand = 0;
            i++;

            while (isspace((unsigned char)line[i])) {
                i++;
            }
            continue;
        }

        if (isspace((unsigned char)line[i])) {
            i++;
            continue;
        }

        if (!in_operand) {
            in_operand = 1;
        }

        /* העתקת תו של אופראנד */
        while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i])) {
            if (j < MAX_LEN_OF_ROW) {
                temp[j++] = line[i++];
            } else {
                break;
            }
        }

        while (isspace((unsigned char)line[i])) {
            i++;
        }
    }

    /* הסרה של פסיק אחרון אם קיים */
    if (j >= 2 && temp[j - 2] == ',' && temp[j - 1] == ' ') {
        j -= 2;
        error = 1;
        fprintf(stderr, "Error: Comma at end of line.\n");
    }

    temp[j] = '\0';

    /* העתקה חזרה ל־line */
    strncpy(line, temp, MAX_LEN_OF_ROW);
    line[MAX_LEN_OF_ROW] = '\0'; /* וידוא סיום תקין */
}

