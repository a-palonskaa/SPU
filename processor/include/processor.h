#ifndef PROCESSER_H
#define PROCESSER_H

#include <stdio.h>
#include "my_stack.h"
#include "stack.h"

//====================================================================================================

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
    CMD_POP   = 19,
    CMD_CALL  = 20,
    CMD_RET   = 21,
    CMD_DROW  = 22,
    CMD_SQR   = 23,
    CMD_POW   = 24,
    CMD_ABS   = 25,
    CMD_LOG   = 26,
} commands_name_t;

typedef struct {
    commands_name_t name;
    size_t args_amount;
} commands_t;

const commands_t commands[] = {{CMD_PUSH,  1},
                               {CMD_ADD,   0},
                               {CMD_SUB,   0},
                               {CMD_MUL,   0},
                               {CMD_DIV,   0},
                               {CMD_OUT,   0},
                               {CMD_IN,    0},
                               {CMD_SQRT,  0},
                               {CMD_SIN,   0},
                               {CMD_COS,   0},
                               {CMD_DUMP,  0},
                               {CMD_HLT,   0},
                               {CMD_JA,    1},
                               {CMD_JAE,   1},
                               {CMD_JB,    1},
                               {CMD_JBE,   1},
                               {CMD_JNE,   1},
                               {CMD_JMP,   1},
                               {CMD_POP,   1},
                               {CMD_CALL,  1},
                               {CMD_RET,   0},
                               {CMD_DROW,  0},
                               {CMD_SQR,   0},
                               {CMD_POW,   0},
                               {CMD_ABS,   0},
                               {CMD_LOG,   0}};

//====================================================================================================

typedef struct {
    unsigned char* code;
    double registres[8];
    double ram[100];

    size_t size;
    size_t ip;

    my_stack_t* stk;
    my_stack_t* addr_stk;
} processor_t;

void processor_ctor(processor_t* processor, size_t code_size);
void processor_dtor(processor_t* processor);

size_t get_code(FILE* istream, processor_t* processor, size_t code_size);
void run(processor_t* processor, FILE* ostream);

//====================================================================================================

void parse_code(processor_t* processor, unsigned char* text, size_t file_size);
ssize_t find_file_size(FILE* istream);
size_t get_double (unsigned char* buffer, double* number);
ssize_t get_int(unsigned char* buffer, unsigned char* number);

#endif /* PROCESSER_H */
