#include "assembler.h"



FILE* end_file_name_as(int argc, char * argv[], int i)
{
	char * file_name;
	FILE * f;
	file_name = (char*)calloc(strlen(argv[i]) + strlen(END_FILE_NAME_AS) + 1, sizeof(char));
	if(file_name == NULL)
	{
		fprintf(stderr,"error,space allocation failed\n");
		error=1;
		return NULL;
	}
	strcpy(file_name, argv[i]);
	strcat(file_name, END_FILE_NAME_AS);
	printf("%s\n", file_name);

	f=fopen(file_name, "r");  /* "r" במקום "r+" */
	if(f==NULL)
	{
		fprintf(stderr,"error,no file named: %s\n", file_name);
		error=1;
		free(file_name);
		return NULL;
	}
	free(file_name);  /* שחרור זיכרון */
	return f;
}

FILE* end_file_name_am(int argc, char * argv[], int i)
{
    char * file_name;
    FILE * f;
    
    file_name = (char*)calloc(strlen(argv[i]) + strlen(END_FILE_NAME_AM) + 1, sizeof(char));
    if(file_name == NULL)
    {
        fprintf(stderr,"error,space allocation failed\n");
        error=1;
        return NULL;
    }
    
    strcpy(file_name, argv[i]);
    strcat(file_name, END_FILE_NAME_AM);
    printf("%s\n", file_name);
    
    f = fopen(file_name, "w+");  /* יוצר קובץ חדש או מחליף קיים */
    if(f == NULL)
    {
        fprintf(stderr,"error, cannot create file: %s\n", file_name);
        error=1;
        free(file_name);
        return NULL;
    }
    
    free(file_name);  /* שחרור זיכרון */
    return f;
}
