#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include "label.h"
#include "text_lib.h"
#include "vector.h"

//==================================================================================================

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

status_t check_for_unused_parameters(unsigned char* cmd);
void printf_diagnostic(unsigned char* buffer, size_t line_num, unsigned char* comment_ptr, unsigned char* error,
                       const char* file, size_t line, const char* func, const char* message);

#endif /* ASM_H */
