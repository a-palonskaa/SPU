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

static void print_header(FILE* ostream, size_t bytes_cnt);

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

    assm->code = (unsigned char*) calloc(assm->text.symbols_amount, sizeof(double));
    if (assm->code == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        return ASM_MEM_ERROR;
    }
    printf("%p %d\n\n\n", assm->code, assm->text.symbols_amount * sizeof(double));

    assm->name_table = new_vector();
    vector_reserve_((vector_t*) assm->name_table, LABELS_AMOUNT * sizeof(label_t));

    assm->fixup = new_vector();
    vector_reserve_((vector_t*) assm->fixup, LABELS_AMOUNT * sizeof(parent_t));
    return ASM_NO_ERRORS;
}
 // TODO: onegin
 // TODO: add ctor, struct
 // TODO: add first pass coomp
void assemble(FILE* istream, FILE* ostream) {
    assert(istream != nullptr);
    assert(ostream != nullptr);

    assembler_t assm = {};

    assembler_ctor(&assm, istream);

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

    print_header(ostream, assm.bytes_cnt);
    fwrite(assm.code, sizeof(char), assm.bytes_cnt, ostream);

    assembler_dtor(&assm);
    LOG(INFO, "Successfully finished assembling\n");
}

//==================================================================================================

static bool first_pass_compilation(assembler_t* assm) {
    assert(assm != nullptr);

    size_t bytes_cnt = 0;
    size_t line_cnt = 0;
    printf("%d\n\n\n", assm->text.symbols_amount * sizeof(double));
    printf("Lines amount %d\n", assm->text.strings_amount);
    for (size_t i = 0; i < assm->text.strings_amount - 1; i++) {
        printf("str %zu[%p]: %s\n",i, assm->text.strings[i].begin, assm->text.strings[i].begin);
    }

    printf("name_table->data %p %p\nassm %p\nassm->fixup%p\n", assm->name_table, assm->name_table->data, assm, assm->fixup);
    while (line_cnt < assm->text.strings_amount - 1) {
        unsigned char* cmd = assm->text.strings[line_cnt++].begin;
        printf("will parse string[%p] %s, bytes cnt is %zu\n", cmd, cmd, bytes_cnt);
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

        switch (i) {  // FIXME: hard code
            case commands_size: {
                if (parse_if_label(assm, cmd, bytes_cnt) == CORRECT) {
                    continue;
                }
                LOG(ERROR, "Incorr label, return\n");
                return EXIT_FAILURE;
            }
            case (CMD_PUSH - 1): {
                unsigned char push = (unsigned char) CMD_PUSH;
                printf("\taddr of code is %p\n", &assm->code[bytes_cnt]);
                memcpy(&assm->code[bytes_cnt], &push, sizeof(char));
                bytes_cnt += sizeof(char);

                printf("\taddr of code is %p\n", assm->code);
                if (process_args(cmd, assm->code, &bytes_cnt, CMD_PUSH) == EXIT_SUCCESS) {
                    continue;
                }
                LOG(ERROR, "Incorr push args, return\n");
                return EXIT_FAILURE;
            }
            case (CMD_POP - 1): {
                unsigned char pop = (unsigned char) CMD_POP;
                printf("\taddr of code is %p\n", &assm->code[bytes_cnt]);
                memcpy(&assm->code[bytes_cnt], &pop, sizeof(char));
                bytes_cnt += sizeof(char);
                printf("\taddr of code is %p\n", assm->code);
                if (process_args(cmd, assm->code, &bytes_cnt, CMD_POP) == EXIT_SUCCESS) {
                    continue;
                }
                LOG(ERROR, "Incorr pop args, return\n");
                return EXIT_FAILURE;
            }
            default:
                break;
        }
        printf("\taddr of code is %p\n", &assm->code[bytes_cnt]);
        memcpy(&assm->code[bytes_cnt], &commands[i].name, sizeof(command)); // TODO: typedef command
        bytes_cnt += sizeof(command);
        if (commands[i].args_amount == 0) { //TODO - check for syntax shit(kinda after)
            continue;
        }
        char symb1[10] = {};
        char symb2 = 0;

        if (sscanf((const char*) cmd, "%[^:]%[:]", symb1, &symb2) == 2) {
            parse_arg_with_label(cmd, assm, &bytes_cnt);
        }

        if (sscanf((char*) cmd, "%c%[x]", symb1, &symb2) == 2) {
            parse_register(cmd, &assm->code[bytes_cnt]);
            bytes_cnt += sizeof(reg);
            continue;
        }
        printf("\taddr of code is %p\n", assm->code);
        parse_number(cmd, assm->code, &bytes_cnt);
    }

    assm->bytes_cnt = bytes_cnt;
    return EXIT_SUCCESS;
}

