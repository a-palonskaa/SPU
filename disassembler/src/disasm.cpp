#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "disasm.h"
#include "commands.h"
#include "buffer_holder.h"

const unsigned char REG_TYPE = 1 << 1;
const unsigned char NUM_TYPE = 1 << 2;
const unsigned char RAM_TYPE = 1 << 3;

static void print_address(FILE* ostream, unsigned char* code);
static void print_number(FILE* ostream, unsigned char* code);
static void print_register(FILE* ostream, unsigned char* code);

void disassemble(FILE* istream, FILE* ostream) {
    assert(istream != nullptr);
    assert(ostream != nullptr);

    buffer_holder_t buffer_holder = {};
    buffer_status_t status = buffer_ctor(&buffer_holder, istream);
    if (status != VALID) {
        return;
    }

    size_t i = 0;
    while (i < buffer_holder.bytecode_size) {
        switch (buffer_holder.bytecode[i]) {
#define DEF_COMMAND_(cmd, name, has_arg, arg_type, _)                        \
    case CMD_##cmd: {                                                        \
        fprintf(ostream, name " ");                                          \
        i++;                                                                 \
        if (has_arg) {                                                       \
            if (strstr(#arg_type, "ORN") != nullptr) {                       \
                parse_cmd_args(&buffer_holder.bytecode[i], &i, ostream);     \
                i++;                                                         \
            }                                                                \
            else {                                                           \
                print_address(ostream, &buffer_holder.bytecode[i]);          \
                i += sizeof(addr);                                           \
            }                                                                \
        }                                                                    \
        break;                                                               \
    }
#include "cmd_def.h"
#undef DEF_COMMAND_
            default:
                break;
        }
        fprintf(ostream, "\n");
    }

    buffer_dtor(&buffer_holder);
}

bool parse_cmd_args(unsigned char* code, size_t* i, FILE* ostream) {
    assert(code != nullptr);
    assert(i != nullptr);

    if (code[0] & !(REG_TYPE | NUM_TYPE | RAM_TYPE)) {
        return EXIT_FAILURE;
    }

    if ((code[0] & (REG_TYPE | NUM_TYPE | RAM_TYPE)) == (REG_TYPE | NUM_TYPE | RAM_TYPE)) {
        return EXIT_SUCCESS;
    }

    if (!(code[0] & RAM_TYPE)) {
        print_main_part(code, i, ostream);
    }
    else {
        fprintf(ostream, "[");
        print_main_part(code, i, ostream);
        fprintf(ostream, "]");
    }
    return EXIT_SUCCESS;
}

void print_main_part(unsigned char* code, size_t* i, FILE* ostream) {
    assert(code != nullptr);
    assert(i != nullptr);

    if ((code[0] & (REG_TYPE | NUM_TYPE)) ==  (REG_TYPE | NUM_TYPE)){
        print_register(ostream, &code[1]);
        *i += sizeof(reg);
        print_number(ostream, &code[1 + sizeof(reg)]);
        *i += sizeof(imm);
    }
    else if (code[0] & REG_TYPE) {
        print_register(ostream, &code[1]);
        *i += sizeof(reg);
    }
    else {
        print_number(ostream, &code[1]);
        *i += sizeof(imm);
    }
}

static void print_address(FILE* ostream, unsigned char* code) {
    assert(ostream != nullptr);
    assert(code != nullptr);

    size_t address = 0;
    memcpy(&address, code, sizeof(addr));
    fprintf(ostream, "%zu", address);
}

static void print_number(FILE* ostream, unsigned char* code) {
    assert(ostream != nullptr);
    assert(code != nullptr);

    double number = 0;
    memcpy(&number, code, sizeof(imm));
    fprintf(ostream, "%f", number);
}

static void print_register(FILE* ostream, unsigned char* code) {
    switch ((int) code[0]) {
        case 0:
            fprintf(ostream, "ax");
            break;
        case 1:
            fprintf(ostream, "bx");
            break;
        case 2:
            fprintf(ostream, "cx");
            break;
        case 3:
            fprintf(ostream, "dx");
            break;
        case 4:
            fprintf(ostream, "ex");
            break;
        case 5:
            fprintf(ostream, "fx");
            break;
        case 6:
            fprintf(ostream, "gx");
            break;
        case 7:
            fprintf(ostream, "hx");
            break;
        default:
            break;
    }
    fprintf(ostream, " ");
}
