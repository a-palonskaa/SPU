#define POP_(elm) stack_pop(processor->stk, &(elm))
#define PUSH_(elm) stack_push(processor->stk, &elm)

#define DEF_BINARY_(func)   \
    do {                    \
        double elm1 = 0;    \
        double elm2 = 0;    \
        double result = 0;  \
        POP_(elm2);         \
        POP_(elm1);         \
        func                \
        PUSH_(result);      \
        processor->ip++;    \
    } while(0)

#define DEF_JUMP_(func)                                                         \
    do {                                                                        \
        double elm1 = 0;                                                        \
        double elm2 = 0;                                                        \
        size_t address = 0;                                                     \
        memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));  \
        POP_(elm2);                                                             \
        POP_(elm1);                                                             \
        func                                                                    \
    } while(0)

#define DEF_UNARY_(func)    \
    do {                    \
        double elm1 = 0;    \
        double result = 0;  \
        POP_(elm1);         \
        func                \
        PUSH_(result);      \
        processor->ip++;    \
    } while(0)
