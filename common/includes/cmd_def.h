DEF_COMMAND_(PUSH, "push", true, ORN, 0xAA, {
    processor->ip++;
    stack_push(processor->stk, get_arg(processor, &processor->ip));
})
DEF_COMMAND_(POP, "pop", true, ORN, 0xAA, {
    processor->ip++;
    stack_pop(processor->stk, get_arg(processor, &processor->ip));
})
DEF_COMMAND_(ADD, "add", false, NAN, 0x2F, {
    result = elm1 + elm2;
})
DEF_COMMAND_(SUB, "sub", false, NAN, 0x2F, {
    result = elm1 - elm2;
})
DEF_COMMAND_(MUL, "mul", false, NAN, 0x2F, {
    result = elm1 * elm2;
})
DEF_COMMAND_(DIV, "div", false, NAN, 0x2F, {
    result = elm1 / elm2;
})
DEF_COMMAND_(OUT, "out", false, NAN, 0xAA, {
    stack_pop(processor->stk, &result);
    fprintf(ostream, "Result is %f\n", result);
    processor->ip++;
})
DEF_COMMAND_(IN, "in", false, NAN, 0xAA,  {

})
DEF_COMMAND_(SQRT, "sqrt", false, NAN, 0x1F, {
    result = sqrt(elm1);
})
DEF_COMMAND_(SIN, "sin", false, NAN, 0x1F, {
    result = sin(elm1);
})
DEF_COMMAND_(COS, "cod", false, NAN, 0x1F, {
    result = cos(elm1);
})
DEF_COMMAND_(DUMP, "dump", false, NAN, 0xAA, {
    processor_dump(stdout, processor, _POS);
    processor->ip++;
})
DEF_COMMAND_(HLT, "hlt", false, NAN, 0xAA, {
    run_flag = 0;
    processor->ip++;
})
DEF_COMMAND_(JA, "ja", true, N, 0x2C, {
    processor->ip = (elm1 > elm2) ? address : (processor->ip + 1 + sizeof(size_t));
})
DEF_COMMAND_(JAE, "jae", true, N, 0x2C, {
    processor->ip = (elm1 >= elm2) ? address : (processor->ip + 1 + sizeof(size_t));
})
DEF_COMMAND_(JB, "jb", true, N, 0x2C, {
    processor->ip = (elm1 < elm2) ? address : (processor->ip + 1 + sizeof(size_t));
})
DEF_COMMAND_(JBE, "jbe", true, N, 0x2C, {
    processor->ip = (elm1 <= elm2) ? address : (processor->ip + 1 + sizeof(size_t));
})
DEF_COMMAND_(JNE, "jne", true, N, 0x2C, {
    processor->ip = are_doubles_equal(elm1, elm2) ? address : (processor->ip + 1 + sizeof(size_t));
})
DEF_COMMAND_(JMP, "jmp", true, N, 0xAA,  {
    memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
    processor->ip = address;
})
DEF_COMMAND_(CALL, "call", true, N, 0xAA, {
    size_t next_cmd_ip = processor->ip + 1 + sizeof(size_t);
    stack_push(processor->addr_stk, &next_cmd_ip);
    memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
    processor->ip = address;
})
DEF_COMMAND_(RET, "ret", false, NAN, 0xAA, {
    stack_pop(processor->addr_stk, &address);
    processor->ip = address;
})
DEF_COMMAND_(DROW, "drow", false, NAN, 0xAA, {
    processor->ip++;
    drow(processor->ram);
})
DEF_COMMAND_(SQR, "sqr", false, NAN, 0x1F, {
    result = elm1 * elm1;
})
DEF_COMMAND_(POW, "pow", false, NAN, 0x2F, {
    result = pow(elm1, elm2);
})
DEF_COMMAND_(ABS, "abs", false, NAN, 0x1F, {
    result = fabs(elm1);
})
DEF_COMMAND_(LOG, "log", false, NAN, 0x1F, {
    result = log(elm1);
})
