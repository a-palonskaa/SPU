#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include "label.h"

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
    CMD_CALL  = 21,
    CMD_RET   = 22,
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
                               {CMD_POP,   "pop",  1},
                               {CMD_CALL,  "call", 1},
                               {CMD_RET,   "ret",  0}};

const size_t commands_size = sizeof(commands) / sizeof(commands[0]);

//==================================================================================================

typedef enum {
    SYNTAX_ERROR = 0,
    CONTINUE  = 1,
} status_t;

//==================================================================================================

void assemble(FILE* istream, FILE* ostream);
void print_header(FILE* ostream, size_t bytes_cnt);

ssize_t find_file_size(FILE* istream);
size_t assemble_text_format (char* cmd, char* code, size_t bytes_cnt, size_t* size_bytes);

#endif /* ASM_H */
