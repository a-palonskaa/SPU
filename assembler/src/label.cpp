#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include "logger.h"
#include "label.h"

static ssize_t find_name(name_table_t* name_table, char* name);

const size_t MIN_LABEL_CAPACITY = 2;

name_table_t* new_name_table(size_t capacity) {
    name_table_t* name_table = (name_table_t*) calloc(sizeof(name_table_t), sizeof(char));
    if (name_table == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        return nullptr;
    }

    if (capacity <  MIN_LABEL_CAPACITY) {
        capacity = MIN_LABEL_CAPACITY;
    }

    name_table->label = (label_t*) calloc(capacity, sizeof(label_t));
    if (name_table->label == nullptr) {
        LOG(ERROR, "MEMORY ALLOCATION ERROR" STRERROR(errno));
        free(name_table);
        return nullptr;
    }

    name_table->size = 0;
    name_table->capacity = capacity;

    for (size_t i = 0; i < capacity; i++) {
        name_table->label[i].cmd_ptr = -1;
    }

    return name_table;
}

void name_table_dtor(name_table_t* name_table) {
    free(name_table->label);
    name_table->label = nullptr;

    name_table->size = 0;
    name_table->capacity = 0;

    free(name_table);
    name_table = nullptr;
}

//==================================================================================================

void name_table_dump(name_table_t* name_table, FILE* ostream) {
    assert(name_table != nullptr);

    for (size_t i = 0; i < name_table->size; i++) {
        fprintf(ostream, "[%zu]\n\t[label name]: %s\n\t[label points at]: %p\n-------------\n",
                         i, name_table->label[i].name, (void*) name_table->label[i].cmd_ptr);
    }

}

//==================================================================================================

ssize_t return_label_address(name_table_t* name_table, char* name, size_t* i) {
    assert(name_table != nullptr);
    assert(name != nullptr);

    ssize_t index = find_name(name_table, name);

    if (index == -1) {
        strcpy(name_table->label[name_table->size].name, name);
        *i = name_table->size;
        name_table->label[name_table->size++].cmd_ptr = -1;
        return -1;
    }

    *i = (size_t) index;
    return name_table->label[index].cmd_ptr;
}

bool add_label(name_table_t* name_table, char* name, size_t cmd_address) {
    assert(name_table != nullptr);
    assert(name != nullptr);
    assert(cmd_address != 0);

    ssize_t index = find_name(name_table, name);
    if (index == -1) {
        strcpy(name_table->label[name_table->size].name, name);
        name_table->label[name_table->size++].cmd_ptr = (ssize_t) cmd_address;
        return true;
    }

    if (name_table->label[index].cmd_ptr != -1) {
        return false;
    }

    name_table->label[index].cmd_ptr = (ssize_t) cmd_address;
    return true;
}

static ssize_t find_name(name_table_t* name_table, char* name) {
    assert(name_table != nullptr);
    assert(name != nullptr);

    size_t i = 0;
    for (; i < name_table->size; i++) {
        if (strstr(name, name_table->label[i].name) != nullptr) {
            return (ssize_t) i;
        }
    }

    return -1;
}
//==================================================================================================
