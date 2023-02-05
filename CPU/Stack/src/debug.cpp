#include <stdio.h>
#include <assert.h>

#include "../includes/debug.h"
#include "../includes/log.h"
#include "../includes/stack.h"

void Stack_Verify(Stack * const my_stack, const char * const File, const char * const Function, const int Line)
{
    assert(my_stack);

#ifdef DUMP_DEBUG
    Stack_Dump(my_stack);
#endif

    if (my_stack->size < 0)
        Stack_Print_Err(my_stack, Underflow, File, Function, Line);

    if (my_stack->size > my_stack->capacity)
        Stack_Print_Err(my_stack, Overflow,  File, Function, Line);

    if (my_stack->data == nullptr)
        Stack_Print_Err(my_stack, Alloc_Err, File, Function, Line);

#ifdef CANARY_PROTECT
    if (my_stack->canary_left  != Canary_Value)
        Stack_Print_Err(my_stack, Canary_Left_Failed,  File, Function, Line);
    
    if (my_stack->canary_right != Canary_Value)
        Stack_Print_Err(my_stack, Canary_Right_Failed, File, Function, Line);
    
    if (*my_stack->canary_left_data  != Canary_Value)
        Stack_Print_Err(my_stack, Canary_Data_Left_Failed,  File, Function, Line);
    
    if (*my_stack->canary_right_data != Canary_Value)
        Stack_Print_Err(my_stack, Canary_Data_Right_Failed, File, Function, Line);
#endif
}

//---------------------------------------------------------------------------------------------//

void Stack_Print_Err(Stack * const my_stack, const int Err_Code, const char * const File, const char * const Function, const int Line)
{
    assert(my_stack);

    if (Err_Code == Underflow)
        fprintf(stderr, "INCORRECT THE VALUE OF SIZE IS: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                my_stack->size, File, Function, Line);

    if (Err_Code == Overflow)
        fprintf(stderr, "THE VALUE OF SIZE %zu IS BIGGER THAN VALUE OF CAPACITY %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n",
                my_stack->size, my_stack->capacity, File, Function, Line);
    
    if (Err_Code == Alloc_Err)
        fprintf(stderr, "MEMEORY ALLOCATION WAS FAILED IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n",
                File, Function, Line);

#ifdef CANARY_PROTECT
    if (Err_Code == Canary_Left_Failed)
        fprintf(stderr, "THE VALUE OF LEFT CANARY IN my_stack WAS CHANGED FROM %zu TO %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, my_stack->canary_left, File, Function, Line);

    if (Err_Code == Canary_Right_Failed) 
        fprintf(stderr, "THE VALUE OF RIGHT CANARY IN my_stack WAS CHANGED FROM %zu TO: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, my_stack->canary_right, File, Function, Line);

    if (Err_Code == Canary_Data_Left_Failed)
        fprintf(stderr, "THE VALUE OF CANARY IN THE START OF DATA WAS CHANGED FROM %zu TO: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, *my_stack->canary_left_data, File, Function, Line);

    if (Err_Code == Canary_Data_Right_Failed)
        fprintf(stderr, "THE VALUE OF CANARY IN THE END OF DATA WAS CHANGED FROM %zu TO: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, *my_stack->canary_right_data, File, Function, Line);
#endif

#ifdef USE_LOGS
    Logfile_Stack_Print_Err(my_stack, Err_Code, File, Function, Line);
#endif
}

//---------------------------------------------------------------------------------------------//

void Stack_Dump(Stack * const my_stack)
{
    assert(my_stack);

#ifdef USE_LOGS
    Logfile_Stack_Dump(my_stack, File, Function, Line);
#else
    printf("The capasity of stack is %zu. The size of stack is %zu\n", my_stack->capacity, my_stack->size);
    
    for (size_t i = 0; i < my_stack->capacity; i++)
        printf("The [%zu] elem of data is %.2f.\n", i, my_stack->data[i]);
#endif
}

//---------------------------------------------------------------------------------------------//

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

void Re_Calc_Hash(Stack * const my_stack)
{
 #ifdef HASH_PROTECT
    my_stack->stack_hash = Calculate_Hash(my_stack, sizeof(my_stack));
    #ifdef CANARY_PROTECT
        my_stack->data_hash = Calculate_Hash(my_stack->canary_left_data, 2 * sizeof(canary_t) + my_stack->capacity * sizeof(elem_t));
    #else
        my_stack->data_hash = Calculate_Hash(my_stack->data, sizeof(elem_t));
    #endif
#endif
}

//---------------------------------------------------------------------------------------------//

int Check_Hash(Stack * const my_stack, const char * const File, const char * const Function, const int Line)
{
    hash_t cur_stack_hash = Calculate_Hash(my_stack, sizeof(my_stack));

#ifdef CANARY_PROTECT
    hash_t cur_data_hash  = Calculate_Hash(my_stack->canary_left_data, 2 * sizeof(canary_t) + my_stack->capacity * sizeof(elem_t));
#else
    hash_t cur_data_hash  = Calculate_Hash(my_stack->data, sizeof(elem_t));
#endif

    if (my_stack->data_hash != cur_data_hash)
    {
        fprintf(stderr, "ERROR IN FUNCION %s, IN FILE %s, IN LINE %d.\n THE VALUE OF DATA_HASH WAS CHANGED FROM %zu TO %zu\n\n",
                File, Function, Line, cur_data_hash, my_stack->data_hash);
    #ifdef USE_LOGS
        Logfile_Hash_Err(my_stack, cur_data_hash, Data_Hash_Err, File, Function, Line);
    #endif
        return Data_Hash_Err;
    }

    if (my_stack->stack_hash != cur_stack_hash)
    {
        fprintf(stderr, "ERROR IN FUNCION %s, IN FILE %s, IN LINE %d.\n THE VALUE OF STACK_HASH WAS CHANGED FROM %zu TO %zu\n\n",
                File, Function, Line, cur_stack_hash, my_stack->stack_hash);
    #ifdef USE_LOGS
        Logfile_Hash_Err(my_stack, cur_stack_hash, Stack_Hash_Err, File, Function, Line);
    #endif

        return Stack_Hash_Err;
    }

    return No_Err;
}
#endif

//---------------------------------------------------------------------------------------------//