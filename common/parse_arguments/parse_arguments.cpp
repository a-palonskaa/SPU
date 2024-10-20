#include <assert.h>
#include <string.h>

#include "parse_arguments.h"

cmd_error_t ArgParser(int argc, const char* argv[], void* flags,
                      const option_t* commands, size_t commands_len,
                      cmd_error_t (*validate_input)(const void*)) {
    assert(argv  != nullptr);
    assert(flags != nullptr);
    assert(argc >= 0);

    const size_t COMMANDS_ARRAY_LENGTH = commands_len;
    if (argc == 0) {
        return INPUT_ERROR;
    }

    while (--argc > 0) {
        argv++;
        int has_arg_validate = 0;

        for (size_t i = 0; i < COMMANDS_ARRAY_LENGTH; i++) {
            if(!(strcmp(*argv, commands[i].name) &&
                 strcmp(*argv, commands[i].long_name))) {

                has_arg_validate++;

                if (commands[i].has_arg && argc <= 1) {
                    return INPUT_ERROR;
                }

                cmd_error_t state = NO_CMD_ERRORS;

                if (!commands[i].has_arg) {
                    state = commands[i].change(flags, nullptr);
                }
                else {
                    argc--;
                    argv++;
                    if (strstr(*argv, "-") != nullptr && strstr(*argv, "\"-") == nullptr) {
                        printf("ERROR OF COMMAND\n");
                        return INPUT_ERROR;
                    }
                    state = commands[i].change(flags, *argv);
                }

                if (state == INPUT_ERROR) {
                    printf("ERROR OF COMMAND\n");
                    return INPUT_ERROR;
                }
            }
        }
        if (!has_arg_validate) {
            printf("UNKNOWN COMMAND\n");
            return INPUT_ERROR;
        }
    }
    return validate_input(flags);
}
