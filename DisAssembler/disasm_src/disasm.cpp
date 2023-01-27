#include <assert.h>

#include "../disasm_includes/disasm.h"

static void Get_Arg(disasm_info * const disasm, FILE * dst_file, int * ip, int number);

//---------------------------------------------------------------------------------------------//

int Disasm_Ctor(disasm_info * const disasm, FILE * src_file)
{
    assert(disasm);

    int CP = 0;
    fread(&CP, 1, sizeof(int), src_file);
    if (CP != DEF_CP)
        return Src_File_Err;

    fread(&disasm->size, 1, sizeof(int), src_file);

    disasm->code = (data_t *)calloc (disasm->size, sizeof(data_t));
    if (disasm->code == nullptr)
        return Alloc_Err;

    fread(disasm->code, sizeof(data_t), disasm->size, src_file);

    return No_Error;
}

//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...)                             \
    case number:                                                    \
        if (arg == Digit){                                          \
            fprintf(dst_file, "%s ", #name);                        \
            Get_Arg(disasm, dst_file, &ip, number);}                \
        else if (arg == Label)                                      \
            fprintf(dst_file, "%s %d\n", #name, disasm->code[++ip]);\
        else if (arg == 0)                                          \
            fprintf(dst_file, "%s\n", #name);                       \
        break;

//---------------------------------------------------------------------------------------------//

int Disasm_Compile(disasm_info * const disasm, FILE * dst_file)
{
    assert(disasm);

    int ip = 0;
    while (ip < disasm->size)
    {
        switch (disasm->code[ip] & CMD_MASK)
        {
            #include "../../Architecture/cmd.h"

            default:
                fprintf(stderr, "Undefined arg in %d %d\n", ip, disasm->code[ip]);
        }   
        ip++;
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static void Get_Arg(disasm_info * const disasm, FILE * dst_file, int * ip, int number)
{
    assert(disasm);

    if (disasm->code[*ip] & ARG_RAM)
    {
        *ip+=1;
        fprintf(dst_file, "[");
        if (disasm->code[*ip-1] & ARG_IMMED && disasm->code[*ip-1] & ARG_REG)
        {
            fprintf(dst_file, "%d+%cx]\n", disasm->code[*ip], disasm->code[*ip+1] + 96);
            *ip+=1;
        }
        else if (disasm->code[*ip-1] & ARG_IMMED)
            fprintf(dst_file, "%d]\n", disasm->code[*ip]);
        else if (disasm->code[*ip-1] & ARG_REG)
            fprintf(dst_file, "%cx]\n", disasm->code[*ip] + 96);
    }

    else if (disasm->code[*ip] & ARG_IMMED)
    {
        fprintf(dst_file, "%d\n", disasm->code[*ip+1]);
        *ip+=1;
        if (disasm->code[*ip] & ARG_REG)
        {
            fprintf(dst_file, "+%cx\n", disasm->code[*ip+2] + 96);
            *ip+=1;
        }
    }

    else if (disasm->code[*ip] & ARG_REG)
    {
        fprintf(dst_file, "%cx\n", disasm->code[*ip+1] + 96);
        *ip+=1;
    }

    else
        fprintf(stderr, "Undefined arg %d\n", disasm->code[*ip]);
}

//---------------------------------------------------------------------------------------------//

int Disasm_Dtor(disasm_info * const disasm, FILE * dst_file)
{
    if (fclose(dst_file) == false)
        return Reading_File_Err;
    free(disasm->code);
}

//---------------------------------------------------------------------------------------------//