#ifndef LABEL_H
#define LABEL_H

#define NAME_MAX_LEN 10

#include <stdio.h>
#include "vector.h"

typedef struct {
    char name[NAME_MAX_LEN];
    ssize_t offset;
} label_t;

typedef enum {
    NEW = 0,
    ADD = 1,
    EXIST = 2,
} label_status_t;

ssize_t return_label_address(vector_t* name_table, char* name, size_t* i);
label_status_t add_label(vector_t* name_table, char* name, size_t cmd_address);

#endif /* LABEL_H */
