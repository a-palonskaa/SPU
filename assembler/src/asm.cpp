#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>

#include "asm.h"
#include "logger.h"
#include "fixup.h"
#include "label.h"

//==================================================================================================

static void second_pass_compilation(fixup_t* fixup, name_table_t* name_table, unsigned char* code);

static void parse_register(unsigned char* cmd, unsigned char* code);
static status_t parse_number(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt);
static void parse_push(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt);
static void parse_pop(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt);
static status_t parse_if_label(unsigned char* cmd, name_table_t* name_table, size_t bytes_cnt);
static status_t parse_if_has_label(unsigned char* cmd, unsigned char* code, name_table_t* name_table,
                                   size_t* bytes_cnt, fixup_t* fixup);

static unsigned char get_arg_type(unsigned char* cmd);
static size_t get_double(unsigned char* buffer, double* number);
static ssize_t get_uint(unsigned char* buffer, size_t* number);

static void assembler_dtor(name_table_t* name_table, fixup_t* fixup, unsigned char* code, unsigned char* cmd);

//==================================================================================================

static size_t print_number(char* cmd, char* code);
static size_t print_register(char* cmd, char* code);

//==================================================================================================

const size_t LABELS_AMOUNT = 10;
const char* SIGNATURE = "aliffka";
const char* VERSION = "4.3";

const unsigned char REG_TYPE = 1 << 1;
const unsigned char NUM_TYPE = 1 << 2;
const unsigned char RAM_TYPE = 1 << 3;
//==================================================================================================

void assemble(FILE* istream, FILE* ostream) {
    assert(istream != nullptr);
    assert(ostream != nullptr);

    size_t bytes_cnt = 0;
    size_t file_size = (size_t) find_file_size(istream);

    unsigned char* code = (unsigned char*) calloc(file_size, sizeof(char));
    if (code == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        return;
    }

    name_table_t* name_table = new_name_table(LABELS_AMOUNT);
    fixup_t* fixup = new_fixup(LABELS_AMOUNT);

    unsigned char* cmd = nullptr;
    size_t line_len = 0;

    while (getline((char**) &cmd, &line_len, istream) != -1) {
        void* comment_ptr = (void*) strstr((const char*) cmd, ";");
        if (comment_ptr != nullptr) {
            *((unsigned char*) comment_ptr) = '\0';
        }

        size_t i = 0;
        for (; i < commands_size; i++) {
            if (strstr((const char*) cmd, commands[i].alias) != nullptr) {
                break;
            }
        }

        switch (i) {
            case commands_size: {
                if (parse_if_label(cmd, name_table, bytes_cnt) == CONTINUE) {
                    continue;
                }
                free(cmd);
                free(code);
                return;
            }
            case 0: {
                unsigned char push = (unsigned char) CMD_PUSH;
                memcpy(&code[bytes_cnt], &push, sizeof(char));
                bytes_cnt += sizeof(char);
                parse_push(cmd, code, &bytes_cnt);
                continue;
            }
            case (CMD_POP - 1): {
                unsigned char pop = (unsigned char) CMD_POP;
                memcpy(&code[bytes_cnt], &pop, sizeof(char));
                bytes_cnt += sizeof(char);

                parse_pop(cmd, code, &bytes_cnt);
                continue;
            }
            default:
                break;
        }

        memcpy(&code[bytes_cnt], &commands[i].name, sizeof(char));
        bytes_cnt += sizeof(char);

        if (commands[i].args_amount == 0) {
            continue;
        }

        if (parse_if_has_label(cmd, code, name_table, &bytes_cnt, fixup) == CONTINUE) {
            continue;
        }

        if (strstr((const char *) cmd, "x") != nullptr) {
            parse_register(cmd + 4, &code[bytes_cnt]);
            bytes_cnt += sizeof(char);
            continue;
        }

        if (parse_number(cmd, code, &bytes_cnt) != CONTINUE) {
            free(cmd);
            free(code);
            return;
        }
    }

    second_pass_compilation(fixup, name_table, code);

    print_header(ostream, bytes_cnt);
    fwrite(code, sizeof(char), bytes_cnt, ostream);

    assembler_dtor(name_table, fixup, code, cmd);

    LOG(INFO, "SUCCESSFULLY FINISHED ASSEMBLING\n");
}

//==================================================================================================

