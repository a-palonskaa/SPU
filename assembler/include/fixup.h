#ifndef FIXUP_H
#define FIXUP_H

#include <stdio.h>

typedef struct {
    size_t label_index;
    size_t label_addr;
} parent_t;

void fixup_dump(parent_t* fixup, size_t size, FILE* ostream);

#endif /* FIXUP_H */
