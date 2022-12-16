#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../asm_includes/assembler.h"
#include "../../proc_config.h"
#include "../../architecture.h"
#include "../Onegin/text_functions.h"

int main(int argc, char ** argv)
{
    Text_Info src_file = {.buffer = nullptr, .lines_count = 0, .symbols_count = 0, .pointers = nullptr};

    asm_file_info asmbly = {};

    FILE * input_file = nullptr, * listing_file = nullptr, * bin_file = nullptr;

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

#ifdef TEXT_FILE

    if ((listing_file = fopen(argv[3], "w")) == nullptr)
    {
        fprintf(stderr, "Incorrect file %s.\n%s\n", argv[3], strerror(errno));
        return Src_File_Err;
    }

#endif

    int d = 0;
    if ((d = Asm_Ctor(input_file, &src_file, &asmbly)) != No_Error) // TODO use ret value
    {
        fprintf(stderr, "Reading %dof input_file failed\n", d);
        return Reading_File_Err;
    }
    
    if (Fisrt_Asm_Compile(&src_file, &asmbly) != No_Error)
    {
        fprintf(stderr, "Building of asm file failed\n");
        Asm_Dtor(&src_file, &asmbly, input_file);
        return Asm_Compile_Error;
    } 

    printf("First is ok\n");

    /*for (int label = 0; label < asmbly.label_array_size; label++)
    {
        printf("|%s|; %d\n", asmbly.labels[label].label_line, asmbly.labels[label].label_num);
    }

    for (int i = 0; i < DEF_LABEL_SIZE; i++)
        printf("%d\n", asmbly.jumps_index[i]);*/

    if (Second_Asm_Compile(&src_file, &asmbly) != No_Error)
    {
        fprintf(stderr, "Building of asm file failed\n");
        Asm_Dtor(&src_file, &asmbly, input_file);
        return Asm_Compile_Error;
    }

    printf("Second is ok\n");

    fwrite(&DEF_CP, sizeof(int), 1, bin_file); // TODO struct Header
    fwrite(&asmbly.cmd_num, sizeof(int), 1, bin_file);
    fwrite(asmbly.code_arr, sizeof(int), (unsigned long) asmbly.cmd_num, bin_file);


#ifdef TEXT_FILE

    fprintf(listing_file, "%d\n%d\n", DEF_CP, asmbly.cmd_num); // TODO: listing

    for (int cmd = 0; cmd < asmbly.cmd_num; cmd++)
        fprintf(listing_file, "%d ", asmbly.code_arr[cmd]);

    fclose(listing_file);

#endif

    Asm_Dtor(&src_file, &asmbly, input_file);

    return 0;
}