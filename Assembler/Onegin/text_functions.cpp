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
static int Get_File_Size(FILE *);

/// functions finds out the number of strings in file
static int Get_Num_Str(FILE *, struct Text_Info * Onegin);

/// function make strings from input file
static int Make_Strings(struct Text_Info * Onegin);

/// function swaps strings
static void Onegin_Swap(struct Text_Info * Onegin, int i_elem, int j_elem);

/// function swaps strings
static void New_Onegin_Swap(void * first, void * second, size_t size)


int Direct_Lex_Cmp(const void * p1, const void * p2)
{
    Onegin_Line_Info * Onegin_Struct_1 = (Onegin_Line_Info *) p1;
    Onegin_Line_Info * Onehin_Struct_2 = (Onegin_Line_Info *) p2;

    char * str1 = Onegin_Struct_1->string;
    char * str2 = Onehin_Struct_2->string;

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
    Onegin_Line_Info * Onegin_Struct_1 = (Onegin_Line_Info *) p1;
    Onegin_Line_Info * Onegin_Struct_2 = (Onegin_Line_Info *) p2;

    int len1 = Onegin_Struct_1->len_str;
    int len2 = Onegin_Struct_2->len_str;

    char * str1 = Onegin_Struct_1->string + len1 - 1;
    char * str2 = Onegin_Struct_2->string + len2 - 1;

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


int Check_Cmdline_Arg(int argc)
{
    if (argc != 5)
    {
        ONEGIN_ERROR();
        fprintf(stderr, "Error: wrong number of command line args: %d\n", argc);
        return Cmdline_Error;
    }
    return 0;
}


int Onegin_Read(struct Text_Info * Onegin, FILE * input_file) 
{
    if ((Onegin->n_symb = Get_File_Size(input_file)) == Reading_File_Error) 
        return Reading_File_Error;

    if ((Onegin->text_string = (char *)calloc (Onegin->n_symb + 1, sizeof(char))) == NULL)
    {
        ONEGIN_ERROR();
        return Reading_File_Error;
    }

    if (fread(Onegin->text_string, sizeof(char), Onegin->n_symb + 1, input_file) < Onegin->n_str)
    {
        ONEGIN_ERROR();
        return Reading_File_Error;
    }

    Get_Num_Str(input_file, Onegin);

    if (Make_Strings(Onegin) == Alloc_Error)
        return Reading_File_Error;

    if (Onegin->n_str == 0 || Onegin->n_symb == 0)
    {
        fprintf(stderr, "Error: number of srings Onegin->Strings is %lu\n"
                        "number of symols in Onegin->Strings is %lu\n", Onegin->n_str, Onegin->n_symb);
        return Reading_File_Error;
    }

    if (Onegin->Strings == NULL)
        return Reading_File_Error;

    return 0;
}


void Onegin_Sort(struct Text_Info * Onegin, int Comporator(const void *, const void *))
{
    for (int i = 0; i < Onegin->n_str - 1; i++)
        for (int j = i + 1; j < Onegin->n_str; j++)
            if (Comporator(&Onegin->Strings[i], &Onegin->Strings[j]) > 0)
                Onegin_Swap(Onegin, i, j);
}


void Onegin_Qsort(struct Text_Info * Onegin, int first, int last, int Comporator (const void *, const void *))
{   
    if (first < last)
    {
        int left = first, right = last;

        Onegin_Line_Info middle = Onegin->Strings[(left + right) / 2];

        do
        {
            while (Comporator(&Onegin->Strings[left], &middle) < 0)
                left++;

            while (Comporator(&Onegin->Strings[right], &middle) > 0) 
                right--;

            if (left <= right)
            {
                New_Onegin_Swap(&Onegin->Strings[left], &Onegin->Strings[right], sizeof(Onegin_Line_Info));
                left++;
                right--;
            }
        } while (left <= right);

        Onegin_Qsort(Onegin, first, right, Comporator);
        Onegin_Qsort(Onegin, left, last, Comporator);
    }
}


void Onegin_Print_To_File(struct Text_Info * Onegin, FILE * fp)
{
    for (int cur_str = 0; cur_str < Onegin->n_str; cur_str++)
        fprintf(fp, "%s", Onegin->Strings[cur_str].string);
}


void Onegin_Dtor(struct Text_Info * Onegin)
{
    free(Onegin->Strings);
    free(Onegin->text_string);
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


static int Get_File_Size(FILE * fp)
{
    struct stat buf = {};

    if (fstat(fileno(fp), &buf) == -1)
    {
        ONEGIN_ERROR();
        return Reading_File_Error;
    }

    return buf.st_size;
}


static int Get_Num_Str(FILE * fp, struct Text_Info * Onegin)
{
    int cur_ch = 0, str_num = 0;
    while (cur_ch < Onegin->n_symb)
    {
        if (Onegin->text_string[cur_ch++] == '\n')
        {
            Onegin->n_str++;
        }
    }
    return Onegin->n_str;
}


static int Make_Strings(struct Text_Info * Onegin)
{
    if ((Onegin->Strings = (Onegin_Line_Info *)calloc (Onegin->n_str+1, sizeof(Onegin_Line_Info))) == NULL)
    {
        ONEGIN_ERROR();
        return Alloc_Error;
    }
    
    int ch = 0, cur_len = 0, num_str = 0;
    while (num_str < Onegin->n_str)
    {
        cur_len++;
        ch++;
        if (Onegin->text_string[ch] == '\n')
        {
            Onegin->text_string[ch] = '\0';

            Onegin->Strings[num_str].len_str = cur_len;
            Onegin->Strings[num_str].string = &Onegin->text_string[ch - cur_len];

            num_str++;
            cur_len = 0;
            ch++;
        }
    }
    return 0;
}


static void Onegin_Swap(struct Text_Info * Onegin, int i_elem, int j_elem)
{
    Onegin_Line_Info Temp = {};

    Temp = Onegin->Strings[i_elem];

    Onegin->Strings[i_elem] = Onegin->Strings[j_elem];

    Onegin->Strings[j_elem] = Temp;
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
