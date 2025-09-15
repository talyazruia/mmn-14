#ifndef ROW_ANALYSIS_H
#define ROW_ANALYSIS_H

#include "assembler.h"

/*
 * ------------------------
 * Purpose:  	Analyze a one-operand instruction line and increment the global IC
 *		(Instruction Counter) according to the number of words the instruction
 *		will occupy in memory.
 * Parameters: row  - The line text containing the instruction and its operand.
 * Returns:void.
 * Side Effects:May increase the global IC counter by 1 or more depending on addressing mode.
 * Errors:None (assumes syntax is valid when called).
 */
void ic_count_1_arg(char* row);

/*
 * ------------------------
 * Purpose:	Analyze a two-operand instruction line and increment the global IC
 *		according to the number of words required to store the instruction
 *		and its operands in memory.
 * Parameters: row  - The line text containing the instruction and its operands.
 * Returns: void.
 * Side Effects: Increments the global IC counter by the calculated number of words.
 * Errors: None (assumes valid syntax when called).
 */
void ic_count_2_arg(char* row);

/*
 * -----------------------
 * Purpose: 	Count the number of numeric values in a .data directive and increment
 *		the global DC (Data Counter) accordingly.
 * Parameters: row  - The line text containing the .data directive.
 * Returns: void.
 * Side Effects: Increments the global DC counter by the number of integers found.
 * Errors: None here (assumes list validity checked elsewhere).
 */
void dc_count_data(char* row);

/*
 * -------------------------
 * Purpose: 	Increment the global DC based on the length of the string operand
 *		in a .string directive, including the terminating '\0' character.
 * Parameters: row  - The line text containing the .string directive.
 * Returns: void.
 * Side Effects: Increments the global DC counter by (string length + 1).
 * Errors: None here (assumes valid .string syntax when called).
 */
void dc_count_string(char* row);

/*
 * ----------------------
 * Purpose: 	Increment the global DC based on the total number of elements in
 *		a matrix defined by a .mat directive, filling with zero where no
 *		value is specified.
 * Parameters: row  - The line text containing the .mat directive.
 * Returns: void.
 * Side Effects: Increments the global DC counter by (rows * columns).
 * Errors: None here (assumes matrix format validity checked elsewhere).
 */
void dc_count_mat(char* row);

/*
 * ----------------------
 * Purpose:	Perform the first pass over the input file:
 *		- Handle macro definitions/expansions.
 *		- Parse each line, identify labels, instructions, and directives.
 *		- Count IC/DC for memory allocation.
 *		- Record symbols into the symbol table.
 * Parameters:	f			 - Input file stream (.as).
 *		macro_count  - Number of macros defined.
 *		macros		 - Pointer to the macro definitions array.
 *		cmd			 - Command table for main instruction set.
 *		cmd1		 - Command table for additional commands (e.g., pseudo-ops).
 *		SEMELS		 - Pointer to the symbol table pointer (may be reallocated).
 *		semel_count  - Pointer to the number of symbols in the table.
 * Returns: void.
 * Side Effects: Appends symbols to SEMELS, updates IC/DC counters, may set global error flag.
 * Errors: Reports syntax errors, duplicate labels, or invalid constructs.
 */
void row_analysis(FILE *f,int macro_count,macro **macros,command cmd[], command1 cmd1[],SEMEL ***SEMELS,int *semel_count,binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels); 

/*
 * ------------------------
 * Purpose:	Clear the input buffer in cases where a line is too long and overflows
 *		the maximum allowed length.
 * Parameters: None.
 * Returns: void.
 * Side Effects: Consumes characters from stdin (or file) until newline or EOF.
 * Errors: None.
 */
void clear_row_arry(void);

/*
 * -------------------
 * Purpose: 	Add a new symbol (SEMEL) to the symbol table if it has a valid format
 *		and does not already exist.
 * Parameters:	label		 - The symbol name.
 *		type		 - Symbol type (e.g., code, data).
 *		addres		 - Symbol address.
 *		semels		 - Pointer to the symbol table pointer (may be reallocated).
 *		semel_count  - Pointer to the current number of symbols.
 *		ex_en		 - Flag for extern/entry status.
 * Returns: void.
 * Side Effects: Reallocates the symbol table if needed; updates *semel_count.
 * Errors: Invalid name format or duplicate symbol name.
 */
void add_SEMEL(char* label,int type,int addres,SEMEL*** semels,int* semel_count,int ex_en, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels, int macro_count, macro*** macros);

/*
 * -------------------------------
 * Purpose:	Check whether a label has a valid format:
 *		- Starts with a letter.
 *		- Contains only alphanumeric characters.
 *		- Length does not exceed 30 characters.
 * Parameters: label - The label name to check.
 * Returns: int	  - 1 if valid format, 0 otherwise.
 * Side Effects: None.
 * Errors: None.
 */
int is_valid_label_format (char* label_mcro,int type,command cmd[]);

/*
 * --------------------------------------
 * Purpose:	Check if the given string contains exactly two valid pairs of square
 *		brackets, as required by matrix syntax.
 * Parameters: str	 - String to examine.
 * Returns: int	 - 1 if exactly two pairs of brackets are found, 0 otherwise.
 * Side Effects: None.
 * Errors: None.
 */
int has_two_square_bracket_pairs(char* str);

#endif /* ROW_ANALYSIS_H */

