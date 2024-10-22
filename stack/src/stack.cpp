#include "stack.h"
#include "verify.h"
#include "define.h"
#include "hash.h"
#include "my_stack.h"

static void fill_poison_value(my_stack_t* stk);
static void set_poison_value(my_stack_t* stk, char* buffer);

error_t stack_ctor(my_stack_t* stk, size_t elm_size, size_t base_capacity, print_t print
                   ON_DEBUG(, location_info_t location_info)) {
    assert(stk != nullptr);

    if (stack_error(stk) == NO_ERRORS) {
        ON_DEBUG(STACK_DUMP_(stk);)
        return STACK_ALREADY_INITIALIZED_ERROR;
    }
    stk->error = NO_ERRORS;

#ifdef DEBUG
    stk->location_info.stack_name = location_info.stack_name;
    stk->location_info.file = location_info.file;
    stk->location_info.line = location_info.line;
    stk->location_info.func = location_info.func;
#endif /* DEBUG */

#ifdef CANARY_PROTECT
    stk->left_canary  = LEFT_CANARY_MASK  ^ (canary_t) stk;
    stk->rigth_canary = RIGTH_CANARY_MASK ^ (canary_t) stk;
#endif /* CANARY_PROTECT */

    if (!elm_size) {
        stk->error = ELEMENT_WIDTH_ERROR;
        ON_DEBUG(STACK_DUMP_(stk);)
        return ELEMENT_WIDTH_ERROR;
    }
    stk->elm_width = elm_size;

    stk->poison_value = POISON_VALUE; // FIXME: remove field, extra memory
    char* buffer = (char*) calloc(stk->elm_width, sizeof(char));
    if (!buffer) {
        stk->error = MEMORY_ALLOCATION_ERROR;
        ON_DEBUG(STACK_DUMP_(stk);)
        return MEMORY_ALLOCATION_ERROR;
    }
    set_poison_value(stk, buffer);
    stk->poison_value_buffer = buffer;

    STATIC_ASSERT(MIN_CAPACITY > 0, Minimum_capacity_should_be_greater_than_0);
    stk->capacity = base_capacity > MIN_CAPACITY ?
                    base_capacity : MIN_CAPACITY;
    stk->base_capacity = stk->capacity;

    stk->print = print;
#ifdef CANARY_PROTECT
    if (!(stk->data = calloc(stk->capacity * stk->elm_width + 2 * sizeof(canary_t), sizeof(char)))) {
        stk->error = MEMORY_ALLOCATION_ERROR;
        ON_DEBUG(STACK_DUMP_(stk);)
        return MEMORY_ALLOCATION_ERROR;
    };

    memcpy(stk->data, &DATA_CANARY, sizeof(canary_t));
    stk->data = (void*) ((char*) stk->data + sizeof(canary_t));
    memcpy((char*) stk->data + stk->capacity * stk->elm_width, &DATA_CANARY, sizeof(canary_t));

#else
    if (!(stk->data = calloc(stk->capacity, stk->elm_width))) {
        stk->error = MEMORY_ALLOCATION_ERROR;
        ON_DEBUG(STACK_DUMP_(stk);)
        return MEMORY_ALLOCATION_ERROR;
    };
#endif /* CANARY_PROTECT */
    fill_poison_value(stk);

    ON_HASH_PROTECT(set_stack_hash(stk);)
    STACK_ASSERT_(stk);
    ON_DEBUG(STACK_DUMP_(stk);)
    return NO_ERRORS;
}

//------------------------------------------------------------------------------------------------

error_t stack_dtor(my_stack_t* stk) {
    assert(stk != nullptr);
#ifdef CANARY_PROTECT
    free((char*) stk->data - sizeof(canary_t));
#else
    free(stk->data);
#endif /* CANARY_PROTECT */
    free(stk->poison_value_buffer);
    stk->poison_value_buffer = nullptr;
    stk->data = nullptr;
    stk->size = 0;
    stk->elm_width = 0;
    stk->capacity = 0;
    stk->error = NO_ERRORS;

#ifdef HASH_PROTECT
    stk->hash_stack = 0;
    stk->hash_data  = 0;
#endif /* HASH_PROTECT */

    ON_DEBUG(STACK_DUMP_(stk);)
    return NO_ERRORS;
}

error_t stack_resize(my_stack_t* stk, mem_modify_t mode) {
    assert(stk != nullptr);
    STACK_ASSERT_(stk);

    if (mode == EXPAND) {
        stk->capacity = stk->capacity * CAPACITY_COEFF;
    }
    else if (mode == SQUEEZE){
        stk->capacity = stk->capacity / CAPACITY_COEFF;
    }
    else {
        stk->capacity = stk->size;
    }
#ifdef CANARY_PROTECT
    void* new_data_ptr = realloc((char*) stk->data - sizeof(canary_t),
                                  stk->capacity * stk->elm_width + 2 * sizeof(canary_t));
    if (!new_data_ptr) {
        stk->error = MEMORY_REALLOCATION_ERROR;
        return MEMORY_REALLOCATION_ERROR;
    }

    memcpy(new_data_ptr, &DATA_CANARY, sizeof(canary_t));
    stk->data = (void*) ((char*) new_data_ptr + sizeof(canary_t));
    memcpy((char*) stk->data + stk->capacity * stk->elm_width, &DATA_CANARY, sizeof(canary_t));
#else
    void* new_data_ptr = realloc(stk->data, stk->capacity * stk->elm_width);
    if (!new_data_ptr) {
        stk->error = MEMORY_REALLOCATION_ERROR;
        return MEMORY_REALLOCATION_ERROR;
    }
    stk->data = new_data_ptr;
#endif /* CANARY_PROTECT */
    fill_poison_value(stk);
    return NO_ERRORS;
}

//------------------------------------------------------------------------------------------------

static void fill_poison_value(my_stack_t* stk) {
    assert(stk != nullptr);
    for (size_t i = stk->size; i < stk->capacity; i++) {
        memcpy ((char*) stk->data + i * stk->elm_width, stk->poison_value_buffer, stk->elm_width);
    }
}

static void set_poison_value(my_stack_t* stk, char* buffer) {
    if (stk->elm_width <= sizeof(stk->poison_value)) {
        memcpy(buffer, &stk->poison_value, stk->elm_width);
    }
    else {
        size_t j = 0;
        for (; j < stk->elm_width - sizeof(stk->poison_value); j += sizeof(stk->poison_value)) {
            memcpy(buffer + j, &stk->poison_value, sizeof(stk->poison_value));
        }
        memcpy(buffer + j, &stk->poison_value, stk->elm_width - j);
    }
}

//------------------------------------------------------------------------------------------------

my_stack_t* new_stack(size_t elm_size, size_t base_capacity, print_t print
                   ON_DEBUG(, location_info_t location_info)) {
    my_stack_t* stack = (my_stack_t*) calloc(sizeof(my_stack_t), sizeof(char));

    if (stack_ctor(stack, elm_size, base_capacity, print ON_DEBUG(, location_info)) != NO_ERRORS) {
        return nullptr;
    }

    return stack;
}

void delete_stack(my_stack_t* stk) {
    stack_dtor(stk);
    free(stk);
    stk = nullptr;
}
