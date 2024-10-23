#ifndef PROCESSER_H
#define PROCESSER_H

#include <stdio.h>
#include "my_stack.h"
#include "stack.h"
#include "commands.h"

//====================================================================================================

typedef struct {
    unsigned char* code;
    double registres[9];
    double ram[10000];

    size_t size;
    size_t ip;

    my_stack_t* stk;
    my_stack_t* addr_stk;
} processor_t;

void processor_ctor(processor_t* processor, size_t code_size);
void processor_dtor(processor_t* processor);

size_t get_code(FILE* istream, processor_t* processor, size_t code_size);
bool run(processor_t* processor, FILE* ostream) ;

#endif /* PROCESSER_H */
