#ifndef CPU_H_
#define CPU_H_

#include "../Stack/Stack/includes/stack.h"

const int CP = 0xABCD;

struct Cpu_Info {
    int CP;
    int ip;
    int * Code;
};

#define DEF_CMD(name, number, arg, ...) \
    case number:                        \
    __VA_ARGS__                         \
    break;

/// @brief 
/// @param Cpu
/// @param src_file 
/// @return
int CPU_Ctor(Cpu_Info * const Cpu, FILE * src_file, Stack * const My_Stack);

/// @brief 
/// @param Cpu 
/// @return 
int CPU_Compile(Cpu_Info * const Cpu, Stack * const My_Stack);

/// @brief 
/// @param Cpu 
/// @param src_file 
void CPU_Dtor(Cpu_Info * const Cpu, FILE * src_file);

#endif