#include "cpu.hpp"

//===================================================================================//

int CpuCtor(my_cpu *const cpu, FILE *bin)
{
    if (fread(&cpu->size, sizeof(int), 1, bin) < 1)
    {
        fprintf(stderr, "Failed reading size of array in %s\n", __func__);
        return File_Error;
    }

    cpu->array = (code_t *)calloc (cpu->size, sizeof(code_t));
    if(cpu->array == nullptr)
    {
        fprintf(stderr, "Failed allocation memory for cpu array in %s\n", __func__);
        return Bad_Alloc;
    }

    if (fread(cpu->array, sizeof(code_t), cpu->size, bin) < cpu->size)
    {
        fprintf(stderr, "Failed reading array in %s\n", __func__);
        return File_Error;
    }

    cpu->registers = (int *)calloc (Def_Register_Size, sizeof(int));
    if (cpu->registers == nullptr)
    {
        fprintf(stderr, "Failed allocation memory for registers array in %s\n", __func__);
        return Bad_Alloc;
    }

    cpu->RAM = (int *)calloc (Def_RAM_Size, sizeof(int));
    if (cpu->RAM == nullptr)
    {
        fprintf(stderr, "Failed allocation memory for RAM array in %s\n", __func__);
        return Bad_Alloc; 
    }

    cpu->stk = {};
    cpu->ret_stk = {};

    Stack_Ctor(&cpu->stk);
    Stack_Ctor(&cpu->ret_stk);

    return No_Error;
}

//===================================================================================//

int CpuExecute(my_cpu *const cpu)
{
    bool mode = true;

    while (mode)
    {
        switch(cpu->array[cpu->ip] & CMD_MASK)
        {

#   define  DEF_CMD(word_code, number, length, arg_type, code)      \
        case number:                                                \
        code                                                        \
        break;

#   include "../Architecture/cmd.hpp"

#   undef   DEF_CMD

        default:
        {
            fprintf(stderr, "Error, the command %d wasn't found\n", cpu->array[cpu->ip]);
            return Incorrect_Command;
        }

        }
        
        cpu->ip++;
    }
    return No_Error;
}

//===================================================================================//

int GetPushArg(my_cpu *const cpu, int command)
{
    cpu->ip++;

    int arg = 0;

    if (command & ARG_IMMED)
    {
        int immed_arg = 0;
        memcpy(&immed_arg, &cpu->array[cpu->ip], sizeof(int));
        
        arg     += immed_arg;
        cpu->ip += sizeof(int);
    }

    if (command & ARG_REG)
    {
        int reg_arg = 0;
        memcpy(&reg_arg, &cpu->array[cpu->ip], sizeof(int));
        
        if (reg_arg >= Def_Register_Size)
        {
            fprintf(stderr, "Value is out of registers array %d\n", reg_arg);
            return Incorrect_Arg;
        }

        arg     += cpu->registers[reg_arg];
        cpu->ip += sizeof(int);
    }

    if (command & ARG_RAM)
    {
        if (arg >= Def_RAM_Size)
        {
            fprintf(stderr, "Value is out of RAM array %d\n", arg);
            return Incorrect_Arg;
        }

        arg = cpu->RAM[arg];
    }

    cpu->ip--;

    return arg;
}

//===================================================================================//

int *GetPopArg(my_cpu *const cpu, int command)
{
    cpu->ip++;

    if (command & ARG_RAM)
    {
        int ram_index = 0;

        if (command & ARG_IMMED)
        {
            int immed_arg = 0;
            memcpy(&immed_arg, &cpu->array[cpu->ip], sizeof(int));

            ram_index += immed_arg;
            cpu->ip   += sizeof(int);
        }

        if (command & ARG_REG)
        {
            int reg_arg = 0;
            memcpy(&reg_arg, &cpu->array[cpu->ip], sizeof(int));

            if (reg_arg >= Def_Register_Size)
            {
                fprintf(stderr, "Value is out of registers array %d\n", reg_arg);
                return nullptr;
            }

            ram_index += cpu->registers[reg_arg];
            cpu->ip   += sizeof(int);
        }

        if (ram_index >= Def_RAM_Size)
        {
            fprintf(stderr, "Out of RAM in %d\n", ram_index);
            return nullptr;
        }

        cpu->ip--;

        return &cpu->RAM[ram_index];
    }

    if (command & ARG_REG)
    {
        int reg_arg = 0;
        memcpy(&reg_arg, &cpu->array[cpu->ip], sizeof(int));

        if (reg_arg >= Def_Register_Size)
        {
            fprintf(stderr, "Value is out of registers array %d\n", reg_arg);
            return nullptr;
        }     

        cpu->ip += sizeof(int) - 1;

        return &cpu->registers[reg_arg];
    }
    return 0;
}

//===================================================================================//

int JumpArg(my_cpu *const cpu)
{
    int jump_arg = 0;

    memcpy(&jump_arg, &cpu->array[cpu->ip], sizeof(int));

    return jump_arg - 1;
}

//===================================================================================//

int CpuDtor(my_cpu *const cpu)
{
    Stack_Dtor(&cpu->stk);
    Stack_Dtor(&cpu->ret_stk);

    free(cpu->array);
    free(cpu->registers);
    free(cpu->RAM);

    return No_Error;
}

//===================================================================================//