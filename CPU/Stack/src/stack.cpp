#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "../includes/stack.h"
#include "../includes/debug.h"
#include "../includes/log.h"

static const elem_t Poison = -1;

//---------------------------------------------------------------------------------------------//

/// @brief Function sets canary after the allocation of memory
/// @param my_stack is ptr on struct stack
/// @param alloc_mem is ptr on allocated memory for stack data 
static void Make_Canary_Protect(Stack * const my_stack, char * alloc_mem);

//---------------------------------------------------------------------------------------------//

/// @brief Function resizes the data of Stack
/// @param my_stack is ptr on struct Stack
/// @return Alloc_Err if resizing failed an No_Err if it's OK
static int Stack_Resize(Stack * const my_stack, const int mode);

//---------------------------------------------------------------------------------------------//

/// @brief Function reallocates the memory for data
/// @param my_stack is ptr on struct Stack
/// @return Alloc_errif reallocation failed an No_Err if it's OK
static char * Stack_Recalloc(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

/// @brief Function sets canaries after the reallocation of memeory
/// @param my_stack is ptr on struct Stack
static void Canary_Recalloc(Stack * const my_stack, const char * const add_alloc_mem);

//---------------------------------------------------------------------------------------------//

/// @brief Function checks is empty Stack
/// @param my_stack is ptr on struct Stack
/// @return If Stack is overflow - 1, else - 0
static int Is_Empty(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

int Stack_Ctor(Stack * const my_stack)
{
    assert(my_stack);

    my_stack->capacity = DEF_CAPACITY;
    my_stack->size = 0;
    my_stack->error = No_Err;

#ifdef CANARY_PROTECT
    size_t Mem_Value = 2 * sizeof(canary_t) + my_stack->capacity * sizeof(elem_t) + 
    (sizeof(canary_t) - (my_stack->capacity % sizeof(canary_t))) * sizeof(elem_t);
#else
    size_t Mem_Value = my_stack->capacity * sizeof(elem_t);
#endif

    char * alloc_mem = (char *)calloc (Mem_Value, sizeof(char));
    if (alloc_mem == nullptr)
    {
        fprintf(stderr, "THE INITIALIZATION OF MEMORY FAILED\n");
        my_stack->error = Alloc_Err;
        return Alloc_Err;
    }

#ifdef CANARY_PROTECT
    Make_Canary_Protect(my_stack, alloc_mem);
#else
    my_stack->data = (elem_t *) (alloc_mem);
#endif

    for (int i = 0; i < (elem_t) my_stack->capacity; i++)
        my_stack->data[i] = Poison;

#ifdef HASH_PROTECT
    Re_Calc_Hash(my_stack);
#endif

    STACK_VERIFY(my_stack);

#ifdef HASH_PROTECT
    CHECK_HASH(my_stack);
#endif

    return No_Err;
}

//---------------------------------------------------------------------------------------------//

int Stack_Push(Stack * const my_stack, const int value)
{
    assert(my_stack);

    if (my_stack->size == my_stack->capacity)
        if (Stack_Resize(my_stack, UP_MODE) == Alloc_Err)
        {
            my_stack->error = Alloc_Err;
            STACK_STOP_PROGRAMM(my_stack, __FILE__, __PRETTY_FUNCTION__, __LINE__);
            return Alloc_Err;
        }
            
    my_stack->data[my_stack->size++] = value; 

    STACK_VERIFY(my_stack);

#ifdef HASH_PROTECT
    Re_Calc_Hash(my_stack);
#endif

    return No_Err;
}

//---------------------------------------------------------------------------------------------//

elem_t Stack_Pop(Stack * const my_stack) 
{
    assert(my_stack);

    if (Is_Empty(my_stack))
    {
        my_stack->error = Underflow;
        STACK_STOP_PROGRAMM(my_stack, __FILE__, __PRETTY_FUNCTION__, __LINE__);
        return Underflow;
    }

    if (MIN_SIZE * my_stack->size <= my_stack->capacity)
        if (Stack_Resize(my_stack, LOW_MODE) == Alloc_Err)
        {
            my_stack->error = Alloc_Err;
            STACK_STOP_PROGRAMM(my_stack, __FILE__, __PRETTY_FUNCTION__, __LINE__);
            return Alloc_Err;
        }

    elem_t Popping_Elem = my_stack->data[my_stack->size - 1];

    my_stack->data[--my_stack->size] = Poison;

    STACK_VERIFY(my_stack);

#ifdef HASH_PROTECT
    Re_Calc_Hash(my_stack);
#endif

    return Popping_Elem;
}

//---------------------------------------------------------------------------------------------//

static int Stack_Resize(Stack * my_stack, int mode)
{
    assert(my_stack);

    if (mode == UP_MODE)
        my_stack->capacity *= MULTIPLIER;
    else if (mode == LOW_MODE)
        my_stack->capacity /= MULTIPLIER;
    else
    {
        fprintf(stderr, "INCORRECT VALUE OF MODE %d. STACK CAN'T BE RESIZED\n", mode);
        return Alloc_Err;
    }

    char * add_alloc_mem = Stack_Recalloc(my_stack);
    if (add_alloc_mem == nullptr)
    {
        Stack_Print_Err(my_stack, my_stack->error = Alloc_Err, __FILE__, __PRETTY_FUNCTION__, __LINE__);
        return Alloc_Err;
    }

#ifdef CANARY_PROTECT
    Canary_Recalloc(my_stack, add_alloc_mem);
#else
    my_stack->data = (elem_t *) (add_alloc_mem);
#endif

    if (mode == UP_MODE)
        for (size_t i = my_stack->capacity / MULTIPLIER; i < my_stack->capacity; i++)
            my_stack->data[i] = Poison;
            
    STACK_VERIFY(my_stack);

#ifdef HASH_PROTECT
    CHECK_HASH(my_stack);
#endif

    return No_Err;
}

//---------------------------------------------------------------------------------------------//

static char * Stack_Recalloc(Stack * const my_stack)
{
    assert(my_stack);

#ifdef CANARY_PROTECT
    size_t Add_Mem_Value = 2 * sizeof(canary_t) + my_stack->capacity * sizeof(elem_t) +
    (sizeof(canary_t) - (my_stack->capacity % sizeof(canary_t))) * sizeof(elem_t);
#else
    size_t Add_Mem_Value = my_stack->capacity * sizeof(elem_t);
#endif

#ifdef CANARY_PROTECT
    char * add_alloc_mem = (char *)realloc (my_stack->canary_left_data, Add_Mem_Value * sizeof(canary_t));
#else
    char * add_alloc_mem = (char *)realloc (my_stack->data, Add_Mem_Value * sizeof(elem_t));
#endif

    if (add_alloc_mem == nullptr)
    {
        Stack_Print_Err(my_stack, Alloc_Err, __FILE__, __PRETTY_FUNCTION__, __LINE__);
#ifdef USE_LOGS
        Logfile_Stack_Print_Err(my_stack, Alloc_Err,  __FILE__, __PRETTY_FUNCTION__, __LINE__);
#endif
        return nullptr;
    }

#ifdef HASH_PROTECT
    Re_Calc_Hash(my_stack);
#endif

    return add_alloc_mem;
}

//---------------------------------------------------------------------------------------------//

#ifdef CANARY_PROTECT
static void Canary_Recalloc(Stack * const my_stack, const char * const add_alloc_mem)
{
    assert(add_alloc_mem);

    my_stack->canary_left_data = (canary_t *) (add_alloc_mem);

    my_stack->data = (elem_t *) (add_alloc_mem + sizeof(canary_t));

    if (my_stack->capacity % sizeof(canary_t) == 0)
        my_stack->canary_right_data = (canary_t *) (add_alloc_mem + my_stack->capacity * sizeof(elem_t) + sizeof(canary_t));
    else
        my_stack->canary_right_data = (canary_t *) (add_alloc_mem + my_stack->capacity * sizeof(elem_t) + sizeof(canary_t) +
        (sizeof(canary_t) - (my_stack->capacity % sizeof(canary_t))) * sizeof(elem_t));

    *(my_stack->canary_left_data)  = Canary_Value;

    *(my_stack->canary_right_data) = Canary_Value;  
}
#endif

//---------------------------------------------------------------------------------------------//

static int Is_Empty(Stack * const my_stack)
{
#ifdef HASH_PROTECT
    CHECK_HASH(my_stack);
#endif
    return my_stack->size == 0;
}

//---------------------------------------------------------------------------------------------//

#ifdef CANARY_PROTECT
static void Make_Canary_Protect(Stack * const my_stack, char * alloc_mem)
{
    my_stack->canary_left_data = (canary_t *) (alloc_mem);

    my_stack->data = (elem_t *) (alloc_mem + sizeof(canary_t));

    if (my_stack->capacity % sizeof(canary_t) == 0)
        my_stack->canary_right_data = (canary_t *) (alloc_mem + sizeof(canary_t) + my_stack->capacity * sizeof(elem_t));
    else
        my_stack->canary_right_data = (canary_t *) (alloc_mem + sizeof(canary_t) + my_stack->capacity * sizeof(elem_t) + 
        (sizeof(canary_t) - (my_stack->capacity % sizeof(canary_t))) * sizeof(elem_t));

    *(my_stack->canary_left_data)  = Canary_Value;

    *(my_stack->canary_right_data) = Canary_Value;
}
#endif

//---------------------------------------------------------------------------------------------//

int Stack_Dtor(Stack * const my_stack)
{
    assert(my_stack);

#ifdef CANARY_PROTECT
    free(my_stack->canary_left_data);

    my_stack->canary_left_data  = nullptr;

    my_stack->canary_right_data = nullptr;
#else
    free(my_stack->data);
#endif
    my_stack->capacity = 0;
    my_stack->size     = 0;
    my_stack->data     = nullptr;

    return No_Err;
}