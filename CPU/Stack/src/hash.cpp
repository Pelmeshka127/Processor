#include <stdio.h>
#include <stdlib.h>

#include "hash.h"
#include "log.h"
#include "debug.h"

#ifdef HASH_PROTECT

hash_t Calculate_Hash(const void * const src, size_t size)
{
    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int hash = seed ^ size;

    const unsigned char * data = (const unsigned char *) src;
    unsigned int k = 0;

    while (size >= 4)
    {
        k  = size;
        k |= size << 8;
        k |= size << 16;
        k |= size << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        hash *= m;
        hash ^= k;

        data += 4;
        size -= 4;
    }

    switch (size)
    {
        case 3:
        hash ^= data[2] << 16;
        case 2:
        hash ^= data[1] << 8;
        case 1:
        hash ^= data[0];
        hash *= m;
    };

    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;

    return hash;
}

//---------------------------------------------------------------------------------------------//

int Check_Hash(Stack * const My_Stack, const char * const File, const char * const Function, const int Line)
{
    hash_t cur_stack_hash = Calculate_Hash(My_Stack, sizeof(My_Stack));

#ifdef CANARY_PROTECT
    hash_t cur_data_hash  = Calculate_Hash(My_Stack->canary_left_data, 2 * sizeof(canary_t) + My_Stack->capacity * sizeof(elem_t));
#else
    hash_t cur_data_hash  = Calculate_Hash(My_Stack->data, sizeof(elem_t));
#endif

    if (My_Stack->data_hash != cur_data_hash)
    {
        fprintf(stderr, "ERROR IN FUNCION %s, IN FILE %s, IN LINE %d.\n THE VALUE OF DATA_HASH WAS CHANGED FROM %zu TO %zu\n\n",
                File, Function, Line, cur_data_hash, My_Stack->data_hash);
    #ifdef USE_LOGS
        Logfile_Hash_Err(My_Stack, cur_data_hash, Data_Hash_Err, File, Function, Line);
    #endif

        return Data_Hash_Err;
    }

    if (My_Stack->stack_hash != cur_stack_hash)
    {
        fprintf(stderr, "ERROR IN FUNCION %s, IN FILE %s, IN LINE %d.\n THE VALUE OF STACK_HASH WAS CHANGED FROM %zu TO %zu\n\n",
                File, Function, Line, cur_stack_hash, My_Stack->stack_hash);
    #ifdef USE_LOGS
        Logfile_Hash_Err(My_Stack, cur_stack_hash, Stack_Hash_Err, File, Function, Line);
    #endif

        return Stack_Hash_Err;
    }

    return No_Err;
}

#endif

//---------------------------------------------------------------------------------------------//
