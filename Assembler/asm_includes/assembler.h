#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <stdio.h>

#define TEXT_FILE

#define BIN_FILE

struct Asm_File_Info {
    char * buffer;
    unsigned long symbols_count;
    unsigned long lines_count;
    char ** pointers;
    int cmd_num;
    int cmd_args;
    int error;
};

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out info about source file and allocates dynamic memory
/// @param fp is pointer on the source file
/// @param Asm_File is ptr on Asm_File_Info struct
/// @return Reading file error if counstruction failed, No_Error if it's ok
int Asm_Ctor(FILE * fp, Asm_File_Info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function writes down the codes of the comands to the code_array
/// @param Asm_File is ptr on Asm_File_Info struct
/// @param code_array is the array of codes of the commands
void Asm_Compile(Asm_File_Info * const asmbly, int * const code_array);

//---------------------------------------------------------------------------------------------//

/// @brief Function makes file with codes of the commands
/// @param interpret_file is ptr on result file
/// @param Asm_File is ptr on Asm_File_Info struct
/// @param code_array is the array of codes of the commands
void Asm_Print_File(FILE * text_file, FILE * bin_file, Asm_File_Info * const asmbly, int * const code_array);

//---------------------------------------------------------------------------------------------//

/// @brief Function frees allocated memory and closes files
/// @param input_file is ptr on source file
/// @param output_file is ptr on result file
/// @param Asm_File is ptr on Asm_File_Info struct
void Asm_Dtor(Asm_File_Info * const asmbly, int * const code_array);

//---------------------------------------------------------------------------------------------//

/// @brief Function check the command lines arguments
/// @param argc is a count of arguments
/// @return Cmd_Line_Error if there isn't correct number of arguments, No_Error if it's ok
int Check_Cmdline_Arg(int argc);

//---------------------------------------------------------------------------------------------//

#endif