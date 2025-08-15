#ifndef SECOND_ROW_ANALYSIS_H
#define SECOND_ROW_ANALYSIS_H

#include "assembler.h"

/*
 * -----------------------------
 * Purpose:	Perform the second pass over the source file: parse each line,
 *		validate operands, encode instructions and directives into machine
 *		words, and record uses of extern/entry symbols as required.
 * Parameters:	f					- Input stream of the macro-expanded source (.am).
 *		cmd					- Command table (name, operand count, opcode).
 *		SEMELS				- Pointer to the symbol table pointer (may be reallocated).
 *		semel_count			- Pointer to the number of symbols in the table.
 *		array				- Pointer to the instruction words array (IC segment).
 *		struct_DC			- Pointer to the directive words array (DC segment).
 *		extern_labels		- Pointer to the extern-usage records array.
 *		count_of_extern_labels
 *							- Pointer to the number of extern-usage records.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Array of macro definitions.
 * Returns: void.
 * Side Effects: May append to or reallocate the instruction/directive arrays,
 *		update the symbol table, grow extern records, and advance the
 *		global IC/DC counters. May set a global error flag and print
 *		diagnostics to stderr.
 * Errors: Reports invalid syntax, wrong operand counts/types, illegal symbol
 *		usage, malformed directives, and other semantic errors.
 * Notes: Assumes the first pass already collected labels and base addresses.
 */
void second_row_analysis(FILE *f,command cmd[],SEMEL ***SEMELS,int *semel_count,binary_code **array,binary_directive **struct_DC,extern_label **extern_labels,int *count_of_extern_labels,int macro_count, macro** macros);

/*
 * -----------------------------
 * Purpose:	Parse and encode a single instruction line: identify the mnemonic,
 *		validate and classify operands, emit one or more machine words,
 *		and record extern references as needed.
 * Parameters:	row					- The input line text (mutable buffer).
 *		semels				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 *		cmd					- Command table (mnemonics/opcodes).
 *		array				- Pointer to the instruction words array to append to.
 *		extern_labels		- Pointer to the extern-usage records array to append to.
 *		count_of_extern_label
 *							- Pointer to the number of extern-usage records.
 *		struct_DC			- Pointer to the directive words array.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Array of macro definitions.
 * Returns: int					- Implementation-defined success indicator (e.g., number
 *							  of words emitted or 0/1 for failure/success).
 * Side Effects: Appends encoded words to *array, advances the global IC, and may add
 *		extern-usage records.
 * Errors: Invalid mnemonic, wrong operand count, illegal addressing modes,
 *		unknown symbols, or malformed operands.
 */
int IC_command_analysis(char row[], SEMEL** semels, int* semel_count, command cmd[], binary_code** array, extern_label **extern_labels, int* count_of_extern_label ,binary_directive **struct_DC,int macro_count, macro** macros);

/*
 * -------------------------
 * Purpose:	Validate numeric text under a selected policy ("mode"), optionally
 *		consuming a leading '#' for immediate values, enforcing sign rules,
 *		and checking that the value fits in the range of 'int'.
 * Parameters:	str					- Mutable string to validate; may be edited in-place
 *							  (e.g., removing the leading '#').
 *		mode				- Validation mode:
 *								0 = immediate form, requires leading '#', no space after it.
 *								1 = signed integer (range-checked).
 *								2 = non-negative integer only (range-checked, '-' forbidden).
 * Returns: int					- 1 if valid according to the selected mode, 0 otherwise.
 * Side Effects:May shift the buffer left by one to remove '#'. Trims surrounding
 *		whitespace. Does not allocate memory.
 * Errors: Rejects non-digits (except a single leading '+' or '-' where allowed),
 *		embedded non-space junk, extra non-space after digits, or out-of-range values.
 */
int is_valid_number(char *str, int mode);

/*
 * -------------
 * Purpose:	Check whether a token denotes a valid register in the form r0..r7.
 *		Leading/trailing spaces around the token are ignored.
 * Parameters: str					- Candidate token (may contain surrounding whitespace).
 * Returns: int					- 1 if the token is exactly r0..r7, 0 otherwise.
 * Side Effects: None.
 * Errors: None beyond returning 0 for invalid formats.
 */
int reg(char *str);

/*
 * ----------------------
 * Purpose: 	Validate a matrix operand of the form:
 *			name[rX][rY]
 *		Where 'name' is a symbol in the table, and rX, rY are registers r0..r7.
 *		On success, returns a newly-allocated int[3] with:
 *			[index_in_symbol_table, regX_number, regY_number].
 * Parameters: 	str					- The matrix operand text.
 *		SEMELS				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 *		extern_labels		- Pointer to the extern-usage records array.
 *		COUNT_OF_EXTERN_LABEL
 *							- Pointer to the number of extern-usage records.
 *		array				- Pointer to the instruction words array.
 *		struct_DC			- Pointer to the directive words array.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Pointer to array of macro pointers (triple pointer).
 * Returns:	int *				- Allocated array of length 3 on success; NULL on failure.
 * Side Effects:Allocates memory for the 3-int result; caller must free().
 *		May call error_allocation on memory allocation failure.
 * Errors:Invalid syntax, unknown symbol, non-register indices, or out-of-range registers.
 */
int* valid_matrix(char *str, SEMEL** SEMELS, int* semel_count, extern_label **extern_labels, int* COUNT_OF_EXTERN_LABEL, binary_code** array, binary_directive **struct_DC,int macro_count, macro*** macros);

