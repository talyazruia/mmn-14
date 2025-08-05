#include "assembler.h"


void row_analysis(FILE * f , int macro_count, macro** macros, command cmd[],command1 cmd1[],SEMEL*** SEMELS, int* semel_count)
{
	char row[MAX_LEN_OF_ROW+2];
	char row_copy[MAX_LEN_OF_ROW+2];
	char new_row_copy[MAX_LEN_OF_ROW + 2];
	char label[31];
	char *token;
	char *token2;
	size_t len;
	size_t i;
	int valid_label;
	int j;
	int type;
	int addres;
	char *new_row;
	 char *colon;
	int found_command;
	char* cmd_2_arg[] = {"mov","cmp","add", "sub","lea"};
	char * cmd_1_arg[]={"not","clr","inc","dec","jmp","bne","red","prn","jsr"};
	char * cmd_0_arg[]={"rts","stop"};
	

	while (fgets(row, sizeof(row), f))
	{
		sum_of_row++;
		if (strchr(row, '\n') == NULL) 
		{
            		fprintf(stderr,"error in line %d: line too long (more than %d characters)\n",sum_of_row, MAX_LEN_OF_ROW);
			error=1;
			clear_row_arry();
			continue;
		}
		if(row[0]==';')
		{
			continue;
		}
		strcpy(row_copy, row);
        	token = strtok(row_copy, " \t\n\r");
		if(token == NULL)
		{
			continue;
		}
		valid_label=1;
        	
        	len = strlen(token);
        	if (len > 0 && token[len - 1] == ':')
        	{
			token[len-1]='\0';
			strcpy(label,token);
			if (!is_valid_label_format(label)) 
			{
    				error = 1;
    				valid_label = 0;
			}
		
			if(valid_label==1)
			{
				for (j = 0; j < macro_count; j++) 
				{
					if (strcmp(label, macros[j]->name) == 0) 
					{
						fprintf(stderr, "error in line:%d, label cant be macro name: %s\n",sum_of_row, label);
						error=1;
						valid_label=0;
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
						fprintf(stderr,"error in line:%d, label cannot be the name of a command: %s\n",sum_of_row, label);
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
						fprintf(stderr, "error in line:%d, label cannot precede .entry or .extern: %s\n",sum_of_row, label);
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
					if(found_command){
						add_SEMEL(label, type, addres, SEMELS, semel_count, 2);
					if(SEMELS!=NULL)
					{
				 		colon = strchr(row, ':');
						if (colon != NULL) 
      							new_row = colon + 1;
						else
							new_row=row;
						for(i=0; i<5; i++)
						{
							if(strcmp(token, cmd_2_arg[i])==0)
							{
								strncpy(new_row_copy, new_row, MAX_LEN_OF_ROW + 1);
								new_row_copy[MAX_LEN_OF_ROW + 1] = '\0';
								ic_count_2_arg(new_row_copy);
								break;
							}
						}
				 		for (j = 0; j < 9; j++) 
						{
						
							if (strcmp(token, cmd_1_arg[j]) == 0) 
							{
								strncpy(new_row_copy, new_row, MAX_LEN_OF_ROW + 1);
								new_row_copy[MAX_LEN_OF_ROW + 1] = '\0';
								ic_count_1_arg(new_row_copy);	
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
							strncpy(new_row_copy, new_row, MAX_LEN_OF_ROW + 1);
							new_row_copy[MAX_LEN_OF_ROW + 1] = '\0';
							dc_count_data(new_row_copy);
						}
						if(strcmp(token, ".string") == 0) 
						{
							strncpy(new_row_copy, new_row, MAX_LEN_OF_ROW + 1);
							new_row_copy[MAX_LEN_OF_ROW + 1] = '\0';
							dc_count_string(new_row_copy);
						}
						if(strcmp(token, ".mat") == 0) 
						{
							strncpy(new_row_copy, new_row, MAX_LEN_OF_ROW + 1);
							new_row_copy[MAX_LEN_OF_ROW + 1] = '\0';
							dc_count_mat(new_row_copy);
						}
						if(strcmp(token, ".extern")==0)
						{
							token2 = strtok(NULL, " \t\n\r");
							if(token2==NULL)
							{
								error=1;
								fprintf(stderr, "missing label in func extern\n");
								continue;
							}
							if (is_valid_label_format(token2))
								add_SEMEL(token2,2/*type for extern*/,0, SEMELS, semel_count, 1);
							else
							{
								fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
								error=1;
							}
						}
						if(strcmp(token, ".entry")==0)
					    	{
							token2 = strtok(NULL, " \t\n\r");
							if(token2==NULL)
							{
								error=1;
								fprintf(stderr, "missing label in func entry\n");
								continue;
							}
							found_command = 1;
							if (is_valid_label_format(token2))
								add_SEMEL(token2,1/*type for entry*/,1, SEMELS, semel_count, 0);
							else
							{
								fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
								error=1;
							}
						}
					}
				}
				else
				{
					fprintf(stderr,"error in line:%d, unknown command after label: %s\n", sum_of_row,token);
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
						found_command=1;
						ic_count_2_arg(row);
						break;
					}
				}
				if(!found_command) {
				for (j = 0; j < 9; j++) 
				{
					if (strcmp(token, cmd_1_arg[j]) == 0) 
					{
						found_command=1;
						ic_count_1_arg(row);
						break;
					}
				}
				}
				if(!found_command) {
				for (j = 0; j < 2; j++) 
				{
					if (strcmp(token, cmd_0_arg[j]) == 0) 
					{
						found_command=1;
						IC++;
						break;
					}
				}
				}
				if(strcmp(token, ".data") == 0) 
				{
					dc_count_data(row);
					found_command = 1;
				}
				if(strcmp(token, ".string") == 0) 
				{
					dc_count_string(row);
					found_command = 1;
				}
				if(strcmp(token, ".mat") == 0) 
				{
					dc_count_mat(row);
					found_command = 1;
				}
				if(strcmp(token, ".extern")==0)
				{
					token2 = strtok(NULL, " \t\n\r");
					if(token2==NULL)
					{
						error=1;
						fprintf(stderr, "missing label in func extern\n");
						continue;
					}
					found_command = 1;
					if (is_valid_label_format(token2))
						add_SEMEL(token2,2/*type for extern*/,0, SEMELS, semel_count, 1);
					else
					{
						fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
						error=1;
					}
				}
				if(strcmp(token, ".entry")==0)
				{
					token2 = strtok(NULL, " \t\n\r");
					if(token2==NULL)
					{
						error=1;
						fprintf(stderr, "error in line:%d missing label in func entry\n",sum_of_row);
						continue;
					}
					found_command = 1;
					if (is_valid_label_format(token2))
						add_SEMEL(token2,1/*type for entry*/,1, SEMELS, semel_count, 0);
					else
					{
						fprintf(stderr, "error in line:%d Invalid label\n",sum_of_row);
						error=1;
					}
				}
					

				if(!found_command)
				{
					/*fprintf(stderr,"error, func name is not valid\n");*/
					error=1;
				}
			}	
		}
 
	for (j = 0; j < macro_count; j++) 
	{
		if(macros[j])
		{
        		free(macros[j]->name);
        		free(macros[j]->content);
        		free(macros[j]);
		}
    	}
    	free(macros);
}


int is_valid_label_format(const char* label) 
{
    	size_t i;
    	size_t len = strlen(label);
    
    	if (len > 30) 
	{
        	fprintf(stderr, "error in line %d: label too long (max 30 characters): %s\n", sum_of_row,label);
		error=1;
        	return 0;
    	}
    
    	if (!isalpha(label[0])) 
	{
        	fprintf(stderr, "error in line:%d, label must start with a letter: %s\n",sum_of_row, label);
		error=1;
        	return 0;
    	}
    
    	for(i = 1; i < len; i++) 
	{
        	if (!isalnum(label[i])) 
		{
            		fprintf(stderr, "error in line:%d, label can contain only letters and digits: %s\n", sum_of_row,label);
			error=1;
            		return 0;
        	}
    	}
    return 1;
}

void clear_row_arry() 
{
	char c;
    	while ((c = getchar()) != '\n' && c != EOF);
}


void add_SEMEL(char* label, int type, int addres, SEMEL*** SEMELS, int* semel_count, int ex_en)
{
	SEMEL* new_SEMEL;
	SEMEL** temp;
	int j;

	if(SEMELS != NULL && *SEMELS != NULL)
	{
		for (j = 0; j<*semel_count; j++) 
		{
			if (strcmp(label, (*SEMELS)[j]->name) == 0) 
			{
				if(((*SEMELS)[j]->ex_en==1 && ex_en==2 )||((*SEMELS)[j]->ex_en==2 && ex_en==1))/*זה תווית אקסטרן ומה שאני מכניסה היא רגילה רק נשנה את הסוג ולא נוסיף כי היא כבר קיימת*/
				{	
					fprintf(stderr, "error in line:%d a regular label cannot be an extern label and vice versa.\n",sum_of_row);
					error=1;
					return;
				}
				else if(((*SEMELS)[j]->ex_en==1 && ex_en==0) || ((*SEMELS)[j]->ex_en==0 && ex_en==1))
				{
					fprintf(stderr, "error in line:%d, a label cannot be both extern and entry.\n",sum_of_row);/*תווית לא יכולה להיות גם אקסטרן וגם אנטרי*/
					error=1;
					return;
				}
				else if((*SEMELS)[j]->ex_en==2 &&  ex_en==0)
				{
					(*SEMELS)[j]->ex_en=ex_en;       
					return;
				}
				else if((*SEMELS)[j]->ex_en==0 && ex_en==2)
				{
					if(addres==IC)
						(*SEMELS)[j]->type=0;
					(*SEMELS)[j]->addres=addres;
					return;
				}
				else if((*SEMELS)[j]->ex_en==2 && ex_en==2)
				{
					fprintf(stderr, "error in line:%d, label already defined: %s\n",sum_of_row, label);
					error = 1;
					return;
				}
			}
		}
	}

	new_SEMEL = (SEMEL*)malloc(sizeof(SEMEL));
	if (new_SEMEL == NULL) 
	{
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;
		return;
	}

        new_SEMEL->name = (char*)malloc(strlen(label) + 1);
        if (new_SEMEL->name == NULL) 
	{
                fprintf(stderr, "error, memory allocation failed\n");
                error = 1;
                free(new_SEMEL);
		return;
	}
        strcpy(new_SEMEL->name, label);

        new_SEMEL->type = type;
	new_SEMEL->addres = addres;
	new_SEMEL->ex_en = ex_en; 

	temp = realloc(*SEMELS, (*semel_count + 1) * sizeof(SEMEL*));
	if (!temp) 
	{
    		fprintf(stderr, "error, memory allocation failed\n");
    		error = 1;
		free(new_SEMEL->name);
        	free(new_SEMEL);
    		return;
	}
	*SEMELS = temp;
        (*SEMELS)[*semel_count] = new_SEMEL;
    	(*semel_count)++;
}
