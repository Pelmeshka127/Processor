#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

#include "text_functions.h"

/// function moves pointer of the string forward to '\0'
static void Move_Strptr(char ** str);

/// function moves pointer of the string back from '\0'
static void Back_Move_Strptr(char ** str);

/// functions finds out the number of symbols in file
static unsigned long Get_File_Size(FILE *);

/// functions finds out the number of strings in file
static unsigned long Get_Line_Count(struct Text_Info * Onegin);

/// function make strings from input file
static int Make_Strings(struct Text_Info * Onegin);

/*
/// function swaps strings
static void Onegin_Swap(struct Text_Info * Onegin, int i_elem, int j_elem);

/// function swaps strings
static void New_Onegin_Swap(void * first, void * second, size_t size);
*/

int Direct_Lex_Cmp(const void * p1, const void * p2)
{
    char * str1 = * (char **) p1;
    char * str2 = * (char **) p2;

    while (*str1 != '\0' && *str2 != '\0')
    {
        Move_Strptr(&str1);
        Move_Strptr(&str2);
        
        if (*str1 == *str2 && isalpha(*str1) && isalpha(*str2))
        {
            str1++;
            str2++;
        }
        if (*str1 != *str2 && isalpha(*str1) && isalpha(*str2))
        {
            break;
        }
        if (*str1 == '\0' && *str2 == '\0')
        {
            return 0;
        }
    }
    return *str1 - *str2;
}


int Reverse_Lex_Cmp(const void * p1, const void * p2)
{ 
    char * s1 = * (char **) p1;
    char * s2 = * (char **) p2;

    int len1 = strlen(s1);
    int len2 = strlen(s2);

    char * str1 = s1 + len1 - 1;
    char * str2 = s2 + len2 - 1;

    while(len1-- != 0 && len2-- != 0)
    {
        if (len1 ==0 && len2 == 0)
        {
            return 0;
        }

        Back_Move_Strptr(&str1);
        Back_Move_Strptr(&str2);
        
        if (*str1 == *str2 && isalpha(*str1) && isalpha(*str2))
        {
            str1--;
            str2--;
        }
        if (*str1 != *str2 && isalpha(*str1) && isalpha(*str2))
        {
            break;
        }
    }
    return *str2 - *str1;
}


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

/*
void Onegin_Sort(struct Text_Info * Onegin, int Comporator(const void *, const void *))
{
    for (int i = 0; i < Onegin->lines_count - 1; i++)
        for (int j = i + 1; j < Onegin->lines_count; j++)
            if (Comporator(&Onegin->pointers[i], &Onegin->pointers[j]) > 0)
                Onegin_Swap(Onegin, i, j);
}


void Onegin_Qsort(struct Text_Info * Onegin, int first, int last, int Comporator (const void *, const void *))
{   
    if (first < last)
    {
        int left = first, right = last;

        char * middle = Onegin->pointers[(left + right) / 2];

        do
        {
            while (Comporator(&Onegin->pointers[left], &middle) < 0)
                left++;

            while (Comporator(&Onegin->pointers[right], &middle) > 0) 
                right--;

            if (left <= right)
            {
                New_Onegin_Swap(&Onegin->pointers[left], &Onegin->pointers[right], sizeof(char *));
                left++;
                right--;
            }
        } while (left <= right);

        Onegin_Qsort(Onegin, first, right, Comporator);
        Onegin_Qsort(Onegin, left, last, Comporator);
    }
}
*/

void Onegin_Print_To_File(struct Text_Info * Onegin, FILE * fp)
{
    for (int cur_str = 0; cur_str < Onegin->lines_count; cur_str++)
        fprintf(fp, "%s\n", Onegin->pointers[cur_str]);
}


void Onegin_Dtor(struct Text_Info * Onegin)
{
    free(Onegin->pointers);
    free(Onegin->buffer);
}


static void Move_Strptr(char **str)
{
    assert(str);
    assert(*str);

    while (**str != '\0' && !(isalpha(**str)))
        (*str)++;
}


static void Back_Move_Strptr(char ** str)
{
    assert(str);
    assert(*str);

    while (**str != '\0' && !(isalpha(**str)))
        (*str)--;
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

//---------------------------------------------------------------------------------------------//

/*

static void Onegin_Swap(struct Text_Info * Onegin, int i_elem, int j_elem)
{
    char * Temp = Onegin->pointers[i_elem];

    Onegin->pointers[i_elem] = Onegin->pointers[j_elem];

    Onegin->pointers[j_elem] = Temp;
}



static void New_Onegin_Swap(void * first, void * second, size_t size)
{
    char * first_ptr = (char *) first;
    char * second_ptr = (char *) second;

    while (size >= sizeof(uint64_t))
    {
        uint64_t temp = *((uint64_t *) first_ptr);

        *((uint64_t *) first_ptr) = *((uint64_t *) second_ptr);

        *((uint64_t *) second_ptr) = temp;

        first_ptr += sizeof(uint64_t);

        second_ptr += sizeof(uint64_t);

        size -= sizeof(uint64_t);
    }

    while (size >= sizeof(uint32_t))
    {
        uint32_t temp = *((uint32_t *) first_ptr);

        *((uint32_t *) first_ptr) = *((uint32_t *) second_ptr);

        *((uint32_t *) second_ptr) = temp;

        first_ptr += sizeof(uint32_t);

        second_ptr += sizeof(uint32_t);

        size -= sizeof(uint32_t);
    }

    while (size >= sizeof(uint16_t))
    {
        uint16_t temp = *((uint16_t *) first_ptr);

        *((uint16_t *) first_ptr) = *((uint16_t *) second_ptr);

        *((uint16_t *) second_ptr) = temp;

        first_ptr += sizeof(uint16_t);

        second_ptr += sizeof(uint16_t);

        size -= sizeof(uint16_t);
    }

    while (size >= sizeof(uint8_t))
    {
        uint8_t temp = *((uint8_t *) first_ptr);

        *((uint8_t *) first_ptr) = *((uint8_t *) second_ptr);

        *((uint8_t *) second_ptr) = temp;

        first_ptr += sizeof(uint8_t);

        second_ptr += sizeof(uint8_t);

        size -= sizeof(uint8_t);
    }
}
*/