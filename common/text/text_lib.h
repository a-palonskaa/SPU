#ifndef TEXT_LIB_H
#define TEXT_LIB_H

#include <stdio.h>

typedef struct {
    unsigned char* begin;
    size_t length;
} string_t;

typedef struct {
    size_t symbols_amount;
    size_t strings_amount;

    unsigned char* symbols;
    string_t* strings;
} text_t;

typedef enum {
    NO_ERRORS                     = 0,
    FILE_READ_ERROR               = 1,
    MEMORY_ALLOCATE_ERROR         = 2,
    INFILE_PTR_MOVING_ERROR       = 3,
    EMPTY_FILE_ERROR              = 4,
    PTR_POSITION_INDICATION_ERROR = 5,
} error_t;

error_t text_ctor(text_t* text, FILE* istream);
void text_dtor(text_t* text);

ssize_t find_file_size(FILE* istream);
size_t count_text_lines(text_t* text);

void get_text_symbols(text_t* text, FILE* istream);
void parse_text(text_t* text);

#endif /* TEXT_LIB_H */

