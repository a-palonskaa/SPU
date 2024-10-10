#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "asm.h"


void assemble(FILE* istream, FILE* ostream) {
    char cmd[50] = "";

    while (fgets(cmd, 50, istream)) {
        if (strstr(cmd, "push") != nullptr) {
            fprintf(ostream, "1 ");
            fprintf(ostream, "%s", cmd + 4);
        }
        else if (strstr(cmd, "add") != nullptr) {
            fprintf(ostream, "2\n");
        }
        else if (strstr(cmd, "sub") != nullptr) {
            fprintf(ostream, "3\n");
        }
        else if (strstr(cmd, "mul") != nullptr) {
            fprintf(ostream, "4\n");
        }
        else if (strstr(cmd, "div") != nullptr) {
            fprintf(ostream, "5\n");
        }
        else if (strstr(cmd, "out") != nullptr) {
            fprintf(ostream, "6\n");
        }
        else if (strstr(cmd, "in") != nullptr) {
            fprintf(ostream, "7\n");
        }
        else if (strstr(cmd, "sqrt") != nullptr) {
            fprintf(ostream, "8\n");
        }
        else if (strstr(cmd, "sin") != nullptr) {
            fprintf(ostream, "9\n");
        }
        else if (strstr(cmd, "cos") != nullptr) {
            fprintf(ostream, "10\n");
        }
        else if (strstr(cmd, "dump") != nullptr) {
            fprintf(ostream, "11\n");
        }
        else if (strstr(cmd, "hlt") != nullptr) {
            fprintf(ostream, "12\n");
        }
    }
}
