#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

#include "../cpu_includes/cpu.h"
#include "../cpu_includes/cpu_config.h"
#include "../Stack/Stack/includes/stack.h"
 
int CPU_Ctor(Cpu_Info * const Cpu, FILE * src_file, Stack * const My_Stack)
{
    assert(Cpu);
    assert(src_file);

    //fscanf(src_file, "%d", &Cpu->CP); 

   /*if (Cpu->CP != DEF_CP)
    {   
        fprintf(stderr, "Incorrect value of CP:%d, expected%d\n", Cpu->CP, DEF_CP);
        return CP_Error;
    }*/

    //fscanf(src_file, "%d", &Cpu->ip);

    int * Alloc_Mem = (int *)calloc (13, sizeof(int));
    if (Alloc_Mem == nullptr)
        return Alloc_Err;
    
    Cpu->Code = Alloc_Mem;

    /*for (int cmd = 0; cmd < 13; cmd++)
        fscanf(src_file, "%d", &Cpu->Code[cmd]);*/

    fread(Cpu->Code, sizeof(int), 13, src_file);

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
    while (ip < 13)
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
                printf("%d =  %d / %d", c, b, a);
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