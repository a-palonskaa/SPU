#ifndef VERIFY_H
#define VERIFY_H

#include <stdio.h>
#include "processor.h"

typedef enum {
    VALID_HEADER          = 1 << 1, //0x0001
    VALID_PROCESSOR       = 1 << 2, //0x0002
    UNKNOWN_CREATOR       = 1 << 3, //0x0004
    INAPPROPRIATE_VERSION = 1 << 4, //0x0008
    NULL_REGISTRES_PTR    = 1 << 5, //0x0010
    NULL_CODE_PTR         = 1 << 6, //0x0020
    NULl_STACK_PTR        = 1 << 7, //0x0040
    EMPTY_CODE            = 1 << 8, //0x0080
    INVALID_STACK         = 1 << 9, //0x0100
} verify_t;

verify_t verify_file(FILE* istream, size_t* bytes_cnt);
void processor_dump(FILE* ostream, processor_t* processor, const char* file, size_t line, const char* func);
const char* str_error_status(verify_t error_status);

#endif /* VERIFY_H */
