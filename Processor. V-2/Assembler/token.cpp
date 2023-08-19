#include <string.h>
#include <ctype.h>

#include "token.hpp"

//===================================================================================//
 
static void     SkipComments(Text_Info *const onegin);

static void     SkipSpaces(Text_Info *const onegin);

static void     MovePointers(Text_Info *const onegin);

static bool     IsDigit(Text_Info *const onegin);

static bool     IsMinus(Text_Info *const onegin);

static bool     IsBracket(Text_Info *const onegin);

static bool     IsPlus(Text_Info *const onegin);

//===================================================================================//

int TokenCtor(token_s *const tokens, Text_Info *const onegin)
{
    tokens->array = (token_info *)calloc (size_t(onegin->symbols_count + 1), sizeof(token_info));

    if (tokens->array == nullptr)
    {
        fprintf(stderr, "Tokens allocation error in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    return No_Error;
}

//===================================================================================//

int Tokenizer(token_s *const tokens, Text_Info *const onegin)
{
    assert(tokens);
    assert(onegin);

    for (int i = 0; i < onegin->symbols_count; i++)
    {
        if (onegin->buffer[i] == '\0')
            onegin->buffer[i] = '\n';
    }

    while (true)
    {
        // printf("%s\n", *onegin->pointers);
        if (**onegin->pointers == '\0')
            break;

        if (**onegin->pointers == ';')
            SkipComments(onegin);

        else if (IsDigit(onegin))
            TokenCreateNumber(tokens, onegin);

        else if (IsBracket(onegin))
            TokenCreateBracket(tokens, onegin);

        else if (IsPlus(onegin))
            TokenCreatePlus(tokens, onegin);

        else 
    
#       define  DEF_CMD(code_word, number, length, arg_type, code)                  \
                if (Strincmp(#code_word, *onegin->pointers, length) == 0)           \
                    TokenCreateWord(tokens, onegin, length, number);                \

#       include     "../Architecture/cmd.hpp"

#       undef   DEF_CMD

        else 

#       define  DEF_REG(code_word, number, length)                                  \
                if (Strincmp(#code_word, *onegin->pointers, length) == 0)           \
                    TokenCreateRegister(tokens, onegin, length, number);            \

#       include     "../Architecture/regs.hpp" 

#       undef   DEF_REG

        if (isalpha(**onegin->pointers))
        {
            TokenCreateLabel(tokens, onegin);
        }

        SkipSpaces(onegin);
    }

    return No_Error;
}

//===================================================================================//

int TokenDtor(token_s *const tokens)
{
    for (int i = 0; i < tokens->capacity; i++)
        free(tokens->array[i].name);

    free(tokens->array);
    tokens->size        = 0;
    tokens->capacity    = 0;

    return No_Error;
}

//===================================================================================//

int TokenCreateNumber(token_s *const tokens, Text_Info *const onegin)
{
    if (TokenAllocMem(tokens, Max_Size) == Bad_Alloc)
    {
        fprintf(stderr, "Allocation memory for digit failed in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    tokens->array[tokens->capacity].type = Num_Type;

    char *word = tokens->array[tokens->capacity].name;

    if (IsMinus(onegin))
    {
        *word++ = **onegin->pointers;
        MovePointers(onegin);
    }    

    while (isdigit(**onegin->pointers))
    {
        *word++ = **onegin->pointers;
        MovePointers(onegin);
    }

    tokens->capacity++;

    return No_Error;
}

//===================================================================================//

int TokenCreateBracket(token_s *const tokens, Text_Info *const onegin)
{
    if (TokenAllocMem(tokens, Min_Size) == Bad_Alloc)
    {
        fprintf(stderr, "Allocation memory for bracket failed in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    tokens->array[tokens->capacity].type = Bracket_Type;

    strncpy(tokens->array[tokens->capacity].name, *onegin->pointers, 1);

    MovePointers(onegin);

    tokens->capacity++;

    return No_Error;
}

//===================================================================================//

int TokenCreatePlus(token_s *const tokens, Text_Info *const onegin)
{
    if (TokenAllocMem(tokens, Min_Size) == Bad_Alloc)
    {
        fprintf(stderr, "Allocation memory for plus failed in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    tokens->array[tokens->capacity].type = Plus_Type;

    strncpy(tokens->array[tokens->capacity].name, *onegin->pointers, 1);

    MovePointers(onegin);

    tokens->capacity++;

    return No_Error;
}

//===================================================================================//

int TokenCreateWord(token_s *const tokens, Text_Info *const onegin, const int length, const int number)
{
    if (TokenAllocMem(tokens, (size_t) (length + 1)) == Bad_Alloc)
    {
        fprintf(stderr, "Allocation memory for word failed in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    tokens->array[tokens->capacity].type = Word_Type;

    tokens->array[tokens->capacity].data = number;

    strncpy(tokens->array[tokens->capacity].name, *onegin->pointers, (size_t) length);

    (*onegin->pointers) += length;

    tokens->capacity++;

    return No_Error;
}

//===================================================================================//

int TokenCreateRegister(token_s *const tokens, Text_Info *const onegin, const int length, const int number)
{
    if (TokenAllocMem(tokens, (size_t) (length + 1)) == Bad_Alloc)
    {
        fprintf(stderr, "Allocation memory for register failed in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    tokens->array[tokens->capacity].type = Reg_Type;

    tokens->array[tokens->capacity].data = number;

    strncpy(tokens->array[tokens->capacity].name, *onegin->pointers, (size_t) length);

    (*onegin->pointers) += length;

    tokens->capacity++;

    return No_Error;
}

//===================================================================================//

int TokenCreateLabel(token_s *const tokens, Text_Info *const onegin)
{
    if (TokenAllocMem(tokens, Max_Label_Word_Size) == Bad_Alloc)
    {
        fprintf(stderr, "Allocation memory for rlabel failed in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    tokens->array[tokens->capacity].type = Call_Label_Type;

    char *word = tokens->array[tokens->capacity].name;

    while (isalpha(**onegin->pointers))
    {
        *word++ = **onegin->pointers;
        MovePointers(onegin);

        if (**onegin->pointers == ':')
        {
            tokens->array[tokens->capacity].type = Start_Label_Type;
            *word++ = **onegin->pointers;
            MovePointers(onegin);
        }
    }

    tokens->capacity++;

    return No_Error;
}

//===================================================================================//

int TokenAllocMem(token_s *const tokens, const size_t size)
{
    tokens->array[tokens->capacity].name = (char *)calloc (size, sizeof(char));
    if (tokens->array[tokens->capacity].name == nullptr)
    {
        fprintf(stderr, "Failed allocation memory in %s\n", __PRETTY_FUNCTION__);
        return Bad_Alloc;
    }

    return No_Error;
}

//===================================================================================//

static void SkipComments(Text_Info *const onegin)
{
    while (**onegin->pointers != '\n')
    {
        MovePointers(onegin);
        if (**onegin->pointers == '\0')
            break;
    }
}

//===================================================================================//

static void SkipSpaces(Text_Info *const onegin)
{
    while (isspace(**onegin->pointers))
        MovePointers(onegin);
}

//===================================================================================//

static void MovePointers(Text_Info *const onegin)
{
    (*onegin->pointers)++;
}

//===================================================================================//

static bool IsDigit(Text_Info *const onegin)
{
    if (isdigit(**onegin->pointers) || (IsMinus(onegin) && isdigit(*(*onegin->pointers + 1)) ))
        return true;
    
    return false;
}

//===================================================================================//

static bool IsMinus(Text_Info *const onegin)
{
    return (**onegin->pointers == '-');
}


//===================================================================================//

static bool IsBracket(Text_Info *const onegin)
{
    return (**onegin->pointers == '[' || **onegin->pointers == ']');
}


//===================================================================================//

static bool IsPlus(Text_Info *const onegin)
{
    return (**onegin->pointers == '+');
}

//===================================================================================//

int Strincmp(const char * str1, const char * str2, int const len)
{
    assert(str1);
    assert(str2);

    int i = 1;

    while(tolower(*str1) == tolower(*str2))
    {
        if (i == len)
            return 0;

        if (*str1 == '\0')
            return 0;

        str1++;
        str2++;
        i++;
    }

    return (int) (str1 - str2);
}

//===================================================================================//