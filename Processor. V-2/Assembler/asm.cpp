#include "asm.hpp"
#include "token.hpp"
#include "../Architecture/dsl.hpp"

#include <ctype.h>

static FILE                 *listing = nullptr;

static const char           *const list_file_name = "listing.txt";

static int                  IsExtraArg(token_s *const tokens);

static int                  ListingArgument(my_asm *const assembler, int index);

//===================================================================================//

int CkeckCmdLineArgs(const int argc)
{
    if (argc != Cmd_Line_Arg_Count)
    {
        fprintf(stderr, "Incorrect count of command line arguments\n");

        fprintf(stderr, "Expected:  %d\n", Cmd_Line_Arg_Count);

        fprintf(stderr, "Real:      %d\n", argc);
        
        return Cmd_Line_Arg_Error;
    }

    return No_Error;
}

//===================================================================================//

int AsmCtor(my_asm *const assembler, Text_Info *const onegin)
{
    assembler->array = (code_t *)calloc ((size_t) (4 * onegin->lines_count), sizeof(code_t));

    if (assembler->array == nullptr)
    {
        fprintf(stderr, "Failed allocation memory for asm array in %s\n", __func__);
        return Bad_Alloc;
    }

    assembler->jump_indexes = (int *)calloc (Def_Label_Size, sizeof(int));

    if (assembler->jump_indexes == nullptr)
    {
        fprintf(stderr, "Failed allocation memory for jumps indexes array in %s\n", __func__);
        return Bad_Alloc;
    }

    assembler->label_size = Def_Label_Size;

    assembler->labels = (label_s *)calloc (assembler->label_size, sizeof(label_s));

    if (assembler->labels == nullptr)
    {
        fprintf(stderr, "Failed allocation memory for label array in %s\n", __func__);
        return Bad_Alloc;
    }

    return No_Error;
}

//===================================================================================//

int FirstAsmCompile(my_asm *const assembler, token_s *const tokens)
{
    int jump_index = 0;

    while (tokens->size != tokens->capacity)
    {
        if (Token_Type == Word_Type)
        {
            switch(Token_Data)
            {
                case CMD_PUSH:
                    GetArg(assembler, tokens);
                    break;

                case CMD_POP:
                    GetArg(assembler, tokens);
                    break;
                
                case CMD_CALL:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                case CMD_JMP:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                case CMD_JA:
                    GetLabel(assembler, tokens, &jump_index);
                    break;
                
                case CMD_JAE:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                case CMD_JB:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                case CMD_JBE:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                case CMD_JE:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                case CMD_JNE:
                    GetLabel(assembler, tokens, &jump_index);
                    break;

                default:
                {
                    if (IsExtraArg(tokens) == Extra_Arg)
                    {
                        fprintf(stderr, "Assembly failed in line %d\n", __LINE__);
                        return Asm_Failed;
                    }
                    assembler->array[assembler->file_size++] = (code_t) Token_Data;
                    tokens->size++;
                    break;
                }
            }
        }
        
        else if (Token_Type == Start_Label_Type)
        {
            FindLabels(assembler, tokens); 
        }
    }

    return No_Error;
}

//===================================================================================//

int GetArg(my_asm *const assembler, token_s *const tokens) // push <arg> / pop <arg>
{
    int command = Token_Data;
    //assembler->array[assembler->file_size] = command;
    tokens->size++; // now it's <arg>

    switch(Token_Type)
    {
        case Bracket_Type:
            GetRamArgument(assembler, tokens, command);
            break;

        case Reg_Type:
            GetRegisterArgument(assembler, tokens, command);
            break;

        case Num_Type:
            GetNumberArgument(assembler, tokens, command);
            break;

        default:
        {
            fprintf(stderr, "Incorrect argument %s in %s\n", Token_Name, __func__);
            return Incorrect_Arg;
        }
    }

    return No_Error;
}

//===================================================================================//

