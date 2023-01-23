#ifndef STACK_H_
#define STACK_H_

#include "stackconfig.h"

#undef CANARY_PROTECT

#undef HASH_PROTECT

enum MODES {
    UP_MODE = 1,
    LOW_MODE = 2,
};

//---------------------------------------------------------------------------------------------//

#ifdef CANARY_PROTECT
    typedef unsigned long canary_t;
    const canary_t Canary_Value = 0xDEADBEEF;
#endif

//---------------------------------------------------------------------------------------------//

#ifdef HASH_PROTECT
    typedef unsigned long hash_t;
#endif

//---------------------------------------------------------------------------------------------//

struct Stack
{
    #ifdef CANARY_PROTECT
        canary_t canary_left = Canary_Value;
    #endif

    size_t capacity = 0;
    size_t size = 0;

    #ifdef CANARY_PROTECT
        canary_t * canary_left_data = nullptr;
    #endif

    elem_t * data = nullptr;

    #ifdef CANARY_PROTECT
        canary_t * canary_right_data = nullptr;
    #endif

    #ifdef HASH_PROTECT
        hash_t stack_hash = 0;
        hash_t data_hash  = 0;
    #endif

    #ifdef CANARY_PROTECT
        canary_t canary_right = Canary_Value;
    #endif

    int error;
};

//---------------------------------------------------------------------------------------------//

/// @brief Function initializes Stack
/// @param my_stack is ptr on struct Stack
/// @param capacity is the value that will be the capacity of Stack
/// @return Alloc_Err if calloc fell down or No_Error if it's OK
int Stack_Ctor(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

/// @brief Function pushes an element into the stack
/// @param my_stack is ptr on struct Stack
/// @param n is an element that will be pushed into the stack
/// @return Alloc_Err if calloc fell down or No_Error if it's OK
int Stack_Push(Stack * const my_stack, const elem_t value);

//---------------------------------------------------------------------------------------------//

/// @brief Function reduces the size of stack
/// @param my_stack is ptr on struct Stack
/// @return Stack_Underflow if size = 0 or the last elem from stack if it's OK
elem_t Stack_Pop(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

/// @brief Free allocated memory
/// @param my_stack is ptr on struct Stack
/// @return 
int Stack_Dtor(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

#endif