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
#include "commands.h"
#include "vector.h"
#include "define_colors.h"
#include "define.h"

// TODO: sscanf, syntax error, invalid cmd - pop ax + 4,   pop (просто) - valid

//==================================================================================================

static bool first_pass_compilation(assembler_t* assm);
static bool second_pass_compilation(assembler_t* assm);

static status_t parse_register(unsigned char* cmd, unsigned char* const code);
static void parse_number(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt);
static status_t parse_if_label(assembler_t* assm, unsigned char* cmd, size_t bytes_cnt);
static void parse_arg_with_label(unsigned char* cmd, assembler_t* assm, size_t* bytes_cnt);

static unsigned char get_arg_type(unsigned char* cmd);
static bool process_args(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt, commands_name_t cmd_name);
static bool validate_arg_type(unsigned char arg_type_byte, commands_name_t cmd);

static void print_header(FILE* ostream);

//==================================================================================================

const size_t LABELS_AMOUNT = 10;
const char* SIGNATURE = "aliffka";
const char* VERSION = "5.2";

const unsigned char REG_TYPE = 1 << 1;
const unsigned char NUM_TYPE = 1 << 2;
const unsigned char RAM_TYPE = 1 << 3;

//==================================================================================================

asm_error_t assembler_ctor(assembler_t* assm, FILE* istream) {
    assert(assm != nullptr);

    text_ctor(&assm->text, istream);

    assm->code = (unsigned char*) calloc(assm->text.symbols_amount + 1, sizeof(double));
    if (assm->code == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        return ASM_MEM_ERROR;
    }

    assm->name_table = new_vector();
    vector_reserve_((vector_t*) assm->name_table, LABELS_AMOUNT * sizeof(label_t));

    assm->fixup = new_vector();
    vector_reserve_((vector_t*) assm->fixup, LABELS_AMOUNT * sizeof(parent_t));
    return ASM_NO_ERRORS;
}

void assembler_dtor(assembler_t* assm) {
    assert(assm != nullptr);

    text_dtor(&assm->text);

    vector_delete(assm->name_table);
    vector_delete(assm->fixup);

    free(assm->code);
    assm->code = nullptr;
}

//==================================================================================================

void assemble(FILE* istream, FILE* ostream) {
    assert(istream != nullptr);
    assert(ostream != nullptr);

    assembler_t assm = {};

    assembler_ctor(&assm, istream);
    assm.code += sizeof(size_t);

    if (first_pass_compilation(&assm) == EXIT_FAILURE) {
        LOG(ERROR, "First pass compilation failed\n");
        assembler_dtor(&assm);
        return;
    }

    if (second_pass_compilation(&assm) == EXIT_FAILURE) {
        LOG(ERROR, "Second pass compilation failed\n");
        assembler_dtor(&assm);
        return;
    }

    assm.code -= sizeof(size_t);
    memcpy(assm.code, &assm.bytes_cnt, sizeof(size_t));
    print_header(ostream);
    fwrite(assm.code, sizeof(char), assm.bytes_cnt + sizeof(size_t), ostream);

    assembler_dtor(&assm);
    LOG(INFO, "Successfully finished assembling\n");
}

//==================================================================================================

