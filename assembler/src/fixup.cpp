#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include "logger.h"
#include "fixup.h"

const size_t MIN_FIXUP_CAPACITY = 2;

fixup_t* new_fixup(size_t capacity) {
    if (capacity < MIN_FIXUP_CAPACITY) {
        capacity = MIN_FIXUP_CAPACITY;
    }

    fixup_t* fixup = (fixup_t*) calloc(sizeof(fixup_t), sizeof(char));
    if (fixup == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        return nullptr;
    }

    parent_t* parents = (parent_t*) calloc(sizeof(parent_t), capacity);
    if (parents == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        free(fixup);
        return nullptr;
    }

    fixup->parents = parents;
    fixup->capacity = capacity;
    fixup->size = 0;

    for (size_t i = 0; i < capacity; i++) {
        fixup->parents[i].label_index = 0;
        fixup->parents[i].label_addr = 0;
    }

    return fixup;
}

void fixup_dtor(fixup_t* fixup) {
    if (fixup == nullptr) {
        return;
    }

    free(fixup->parents);
    fixup->parents = nullptr;

    fixup->size = 0;
    fixup->capacity = 0;

    free(fixup);
    fixup = nullptr;
}

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
