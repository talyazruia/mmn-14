/* chang_file_name.c - Utility functions for handling file names and opening files
 *
 * This module provides functions to open input source files with the ".as" extension
 * and create output files with appropriate extensions such as .am, .ob, .ent, and .ext.
 * It ensures proper memory allocation, file naming, and error handling.
 */

#include "chang_file_name.h"

/*
 * end_file_name_as - Opens the input source file with ".as" extension.
 *
 * @param argc: The number of command-line arguments.
 * @param argv: Array of argument strings.
 * @param i: The index of the current input file in argv.
 *
 * @return A FILE pointer to the opened source file. If the file could not be opened
 *         or memory allocation fails, returns NULL and sets the global error flag.
 */
FILE* end_file_name_as(int argc, char * argv[], int i)
{
	char * file_name;
	FILE * f;

	/* Allocate space for the full filename with extension */
	file_name = (char*)calloc(strlen(argv[i]) + strlen(END_FILE_NAME_AS) + 1, sizeof(char));
	if(file_name == NULL)
	{
		fprintf(stderr,"error, space allocation failed\n");
		error = 1;
		return NULL;
	}

	/* Construct the filename by appending the .as extension */
	strcpy(file_name, argv[i]);
	strcat(file_name, END_FILE_NAME_AS);
	printf("%s\n", file_name);  /* Optional: can be removed later */

	/* Attempt to open the file in read mode */
	f = fopen(file_name, "r");
	if(f == NULL)
	{
		fprintf(stderr,"error, no file named: %s\n", file_name);
		error = 1;
		free(file_name);
		return NULL;
	}

	free(file_name); /* Free dynamically allocated memory */
	return f;
}

/*
 * end_file_name - Creates or opens an output file with the specified extension.
 *
 * @param argc: The number of command-line arguments.
 * @param argv: Array of argument strings.
 * @param i: The index of the current input file in argv.
 * @param file_type: An integer representing the type of file to create:
 *                  AM - Macro-expanded source (.am)
 *                  OB - Object code (.ob)
 *                  ENT - Entry labels file (.ent)
 *                  EXT - Extern labels file (.ext)
 *
 * @return A FILE pointer to the opened file. If creation fails, returns NULL
 *         and sets the global error flag.
 */
FILE* end_file_name(int argc, char *argv[], int i, int file_type)
{
	char *file_name;
	const char *extension;
	FILE *f;

	/* Choose extension based on file type */
	switch (file_type)
	{
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

	/* Allocate memory for the new filename */
	file_name = (char*)calloc(strlen(argv[i]) + strlen(extension) + 1, sizeof(char));
	if (file_name == NULL) 
	{
		fprintf(stderr, "error: memory allocation failed\n");
		error = 1;
		return NULL;
	}

	/* Build the full filename with extension */
	strcpy(file_name, argv[i]);
	strcat(file_name, extension);
	printf("%s\n", file_name);  /* Optional: remove in final version */

	/* Open the file in write+ mode (create or truncate) */
	f = fopen(file_name, "w+");
	if (f == NULL) 
	{
		fprintf(stderr, "error: cannot create file: %s\n", file_name);
		error = 1;
		free(file_name);
		return NULL;
	}

	free(file_name);
	return f;
}

