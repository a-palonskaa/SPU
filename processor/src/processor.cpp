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

static double get_arg_push(processor_t* processor, size_t* ip);
static double* get_arg_pop(processor_t* processor, size_t* ip);

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

    for (size_t i = 0; i < 100; i++) {
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

#ifdef DEBUG
    printf("Code size is %zu\n", code_size);
    for (size_t i = 0; i < code_size; i++) {
        printf("%zu = %02x \n", i, processor->code[i]);
    }
#endif /* DEBUG */

    LOG(INFO, "Processor structure is successfully created and initialized.\n");
    return processor->size;
}

//====================================================================================================

void run(processor_t* processor, FILE* ostream) {
    assert(processor != nullptr);
    assert(processor->code != nullptr);

    processor->ip = 0;
    bool run_flag = 1;

    while (run_flag) {
        switch (processor->code[processor->ip]) {
            case CMD_PUSH: {
                processor->ip++;
                double value = get_arg_push(processor, &processor->ip);
                stack_push(processor->stk, &value);
                break;
            }
            case CMD_POP: {
                processor->ip++;
                double value = 0;
                stack_pop(processor->stk, &value);
                *get_arg_pop(processor, &processor->ip) = value;
                break;
            }
            case CMD_ADD: {
                double elm1 = 0, elm2 = 0, result = 0;

                stack_pop(processor->stk, &elm2);
                stack_pop(processor->stk, &elm1);
                result = elm1 + elm2;
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_SUB: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(processor->stk, &elm2);
                stack_pop(processor->stk, &elm1);
                result = elm1 - elm2;
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_MUL: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(processor->stk, &elm2);
                stack_pop(processor->stk, &elm1);
                result = elm1 * elm2;
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_DIV: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(processor->stk, &elm2);
                stack_pop(processor->stk, &elm1);
                result = elm1 / elm2;
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_SQRT: {
                double elm = 0;
                stack_pop(processor->stk, &elm);
                elm = sqrt(elm);
                stack_push(processor->stk, &elm);
                processor->ip++;
                break;
            }
            case CMD_SIN: {
                double elm = 0;
                stack_pop(processor->stk, &elm);
                elm = sin(elm);
                stack_push(processor->stk, &elm);
                processor->ip++;
                break;
            }
            case CMD_COS: {
                double elm = 0;
                stack_pop(processor->stk, &elm);
                elm = cos(elm);
                stack_push(processor->stk, &elm);
                processor->ip++;
                break;
            }
            case CMD_SQR: {
                double elm1 = 0, elm2 = 0;
                stack_pop(processor->stk, &elm1);
                elm2 = elm1 * elm1;
                stack_push(processor->stk, &elm2);
                processor->ip++;
                break;
            }
            case CMD_POW: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(processor->stk, &elm2);
                stack_pop(processor->stk, &elm1);
                result = pow(elm1, elm2);
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_ABS: {
                double elm = 0, result = 0;
                stack_pop(processor->stk, &elm);
                result = fabs(elm);
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_LOG: {
                double elm = 0, result = 0;
                stack_pop(processor->stk, &elm);
                result = log(elm);
                stack_push(processor->stk, &result);
                processor->ip++;
                break;
            }
            case CMD_JA: {
                double elm1 = 0, elm2 = 0;
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                stack_pop(processor->stk, &elm1);
                stack_pop(processor->stk, &elm2);
                processor->ip = (elm2 > elm1) ? address : (processor->ip + 1 + sizeof(size_t));
                break;
            }
            case CMD_JAE: {
                double elm1 = 0, elm2 = 0;
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                stack_pop(processor->stk, &elm1);
                stack_pop(processor->stk, &elm2);
                processor->ip = (elm2 >= elm1) ? address : (processor->ip + 1 + sizeof(size_t));
                break;
            }
            case CMD_JB: {
                double elm1 = 0, elm2 = 0;
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                stack_pop(processor->stk, &elm1);
                stack_pop(processor->stk, &elm2);
                processor->ip = (elm2 < elm1) ? address : (processor->ip + 1 + sizeof(size_t));
                break;
            }
            case CMD_JBE: {
                double elm1 = 0, elm2 = 0;
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                stack_pop(processor->stk, &elm1);
                stack_pop(processor->stk, &elm2);
                processor->ip = (elm2 <= elm1) ? address : (processor->ip + 1 + sizeof(size_t));
                break;
            }
            case CMD_JNE: {
                double elm1 = 0, elm2 = 0;
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                stack_pop(processor->stk, &elm1);
                stack_pop(processor->stk, &elm2);
                processor->ip = are_doubles_equal(elm2, elm1) ?
                                address : (processor->ip + 1 + sizeof(size_t));
                break;
            }
            case CMD_JMP: {
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                processor->ip = address;
                break;
            }
            case CMD_CALL: {
                size_t next_cmd_ip = processor->ip + 1 + sizeof(size_t);
                stack_push(processor->addr_stk, &next_cmd_ip);
                size_t address = 0;
                memcpy(&address, &processor->code[processor->ip + 1], sizeof(size_t));
                processor->ip = address;
                break;
            }
            case CMD_RET: {
                size_t addr = 0;
                stack_pop(processor->addr_stk, &addr);
                processor->ip = addr;
                break;
            }
            case CMD_OUT: {
                double result = 0;
                stack_pop(processor->stk, &result);
                fprintf(ostream, "Result is %f\n", result);
                processor->ip++;
                break;
            }
            case CMD_HLT: {
                run_flag = 0;
                processor->ip++;
                break;
            }
            case CMD_DROW: {
                processor->ip++;
                drow(processor->ram);
                break;
            }
            case CMD_DUMP: {
                processor_dump(stdout, processor, _POS);
                processor->ip++;
                break;
            }
            default: {
                fprintf(ostream, "SNIXERR: %d\n", processor->code[processor->ip]);
                LOG(ERROR, "Undefined command ""%d"" ip = %zu\n",
                           processor->code[processor->ip], processor->ip);
                return;
                break;
            }
        }
    }
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

//====================================================================================================

void parse_code(processor_t* processor, unsigned char* text, size_t file_size) {
    assert(processor != nullptr);
    LOG(INFO, "Parsing has been started.\n");

    processor->code = (unsigned char*) calloc(processor->size, sizeof(double));
    if (processor->code == nullptr) {
        LOG(ERROR, "Memory allocation error.\n" STRERROR(errno));
        return;
    }

    size_t i = 0, j = 0;;
    unsigned char command = 0;
    double arg = 0;
    ssize_t cnt = 0;

    while (i < file_size && j <= processor->size) {
        cnt = get_int(&text[i], &command);
        if (cnt == -1) {
            break;
        }
        else {
            i += (size_t) cnt;
        }

        processor->code[j++] = command;


        if (commands[command - 1].args_amount) {
            i += get_double(&text[i], &arg);
            memcpy(processor->code + j, &arg, sizeof(double));
            j += sizeof(double);
        }
    }
    LOG(INFO, "Successfully parsed the text.\n");
}

size_t get_double(unsigned char* buffer, double* number) {
    double val = 0;
    double power = 0;
    size_t i = 0;
    int sign = 0;

    for (; isspace(buffer[i]); i++) {
        ;
    }

    sign = (buffer[i] == '-') ? -1 : 1;
    if (buffer[i] == '+' || buffer[i] == '-') {
        i++;
    }

    for (; isdigit(buffer[i]); i++) {
        val = 10 * val + (buffer[i] - '0');
    }

    if (buffer[i] =='.') {
        i++;
    }

    for (power = 1; isdigit(buffer[i]); i++) {
        val = 10 * val + (buffer[i] - '0');
        power *= 10;
    }

    *number = sign * val / power;
    return i;
}

ssize_t get_int(unsigned char* buffer, unsigned char* number) {
    size_t i = 0;
    unsigned int val = 0;

    for (; isspace(buffer[i]); i++) {
        ;
    }

    for (; isdigit(buffer[i]); i++) {
        val = 10 * val + (buffer[i] - '0');
    }

    if (val) {
        *number = (unsigned char) val;
        return (ssize_t) i;
    }
    return -1;
}


static double get_arg_push(processor_t* processor, size_t* ip) {
    assert(processor != nullptr);
    assert(ip != nullptr);

    unsigned char arg_type = processor->code[(*ip)++];
    double result = 0;

    if (arg_type & REG_TYPE) {
        unsigned char reg = 0;
        memcpy(&reg, &processor->code[*ip], sizeof(char));
        *ip += sizeof(char);
        result += processor->registres[(size_t) reg];
    }

    if (arg_type & NUM_TYPE) {
        double number = 0;
        memcpy(&number, &processor->code[*ip], sizeof(double));
        *ip += sizeof(double);
        result += number;
    }

    if (arg_type & RAM_TYPE) {
        return processor->ram[(size_t) result];
    }
    return result;
}

// TODO: add general mask proccessing: get_arg + validate (debug)
// TODO: typedef immediate (elemement типо не дабл), imm


static double* get_arg_pop(processor_t* processor, size_t* ip) {
    assert(processor != nullptr);
    assert(ip != nullptr);

    unsigned char arg_type = processor->code[(*ip)++];
    double address = 0;

    if (arg_type & RAM_TYPE) {
        if (arg_type & NUM_TYPE) {
            double number = 0;
            memcpy(&number, &processor->code[*ip], sizeof(double));
            *ip += sizeof(double);
            address += number;
        }

        if (arg_type & REG_TYPE){
            unsigned char reg = 0;
            memcpy(&reg, &processor->code[*ip], sizeof(char));
            *ip += sizeof(char);
            address += processor->registres[(size_t) reg];
        }
        return &processor->ram[(size_t) address];
    }

    unsigned char reg = 0;
    memcpy(&reg, &processor->code[*ip], sizeof(char));
    *ip += sizeof(char);
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
