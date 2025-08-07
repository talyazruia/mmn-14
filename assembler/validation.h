#ifndef VALIDATION_H
#define VALIDATION_H

#include "assembler.h"

/*
 * Function: check_commas
 * ---------------------------------------
 * Checks for comma-related syntax errors in an assembly line, with support for
 * special formatting exceptions used in the `.mat` directive.
 *
 * The function identifies the following types of errors:
 *   - Unexpected commas (e.g., at the start or multiple consecutive commas).
 *   - Missing commas between operands (unless explicitly allowed for `.mat`).
 *   - A trailing comma at the end of the line.
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
int check_commas(const char *line);


/*
 * Updates the addresses of all data symbols in the symbol table.
 * Adds IC to each data symbol's address.
 * Also checks for .entry symbols that were not defined in the code
 * and reports an error if found (ex_en == 0 && address == 3).
 *
 * Parameters:
 *   semels - array of pointers to symbol entries (SEMEL structs)
 *   semel_count - pointer to the number of symbols in the table
 */
void update_data_symbol_addresses(SEMEL** semels, int* semel_count);

#endif

