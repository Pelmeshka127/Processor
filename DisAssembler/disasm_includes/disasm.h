#ifndef DISASM_H_
#define DISASM_H_

#include <stdio.h>

#include "../../Architecture/proc_config.h"
#include "../../Architecture/architecture.h"

struct disasm_info {
    size_t size;
    data_t * code;
};

/// @brief 
/// @param disasm 
/// @param src_file 
/// @return 
int Disasm_Ctor(disasm_info * const disasm, FILE * src_file);

int Disasm_Compile(disasm_info * const disasm, FILE * dst_file);

#endif