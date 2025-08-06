#ifndef CHANG_FILE_NAME_H
#define CHANG_FILE_NAME_H

#include "assembler.h"

/*
 * Opens a file with the extension ".as" for reading, based on a given argument.
 * The function allocates memory, appends the ".as" extension to the file name,
 * and attempts to open the file in read mode.
 * If the file doesn't exist or memory allocation fails, an error is printed.
 *
 * Parameters:
 *   argc - number of arguments passed to main
 *   argv - array of command-line arguments
 *   i    - index in argv to use for the file name
 *
 * Returns:
 *   A pointer to the opened file, or NULL if an error occurs.
 */
FILE *end_file_name_as(int argc, char * argv[], int i);

/*
 * Creates and opens a new file with a specific extension (.am, .ob, .ent, .ext),
 * depending on the file type provided. The function builds the full file name,
 * opens it in write mode (creating or replacing the file), and returns a FILE pointer.
 * If an invalid type is provided or memory allocation fails, an error is reported.
 *
 * Parameters:
 *   argc      - number of arguments passed to main
 *   argv      - array of command-line arguments
 *   i         - index in argv to use for the file name
 *   file_type - the type of file to create (AM, OB, ENT, EXT)
 *
 * Returns:
 *   A pointer to the created file, or NULL if an error occurs.
 */
FILE *end_file_name(int argc, char * argv[], int i, int file_type); 

#endif

