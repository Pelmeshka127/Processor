#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../asm_includes/assembler.h"
#include "../../Architecture/proc_config.h"
#include "../../Architecture/architecture.h"
#include "../Onegin/text_functions.h"

int main(int argc, char ** argv)
{
    Text_Info src_file = {.buffer = nullptr, .lines_count = 0, .symbols_count = 0, .pointers = nullptr};

    asm_file_info asmbly = {.cmd_num = 0, .code_arr = nullptr, .error = No_Error, .jumps_index = nullptr,
                            .label_array_size = DEF_LABEL_SIZE, .labels = nullptr};

    FILE * input_file = nullptr, * bin_file = nullptr;

    if (Check_Cmdline_Arg(argc) == Cmd_Line_Arg_Err)
        return Cmd_Line_Arg_Err;

    if ((input_file = fopen(argv[1], "r")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[1], strerror(errno));
        return Src_File_Err;
    }

    if ((bin_file = fopen(argv[2], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[2], strerror(errno));
        return Src_File_Err;
    }

    if (Asm_Ctor(input_file, &src_file, &asmbly) != No_Error)
    {
        fprintf(stderr, "Reading of input_file failed\n");
        return Reading_File_Err;
    }
    
    if (Fisrt_Asm_Compile(&src_file, &asmbly) != No_Error)
    {
        fprintf(stderr, "First assembly failed\n");
        Asm_Dtor(&src_file, &asmbly, input_file);
        return Asm_Compile_Error;
    } 

    if (Second_Asm_Compile(&src_file, &asmbly) != No_Error)
    {
        fprintf(stderr, "Second assembly failed\n");
        Asm_Dtor(&src_file, &asmbly, input_file);
        return Asm_Compile_Error;
    }

    if (fwrite(&DEF_CP, sizeof(int), 1, bin_file) < 1)
        return Writing_Error;
    if (fwrite(&asmbly.cmd_num, sizeof(int), 1, bin_file) < 1)
        return Writing_Error;
    if (fwrite(asmbly.code_arr, sizeof(char), (unsigned long) asmbly.cmd_num, bin_file) < (size_t) asmbly.cmd_num)
        return Writing_Error;

    Asm_Dtor(&src_file, &asmbly, input_file);

    return 0;
}