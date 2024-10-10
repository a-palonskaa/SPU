#ifndef POP_PUSH_H
#define POP_PUSH_H

#include "stack.h"

error_t stack_push(my_stack_t* stk, const void* elm);
error_t stack_pop(my_stack_t* stk, void* elm);

#endif /* POP_PUSH_H */
