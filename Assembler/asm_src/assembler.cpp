#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>

#include "../asm_includes/assembler.h"
#include "../asm_includes/proc_config.h"

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out the size of source file
/// @param fp is a ptr on source file
/// @return The csize of file
static unsigned long Get_File_Size(FILE * fp);

//---------------------------------------------------------------------------------------------//

/// @brief Function fimds out the number of strings in file
/// @param asmbly is ptr on Asm_File_Info struct
/// @return The nuber of strings
static unsigned long Get_Line_Count(Asm_File_Info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function makes strings out of source buffer
/// @param asmbly is ptr on Asm_File_Info struct
/// @return Alloc_Err if making of strings failed, No_Error if it's ok
static int Make_Strings(Asm_File_Info * const asmbly);

//---------------------------------------------------------------------------------------------//

/// @brief Function skips spaces until it finds out the digit
/// @param str is ptr on the letter of string
static void Skip_Space_To_Digit(char ** str);

//---------------------------------------------------------------------------------------------//

/// @brief Function finds out incorrect commands in source file
/// @param asmbly is ptr on Asm_File_Info struct
/// @param cur_num is a number of current string
static void Is_Space_Line(Asm_File_Info * const asmbly, int cur_num);

//---------------------------------------------------------------------------------------------//

static void Is_Extra_Arg(Asm_File_Info * const asmbly, int cur_num);

//---------------------------------------------------------------------------------------------//

int Asm_Ctor(FILE * input_file, Asm_File_Info * const asmbly)
{
    assert(input_file);
    assert(asmbly);

    if ((asmbly->symbols_count = Get_File_Size(input_file)) == Reading_File_Err)
        return Reading_File_Err;

    char * tmp_alloc_mem = (char *) calloc(asmbly->symbols_count + 1, sizeof(char));

    if (tmp_alloc_mem == nullptr)
        return Reading_File_Err;
    
    asmbly->buffer = tmp_alloc_mem;

    if (fread(asmbly->buffer, sizeof(char), asmbly->symbols_count, input_file) < asmbly->lines_count)
        return Reading_File_Err;

    asmbly->lines_count = Get_Line_Count(asmbly) + 1;

    if (Make_Strings(asmbly) == Alloc_Err)
        return Reading_File_Err;

    return No_Error;
}

//---------------------------------------------------------------------------------------------//

void Asm_Compile(Asm_File_Info * const asmbly, int * const code_array)
{
    assert(asmbly);
    assert(code_array);

    int cur_num_str = 0;

    while (cur_num_str < asmbly->lines_count)
    {
        char cmd[DEF_CMD_LEN] = " ";
        
        sscanf(asmbly->pointers[cur_num_str], "%s", cmd);

        if (strcmp(cmd, "push") == 0)
        {
            int value = 0;
            Skip_Space_To_Digit(&asmbly->pointers[cur_num_str]);
            sscanf(asmbly->pointers[cur_num_str], "%d", &value);

            code_array[asmbly->cmd_num] = CMD_PUSH;
            code_array[asmbly->cmd_num + 1] = value;
            cur_num_str++;
            asmbly->cmd_num+=2;
        }

        else if (strcmp(cmd, "pop") == 0)
        {
            code_array[asmbly->cmd_num] = CMD_POP;
            cur_num_str++;
            asmbly->cmd_num++;
        }

        else if (strcmp(cmd, "add") == 0)
        {
            code_array[asmbly->cmd_num] = CMD_ADD;
            cur_num_str++;
            asmbly->cmd_num++;
        }

        else if (strcmp(cmd, "sub") == 0)
        {
            code_array[asmbly->cmd_num] = CMD_SUB;
            cur_num_str++;
            asmbly->cmd_num++;            
        }

        else if (strcmp(cmd, "mul") == 0)
        {
            code_array[asmbly->cmd_num] = CMD_MUL;
            cur_num_str++;
            asmbly->cmd_num++;
        }

        else if (strcmp(cmd, "div") == 0)
        {
            //Is_Extra_Arg(asmbly, cur_num_str);
            if (asmbly->error == Extra_Arg)
            {
                fprintf(stderr, "The string %s in line %d has extra argument\n", asmbly->pointers[cur_num_str], cur_num_str + 1);
                break;
            }
            code_array[asmbly->cmd_num] = CMD_DIV;
            cur_num_str++;
            asmbly->cmd_num++;
        }

        else if (strcmp(cmd, "out") == 0)
        {
            code_array[asmbly->cmd_num] = CMD_OUT;
            cur_num_str++;
            asmbly->cmd_num++;
        }

        else if (strcmp(cmd, "hlt") == 0)
        {
            code_array[asmbly->cmd_num] = CMD_HLT;
            cur_num_str++;
            asmbly->cmd_num++;
        }

        else
        {
            Is_Space_Line(asmbly, cur_num_str);
            if (asmbly->error == Undefined_Cmd)
            {
                fprintf(stderr, "Error: Incorrect command '%s' in input file in line %d\n", asmbly->pointers[cur_num_str], cur_num_str + 1);
                break;
            }
            else
                cur_num_str++;
        }

    }
}

//---------------------------------------------------------------------------------------------//

void Asm_Print_File(FILE * text_file, FILE * bin_file, Asm_File_Info * const asmbly, int * const code_array)
{
    assert(text_file);
    assert(bin_file);
    assert(asmbly);
    assert(code_array);
    
#ifdef TEXT_FILE
    //fprintf(interpret_file, "%d\n%d\n", DEF_CP, asmbly->cmd_num);

    for (int cmd = 0; cmd < asmbly->cmd_num; cmd++)
        fprintf(text_file, "%d ", code_array[cmd]);
#endif

#ifdef BIN_FILE
    fwrite(code_array, sizeof(int), asmbly->cmd_num, bin_file);
#endif

}

//---------------------------------------------------------------------------------------------//

void Asm_Dtor(Asm_File_Info * const asmbly, int * const code_array)
{
    free(asmbly->buffer);
    free(asmbly->pointers);
    free(code_array);
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

static unsigned long Get_Line_Count(Asm_File_Info * const asmbly)
{
    assert(asmbly);

    unsigned long str_num = 0;

    for (int ch = 0; ch < asmbly->symbols_count; ch++)
        if (asmbly->buffer[ch] == '\n')
            str_num++;

    return str_num;
}

//---------------------------------------------------------------------------------------------//

static int Make_Strings(Asm_File_Info * asmbly)
{
    assert(asmbly);

    char ** Tmp_Mem = (char **)calloc (asmbly->lines_count + 1, sizeof(char *));
    
    if (Tmp_Mem == nullptr)
        return Alloc_Err;

    asmbly->pointers = Tmp_Mem;

    *(asmbly->pointers) = asmbly->buffer;

    for (int ch = 0, cur_str = 1; ch < asmbly->symbols_count; ch++)
    {
        if (asmbly->buffer[ch] == '\n')
        {
            asmbly->buffer[ch] = '\0';
            asmbly->pointers[cur_str++] = &asmbly->buffer[ch + 1];
        }
    }
    return No_Error;
}

//---------------------------------------------------------------------------------------------//

static void Skip_Space_To_Digit(char ** str)
{
    assert(str);
    assert(*str);

    while (!(isdigit(**str)))
        (*str)++;
}  

//---------------------------------------------------------------------------------------------//

static void Is_Space_Line(Asm_File_Info * const asmbly, int cur_num)
{
    assert(asmbly);
    assert(cur_num);

    int space_count = 0;
    while(**asmbly->pointers != '\0')
    {
        if (isspace(**asmbly->pointers))
            space_count++;
        (**asmbly->pointers)++;
    }
    if (space_count != strlen(asmbly->pointers[cur_num]))
        asmbly->error = Undefined_Cmd;
    else
        asmbly->error = No_Error;
}

//---------------------------------------------------------------------------------------------//

static void Is_Extra_Arg(Asm_File_Info * const asmbly, int cur_num)
{
    assert(asmbly);
    
    while(**asmbly->pointers != '\0' && !(isspace(**asmbly->pointers)))
        (**asmbly->pointers)++;
    printf("%s", *asmbly->pointers);
    
    while(**asmbly->pointers != '\0')
    {
        if (!(isspace(**asmbly->pointers)) && **asmbly->pointers != '\0')
        {
            asmbly->error = Extra_Arg;
            break;
        }
        (**asmbly->pointers)++;
    }
}