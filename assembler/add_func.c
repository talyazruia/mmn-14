#include "assembler.h"

void add_number(int num, binary_code* array) 
{
	binary_code* temp;
    /* הקצאה מחדש עם מקום לעוד מבנה אחד */
    	temp = (binary_code*) realloc(array, (IC + 1) * sizeof(binary_code));
    	if (temp == NULL) 
	{
  /* במקרה של כישלון הקצאה */
        free(array);
        return;
    	}

    /* רק 8 הסיביות הראשונות - נשמרות ב-first */
    	temp[IC].first = (char)(num & 0xFF);

    /* שאר הסיביות (אם בכלל) נשמרות ב-second */
    	temp[IC].second = (char)((num >> 8) & 0xFF);

    /* עדכון IC */
    	IC++;

    	
}
void add_two_numbers(int num1, int num2, binary_code* array) 
{
    	binary_code* temp;

    	temp = (binary_code*) realloc(array, (IC + 1) * sizeof(binary_code));
    	if (temp == NULL) 
	{
        	free(array);
        	return;
    	}

    	temp[IC].first = (char)(((num1 & 0x0F) << 4) | (num2 & 0x0F));
    	temp[IC].second = 0;

    	IC++;

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

/*void is_valid_operands_line_with_errors( char *line) 
{
    	 char *ptr = line;
    	int in_operand = 0;
    	int expect_operand = 0;
    	int operand_count = 0;*/

    /* דילוג על רווחים בתחילת השורה */
    	/*while (isspace((unsigned char)*ptr)) ptr++;*/

    /* דילוג על שם הפקודה (המילה הראשונה) */
	/*while (*ptr && !isspace((unsigned char)*ptr) && *ptr != ',') ptr++;*/

    /* דילוג על רווחים אחרי שם הפקודה */
    	/*while (isspace((unsigned char)*ptr)) ptr++;
	while (*ptr) 
	{
        	if (*ptr == ',') 
		{
            		if (expect_operand || operand_count == 0) 	
			{
                		if (operand_count == 0) 
				{
					error=1;
                    			fprintf(stderr,"Error: Comma immediately after command name is not allowed.\n");
                		} 	
				else 
				{
					error=1;
                    			fprintf(stderr,"Error: Multiple commas or comma without following operand.\n");
                		}
                		return 0;
            		}
            		expect_operand = 1;
            		in_operand = 0;
            		ptr++;*/
            /* דילוג על רווחים אחרי פסיק */
            		/*while (isspace((unsigned char)*ptr)) ptr++;
            		if (*ptr == '\0') 
			{
				error=1;
                		fprintf(stderr, "Error: Comma at end of line with no operand after it.\n");
                		return 0;
            		}
            		continue;
        	}*/

        	/*if (!isspace((unsigned char)*ptr)) 
		{
            		if (!in_operand) 
			{
                		in_operand = 1;
                		expect_operand = 0;
                		operand_count++;
            		}
        	}*/

        	/*ptr++;
    	}

    	if (expect_operand) 
	{
		error=1;
        	fprintf(stderr, "Error: Line ends with a comma but no operand follows.\n");
        	return 0;
    	}*/

    /* בדיקה שאין תווים לא לבנים אחרי האופראנד האחרון */
    	/*while (isspace((unsigned char)*ptr)) ptr++;
    	if (*ptr != '\0') 
	{
		error=1;
        	fprintf(stderr, "Error: Unexpected character after last operand.\n");
        	return 0;
   	}*/

    	/*return 1;
}*/
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

