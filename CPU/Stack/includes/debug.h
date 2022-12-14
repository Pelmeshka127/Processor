#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

#include "stack.h"

#undef DUMP_DEBUG

/// @brief includes the codes of errors
enum Err_Codes {
    No_Err                   = 0,

    Undef_Capacity           = 1,
    
    Alloc_Err                = 2,
    
    Underflow                = 3,
    
    Overflow                 = 4,
    
    Canary_Left_Failed       = 5,
    
    Canary_Right_Failed      = 6,

	Canary_Data_Left_Failed  = 7,

	Canary_Data_Right_Failed = 8,

	Logfile_Error            = 9,

    Stack_Hash_Err           = 10,

    Data_Hash_Err            = 11,
};

//---------------------------------------------------------------------------------------------//

/// @brief Funcrion verifies the errors in stack
/// @param my_stack is ptr on struct Stack
/// @param Err_Code the code of error
/// @param File the pointer on file
/// @param Function the pointer on function
/// @param Line the pointer on line
void Stack_Verify(Stack * const my_stack, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//

/// @brief Function prints the description of error to stderr (or to the logfile if it includes)
/// @param my_stack is ptr on struct Stack
/// @param Err_Code the code of error
/// @param File the pointer on file
/// @param Function the pointer of function
/// @param Line the pointer of line
void Stack_Print_Err(Stack * const my_stack, const int Err_Code, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//

/// @brief Function prints the status of stack
/// @param my_stack is ptr on struct Stack
/// @param File the pointer on file
/// @param Function the pointer of function
/// @param Line the pointer of line
void Stack_Dump(Stack * const my_stack, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//

#define STACK_VERIFY(my_stack) \
Stack_Verify(my_stack, __FILE__, __PRETTY_FUNCTION__, __LINE__);

//---------------------------------------------------------------------------------------------//

#define STACK_STOP_PROGRAMM(my_stack, File, Function, Line)                 \
STACK_VERIFY(my_stack);                                                     \
Stack_Print_Err(my_stack, my_stack->error, File, Function, Line);           \
Stack_Dtor(my_stack);

//---------------------------------------------------------------------------------------------//

#ifdef HASH_PROTECT

/// @brief Function calculates the hash of stack and the hash of stack data
/// @param src is ptr on the struct to calc its hash
/// @param size the size of the src
/// @return the value oh the hash
hash_t Calculate_Hash(const void * const src, size_t size);

//---------------------------------------------------------------------------------------------//

/// @brief Function recalcultes the hash of the stack
/// @param my_stack is ptr on struct Stack
void Re_Calc_Hash(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

/// @brief Function checks changes in the hash
/// @param my_stack is ptr struct Stack
/// @return Hash_Err if hash was changed, No_Err if it's ok
int Check_Hash(Stack * const my_stack, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//

#define CHECK_HASH(my_stack)   \
Check_Hash(my_stack, __FILE__, __PRETTY_FUNCTION__, __LINE__); 

#endif

#endif