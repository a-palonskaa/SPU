#ifndef DISASM_H
#define DISASM_H

void disassemble(FILE* istream, FILE* ostream);
bool parse_cmd_args(unsigned char* code, size_t* i, FILE* ostream);
void print_main_part(unsigned char* code, size_t* i, FILE* ostream);

#endif /* DISASM_H */
