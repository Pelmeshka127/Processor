#ifndef DISASM_H_
#define DISASM_H_

#include <stdio.h>

struct disasm_info {
    size_t size;
    int * code;
    int error;
};

int Disasm_Ctor(disasm_info * const disasm, FILE * src_file);

#endif