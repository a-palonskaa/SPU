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

static void print_register(unsigned char* cmd, unsigned char* code);
static size_t get_double(unsigned char* buffer, double* number);
static ssize_t get_uint(unsigned char* buffer, size_t* number);

const size_t LABELS_AMOUNT = 10;

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

    unsigned char cmd[50] = "";
    while (fgets((char*) cmd, 50, istream)) {
        size_t i = 0;
        for (; i < commands_size; i++) {
            if (strstr((const char*) cmd, commands[i].alias) != nullptr) {
                break;
            }
        }
        if (i == commands_size) {
            if (strstr((const char*) cmd, ":") != nullptr) {
                char name[10] = "";
                strncpy(name, (const char*) cmd, (size_t) (((unsigned char *) strstr((const char *) cmd, ":")) - cmd));
                printf("[DEBUG INFO]: label with name %s (len = %zu) and addr %zu is adding\n",
                       name, (size_t) (((unsigned char *) strstr((const char *) cmd, ":")) - cmd), bytes_cnt);   //REVIEW -
                if (!add_label(name_table, name, bytes_cnt)) {
                    LOG(ERROR, "SYNTAX ERROR LABEL %s is already exists\n", name);
                    return;
                }
            }
            else {
                LOG(ERROR, "SYNTAX ERROR command %s not found\n", cmd);
                return;
            }
        }


        if (i == 0) {
            if (strstr((const char*) cmd, "x") != nullptr) {
                unsigned char pushr = (unsigned char) CMD_PUSHR;
                memcpy(&code[bytes_cnt], &pushr, sizeof(char)); //NOTE - careful!
                bytes_cnt += sizeof(char);

                print_register(cmd + 4, &code[bytes_cnt]);
                bytes_cnt += sizeof(char);
            }
            else {
                unsigned char push = (unsigned char) CMD_PUSH;
                memcpy(&code[bytes_cnt], &push, sizeof(char));
                bytes_cnt += sizeof(char);

                double number = 0;
                get_double(cmd + 4, &number);
                unsigned char dbl[8];
                memcpy(dbl, &number, 8);
                for (size_t i = 0; i < 8; i++) {
                    printf("%02x ", dbl[i]);
                }
                printf("\n", dbl[i]);
                memcpy(&code[bytes_cnt], &number, sizeof(double));
                bytes_cnt += sizeof(double);
            }
            continue;
        }

        memcpy(&code[bytes_cnt], &commands[i].name, sizeof(char));
        bytes_cnt += sizeof(char);

        if (commands[i].args_amount == 0) {
            continue;
        }

        if (strstr((const char *) cmd, ":") != nullptr) {
            char name[10] = "";
            strncpy(name, (const char*) cmd, (size_t) (((unsigned char *) strstr((const char *) cmd, ":")) - cmd));
            printf("[DEBUG INFO]: label with name %s (len = %zu)\n", name, (size_t) (((unsigned char *) strstr((const char *) cmd, ":")) - cmd));   //REVIEW -

            size_t index = 0;
            ssize_t addr = return_label_address(name_table, name, &index);
            if (addr == -1) {
                fixup->parents[fixup->size].label_index = index;  //FIXME - checkups
                fixup->parents[fixup->size++].label_addr = bytes_cnt;
            }
            else {
                memcpy(&code[bytes_cnt], &addr, sizeof(size_t));
            }
            bytes_cnt += sizeof(size_t);
            continue;
        }

        if (strstr((const char *) cmd, "x") != nullptr) {
            print_register(cmd + 4, &code[bytes_cnt]);
            bytes_cnt += sizeof(char);
            continue;
        }

        size_t addr = 0;
        if (get_uint(cmd, &addr) == -1) {
            LOG(ERROR, "SYNTAX ERROR command %s not found\n", cmd);
            return;
        }
        memcpy(&code[bytes_cnt], &addr, sizeof(size_t));
        bytes_cnt += sizeof(size_t);
    }

