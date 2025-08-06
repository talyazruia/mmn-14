#ifndef VALIDATION_H
#define VALIDATION_H

#include "assembler.h"

/*
 * Fixes the placement of commas in a given line.
 * Removes unexpected commas (e.g., double commas, comma at start/end),
 * and formats the operands correctly with single commas and spaces.
 * If errors are found, sets the global 'error' flag and reports the issue.
 *
 * Parameters:
 *   line - the input string to be validated and cleaned in place.
 */
void fix_commas_in_place(char *line);

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

