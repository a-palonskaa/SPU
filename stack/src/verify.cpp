#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "verify.h"
#include "my_stack.h"

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/vm_region.h>
#include <mach/mach_vm.h>
#endif /* APPLE */

#ifdef HASH_PROTECT
#include "hash.h"
#endif /* HASH_PROTECT */

static void print_time(FILE *ostream);
static const char* stack_error_message(error_t error);
static FILE** stack_dump_ostream();

//------------------------------------------------------------------------------------------------

static FILE** stack_dump_ostream() {
    static FILE* out = nullptr;
    return &out;
}

void set_stack_dump_ostream(FILE* ostream) {
    *stack_dump_ostream() = ostream;
}

//------------------------------------------------------------------------------------------------

error_t stack_assert(my_stack_t* stk, const char* file, size_t line, const char* func) {
    if (stack_error(stk) != NO_ERRORS && stk->error != NO_ELEMNTS_TO_POP_ERROR) {
        stack_dump(stk, file, line, func);
        stack_dtor(stk);
        assert(0);
    }
    return NO_ERRORS;
}

error_t stack_dump(const my_stack_t* stk, const char* file, size_t line, const char* func) {
    assert(stk != nullptr);
    assert(file != nullptr);
    assert(line > 0);

    FILE* ostream = *stack_dump_ostream();
    if (ostream == nullptr) {
        ostream = stdout;
    }

    ptr_protect_t ptr_permissions = validate_ptr(stk->data);

    fprintf(ostream, "stack_t [%p]\n", stk);
    print_time(ostream);
    fprintf(ostream, "\n");
    fprintf(ostream, "called from %s:%zu(%s)\n", file, line, func);
#ifdef DEBUG
    fprintf(ostream, "name ""%s"" born at %s:%zu(%s)\n",
                     stk->location_info.stack_name, stk->location_info.file,
                     stk->location_info.line, stk->location_info.func);
#endif
    fprintf(ostream, "\n[ERROR STATUS]: ");
    fprintf(ostream, "%s\n", stack_error_message(stk->error));
    fprintf(ostream, "element width = %zu\n", stk->elm_width);
    fprintf(ostream, "capacity = %zu\n", stk->capacity);
    fprintf(ostream, "size = %zu\n\n", stk->size);
    if (ptr_permissions) {
        fprintf(ostream, "data[%p]\n", stk->data);
#ifdef DEBUG
#ifdef __APPLE__
        fprintf(ostream,"[data permissions]: ");
        if (ptr_permissions & READ) {
            fprintf(ostream, "READ ");
        }
        if (ptr_permissions & WRITE) {
            fprintf(ostream, "WRITE ");
        }
        if (ptr_permissions & EXECUTE) {
            fprintf(ostream, "EXECUTE");
        }
        fprintf(ostream, "\n");
#endif /* __APPLE__ */
#endif /* DEBUG */
        fprintf(ostream, "{\n");
        for (size_t i = 0; i < stk->capacity; i++) {
            if (!memcmp((char*) stk->data + i * stk->elm_width, stk->poison_value_buffer, stk->elm_width)) {
                fprintf(ostream, " [%08zX] = ", i);
                stk->print((char*) stk->data + i * stk->elm_width, ostream);
                fprintf(ostream, "  (poison value)");
            }
            else {
                fprintf(ostream, "*[%08zX] = ", i);
                stk->print((char*) stk->data + i * stk->elm_width, ostream);
            }
            fprintf(ostream, "\n");
        }
    }
    fprintf(ostream, "}\n\n");
#ifdef CANARY_PROTECT
    fprintf(ostream, "LEFT  STACK CANARY PROTECT: \n\t%llX(without mask)\n\t%llx(with mask)\n",
                     stk->left_canary,  stk->left_canary  ^ (canary_t) stk);
    fprintf(ostream, "RIGHT STACK CANARY PROTECT: \n\t%llX(without mask)\n\t%llx(with mask)\n\n",
                     stk->rigth_canary, stk->rigth_canary ^ (canary_t) stk);

    if (ptr_permissions) {
        canary_t rigth_canary = 0;
        canary_t left_canary  = 0;

        memcpy(&left_canary, (char*) stk->data - sizeof(canary_t), sizeof(canary_t));
        memcpy(&rigth_canary, (char*) stk->data + stk->capacity * stk->elm_width, sizeof(canary_t));

        fprintf(ostream, "LEFT DATA CANARY PROTECT:  %llX\n", left_canary);
        fprintf(ostream, "RIGHT DATA CANARY PROTECT: %llX\n\n", rigth_canary);
    }
#endif /* CANARY_PROTECT */
#ifdef HASH_PROTECT
    fprintf(ostream, "STACK HASH:  " HASH_SPEC "\nDATA HASH:   " HASH_SPEC "\n",
                     stk->hash_data, stk->hash_stack);
#endif /* HASH_PROTECTION */
    fprintf(ostream, "---------------------------------------------------------------------------------\n\n");
    return stk->error;
}

//------------------------------------------------------------------------------------------------

static void print_time(FILE *ostream) {
    assert(ostream != nullptr);

    time_t mytime = time(NULL);
    struct tm *time = localtime(&mytime);

    fprintf(ostream, "%02d.%02d.%d %02d:%02d:%02d ",
            time->tm_mday, time->tm_mon + 1, time->tm_year + 1900,
            time->tm_hour, time->tm_min,     time->tm_sec);
}

