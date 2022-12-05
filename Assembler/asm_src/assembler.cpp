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
/// @return The csize of file                                       from Onegin
static unsigned long Get_File_Size(FILE * fp);

//---------------------------------------------------------------------------------------------//

/// @brief Function fimds out the number of strings in file
/// @param src_file is ptr on src_file_info struct                  from Onegin
/// @return The nuber of strings
static unsigned long Get_Line_Count(src_file_info * const src_file);

//-----------------------------------------------------_----------------------------------------//

/// @brief Function makes strings out of source buffer
/// @param src_file is ptr on src_file_info struct
/// @return Alloc_Err if making of strings failed, No_Error if it's ok      from Onegin
static int Make_Strings(src_file_info * const src_file);

//---------------------------------------------------------------------------------------------//

/// @brief Function skips spaces until it finds out the digit
/// @param str is ptr on the string
/// @return Incorrect_Push_Arg if there's error, No_Error if isn't          Checks errors
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
/// @return Extra_Arg if there's error, No_Error if isn't               checks erros
static int Is_Extra_Arg(char ** str, int cur_num);

//---------------------------------------------------------------------------------------------//

/// @brief Function compares twp strings ignoring letter height
/// @param str1 is ptr on the first string
/// @param str2 is ptr on the second string                             important
/// @return 0 if strings are equal, another digit if they're not
static int Stricmp(const char * str1, const char * str2);

//---------------------------------------------------------------------------------------------//\

/// @brief Function workes with argument and adds it to the code array
/// @param src_file is ptr on src_file_info struct 
/// @param asmbly is ptr in asm_file_info struct
/// @param cur_num_str is a number of current string
/// @param cmd the value of command
/// @return the code of argument or Undefined_Arg if argument is incorrect
static int Get_Arg(src_file_info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd);

//---------------------------------------------------------------------------------------------//

/// @brief Functions checks if the argument is register
/// @param str is ptr on the string
/// @param asmbly is ptr in asm_file_info struct
/// @return The code of register or 0 if there's error
static int Is_Reg(char ** str, asm_file_info * const asmbly);

//---------------------------------------------------------------------------------------------//

void Asm_Ctor(FILE * input_file, src_file_info * const src_file, asm_file_info * const asmbly)
{
    assert(input_file);
    assert(src_file);

    if ((src_file->symbols_count = Get_File_Size(input_file)) == Reading_File_Err)
        asmbly->error = Reading_File_Err;

    char * tmp_alloc_mem = (char *) calloc(src_file->symbols_count + 1, sizeof(char));

    if (tmp_alloc_mem == nullptr)
        asmbly->error = Alloc_Err;
    
    src_file->buffer = tmp_alloc_mem;

    if (fread(src_file->buffer, sizeof(char), src_file->symbols_count, input_file) < src_file->lines_count)
        asmbly->error = Reading_File_Err;

    src_file->lines_count = Get_Line_Count(src_file) + 1;

    if (Make_Strings(src_file) == Alloc_Err)
        asmbly->error = Alloc_Err;

    asmbly->code_arr = (int *)calloc (2 * src_file->lines_count, sizeof(int));
    if (asmbly->code_arr == nullptr)
        asmbly->error = Alloc_Err;

    asmbly->jumps_index = (int *)calloc (2 * src_file->lines_count, sizeof(int));
    if (asmbly->jumps_index == nullptr)
        asmbly->error = Alloc_Err;
    
    asmbly->labels = (int *)calloc (DEF_LABEL_SIZE, sizeof(int));
    if (asmbly->labels == nullptr)
        asmbly->error = Alloc_Err;

    for (int label = 0; label < DEF_LABEL_SIZE; label++)
        asmbly->labels[label] = -1;
}

//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...)             \
    else if (Stricmp(#name, cmd) == 0)              \
    {                                               \
        if (arg)             \
        {                                               \
            (src_file->pointers[cur_num_str])+=cmd_len; \
            Get_Arg(src_file, asmbly, cur_num_str, number); \
            if (asmbly->error == Undefined_Arg)             \
            {                                                                               \
                fprintf(stderr, "Incorrect arg for command in line %d\n", cur_num_str + 1); \
                break;  \
            }           \
        }                       \
        else if (arg == 0)           \
        {                                                   \
            asmbly->code_arr[asmbly->cmd_num++] = number;                               \
            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)   \
            {                                                                           \
                asmbly->error = Extra_Arg;                                                                      \
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);  \
                break;  \
            }           \
        }               \
        cur_num_str++;  \
    }
