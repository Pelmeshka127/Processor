#ifndef ASM_HPP_
#define ASM_HPP_

#include <stdlib.h>
#include <string.h>

#include "../Architecture/arch.hpp"
#include "token.hpp"
#include "Parsering/parser.h"

const int Cmd_Line_Arg_Count = 3;

struct label_s {
    int     label_num;
    char    *label_name;
};

struct my_asm {
    size_t  label_size;
    int     file_size;
    int     label_count;
    int     *jump_indexes;
    label_s *labels;
    code_t  *array;
};

int CkeckCmdLineArgs(const int argc);

int AsmCtor(my_asm *const assembler, Text_Info *const onegin);

int FirstAsmCompile(my_asm *const assembler, token_s *const tokens);

int GetLabel(my_asm *const assembler, token_s *const tokens, int *jump_index);

int GetArg(my_asm *const assembler, token_s *const tokens);

int GetRamArgument(my_asm *const assembler, token_s *const tokens, int command);

int GetRegisterArgument(my_asm *const assembler, token_s *const tokens, int command);

int GetNumberArgument(my_asm *const assembler, token_s *const tokens, int command);

int FindLabels(my_asm *const assembler, token_s *const tokens);

int SecondAsmCompile(my_asm *const assembler, token_s *const tokens);

int SetLabels(my_asm *const assembler, token_s *const tokens, int *jump_index);

int AsmDtor(my_asm *const assembler);

#ifdef LISTING

    int ListingCtor();

    int ListingMake(my_asm *const assembler, Text_Info *const onegin);

    int ListingDtor();

#endif

#endif