#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>

#include "../asm_includes/assembler.h"
#include "../../proc_config.h"
#include "../../architecture.h"
#include "../Onegin/text_functions.h"

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param src_file 
/// @param asmbly 
/// @param cur_num_str 
/// @param label_index 
/// @return 
static int Find_Labels(Text_Info * const src_file, asm_file_info * const asmbly, int * cur_num_str, int * label_index);

//---------------------------------------------------------------------------------------------//

static int Check_Label(asm_file_info * const asmbly, char * cur_label);

//---------------------------------------------------------------------------------------------//

static int Set_Labels(char * cur_str, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

static int Skip_Space_To_Label(char ** str);

//---------------------------------------------------------------------------------------------//

/// @brief Function skips spaces until it finds out the digit
/// @param str is ptr on the string
/// @return Incorrect_Push_Arg if there's error, No_Error if isn't          
static int Skip_Space_To_Digit(char ** str);

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out incorrect commands in source file
/// @param str is ptr on the string 
/// @param asmbly is ptr in asm_file_info struct                         
/// @param cur_num is a number of current string
static void Is_Space_Line(char ** str, asm_file_info * const asmbly, const int cur_num);

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out extra arguments of commands
/// @param str is prt on the string
/// @param cur_num is a number of current string
/// @return Extra_Arg if there's error, No_Error if isn't               
static int Is_Extra_Arg(char ** str, int cur_num);

//---------------------------------------------------------------------------------------------//

/// @brief Function compares twp strings ignoring letter height
/// @param str1 is ptr on the first string
/// @param str2 is ptr on the second string                         
/// @return 0 if strings are equal, another digit if they're not
static int Stricmp(const char * str1, const char * str2);

//---------------------------------------------------------------------------------------------//\

/// @brief Function workes with argument and adds it to the code array
/// @param src_file is ptr on Text_Info struct 
/// @param asmbly is ptr in asm_file_info struct
/// @param cur_num_str is a number of current string
/// @param cmd the value of command
/// @return the code of argument or Undefined_Arg if argument is incorrect
static int Get_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd);

//---------------------------------------------------------------------------------------------//

static int Make_Ram_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd);

static int Make_Common_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd);