//---------------------------------------------------------------------------------------------//

void Fisrt_Asm_Compile(src_file_info * const src_file, asm_file_info * const asmbly)
{
    assert(src_file);
    assert(asmbly);

    int cur_num_str = 0;
    int jump_index=  0;

    while (cur_num_str < (int) src_file->lines_count)
    {
        char cmd[DEF_CMD_LEN] = " ";
        int cmd_len = 0;
        sscanf(src_file->pointers[cur_num_str], "%s%n", cmd, &cmd_len);
        char * cur_str = src_file->pointers[cur_num_str];

        if (strchr(cmd, ':') != nullptr)
        {
            Skip_Space_To_Digit(&src_file->pointers[cur_num_str]);
            int label = 0;
            sscanf(src_file->pointers[cur_num_str], "%d", &label);
            asmbly->labels[label] = asmbly->cmd_num;
            cur_num_str++;
        }

        else if (strchr(cmd, 'j') != nullptr)
        {
            asmbly->jumps_index[jump_index++] = asmbly->cmd_num;
            asmbly->code_arr[asmbly->cmd_num++] = -1;
            asmbly->code_arr[asmbly->cmd_num++] = -1;
            cur_num_str++;
        }

        #include "../../cmd.h"

        else
        {
            Is_Space_Line(&src_file->pointers[cur_num_str], asmbly, cur_num_str);
            if (asmbly->error == Undefined_Cmd)
            {
                fprintf(stderr, "Error: Incorrect command '%s' in input file in line %d\n", cur_str, cur_num_str + 1);
                break;
            }
            else
                cur_num_str++;
        }
    }
}
#undef DEF_CMD
//---------------------------------------------------------------------------------------------//

#define DEF_CMD(name, number, arg, ...)             \
    else if (Stricmp(#name, cmd) == 0)               \
    {                                               \
        asmbly->code_arr[asmbly->jumps_index[jump_index]] = number; \
        (src_file->pointers[cur_num_str])+=cmd_len; \
        Skip_Space_To_Digit(&src_file->pointers[cur_num_str]);  \
        int label = 0;                                          \
        sscanf(src_file->pointers[cur_num_str], "%d", &label);      \
        asmbly->code_arr[asmbly->jumps_index[jump_index++] + 1] = asmbly->labels[label];\
        cur_num_str;\
    }

//---------------------------------------------------------------------------------------------//

void Second_Asm_Compile(src_file_info * const src_file, asm_file_info * const asmbly)
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
        char * cur_str = src_file->pointers[cur_num_str];

        if (strchr(cmd, ':') != nullptr)
            cur_num_str++;

        #include "../../jumps.h"

        else
        {
            cur_num_str++;
        }
    }
}
#undef DEF_CMD
//---------------------------------------------------------------------------------------------//

static int Get_Arg(src_file_info * const src_file, asm_file_info * const asmbly, int cur_num_str, int cmd)
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

//---------------------------------------------------------------------------------------------//

static int Is_Reg(char ** str, asm_file_info * const asmbly)
{
    assert(str);
    assert(*str);
    assert(asmbly);

    char reg[DEF_CMD_LEN] = " ";

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
        return REG_AX;

    else if (Stricmp(*str, "bx") == 0)
        return REG_BX;    
    
    else if (Stricmp(*str, "cx") == 0)
        return REG_CX;
    
    else if (Stricmp(*str, "dx") == 0)
        return REG_DX;

    else
    {
        asmbly->error = Undefined_Arg;
        fprintf(stderr, "Incorrect reg\n");
        return 0;
    }
}

//---------------------------------------------------------------------------------------------//             

