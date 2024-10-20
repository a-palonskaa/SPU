#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "logger.h"
#include "text_lib.h"

static error_t strings_ctor(text_t* text);
//----------------------------------------------------------------------------------------------

ssize_t find_file_size(FILE* istream) {
    assert(istream != nullptr);

    struct stat file_data = {};

    if (fstat(fileno(istream), &file_data) == -1) {
        LOG(ERROR, "Failed to read a file\n" STRERROR(errno));
        return -1;
    }

    if (file_data.st_size == 0) {
        LOG(WARNING, "Empty file\n" STRERROR(errno));
    }

    LOG(INFO, "File size: %zu\n", file_data.st_size);
    return (ssize_t) file_data.st_size;
}

size_t count_text_lines(text_t* text) {
    assert(text != nullptr);

    size_t last_newline_index = 0;
    size_t lines_cnt = 0;

    for (size_t i = 0; i < text->symbols_amount; i++) {
        if (text->symbols[i] == '\n') {
            lines_cnt++;
            last_newline_index = i;
        }
    }

    if (text->symbols_amount - last_newline_index > 1) {
        lines_cnt++;
    }

    LOG(INFO, "Lines amount: %zu\n", lines_cnt);
    text->strings_amount = lines_cnt;

    return lines_cnt;
}

//----------------------------------------------------------------------------------------------

void parse_text(text_t* text) {
    assert(text != nullptr);

    const size_t str_cnt = text->strings_amount;
    const size_t smb_cnt = text->symbols_amount;
    string_t* string = text->strings;

    if (str_cnt == 0) {
        return;
    }

    string[0].begin = text->symbols;

    for (size_t i = 0, j = 1; i < smb_cnt && j < str_cnt; i++) {
        if (text->symbols[i] == '\n') {
            string[j].begin = &text->symbols[i+1];
            string[j - 1].length = (size_t) (string[j].begin - string[j - 1].begin);
            text->symbols[i] = '\0';
            j++;
        }
    }

    string[str_cnt - 1].length =
        (size_t) (&text->symbols[smb_cnt - 1] - string[str_cnt - 1].begin);

    LOG(INFO, "Text was successfully parsed\n");
}

//----------------------------------------------------------------------------------------------

error_t text_ctor(text_t* text, FILE* istream) {
    assert(istream != nullptr);
    assert(text != nullptr);

    ssize_t symbols_amount = find_file_size(istream);

    if (symbols_amount == -1) {
        return FILE_READ_ERROR;
    }

    symbols_amount++;

    text->symbols_amount = (size_t) symbols_amount;

    text->symbols = (unsigned char*) calloc(text->symbols_amount, sizeof(char));
    if (text->symbols == nullptr) {
        LOG(ERROR, "FAILED TO ALLOCATE THE MEMORY\n" STRERROR(errno));
        return MEMORY_ALLOCATE_ERROR;
    }

    ssize_t position_in_file = ftell(istream);
    if (position_in_file == -1) {
        return PTR_POSITION_INDICATION_ERROR;
    }

    if (fseek(istream, 0, SEEK_SET)) {
        LOG(ERROR, "FAILED TO MOVE POINTER IN THE FILE\n" STRERROR(errno));
        text_dtor(text);
        return INFILE_PTR_MOVING_ERROR;
    }

    if ((fread(text->symbols, sizeof(char), text->symbols_amount, istream) != text->symbols_amount) &&
         !feof(istream) &&
         ferror(istream)) {
        LOG(ERROR, "FILE READ ERROR\n" STRERROR(errno));
        text_dtor(text);
        return FILE_READ_ERROR;
    }

    if (fseek(istream, position_in_file, SEEK_SET)) {
        LOG(ERROR, "FAILED TO MOVE POINTER IN THE FILE\n" STRERROR(errno));
        text_dtor(text);
        return INFILE_PTR_MOVING_ERROR;
    }

    count_text_lines(text);

    error_t str_ctor_error = strings_ctor(text);
    if (str_ctor_error != NO_ERRORS) {
        return str_ctor_error;
    }

    parse_text(text);
    LOG(INFO, "Text structure was successfully created and initialized\n");
    return NO_ERRORS;
}

void text_dtor(text_t* text) {
    assert(text != nullptr);

    free(text->strings);
    text->strings = nullptr;

    free(text->symbols);
    text->symbols = nullptr;

    text->strings_amount = 0;
    text->symbols_amount = 0;
    LOG(INFO, "Text stracture was sucessfully destructed\n");
}

//----------------------------------------------------------------------------------------------

void get_text_symbols(text_t* text, FILE* istream) {
    assert(text != nullptr);
    assert(istream != nullptr);

    int c = 0;

    for (size_t i = 0; i < text->symbols_amount && ((c = fgetc(istream)) != EOF); i++) {
        text->symbols[i] = (unsigned char) c;
    }

    text->symbols[text->symbols_amount - 1] = '\0';
    LOG(INFO, "TEXT WAS SUCCESSFULLY READ\n");
}

static error_t strings_ctor(text_t* text) {
    assert(text != nullptr);

    text->strings = (string_t*) calloc(text->strings_amount, sizeof(string_t));

    if (text->strings != nullptr) {
        return NO_ERRORS;
    }

    text_dtor(text);
    LOG(ERROR, "FAILED TO ALLOCATE THE MEMORY\n" STRERROR(errno));
    return MEMORY_ALLOCATE_ERROR;
}
