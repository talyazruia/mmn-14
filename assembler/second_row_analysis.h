#ifndef SECOND_ROW_ANALYSIS_H
#define SECOND_ROW_ANALYSIS_H

#include "assembler.h"

/* 
 * Performs the second pass over the assembly source file.
 * Handles instruction and directive encoding, entry/extern handling.
 */
void second_row_analysis(FILE *f, command cmd[], SEMEL ***SEMELS, int *semel_count, binary_code **array, binary_directive **struct_DC, extern_label **extern_labels, int *count_of_extern_labels);

/*
 * Parses and encodes an instruction line into binary format.
 * Validates operands and adds any relevant external labels.
 * Returns 0 if failed, 1 if successful.
 */
int IC_command_analysis(char row[], SEMEL **SEMELS, int *semel_count, command cmd[],
                        binary_code **array, extern_label **extern_labels,
                        int *COUNT_OF_EXTERN_LABEL);

/*
 * Checks if a string represents a valid integer (with optional #, +, -).
 * Mode: 0 = immediate with #, 1 = normal integer, 2 = positive only.
 * Returns 1 if valid, 0 otherwise.
 */
int is_valid_number(char *str, int mode);

/*
 * Checks if a string is a valid register name ("r0" to "r7").
 * Returns 1 if valid, 0 otherwise.
 */
int reg(char *str);

/*
 * Validates a matrix operand (e.g., mat[r1][r2]).
 * Returns array of 3 ints: [symbol index, r1, r2] if valid, NULL otherwise.
 */
int *valid_matrix(char *str, SEMEL **SEMELS, int *semel_count);

/*
 * Validates that a symbol exists in the SEMELS table and is not .entry.
 * Returns the index if valid, -1 otherwise.
 */
int valid_SEMEL(char *str, SEMEL **SEMELS, int *semel_count);

/*
 * Adds an external symbol to the extern_labels table using the current IC.
 */
void add_to_extern_label(extern_label **extern_labels, int *COUNT_OF_EXTERN_LABEL, char *str);

/*
 * Processes a .data directive and encodes the numbers into the directive segment.
 */
void data(char row[], binary_directive **struct_DC);

/*
 * Processes a .string directive and encodes each character and null terminator.
 */
void string(char row[], binary_directive **struct_DC);

/*
 * Processes a .mat directive and encodes the matrix dimensions and values.
 */
void mat(char row[], binary_directive **struct_DC);

/*
 * Validates that .entry or .extern has only one parameter.
 */
void entry_extern(char row[]);

#endif

