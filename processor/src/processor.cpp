#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include "processor.h"
#include "stack.h"
#include "pop_push.h"
#include "verify.h"
#include "logger.h"

static void print_double (void* elm, FILE* ostream) {
    double* element = (double*) elm;
    fprintf(ostream, "%f\n", *element);
}

void run(processor_t* processor) {
    my_stack_t* stk = NEW_STACK_(sizeof(double), 10, print_double);
    size_t ip = 0;
    bool run_flag = 1;

    while (run_flag) {
        switch (*((unsigned char*) processor->code + ip)) {
            case CMD_PUSH: {
                stack_push(stk, &processor->code[ip+1]);
                ip += 1 + sizeof(double);
                break;
            }
            case CMD_ADD: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(stk, &elm2);
                stack_pop(stk, &elm1);
                result = elm1 + elm2;
                stack_push(stk, &result);
                ip += 1;
                break;
            }
            case CMD_SUB: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(stk, &elm2);
                stack_pop(stk, &elm1);
                result = elm1 - elm2;
                stack_push(stk, &result);
                ip += 1;
                break;
            }
            case CMD_MUL: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(stk, &elm2);
                stack_pop(stk, &elm1);
                result = elm1 * elm2;
                stack_push(stk, &result);
                ip += 1;
                break;
            }
            case CMD_DIV: {
                double elm1 = 0, elm2 = 0, result = 0;
                stack_pop(stk, &elm2);
                stack_pop(stk, &elm1);
                result = elm1 / elm2;
                stack_push(stk, &result);
                ip += 1;
                break;
            }
            case CMD_OUT: {
                double result = 0;
                stack_pop(stk, &result);
                fprintf(stdout, "Result is %f\n", result);
                ip += 1;
                break;
            }
            case CMD_SQRT: {
                double elm = 0;
                stack_pop(stk, &elm);
                elm = sqrt(elm);
                stack_push(stk, &elm);
                ip += 1;
                break;
            }
            case CMD_SIN: {
                double elm = 0;
                stack_pop(stk, &elm);
                elm = sin(elm);
                stack_push(stk, &elm);
                ip += 1;
                break;
            }
            case CMD_COS: {
                double elm = 0;
                stack_pop(stk, &elm);
                elm = cos(elm);
                stack_push(stk, &elm);
                ip += 1;
                break;
            }
            case CMD_DUMP: {
                stack_dump(stk, __FILE__, __LINE__, __func__);
                ip += 1;
                break;
            }
            case CMD_HLT: {
                run_flag = 0;
                ip += 1;
                break;

            }
            default: {
                fprintf(stdout, "SNIXERR: %d\n", *((unsigned char*) processor->code + ip));
                LOG(ERROR, "Undefined command ""%d""\n", *((unsigned char*) processor->code + ip));
                break;
            }
        }
    }
    stack_dtor(stk);
}

size_t get_code(FILE* istream, processor_t* processor) {
    assert(istream);
    assert(processor);

    size_t file_size = (size_t) find_file_size(istream);
    unsigned char* text = (unsigned char*) calloc(file_size, sizeof(char));
    assert(text);

    if ((fread(text, sizeof(char), file_size, istream) != file_size) &&
        !feof(istream) && ferror(istream)) {
        return 0;
    }

    parse_code(processor, text, file_size);

    free(text);
    text = nullptr;

    LOG(INFO, "Processor structure is successfully created and initialized\n");
    return processor->size;
}


ssize_t find_file_size(FILE* istream) {
    assert(istream);

    struct stat file_data = {};

    if (fstat(fileno(istream), &file_data) == -1) {
        return -1;
    }
    return (ssize_t) file_data.st_size;
}

void parse_code(processor_t* processor, unsigned char* text, size_t file_size) {
    assert(processor);
    //file_size != 0;
    processor->code = (unsigned char*) calloc(file_size, sizeof(double));
    assert(processor->code);

    size_t i = 0, j = 0;;
    unsigned char command = 0;
    double arg = 0;
    ssize_t cnt = 0 ;

    while (i < file_size) {
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

    processor->code = (unsigned char*) realloc(processor->code, j);
    processor->size = j;
}

size_t get_double (unsigned char* buffer, double* number) {
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