static bool first_pass_compilation(assembler_t* assm) {
    assert(assm != nullptr);

    size_t bytes_cnt = 0;
    size_t line_cnt = 0;

    while (line_cnt < assm->text.strings_amount - 1) {
        unsigned char* cmd = assm->text.strings[line_cnt++].begin;

        unsigned char* comment_ptr = (unsigned char*) strstr((char*) cmd, ";");
        if (comment_ptr != nullptr) {
            *(comment_ptr) = '\0';
            comment_ptr++;
        }

        size_t i = 0;
        for (; i < commands_size; i++) {
            if (strstr((char*) cmd, commands[i].alias) != nullptr) {
                cmd += strlen(commands[i].alias) + 1;
                break;
            }
        }

        switch (i) {
            case commands_size: {
                if (parse_if_label(assm, cmd, bytes_cnt) == CORRECT) {
                    cmd = (unsigned char*) strstr((char*) cmd, ":") + 1;
                    if (check_for_unused_parameters(cmd) == CORRECT) {
                        continue;
                    }
                    printf_diagnostic(assm->text.strings[line_cnt - 1].begin, line_cnt, comment_ptr,
                                      cmd, _POS, "command does not support arguments");
                    return EXIT_FAILURE;
                }

                printf_diagnostic(assm->text.strings[line_cnt - 1].begin, line_cnt, comment_ptr,
                                  cmd, _POS, "unknown command");
                return EXIT_FAILURE;
            }
            case (CMD_PUSH - 1): {
                unsigned char push = (unsigned char) CMD_PUSH;
                memcpy(&assm->code[bytes_cnt], &push, sizeof(char));
                bytes_cnt += sizeof(char);

                if (process_args(cmd, assm->code, &bytes_cnt, CMD_PUSH) == EXIT_SUCCESS) {
                    continue;
                }

                printf_diagnostic(assm->text.strings[line_cnt - 1].begin, line_cnt, comment_ptr,
                                  cmd, _POS, "incorrect arguments for push");
                return EXIT_FAILURE;
            }
            case (CMD_POP - 1): {
                unsigned char pop = (unsigned char) CMD_POP;
                memcpy(&assm->code[bytes_cnt], &pop, sizeof(char));
                bytes_cnt += sizeof(char);

                if (process_args(cmd, assm->code, &bytes_cnt, CMD_POP) == EXIT_SUCCESS) {
                    continue;
                }

                printf_diagnostic(assm->text.strings[line_cnt - 1].begin, line_cnt, comment_ptr,
                                  cmd, _POS, "incorrect arguments for pop");
                return EXIT_FAILURE;
            }
            default:
                break;
        }

        memcpy(&assm->code[bytes_cnt], &commands[i].code, sizeof(command));
        bytes_cnt += sizeof(command);

        if (cmd == assm->text.strings[line_cnt].begin) {
            continue;
        }

        if (commands[i].args_amount == 0) {
            if (check_for_unused_parameters(cmd) != CORRECT) {
                printf_diagnostic(assm->text.strings[line_cnt - 1].begin, line_cnt, comment_ptr,
                                  cmd, _POS, "command does not support arguments");
                return EXIT_FAILURE;
            }
            continue;
        }

        if (strstr((char*) cmd, ":") != nullptr) {
            parse_arg_with_label(cmd, assm, &bytes_cnt);
            bytes_cnt += sizeof(addr);
            continue;
        }

        if (strstr((char*) cmd, "x") != nullptr) {
            parse_register(cmd, &assm->code[bytes_cnt]);
            bytes_cnt += sizeof(reg);
            continue;
        }

        parse_number(cmd, assm->code, &bytes_cnt);
    }

    assm->bytes_cnt = bytes_cnt;
    return EXIT_SUCCESS;
}

static bool second_pass_compilation(assembler_t* assm) {
    assert(assm != nullptr);

    for (size_t i = 0; i < vector_size(assm->fixup) / sizeof(parent_t); i++) {
        size_t index = ((parent_t*) vector_element_ptr(assm->fixup, i, sizeof(parent_t)))->label_index;
        ssize_t address = ((label_t*) vector_element_ptr(assm->name_table, index, sizeof(label_t)))->offset;

        if (address == -1) {
            LOG(ERROR, "[SYNTAX ERROR]: label %s was not found\n",
                       ((label_t*) vector_element_ptr(assm->name_table, index, sizeof(label_t)))->name);
            return EXIT_FAILURE;
        }

        size_t valid_addr = (size_t) address;
        memcpy(&assm->code[((parent_t*) vector_element_ptr(assm->fixup, i, sizeof(parent_t)))->label_addr],
               &valid_addr, sizeof(addr));
    }
    return EXIT_SUCCESS;
}

//==================================================================================================

void print_header(FILE* ostream) {
    assert(ostream);

    fprintf(ostream, "%s\n""[version]: %s\n", SIGNATURE, VERSION);
}

//===================================================================================================

static unsigned char get_arg_type(unsigned char* cmd) {
    assert(cmd != nullptr);

    unsigned char arg_type_byte = 0;

    if (strstr((char*) cmd, "[") != nullptr && strstr((char*) cmd, "]") != nullptr) {
        *strstr((char*) cmd, "[") = ' ';
        *strstr((char*) cmd, "]") = ' ';
        arg_type_byte = RAM_TYPE;
    }

    if (strstr((char*) cmd, "+") != nullptr) {
        arg_type_byte |= REG_TYPE | NUM_TYPE;
    }

    else if (strstr((char*) cmd, "x") != nullptr) {
        arg_type_byte |= REG_TYPE;
    }
    else {
        arg_type_byte |= NUM_TYPE;
    }

    return arg_type_byte;
}

//==================================================================================================

static status_t parse_register(unsigned char* cmd, unsigned char* const code) {
    assert(cmd != nullptr);
    assert(code != nullptr);

    char* symb = (char*)((strstr((char*) cmd, "x") - 1));

    if (!(*symb >= 'a' && *symb <= 'h')) {
        return SYNTAX_ERROR;
    }

    code[0] = ((unsigned char) *symb) - 'a';
    return CORRECT;
}

