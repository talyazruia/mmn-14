#ifndef ENCODING_H
#define ENCODING_H

#include "assembler.h"

/* Converts an instruction or data structure to base-4 representation and writes to output file.
 * struct_type:
 *   1 = instruction array
 *   2 = directive array
 *   3 = extern label table
 *   4 = entry symbol table
 *   5 = IC and DC values
 */
void BinaryToBase4(void** array, int argc, char *argv[], int i, FILE* f2, int struct_type, int* semel_count, int ic);

/* Converts two bytes (a and b) into a base-4 encoded string of 5 characters + null terminator.
 * The result must be a preallocated buffer of at least 6 bytes.
 */
char *base4_convert(char a, char b, char* result);

/* Adds a single number to the appropriate array (instruction or directive), encoded with ARE value.
 * - num: number to encode.
 * - array: pointer to the target array.
 * - type: TYPE_INSTRUCTION or TYPE_DIRECTIVE.
 * - flag: ARE value (0-3).
 */
void add_number(int num, void** array, array_type type, int flag);

/* Encodes two numbers into a single binary instruction and appends to the instruction array. */
void add_two_numbers(int num1, int num2, binary_code** array);

/* Encodes opcode and operand types into a single binary instruction and appends to instruction array. */
void to_binary(int opcode, int op1, int op2, binary_code** array);

#endif

