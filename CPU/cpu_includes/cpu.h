#ifndef CPU_H_
#define CPU_H_

#include "../Stack/includes/stack.h"

//---------------------------------------------------------------------------------------------//

/// @brief This struct includes the information about our CPU
struct cpu_info {
    int CP;             // The value of CP for checking correct file
    size_t size;        // The size of commands in file
    int * registers;    // The arrray of registers
    int * code;         // The array of code which includes commands
    Stack stack;        // The struct of Stack
    Stack ret_stack;         
};

//---------------------------------------------------------------------------------------------//

/// @brief Function constructs cpu_info struct with info from source file and Stack library
/// @param cpu is pts on cpu_info struct
/// @param src_file is ptr on source file
/// @return Alloc_Err if construction failed, No_Error if it's ok
int CPU_Ctor(cpu_info * const cpu, FILE * src_file);

//---------------------------------------------------------------------------------------------//

/// @brief Function compiles the array of commands
/// @param cpu is pts on cpu_info struct
/// @return CPU_Compile_Error if compiling failed, No_Error if it's ok
int CPU_Compile(cpu_info * const cpu);

//---------------------------------------------------------------------------------------------//

/// @brief Function closes files and cleans allocated memory
/// @param cpu is ptr on cpu_info struct
/// @param src_file is ptr on source file
void CPU_Dtor(cpu_info * const cpu, FILE * src_file);

//---------------------------------------------------------------------------------------------//

#endif