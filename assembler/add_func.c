#include "assembler.h"

void add_number(int num, void** array, array_type type, int ARE) {
    void* temp;
    int* current_size;
    size_t item_size;
    
    switch (type) {
        case TYPE_INSTRUCTION:
            current_size = &current_size_instaction_struct;
            item_size = sizeof(binary_code);
            break;
        case TYPE_DIRECTIVE:
            current_size = &current_size_directive_struct;
            item_size = sizeof(binary_directive);
            break;
        default:
            return; 
    }
    
    if (*array == NULL) {
        temp = malloc(item_size);
    } else {
        temp = realloc(*array, (*current_size + 1) * item_size);
    }
    
    if (temp == NULL) {
        free(*array);
        *array = NULL;
        return;
    }
    
    *array = temp;
    
    if (type == TYPE_INSTRUCTION) {
        binary_code* bc_array = (binary_code*)(*array);
        bc_array[*current_size].first = (char)num;
        bc_array[*current_size].second = (char)(ARE << 6);
	IC++;
    } else {
        binary_directive* bd_array = (binary_directive*)(*array);
        bd_array[*current_size].first  = (char)(num >> 2);             
        bd_array[*current_size].second = (char)((num & 0x3) << 6);
    }
    
    (*current_size)++;
}

void add_two_numbers(int num1, int num2, binary_code** array) 
{
    binary_code* temp;
    
    if (array == NULL) {
        return;
    }
    
    temp = (binary_code*) realloc(*array, (current_size_instaction_struct+1) * sizeof(binary_code));
    
    if (temp == NULL) {
        error=1;
        return;
    }
    
    *array = temp;
    
    (*array)[current_size_instaction_struct].first = (char)(((num1 & 0x0F) << 4) | (num2 & 0x0F));
    (*array)[current_size_instaction_struct].second = 0;
    
    current_size_instaction_struct++;
	IC++;
}

void  update_data_symbol_addresses(SEMEL** semels, int* semel_count) 
{
	int i;
    	for (i = 0; i < *semel_count; i++) 
	{
		if(semels[i]->ex_en==0 && semels[i]->addres==3)
		{
			error=1;
			fprintf(stderr,"error,entry label has to be defined %s\n",semels[i]->name);
		}
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

