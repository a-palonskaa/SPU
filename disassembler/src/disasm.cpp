#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include "disasm.h"

static size_t print_number(FILE* ostream, unsigned char* code);
static size_t print_register(FILE* ostream, unsigned char* code);

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
        switch(code[i]) {
            case '1': {
                switch(code[i + 1]) {
                    case ' ':
                        fprintf(ostream, "push ");
                        i += 2;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '0':
                        fprintf(ostream, "cos ");
                        i += 3;
                        break;
                    case '1':
                        fprintf(ostream, "dump ");
                        i += 3;
                        break;
                    case '2':
                        fprintf(ostream, "hlt ");
                        i += 3;
                        break;
                    case '3':
                        fprintf(ostream, "ja ");
                        i += 3;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '4':
                        fprintf(ostream, "jae ");
                        i += 3;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '5':
                        fprintf(ostream, "jb ");
                        i += 3;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '6':
                        fprintf(ostream, "jbe ");
                        i += 3;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '7':
                        fprintf(ostream, "jne ");
                        i += 3;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '8':
                        fprintf(ostream, "jmp ");
                        i += 3;
                        i += print_number(ostream, &code[i]);
                        break;
                    case '9':
                        fprintf(ostream, "push ");
                        i += 3;
                        i += print_register(ostream, &code[i]);
                        break;
                    default:
                        i++;
                        break;
                }
                break;
            }
            case '2': {
                switch(code[i + 1]) {
                    case ' ':
                        fprintf(ostream, "add ");
                        i += 2;
                        break;
                    case '0':
                        fprintf(ostream, "pop ");
                        i += 3;
                        i += print_register(ostream, &code[i]);
                        break;
                    default:
                        i++;
                        break;
                }
                break;
            }
            case '3':
                fprintf(ostream, "sub ");
                i += 2;
                break;
            case '4':
                fprintf(ostream, "mul ");
                i += 2;
                break;
            case '5':
                fprintf(ostream, "div ");
                i += 2;
                break;
            case '6':
                fprintf(ostream, "out ");
                i += 2;
                break;
            case '7':
                fprintf(ostream, "in ");
                i += 2;
                i += print_number(ostream, &code[i]);
                break;
            case '8':
                fprintf(ostream, "sqrt ");
                i += 2;
                break;
            case '9':
                fprintf(ostream, "sin ");
                i += 2;
                break;
            default:
                i++;
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

static size_t print_number(FILE* ostream, unsigned char* code) {
    size_t i = 0;
    while (!isspace(code[i])) {
        fprintf(ostream, "%c", code[i++]);
    }
    fprintf(ostream, " ");
    return i + 1;
}

static size_t print_register(FILE* ostream, unsigned char* code) {
    switch (code[0]) {
        case '1':
            fprintf(ostream, "ax");
            break;
        case '2':
            fprintf(ostream, "bx");
            break;
        case '3':
            fprintf(ostream, "cx");
            break;
        case '4':
            fprintf(ostream, "dx");
            break;
        case '5':
            fprintf(ostream, "ex");
            break;
        case '6':
            fprintf(ostream, "fx");
            break;
        case '7':
            fprintf(ostream, "gx");
            break;
        case '8':
            fprintf(ostream, "hx");
            break;
        default:
            break;
    }
    fprintf(ostream, " ");
    return 2;
}