int GetLabel(my_asm *const assembler, token_s *const tokens, int *jump_index)
{
    assembler->array[assembler->file_size]  = (code_t) Token_Data;

    assembler->jump_indexes[*jump_index]    = assembler->file_size + 1;

    (*jump_index)++;

    tokens->size++;

    int label_ind = 0;

    memcpy(&assembler->array[assembler->file_size + 1], &label_ind, sizeof(int));

    assembler->file_size += 1 + sizeof(int);

    tokens->size++;

    return No_Error;
}

//===================================================================================//

static int IsExtraArg(token_s *const tokens)
{
    if (Token_Type != Word_Type)
    {
        fprintf(stderr, "There's extra argument %s\n", Token_Name);
        return Extra_Arg;
    }

    return No_Error;
}

//===================================================================================//

int GetRamArgument(my_asm *const assembler, token_s *const tokens, int command) // [ <fisrt_arg> + <second_arg> ]
{
    command |= ARG_RAM;
    tokens->size++; //now it's <first_arg>

    if (tokens->array[tokens->size + 1].type == Plus_Type)
    {
        assert(Token_Type == Reg_Type);

        memcpy(&assembler->array[assembler->file_size + 1], &Token_Data, sizeof(int));

        tokens->size++; // now it's +

        assert(Token_Type == Plus_Type);

        tokens->size++; // now it's <second_arg>

        assert(Token_Type == Num_Type);

        int number_value = 0;

        sscanf(tokens->array[tokens->size].name, "%d", &number_value);

        memcpy(&assembler->array[assembler->file_size + 1 + sizeof(int)], &number_value, sizeof(int));

        command = command | ARG_IMMED | ARG_REG;

        assembler->array[assembler->file_size] = (code_t) command;

        assembler->file_size += 1 + 2 * sizeof(int);

        tokens->size++; // now it's ]
    }

    else 
    {
        if (Token_Type == Reg_Type)
        {
            memcpy(&assembler->array[assembler->file_size + 1], &Token_Data, sizeof(int));

            command |= ARG_REG;

            assembler->array[assembler->file_size] = (code_t) command;

            assembler->file_size += 1 + sizeof(int);

            tokens->size++; // now it's ]
        }

        else if (Token_Type == Num_Type)
        {
            int number_value = 0;

            sscanf(tokens->array[tokens->size].name, "%d", &number_value);

            memcpy(&assembler->array[assembler->file_size + 1], &number_value, sizeof(int));

            command |= ARG_IMMED;

            assembler->array[assembler->file_size] = (code_t) command;

            assembler->file_size += 1 + sizeof(int);

            tokens->size++; // now it's ]
        }
    }

    assert(Token_Type == Bracket_Type);

    tokens->size++;

    return No_Error;
}

//===================================================================================//

int GetRegisterArgument(my_asm *const assembler, token_s *const tokens, int command)
{
    command |= ARG_REG;

    assembler->array[assembler->file_size] = (code_t) command;

    memcpy(&assembler->array[assembler->file_size + 1], &Token_Data, sizeof(int));

    assembler->file_size += 1 + sizeof(int);

    tokens->size++;

    return No_Error;
}

//===================================================================================//

int GetNumberArgument(my_asm *const assembler, token_s *const tokens, int command)
{
    int number_value = 0;

    sscanf(tokens->array[tokens->size].name, "%d", &number_value);

    command |= ARG_IMMED;

    assembler->array[assembler->file_size] = (code_t) command;

    memcpy(&assembler->array[assembler->file_size + 1], &number_value, sizeof(int));

    assembler->file_size += 1 + sizeof(int);

    tokens->size++;

    return No_Error;
}

//===================================================================================//

int FindLabels(my_asm *const assembler, token_s *const tokens)
{
    assembler->labels[assembler->label_count].label_name = tokens->array[tokens->size].name;

    tokens->array[tokens->size].name[strlen(tokens->array[tokens->size].name) - 1] = '\0';
    
    assembler->labels[assembler->label_count].label_num  = assembler->file_size;

    assembler->label_count++;

    tokens->size++;

    return No_Error;
}

//===================================================================================//

