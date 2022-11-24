#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../asm_includes/assembler.h"
#include "../asm_includes/proc_config.h"

int main(int argc, char ** argv)
{
    Asm_File_Info asmbly = {};

    FILE * input_file = nullptr, * text_file, * bin_file = nullptr;

    if (Check_Cmdline_Arg(argc) == Cmd_Line_Arg_Err)
        return Cmd_Line_Arg_Err;

    if ((input_file = fopen(argv[1], "r")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[1], strerror(errno));
        return Input_File_Err;
    }

    if ((text_file = fopen(argv[2], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[2], strerror(errno));
        return Interpret_File_Err;
    }

    if ((bin_file = fopen(argv[3], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[3], strerror(errno));
        return Interpret_File_Err;
    }

    if (Asm_Ctor(input_file, &asmbly) == Reading_File_Err)
    {
        fprintf(stderr, "Reading of input_file failed\n");
        return Reading_File_Err;
    }

    int * code_array = (int *)calloc (2 * asmbly.lines_count, sizeof(int));
    
    if (code_array == nullptr)
        return Alloc_Err;

    Asm_Compile(&asmbly, code_array);

    Asm_Print_File(text_file, bin_file, &asmbly, code_array); // rename function

    Asm_Dtor(&asmbly, code_array);

    fclose(input_file);
    fclose(text_file);
    fclose(bin_file);

    return 0;
}