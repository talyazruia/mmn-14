#include "assembler.h"

int has_two_square_bracket_pairs(const char* str) {
    int i = 0;
    int bracket_count = 0;
    int inside = 0; 

    while (str[i] != '\0') {
        if (str[i] == '[') {
            if (inside) return 0; 
            inside = 1;
            bracket_count++;
        }
        else if (str[i] == ']') {
            if (!inside) return 0; 
            inside = 0;
        }
        i++;
    }

    return (bracket_count == 2) && (inside == 0);
}

void ic_count_1_arg(char* row) {
     char* command;
    char* op1;
    command = strtok(row, " ");
    op1 = strtok(NULL, " ");
    if (op1 == NULL) {
        error = 1;
        IC += 1;
    }

    if (has_two_square_bracket_pairs(op1))
        IC += 3;
    else
        IC += 2; 
}

void ic_count_2_arg(char* row) {
    const char* command;
    const char* op1;
    const char* op2;
    command = strtok(row, " ");
    op1 = strtok(NULL, ",");
    op2 = strtok(NULL, ",");
    if (op1 == NULL) {
        error = 1;
        IC += 1;
    }
    if (has_two_square_bracket_pairs(op1))
        IC += 3;
    else
        IC += 2;
    if (has_two_square_bracket_pairs(op2))
        IC += 2;
    else
        IC += 1;
    if (isHashNumber(op1) && isHashNumber(op2))
        IC--;
    if (regisrer(op1) || regisrer(op2))
        IC--;
}

void dc_count_data(char* row) {
    char* command;
    char* op1;
    command = strtok(row, " ");
    op1 = strtok(NULL, ",");
    while (op1 != NULL) {
        DC++;
        op1 = strtok(NULL, " ,");
    }
}

void dc_count_string(char* row) {
     char* command;
     char* op1;
    command = strtok(row, " ");
    op1 = strtok(NULL, " ");
    DC = strlen(op1) + 1;
}

void dc_count_mat(char* row) {
    char* command;
    command = strchr(row, ']');
    command = strchr(command + 1, ']');
    command++;
    command = strtok(command, ",");
    while (command != NULL) {
        DC++;
        command = strtok(NULL, " ,");
    }
}

