#include <stdio.h>
#include "asm.h"

int main() {
    FILE* istream = fopen("files/in1.txt", "r");
    FILE* ostream = fopen("files/out.txt", "w");

    assemble(istream, ostream);

    fclose(istream);
    fclose(ostream);
}
