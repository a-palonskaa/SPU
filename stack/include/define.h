#ifndef DEFINE_H
#define DEFINE_H

//------------------------------------------------------------------------------------------------

#define STATIC_ASSERT(COND,MSG)                           \
    do                                                    \
    {                                                     \
        typedef char static_assertion_##MSG[(COND)?1:-1]; \
        static_assertion_##MSG x = {};                    \
        (void) x;                                         \
    } while (0)

#define _POS __FILE__, __LINE__, __FUNCTION__

#define CASE_(stk)   \
    case stk:        \
        return #stk; \
        break

//------------------------------------------------------------------------------------------------

#define STACK_DUMP_(stk)     \
        stack_dump(stk, _POS)

#define STACK_ASSERT_(stk)      \
        stack_assert(stk, _POS)

#define STACK_CTOR_(stk, size, capacity, print)                         \
        stack_ctor(stk, size, capacity, print ON_DEBUG(,{#stk, _POS}))

#define NEW_STACK_(size, capacity, print)                        \
        new_stack(size, capacity, print ON_DEBUG(,{"stk", _POS}));
//------------------------------------------------------------------------------------------------

#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif /* DEBUG */

#ifdef HASH_PROTECT
    #define ON_HASH_PROTECT(...) __VA_ARGS__
#else
    #define ON_HASH_PROTECT(...)
#endif /* HASH_PROTECT */

#endif /* DEFINE_H */
