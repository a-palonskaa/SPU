#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include "disasm.h"

static void print_address(FILE* ostream, unsigned char* code);
static void print_number(FILE* ostream, unsigned char* code);
static void print_register(FILE* ostream, unsigned char* code);

void disassemble(FILE* istream, FILE* ostream) {
    size_t i = 0;
    size_t code_size = (size_t) find_file_size(istream);
    unsigned char* code = (unsigned char*) calloc(code_size, sizeof(char));

    if ((fread(code, sizeof(char), code_size, istream) != code_size) &&
        !feof(istream) && ferror(istream)) {
        return;
    }

    for (size_t str_cnt = 0; str_cnt < 3; i++) {
        if (code[i] == '\n') {
            str_cnt++;
        }
    }

    while (i < code_size) {
        switch (code[i]) {
            case CMD_PUSH: {
                fprintf(ostream, "push ");
                print_number(ostream, &code[++i]);
                i += sizeof(double);
                break;
            }
            case CMD_ADD: {
                fprintf(ostream, "add ");
                i++;
                break;
            }
            case CMD_MUL: {
                fprintf(ostream, "mul ");
                i++;
                break;
            }
            case CMD_DIV: {
                fprintf(ostream, "div ");
                i++;
                break;
            }
            case CMD_OUT: {
                fprintf(ostream, "out ");
                i++;
                break;
            }
            case CMD_IN: {
                fprintf(ostream, "in ");
                i++;
                break;
            }
            case CMD_SQRT: {
                fprintf(ostream, "sqrt ");
                i++;
                break;
            }
            case CMD_SIN: {
                fprintf(ostream, "sin ");
                i++;
                break;
            }
            case CMD_COS: {
                fprintf(ostream, "cos ");
                i++;
                break;
            }
            case CMD_DUMP: {
                fprintf(ostream, "dump ");
                i++;
                break;
            }
            case CMD_HLT: {
                fprintf(ostream, "hlt ");
                i++;
                break;
            }
            case CMD_JA: {
                fprintf(ostream, "ja ");
                print_address(ostream, &code[++i]);
                i += sizeof(size_t);
                break;
            }
            case CMD_JAE: {
                fprintf(ostream, "jae ");
                print_address(ostream, &code[++i]);
                i += sizeof(size_t);
                break;
            }
            case CMD_JB: {
                fprintf(ostream, "jb ");
                print_address(ostream, &code[++i]);
                i += sizeof(size_t);
                break;
            }
            case CMD_JBE: {
                fprintf(ostream, "jbe ");
                print_address(ostream, &code[++i]);
                i += sizeof(size_t);
                break;
            }
            case CMD_JNE: {
                fprintf(ostream, "jne ");
                print_address(ostream, &code[++i]);
                i += sizeof(size_t);
                break;
            }
            case CMD_JMP: {
                fprintf(ostream, "jmp ");
                print_address(ostream, &code[++i]);
                i += sizeof(size_t);
                break;
            }
            case CMD_PUSHR: {
                fprintf(ostream, "push ");
                print_register(ostream, &code[++i]);
                i += sizeof(char);
                break;
            }
            case CMD_POP: {
                fprintf(ostream, "pop ");
                print_register(ostream, &code[++i]);
                i += sizeof(char);
                break;
            }
            default:
                break;
        }
        fprintf(ostream, "\n");
    }

    free(code);
}


ssize_t find_file_size(FILE* istream) {
    assert(istream);

    struct stat file_data = {};

    if (fstat(fileno(istream), &file_data) == -1) {
        return -1;
    }
    return (ssize_t) file_data.st_size;
}

static void print_address(FILE* ostream, unsigned char* code) {
    assert(ostream != nullptr);
    assert(code != nullptr);

    size_t addr = 0;
    memcpy(&addr, code, sizeof(size_t));
    fprintf(ostream, "%zu", addr);
}

static void print_number(FILE* ostream, unsigned char* code) {
    assert(ostream != nullptr);
    assert(code != nullptr);

    double number = 0;
    memcpy(&number, code, sizeof(double));
    fprintf(ostream, "%f", number);
}

static void print_register(FILE* ostream, unsigned char* code) {
    printf("%d %d %d", code[0], code[0] == 1, (int) code[0] == 1);
    switch ((int) code[0]) {
        case 1:
            fprintf(ostream, "ax");
            break;
        case 2:
            fprintf(ostream, "bx");
            break;
        case 3:
            fprintf(ostream, "cx");
            break;
        case 4:
            fprintf(ostream, "dx");
            break;
        case 5:
            fprintf(ostream, "ex");
            break;
        case 6:
            fprintf(ostream, "fx");
            break;
        case 7:
            fprintf(ostream, "gx");
            break;
        case 8:
            fprintf(ostream, "hx");
            break;
        default:
            break;
    }
    fprintf(ostream, " ");
}



    // while (i < code_size) {
    //     switch (code[i]) {
    //         case '1': {
    //             switch (code[i + 1]) {
    //                 case ' ':
    //                     fprintf(ostream, "push ");
    //                     i += 2;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '0':
    //                     fprintf(ostream, "cos ");
    //                     i += 3;
    //                     break;
    //                 case '1':
    //                     fprintf(ostream, "dump ");
    //                     i += 3;
    //                     break;
    //                 case '2':
    //                     fprintf(ostream, "hlt ");
    //                     i += 3;
    //                     break;
    //                 case '3':
    //                     fprintf(ostream, "ja ");
    //                     i += 3;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '4':
    //                     fprintf(ostream, "jae ");
    //                     i += 3;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '5':
    //                     fprintf(ostream, "jb ");
    //                     i += 3;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '6':
    //                     fprintf(ostream, "jbe ");
    //                     i += 3;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '7':
    //                     fprintf(ostream, "jne ");
    //                     i += 3;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '8':
    //                     fprintf(ostream, "jmp ");
    //                     i += 3;
    //                     i += print_number(ostream, &code[i]);
    //                     break;
    //                 case '9':
    //                     fprintf(ostream, "push ");
    //                     i += 3;
    //                     i += print_register(ostream, &code[i]);
    //                     break;
    //                 default:
    //                     i++;
    //                     break;
    //             }
    //             break;
    //         }
    //         case '2': {
    //             switch(code[i + 1]) {
    //                 case ' ':
    //                     fprintf(ostream, "add ");
    //                     i += 2;
    //                     break;
    //                 case '0':
    //                     fprintf(ostream, "pop ");
    //                     i += 3;
    //                     i += print_register(ostream, &code[i]);
    //                     break;
    //                 default:
    //                     i++;
    //                     break;
    //             }
    //             break;
    //         }
    //         case '3':
    //             fprintf(ostream, "sub ");
    //             i += 2;
    //             break;
    //         case '4':
    //             fprintf(ostream, "mul ");
    //             i += 2;
    //             break;
    //         case '5':
    //             fprintf(ostream, "div ");
    //             i += 2;
    //             break;
    //         case '6':
    //             fprintf(ostream, "out ");
    //             i += 2;
    //             break;
    //         case '7':
    //             fprintf(ostream, "in ");
    //             i += 2;
    //             i += print_number(ostream, &code[i]);
    //             break;
    //         case '8':
    //             fprintf(ostream, "sqrt ");
    //             i += 2;
    //             break;
    //         case '9':
    //             fprintf(ostream, "sin ");
    //             i += 2;
    //             break;
    //         default:
    //             i++;
    //             break;
    //     }
    //     fprintf(ostream, "\n");
    // }
