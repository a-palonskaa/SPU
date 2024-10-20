#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "fixup.h"

//==================================================================================================

void fixup_dump(fixup_t* fixup, FILE* ostream) {
    assert(fixup != nullptr);

    fprintf(ostream, "[size]: %zu\n[capacity]: %zu\n", fixup->size, fixup->capacity);

    if (fixup->parents == nullptr) {
        fprintf(ostream, "fixup->parents adress is nullptr\n");
        return;
    }

    for (size_t i = 0; i < fixup->size; i++) {
        fprintf(ostream, "[%zu]:\n\t[label index]: %zu\n\t[lable adress]: %zu\n",
                         i, fixup->parents[i].label_index, fixup->parents[i].label_addr);
    }
}

//==================================================================================================
