#ifndef PROC_ARG_PARSER_H
#define PROC_ARG_PARSER_H

#include "parse_arguments.h"

typedef struct {
    const char* input_file_name;
    const char* output_file_name;
    const char* logger_output_file;

    int input_valid;
    int output_valid;
} flags_t;

void InitiallizeFlags(void* flag);

cmd_error_t ChangeFlagInputFile(void* flags, const char* arg);
cmd_error_t ChangeFlagOutputFile(void* flags, const char* arg);
cmd_error_t ChangeFlagLoggerOutput(void* flag, const char* arg);

cmd_error_t Help(void* flags, const char* arg);

cmd_error_t ValidateInput(const void* flag);

//====================================================================================================

const option_t COMMANDS[] = {
    // short_name  long_name         changeflag function         description                     has_arg
        {"-i",  "--input_file",    &ChangeFlagInputFile,      "Read data from the file"          , true},
        {"-o",  "--output_file",   &ChangeFlagOutputFile,     "Print results in the file "       , true},
        {"-h",  "--help",          &Help,                     "Help"                             , false},
        {"-l",  "--logger_output", &ChangeFlagLoggerOutput,   "Print logger messages in the file", true},

};

const size_t COMMANDS_ARRAY_LENGTH = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

#endif /* PROC_ARG_PARSER_H */
