#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include <math.h>

#include "../cpu_includes/cpu.h"
#include "../Stack/includes/stack.h"

//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...) \
    case number:                        \
    __VA_ARGS__                         \
    break;
 
//---------------------------------------------------------------------------------------------//

int CPU_Ctor(cpu_info * const cpu, FILE * src_file)
{
    assert(cpu);
    assert(src_file);

    int CP = 0;
    fread(&CP, sizeof(int), 1, src_file);
    fread(&cpu->size, sizeof(int), 1, src_file);

    if (CP != DEF_CP)
        return CP_Error;

    cpu->code = (data_t *)calloc (cpu->size, sizeof(data_t));
    if (cpu->code == nullptr)
        return Alloc_Err;
    
    cpu->registers = (int *)calloc (DEF_REG_SIZE, sizeof(int));
    if (cpu->registers == nullptr)
        return Alloc_Err;

    cpu->RAM = (int *)calloc (DEF_RAM_SIZE, sizeof(int));
    if (cpu->RAM == nullptr)
        return Alloc_Err;

    fread(cpu->code, sizeof(data_t), cpu->size, src_file);

    if (Stack_Ctor(&cpu->stack) == No_Error)
    {
		fprintf(stderr, "FAILED THE INITIALIZATION OF STACK\n");
		return Alloc_Err;
	}

    if (Stack_Ctor(&cpu->ret_stack) == No_Error)
    {
        fprintf(stderr, "FIALED THE INITIALIZATION OF RET_STACK\n");
        return Alloc_Err;
    }

    return No_Error;
}

//---------------------------------------------------------------------------------------------//

int CPU_Compile(cpu_info * const cpu)
{   
    assert(cpu);

    int mode = true;
    
    while (mode)
    {
        switch (cpu->code[cpu->ip] & CMD_MASK)
        {
            #include "../../Architecture/cmd.h"

            default:
            {
                fprintf(stderr, "Error, The command %d wasn't found\n", cpu->code[cpu->ip]);
                return CPU_Compile_Error;
            }
        }
        cpu->ip++;
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

int Get_Push_Arg(cpu_info * const cpu, int cmd)
{
    assert(cpu);
    int arg = 0;
    cpu->ip++;

    if (cmd & ARG_IMMED)
        arg += cpu->code[cpu->ip];
    if (cmd & ARG_REG)
        arg += cpu->registers[cpu->code[cpu->ip]];
    if (cmd & ARG_RAM)
        arg = cpu->RAM[arg];
    return arg;
}

//---------------------------------------------------------------------------------------------//

void Get_Pop_Arg(cpu_info * const cpu, int cmd, int arg)
{
    assert(cpu);
    cpu->ip++;

    if (cmd & ARG_RAM)
    {
        if (cmd & ARG_IMMED)   
            cpu->RAM[cpu->code[cpu->ip]] = arg;
        else if (cmd & ARG_REG)
            cpu->RAM[cpu->registers[cpu->code[cpu->ip]]] = arg;
    }
    else if (cmd & ARG_REG)
        cpu->registers[cpu->code[cpu->ip]] = arg;
}

//---------------------------------------------------------------------------------------------//

void CPU_Dtor(cpu_info * const cpu, FILE * src_file)
{
    assert(cpu);
    assert(src_file);

    Stack_Dtor(&cpu->stack);
    Stack_Dtor(&cpu->ret_stack);

    free(cpu->code);
    free(cpu->registers);
    free(cpu->RAM);
    fclose(src_file);
}

//---------------------------------------------------------------------------------------------//