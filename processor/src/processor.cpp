#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <errno.h>

#include "processor.h"
#include "stack.h"
#include "proc_verify.h"
#include "pop_push.h"
#include "verify.h"
#include "logger.h"
#include "drow.h"

//====================================================================================================

const size_t STACK_CAPACITY = 10;
const double REG_POISON_VALUE = -666.666;

//====================================================================================================

const unsigned char REG_TYPE = 1 << 1;
const unsigned char NUM_TYPE = 1 << 2;
const unsigned char RAM_TYPE = 1 << 3;

//====================================================================================================

static void print_double(void* elm, FILE* ostream);
static void print_addr(void* elm, FILE* ostream);

static double* get_arg(processor_t* processor, size_t* ip);
#ifdef DEBUG
static bool validate_arg(unsigned char arg_type_byte, commands_name_t cmd);
static bool validate_move_arg(unsigned char arg_type_byte, size_t arg_num);
#endif /* DEBUG */
//====================================================================================================

static int are_doubles_equal(const double a, const double b);
static double max_two(const double a, const double b);
static double max_three(const double a, const double b, const double c);

//====================================================================================================

void processor_ctor(processor_t* processor, size_t code_size) {
    assert(processor);
    if (code_size == 0) {
        return;
    }

    processor->size = code_size;

    processor->code = (unsigned char*) calloc(code_size, sizeof(char));
    if (processor->code == nullptr) {
        LOG(ERROR, "Memory allocation error\n" STRERROR(errno));
        processor_dtor(processor);
        return;
    }

    processor->stk = NEW_STACK_(sizeof(double), STACK_CAPACITY, print_double);
    processor->addr_stk = NEW_STACK_(sizeof(size_t), STACK_CAPACITY, print_addr);

    for (size_t i = 0; i < 8; i++) {
        processor->registres[i] = REG_POISON_VALUE;
    }

    for (size_t i = 0; i < 10000; i++) {
        processor->ram[i] = 0;
    }
}

void processor_dtor(processor_t* processor) {
    free(processor->code);
    processor->code = nullptr;

    stack_dtor(processor->stk);
    stack_dtor(processor->addr_stk);

    processor->size = 0;

    for (size_t i = 0; i < 8; i++) {
        processor->registres[i] = REG_POISON_VALUE;
    }
}

//====================================================================================================

size_t get_code(FILE* istream, processor_t* processor, size_t code_size) {
    assert(istream);
    assert(processor);

    if ((fread(processor->code, sizeof(char), code_size, istream) != code_size) &&
        !feof(istream) && ferror(istream)) {
        return 0;
    }

    LOG(INFO, "Processor structure is successfully created and initialized.\n");
    return processor->size;
}

//====================================================================================================

bool run(processor_t* processor, FILE* ostream) {
    assert(processor != nullptr);
    assert(processor->code != nullptr);
    (void) ostream;

    processor->ip = 0;
    bool run_flag = 1;

    while (run_flag == 1) {
        switch (processor->code[processor->ip]) {
#define DEF_COMMAND_(cmd, name, has_arg, arg_type, func)                                        \
            case CMD_##cmd: {                                                                   \
                func                                                                            \
                break;                                                                          \
            }
#include "cmd_def.h"
#undef DEF_COMMAND_
            default: {
                run_flag = -1;
                LOG(ERROR, "Unknown command %2x, ip = %zu\n", processor->code[processor->ip], processor->ip);
                break;
            }
        }
    }

    return (run_flag == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

//====================================================================================================

static void print_double(void* elm, FILE* ostream) {
    double* element = (double*) elm;
    fprintf(ostream, "%f\n", *element);
}

static void print_addr(void* elm, FILE* ostream) {
    size_t* addr = (size_t*) elm;
    fprintf(ostream, "0x%p\n", (void*) *addr);
}

//===================================================================================================
#ifdef DEBUG
static bool validate_arg(unsigned char arg_type_byte, commands_name_t cmd) {
    if (arg_type_byte & !(REG_TYPE | NUM_TYPE | RAM_TYPE)) {
        LOG(ERROR, "Unused bits exposed\n");
        return EXIT_FAILURE;
    }

    if ((cmd == CMD_PUSH) && !(arg_type_byte & (REG_TYPE | NUM_TYPE | RAM_TYPE))) {
        LOG(ERROR, "Push without argument error\n");
        return EXIT_FAILURE;
    }

    if ((cmd == CMD_POP) && ((arg_type_byte & (REG_TYPE | NUM_TYPE)) == (REG_TYPE | NUM_TYPE))) {
        LOG(ERROR, "Pop with reg and addr, but without ram is invalid\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static bool validate_move_arg(unsigned char arg_type_byte, size_t arg_num) {
     if (arg_type_byte & !(REG_TYPE | NUM_TYPE | RAM_TYPE)) {
        LOG(ERROR, "Unused bits exposed\n");
        return EXIT_FAILURE;
    }

    if ((arg_num == 1) && (arg_type_byte & NUM_TYPE) && !(arg_type_byte & RAM_TYPE))) {
        LOG(ERROR, "Cannot make a destination a number\n");
        return EXIT_FAILURE;
    }

    if ((arg_num == 2) && (arg_type_byte & RAM_TYPE))) {
        LOG(ERROR, "Source cannot be a ram\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif /* DEBUG */

static double* get_arg(processor_t* processor, size_t* ip) {
    assert(processor != nullptr);
    assert(ip != nullptr);

    unsigned char arg_type = processor->code[(*ip)++];
    double result = 0;

    unsigned char reg = 8;
    if (arg_type & REG_TYPE){
        memcpy(&reg, &processor->code[*ip], sizeof(reg));
        *ip += sizeof(reg);
        result += processor->registres[(size_t) reg];
    }

    if (arg_type & NUM_TYPE) {
        double number = 0;
        memcpy(&number, &processor->code[*ip], sizeof(imm));
        *ip += sizeof(imm);
        result += number;
    }

    if (arg_type & RAM_TYPE) {
        return &processor->ram[(size_t) result];
    }

    processor->registres[8] = result;
    return &processor->registres[(size_t) reg];
}

//====================================================================================================

static int are_doubles_equal(const double a, const double b) {
    const double epsilon = 1e-8;
    return fabs(a - b) <= epsilon * max_three(1, fabs(a), fabs(b));
}

static double max_two(const double a, const double b) {
    return (a > b) ? a : b;
}

static double max_three(const double a, const double b, const double c) {
    return max_two(c, max_two(a, b));
}