//REVIEW - review label
//FIXME - переделать с учетом векторов

static bool second_pass_compilation(assembler_t* assm) {
    assert(assm != nullptr);

    for (size_t i = 0; i < vector_size(assm->fixup); i++) {
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

void assembler_dtor(assembler_t* assm) {
    assert(assm != nullptr);

    text_dtor(&assm->text);

    vector_delete(assm->name_table);
    vector_delete(assm->fixup);

    free(assm->code);
    assm->code = nullptr;
}

//==================================================================================================

void print_header(FILE* ostream, size_t bytes_cnt) {
    assert(ostream);
    // TODO: add binary bytes_cnt (first 8 bytes in code -size)
    fprintf(ostream, "%s\n""[version]: %s\n""[bytes amount]: %zu\n", SIGNATURE, VERSION, bytes_cnt);
}

//===================================================================================================
    // TODO: sscanf
    // push [ax], push 5, push ax, push [4], push [ax+4], push [4+ax], push ax+5
    // TODO: syntax error

    // pop ax, pop [ax], pop [ax + 4],
    // TODO: невалидная команда - pop ax + 4
    // TODO: pop (просто)
static unsigned char get_arg_type(unsigned char* cmd) {
    assert(cmd != nullptr);

    unsigned char arg_type_byte = 0;

    char symb[2] = {};

    if (sscanf((char*) cmd, "%[^[][%[^]]]", symb, symb) == 2) {
        arg_type_byte = RAM_TYPE;
    }

    if (sscanf((char*) cmd, "%[^+]+%c", symb, symb) == 2) {
        arg_type_byte |= REG_TYPE | NUM_TYPE;
    }

    else if (sscanf((char*) cmd, "%c%[x]", symb, symb) == 2) {
        arg_type_byte |= REG_TYPE;
    }
    else { //ХУЙНЯ - check
        arg_type_byte |= NUM_TYPE;
    }

    return arg_type_byte;
}

// TODO: strtod, sscanf
//==================================================================================================

static status_t parse_register(unsigned char* cmd, unsigned char* const code) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    printf("code[0] %p %d\n", &code[0], code[0]);
    printf("REG cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4], cmd[5], cmd[5], cmd[6], cmd[6], cmd[7], cmd[7], cmd[8], cmd[8],  cmd[9], cmd[9],  cmd[10], cmd[10]);
    char symb[2] = {};
    printf("\t\tcode[0] %p %d\n", &code[0], code[0]);
    if (!(sscanf((char*) cmd, "%c%[x]", &symb[0], &symb[1]) == 2)) {
        return SYNTAX_ERROR; //TODO - exactly!
    }
    printf("\t\tcode[0] %p %d\n", &code[0], code[0]);
    printf("REG cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4], cmd[5], cmd[5], cmd[6], cmd[6], cmd[7], cmd[7], cmd[8], cmd[8],  cmd[9], cmd[9],  cmd[10], cmd[10]);
    printf("%c %c\n", symb[0], symb[1]);
    printf("\t\tcode[0] %p %d\n", &code[0], code[0]);
    if (!(*symb >= 'a' && *symb <= 'h')) {
        return SYNTAX_ERROR; //TODO - exactly!
    }
    printf("code[0] %p %d\n", &code[0], code[0]);
    printf("REG cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4], cmd[5], cmd[5], cmd[6], cmd[6], cmd[7], cmd[7], cmd[8], cmd[8],  cmd[9], cmd[9],  cmd[10], cmd[10]);
    printf("code[0] %p %d\n", &code[0], code[0]);
    code[0] = ((unsigned char) *symb) + 1 - 'a';
    printf("REG cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4], cmd[5], cmd[5], cmd[6], cmd[6], cmd[7], cmd[7], cmd[8], cmd[8],  cmd[9], cmd[9],  cmd[10], cmd[10]);
    return CORRECT;
}

static void parse_number(unsigned char* cmd, unsigned char* code, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(code != nullptr);
    assert(bytes_cnt != nullptr);

    size_t address = strtoul((char*) cmd, nullptr, 10); //TODO 10-> const of system od counting

    memcpy(&code[*bytes_cnt], &address, sizeof(addr));
    *bytes_cnt += sizeof(addr);
}

//==================================================================================================
// TODO: add general proccessing arg: parse arg + validate
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
        printf("BYTES_CNT %zu\n\n\n", *bytes_cnt);
        printf("\taddr of code is %p\n", &code[*bytes_cnt]);
        parse_register(cmd, &code[*bytes_cnt]);
        *bytes_cnt += sizeof(reg);
    }

    printf("\nPROC ARG cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4]);
    if (arg_type_byte & NUM_TYPE) {
        switch (cmd_name) {
            case CMD_PUSH: {
                double number = strtod((char*) cmd, nullptr);
                memcpy(&code[*bytes_cnt], &number, sizeof(double));
                *bytes_cnt += sizeof(imm);
                break;
            }
            case CMD_POP: {
                size_t address = strtoul((char*) cmd, nullptr, 10); //TODO 10-> const of system od counting
                memcpy(&code[*bytes_cnt], &address, sizeof(addr));
                *bytes_cnt += sizeof(addr);
                break;
            }
            default:
                break;
        }
    }
    printf("PROC ARG cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4]);
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
    printf("PARSING CMD = %s\n", cmd);
    printf("cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4]);
    char name[NAME_MAX_LEN] = "";
    char symb = 0;
    if (sscanf((char*) cmd, "%[^:]%[:]", name, &symb)) {
        printf("NAME is %s\n", name);
        if (add_label(assm->name_table, name, bytes_cnt) == EXIST) {
            LOG(ERROR, "[SYNTAX ERROR]: Label %s is already exists\n", name); // TODO better diagnostic(like where, what addr, etc+ colors)
            return SYNTAX_ERROR;
        }
    }
    else {
        LOG(ERROR, "[SYNTAX ERROR]: Command %s not found\n", cmd); // TODO better diagnostic, invalid what (cmd, reg, number, where, print full srt)
        return SYNTAX_ERROR;
    }
    printf("cmd arrd: %p [%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c][%2x %c] [%2x %c][%2x %c]\n", cmd, cmd[-5], cmd[-5], cmd[-4], cmd[-4],  cmd[-3], cmd[-3], cmd[-2], cmd[-2], cmd[-1], cmd[-1], cmd[0], cmd[0], cmd[1], cmd[1],cmd[2], cmd[2], cmd[3], cmd[3], cmd[4], cmd[4]);
    return CORRECT;
}

static void parse_arg_with_label(unsigned char* cmd, assembler_t* assm, size_t* bytes_cnt) {
    assert(cmd != nullptr);
    assert(assm != nullptr);
    assert(bytes_cnt != nullptr);

    char name[NAME_MAX_LEN] = "";
    printf("PARSING CMD = %s\n", cmd);
    sscanf((char*) cmd, "%[^:]", name);
    printf("label NAME is %s\n", name);
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

    *bytes_cnt += sizeof(addr);
}