static const char* stack_error_message(error_t error) {
    switch(error) {
        CASE_(NO_ERRORS);
        CASE_(MEMORY_ALLOCATION_ERROR);
        CASE_(MEMORY_REALLOCATION_ERROR);
        CASE_(ELEMENT_WIDTH_ERROR);
        CASE_(NO_ELEMNTS_TO_POP_ERROR);
        CASE_(NULL_CAPACITY_ERROR);
        CASE_(INVALID_SIZE_ERROR);
        CASE_(DATA_INVALID_POINTER_ERROR);
        CASE_(NULL_ELEMENT_WIDTH_ERROR);
        CASE_(STACK_ALREADY_INITIALIZED_ERROR);
        CASE_(CAPACITY_LIMIT_EXCEED_ERROR);
        CASE_(NON_VALID_POINTER_ERROR);
#ifdef CANARY_PROTECT
        CASE_(LEFT_CANARY_PROTECT_FAILURE);
        CASE_(RIGTH_CANARY_PROTECT_FAILURE);
        CASE_(DATA_CANARY_PROTECT_FAILURE);
#endif /* CANARY_PROTECT */
#ifdef HASH_PROTECT
        CASE_(HASH_PROTECTION_FAILED);
#endif /* HASH_PROTECT */
        default:
            return "UNDEFINED ERROR";
            break;
    }
}

//------------------------------------------------------------------------------------------------

error_t stack_error(my_stack_t* stk) {
    assert(stk != nullptr);

#ifdef CANARY_PROTECT
    if ((stk->left_canary ^ (canary_t) stk) != LEFT_CANARY_MASK) {
        stk->error = LEFT_CANARY_PROTECT_FAILURE;
        return LEFT_CANARY_PROTECT_FAILURE;
    }

    if ((stk->rigth_canary ^ (canary_t) stk) != RIGTH_CANARY_MASK) {
        stk->error = RIGTH_CANARY_PROTECT_FAILURE;
        return RIGTH_CANARY_PROTECT_FAILURE;
    }
#endif /* CANARY_PROTECT */

    if (stk->error != NO_ERRORS) {
        return stk->error;
    }

    if (!stk->capacity) {
        stk->error = NULL_CAPACITY_ERROR;
        return NULL_CAPACITY_ERROR;
    }

    if (stk->capacity >= MAX_CAPACITY) {
        stk->error = CAPACITY_LIMIT_EXCEED_ERROR;
        return CAPACITY_LIMIT_EXCEED_ERROR;
    }

    if (stk->capacity < stk->size) {
        stk->error = INVALID_SIZE_ERROR;
        return INVALID_SIZE_ERROR;
    }

    ptr_protect_t ptr_permissions = validate_ptr(stk->data);
    if (!ptr_permissions) {
        stk->error = DATA_INVALID_POINTER_ERROR;
        return DATA_INVALID_POINTER_ERROR;
    }

#ifdef __APPLE__
    stk->data_permissions = ptr_permissions;
#endif /* __APLE__ */

    if (!stk->elm_width) {
        stk->error = NULL_ELEMENT_WIDTH_ERROR;
        return NULL_ELEMENT_WIDTH_ERROR;
    }

#ifdef CANARY_PROTECT
    if (memcmp((canary_t*) stk->data - 1, &DATA_CANARY, sizeof(canary_t)) ||
        memcmp((char*) stk->data + stk->capacity * stk->elm_width, &DATA_CANARY, sizeof(canary_t))) {
        stk->error = DATA_CANARY_PROTECT_FAILURE;
        return DATA_CANARY_PROTECT_FAILURE;
    }
#endif /* CANARY_PROTECT */

#ifdef HASH_PROTECT
    hash_t hash_data = 0;
    hash_t hash_stack = 0;
    stack_hash(stk, &hash_data, &hash_stack);
    if (hash_data != stk->hash_data || hash_stack != stk->hash_stack) {
        stk->error = HASH_PROTECTION_FAILED;
        return HASH_PROTECTION_FAILED;
    }
#endif /* HASH_PROTECT */

    stk->error = NO_ERRORS;
    return NO_ERRORS;
}

//------------------------------------------------------------------------------------------------

ptr_protect_t validate_ptr(const void* ptr) {
    if (!ptr) {
        return NONE;
    }

#ifdef __APPLE__
    bool reading_permission   = false;
    bool writing_permission   = false;
    bool execution_permission = false;

    mach_port_t task = mach_task_self();
    kern_return_t kret;
    vm_region_basic_info_data_t info;
    mach_vm_size_t size;
    mach_port_t object_name;
    mach_msg_type_number_t count;
    count = VM_REGION_BASIC_INFO_COUNT_64;
    mach_vm_address_t address = 0;

#ifdef __LP64__
    memcpy(&address, &ptr, sizeof(uint64_t));
#elif defined(__LP32__)
    memset(&adress, 0, sizeof(uint64_t));
    memcpy(&address + sizeof(uint32_t), &ptr, sizeof(uint32_t));
#endif /*__LP64__*/
    kret = mach_vm_region(task, &address, &size, VM_REGION_BASIC_INFO,
                         (vm_region_info_t) &info, &count, &object_name);
    if (kret == KERN_SUCCESS) {
        ON_DEBUG(printf("Address: 0x%llx Size: 0x%llx\n", address, size);)

        if (info.protection & VM_PROT_NONE) {
            return NONE;
        }

        reading_permission   = info.protection & VM_PROT_READ;
        writing_permission   = info.protection & VM_PROT_WRITE;
        execution_permission = info.protection & VM_PROT_EXECUTE;
    }
// FIXME - create enum with 8 options, return info.protection, and then decode it
    if (reading_permission) {
        if (writing_permission) {
            if (execution_permission) {
                return ALL;
            }
            return DEFAULT;
        }
        return READ;
    }
    return WRITE;
#endif /* __APPLE__ */
    return ALL;
}

//------------------------------------------------------------------------------------------------
