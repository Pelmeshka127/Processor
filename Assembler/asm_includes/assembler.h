#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "../Onegin/text_functions.h"

#define TEXT_FILE

#define BIN_FILE

struct asm_file_info {
    int cp;
    int cmd_num;
    int * code_arr;
    int * labels;
    int * jumps_index;
    int error;
};

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out labels and writes them to the label arr
/// @param src_file is ptr on Text_Info struct
/// @param asmbly is ptr on asm_file_info struct
void Fisrt_Asm_Compile(Text_Info * const src_file, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function writes down the codes of the comands to the code_array
/// @param src_file is ptr on Text_Info struct
/// @param asmbly is ptr on asm_file_info struct
void Second_Asm_Compile(Text_Info * const src_file, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out info about source file and allocates dynamic memory
/// @param fp is pointer on the source file
/// @param src_file is ptr on Text_Info struct
/// @return Reading file error if counstruction failed, No_Error if it's ok
void Asm_Ctor(FILE * input_file, Text_Info * const src_file, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function frees allocated memory and closes files
/// @param src_file is ptr on Text_Info struct
/// @param asmbly is ptr on asm_file_info struct
/// @param input_file is ptr on source file
void Asm_Dtor(Text_Info * const src_file, asm_file_info * const asmbly, FILE * input_file);

//---------------------------------------------------------------------------------------------//

/// @brief Function check the command lines arguments
/// @param argc is a count of arguments
/// @return Cmd_Line_Error if there isn't correct number of arguments, No_Error if it's ok
int Check_Cmdline_Arg(int argc);

//---------------------------------------------------------------------------------------------//

#endif