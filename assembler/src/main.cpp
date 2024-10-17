#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "asm.h"
#include "logger.h"
#include "parse_arguments.h"
#include "asm_arg_parser.h"

int main(int argc, const char* argv[]) {
    flags_t flags = {};
    InitiallizeFlags(&flags);

    if (ArgParser(argc, argv, &flags, COMMANDS, COMMANDS_ARRAY_LENGTH, &ValidateInput) == INPUT_ERROR) {
        return EXIT_FAILURE;
    }

    FILE* logger_stream = fopen(flags.logger_output_file, "w");
    if (logger_stream == nullptr) {
        logger_stream = stderr;
        fprintf(stderr, "FAILED TO OPEN LOGGER OUTPUT FILE");
    }

    LoggerSetFile(logger_stream);
    LoggerSetLevel(DEBUG);

    FILE* istream = fopen(flags.input_file_name, "r");
    if (istream == nullptr) {
        LOG(ERROR, "FAILED TO OPEN INPUT FILE\n" STRERROR(errno));
        return EXIT_FAILURE;
    }

    FILE* ostream = fopen(flags.output_file_name, "wb");
    if (ostream == nullptr) {
        LOG(ERROR, "FAILED TO OPEN OUTPUT FILE\n" STRERROR(errno));
        ostream = stdout;
    }

    assemble(istream, ostream);

    if (fclose(istream) == EOF) {
        LOG(ERROR, "FAILED TO CLOSE INPUT FILE\n" STRERROR(errno));
        return EXIT_FAILURE;
    }
    if (fclose(ostream) == EOF) {
        LOG(ERROR, "FAILED TO CLOSE OUTPUT FILE\n" STRERROR(errno));
        return EXIT_FAILURE;
    }
    if (fclose(logger_stream) == EOF) {
        perror("FAILED TO CLOSE LOGGER OUTPUT FILE\n");
        return EXIT_FAILURE;
    }
}

