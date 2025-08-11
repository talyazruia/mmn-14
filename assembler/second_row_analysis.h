#ifndef SECOND_ROW_ANALYSIS_H
#define SECOND_ROW_ANALYSIS_H

#include "assembler.h"

/* Second pass: parse lines, encode instructions/directives, record extern/entry. */
void second_row_analysis(FILE *f, command cmd[], SEMEL ***SEMELS, int *semel_count, binary_code **array, binary_directive **struct_DC, extern_label **extern_labels, int *count_of_extern_labels);

/* Parse one instruction line, validate operands, emit words; record extern uses. */
int IC_command_analysis(char row[], SEMEL **SEMELS, int *semel_count, command cmd[], binary_code **array, extern_label **extern_labels, int *COUNT_OF_EXTERN_LABEL);

/* Validate integer text by mode: 0=#immediate, 1=signed, 2=positive-only. */
int is_valid_number(char *str, int mode);

/* Return 1 if token is a register r0..r7 (ignores surrounding spaces). */
int reg(char *str);

/* Validate matrix operand: name[rX][rY]; return [idx,rX,rY] or NULL. */
int *valid_matrix(char *str, SEMEL **SEMELS, int *semel_count);

/* Find symbol by name and ensure it is allowed as operand; return index or -1. */
int valid_SEMEL(char *str, SEMEL **SEMELS, int *semel_count);

/* Append extern reference at current IC; alloc/realloc array as needed. */
void add_to_extern_label(extern_label **extern_labels, int *COUNT_OF_EXTERN_LABEL, char *str);

/* Parse .data list and append numbers to directive segment. */
void data(char row[], binary_directive **struct_DC);

/* Parse .string and append chars plus '\0' to directive segment. */
void string(char row[], binary_directive **struct_DC);

/* Parse .mat dimensions/values; fill zeros or listed values into data segment. */
void mat(char row[], binary_directive **struct_DC);

/* Validate .entry/.extern has exactly one parameter (no extras). */
void entry_extern(char row[]);

/* Validate .string line: opening/closing quotes and nothing after closing quote. */
int string_commas_check(char* row);

#endif

