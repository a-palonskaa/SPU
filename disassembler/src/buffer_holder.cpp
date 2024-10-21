#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "logger.h"
#include "buffer_holder.h"
#include "text_lib.h"

const char* SIGNATURE = "aliffka";
const char* VERSION = "[version]: 5.2";

buffer_status_t buffer_ctor(buffer_holder_t* buffer_holder, FILE* istream) {
    assert(buffer_holder != nullptr);

    size_t code_size = (size_t) find_file_size(istream);

    unsigned char* code = (unsigned char*) calloc(code_size, sizeof(char));
    if (code == nullptr) {
        LOG(ERROR, "Failed to allocate memory\n" STRERROR(errno));
        return MEMORY_ALLOCATION_ERROR;
    }
    buffer_holder->code = code;

    if ((fread(code, sizeof(char), code_size, istream) != code_size) &&
        !feof(istream) && ferror(istream)) {
        LOG(ERROR, "Failed to read the file\n" STRERROR(errno));
        return FILE_READING_ERROR;
    }

    return buffer_validation(buffer_holder, code_size);
}

void buffer_dtor(buffer_holder_t* buffer_holder) {
    assert(buffer_holder != nullptr);

    free(buffer_holder->code);
    buffer_holder->code = nullptr;
    buffer_holder->bytecode = nullptr;

    buffer_holder->bytecode_size = 0;
}

buffer_status_t buffer_validation(buffer_holder_t* buffer_holder, size_t code_size) {
    assert(buffer_holder != nullptr);

     unsigned char* second_string = (unsigned char*) strstr((char*) buffer_holder->code, "\n");
    if (second_string == nullptr) {
        LOG(ERROR, "Invalid format: file has to have signature & version\n");
        return INVALID_FORMAT;
    }

    if (strnstr((char*) buffer_holder->code, SIGNATURE,
       (size_t) (second_string - buffer_holder->code)) == nullptr) {
        *second_string = '\0';
        LOG(ERROR, "Signature dismatch, expecting %s, recieving %s\n", &SIGNATURE, buffer_holder->code);
        *second_string = '\n';
        return SIGNATURE_DISMATCH;
    }

    unsigned char* third_string = (unsigned char*) strstr((char*) second_string + 1, "\n");
    if (third_string == nullptr) {
        LOG(ERROR, "Invalid format: file has to have signature & version\n");
        return INVALID_FORMAT;
    }

    if (strnstr((char*) second_string, VERSION, (size_t) (third_string - second_string)) == nullptr) {
        LOG(ERROR, "Versions dismatch, expecting %s, recieving %s %d\n", VERSION, second_string);
        return VERSION_DISMATCH;
    }

    size_t bytecode_size = code_size - (size_t) (third_string - buffer_holder->code) - sizeof(size_t) - 1;
    size_t read_size = 0;
    memcpy(&read_size, (char*) third_string + 1, sizeof(size_t));

    if (read_size != bytecode_size) {
        LOG(ERROR, "Decoded size is %zu when real is %zu\n", read_size, bytecode_size);
        return SIZE_DISMATCH;
    }

    buffer_holder->bytecode = third_string  + 1 + sizeof(size_t);
    buffer_holder->bytecode_size = bytecode_size;
    LOG(INFO, "Code successfully passed validation\n");
    return VALID;
}
