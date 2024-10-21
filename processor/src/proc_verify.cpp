#include "ctype.h"
#include "proc_verify.h"
#include "processor.h"
#include "logger.h"
#include "verify.h"

//====================================================================================================

const char* MY_SIGNATURE = "aliffka";
const char* CURRENT_VERSION = "5.2";

//====================================================================================================

verify_t verify_file(FILE* istream, size_t* bytes_cnt) {
    char signature[50] = "";
    char version[50] = "";

    fgets(signature, 50, istream);
    fgets(version, 50, istream);

    size_t size = 0;
    fread(&size, sizeof(size_t), 1, istream);

    if (strstr(signature, MY_SIGNATURE) == nullptr) {
        LOG(ERROR, "INAVALID FILE SIGNATURE\n");
        return UNKNOWN_CREATOR;
    }

    if (strstr(version, CURRENT_VERSION) == nullptr) {
        LOG(ERROR, "INAPPROPRIATE VERSION OF CODE\n");
        return INAPPROPRIATE_VERSION;
    }

    *bytes_cnt = size;

    LOG(INFO, "FILE WAS VERIFIED\n@%s:[version]:%s\n\n", MY_SIGNATURE, CURRENT_VERSION);
    return VALID_HEADER;
}

//====================================================================================================

verify_t verify_processor(processor_t* processor) {
    assert(processor != nullptr);

    if (processor->code == nullptr) {
        LOG(ERROR, "NULL CODE POINTER\n");
        return NULL_CODE_PTR;
    }

    if (processor->stk == nullptr) {
        LOG(ERROR, "NULL STACK POINTER");
        return NULl_STACK_PTR;
    }

    if (processor->size == 0) {
        LOG(INFO, "EMPTY CODE\n");
        return EMPTY_CODE;
    }

    if (stack_error(processor->stk) != NO_ERRORS) {
        STACK_DUMP_(processor->stk);
        LOG(ERROR, "INVALID STACK\n");
        return INVALID_STACK;
    }
    return VALID_PROCESSOR;
}

//====================================================================================================

void processor_dump(FILE* ostream, processor_t* processor, const char* file, size_t line, const char* func) {
    assert(processor != nullptr);
    verify_t error_status = verify_processor(processor);

    fprintf(ostream, "%s:%zu(%s)\n", file, line, func);

    fprintf(ostream, "[error status]: %s\n", str_error_status(error_status));

    for (size_t i = 0; i < processor->size; i++) {
        fprintf(ostream, "%2zx ", i);
    }

    fprintf(ostream, "\n");

    for (size_t i = 0; i < processor->size; i++) {
        fprintf(ostream, "%2x ", processor->code[i]);
    }

    for (size_t i = 0; i < processor->ip; i++) {
        fprintf(ostream, "-");
    }

    fprintf(ostream, "^");

    for (size_t i = processor->ip; i < processor->size; i++) {
        fprintf(ostream, "-");
    }

    fprintf(ostream, "\n");
}

const char* str_error_status(verify_t error_status) {
    switch(error_status) {
        CASE_(VALID_HEADER);
        CASE_(VALID_PROCESSOR);
        CASE_(UNKNOWN_CREATOR);
        CASE_(INAPPROPRIATE_VERSION);
        CASE_(NULL_REGISTRES_PTR);
        CASE_(NULL_CODE_PTR);
        CASE_(NULl_STACK_PTR);
        CASE_(EMPTY_CODE);
        CASE_(INVALID_STACK);
        default:
            break;
    }
}

//====================================================================================================
