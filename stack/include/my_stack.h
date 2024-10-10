#ifndef MY_STACK_H
#define MY_STACK_H

#include "stack.h"

#ifdef CANARY_PROTECT
typedef unsigned long long canary_t;
const canary_t LEFT_CANARY_MASK  = 0xC0FFEE;
const canary_t RIGTH_CANARY_MASK = 0xFBADBEEF;
const canary_t DATA_CANARY       = 0x3DAD;
#endif /* CANARY_PROTECT */

struct my_stack_t{
#ifdef CANARY_PROTECT
    canary_t left_canary;
#endif /* CANARY_PROTECT */
    void* data;
    size_t elm_width;
    size_t size;
    size_t capacity;
    size_t base_capacity;
    error_t error;
    uint64_t poison_value;
    char* poison_value_buffer;
    print_t print;
#ifdef __APPLE__
    ptr_protect_t data_permissions;
#endif /* __APPLE__ */
#ifdef DEBUG
    location_info_t location_info;
#endif /* DEBUG */
#ifdef CANARY_PROTECT
    canary_t rigth_canary;
#endif /* CANARY_PROTECT */
#ifdef HASH_PROTECT
    hash_t hash_stack;
    hash_t hash_data;
#endif /* HASH_PROTECT */
};

#endif /* MY_STACK_H */
