#ifndef PROCESSER_H
#define PROCESSER_H

#include <stdio.h>
#include "my_stack.h"
#include "stack.h"
#include "commands.h"
//====================================================================================================

typedef struct {
    unsigned char* code;
    double registres[8];
    double ram[10000];

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
