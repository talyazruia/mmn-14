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

FILE* end_file_name(int argc, char *argv[], int i, int file_type)
{
    char *file_name;
    const char *extension;
    FILE *f;

    /* קביעת הסיומת לפי file_type */
    switch (file_type) {
        case AM:
            extension = END_FILE_NAME_AM;
            break;
        case OB:
            extension = END_FILE_NAME_OB;
            break;
        case ENT:
            extension = END_FILE_NAME_ENT;
            break;
        case EXT:
            extension = END_FILE_NAME_EXT;
            break;
        default:
            fprintf(stderr, "error: invalid file type\n");
            error = 1;
            return NULL;
    }

    /* הקצאת מקום לשם הקובץ החדש */
    file_name = (char*)calloc(strlen(argv[i]) + strlen(extension) + 1, sizeof(char));
    if (file_name == NULL) {
        fprintf(stderr, "error: memory allocation failed\n");
        error = 1;
        return NULL;
    }

    /* בניית שם הקובץ */
    strcpy(file_name, argv[i]);
    strcat(file_name, extension);
    printf("%s\n", file_name); /* אפשר למחוק בשלב סופי */

    /* פתיחת קובץ לכתיבה */
    f = fopen(file_name, "w+");  /* יוצר או מחליף קובץ */
    if (f == NULL) {
        fprintf(stderr, "error: cannot create file: %s\n", file_name);
        error = 1;
        free(file_name);
        return NULL;
    }

    free(file_name);
    return f;
}

