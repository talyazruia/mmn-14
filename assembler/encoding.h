#ifndef ENCODING_H
#define ENCODING_H

#include "assembler.h"

/*
 * -----------------------
 * Purpose:	Convert encoded machine data (instructions, directives, symbols) into
 *		a base-4 textual representation and write it to the specified output file.
 *		The format and source array are selected by the struct_type parameter.
 * Parameters:	array		 - Pointer to the source array (type depends on struct_type).
 *		argc		 - Argument count from main().
 *		argv		 - Argument vector from main() (used for file naming).
 *		i			 - Index of the current file in argv[].
 *		f2			 - Output file stream for writing base-4 data.
 *		struct_type  - Data type selector:
 *						1 = instruction array (IC segment)
 *						2 = directive array  (DC segment)
 *						3 = extern label table
 *						4 = entry symbol table
 *						5 = IC and DC values
 *		semel_count  - Pointer to symbol or extern label count (used for types 3/4).
 *		ic			 - Final IC value (used for type 5).
 * Returns:	void.
 * Side Effects:Writes formatted base-4 lines to f2. May read from global counters such
 *		as IC, DC, and current_size_* variables.
 * Errors:	Prints an error message and returns if array or f2 is NULL,
 *		or if struct_type is invalid.
 */
void BinaryToBase4(int argc,char *argv[],int i,FILE* f2,int struct_type,int* semel_count,int ic, int macro_count, macro*** macros, SEMEL** semels, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_label);

/*
 * -----------------------
 * Purpose:	Convert two bytes (a, b) into a base-4 representation using a custom
 *		'a'..'d' alphabet for the digits. The first byte produces 4 base-4
 *		digits, and the top 2 bits of the second byte produce the 5th digit.
 * Parameters:	a		- First byte of data (8 bits).
 *		b		- Second byte of data (8 bits).
 *		result	- Pointer to a preallocated buffer of at least 6 chars
 *				  (5 output characters + '\0').
 * Returns:	char *	- Pointer to the result buffer containing the base-4 string.
 * Side Effects:Writes to the provided result buffer; does not allocate memory.
 * Errors:	None (caller must ensure valid buffer size).
 */
char *base4_convert(char a, char b, char* result);

/*
 * --------------------
 * Purpose:	Append a single encoded number to the target array (instruction or
 *		directive segment). Numbers in instruction words also receive the
 *		ARE bits in the high two bits of the second byte.
 * Parameters:	num		- Number to encode.
 *		array	- Pointer to the target array (binary_code** or binary_directive**).
 *		type	- TYPE_INSTRUCTION or TYPE_DIRECTIVE.
 *		flag	- ARE value (0-3) to store in high bits of the second byte
 *				  for TYPE_INSTRUCTION; ignored for TYPE_DIRECTIVE.
 * Returns:	void.
 * Side Effects:May realloc *array to grow by one element. Updates global IC or DC
 *		and the corresponding current_size_* counter.
 * Errors:	On allocation failure, frees the array, sets it to NULL, and returns.
 */
void add_number(int num, array_type type, int ARE , int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_label);

/*
 * -------------------------
 * Purpose:	Pack two small numbers (4 bits each) into a single byte and append
 *		the resulting word to the instruction segment.
 * Parameters:	num1	- First number (0-15), stored in high nibble.
 *		num2	- Second number (0-15), stored in low nibble.
 *		array	- Pointer to the instruction words array to append to.
 * Returns:	void.
 * Side Effects:Reallocates *array to append one element, increments global IC and
 *		current_size_instaction_struct.
 * Errors:	On allocation failure, sets global error flag and returns.
 */
void add_two_numbers(int num1, int num2, binary_code** array ,int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_label);

/*
 * -------------------
 * Purpose:	Encode an instruction word from its opcode and operand addressing mode
 *		values, and append it to the instruction segment.
 * Parameters:	opcode	- Instruction opcode (0-15, stored in bits 7-4).
 *		op1		- First operand addressing mode (0-3, stored in bits 3-2).
 *		op2		- Second operand addressing mode (0-3, stored in bits 1-0).
 *		array	- Pointer to the instruction words array to append to.
 * Returns:	void.
 * Side Effects:Allocates or reallocates *array to grow by one element, writes the
 *		encoded byte, clears the second byte to 0, and increments IC and
 *		current_size_instaction_struct.
 * Errors:	On allocation failure, frees the array, sets it to NULL, sets the
 *		global error flag, and returns.
 */
void to_binary(int opcode, int op1, int op2, binary_code** array,int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_label);

#endif /* ENCODING_H */

