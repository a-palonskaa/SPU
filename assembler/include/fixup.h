#ifndef FIXUP_H
#define FIXUP_H

#include <stdio.h>

typedef struct {
    size_t label_index;
    size_t label_addr;
} parent_t;

// TODO: vector
typedef struct {
    parent_t* parents;
    size_t size;
    size_t capacity;
} fixup_t;

fixup_t* new_fixup(size_t capacity);
void fixup_dtor(fixup_t* fixup);
void fixup_dump(fixup_t* fixup, FILE* ostream);

#endif /* FIXUP_H */
