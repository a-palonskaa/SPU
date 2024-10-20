#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include "label.h"
#include "text_lib.h"
#include "vector.h"
//==================================================================================================

typedef unsigned char command;
typedef double imm;
typedef unsigned char reg;
typedef size_t addr;

typedef enum {
    ASM_NO_ERRORS = 0,
    ASM_MEM_ERROR = 1,
} asm_error_t;

typedef struct {
    text_t text;
    vector_t* name_table;
    vector_t* fixup;
    unsigned char* code;
    size_t bytes_cnt;
} assembler_t;

typedef enum {
    CORRECT      = 0,
    SYNTAX_ERROR = 1,
} status_t;

//==================================================================================================

asm_error_t assembler_ctor(assembler_t* assm, FILE* istream);
void assembler_dtor(assembler_t* assm);
void assemble(FILE* istream, FILE* ostream);

#endif /* ASM_H */
