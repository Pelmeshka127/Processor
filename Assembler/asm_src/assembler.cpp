#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>

#include "../asm_includes/assembler.h"
#include "../../proc_config.h"

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out the size of source file
/// @param fp is a ptr on source file
/// @return The csize of file
static unsigned long Get_File_Size(FILE * fp);

//---------------------------------------------------------------------------------------------//

/// @brief Function fimds out the number of strings in file
/// @param src_file is ptr on src_file_info struct
/// @return The nuber of strings
static unsigned long Get_Line_Count(src_file_info * const src_file);

//-----------------------------------------------------_----------------------------------------//

/// @brief Function makes strings out of source buffer
/// @param src_file is ptr on src_file_info struct
/// @return Alloc_Err if making of strings failed, No_Error if it's ok
static int Make_Strings(src_file_info * const src_file);

//---------------------------------------------------------------------------------------------//

/// @brief Function skips spaces until it finds out the digit
/// @param str is ptr on the letter of string
/// @return Incorrect_Push_Arg if there's error, No_Error if isn't
static int Skip_Space_To_Digit(char ** str, int cmd_len);

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out incorrect commands in source file
/// @param src_file is ptr on src_file_info struct
/// @param cur_num is a number of current string
static void Is_Space_Line(src_file_info * const src_file, const int cur_num);

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out extra arguments of commands
/// @param str is prt on the letter of string
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

/// @brief 
/// @param src_file 
/// @param cur_str 
/// @param cmd_len 
/// @param cur_num_str 
/// @return 
static int Get_Args(src_file_info * const src_file, asm_file_info * const asmbly, char * cur_str, int cmd_len, int cur_num_str);

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param src_file 
/// @param asmbly 
/// @param cur_str 
/// @param cmd_len 
/// @param cur_num_str 
/// @param arg 
/// @param number
/// @return 
static int Write_Cmd_To_Code_Arr(src_file_info * const src_file, asm_file_info * const asmbly, 
                                char * cur_str, int cmd_len, int cur_num_str, int arg, int number);

//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...)                                                                     \
    else if (Stricmp(cmd, #name) == 0)                                                                      \
    {                                                                                                       \
        char * cur_str = src_file->pointers[cur_num_str];                                                   \
        if (arg)                                                                                            \
        {                                                                                                   \
            Get_Args(src_file, asmbly, cur_str, cmd_len, cur_num_str);                                      \
            if (src_file->error != No_Error)                                                                \
                break;                                                                                      \
            cur_num_str++;                                                                                  \
        }                                                                                                   \
        else                                                                                                \
        {                                                                                                   \
            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)                       \
            {                                                                                               \
                src_file->error = Extra_Arg;                                                                \
                fprintf(stderr, "The string %s in line %d has extra argument\n", cur_str, cur_num_str + 1); \
                break;                                                                                      \
            }                                                                                               \
            asmbly->code_arr[asmbly->cmd_num] = number;                                                     \
            asmbly->cmd_num++;                                                                              \
            cur_num_str++;                                                                                  \
        }                                                                                                   \
    }



//---------------------------------------------------------------------------------------------//

void Asm_Ctor(FILE * input_file, src_file_info * const src_file)
{
    assert(input_file);
    assert(src_file);

    if ((src_file->symbols_count = Get_File_Size(input_file)) == Reading_File_Err)
        src_file->error = Reading_File_Err;

    char * tmp_alloc_mem = (char *) calloc(src_file->symbols_count + 1, sizeof(char));

    if (tmp_alloc_mem == nullptr)
        src_file->error = Alloc_Err;
    
    src_file->buffer = tmp_alloc_mem;

    if (fread(src_file->buffer, sizeof(char), src_file->symbols_count, input_file) < src_file->lines_count)
        src_file->error = Reading_File_Err;

    src_file->lines_count = Get_Line_Count(src_file) + 1;

    if (Make_Strings(src_file) == Alloc_Err)
        src_file->error = Alloc_Err;
}

//---------------------------------------------------------------------------------------------//

void Asm_Compile(src_file_info * const src_file, asm_file_info * const asmbly)
{
    assert(src_file);
    assert(asmbly);

    int cur_num_str = 0;

    while (cur_num_str < src_file->lines_count)
    {
        char cmd[DEF_CMD_LEN] = " ";
        int cmd_len = 0;
        sscanf(src_file->pointers[cur_num_str], "%s%n", cmd, &cmd_len);

        if (strchr(cmd, ':') != 0)
            break;

        #include "../../cmd.h"
        
        else
        {
            Is_Space_Line(src_file, cur_num_str);
            if (src_file->error == Undefined_Cmd)
            {
                fprintf(stderr, "Error: Incorrect command '%s' in input file in line %d\n", src_file->pointers[cur_num_str], cur_num_str + 1);
                break;
            }
            else
                cur_num_str++;
        }
    #undef DEF_CMD
    }
}

//---------------------------------------------------------------------------------------------//

void Asm_Dtor(src_file_info * const src_file, asm_file_info * const asmbly, FILE * input_file)
{
    free(src_file->buffer);
    free(src_file->pointers);
    free(asmbly->code_arr);
    fclose(input_file);
}

//---------------------------------------------------------------------------------------------//

static int Write_Cmd_To_Code_Arr(src_file_info * const src_file, asm_file_info * const asmbly, 
                                char * cur_str, int cmd_len, int cur_num_str, int arg, int number)
{
    assert(src_file);
    assert(asmbly);
    assert(cur_str);

    if (!arg)
    {
        if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)                      
        {                                                                                              
            src_file->error = Extra_Arg;                                                               
            fprintf(stderr, "The string %s in line %d has extra argument\n", cur_str, cur_num_str + 1);
            return Extra_Arg;                                                                               
        }                  
    }
                                                                                 
    asmbly->code_arr[asmbly->cmd_num] = number;                                                    
    asmbly->cmd_num++;                                                                             
    cur_num_str++;     

    return No_Error;                                                                            
}                      

