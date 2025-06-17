#include "assembler.h"

void row_analysis(FILE *      )
{
	char row[MAX_LEN_OF_ROW+2];
	char label[31];
	char *token;
	size_t len;
	size_t i;
	int valid_label=0;
	SEMEL *s;
	
	while (fgets(row, sizeof(row), f))
	{
		if (strchr(row, '\n') == NULL) 
		{
            		fprintf(stderr,"Error: line too long (more than %d characters)\n", MAX_LEN_OF_ROW);
			error=1;
			clear_row_arry();
			continue;
		}
		if(row[0]==';')
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
			token[len-1]='\0';
			strcpy(label,token);
			for(i=0; i<strlen(label); i++)
			{
				if(!isalpha(label[i]))
				{
					valid_label=1;
					error=1;
					fprintf(stderr, "error, label must contain only: %s\n", label);
					break;
				} 
			}
			if(valid_label==0)
			{
				for(i=0; i<16; i++)
				{
					if(strcmp(label, cmd[i].name)==0);
					{
						valid_label=1;
						error=1;
						fprintf(stderr,"error, label cannot be the name of a command: %s\n", label);
						break;
					}
				}
			}
			if(valid_label==0)
			{
				s== (SEMEL *)calloc(1, sizeof(SEMEL));
				if (s == NULL) 
				{
    					fprintf(stderr, "Memory allocation failed\n");
					error=1;
				}
				else 
				{
					s.name = (char *)malloc(strlen(label) + 1);
					if (s.name == NULL) 
					{
    						fprintf(stderr, "Memory allocation failed for name\n");
						error=1;
					}
					strcmp(s.name, label);
				
}
				
		}
''            		 /*זה כנראה תווית צריך לבדוק שהשם שלה לא כמו של הפקודות*/
								
        	}
}
}


	


void clear_row_arry() 
{
	char c;
    	while ((c = getchar()) != '\n' && c != EOF);
}
