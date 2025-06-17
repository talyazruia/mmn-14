#include "assembler.h"

void row_analysis(FILE * f, int error)
{
	char row[MAX_LEN_OF_ROW+2];
	char c;
	char label[31];
	char *token;
	size_t len;
	
	while (fgets(row, sizeof(row), f))
	{
		if (strchr(row, '\n') == NULL) 
		{
            		fprintf(stderr,"Error: line too long (more than %d characters)\n", MAX_LEN_OF_ROW);
			error=1;
			clear_row_arry();
			continue;
		}
		c=row[0];
		if(c==';')
		{
			continue;
		}
		token=strtok(row, " \t\n\r");
		if(token == NULL)
		{
			continue;
		}
	/*בדיקת חריגת אורך*/
        	if (strlen(token) >= 31)
        	{
            		fprintf(stderr, "Error: label too long (max 30 characters): %s\n", token);
            		error = 1;
            		continue;
        	}
		 
        	len = strlen(label);
        	if (len > 0 && label[len - 1] == ':')
        	{
            		 /*זה כנראה תווית צריך לבדוק שהשם שלה לא כמו של הפקודות*/
								
        	}
}
}


	


void clear_row_arry() 
{
	char c;
    	while ((c = getchar()) != '\n' && c != EOF);
}
