#ifndef COMMANDS_H
#define COMMANDS_H

typedef unsigned char command;
typedef double imm;
typedef unsigned char reg;
typedef size_t addr;

// TODO:
/*
    TIER SHIT FEATURES LIST:
    0. *.asm -> ?????                                          // +
    1. Reduces cache misses (bad_apple.txt), transform matrix  // +
    2. Add p-frame (delta frame) + rle (own?)
    3. Replace strstr with sscanf
    4. Codegen for enum v
    5. Add SDL                                                 // +
    6. Add fibonacci (recursion), factorial (cycles)
    7. Add custom function memset [from what] [with what] [amount]
    8. DEF_CMD, DEF_JUMP                                       // +
    9. Add square_equation                                     // +
    10. Add custom function mov                                // +

*/
// ORN -> byte mask (00000001
//                   00000010
//                   00000100
//                   00001000)


typedef enum {
    CMD_PUSH   = 1,
    CMD_ADD    = 2,
    CMD_SUB    = 3,
    CMD_MUL    = 4,
    CMD_DIV    = 5,
    CMD_OUT    = 6,
    CMD_IN     = 7,
    CMD_SQRT   = 8,
    CMD_SIN    = 9,
    CMD_COS    = 10,
    CMD_DUMP   = 11,
    CMD_HLT    = 12,
    CMD_JA     = 13,
    CMD_JAE    = 14,
    CMD_JB     = 15,
    CMD_JBE    = 16,
    CMD_JE     = 17,
    CMD_JNE    = 18,
    CMD_JMP    = 19,
    CMD_POP    = 20,
    CMD_CALL   = 21,
    CMD_RET    = 22,
    CMD_SHOW   = 23,
    CMD_SQR    = 24,
    CMD_POW    = 25,
    CMD_ABS    = 26,
    CMD_LOG    = 27,
    CMD_MOV    = 28,
    CMD_MEMSET = 29,
} commands_name_t;

typedef struct {
    commands_name_t code;
    const char* alias;
    size_t args_amount;
} commands_t;

const commands_t commands[] = {{CMD_PUSH,      "push", 1},
                               {CMD_ADD,       "add",  0},
                               {CMD_SUB,       "sub",  0},
                               {CMD_MUL,       "mul",  0},
                               {CMD_DIV,       "div",  0},
                               {CMD_OUT,       "out",  0},
                               {CMD_IN,        "in",   0},
                               {CMD_SQRT,      "sqrt", 0},
                               {CMD_SIN,       "sin",  0},
                               {CMD_COS,       "cos",  0},
                               {CMD_DUMP,      "dump", 0},
                               {CMD_HLT,       "hlt",  0},
                               {CMD_JA,        "ja",   1},
                               {CMD_JAE,       "jae",  1},
                               {CMD_JB,        "jb",   1},
                               {CMD_JBE,       "jbe",  1},
                               {CMD_JE,        "je",   1},
                               {CMD_JNE,       "jne",  1},
                               {CMD_JMP,       "jmp",  1},
                               {CMD_POP,       "pop",  1},
                               {CMD_CALL,      "call", 1},
                               {CMD_RET,       "ret",  0},
                               {CMD_SHOW,      "show", 0},
                               {CMD_SQR,       "sqr",  0},
                               {CMD_POW,       "pow",  0},
                               {CMD_ABS,       "abs",  0},
                               {CMD_LOG,       "log",  0},
                               {CMD_MOV,       "mov",  2},
                               {CMD_MEMSET, "memset",  3}};

const size_t commands_size = sizeof(commands) / sizeof(commands[0]);

#endif /* COMMANDS_H */