//         if (strstr(cmd, "push") != nullptr) {
//             if (strstr(cmd, "x") != nullptr) {
//
//                 memcpy(&code[bytes_cnt], "19 ", 3);
//                 bytes_cnt += 3;
//                 print_register(cmd + 4, &code[bytes_cnt]);
//                 bytes_cnt += 2;
//             }
//             else {
//                 memcpy(&code[bytes_cnt], "1 ", 2);
//                 bytes_cnt += 2;
//                 bytes_cnt += print_number(cmd + 4, &code[bytes_cnt]);
//             }
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "add") != nullptr) {
//             memcpy(&code[bytes_cnt], "2 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "sub") != nullptr) {
//             memcpy(&code[bytes_cnt], "3 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "mul") != nullptr) {
//             memcpy(&code[bytes_cnt], "4 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "div") != nullptr) {
//             memcpy(&code[bytes_cnt], "5 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "out") != nullptr) {
//             memcpy(&code[bytes_cnt], "6 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "in") != nullptr) {
//             memcpy(&code[bytes_cnt], "7 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "sqrt") != nullptr) {
//             memcpy(&code[bytes_cnt], "8 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "sin") != nullptr) {
//             memcpy(&code[bytes_cnt], "9 ", 2);
//             bytes_cnt += 2;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "cos") != nullptr) {
//             memcpy(&code[bytes_cnt], "10 ", 3);
//             bytes_cnt += 3;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "dump") != nullptr) {
//             memcpy(&code[bytes_cnt], "11 ", 3);
//             bytes_cnt += 3;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "hlt") != nullptr) {
//             memcpy(&code[bytes_cnt], "12 ", 3);
//             bytes_cnt += 3;
//             array_size_bytes++;
//         }
//         else if (strstr(cmd, "ja") != nullptr) {
//             memcpy(&code[bytes_cnt], "13 ", 3);
//             bytes_cnt += 3;
//             bytes_cnt += print_number(cmd + 2, &code[bytes_cnt]);
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "jae") != nullptr) {
//             memcpy(&code[bytes_cnt], "14 ", 3);
//             bytes_cnt += 3;
//             bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "jb") != nullptr) {
//             memcpy(&code[bytes_cnt], "15 ", 3);
//             bytes_cnt += 3;
//             bytes_cnt += print_number(cmd + 2, &code[bytes_cnt]);
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "jbe") != nullptr) {
//             memcpy(&code[bytes_cnt], "16 ", 3);
//             bytes_cnt += 3;
//             bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "jne") != nullptr) {
//             memcpy(&code[bytes_cnt], "17 ", 3);
//             bytes_cnt += 3;
//             bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "jmp") != nullptr) {
//             memcpy(&code[bytes_cnt], "18 ", 3);
//             bytes_cnt += 3;
//             bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
//             array_size_bytes += 1 + sizeof(double);
//         }
//         else if (strstr(cmd, "pop") != nullptr) {
//             memcpy(&code[bytes_cnt], "20 ", 3);
//             bytes_cnt += 3;
//             print_register(cmd, &code[bytes_cnt]);
//             bytes_cnt += 2;
//             array_size_bytes += 1 + sizeof(double);
//         }


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

    print_header(ostream, bytes_cnt);
    fwrite(code, sizeof(char), bytes_cnt, ostream);

    free(code);
    code = nullptr;

    name_table_dtor(name_table);
    fixup_dtor(fixup);
    LOG(INFO, "SUCCESSFULLY FINISHED ASSEMBLING\n");
}

//==================================================================================================

static void print_register(unsigned char* cmd, unsigned char* code) {
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

//==================================================================================================

void print_header(FILE* ostream, size_t bytes_cnt) {
    assert(ostream);

    fprintf(ostream, "aliffka\n""[version]: 2.0\n""[bytes amount]: %zu\n", bytes_cnt);

}

ssize_t find_file_size(FILE* istream) {
    assert(istream);

    struct stat file_data = {};

    if (fstat(fileno(istream), &file_data) == -1) {
        return -1;
    }
    return (ssize_t) file_data.st_size;
}

//===================================================================================================


static size_t get_double(unsigned char* buffer, double* number) {
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

static ssize_t get_uint(unsigned char* buffer, size_t* number) {
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
