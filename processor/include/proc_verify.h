#ifndef VERIFY_H
#define VERIFY_H

#include <stdio.h>
#include "processor.h"

typedef enum {
    VALID_HEADER          = 0x0001,
    VALID_PROCESSOR       = 0x0002,
    UNKNOWN_CREATOR       = 0x0004,
    INAPPROPRIATE_VERSION = 0x0008,
    NULL_REGISTRES_PTR    = 0x0010,
    NULL_CODE_PTR         = 0x0020,
    NULl_STACK_PTR        = 0x0040,
    EMPTY_CODE            = 0x0080,
    INVALID_STACK         = 0x0100,
} verify_t;

verify_t verify_file(FILE* istream, size_t* bytes_cnt);
const char* str_error_status(verify_t error_status);
void processor_dump(FILE* ostream, processor_t* processor, size_t id);

#endif /* VERIFY_H */