/// @brief Functions checks if the argument is register
/// @param str is ptr on the string
/// @param asmbly is ptr in asm_file_info struct
/// @return The code of register or 0 if there's error
static int Is_Reg(char ** str, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------//

int Asm_Ctor(FILE * input_file, Text_Info * const src_file, asm_file_info * const asmbly)
{
    assert(input_file);
    assert(src_file); // TODO use Onegin

    if (Onegin_Read(src_file, input_file) != 0)
        return Reading_File_Err;

    asmbly->code_arr = (int *)calloc (2 * src_file->lines_count, sizeof(int));
    if (asmbly->code_arr == nullptr)
        return Alloc_Err;

    asmbly->jumps_index = (int *)calloc (2 * src_file->lines_count, sizeof(int));
    if (asmbly->jumps_index == nullptr)
        return Alloc_Err;

    asmbly->label_array_size = DEF_LABEL_SIZE;
    
    asmbly->labels = (Labels *)calloc (asmbly->label_array_size , sizeof(Labels));
    if (asmbly->labels == nullptr)
        return Alloc_Err;

    for (int label = 0; label < asmbly->label_array_size; label++)
    {
        asmbly->labels[label].label_num = -1;
        asmbly->labels[label].label_line = nullptr;
    }

    return No_Error;
}

//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...)                                                                         \
    else if (Stricmp(#name, cmd) == 0)                                                                          \
    {                                                                                                           \
        if (arg == Digit)                                                                         \
        {                                                                                                       \
            (src_file->pointers[cur_num_str])+=cmd_len;                                                         \
            if (Get_Arg(src_file, asmbly, cur_num_str, number) != No_Error)                              \
            {                                                                                                   \
                fprintf(stderr, "Incorrect arg for command in line %d\n", cur_num_str + 1);                     \
                break;                                                                                          \
            }                                                                                                   \
        }                                                                                                       \
        else if (arg == Label)                                                                                  \
        {                                                                                                       \
            asmbly->jumps_index[jump_index++] = asmbly->cmd_num;                                                \
            asmbly->code_arr[asmbly->cmd_num++] = -1;                                                           \
            asmbly->code_arr[asmbly->cmd_num++] = -1;                                                           \
        }                                                                                                       \
        else if (arg == 0)                                                                                      \
        {                                                                                                       \
            asmbly->code_arr[asmbly->cmd_num++] = number;                                                       \
            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)                           \
            {                                                                                                   \
                asmbly->error = Extra_Arg;                                                                      \
                fprintf(stderr, "The string |%s| in line %d has an extra argument\n", cur_str, cur_num_str + 1);\
                break;                                                                                          \
            }                                                                                                   \
        }                                                                                                       \
        cur_num_str++;                                                                                          \
    }
//---------------------------------------------------------------------------------------------//

int Fisrt_Asm_Compile(Text_Info * const src_file, asm_file_info * const asmbly)
{
    assert(src_file);
    assert(asmbly);

    int cur_num_str = 0;
    int jump_index  = 0;
    int label_index = 0;

    while (cur_num_str < (int) src_file->lines_count)
    {
        char cmd[DEF_CMD_LEN] = { 0 };
        int cmd_len = 0;
        sscanf(src_file->pointers[cur_num_str], "%s%n", cmd, &cmd_len);
        char * cur_str = src_file->pointers[cur_num_str];

        if (strchr(cmd, ':') != nullptr)
        {
            if (Find_Labels(src_file, asmbly, &cur_num_str, &label_index) == Label_Error)
            {
                fprintf(stderr, "Finding of labes failed\n");
                return Label_Error;
            }
        }

        #include "../../cmd.h"

        else
        {
            Is_Space_Line(&src_file->pointers[cur_num_str], asmbly, cur_num_str);
            if (asmbly->error == Undefined_Cmd)
            {
                fprintf(stderr, "Error: Incorrect command [%s] in input file in line %d\n", cur_str, cur_num_str + 1);
                break;
            }
            else
                cur_num_str++;
        }
    }
    return No_Error;
}
#undef DEF_CMD
//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...)                                                     \
    else if (Stricmp(#name, cmd) == 0 && arg == Label)                                      \
    {                                                                                       \
            asmbly->code_arr[asmbly->jumps_index[jump_index]] = number;                     \
            (src_file->pointers[cur_num_str])+=cmd_len;                                     \
            Skip_Space_To_Label(&src_file->pointers[cur_num_str]);                          \
            char * cur_str = src_file->pointers[cur_num_str];                               \
            asmbly->code_arr[asmbly->jumps_index[jump_index++] + 1] = Set_Labels(cur_str, asmbly);\
            cur_num_str++;\
    }

//---------------------------------------------------------------------------------------------//

int Second_Asm_Compile(Text_Info * const src_file, asm_file_info * const asmbly)
{
    assert(src_file);
    assert(asmbly);

    int cur_num_str = 0;
    int jump_index = 0;

    while (cur_num_str < (int) src_file->lines_count)
    {
        char cmd[DEF_CMD_LEN] = " ";
        int cmd_len = 0;
        sscanf(src_file->pointers[cur_num_str], "%s%n", cmd, &cmd_len);

        if (strchr(cmd, ':') != nullptr)
            cur_num_str++;

        #include "../../cmd.h"

        else
            cur_num_str++;
    }
    return No_Error;
}
#undef DEF_CMD

//---------------------------------------------------------------------------------------------//
/*
static int Get_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd)
{
    assert(src_file);
    assert(asmbly);

    int arg = 0;

    int reg = Is_Reg(&src_file->pointers[cur_num_str], asmbly);

    if (asmbly->error == Undefined_Arg)
        return Undefined_Arg;

    if (reg != 0)
    {
        if (cmd == CMD_PUSH)    
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_PUSH_REG;
            asmbly->code_arr[asmbly->cmd_num++] = reg;
            return Reg;
        }
        else if (cmd == CMD_POP)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_POP_REG;
            asmbly->code_arr[asmbly->cmd_num++] = reg;
            return Reg;
        }
    }

    sscanf(src_file->pointers[cur_num_str], "%d", &arg);

    asmbly->code_arr[asmbly->cmd_num++] = cmd;

    asmbly->code_arr[asmbly->cmd_num++] = arg;

    return Digit;

}
*/
//---------------------------------------------------------------------------------------------//

static int Is_Reg(char ** str, asm_file_info * const asmbly)
{
    assert(str);
    assert(*str);
    assert(asmbly);

    char reg[DEF_CMD_LEN] = { 0 };

    while (isspace(**str))
    {   
        (*str)++;

        if (isdigit(**str))
            return 0;
        if (isalpha(**str))
            break;
    }

    sscanf(*str, "%s", reg);

    if      (Stricmp(*str, "ax") == 0)
        return REG_RAX;

    else if (Stricmp(*str, "bx") == 0)
        return REG_RBX;    
    
    else if (Stricmp(*str, "cx") == 0)
        return REG_RCX;
    
    else if (Stricmp(*str, "dx") == 0)
        return REG_RDX;

    else
    {
        asmbly->error = Undefined_Arg;
        fprintf(stderr, "Incorrect reg in %s\n", *str);
        return 0;
    }
}

//---------------------------------------------------------------------------------------------//             

int Asm_Dtor(Text_Info * const src_file, asm_file_info * const asmbly, FILE * input_file)
{
    free(src_file->buffer);
    free(src_file->pointers);
    free(asmbly->code_arr);
    free(asmbly->labels);
    free(asmbly->jumps_index);
    fclose(input_file); // TODO check ret value 
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

int Check_Cmdline_Arg(int argc)
{
    if (argc != 4)
    {
        fprintf(stderr, "Error: wrong number of command line args: %d\n"
                "Expected %d\n", argc, 4);
        return Cmd_Line_Arg_Err;
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Skip_Space_To_Digit(char ** str)
{
    assert(str);
    assert(*str);

    while (!(isdigit(**str)))
    {
        if (isalpha(**str))
            return Incorrext_Cmd_Arg;
        (*str)++;
    }
    return No_Error;
}  

//---------------------------------------------------------------------------------------------//

static void Is_Space_Line(char ** str, asm_file_info * const asmbly, int cur_num)
{
    assert(str);
    assert(*str);
    assert(cur_num);

    int space_count = 0;
    while(**str != '\0')
    {
        if (isspace(**str))
            space_count++;
        (*str)++;
    }
    if (space_count != (int) strlen(*str))
        asmbly->error = Undefined_Cmd;
    else
        asmbly->error = No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Is_Extra_Arg(char ** str, int cmd_len)
{
    assert(str);
    assert(*str);

    (*str)+=cmd_len;

    while (**str != '\0')
    {
        if (!(isspace(**str)))
            return Extra_Arg;
        (*str)++;
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Stricmp(const char * str1, const char * str2)
{
    assert(str1);
    assert(str2);

    while(tolower(*str1) == tolower(*str2))
    {
        if (*str1 == '\0')
            return 0;

        str1++;
        str2++;
    }
    return (int) (str1 - str2);
}

//---------------------------------------------------------------------------------------------//

static int Is_Label(const char ** str)
{
    assert(str);
    assert(*str);

    while (**str != ':')
    {
        if (**str == ':')
            return Label;
        (*str)++;
    }
    return 0;
}

//---------------------------------------------------------------------------------------------//

static int Find_Labels(Text_Info * const src_file, asm_file_info * const asmbly, int * cur_num_str, int * label_index)
{
    assert(src_file);
    assert(asmbly);

    char * cur_label = src_file->pointers[*cur_num_str];
    cur_label[strlen(cur_label) - 1] = '\0';

    printf("%s\n", cur_label);

    if (Check_Label(asmbly, cur_label) == Label_Error)
    {
        fprintf(stderr, "The same label |%s| in line %d\n", cur_label, *cur_num_str + 1);
        return Label_Error;
    }

    asmbly->labels[*label_index].label_line = cur_label;
    asmbly->labels[*label_index].label_num  = asmbly->cmd_num;
    *label_index+=1;

    return No_Error;
}


//---------------------------------------------------------------------------------------------//

static int Check_Label(asm_file_info * const asmbly, char * cur_label)
{
    assert(asmbly);
    assert(cur_label);

    for (int label = 0; label < asmbly->label_array_size; label++)
    {
        if (asmbly->labels[label].label_line != nullptr && Stricmp(cur_label, asmbly->labels[label].label_line) == 0)
        {
            asmbly->error = Label_Error;
            return Label_Error;
        }
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Set_Labels(char * cur_str, asm_file_info * const asmbly)
{
    assert(cur_str);
    assert(asmbly);
    
    int label = 0;

    for (int label = 0; label < asmbly->label_array_size; label++)
    {
        if (Stricmp(asmbly->labels[label].label_line, cur_str) == 0)
            return asmbly->labels[label].label_num;
    }

    return -1;
}

//---------------------------------------------------------------------------------------------//

static int Skip_Space_To_Label(char ** str)
{
    assert(str);
    assert(*str);

    while (!(isalpha(**str)))
    {
        if (isdigit(**str))
            return Incorrext_Cmd_Arg;
        (*str)++;
    }
    return No_Error;
}


//---------------------------------------------------------------------------------------------//

static int Get_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd)
{
    assert(src_file);
    assert(asmbly);

    if (strchr(src_file->pointers[cur_num_str], '[') != nullptr)
    {
        if (Make_Ram_Arg(src_file, asmbly, cur_num_str, cmd) == Undefined_Arg)
        {
            fprintf(stderr, "Error with making ram arg in |%s| in line %d\n", src_file->pointers[cur_num_str], cur_num_str + 1);
            return Undefined_Arg;
        }
    }

    else
    {
        if (Make_Common_Arg(src_file, asmbly, cur_num_str, cmd) == Undefined_Arg)
        {
            fprintf(stderr, "Error with making common arg in |%s| in line %d\n", src_file->pointers[cur_num_str], cur_num_str + 1);
            return Undefined_Arg;
        }
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Make_Ram_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd)
{
    assert(src_file);
    assert(asmbly);

    cmd |= ARG_RAM;

    int immed_arg = 0;
    char reg_arg[DEF_CMD_LEN] = { 0 };
    char * cur_str = strchr(src_file->pointers[cur_num_str], '[') + 1;

    if (strchr(cur_str, '+') != nullptr)
    {
        sscanf(cur_str, "%d+%10s", &immed_arg, reg_arg);
        reg_arg[strlen(reg_arg) - 1] = '\0';
        cur_str = reg_arg;
        int reg = Is_Reg(&cur_str, asmbly);

        if (asmbly->error == Undefined_Arg)
            return Undefined_Arg;

        cmd = cmd | ARG_IMMED | ARG_REG;
        asmbly->code_arr[asmbly->cmd_num++] = cmd;
        asmbly->code_arr[asmbly->cmd_num++] = immed_arg;
        asmbly->code_arr[asmbly->cmd_num++] = reg;
    }

    else
    {
        if (sscanf(cur_str, "%d", &immed_arg))
        {
            cmd |= ARG_IMMED;
            asmbly->code_arr[asmbly->cmd_num++] = cmd;
            asmbly->code_arr[asmbly->cmd_num++] = immed_arg;
        }
        else if (sscanf(cur_str, "%s", reg_arg))
        {
            reg_arg[strlen(reg_arg) - 1] = '\0';
            cur_str = reg_arg;
            int reg = Is_Reg(&cur_str, asmbly);

            if (asmbly->error == Undefined_Arg)
                return Undefined_Arg;

            cmd |= ARG_REG;
            asmbly->code_arr[asmbly->cmd_num++] = cmd;
            asmbly->code_arr[asmbly->cmd_num++] = reg;
        }
        else
            return Undefined_Arg;
    }
    
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Make_Common_Arg(Text_Info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd)
{
    assert(src_file);
    assert(asmbly);

    int arg = 0;

    int reg = Is_Reg(&src_file->pointers[cur_num_str], asmbly);

    if (asmbly->error == Undefined_Arg)
        return Undefined_Arg;

    if (reg != 0)
    {
        cmd |= ARG_REG;
        asmbly->code_arr[asmbly->cmd_num++] = cmd;
        asmbly->code_arr[asmbly->cmd_num++] = reg;
        return Reg;
    }

    sscanf(src_file->pointers[cur_num_str], "%d", &arg);

    cmd |= ARG_IMMED;
    asmbly->code_arr[asmbly->cmd_num++] = cmd;
    asmbly->code_arr[asmbly->cmd_num++] = arg;

    return Digit;
}