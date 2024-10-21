#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include "logger.h"
#include "label.h"
#include "vector.h"

static ssize_t find_name(vector_t* name_table, char* name);

//==================================================================================================

ssize_t return_label_address(vector_t* name_table, char* name, size_t* i) {
    assert(name_table != nullptr);
    assert(name != nullptr);
    assert(i != nullptr);

    ssize_t index = find_name(name_table, name);

    if (index == -1) {
        label_t label = {};
        strncpy(label.name, name, 10);
        label.offset = -1;

        vector_push_back_((vector_t*) name_table, &label, sizeof(label_t));
        *i = (vector_size(name_table) / sizeof(label_t)) - 1;
        return -1;
    }

    *i = (size_t) index;
    return ((label_t*) vector_element_ptr(name_table, (size_t) index, sizeof(label_t)))->offset;
}

label_status_t add_label(vector_t* name_table, char* name, size_t cmd_address) {
    assert(name_table != nullptr);
    assert(name != nullptr);
    assert(cmd_address != 0);

    ssize_t index = find_name(name_table, name);

    if (index == -1) {
        label_t label = {};
        strncpy(label.name, name, 10);
        label.offset = (ssize_t) cmd_address;
        vector_push_back(name_table, &label);
        return NEW;
    }

    if (((label_t*) vector_element_ptr(name_table, (size_t) index, sizeof(label_t)))->offset != -1) {
        return EXIST;
    }

    ((label_t*) vector_element_ptr(name_table, (size_t) index, sizeof(label_t)))->offset = (ssize_t) cmd_address;
    return ADD;
}

static ssize_t find_name(vector_t* name_table, char* name) {
    assert(name_table != nullptr);
    assert(name != nullptr);

    size_t i = 0;

    for (; i < vector_size(name_table) / sizeof(label_t); i++) {
        if (strstr(name, ((label_t*) vector_element_ptr(name_table, i, sizeof(label_t)))->name) != nullptr) {
            return (ssize_t) i;
        }
    }
    return -1;
}
//==================================================================================================
