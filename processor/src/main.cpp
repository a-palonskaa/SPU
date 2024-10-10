#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "processor.h"
#include "proc_arg_parser.h"
#include "parse_arguments.h"
#include "logger.h"

int main(int argc, const char* argv[]) {
    flags_t flags = {};
    InitiallizeFlagsHeaderSort(&flags);

    if (ArgParser(argc, argv, &flags, COMMANDS, COMMANDS_ARRAY_LENGTH, &ValidateInput) == INPUT_ERROR) {
        return 1;
    }

    FILE* istream = fopen(flags.input_file_name, "r");
    FILE* ostream = fopen(flags.output_file_name, "w");
    LoggerSetFile(stdout);
    LoggerSetLevel(DEBUG);

    processor_t processor = {};
    get_code(istream, &processor);
    run(&processor);

    free(processor.code);
    processor.code = nullptr;

    fclose(istream);
    fclose(ostream);
}
