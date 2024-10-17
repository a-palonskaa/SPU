#ifndef ASM_H
#define ASM_H

#include <stdio.h>

typedef enum {
    CMD_PUSH  = 1,
    CMD_ADD   = 2,
    CMD_SUB   = 3,
    CMD_MUL   = 4,
    CMD_DIV   = 5,
    CMD_OUT   = 6,
    CMD_IN    = 7,
    CMD_SQRT  = 8,
    CMD_SIN   = 9,
    CMD_COS   = 10,
    CMD_DUMP  = 11,
    CMD_HLT   = 12,
    CMD_JA    = 13,
    CMD_JAE   = 14,
    CMD_JB    = 15,
    CMD_JBE   = 16,
    CMD_JNE   = 17,
    CMD_JMP   = 18,
    CMD_PUSHR = 19,
    CMD_POP   = 20,
} commands_name_t;

typedef struct {
    commands_name_t name;
    const char* alias;
    size_t args_amount;
} commands_t;

const commands_t commands[] = {{CMD_PUSH,  "push", 1},
                               {CMD_ADD,   "add",  0},
                               {CMD_SUB,   "sub",  0},
                               {CMD_MUL,   "mul",  0},
                               {CMD_DIV,   "div",  0},
                               {CMD_OUT,   "out",  0},
                               {CMD_IN,    "in",   0},
                               {CMD_SQRT,  "sqrt", 0},
                               {CMD_SIN,   "sin",  0},
                               {CMD_COS,   "cos",  0},
                               {CMD_DUMP,  "dump", 0},
                               {CMD_HLT,   "hlt",  0},
                               {CMD_JA,    "ja",   1},
                               {CMD_JAE,   "jae",  1},
                               {CMD_JB,    "jb",   1},
                               {CMD_JBE,   "jbe",  1},
                               {CMD_JNE,   "jne",  1},
                               {CMD_JMP,   "jmp",  1},
                               {CMD_PUSHR, "push", 1},
                               {CMD_POP,   "pop", 1}};

const size_t commands_size = sizeof(commands) / sizeof(commands[0]);

void assemble(FILE* istream, FILE* ostream);
ssize_t find_file_size(FILE* istream);
void print_header(FILE* ostream, size_t bytes_cnt);

#endif /* ASM_H */
