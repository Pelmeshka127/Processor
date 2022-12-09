#ifndef HASH_H_
#define HASH_H_

#include "stack.h"

#ifdef HASH_PROTECT

/// @brief 
/// @param src 
/// @param szie 
/// @return 
hash_t Calculate_Hash(const void * const src, size_t size);

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param My_Stack 
/// @return 
int Check_Hash(Stack * const My_Stack, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//

#define CHECK_HASH(My_Stack)   \
Check_Hash(My_Stack, __FILE__, __PRETTY_FUNCTION__, __LINE__); 

#endif

#endif
