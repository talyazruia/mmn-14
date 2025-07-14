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
	char* cmd_2_arg[] = {"mov","cmp","add", "sub","lea"};
	char * cmd_1_arg[]={"not","clr","inc","dec","jmp","bne","red","prn","jsr"};
	char * cmd_0_arg[]={"rts","stop"};
	

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
		valid_label=1;
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
			if (!isalpha(label[0])) 
			{
				fprintf(stderr, "error, label must start with a letter: %s\n", label);
				error = 1;
				valid_label = 0;
			}
/* שאר התווים יכולים להיות אותיות או ספרות */
			else 
			{
				for(i = 1; i < strlen(label); i++) 
				{
					if (!isalnum(label[i])) 
					{
						fprintf(stderr, "error, label can contain only letters and digits: %s\n", label);
						error = 1;
						valid_label = 0;
						break;
					}
				}
			}
			if(valid_label==1)
			{
				for (j = 0; j < macro_count; j++) 
				{
					if (strcmp(label, macros[j]->name) == 0) 
					{
						fprintf(stderr, "error, label cant be macro name: %s\n", label);
						error=1;
						valid_label=0;
            					break;
					}
				}
			}
			if(valid_label==1 && SEMELS != NULL)
			{
				for (j = 0; SEMELS[j] != NULL; j++) {
					if (strcmp(label, SEMELS[j]->name) == 0) {
						fprintf(stderr, "error, label already defined: %s\n", label);
						error = 1;
						valid_label = 0;
						break;
					}
				}
			}
			if(valid_label==1)
			{
				for(i=0; i<16; i++)
				{
					if(strcmp(label, cmd[i].name)==0)
					{
						valid_label=0;
						error=1;
						fprintf(stderr,"error, label cannot be the name of a command: %s\n", label);
						break;
					}
				}
			}
			if(valid_label==1)
			{
				
				token=strtok(NULL, " \t\n\r");
				if(token!=NULL)
				{
					if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0) {
						fprintf(stderr, "error, label cannot precede .entry or .extern: %s\n", label);
						error = 1;
						continue;
					}
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
						for(i=0; i<5; i++)
						{
							if(strcmp(token, cmd_2_arg[i])==0)
							{
								ic_count_2_arg(new_row);
								break;
							}
						}
				 		for (j = 0; j < 9; j++) 
						{
						
							if (strcmp(token, cmd_1_arg[j]) == 0) 
							{
								ic_count_1_arg(new_row);	
								break;
							}
						}
						for (j = 0; j < 2; j++) 
						{
							if (strcmp(token, cmd_0_arg[j]) == 0) 
							{
								IC++;
								break;
							}
						}
						if(strcmp(token, ".data") == 0) 
						{
							dc_count_data(new_row);
						}
						if(strcmp(token, ".string") == 0) 
						{
							dc_count_string(new_row);
						}
						if(strcmp(token, ".mat") == 0) 
						{
							dc_count_mat(new_row);
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
				
				for(i=0; i<5; i++)
				{
					if(strcmp(token, cmd_2_arg[i])==0)
					{
						is_valid_func=1;
						found_command=1;
						ic_count_2_arg(row);
						break;
					}
				}
				for (j = 0; j < 9; j++) 
				{
					if (strcmp(token, cmd_1_arg[j]) == 0) 
					{
						is_valid_func=1;
						found_command=1;
						ic_count_1_arg(row);
						break;
					}
				}
				for (j = 0; j < 2; j++) 
				{
					if (strcmp(token, cmd_0_arg[j]) == 0) 
					{
						is_valid_func=1;
						found_command=1;
						IC++;
						break;
					}
				}
				if(strcmp(token, ".data") == 0) 
				{
					dc_count_data(new_row);
				}
				if(strcmp(token, ".string") == 0) 
				{
					dc_count_string(new_row);
				}
				if(strcmp(token, ".mat") == 0) 
				{
					dc_count_mat(new_row);
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
