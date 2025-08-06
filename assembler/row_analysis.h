#ifndef ROW_ANALYSIS_H
#define ROW_ANALYSIS_H

#include "assembler.h"

/* Increments IC based on one-operand instruction */
void ic_count_1_arg(char* row);

/* Increments IC based on two-operand instruction */
int ic_count_2_arg(char* row);

/* Increments DC for each number in a .data directive */
int dc_count_data(char* row);

/* Increments DC based on the length of a string in a .string directive */
int dc_count_string(char* row);

/* Increments DC based on the size and values of a matrix in a .mat directive */
void dc_count_mat(char* row);

/* Parses and analyzes each line of the input file (first pass) */
void row_analysis(FILE *f, int macro_count, macro **macros, command cmd[], command1 cmd1[], SEMEL ***SEMELS, int *semel_count );

/* Clears input buffer when line is too long */
void clear_row_arry();

/* Adds a new SEMEL (symbol) to the SEMELS array if valid */
void add_SEMEL(char* label, int type, int addres, SEMEL*** semels, int* semel_count, int ex_en);

/* Checks whether a label has a valid format (starts with letter, contains only alphanumerics, up to 30 characters) */
int is_valid_label_format(const char* label);

/* Validates if a SEMEL (symbol) exists and returns its index or -1 if not found */
int valid_SEMEL(char* str, SEMEL** SEMELS, int* semel_count);

/* Checks if a string contains exactly two valid pairs of square brackets */
int has_two_square_bracket_pairs(char* str);

#endif

