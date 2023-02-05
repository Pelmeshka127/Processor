#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

#include "text_functions.h"

/// functions finds out the number of symbols in file
static unsigned long Get_File_Size(FILE *);

/// functions finds out the number of strings in file
static unsigned long Get_Line_Count(struct Text_Info * Onegin);

/// function make strings from input file
static int Make_Strings(struct Text_Info * Onegin);

int Onegin_Read(struct Text_Info * Onegin, FILE * input_file)
{
    if ((Onegin->symbols_count = Get_File_Size(input_file)) == Reading_File_Error)
        return Reading_File_Error;

    Onegin->buffer = (char *) calloc(Onegin->symbols_count + 1, sizeof(char));
    if (Onegin->buffer == NULL)
        return Alloc_Error;

    if (fread(Onegin->buffer, sizeof(char), Onegin->symbols_count, input_file) < Onegin->lines_count)
        return Reading_File_Error;

    Onegin->lines_count = Get_Line_Count(Onegin) + 1;

    if (Make_Strings(Onegin) == Alloc_Error)
        return Alloc_Error;

    return 0;
}

void Onegin_Print_To_File(struct Text_Info * Onegin, FILE * fp)
{
    for (size_t cur_str = 0; cur_str < Onegin->lines_count; cur_str++)
        fprintf(fp, "%s\n", Onegin->pointers[cur_str]);
}


void Onegin_Dtor(struct Text_Info * Onegin)
{
    free(Onegin->pointers);
    free(Onegin->buffer);
}

static unsigned long Get_File_Size(FILE * fp)
{
    struct stat buf = {};

    if (fstat(fileno(fp), &buf) == -1)
        return Reading_File_Error;

    return (unsigned long) buf.st_size;
}

//---------------------------------------------------------------------------------------------//

static unsigned long Get_Line_Count(Text_Info * const Onegin)
{
    assert(Onegin);

    unsigned long str_num = 0;

    for (unsigned long ch = 0; ch < Onegin->symbols_count; ch++)
        if (Onegin->buffer[ch] == '\n')
            str_num++;
            
    return str_num;
}

//---------------------------------------------------------------------------------------------//

static int Make_Strings(Text_Info * const Onegin)
{
    assert(Onegin);

    char ** Tmp_Mem = (char **)calloc (Onegin->lines_count + 1, sizeof(char *));
    
    if (Tmp_Mem == NULL)
        return Alloc_Error;

    Onegin->pointers = Tmp_Mem;

    *(Onegin->pointers) = Onegin->buffer;

    for (unsigned long ch = 0, cur_str = 1; ch < Onegin->symbols_count; ch++)
    {
        if (Onegin->buffer[ch] == '\n')
        {
            Onegin->buffer[ch] = '\0';
            Onegin->pointers[cur_str++] = &Onegin->buffer[ch + 1];
        }
    }
    return 0;
}
