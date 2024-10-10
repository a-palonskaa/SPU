#ifndef PROCESSER_H
#define PROCESSER_H

#include <stdio.h>

typedef enum {
    CMD_PUSH = 1,
    CMD_ADD  = 2,
    CMD_SUB  = 3,
    CMD_MUL  = 4,
    CMD_DIV  = 5,
    CMD_OUT  = 6,
    CMD_IN   = 7,
    CMD_SQRT = 8,
    CMD_SIN  = 9,
    CMD_COS  = 10,
    CMD_DUMP = 11,
    CMD_HLT  = 12,
} commands_name_t;

typedef struct {
    commands_name_t name;
    size_t args_amount;
} commands_t;

const commands_t commands[] = {{CMD_PUSH, 1},
                               {CMD_ADD,  0},
                               {CMD_SUB,  0},
                               {CMD_MUL,  0},
                               {CMD_DIV,  0},
                               {CMD_OUT,  0},
                               {CMD_IN,   1},
                               {CMD_SQRT, 0},
                               {CMD_SIN,  0},
                               {CMD_COS,  0},
                               {CMD_DUMP, 0},
                               {CMD_HLT,  0}};

typedef struct {
    unsigned char* code;
    size_t size;
} processor_t;

void run(processor_t* processor);
size_t get_code(FILE* istream, processor_t* processor);
ssize_t find_file_size(FILE* istream);
void parse_code(processor_t* processor, unsigned char* text, size_t file_size);
size_t get_double (unsigned char* buffer, double* number);
ssize_t get_int(unsigned char* buffer, unsigned char* number);

#endif /* PROCESSER_H */