/*
 * ---------------------
 * Purpose:	Locate a symbol by name and verify it is valid for use as an operand
 *		in the current context.
 * Parameters:	str					- Symbol name token.
 *		SEMELS				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 * Returns:	int					- Index of the symbol in the table, or -1 on failure.
 * Side Effects:None (lookup only).
 * Errors:	Returns -1 for missing symbols or disallowed symbol kinds.
 */
int valid_SEMEL(char *str, SEMEL **SEMELS, int *semel_count);

/*
 * -----------------------------
 * Purpose:	Append a record of an extern symbol reference at the current IC.
 *		The function grows the extern-labels array as needed.
 * Parameters:	extern_labels		- Pointer to the extern records array.
 *		COUNT_OF_EXTERN_LABEL
 *							- Pointer to the count of extern records.
 *		str					- Extern symbol name used at the current IC.
 *		SEMELS				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 *		array				- Pointer to the instruction words array.
 *		struct_DC			- Pointer to the directive words array.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Pointer to array of macro pointers (triple pointer).
 * Returns:	void.
 * Side Effects:May realloc the array and write a new record (name + address).
 *		May call error_allocation on memory allocation failure.
 * Errors:	On allocation failure, may set a global error flag and/or leave the
 *		array unchanged.
 */
void add_to_extern_label(extern_label **extern_labels, int* COUNT_OF_EXTERN_LABEL, char *str, SEMEL** SEMELS, int* semel_count , binary_code** array, binary_directive **struct_DC,int macro_count, macro*** macros);

/*
 * --------------
 * Purpose:	Parse a .data directive line, validate a comma-separated list of integers,
 *		and append the encoded values to the directive (DC) segment.
 * Parameters:	row					- Line text containing the .data directive.
 *		struct_DC			- Pointer to the directive words array to append to.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Pointer to array of macro pointers (triple pointer).
 *		semels				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 *		array				- Pointer to the instruction words array.
 *		extern_labels		- Pointer to the extern-usage records array.
 *		count_of_extern_labels
 *							- Pointer to the number of extern-usage records.
 * Returns:	void.
 * Side Effects:Appends words to *struct_DC and advances the global DC counter.
 *		May call error_allocation on memory allocation failure.
 * Errors:	Invalid list syntax, non-numeric items, overflow, or comma rules violations.
 */
void data(char row[], binary_directive **struct_DC,int macro_count,macro*** macros,SEMEL** semels,int* semel_count,binary_code **array, extern_label**extern_labels, int *count_of_extern_labels);

/*
 * ----------------
 * Purpose:	Parse a .string directive, extract the quoted literal, and append its
 *		bytes plus a terminating '\0' to the directive (DC) segment.
 * Parameters:	row					- Line text containing the .string directive.
 *		struct_DC			- Pointer to the directive words array to append to.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Pointer to array of macro pointers (triple pointer).
 *		semels				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 *		array				- Pointer to the instruction words array.
 *		extern_labels		- Pointer to the extern-usage records array.
 *		count_of_extern_labels
 *							- Pointer to the number of extern-usage records.
 * Returns:	void.
 * Side Effects:Appends characters and a terminating zero to *struct_DC; advances DC.
 *		May call error_allocation on memory allocation failure.
 * Errors:	Missing opening/closing quote, illegal characters after the closing quote,
 *		or malformed directive syntax.
 */
void string(char row[], binary_directive **struct_DC,int macro_count,macro*** macros,SEMEL** semels,int* semel_count,binary_code **array, extern_label**extern_labels, int *count_of_extern_labels);

/*
 * -------------
 * Purpose:	Parse a .mat directive describing a matrix: dimensions and optional values.
 *		Validate the format and fill the data segment with either the listed values
 *		or zeros where values are omitted.
 * Parameters:	row					- Line text containing the .mat directive.
 *		struct_DC			- Pointer to the directive words array to append to.
 *		macro_count			- Number of macros in the macro table.
 *		macros				- Pointer to array of macro pointers (triple pointer).
 *		semels				- Pointer to the symbol table.
 *		semel_count			- Pointer to the number of symbols.
 *		array				- Pointer to the instruction words array.
 *		extern_labels		- Pointer to the extern-usage records array.
 *		count_of_extern_labels
 *							- Pointer to the number of extern-usage records.
 * Returns:	void.
 * Side Effects:Appends the encoded matrix contents to *struct_DC and advances DC.
 *		May call error_allocation on memory allocation failure.
 * Errors:	Invalid bracket/comma rules, non-numeric entries, wrong counts, or overflow.
 */
void mat(char row[], binary_directive **struct_DC,int macro_count,macro*** macros,SEMEL** semels,int* semel_count,binary_code **array, extern_label**extern_labels, int *count_of_extern_labels);

/*
 * ----------------------
 * Purpose:	Validate that a .entry or .extern directive line contains exactly one
 *		non-empty parameter (symbol name) and no extra tokens.
 * Parameters:	row					- Line text containing .entry or .extern.
 * Returns:	void.
 * Side Effects:May set a global error flag and print diagnostics if format is invalid.
 * Errors:	Missing symbol, multiple parameters, or trailing junk after the symbol.
 */
void entry_extern(char row[]);

/*
 * -----------------------------
 * Purpose:	Validate a .string directive's punctuation/quoting: ensure one opening and
 *		one closing quote with no non-whitespace characters after the closing quote.
 * Parameters:	row					- Line text to check.
 * Returns:	int					- 1 if the .string formatting is valid, 0 otherwise.
 * Side Effects:None (inspection only).
 * Errors:	Returns 0 when the quoting is malformed or trailing non-space text exists.
 */
int string_commas_check(char* row);

#endif /* SECOND_ROW_ANALYSIS_H */
