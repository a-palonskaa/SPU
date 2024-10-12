#ifndef ASM_H
#define ASM_H

#include <stdio.h>

void assemble(FILE* istream, FILE* ostream);
ssize_t find_file_size(FILE* istream);
void print_header(FILE* ostream, size_t bytes_cnt);

#endif /* ASM_H */