static void second_pass_compilation(fixup_t* fixup, name_table_t* name_table, unsigned char* code) {
    assert(fixup != nullptr);
    assert(name_table != nullptr);
    assert(code != nullptr);

    for (size_t i = 0; i < fixup->size; i++) {
        ssize_t addr = name_table->label[fixup->parents[i].label_index].cmd_ptr;
        if (addr == -1) {
            LOG(ERROR, "SYNTAX ERROR label %s was not found\n",
                       name_table->label[fixup->parents[i].label_index].name);
            return;
        }
        size_t valid_addr = (size_t) addr;
        memcpy(&code[fixup->parents[i].label_addr], &valid_addr, sizeof(size_t));
    }
}

//==================================================================================================

ssize_t find_file_size(FILE* istream) {
    assert(istream);

    struct stat file_data = {};

    if (fstat(fileno(istream), &file_data) == -1) {
        return -1;
    }
    return (ssize_t) file_data.st_size;
}

static void assembler_dtor(name_table_t* name_table, fixup_t* fixup, unsigned char* code, unsigned char* cmd) {
    name_table_dtor(name_table);
    fixup_dtor(fixup);

    free(code);
    code = nullptr;

    free(cmd);
    cmd = nullptr;
}

//==================================================================================================

void print_header(FILE* ostream, size_t bytes_cnt) {
    assert(ostream);

    fprintf(ostream, "%s\n""[version]: %s\n""[bytes amount]: %zu\n", SIGNATURE, VERSION, bytes_cnt);

}

//===================================================================================================


static unsigned char get_arg_type(unsigned char* cmd) {
    assert(cmd != nullptr);

    unsigned char arg_type_byte = 0;

    if (strstr((const char*) cmd, "[") && (strstr((const char*) cmd, "]"))) {
        arg_type_byte = RAM_TYPE;
    }

    if (strstr((const char*) cmd, "+") != nullptr) {
        arg_type_byte |= REG_TYPE | NUM_TYPE;
    }
    else if (strstr((const char*) cmd, "x") != nullptr) {
        arg_type_byte |= REG_TYPE;
    }
    else {
        arg_type_byte |= NUM_TYPE;
    }
    return arg_type_byte;
}

