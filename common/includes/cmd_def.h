#include "dsl.h"
DEF_COMMAND_(PUSH, "push", true, ORN, {
    processor->ip++;
#ifdef DEBUG
    if (validate_arg(processor->code[processor->ip], CMD_PUSH) == EXIT_FAILURE) {
        run_flag = - 1;
        break;
    }
#endif /* DEBUG */
    stack_push(processor->stk, get_arg(processor, &processor->ip));
})
DEF_COMMAND_(MOV, "mov", true, ORN, {
    processor->ip++;
#ifdef DEBUG
    if (validate_move_arg(processor->code[processor->ip], 1) == EXIT_FAILURE) {
        run_flag = - 1;
        break;
    }
#endif /* DEBUG */
    double* dst = get_arg(processor, &processor->ip);
#ifdef DEBUG
    if (validate_move_arg(processor->code[processor->ip], 2) == EXIT_FAILURE) {
        run_flag = - 1;
        break;
    }
#endif /* DEBUG */
    double* src = get_arg(processor, &processor->ip);
    memcpy(dst, src, sizeof(imm));
})
DEF_COMMAND_(MEMSET, "memset", true, N, {
    processor->ip++;
    size_t elm1 = 0;
    double elm2 = 0;
    size_t elm3 = 0;

    memcpy(&elm1, &processor->code[processor->ip], sizeof(addr));
    processor->ip += sizeof(addr);

    memcpy(&elm2, &processor->code[processor->ip], sizeof(imm));
    processor->ip += sizeof(imm);

    memcpy(&elm3, &processor->code[processor->ip], sizeof(addr));
    processor->ip += sizeof(addr);

    for (size_t i = 0; i < elm3 * sizeof(imm);) {
        memcpy((char*) &processor->ram[elm1] + i, &elm2, sizeof(imm));
        i += sizeof(imm);
    }
})
DEF_COMMAND_(POP, "pop", true, ORN, {
    processor->ip++;
#ifdef DEBUG
    if (validate_arg(processor->code[processor->ip], CMD_POP) == EXIT_FAILURE) {
        run_flag = - 1;
        break;
    }
#endif /* DEBUG */
    stack_pop(processor->stk, get_arg(processor, &processor->ip));
})
DEF_COMMAND_(ADD, "add", false, NAN, {
    DEF_BINARY_(result = elm1 + elm2;);
})
DEF_COMMAND_(SUB, "sub", false, NAN, {
    DEF_BINARY_(result = elm1 - elm2;);
})
DEF_COMMAND_(MUL, "mul", false, NAN, {
    DEF_BINARY_(result = elm1 * elm2;);
})
DEF_COMMAND_(DIV, "div", false, NAN, {
    DEF_BINARY_(result = elm1 / elm2;);
})
DEF_COMMAND_(OUT, "out", false, NAN, {
    double result = 0;
    printf("");
    stack_pop(processor->stk, &result);
    fprintf(stdout, "Result is %f\n", result);
    processor->ip++;
})
DEF_COMMAND_(IN, "in", false, NAN,  {
    double result = 0;
    fprintf(stdout, "Enter a number:\n");
    fscanf(stdin, "%lf", &result);
    PUSH_(result);
    processor->ip++;
})
DEF_COMMAND_(SQRT, "sqrt", false, NAN, {
    DEF_UNARY_(result = sqrt(elm1););
})
DEF_COMMAND_(SIN, "sin", false, NAN, {
    DEF_UNARY_(result = sin(elm1););
})
DEF_COMMAND_(COS, "cos", false, NAN, {
    DEF_UNARY_(result = cos(elm1););
})
DEF_COMMAND_(DUMP, "dump", false, NAN, {
    processor_dump(stdout, processor, _POS);
    processor->ip++;
})
DEF_COMMAND_(HLT, "hlt", false, NAN, {
    run_flag = 0;
    processor->ip++;
})
DEF_COMMAND_(JA, "ja", true, N, {
    DEF_JUMP_(processor->ip = (elm1 > elm2) ? address : (processor->ip + 1 + sizeof(size_t)););
})
DEF_COMMAND_(JAE, "jae", true, N, {
    DEF_JUMP_(processor->ip = (elm1 >= elm2) ? address : (processor->ip + 1 + sizeof(size_t)););
})
DEF_COMMAND_(JB, "jb", true, N, {
    DEF_JUMP_(processor->ip = (elm1 < elm2) ? address : (processor->ip + 1 + sizeof(size_t)););
})
DEF_COMMAND_(JBE, "jbe", true, N, {
    DEF_JUMP_(processor->ip = (elm1 <= elm2) ? address : (processor->ip + 1 + sizeof(size_t)););
})
DEF_COMMAND_(JE, "je", true, N, {
    DEF_JUMP_(processor->ip = are_doubles_equal(elm1, elm2) ? address : (processor->ip + 1 + sizeof(size_t)););
})
DEF_COMMAND_(JNE, "jne", true, N, {
    DEF_JUMP_(processor->ip = !are_doubles_equal(elm1, elm2) ? address : (processor->ip + 1 + sizeof(size_t)););
})
DEF_COMMAND_(JMP, "jmp", true, N,  {
    size_t address = 0;
    memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
    processor->ip = address;
})
DEF_COMMAND_(CALL, "call", true, N, {
    size_t address = 0;
    size_t next_cmd_ip = processor->ip + 1 + sizeof(size_t);
    stack_push(processor->addr_stk, &next_cmd_ip);
    memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
    processor->ip = address;
})
DEF_COMMAND_(RET, "ret", false, NAN, {
    size_t address = 0;
    POP_(address);
    processor->ip = address;
})
DEF_COMMAND_(SHOW, "show", false, NAN, {
    processor->ip++;
    drow(processor->ram);
})
DEF_COMMAND_(SQR, "sqr", false, NAN, {
    DEF_UNARY_(result = elm1 * elm1;);
})
DEF_COMMAND_(POW, "pow", false, NAN, {
    DEF_BINARY_(result = pow(elm1, elm2););
})
DEF_COMMAND_(ABS, "abs", false, NAN, {
    DEF_UNARY_(result = fabs(elm1););
})
DEF_COMMAND_(LOG, "log", false, NAN, {
    DEF_UNARY_(result = log(elm1););
})
#undef DEF_BINARY_
#undef DEF_JUMP_
#undef DEF_UNARY_
#undef POP_
#undef PUSH_
