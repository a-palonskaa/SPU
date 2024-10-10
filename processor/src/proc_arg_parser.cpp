#include "proc_arg_parser.h"
#include <assert.h>

//----------------------------------------------------------------------------------------------

const char* OUTPUT = "files/out.asm";
const char* INPUT = "files/in1.asm";

static void InitiallizeValidationFlags(void* flag);
static void PrintHelp();;

//----------------------------------------------------------------------------------------------

cmd_error_t ChangeFlagInputFile(void* flag, const char* arg) {
    assert(flag != nullptr);
    (void) arg;
    flags_t* flags = (flags_t*) flag;

    flags->input_valid++;
    flags->input_file_name = arg;

    return NO_CMD_ERRORS;
}

cmd_error_t ChangeFlagOutputFile(void* flag, const char* arg) {
    assert(flag != nullptr);
    (void) arg;
    flags_t* flags = (flags_t*) flag;

    flags->output_valid++;
    flags->output_file_name = arg;

    return NO_CMD_ERRORS;
}

//----------------------------------------------------------------------------------------------

cmd_error_t Help(void* flag, const char* arg) {
    (void) arg;
    (void) flag;

    PrintHelp();
    return INPUT_ERROR;
}

static void PrintHelp() {
    for (size_t i = 0; i < COMMANDS_ARRAY_LENGTH; i++) {
        printf("%10s %10s %-10s" "\n", COMMANDS[i].name,
                                    COMMANDS[i].long_name,
                                    COMMANDS[i].description);
    }
}

cmd_error_t ValidateInput(const void* flag) {
    assert(flag != nullptr);
    const flags_t* flags = (const flags_t*) flag;

    if (flags->output_valid > 1 ||
        flags->input_valid > 1) {
        printf("MUTUAL DESTRUCTION ERROR\n");
        return INPUT_ERROR;
    }
    return NO_CMD_ERRORS;
}

//----------------------------------------------------------------------------------------------

void InitiallizeFlagsHeaderSort(void* flag) {
    assert(flag != nullptr);
    flags_t* flags = (flags_t*) flag;

    flags->output_file_name  = OUTPUT;
    flags->input_file_name   = INPUT;

    InitiallizeValidationFlags(flags);
}

static void InitiallizeValidationFlags(void* flag) {
    assert(flag != nullptr);
    flags_t* flags = (flags_t*) flag;

    flags->output_valid    = 0;
    flags->input_valid     = 0;
}
