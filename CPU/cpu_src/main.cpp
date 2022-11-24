#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../cpu_includes/cpu.h"
#include "../Stack/Stack/includes/stack.h"
#include "../cpu_includes/cpu_config.h"

int main(void)
{
    Cpu_Info Cpu = {};

    FILE * text_file = nullptr, * bin_file = nullptr;

    Stack My_Stack = {};

    if ((text_file = fopen("../Assembler/prog.txt", "rb")) == nullptr)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return Src_File_Err;
    }

    if ((bin_file = fopen("../Assembler/prog.bin", "rb")) == nullptr)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return Src_File_Err;
    }

    if (CPU_Ctor(&Cpu, bin_file, &My_Stack) == CP_Error)
        return CP_Error;

    CPU_Compile(&Cpu, &My_Stack);

    CPU_Dtor(&Cpu, bin_file);

    Stack_Dtor(&My_Stack);

    return 0;
}