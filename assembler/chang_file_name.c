#include "chang_file_name.h"
#include "validation.h"

FILE* end_file_name_as(int argc, char* argv[], int i,int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels)
{
	char * file_name; /* Dynamically allocated string for full filename */
	FILE * f; /* File pointer for the opened source file */

	/* Allocate space for the full filename with extension */
	/* Size = base_name_length + extension_length + null_terminator */
	file_name = (char*)calloc(strlen(argv[i]) + strlen(END_FILE_NAME_AS) + 1, sizeof(char));
	if(file_name == NULL)
	{
		fprintf(stdout,"error, space allocation failed\n");
		error_allocation(macro_count, macros, semels, semel_count, array, struct_DC, extern_labels, count_of_extern_labels);
	}
	/* Construct the filename by appending the .as extension */
	strcpy(file_name, argv[i]); /* Copy base filename first */
	strcat(file_name, END_FILE_NAME_AS); /* Append ".as" extension */
	printf("%s\n", file_name); /* Debug output: display full filename (can be removed) */
	/* Attempt to open the file in read mode */
	f = fopen(file_name, "r");
	if(f == NULL)
	{ /* File opening failed - file doesn't exist or permission denied */
		fprintf(stdout,"error, no file named: %s\n", file_name);
		error = 1; /* Set global error flag */
		free(file_name); /* Clean up allocated memory before returning */
		return NULL;
	}
	free(file_name); /* Free dynamically allocated memory - no longer needed */
	return f; /* Return successful file pointer */
}

FILE* end_file_name(int argc, char* argv[], int i, int file_type, int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels)
{
	char *file_name;				/* Dynamically allocated string for complete filename */
	const char *extension;			/* Pointer to the appropriate file extension string */
	FILE *f;						/* File pointer for the created output file */

	/* Choose extension based on file type using switch statement */
	/* Each case corresponds to a different phase of assembly output */
	switch (file_type)
	{
		case AM:
			extension = END_FILE_NAME_AM;	/* ".am" - macro-expanded source */
			break;
		case OB:
			extension = END_FILE_NAME_OB;	/* ".ob" - object code/machine code */
			break;
		case ENT:
			extension = END_FILE_NAME_ENT;	/* ".ent" - entry symbol table */
			break;
		case EXT:
			extension = END_FILE_NAME_EXT;	/* ".ext" - external symbol references */
			break;
		default:
			/* Invalid file type passed - programming error */
			fprintf(stdout, "error: invalid file type\n");
			error = 1;
			return NULL;
	}
	/* Allocate memory for the new filename */
	/* Calculate required space: base_name + extension + null_terminator */
	file_name = (char*)calloc(strlen(argv[i]) + strlen(extension) + 1, sizeof(char));
	if (file_name == NULL) 
	{
		fprintf(stdout, "error: memory allocation failed\n");
		error_allocation(macro_count, macros, semels, semel_count, array, struct_DC, extern_labels, count_of_extern_labels);
	}
	/* Build the full filename with extension */
	strcpy(file_name, argv[i]);		/* Copy base filename */
	strcat(file_name, extension);	/* Append appropriate extension */
	printf("%s\n", file_name);		/* Debug output: show created filename (remove in production) */
	/* Open the file in write+ mode (create new file or truncate existing) */
	/* "w+" mode allows both writing and reading, creates file if doesn't exist */
	f = fopen(file_name, "w+");
	if (f == NULL) 
	{	/* File creation failed - possibly permission issues or disk full */
		fprintf(stdout, "error: cannot create file: %s\n", file_name);
		error = 1;
		free(file_name);			/* Clean up memory before returning */
		return NULL;
	}
	free(file_name);				/* Free allocated memory - filename no longer needed */
	return f;						/* Return file pointer for writing output */
}
