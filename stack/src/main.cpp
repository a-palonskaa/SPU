#include <stdio.h>

#include "stack.h"
#include "pop_push.h"
#include "verify.h"
#include "define.h"
#include "printers.h"

#ifndef HASH_PROTECT
#include "hash.h"
#endif /* HASH_PROTECT */

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/vm_region.h>
#include <mach/mach_vm.h>
#endif /* APPLE */

int main() {

    const int BASE_CAPACITY = 2;

    FILE* ostream = fopen("logger.txt", "w");
    if (!ostream) {
        ostream = stdout;
    }

    set_stack_dump_ostream(ostream);

    my_stack_t* stk = NEW_STACK_(4, BASE_CAPACITY, print_int);

    int el = 0;
    stack_push(stk, &el);

    char* error = (char*) stk;
    for (size_t i = 0; i < 24; i++) {
        printf("%x\n", error[i]);
    }
    int mass = 0;
     printf("\n%x\n", error[16]);
      printf("%x\n", error[17]);
    memcpy(&error[24], &mass, 2);

    delete_stack(stk);

    if (fclose(ostream) == EOF) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
