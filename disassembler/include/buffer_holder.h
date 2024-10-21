#ifndef BUFFER_HOLDER_H
#define BUFFER_HOLDER_H

typedef struct {
    unsigned char* code;
    unsigned char* bytecode;
    size_t bytecode_size;
} buffer_holder_t;

typedef enum {
    VALID = 0,
    SIGNATURE_DISMATCH = 1,
    VERSION_DISMATCH =2,
    SIZE_DISMATCH = 3,
    FILE_READING_ERROR = 4,
    MEMORY_ALLOCATION_ERROR = 5,
    INVALID_FORMAT = 6,
} buffer_status_t;

buffer_status_t buffer_ctor(buffer_holder_t* buffer_holder, FILE* istream);
void buffer_dtor(buffer_holder_t* buffer_holder);
buffer_status_t buffer_validation(buffer_holder_t* buffer_holder, size_t code_size);

#endif /* BUFFER_HOLDER_H */
