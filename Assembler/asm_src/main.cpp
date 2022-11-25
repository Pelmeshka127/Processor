#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../asm_includes/assembler.h"
#include "../../proc_config.h"

int main(int argc, char ** argv)
{
    src_file_info src_file = {nullptr, 0, 0, nullptr, No_Error};

    asm_file_info asmbly = {};

    FILE * input_file = nullptr, * text_file, * bin_file = nullptr;

    if (Check_Cmdline_Arg(argc) == Cmd_Line_Arg_Err)
        return Cmd_Line_Arg_Err;

    if ((input_file = fopen(argv[1], "r")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[1], strerror(errno));
        return Input_File_Err;
    }

#ifdef TEXT_FILE

    if ((text_file = fopen(argv[2], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[2], strerror(errno));
        return Interpret_File_Err;
    }

#endif

#ifdef BIN_FILE

    if ((bin_file = fopen(argv[3], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[3], strerror(errno));
        return Interpret_File_Err;
    }

#endif

    Asm_Ctor(input_file, &src_file);
    
    if (src_file.error != No_Error)
    {
        fprintf(stderr, "Reading of input_file failed\n");
        return Reading_File_Err;
    }

    asmbly.code_arr = (int *)calloc (2 * src_file.lines_count, sizeof(int));
    
    if (asmbly.code_arr == nullptr)
        return Alloc_Err;

    Asm_Compile(&src_file, &asmbly);

    if (src_file.error != No_Error)
    {
        fprintf(stderr, "Building of asm file failed\n");
        return Asm_Compile_Error;
    }

    asmbly.cp = DEF_CP;

#ifdef TEXT_FILE

    fprintf(text_file, "%d\n%d\n", asmbly.cp, asmbly.cmd_num);

    for (int cmd = 0; cmd < asmbly.cmd_num; cmd++)
        fprintf(text_file, "%d ", asmbly.code_arr[cmd]);

    fclose(text_file);

#endif

#ifdef BIN_FILE

    fwrite(&asmbly.cp, sizeof(int), 1, bin_file);
    fwrite(&asmbly.cmd_num, sizeof(int), 1, bin_file);
    fwrite(asmbly.code_arr, sizeof(int), asmbly.cmd_num, bin_file);

#endif

    Asm_Dtor(&src_file, &asmbly, input_file);

    return 0;
}