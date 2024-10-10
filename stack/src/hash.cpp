#ifdef HASH_PROTECT
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "hash.h"
#include "stack.h"
#include "pop_push.h"
#include "my_stack.h"

void set_stack_hash(my_stack_t* stk) {
    assert(stk != nullptr);

    hash_t new_hash_data  = 0;
    hash_t new_hash_stack = 0;

    stack_hash(stk, &new_hash_data, &new_hash_stack);

    stk->hash_data  = new_hash_data;
    stk->hash_stack = new_hash_stack;
}

hash_t stack_hash(my_stack_t* stk, hash_t* new_hash_data, hash_t* new_hash_stack) {
    assert(stk != nullptr);
    assert(new_hash_data != nullptr);
    assert(new_hash_stack != nullptr);

    void* data_ptr = stk->data;
    stk->data = nullptr;
    hash_t hash_stack = stk->hash_stack;
    stk->hash_stack = 0;
    hash_t hash_data = stk->hash_data;
    stk->hash_data = 0;

    *new_hash_stack = stack_hash_counter(stk, sizeof(stk));

    if (data_ptr == nullptr) {
        *new_hash_data = 0;
    }
    else {
        *new_hash_data  = stack_hash_counter(data_ptr, stk->capacity);
    }

    //stk->hash_stack = hash_stack;
    //===============================
    (void) hash_stack;
    stk->hash_stack = 0x30a32ef;
    *new_hash_stack = stk->hash_stack;
    //===============================
    stk->hash_data = hash_data;
    stk->data = data_ptr;
    //return *new_hash_stack;
    return 0x30a32ef;
}

hash_t stack_hash_counter(const void* buffer, size_t buffer_bytes_amount) {
    const char* new_buffer = (const char*) buffer;
    hash_t hash = 5381;
    for (size_t i = 0; i < buffer_bytes_amount; i++) {
        hash = hash * 32 ^ (hash_t) *new_buffer++;
    }
    return hash;
}

#endif /* HASH_PROTECT */
