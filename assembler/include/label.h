#ifndef LABEL_H
#define LABEL_H

#include <stdio.h>

typedef struct {
    char name[10];
    ssize_t cmd_ptr;
} label_t;

typedef struct {
    label_t* label;
    size_t size;
    size_t capacity;
} name_table_t;

name_table_t* new_name_table(size_t capacity);
void name_table_dtor(name_table_t* name_table);
void name_table_dump(name_table_t* name_table, FILE* ostream);

bool add_label(name_table_t* name_table, char* name, size_t cmd_address);
ssize_t return_label_address(name_table_t* name_table, char* name, size_t* i);

#endif /* LABEL_H */
