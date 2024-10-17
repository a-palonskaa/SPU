#ifndef DISASM_H
#define DISASM_H

typedef enum {
    CMD_PUSH  = 1,
    CMD_ADD   = 2,
    CMD_SUB   = 3,
    CMD_MUL   = 4,
    CMD_DIV   = 5,
    CMD_OUT   = 6,
    CMD_IN    = 7,
    CMD_SQRT  = 8,
    CMD_SIN   = 9,
    CMD_COS   = 10,
    CMD_DUMP  = 11,
    CMD_HLT   = 12,
    CMD_JA    = 13,
    CMD_JAE   = 14,
    CMD_JB    = 15,
    CMD_JBE   = 16,
    CMD_JNE   = 17,
    CMD_JMP   = 18,
    CMD_PUSHR = 19,
    CMD_POP   = 20,
} commands_name_t;

void disassemble(FILE* istream, FILE* ostream);
ssize_t find_file_size(FILE* istream);

#endif /* DISASM_H */
