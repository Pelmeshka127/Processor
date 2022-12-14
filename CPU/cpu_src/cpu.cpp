#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

#include "../cpu_includes/cpu.h"
#include "../../proc_config.h"
#include "../../architecture.h"
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

    fread(&cpu->CP, sizeof(int), 1, src_file);
    fread(&cpu->size, sizeof(int), 1, src_file);

    if (cpu->CP != DEF_CP)
        return CP_Error;

    cpu->code = (int *)calloc (cpu->size, sizeof(int));
    if (cpu->code == nullptr)
        return Alloc_Err;
    
    cpu->registers = (int *)calloc (DEF_REG_SIZE, sizeof(int));
    if (cpu->registers == nullptr)
        return Alloc_Err;

    fread(cpu->code, sizeof(int), cpu->size, src_file);

    if (Stack_Ctor(&cpu->stack) == No_Error)
    {
		fprintf(stderr, "FAILED THE INITIALIZATION OF STACK\n");
		return Alloc_Err;
	}

    if (Stack_Ctor(&cpu->ret_stack) == No_Error)
    {
        fprintf(stderr, "FIALED THE INITIALIZATION OF STACK\n");
        return Alloc_Err;
    }

    return No_Error;
}

//---------------------------------------------------------------------------------------------//

int CPU_Compile(cpu_info * const cpu)
{   
    assert(cpu);

    int mode = true;
    int ip = 0;
    
    while (mode)
    {
        switch (cpu->code[ip])
        {
            #include "../../cmd.h"

            default:
            {
                fprintf(stderr, "Error, The command %d wasn't found\n", cpu->code[ip]);
                return CPU_Compile_Error;
            }
        }
        ip++;
    }
    return No_Error;
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
    fclose(src_file);

}

//---------------------------------------------------------------------------------------------//