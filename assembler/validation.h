#ifndef VALIDATION_H
#define VALIDATION_H
#include "assembler.h"

/*
 * ---------------------------------------
 * Checks for comma-related syntax errors in an assembly line, with support for
 * special formatting exceptions used in the `.mat` directive.
 *
 * The function identifies the following types of errors:
 * - Unexpected commas.
 * - A trailing comma at the end of the line.
 * - A trailing comma at the start of the line.
 * - Unexpected commas after command 
 *
 * The function does not modify the input line. It prints error messages to stderr
 * for each issue found.
 *
 * Parameters:
 *   line    - A null-terminated string containing the line to check.
 *   is_mat  - A flag indicating whether the line is a `.mat` directive.
 *             If non-zero, the function ignores missing commas after square brackets.
 *
 * Returns:
 *   1 if no syntax errors were found,
 *   0 if any syntax errors were detected.
 */
int check_commas( char *line);

/*
 * ---------------------------------------
 * Updates the addresses of all data symbols in the symbol table.
 * Adds IC to each data symbol's address.
 * Also checks for .entry symbols that were not defined in the code
 * and reports an error if found (ex_en == 0 && address == 3).
 *
 * Parameters:
 *   semels - array of pointers to symbol entries (SEMEL structs)
 *   semel_count - pointer to the number of symbols in the table
 *
 * Returns:
 *   void
 */
void update_data_symbol_addresses(SEMEL** semels, int* semel_count);

/*
 * ---------------------------------------
 * Handles memory deallocation and cleanup when a memory allocation error occurs.
 * Frees all dynamically allocated memory structures to prevent memory leaks
 * before the program terminates.
 *
 * This function is called when malloc/realloc fails during program execution.
 * It performs a complete cleanup of all allocated data structures including:
 * - Macro definitions and their contents
 * - Symbol table entries and their names
 * - Binary code arrays
 * - Directive structures
 * - External label lists
 *
 * Parameters:
 *   macro_count        - number of macros currently allocated
 *   macros            - pointer to array of macro pointers (passed by reference)
 *   semels            - array of symbol table entries
 *   semel_count       - pointer to count of symbol table entries
 *   array             - pointer to binary code array
 *   struct_DC         - pointer to binary directive array
 *   extern_labels     - pointer to external labels array
 *   count_of_extern_labels - pointer to count of external labels
 *
 * Returns:
 *   void
 *
 * Note:
 *   This function terminates the program with exit() after cleanup.
 *   It should only be called in error conditions where recovery is not possible.
 */
void error_allocation(int macro_count, macro*** macros, SEMEL** semels, int* semel_count, binary_code **array, binary_directive **struct_DC, extern_label**extern_labels, int *count_of_extern_labels);

#endif
