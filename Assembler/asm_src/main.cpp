#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../asm_includes/assembler.h"
#include "../../proc_config.h"
#include "../Onegin/text_functions.h"

int main(int argc, char ** argv)
{
    Text_Info src_file = {.buffer = nullptr, .lines_count = 0, .symbols_count = 0, .pointers = nullptr};

    asm_file_info asmbly = {DEF_CP, 0, nullptr, nullptr, nullptr, No_Error};

    FILE * input_file = nullptr, * text_file = nullptr, * bin_file = nullptr;

    if (Check_Cmdline_Arg(argc) == Cmd_Line_Arg_Err)
        return Cmd_Line_Arg_Err;

    if ((input_file = fopen(argv[1], "r")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[1], strerror(errno));
        return Src_File_Err;
    }

#ifdef TEXT_FILE

    if ((text_file = fopen(argv[2], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[2], strerror(errno));
        return Src_File_Err;
    }

#endif

#ifdef BIN_FILE

    if ((bin_file = fopen(argv[3], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[3], strerror(errno));
        return Src_File_Err;
    }

#endif

    Asm_Ctor(input_file, &src_file, &asmbly); // TODO use ret value
    
    if (asmbly.error != No_Error)
    {
        fprintf(stderr, "Reading of input_file failed\n");
        return Reading_File_Err;
    }

    Fisrt_Asm_Compile(&src_file, &asmbly);
    
    if (asmbly.error != No_Error)
    {
        fprintf(stderr, "Building of asm file failed\n");
        Asm_Dtor(&src_file, &asmbly, input_file);
        return Asm_Compile_Error;
    } 

    Second_Asm_Compile(&src_file, &asmbly);

    if (asmbly.error != No_Error)
    {
        fprintf(stderr, "Building of asm file failed\n");
        Asm_Dtor(&src_file, &asmbly, input_file);
        return Asm_Compile_Error;
    } 

#ifdef TEXT_FILE

    fprintf(text_file, "%d\n%d\n", asmbly.cp, asmbly.cmd_num); // TODO: listing

    for (int cmd = 0; cmd < asmbly.cmd_num; cmd++)
        fprintf(text_file, "%d ", asmbly.code_arr[cmd]);

    fclose(text_file);

#endif

#ifdef BIN_FILE

    fwrite(&asmbly.cp, sizeof(int), 1, bin_file); // TODO struct Header
    fwrite(&asmbly.cmd_num, sizeof(int), 1, bin_file);
    fwrite(asmbly.code_arr, sizeof(int), (unsigned long) asmbly.cmd_num, bin_file);

#endif

    Asm_Dtor(&src_file, &asmbly, input_file);

    return 0;
}