static int Get_Args(src_file_info * const src_file, asm_file_info * const asmbly, char * cur_str, int cmd_len, int cur_num_str)
{
    assert(src_file);
    assert(asmbly);
    assert(cur_str);

    int cmd_arg = 0, arg_len = 0;

    if (Skip_Space_To_Digit(&src_file->pointers[cur_num_str], cmd_len) == Incorrext_Cmd_Arg)
    {
        src_file->error = Incorrext_Cmd_Arg;
        fprintf(stderr, "Incorrect arg in %s, in line%d\n", cur_str, cur_num_str + 1);
        return Incorrext_Cmd_Arg;
    }

    sscanf(src_file->pointers[cur_num_str], "%d%n", &cmd_arg, &arg_len);
    
    if (Is_Extra_Arg(&src_file->pointers[cur_num_str], arg_len) == Extra_Arg)
    {
        src_file->error = Extra_Arg;
        fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
        return Extra_Arg;
    }

    asmbly->code_arr[asmbly->cmd_num] = CMD_PUSH;
    asmbly->code_arr[asmbly->cmd_num+ 1] = cmd_arg;
    asmbly->cmd_num+=2;

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

static unsigned long Get_File_Size(FILE * fp)
{
    struct stat buf = {};

    if (fstat(fileno(fp), &buf) == -1)
        return Reading_File_Err;

    return buf.st_size;
}

//---------------------------------------------------------------------------------------------//

static unsigned long Get_Line_Count(src_file_info * const src_file)
{
    assert(src_file);

    unsigned long str_num = 0;

    for (int ch = 0; ch < src_file->symbols_count; ch++)
        if (src_file->buffer[ch] == '\n')
            str_num++;

    return str_num;
}

//---------------------------------------------------------------------------------------------//

static int Make_Strings(src_file_info * src_file)
{
    assert(src_file);

    char ** Tmp_Mem = (char **)calloc (src_file->lines_count + 1, sizeof(char *));
    
    if (Tmp_Mem == nullptr)
        return Alloc_Err;

    src_file->pointers = Tmp_Mem;

    *(src_file->pointers) = src_file->buffer;

    for (int ch = 0, cur_str = 1; ch < src_file->symbols_count; ch++)
    {
        if (src_file->buffer[ch] == '\n')
        {
            src_file->buffer[ch] = '\0';
            src_file->pointers[cur_str++] = &src_file->buffer[ch + 1];
        }
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static int Skip_Space_To_Digit(char ** str, int cmd_len)
{
    assert(str);
    assert(*str);

    (*str)+=cmd_len;

    while (!(isdigit(**str)))
    {
        if (isalpha(**str))
            return Incorrext_Cmd_Arg;
        (*str)++;
    }

    return No_Error;
}  

//---------------------------------------------------------------------------------------------//

static void Is_Space_Line(src_file_info * const src_file, int cur_num)
{
    assert(src_file);
    assert(cur_num);

    int space_count = 0;
    while(**src_file->pointers != '\0')
    {
        if (isspace(**src_file->pointers))
            space_count++;
        (*src_file->pointers)++; ///// * -> **
    }
    if (space_count != strlen(src_file->pointers[cur_num]))
        src_file->error = Undefined_Cmd;
    else
        src_file->error = No_Error;
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
    assert(str1);

    while(tolower(*str1) == tolower(*str2))
    {
        if (*str1 == '\0')
            return 0;

        str1++;
        str2++;
    }
    
    return str1 - str2;

}
/*
#define DEF_CMD(name, number, arg, ...)                                                                     \
    else if (Stricmp(cmd, #name) == 0)                                                                      \
    {                                                                                                       \
        char * cur_str = src_file->pointers[cur_num_str];                                                   \
                                                                                                            \
        Write_Cmd_To_Code_Arr(src_file, asmbly, cur_str, cur_num_str, arg, number)                          \
        if (src_file->error != No_Error)                                                                \
            break;                                                              \
                                                                                                            \
        if (arg)
        {
            Get_Args(src_file, asmbly, cur_str, cmd_len, )
        }

    }
*/