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
//
// void name_table_dump(void* name_table, FILE* ostream) {
//     assert(name_table != nullptr);
//
//     for (size_t i = 0; i < name_table->size; i++) {
//         fprintf(ostream, "[%zu]\n\t[label name]: %s\n\t[label points at]: %p\n-------------\n",
//                          i, name_table->label[i].name, (void*) name_table->label[i].offset);
//     }
// }

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
        printf("new label name is %s %p %p\n", name, &label.name, &label.offset);
        vector_push_back_((vector_t*) name_table, &label, sizeof(label_t));
        *i = (vector_size(name_table) / sizeof(label_t)) - 1;

        printf("[check]:  %s || %p || %p || %p\n", ((label_t*) vector_element_ptr(name_table, (size_t) 0, sizeof(label_t)))->name,
        ((label_t*) vector_element_ptr(name_table, (size_t) 0, sizeof(label_t)))->name,
        ((label_t*) vector_element_ptr(name_table, (size_t) 0, sizeof(label_t))),
        strstr(name, (char*) vector_element_ptr(name_table, (size_t) 0, sizeof(label_t))));

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
    printf("FOUND index is %zu\n", index);
    if (index == -1) {
        label_t label = {};
        strncpy(label.name, name, 10);
        label.offset = (ssize_t) cmd_address;
        printf("new label name is %s %p %p\n", name, &label.name, &label.offset);
        vector_push_back(name_table, &label);
        printf("PUSHED\n");
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
    printf("SIZE IS %zu wanna find %s\n", vector_size(name_table), name);
    for (; i < vector_size(name_table) / sizeof(label_t); i++) {
        printf("%p , %p,  %d\n", (char*) name_table->data + sizeof(label_t) * i, ((label_t*) vector_element_ptr(name_table, i, sizeof(label_t)))->name, strstr(name, ((label_t*) vector_element_ptr(name_table, i, sizeof(label_t)))->name) != nullptr);
        if (strstr(name, ((label_t*) vector_element_ptr(name_table, i, sizeof(label_t)))->name) != nullptr) {
            printf("did!");
            return (ssize_t) i;
        }
    }
    printf("finished\n");
    return -1;
}
//==================================================================================================
