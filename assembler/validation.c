#include "validation.h"

void  update_data_symbol_addresses(SEMEL** semels, int* semel_count) 
{
	int i;                                      /* Loop counter for symbol table iteration */
    	for (i = 0; i < *semel_count; i++) 
	{	/* Check if entry label is undefined (address == 3 indicates undefined entry) */
		if(semels[i]->ex_en==0 && semels[i]->addres==3)
		{
			error=1;                    /* Set global error flag */
			fprintf(stderr,"error in line :%d ,entry label has to be defined %s\n",sum_of_row,semels[i]->name);
		}
        	if (semels[i]->type == 1)           /* Type 1 indicates data symbol */
            		semels[i]->addres += IC;    /* Adjust data address by adding final IC value */
    	}
}
/*int check_commas_errors(const char *line)
{
    int i = 0;
    int in_operand = 0;
    int last_token_was_operand = 0;
    int error_found = 0;

    /* Skip leading whitespace */
   /* while (isspace((unsigned char)line[i]))
        i++;

    /* Skip command name */
  /*  while (line[i] && !isspace((unsigned char)line[i]) && line[i] != ',')
        i++;

    /* Skip spaces after command */
   /* while (isspace((unsigned char)line[i]))
        i++;

   /* while (line[i]) 
    {
        /* Handle comma */
      /*  if (line[i] == ',') 
        {
            int comma_count = 0;
            while (line[i] == ',') 
            {
                comma_count++;
                i++;
            }

            if (!last_token_was_operand) 
            {
                fprintf(stderr, "error in line:%d: Unexpected comma.\n", sum_of_row);
                error_found = 1;
            }
            else if (comma_count > 1) 
            {
                fprintf(stderr, "error in line:%d: Multiple consecutive commas.\n", sum_of_row);
                error_found = 1;
            }

            in_operand = 0;
            last_token_was_operand = 0;

            while (isspace((unsigned char)line[i]))
                i++;

            continue;
        }

        
        if (isspace((unsigned char)line[i])) 
        {
            i++;
            continue;
        }

        
        if (last_token_was_operand) 
        {
            fprintf(stderr, "error in line:%d: Missing comma between operands.\n", sum_of_row);
            error_found = 1;
        }

     
        last_token_was_operand = 1;
        in_operand = 1;

      
        while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i]))
            i++;

        while (isspace((unsigned char)line[i]))
            i++;
    }

    
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        len--;

    if (len >= 1 && line[len - 1] == ',') 
    {
        fprintf(stderr, "error in line:%d: Comma at end of line.\n", sum_of_row);
        error_found = 1;
    }

    return error_found ? 0 : 1;
}
int check_commas_errors(const char *line)
{
    int i = 0;
    int in_operand = 0;
    int last_token_was_operand = 0;
    int error_found = 0;


    while (isspace((unsigned char)line[i]))
        i++;

    
    while (line[i] && !isspace((unsigned char)line[i]) && line[i] != ',')
        i++;

   
    while (isspace((unsigned char)line[i]))
        i++;

    while (line[i]) 
    {
        
        if (line[i] == ',') 
        {
            int comma_count = 0;
            while (line[i] == ',') 
            {
                comma_count++;
                i++;
            }

            if (!last_token_was_operand) 
            {
                fprintf(stderr, "error in line:%d: Unexpected comma.\n", sum_of_row);
                error_found = 1;
            }
            else if (comma_count > 1) 
            {
                fprintf(stderr, "error in line:%d: Multiple consecutive commas.\n", sum_of_row);
                error_found = 1;
            }

            in_operand = 0;
            last_token_was_operand = 0;

            while (isspace((unsigned char)line[i]))
                i++;

            continue;
        }

       
        if (isspace((unsigned char)line[i])) 
        {
            i++;
            continue;
        }

       
        if (last_token_was_operand) 
        {
            fprintf(stderr, "error in line:%d: Missing comma between operands.\n", sum_of_row);
            error_found = 1;
        }

       
        last_token_was_operand = 1;
        in_operand = 1;

       
        while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i]))
            i++;

        while (isspace((unsigned char)line[i]))
            i++;
    }

    
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        len--;

    if (len >= 1 && line[len - 1] == ',') 
    {
        fprintf(stderr, "error in line:%d: Comma at end of line.\n", sum_of_row);
        error_found = 1;
    }

    return error_found ? 0 : 1;
}
*/
int check_commas(const char *line, int is_mat) {
    int i = 0;
    int in_operand = 0;
    int last_token_was_operand = 0;
    int error_found = 0;
    int after_closing_bracket = 0;
    int trailing_comma_detected = 0;

    /* דילוג על רווחים בתחילת השורה */
    while (isspace((unsigned char)line[i])) {
        i++;
    }

    /* דילוג על שם הפקודה */
    while (line[i] && !isspace((unsigned char)line[i]) && line[i] != ',') {
        i++;
    }

    /* דילוג על רווחים אחרי שם הפקודה */
    while (isspace((unsigned char)line[i])) {
        i++;
    }

    while (line[i]) {
        /* פסיקים */
        if (line[i] == ',') {
            int comma_count = 0;
            int comma_pos = i;

            while (line[i] == ',') {
                comma_count++;
                i++;
            }

            /* שמירה אם זה פסיק אחרון אמיתי בשורה */
            int j = i;
            while (isspace((unsigned char)line[j])) {
                j++;
            }
            if (line[j] == '\0') {
                trailing_comma_detected = 1;
                break;  
            }

            if (!last_token_was_operand) {
                fprintf(stderr, "error in line:%d: Unexpected comma.\n", sum_of_row);
                error_found = 1;
                error = 1;
            } else if (comma_count > 1) {
                fprintf(stderr, "error in line:%d: Multiple consecutive commas.\n", sum_of_row);
                error_found = 1;
                error = 1;
            }

            in_operand = 0;
            last_token_was_operand = 0;

            while (isspace((unsigned char)line[i])) {
                i++;
            }

            continue;
        }

        if (isspace((unsigned char)line[i])) {
            i++;
            continue;
        }

        /* סימון חוסר פסיק בין אופרנדים */
        if (last_token_was_operand) {
            if (!(is_mat && after_closing_bracket)) {
                fprintf(stderr, "error in line:%d: Missing comma between operands.\n", sum_of_row);
                error_found = 1;
                error = 1;
            }
        }

        in_operand = 1;
        last_token_was_operand = 1;

        if (line[i] == ']') {
            after_closing_bracket = 1;
        } else if (!isspace((unsigned char)line[i]) && line[i] != ',') {
            after_closing_bracket = 0;
        }

        while (line[i] && line[i] != ',' && !isspace((unsigned char)line[i])) {
            i++;
        }

        while (isspace((unsigned char)line[i])) {
            i++;
        }
    }

    /* בדיקה אחת בלבד לפסיק בסוף שורה */
    if (trailing_comma_detected) {
        fprintf(stderr, "error in line:%d: Comma at end of line.\n", sum_of_row);
        error_found = 1;
        error = 1;
    }

    return error_found ? 0 : 1;
}