static size_t get_double(unsigned char* buffer, double* number) {
    assert(buffer != nullptr);
    assert(number != nullptr);

    double val = 0;
    double power = 0;
    int sign = 0;

    size_t i = 0;
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

static ssize_t get_uint(unsigned char* buffer, size_t* number) {
    assert(buffer != nullptr);
    assert(number != nullptr);

    size_t i = 0;
    size_t val = 0;

    for (; !isdigit(buffer[i]); i++) {
        ;
    }

    for (; isdigit(buffer[i]); i++) {
        val = 10 * val + (size_t) (buffer[i] - '0');
    }

    if (val) {
        *number = val;
        return (ssize_t) i;
    }
    return -1;
}

//==================================================================================================

static void parse_register(unsigned char* cmd, unsigned char* code) {
    assert(cmd != nullptr);
    assert(code != nullptr);

    switch (*(strstr((const char *) cmd, "x") - 1)) {
        case 'a':
            code[0] = 1;
            break;
        case 'b':
            code[0] = 2;
            break;
        case 'c':
            code[0] = 3;
            break;
        case 'd':
            code[0] = 4;
            break;
        case 'e':
            code[0] = 5;
            break;
        case 'f':
            code[0] = 6;
            break;
        case 'g':
            code[0] = 7;
            break;
        case 'h':
            code[0] = 8;
            break;
        default:
            break;
    }
}

static status_t parse_number(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(bytes_cnt != nullptr);

    size_t addr = 0;
    if (get_uint(cmd, &addr) == -1) {
        LOG(ERROR, "SYNTAX ERROR command %s not found\n", cmd);
        return SYNTAX_ERROR;
    }

    memcpy(&code[*bytes_cnt], &addr, sizeof(size_t));
    *bytes_cnt += sizeof(size_t);
    return CONTINUE;
}

//==================================================================================================

static status_t parse_if_label(unsigned char* cmd, name_table_t* name_table, size_t bytes_cnt) {
    assert(cmd != nullptr);
    assert(name_table != nullptr);

    if (strstr((const char*) cmd, ":") != nullptr) {
        char name[10] = "";
        strncpy(name, (const char*) cmd, (size_t) (((unsigned char *) strstr((char *) cmd, ":")) - cmd));
        LOG(INFO, "[DEBUG INFO]: label with name %s (len = %zu) and addr %zu is adding\n",
                name, (size_t) (((unsigned char *) strstr((char *) cmd, ":")) - cmd), bytes_cnt);   //REVIEW -
        if (!add_label(name_table, name, bytes_cnt)) {
            LOG(ERROR, "SYNTAX ERROR LABEL %s is already exists\n", name);
            return SYNTAX_ERROR;
        }
    }
    else {
        LOG(ERROR, "SYNTAX ERROR command %s not found\n", cmd);
        return SYNTAX_ERROR;
    }

    return CONTINUE;
}


static status_t parse_if_has_label(unsigned char* cmd, unsigned char* code, name_table_t* name_table,
                                   size_t* bytes_cnt, fixup_t* fixup) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(name_table != nullptr);
    assert(bytes_cnt != nullptr);
    assert(fixup != nullptr);

    if (strstr((const char *) cmd, ":") != nullptr) {
            char name[10] = "";
            strncpy(name, (const char*) (strstr((const char *) cmd, " ") + 1),
                          (size_t) (strstr((const char *) cmd, ":") - strstr((const char *) cmd, " ") - 1));
            LOG(INFO, "[DEBUG INFO]: label with name %s (len = %zu)\n", name,
                      (size_t) (strstr((const char *) cmd, ":") - strstr((const char *) cmd, " ") - 1));   //REVIEW -

            size_t index = 0;
            ssize_t addr = return_label_address(name_table, name, &index);

            if (addr == -1) {
                fixup->parents[fixup->size].label_index = index;  //FIXME - checkups
                fixup->parents[fixup->size++].label_addr = *bytes_cnt;
            }
            else {
                size_t valid_addr = (size_t) addr;
                memcpy(&code[*bytes_cnt], &valid_addr, sizeof(size_t));
                // printf("valid %zx\n", valid_addr);
                // printf("writen %x %x %x %x %x %x %x %x\n", code[*bytes_cnt], code[*bytes_cnt + 1], code[*bytes_cnt + 2], code[*bytes_cnt + 3], code[*bytes_cnt + 4], code[*bytes_cnt + 5], code[*bytes_cnt + 6], code[*bytes_cnt + 7]);
            }
            *bytes_cnt += sizeof(size_t);
            return CONTINUE;
    }

    return SYNTAX_ERROR;
}
//ХУЙНЯ - optimization (no copypast<3) ++ ERRORS(SYNTAX ERRORS FOR EX)
static void parse_push(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(bytes_cnt != nullptr);

    unsigned char arg_type_byte = get_arg_type(cmd);

    memcpy(&code[*bytes_cnt], &arg_type_byte, sizeof(char));
    *bytes_cnt += sizeof(char);

    cmd += 4;
    if (arg_type_byte & REG_TYPE) {
        parse_register(cmd, &code[*bytes_cnt]);
        cmd = (unsigned char*) strstr((const char*) cmd, "+") + 1;
        *bytes_cnt += sizeof(char);
    }

    if (arg_type_byte & NUM_TYPE) {
        double number = 0;
        get_double(cmd, &number);
#ifdef DEBUG
        unsigned char dbl[8];
        memcpy(dbl, &number, 8);
        for (size_t i = 0; i < 8; i++) {
            printf("%02x ", dbl[i]);
        }
        printf("\n", dbl[i]);
#endif /* DEBUG */

        memcpy(&code[*bytes_cnt], &number, sizeof(double));
        *bytes_cnt += sizeof(double);
    }
}

static void parse_pop(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(bytes_cnt != nullptr);

    unsigned char arg_type_byte = get_arg_type(cmd);

    memcpy(&code[*bytes_cnt], &arg_type_byte, sizeof(char));
    *bytes_cnt += sizeof(char);

    if (arg_type_byte & REG_TYPE) {
        parse_register(cmd + 4, &code[*bytes_cnt]);
        *bytes_cnt += sizeof(char);
    }

    if (arg_type_byte & NUM_TYPE) {
        size_t addr = 0;
        get_uint(cmd + 4, &addr);
        memcpy(&code[*bytes_cnt], &addr, sizeof(size_t));
        *bytes_cnt += sizeof(size_t);
    }
}

