#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "my_stack.h"
#include "pop_push.h"
#include "define.h"
#include "verify.h"
#include "hash.h"

error_t stack_push(my_stack_t* stk, const void* elm) {
    assert(stk != nullptr);
    assert(elm != nullptr);
    STACK_ASSERT_(stk);

    if (stk->size == stk->capacity) {
        if (stk->capacity * CAPACITY_COEFF >= MAX_CAPACITY) {
            stk->error = CAPACITY_LIMIT_EXCEED_ERROR;
            ON_DEBUG(STACK_DUMP_(stk);)
            return CAPACITY_LIMIT_EXCEED_ERROR;
        }
        if (stack_resize(stk, EXPAND) != NO_ERRORS) {
            ON_DEBUG(STACK_DUMP_(stk);)
            return stk->error;
        }
    }

    memcpy(((char*) stk->data + stk->size * stk->elm_width), elm, stk->elm_width);
    stk->size++;

    ON_HASH_PROTECT(set_stack_hash(stk);)

    ON_DEBUG(STACK_DUMP_(stk);)
    return NO_ERRORS;
}

//------------------------------------------------------------------------------------------------

error_t stack_pop(my_stack_t* stk, void* elm) {
    assert(stk != nullptr);
    assert(elm != nullptr);
    STACK_ASSERT_(stk);

    if (stk->size == 0)  {
        stk->error = NO_ELEMNTS_TO_POP_ERROR;
        ON_DEBUG(STACK_DUMP_(stk);)
        return NO_ELEMNTS_TO_POP_ERROR;
    }

    void* pointer = stk->data;
    memcpy(elm, (char*) pointer + (stk->size - 1) * stk->elm_width, stk->elm_width);
    memcpy((char*) pointer + (stk->size - 1) * stk->elm_width, stk->poison_value_buffer, stk->elm_width);
    stk->size--;

    size_t new_capacity = stk->capacity / (CAPACITY_COEFF * CAPACITY_COEFF);

    //============================
    // stk->size = 0;
    // stk->base_capacity = 0;
    //============================

    if (stk->size <= new_capacity && new_capacity > stk->base_capacity) {
        if (stack_resize(stk, SQUEEZE) != NO_ERRORS) {
            ON_DEBUG(STACK_DUMP_(stk);)
            return stk->error;
        }
    }

    ON_HASH_PROTECT(set_stack_hash(stk);)
    ON_DEBUG(STACK_DUMP_(stk);)
    return NO_ERRORS;
}