void Asm_Dtor(src_file_info * const src_file, asm_file_info * const asmbly, FILE * input_file)
{
    free(src_file->buffer);
    free(src_file->pointers);
    free(asmbly->code_arr);
    free(asmbly->labels);
    free(asmbly->jumps_index);
    fclose(input_file);
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

    return (unsigned long) buf.st_size;
}

//---------------------------------------------------------------------------------------------//

static unsigned long Get_Line_Count(src_file_info * const src_file)
{
    assert(src_file);

    unsigned long str_num = 0;

    for (unsigned long ch = 0; ch < src_file->symbols_count; ch++)
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

    for (unsigned long ch = 0, cur_str = 1; ch < src_file->symbols_count; ch++)
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
        (*str)++; ///// * -> **
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
    assert(str1);

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

/*

        else if (Stricmp("push", cmd) == 0)
        {
            cmd_num+=2;
            cur_num_str++;
        }

        else if (Stricmp("pop", cmd) == 0)
        {
            cmd_num+=2;
            cur_num_str++;
        }

        else if (Stricmp("jb", cmd) == 0)
        {
            cmd_num+=2;
            cur_num_str++;
        }
        
        else if (Stricmp("add", cmd) == 0)
        {
            cmd_num++;
            cur_num_str++;
        }

        else if (Stricmp("mul", cmd) == 0)
        {
            cmd_num++;
            cur_num_str++;
        }

        else if (Stricmp("sub", cmd) == 0)
        {
            cmd_num++;
            cur_num_str++;
        }

        else if (Stricmp("div", cmd) == 0)
        {
            cmd_num++;
            cur_num_str++;
        }

        else if (Stricmp("out", cmd) == 0)
        {
            cmd_num++;
            cur_num_str++;
        }

        else if (Stricmp("hlt", cmd) == 0)
        {
            cmd_num++;
            cur_num_str++;
        }




        else if (Stricmp("push", cmd) == 0)
        {
            (src_file->pointers[cur_num_str])+=cmd_len;

            Get_Arg(src_file, asmbly, cur_num_str, CMD_PUSH);

            if (asmbly->error == Undefined_Arg)
            {
                fprintf(stderr, "Incorrect arg for command in line %d\n", cur_num_str + 1);
                break;
            }

            cur_num_str++;
        } 

        else if (Stricmp("jb", cmd) == 0)
        {
            (src_file->pointers[cur_num_str])+=cmd_len;
            asmbly->code_arr[asmbly->cmd_num++] = CMD_JB;
            Skip_Space_To_Digit(&src_file->pointers[cur_num_str]);
            int label = 0;
            sscanf(src_file->pointers[cur_num_str], "%d", &label);
            asmbly->code_arr[asmbly->cmd_num++] = asmbly->labels[label];
            cur_num_str++;
        }

        else if (Stricmp("pop", cmd) == 0)
        {
            (src_file->pointers[cur_num_str])+=cmd_len;

            Get_Arg(src_file, asmbly, cur_num_str, CMD_POP);

            if (asmbly->error == Undefined_Arg)
            {
                fprintf(stderr, "Incorrect arg for command in line %d\n", cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }

        else if (Stricmp("add", cmd) == 0)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_ADD;

            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)
            {
                asmbly->error = Extra_Arg;
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }

        else if (Stricmp("sub", cmd) == 0)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_SUB;

            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)
            {
                asmbly->error = Extra_Arg;
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }

        else if (Stricmp("mul", cmd) == 0)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_MUL;

            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)
            {
                asmbly->error = Extra_Arg;
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }      

        else if (Stricmp("div", cmd) == 0)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_DIV;

            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)
            {
                asmbly->error = Extra_Arg;
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }  


        else if (Stricmp("out", cmd) == 0)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_OUT;

            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)
            {
                asmbly->error = Extra_Arg;
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }

        else if (Stricmp("hlt", cmd) == 0)
        {
            asmbly->code_arr[asmbly->cmd_num++] = CMD_HLT;

            if (Is_Extra_Arg(&src_file->pointers[cur_num_str], cmd_len) == Extra_Arg)
            {
                asmbly->error = Extra_Arg;
                fprintf(stderr, "The string %s in line %d has an extra argument\n", cur_str, cur_num_str + 1);
                break;
            }

            cur_num_str++;
        }
*/