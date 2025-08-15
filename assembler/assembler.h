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
#define NUM_OF_IC_COMMAND 16
#define NUM_OF_DC_COMMAND 5
#define _2_ARG_COMMAND_IC 5 
#define _1_ARG_COMMAND_IC 9
#define _0_ARG_COMMAND_IC 2
#define MAX_NUMBES_OF_ROW_IN_INPUT 255
#define FIRST_ADDRES 100
#define ENCODING_IC_COMMAND 1
#define ENCODING_DC_COMMAND 2
#define ENCODING_EXTERN_FILE 3
#define ENCODING_ENTERY_FILE 4
#define ENCODING_IC_DC 5
#define LEN_OF_BASE_4_ROW 4
#define ADDRESSING_MODE_HASH_NUM 0
#define ADDRESSING_MODE_SEMEL 1
#define ADDRESSING_MODE_MATRIX 2
#define ADDRESSING_MODE_REG 3
#define OP_OF_FIRST_HASH_NUM 3
#define OP_OF_FIRST_HASH_NUM1 13
#define OP_OF_CMP 1
#define OP_OF_LEA 4
#define start_IC 100
#define TWO_BITS_MASK 0x03  /* Binary mask: 00000011 */
#define FOUR_BITS_MASK 0x0F  /* Binary mask: 00001111 */
#define SHIFT_START_A  6  /* Initial shift for extracting first base4 digit */
#define BASE4_STR_TOTAL_LEN 6 /* BASE4_STR_LEN plus terminating '\0' */
#define num_of_bit 4
#define num_bits 2
#define BASE4_STR_LEN 5	/* 4 chars for 'a'..'d' + 1 extra for b's top-2 bits */
#define ex_en_rgular_label 2
#define type_ex 2/*type for extern*/
#define ex_en_ex_label 1
#define type_en 1/*type for entry*/
#define ex_en_en_label 0
#define Immediate_mode 0
#define valid_num 1
#define positive_num 2
#define semel_type 1
#define macro_type 0
#define LEN_OF_BASE_4_ROW_IC 4
#define LEN_OF_BASE_4_ROW_DC 4
#define MAX_NUM_IN_10_BIT 511
#define MIN_NUM_IN_10_BIT -512
#define MAX_NUM_IN_8_BIT 127
#define MIN_NUM_IN_8_BIT -128

/* File type enumeration */
enum FileType { AM=1 , OB, EXT, ENT };

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
extern int count_of_entry_labels;/*count entry labels*/

/* 
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
 */
FILE * macro_analysis(FILE *f1, command cmd[],int argc, char*  argv[], int i, macro*** macros_out,int* mcro_count_out, SEMEL** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels);

#endif

