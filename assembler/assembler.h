/* הגדרות כלליות למבנים, משתנים גלובליים והצהרות פונקציות כלליות */
#ifndef MYMAT_H
#define MYMAT_H

#include <stdio.h>     /* Standard I/O functions */
#include <string.h>    /* String manipulation functions */
#include <stdlib.h>    /* Memory allocation and conversion functions */
#include <ctype.h>     /* Character classification functions */
#include <limits.h>    /* Definitions for integer limits */

/* Constants for file suffixes */
#define END_FILE_NAME_AS ".as"
#define END_FILE_NAME_AM ".am"
#define END_FILE_NAME_OB ".ob"
#define END_FILE_NAME_ENT ".ent"
#define END_FILE_NAME_EXT ".ext"
#define MAX_LEN_OF_ROW 82
#define MAX_LEN_OF_LABEL 31

/* File type enumeration */
enum FileType { AM = 1, OB, EXT, ENT };

/* Symbol (label) structure */
typedef struct {
	char * name;     /* Symbol name */
	int addres;      /* Symbol address */
	int type;        /* Type: instruction/data/etc. */
	int ex_en;       /* External (1) or entry (2) */
} SEMEL;

/* Command with two operands */
typedef struct {
	char *name;  /* Command name */
	int param;   /* Number of operands */
	int op;      /* Opcode */
} command;

/* Command with one or zero operands */
typedef struct {
	char *name;  /* Command name */
	int param;   /* Number of operands */
} command1;

/* Macro structure */
typedef struct {
	char * name;     /* Macro name */
	char * content;  /* Macro body (text to replace) */
} macro;

/* Binary code for instructions */
typedef struct {
	unsigned char first;   /* First byte */
	unsigned char second;  /* Second byte */
} binary_code;

/* Binary code for data directives */
typedef struct {
	char first;   /* First byte */
	char second;  /* Second byte */
} binary_directive;


/* External label for .ext file output */
typedef struct {
	char * name;   /* Label name */
	int addres;    /* Address where it appears */
} extern_label;

/* Type of binary line: instruction or directive */
typedef enum {
    TYPE_INSTRUCTION,
    TYPE_DIRECTIVE
} array_type;

/* Global counters used across the assembler */
extern int IC; /* Instruction Counter */
extern int DC; /* Data Counter */
extern int error; /* Error flag */

extern int current_size_instaction_struct; /* Number of elements in binary_code array */
extern int current_size_directive_struct;  /* Number of elements in binary_directive array */
extern int sum_of_row; /* Current input code line number being analyzed */

/* =========================================================================
 * Function: macro_analysis
 * -------------------------------------------------------------------------
 * Description:
 *   Processes a source file to detect and expand user-defined macros.
 *   Stores the resulting macro-expanded lines in a new file with `.am` suffix.
 *
 * Parameters:
 *   f1               - Input file pointer (.as file)
 *   cmd, cmd1        - Arrays of known command definitions (2-op and 1/0-op)
 *   argc, argv, i    - Program arguments and current file index
 *   macros_out       - Output parameter: pointer to macro list (array)
 *   macro_count_out  - Output parameter: number of macros found
 *
 * Returns:
 *   FILE* pointer to the created `.am` file, or NULL if an error occurred.
 * =========================================================================
 */
FILE * macro_analysis(FILE *f1, command cmd[], command1 cmd1[], int argc, char *argv[], int i, macro*** macros_out, int* macro_count_out);

#endif

