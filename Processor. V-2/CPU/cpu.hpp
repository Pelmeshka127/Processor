#ifndef CPU_HPP_
#define CPU_HPP_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "../Architecture/arch.hpp"
#include "Stack/includes/stack.h"

struct my_cpu {
    int     ip;
    int     size;
    int     *registers;
    int     *RAM;
    code_t  *array;
    Stack   stk;
    Stack   ret_stk;
};

//===================================================================================//

int CpuCtor(my_cpu *const cpu, FILE *bin);

int CpuExecute(my_cpu *const cpu);

int GetPushArg(my_cpu *const cpu, int command);

int *GetPopArg(my_cpu *const cpu, int command);

int JumpArg(my_cpu *const cpu);

int CpuDtor(my_cpu *const cpu);

//===================================================================================//

#endif