#ifndef CPU_H_
#define CPU_H_

#include "../Stack/Stack/includes/stack.h"

const int CP = 0xABCD;

//---------------------------------------------------------------------------------------------//

struct Cpu_Info {
    int CP;
    int size;
    int * registers;
    int * code;
    int error;
};

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param Cpu
/// @param src_file 
/// @return
int CPU_Ctor(Cpu_Info * const cpu, FILE * src_file, Stack * const My_Stack);

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param cpu 
/// @return 
int CPU_Compile(Cpu_Info * const cpu, Stack * const My_Stack);

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param cpu 
/// @param src_file 
void CPU_Dtor(Cpu_Info * const cpu, FILE * src_file);

//---------------------------------------------------------------------------------------------//

#endif