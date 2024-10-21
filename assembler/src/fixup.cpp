#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "fixup.h"

//==================================================================================================

void fixup_dump(parent_t* fixup, size_t size, FILE* ostream) {
    if (ostream == nullptr) {
        ostream = stdout;
    }

    if (fixup == nullptr) {
        fprintf(ostream, "fixup adress is null\n");
        return;
    }

    fprintf(ostream, "[size]: %zu\n", size);

    for (size_t i = 0; i < size; i++) {
        fprintf(ostream, "[%zu]:\n\t[label index]: %zu\n\t[lable adress]: %zu\n",
                         i, fixup[i].label_index, fixup[i].label_addr);
    }
}

//==================================================================================================