int SecondAsmCompile(my_asm *const assembler, token_s *const tokens)
{
    tokens->size    = 0;

    int jump_index  = 0;

    while (tokens->size != tokens->capacity)
    {
        if (Token_Type == Call_Label_Type)
        {
            if (SetLabels(assembler, tokens, &jump_index) == Incorrect_Label)
            {
                fprintf(stderr, "Failed setting labels in %s\n", __func__);
                return Incorrect_Label;
            }
        }

        tokens->size++;
    }

    return No_Error;
}

//===================================================================================//

int SetLabels(my_asm *const assembler, token_s *const tokens, int *jump_index)
{
    for (int label_idx = 0; label_idx < assembler->label_count; label_idx++)
    {
        if (strcmp(Token_Name, assembler->labels[label_idx].label_name) == 0)
        {
            memcpy(&assembler->array[assembler->jump_indexes[*jump_index]], 
                   &assembler->labels[label_idx].label_num, sizeof(int));

            (*jump_index)++;

            return No_Error;
        }
    }

    return Incorrect_Label;
}

//===================================================================================//

int AsmDtor(my_asm *const assembler)
{
    free(assembler->array);

    free(assembler->labels);
    
    free(assembler->jump_indexes);

    return No_Error;
}

//===================================================================================//

#ifdef LISTING

#       define      DEF_CMD(word_code, number, length, arg_type, code)                                     \
                                                                                                            \
    else if (Strincmp(#word_code, command_name, length) == 0)                                                \
    {                                                                                                         \
        switch(arg_type)                                                                                       \
        {                                                                                                       \
            case Digit_Type:                                                                                     \
            {                                                                                                     \
                fprintf(listing, "\n%04d    %04d    %-25s       %02d    %02d",                                     \
                current_string_number, ip, current_string, assembler->array[ip], ListingArgument(assembler, ip));   \
                ip += 1 + sizeof(int);                                                       \
                break;                                                                        \
            }                                                                                  \
            case Label_Type:                                                                    \
            {                                                                                    \
                fprintf(listing, "\n%04d    %04d    %-25s       %02d    %02d",                    \
                current_string_number, ip, current_string, number, ListingArgument(assembler, ip));\
                ip += 1 + sizeof(int);                                  \
                break;                                                   \
            }                                                             \
            case No_Arg_Type:                                              \
            {                                                               \
                fprintf(listing, "\n%04d    %04d    %-25s       %02d",       \
                current_string_number, ip, current_string, number);           \
                ip++;                                                          \
                break;                                                          \
            }                                                                    \
        }                                                                         \
    }

//===================================================================================//

int ListingCtor()
{
    listing = fopen(list_file_name, "w");

    if (listing == nullptr)
    {
        fprintf(stderr, "Failed openning listing file in %s\n", __func__);
        return File_Error;
    }

    return No_Error;
}

//===================================================================================//

int ListingMake(my_asm *const assembler, Text_Info *const onegin)
{
    int current_string_number   = 0;
    int ip                      = 0;

    fprintf(listing, "Line    Adress  Text                            Code\n");

    while (current_string_number != onegin->lines_count)
    {
        char command_name[Max_String_Len] = "";
        sscanf(onegin->pointers[current_string_number], "%s", command_name);
        char *current_string = onegin->pointers[current_string_number];

        if (strchr(command_name, ':') != nullptr)
        {
            fprintf(listing, "\n%04d    %04d    %-25s", 
            current_string_number, ip, onegin->pointers[current_string_number]);
        }

#           include     "../Architecture/cmd.hpp"

#           undef       DEF_CMD

        else
        {
            fprintf(listing, "\n%04d    %04d    %-25s",
            current_string_number, ip, onegin->pointers[current_string_number]);
        }

        current_string_number++;

    }

    return No_Error;
}

//===================================================================================//

static int ListingArgument(my_asm *const assembler, int index)
{
    int argument =  assembler->array[index + 1] +
                    assembler->array[index + 2] * 256 +
                    assembler->array[index + 3] * 256 * 256 +
                    assembler->array[index + 4] * 256 * 256 *256;

    return argument;
}

//===================================================================================//

int ListingDtor()
{
    if (fclose(listing) == EOF)
    {
        fprintf(stderr, "Failed closing listing file in %s\n", __func__);
        return File_Error;
    }
    
    return No_Error;
}

#endif

//===================================================================================//