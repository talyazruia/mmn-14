#include "assembler.h"

void clear_row_arry();
SEMEL** add_SEMEL(char* label , int type ,int addres); 

void row_analysis(FILE * f , int macro_count, macro** macros, command cmd[],command1 cmd1[])
{
	char row[MAX_LEN_OF_ROW+2];
	char label[31];
	char *token;
	size_t len;
	size_t i;
	int valid_label;
	int j;
	int type;
	int addres;
	char *new_row;
	 char *colon;
	int is_valid_func;
	SEMEL** SEMELS;
	int found_command;
	

	SEMELS=NULL;
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
		valid_label=0;
		is_valid_func=0;
        	
        	len = strlen(token);
        	if (len > 0 && token[len - 1] == ':')
        	{
			if (strlen(token) >= 31)
	        	{
            		fprintf(stderr, "Error: label too long (max 30 characters): %s\n", token);
            		error = 1;
            		continue;
        		}
			token[len-1]='\0';
			strcpy(label,token);
			for(i=0; i<strlen(label); i++)
			{
				if(!isalpha(label[i]))
				{
					valid_label=1;
					error=1;
					fprintf(stderr, "error, label must contain only letters \n");
					break;
				} 
			}
			if(valid_label==0)
			{
				 for (j = 0; j < macro_count; j++) {
					if (strcmp(label, macros[j]->name) == 0) {
						fprintf(stderr, "error, label cant be macro name: %s\n", label);
						error=1;
						valid_label=1;
            					break;
					}
				}
			}
			if(valid_label==0)
			{
				for(i=0; i<16; i++)
				{
					if(strcmp(label, cmd[i].name)==0)
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
				
				token=strtok(NULL, " \t\n\r");
				if(token!=NULL)
				{
					found_command=0;
					type=0;
					addres=IC;
					for(i=0; i<16; i++)
					{
						if(strcmp(token, cmd[i].name)==0)
						{
							type=0;
							addres=IC;
							found_command=1;
							break;
						}
					}
				 	for (j = 0; j < 5; j++) 
					{
						if (strcmp(token, cmd1[j].name) == 0) 
						{
							type=1;
							addres=DC;
							found_command=1;
							break;	
						}
					}
					if(found_command)
					{
					SEMELS=add_SEMEL(label,type,addres);

					if(SEMELS!=NULL)
					{
				 		colon = strchr(row, ':');
						if (colon != NULL) 
      							new_row = colon + 1;
						for(i=0; i<16; i++)
						{
							if(strcmp(token, cmd[i].name)==0)
							{
								cmd[i].func(new_row,SEMELS);
								break;
							}
						}
				 		for (j = 0; j < 5; j++) 
						{
						
							if (strcmp(token, cmd1[j].name) == 0) 
							{
								cmd1[j].func(new_row,SEMELS);	
								break;
							}
						}
					}
				}
				else
					{
						fprintf(stderr,"error, unknown command after label: %s\n", token);
						error=1;
					}
				}
			}
		}
		else {
				found_command=0;
				
				for(i=0; i<16; i++)
				{
					if(strcmp(token, cmd[i].name)==0)
					{
						is_valid_func=1;
						found_command=1;
						cmd[i].func(row,SEMELS);
						break;
					}
				}
				for (j = 0; j < 5; j++) 
				{
					if (strcmp(token, cmd1[j].name) == 0) 
					{
						is_valid_func=1;
						found_command=1;
						cmd1[j].func(row,SEMELS);
						break;
					}
				}
				
				if(is_valid_func==0)
				{
					fprintf(stderr,"error, func name is not valid\n");
					error=1;
				}
			}	
		}
 
	for (j = 0; j < macro_count; j++) {
        free(macros[j]->name);
        free(macros[j]->content);
        free(macros[j]);
    }
    free(macros);

	
}

		

	


void clear_row_arry() 
{
	char c;
    	while ((c = getchar()) != '\n' && c != EOF);
}


SEMEL** add_SEMEL(char* label , int type ,int addres)
{
	
	static SEMEL** SEMELS=NULL;
	SEMEL * new_SEMEL;
	static int semel_count=0;

	new_SEMEL = (SEMEL*)malloc(sizeof(SEMEL));
		if (new_SEMEL == NULL) {
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;
		return NULL;
		}

            new_SEMEL->name = (char*)malloc(strlen(label) + 1);
            if (new_SEMEL->name == NULL) {
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;
                free(new_SEMEL);
		return NULL;
		}
            strcpy(new_SEMEL->name, label);

           
		new_SEMEL->type = type;
		new_SEMEL->addres = addres;
		new_SEMEL->ex = 0; 

           SEMELS = (SEMEL**)realloc(SEMELS, (semel_count + 1) * sizeof(SEMEL*));
            if (SEMELS == NULL) {
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;
		return NULL;
                
            }
            SEMELS[semel_count] = new_SEMEL;
            semel_count++;
           return SEMELS;
}



