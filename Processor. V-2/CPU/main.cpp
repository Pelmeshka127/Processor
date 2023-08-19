#include "cpu.hpp"
#include "../Architecture/arch.hpp"
#include "Stack/includes/stack.h"

int main()
{
    my_cpu cpu = {};

    FILE *bin = fopen("../Assembler/prog.bin", "rb");
    if (bin == nullptr)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return File_Error;
    }

    CpuCtor(&cpu, bin);

    if (fclose(bin) == EOF)
    {
        fprintf(stderr, "Failed closing bin file\n");
        return No_Error;
    }

    CpuExecute(&cpu);

    CpuDtor(&cpu);

    return 0;
}