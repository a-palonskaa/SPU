#ifndef HASH
#define HASH

#ifdef HASH_PROTECT
#include <stdio.h>
#include "define.h"
#include "stack.h"

#define HASH_SPEC "0x%0X"

void set_stack_hash(my_stack_t* stk);
hash_t stack_hash(my_stack_t* stk, hash_t* new_hash_data, hash_t* new_hash_stack);
hash_t stack_hash_counter(const void* buffer, size_t buffer_bytes_amount);

#endif /* HASH_PROTECT */
#endif /* HASH */