static void parse_number(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(bytes_cnt != nullptr);

    size_t address = strtoul((char*) cmd, nullptr, 10);

    memcpy(&code[*bytes_cnt], &address, sizeof(addr));
    *bytes_cnt += sizeof(addr);
}

//==================================================================================================

static bool process_args(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt, commands_name_t cmd_name) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(bytes_cnt != nullptr);

    unsigned char arg_type_byte = get_arg_type(cmd);

    if (validate_arg_type(arg_type_byte, cmd_name) == EXIT_FAILURE) {
        LOG(ERROR, "Validation of argument type failed\n");
        return EXIT_FAILURE;
    }

    memcpy(&code[*bytes_cnt], &arg_type_byte, sizeof(command));
    *bytes_cnt += sizeof(command);

    if (arg_type_byte & REG_TYPE) {
        parse_register(cmd, &code[*bytes_cnt]);
        cmd = (unsigned char*) strstr((char*) cmd, "x") + 1;
        *bytes_cnt += sizeof(reg);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    if (arg_type_byte & NUM_TYPE) {
        switch (cmd_name) {
            case CMD_PUSH: {
                char* endptr = nullptr;
                double number = strtod((char*) cmd, &endptr);
                cmd = (unsigned char*) endptr;
                memcpy(&code[*bytes_cnt], &number, sizeof(double));
                *bytes_cnt += sizeof(imm);
                break;
            }
            case CMD_POP: {
                char* endptr = nullptr;
                size_t address = strtoul((char*) cmd, &endptr, 10);
                cmd = (unsigned char*) endptr;
                memcpy(&code[*bytes_cnt], &address, sizeof(addr));
                *bytes_cnt += sizeof(addr);
                break;
            }
            default:
                break;
        }
    }
#pragma GCC diagnostic pop

    if (check_for_unused_parameters(cmd) != CORRECT) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

bool validate_arg_type(unsigned char arg_type_byte, commands_name_t cmd) {
    if (arg_type_byte & !(REG_TYPE | NUM_TYPE | RAM_TYPE)) {
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

//=====================================================================================================

static status_t parse_if_label(assembler_t* assm, unsigned char* cmd, size_t bytes_cnt) {
    assert(assm != nullptr);

    char name[NAME_MAX_LEN] = "";

    if (strstr((char*) cmd, ":") != nullptr) {
        memcpy(name, cmd, (size_t)(strstr((char*) cmd, ":") - (char*) cmd));
        if (add_label(assm->name_table, name, bytes_cnt) == EXIST) {
            LOG(ERROR, "[SYNTAX ERROR]: Label %s is already exists\n", name);
            return SYNTAX_ERROR;
        }
    }
    else {
        LOG(ERROR, "[SYNTAX ERROR]: Command %s not found\n", cmd);
        return SYNTAX_ERROR;
    }
    return CORRECT;
}

static void parse_arg_with_label(unsigned char* cmd, assembler_t* assm, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(assm != nullptr);
    assert(bytes_cnt != nullptr);

    char name[NAME_MAX_LEN] = "";
    memcpy(name, cmd, (size_t)(strstr((char*) cmd, ":") - (char*) cmd));

    LOG(DEBUG, "[DEBUG INFO]: label with name %s (len = %zu)\n", name, strlen(name));

    size_t index = 0;
    ssize_t address = return_label_address(assm->name_table, name, &index);

    if (address == -1) {
        parent_t new_parent = {index, *bytes_cnt};
        vector_push_back(assm->fixup, &new_parent);
    }
    else {
        size_t valid_addr = (size_t) address;
        memcpy(&assm->code[*bytes_cnt], &valid_addr, sizeof(addr));
    }
}

//=====================================================================================================

status_t check_for_unused_parameters(unsigned char* cmd) {
    assert(cmd != nullptr);

    size_t i = 0;
    while (i < strlen((char*) cmd) && isspace(cmd[i++])) {
        ;
    }

    if (i == strlen((char*) cmd)) {
        return CORRECT;
    }
    return SYNTAX_ERROR;
}

void printf_diagnostic(unsigned char* buffer, size_t line_num, unsigned char* comment_ptr, unsigned char* error,
                       const char* file, size_t line, const char* func, const char* message) {
    assert(buffer != nullptr);
    assert(error != nullptr);

    unsigned char error_first_symb = *error;
    *error = '\0';

    printf("%s:" GREEN("%zu(%s)\t") RED("[error]") "\n", file, line, func);
    printf("\t%4zu|\t%s", line_num, buffer);
    *error = error_first_symb;
    printf(RED("%s"), error);

    if (comment_ptr != nullptr) {
        printf(";%s", ++comment_ptr);
    }
    printf("\n\n" YELLOW("[error message]: ") RED("%s") "\n", message);
}
