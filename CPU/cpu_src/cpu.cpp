#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

#include "../cpu_includes/cpu.h"
#include "../../proc_config.h"
#include "../Stack/Stack/includes/stack.h"
 
int CPU_Ctor(Cpu_Info * const Cpu, FILE * src_file, Stack * const My_Stack)
{
    assert(Cpu);
    assert(src_file);

    fread(&Cpu->CP, sizeof(int), 1, src_file);
    fread(&Cpu->ip, sizeof(int), 1, src_file);

    int * Alloc_Mem = (int *)calloc (Cpu->ip, sizeof(int));
    if (Alloc_Mem == nullptr)
        return Alloc_Err;
    
    Cpu->Code = Alloc_Mem;

    fread(Cpu->Code, sizeof(int), Cpu->ip, src_file);

    if (Stack_Ctor(My_Stack) == No_Error)
    {
		fprintf(stderr, "FAILED THE INITIALIZATION OF STACK\n");
		return Alloc_Err;
	}

    return No_Error;
}

int CPU_Compile(Cpu_Info * const Cpu, Stack * My_Stack)
{   
    assert(Cpu);
    assert(My_Stack);

    int ip = 0;
    while (ip < Cpu->ip)
    {
        switch (Cpu->Code[ip])
        {
            case CMD_PUSH:
            {
                Stack_Push(My_Stack, Cpu->Code[ip + 1]);
                ip++;
                break;
            }

            case CMD_POP:
            {
                Stack_Pop(My_Stack);
                break;
            }

            case CMD_ADD:
            {
                Stack_Push(My_Stack, Stack_Pop(My_Stack) + Stack_Pop(My_Stack));
                break;
            }

            case CMD_SUB:
            {
                Stack_Push(My_Stack, Stack_Pop(My_Stack) - Stack_Pop(My_Stack));
                break;
            }

            case CMD_MUL:
            {
                Stack_Push(My_Stack, Stack_Pop(My_Stack) * Stack_Pop(My_Stack));
                break;
            }

            case CMD_DIV:
            {
                int a = Stack_Pop(My_Stack);
                int b = Stack_Pop(My_Stack);
                int c = b / a;
                Stack_Push(My_Stack, c);
                break;
            }

            case CMD_OUT:
            {
                int value = Stack_Pop(My_Stack);
                printf("The result is %d\n", value);
                break;
            }

            case CMD_HLT:
            {
                printf("The end is near... It's there\n");
                break;
            }

            default:
            {
                fprintf(stderr, "The command wasn't found\n");
            }
        }
        ip++;
    }  
    return No_Error;
}


void CPU_Dtor(Cpu_Info * const Cpu, FILE * src_file)
{
    assert(Cpu);
    assert(src_file);

    free(Cpu->Code);

    fclose(src_file);
}