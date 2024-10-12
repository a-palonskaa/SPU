#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include "asm.h"
#include "logger.h"

static void print_register(char* cmd, unsigned char* code);
static size_t print_number(char* cmd, unsigned char* code);

void assemble(FILE* istream, FILE* ostream) {
    assert(istream != nullptr);
    assert(ostream != nullptr);
    size_t bytes_cnt = 0;
    size_t array_size_bytes = 0;
    size_t file_size = (size_t) find_file_size(istream);
    unsigned char* code = (unsigned char*)calloc(file_size, sizeof(char));
    if (code == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        return;
    }

    char cmd[50] = "";
    while (fgets(cmd, 50, istream)) {
        if (strstr(cmd, "push") != nullptr) {
            if (strstr(cmd, "x") != nullptr) {
                memcpy(&code[bytes_cnt], "19 ", 3);
                bytes_cnt += 3;
                //fprintf(ostream, "19 ");
                print_register(cmd + 4, &code[bytes_cnt]);
                bytes_cnt += 2;
                //fprintf(ostream, " ");
            }
            else {
                memcpy(&code[bytes_cnt], "1 ", 2);
                //fprintf(ostream, "1 ");
                bytes_cnt += 2;
                bytes_cnt += print_number(cmd + 4, &code[bytes_cnt]);
            }
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "add") != nullptr) {
            //fprintf(ostream, "2 ");
            memcpy(&code[bytes_cnt], "2 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "sub") != nullptr) {
            //fprintf(ostream, "3 ");
            memcpy(&code[bytes_cnt], "3 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "mul") != nullptr) {
            //fprintf(ostream, "4 ");
            memcpy(&code[bytes_cnt], "4 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "div") != nullptr) {
            //fprintf(ostream, "5 ");
            memcpy(&code[bytes_cnt], "5 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "out") != nullptr) {
            //fprintf(ostream, "6 ");
            memcpy(&code[bytes_cnt], "6 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "in") != nullptr) {
            //fprintf(ostream, "7 ");
            memcpy(&code[bytes_cnt], "7 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "sqrt") != nullptr) {
            //fprintf(ostream, "8 ");
            memcpy(&code[bytes_cnt], "8 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "sin") != nullptr) {
            //fprintf(ostream, "9 ");
            memcpy(&code[bytes_cnt], "9 ", 2);
            bytes_cnt += 2;
            array_size_bytes++;
        }
        else if (strstr(cmd, "cos") != nullptr) {
            memcpy(&code[bytes_cnt], "10 ", 3);
            bytes_cnt += 3;
            array_size_bytes++;
            //fprintf(ostream, "10 ");
        }
        else if (strstr(cmd, "dump") != nullptr) {
            //fprintf(ostream, "11 ");
            memcpy(&code[bytes_cnt], "11 ", 3);
            bytes_cnt += 3;
            array_size_bytes++;
        }
        else if (strstr(cmd, "hlt") != nullptr) {
            //fprintf(ostream, "12 ");
            memcpy(&code[bytes_cnt], "12 ", 3);
            bytes_cnt += 3;
            array_size_bytes++;
        }
        else if (strstr(cmd, "ja") != nullptr) {
            //fprintf(ostream, "13 ");
            memcpy(&code[bytes_cnt], "13 ", 3);
            bytes_cnt += 3;
            bytes_cnt += print_number(cmd + 2, &code[bytes_cnt]);
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "jae") != nullptr) {
            //fprintf(ostream, "14 ");
            memcpy(&code[bytes_cnt], "14 ", 3);
            bytes_cnt += 3;
            bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "jb") != nullptr) {
            //fprintf(ostream, "15 ");
            memcpy(&code[bytes_cnt], "15 ", 3);
            bytes_cnt += 3;
            bytes_cnt += print_number(cmd + 2, &code[bytes_cnt]);
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "jbe") != nullptr) {
            //fprintf(ostream, "16 ");
            memcpy(&code[bytes_cnt], "16 ", 3);
            bytes_cnt += 3;
            bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "jne") != nullptr) {
            //fprintf(ostream, "17 ");
            memcpy(&code[bytes_cnt], "17 ", 3);
            bytes_cnt += 3;
            bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "jmp") != nullptr) {
            //fprintf(ostream, "18 ");
            memcpy(&code[bytes_cnt], "18 ", 3);
            bytes_cnt += 3;
            bytes_cnt += print_number(cmd + 3, &code[bytes_cnt]);
            array_size_bytes += 1 + sizeof(double);
        }
        else if (strstr(cmd, "pop") != nullptr) {
            //fprintf(ostream, "20 ");
            memcpy(&code[bytes_cnt], "20 ", 3);
            bytes_cnt += 3;
            print_register(cmd, &code[bytes_cnt]);
            bytes_cnt += 2;
            array_size_bytes += 1 + sizeof(double);
            //fprintf(ostream, " ");
        }
    }
    code[++bytes_cnt] = '\0';

    print_header(ostream, array_size_bytes);
    fputs((const char*) code, ostream);

    free(code);
    code = nullptr;

    LOG(INFO, "SUCCESSFULLY FINISHED ASSEMBLING\n");
}

//==================================================================================================

static size_t print_number(char* cmd, unsigned char* code) {
    size_t i = 0, j = 0;

    while(isspace(cmd[i])) {
        i++;
    }

    while (!isspace(cmd[i])) {
        *code++ = (unsigned char) cmd[i++];
        j++;
    }

    *code = ' ';
    return j + 1;
    // cmd[i - 1] = '\0';
    // fputs(cmd, ostream);
}

static void print_register(char* cmd, unsigned char* code) {
    switch (*(strstr(cmd, "x") - 1)) {
        case 'a':
            //fprintf(ostream, "1");
            *code = '1';
            break;
        case 'b':
            //fprintf(ostream, "2");
            *code = '2';
            break;
        case 'c':
            //fprintf(ostream, "3");
            *code = '3';
            break;
        case 'd':
            //fprintf(ostream, "4");
            *code = '4';
            break;
        case 'e':
            //fprintf(ostream, "5");
            *code = '5';
            break;
        case 'f':
            //fprintf(ostream, "6");
            *code = '6';
            break;
        case 'g':
            //fprintf(ostream, "7");
            *code = '7';
            break;
        case 'h':
            //fprintf(ostream, "8");
            *code = '8';
            break;
        default:
            break;
    }
    *++code = ' ';
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
