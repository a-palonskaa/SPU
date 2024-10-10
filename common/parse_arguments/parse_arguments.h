#ifndef PARSE_ARGUMENTS_H
#define PARSE_ARGUMENTS_H

#include <stdio.h>

typedef enum {
    NO_CMD_ERRORS = 0,
    INPUT_ERROR   = 1
} cmd_error_t;

typedef cmd_error_t (*changeflag_t) (void* flags, const char* argv);

typedef struct{
    const char* name;
    const char* long_name;
    changeflag_t change;
    const char* description;
    bool has_arg;
} option_t;

//----------------------------------------------------------------------------------------------

cmd_error_t ArgParser(int argc, const char* argv[], void* flags,
                      const option_t* commands, size_t commands_len,
                      cmd_error_t (*validate_input) (const void*));

#endif /* PARSE_ARGUMENTS_H */
