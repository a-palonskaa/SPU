#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "define.h"

//------------------------------------------------------------------------------------------------

ON_HASH_PROTECT(typedef uint32_t hash_t;)
typedef void (*print_t) (void* elm, FILE* ostream);

struct my_stack_t;
const size_t CAPACITY_COEFF = 2;
const size_t MIN_CAPACITY = 1;
const size_t MAX_CAPACITY = 1e6;
const uint64_t POISON_VALUE = 0xDEADDEADDEADDEAD;

//------------------------------------------------------------------------------------------------

typedef enum {
    NO_ERRORS                       = 0x00000000,
    MEMORY_ALLOCATION_ERROR         = 0x00000001,
    MEMORY_REALLOCATION_ERROR       = 0x00000002,
    ELEMENT_WIDTH_ERROR             = 0x00000004,
    NO_ELEMNTS_TO_POP_ERROR         = 0x00000010,
    NULL_CAPACITY_ERROR             = 0x00000020,
    INVALID_SIZE_ERROR              = 0x00000040,
    DATA_INVALID_POINTER_ERROR      = 0x00000100,
    NULL_ELEMENT_WIDTH_ERROR        = 0x00000200,
    STACK_ALREADY_INITIALIZED_ERROR = 0x00000400,
    CAPACITY_LIMIT_EXCEED_ERROR     = 0x00001000,
    NON_VALID_POINTER_ERROR         = 0x00002000,

#ifdef CANARY_PROTECT
    LEFT_CANARY_PROTECT_FAILURE     = 0x00004000,
    RIGTH_CANARY_PROTECT_FAILURE    = 0x00010000,
    DATA_CANARY_PROTECT_FAILURE     = 0x00020000,
#endif /* CANARY_PROTECT */

#ifdef HASH_PROTECT
    HASH_PROTECTION_FAILED          = 0x00040000,
#endif /* HASH_PROTECT */
} error_t;

typedef enum {
    NONE    = 0x00,
    READ    = 0x01,
    WRITE   = 0x02,
    EXECUTE = 0x04,
    DEFAULT = READ | WRITE,
    ALL = READ | WRITE | EXECUTE,
} ptr_protect_t;

typedef enum {
    EXPAND       = 0,
    SQUEEZE      = 1,
    SHINK_TO_FIT = 2,

} mem_modify_t;

//------------------------------------------------------------------------------------------------

#ifdef DEBUG
    typedef struct {
        const char* stack_name;
        const char* file;
        size_t line;
        const char* func;
    } location_info_t;
#endif /* DEBUG */

//------------------------------------------------------------------------------------------------

my_stack_t* new_stack(size_t elm_size, size_t base_capacity, print_t print
                   ON_DEBUG(, location_info_t location_info));

void delete_stack(my_stack_t* stk);
//------------------------------------------------------------------------------------------------

error_t stack_ctor(my_stack_t* stk, size_t elm_size, size_t base_capacity, print_t print
                   ON_DEBUG(, location_info_t location_info));
error_t stack_dtor(my_stack_t* stk);
error_t stack_resize(my_stack_t* stk, mem_modify_t mode);

#endif /* STACK_H */

