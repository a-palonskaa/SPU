#include <stdio.h>
#include "printers.h"

void print_int(void* elm, FILE* ostream) {
    int* element = (int*) elm;
    fprintf(ostream, "%d", *element);
}

void print_10bytes(void* elm, FILE* ostream) {
    unsigned char* element = (unsigned char*) elm;
    for (ssize_t j = 9; j >= 0; j--) {
        fprintf(ostream, "%.2x ", *(element + j));
    }
}
