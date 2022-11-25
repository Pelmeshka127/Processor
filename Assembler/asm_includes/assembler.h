#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <stdio.h>

#define TEXT_FILE

#define BIN_FILE

struct src_file_info {
    char * buffer;
    unsigned long symbols_count;
    unsigned long lines_count;
    char ** pointers;
    int error;
};

struct asm_file_info {
    int cp;
    int cmd_num;
    int * code_arr;
};

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out info about source file and allocates dynamic memory
/// @param fp is pointer on the source file
/// @param src_file is ptr on src_file_info struct
/// @return Reading file error if counstruction failed, No_Error if it's ok
void Asm_Ctor(FILE * fp, src_file_info * const src_file);

//---------------------------------------------------------------------------------------------//

/// @brief Function writes down the codes of the comands to the code_array
/// @param src_file is ptr on src_file_info struct
/// @param code_array is the array of codes of the commands
void Asm_Compile(src_file_info * const src_file, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function frees allocated memory and closes files
/// @param input_file is ptr on source file
/// @param output_file is ptr on result file
/// @param src_file is ptr on src_file_info struct
void Asm_Dtor(src_file_info * const src_file, asm_file_info * const asmbly, FILE * input_file);

//---------------------------------------------------------------------------------------------//

/// @brief Function check the command lines arguments
/// @param argc is a count of arguments
/// @return Cmd_Line_Error if there isn't correct number of arguments, No_Error if it's ok
int Check_Cmdline_Arg(int argc);

//---------------------------------------------------------------------------------------------//

#endif