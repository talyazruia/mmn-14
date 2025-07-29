#include "assembler.h"

void add_number(int num, void** array, array_type type,int ARE) {
	fprintf(stderr, "=== ENTERING add_number ===\n");
    fprintf(stderr, "num=%d, array=%p, type=%d, ARE=%d\n", num, (void*)array, type, ARE);
    void* temp;
    int* current_size;
    size_t item_size;

	fprintf(stderr, "About to switch on type\n");
    switch (type) {
        case TYPE_INSTRUCTION:
	fprintf(stderr, "TYPE_INSTRUCTION case\n");
            current_size = &current_size_instaction_struct;
            item_size = sizeof(binary_code);
		fprintf(stderr, "current_size_instaction_struct=%d, item_size=%zu\n", 
                    current_size_instaction_struct, item_size);
            break;
        case TYPE_DIRECTIVE:
	fprintf(stderr, "TYPE_DIRECTIVE case\n");
            current_size = &current_size_directive_struct;
            item_size = sizeof(binary_directive);
	 fprintf(stderr, "current_size_directive_struct=%d, item_size=%zu\n", 
                    current_size_directive_struct, item_size);
            break;
        default:
		fprintf(stderr, "ERROR: Unknown type %d\n", type);
            return; 
    }
	fprintf(stderr, "About to check if array is NULL\n");
	if (*array == NULL) {
	 fprintf(stderr, "Array is NULL, allocating new memory\n");
        temp = malloc(item_size);
    } else {
	fprintf(stderr, "Array exists, reallocating. current_size=%d\n", *current_size);
        temp = realloc(*array, (*current_size + 1) * item_size);
    }
	 fprintf(stderr, "Memory operation result: temp=%p\n", temp);
    
    if (temp == NULL) {
	fprintf(stderr, "ERROR: Memory allocation failed!\n");
    
        free(*array);
        *array = NULL;
        return;
    }
     fprintf(stderr, "About to assign temp to array\n");
    *array = temp;
fprintf(stderr, "Assignment successful\n");
    if (type == TYPE_INSTRUCTION) {
    	binary_code* bc_array = (binary_code*)(*array);
	bc_array[*current_size].first = (char)num;
	bc_array[*current_size].second = (char)(ARE << 6);

   } 
else {
        binary_directive* bd_array = (binary_directive*)(*array);
	bd_array[*current_size].first  = (char)(num >> 2);             
	bd_array[*current_size].second = (char)((num & 0x3) << 6);
    }
    
    (*current_size)++;
}

void add_two_numbers(int num1, int num2, binary_code** array) 
{
	fprintf(stderr, "=== ENTERING add_two_numbers ===\n");
    fprintf(stderr, "num1=%d, num2=%d, array=%p\n", num1, num2, (void*)array);
    	binary_code* temp;
	
	if (array == NULL) {
        fprintf(stderr, "ERROR: array parameter is NULL\n");
        return;
    }
    
    fprintf(stderr, "*array = %p\n", (void*)*array);
    fprintf(stderr, "current_size_instaction_struct = %d\n", current_size_instaction_struct);

	fprintf(stderr, "About to realloc\n");
    	temp = (binary_code*) realloc(*array, (current_size_instaction_struct+1) * sizeof(binary_code));
	fprintf(stderr, "realloc result: temp = %p\n", (void*)temp);
    	if (temp == NULL) 
	{
        	fprintf(stderr, "Error: realloc failed\n");
        	error=1;
        	return;
    	}
	fprintf(stderr, "About to assign temp to *array\n");
    	*array = temp;
	fprintf(stderr, "Assignment successful\n");
    
    fprintf(stderr, "About to set values at index %d\n", current_size_instaction_struct);
    	(*array)[current_size_instaction_struct].first = (char)(((num1 & 0x0F) << 4) | (num2 & 0x0F));
    	(*array)[current_size_instaction_struct].second = 0;
	fprintf(stderr, "Values set successfully\n");
    
    fprintf(stderr, "About to increment current_size_instaction_struct\n");
    	current_size_instaction_struct++;
	fprintf(stderr, "Increment successful, new size = %d\n", current_size_instaction_struct);
    
    fprintf(stderr, "=== EXITING add_two_numbers ===\n");

}

void  update_data_symbol_addresses(SEMEL** semels, int semel_count) 
{
	int i;
    	for (i = 0; i < semel_count; i++) 
	{
		if(semels[i]->ex_en==0&&semels[i]->addres==0)
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