//==================================================================================================

size_t assemble_text_format(char* cmd, char* code, size_t bytes_cnt, size_t* size_bytes) {
    size_t array_size_bytes = 0;

    if (strstr(cmd, "push") != nullptr) {
        if (strstr(cmd, "x") != nullptr) {
            memcpy(&code[bytes_cnt], "19 ", 3);
            bytes_cnt += 3;
            print_register(cmd + 4, &code[bytes_cnt]);
            bytes_cnt += 2;
        }
        else {
            memcpy(&code[bytes_cnt], "1 ", 2);
            bytes_cnt += 2;
            bytes_cnt += print_number(cmd + 4, &code[bytes_cnt]);
        }
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "add") != nullptr) {
        memcpy(&code[bytes_cnt], "2 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "sub") != nullptr) {
        memcpy(&code[bytes_cnt], "3 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "mul") != nullptr) {
        memcpy(&code[bytes_cnt], "4 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "div") != nullptr) {
        memcpy(&code[bytes_cnt], "5 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "out") != nullptr) {
        memcpy(&code[bytes_cnt], "6 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "in") != nullptr) {
        memcpy(&code[bytes_cnt], "7 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "sqrt") != nullptr) {
        memcpy(&code[bytes_cnt], "8 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "sin") != nullptr) {
        memcpy(&code[bytes_cnt], "9 ", 2);
        bytes_cnt += 2;
        array_size_bytes++;
    }
    else if (strstr(cmd, "cos") != nullptr) {
        memcpy(&code[bytes_cnt], "10 ", 3);
        bytes_cnt += 3;
        array_size_bytes++;
    }
    else if (strstr(cmd, "dump") != nullptr) {
        memcpy(&code[bytes_cnt], "11 ", 3);
        bytes_cnt += 3;
        array_size_bytes++;
    }
    else if (strstr(cmd, "hlt") != nullptr) {
        memcpy(&code[bytes_cnt], "12 ", 3);
        bytes_cnt += 3;
        array_size_bytes++;
    }
    else if (strstr(cmd, "ja") != nullptr) {
        memcpy(&code[bytes_cnt], "13 ", 3);
        bytes_cnt += 3;
        bytes_cnt += print_number(cmd + 2, &code[bytes_cnt]);
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "jae") != nullptr) {
        memcpy(&code[bytes_cnt], "14 ", 3);
        bytes_cnt += 3;
        bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "jb") != nullptr) {
        memcpy(&code[bytes_cnt], "15 ", 3);
        bytes_cnt += 3;
        bytes_cnt += print_number(cmd + 2, &code[bytes_cnt]);
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "jbe") != nullptr) {
        memcpy(&code[bytes_cnt], "16 ", 3);
        bytes_cnt += 3;
        bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "jne") != nullptr) {
        memcpy(&code[bytes_cnt], "17 ", 3);
        bytes_cnt += 3;
        bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "jmp") != nullptr) {
        memcpy(&code[bytes_cnt], "18 ", 3);
        bytes_cnt += 3;
        bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
        array_size_bytes += 1 + sizeof(double);
    }
    else if (strstr(cmd, "pop") != nullptr) {
        memcpy(&code[bytes_cnt], "20 ", 3);
        bytes_cnt += 3;
        print_register(cmd, &code[bytes_cnt]);
        bytes_cnt += 2;
        array_size_bytes += 1 + sizeof(double);
    }

    *size_bytes += array_size_bytes;
    return bytes_cnt;
}

static size_t print_number(char* cmd, char* code) {
    size_t i = 0;
    (void) cmd;

    while (!isspace(code[i])) {
        printf("%c", code[i++]);
    }
    printf(" ");
    return i + 1;
}


static size_t print_register(char* cmd, char* code) {
    switch (cmd[0]) {
        case '1':
            memcpy(code, "ax", 2);
            break;
        case '2':
            memcpy(code, "bx", 2);
            break;
        case '3':
            memcpy(code, "cx", 2);
            break;
        case '4':
            memcpy(code, "dx", 2);
            break;
        case '5':
            memcpy(code, "ex", 2);
            break;
        case '6':
            memcpy(code, "fx", 2);
            break;
        case '7':
            memcpy(code, "ex", 2);
            break;
        case '8':
            memcpy(code, "hx", 2);
            break;
        default:
            break;
    }
    return 2;
}
