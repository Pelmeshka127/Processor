#ifndef LOG_H_
#define LOG_H_

#undef USE_LOGS

//---------------------------------------------------------------------------------------------//

/// @brief Opens logfile 
/// @return Logfile_Err if openning failed and No_Err if it's ok
int Logfile_Ctor();

//---------------------------------------------------------------------------------------------//

/// @brief Close logfile
/// @return Logfile_Err if openning failed and No_Err if it's ok
int Logfile_Dtor();

//---------------------------------------------------------------------------------------------//

/// @brief Function prints the info about error to the logfile
/// @param my_stack is ptr on struct Stack
/// @param Err_Code the code of error
/// @param File is ptr on current file
/// @param Function is ptr on current function
/// @param Line is ptr on current line
void Logfile_Stack_Print_Err(Stack * const my_stack, const int Err_Code, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//

#ifdef HASH_PROTECT
/// @brief Function prints the info about hash error to the logfile
/// @param my_stack is ptr on struct Stack
/// @param Cur_Hash_Value the current value of hash
/// @param File is ptr on current file
/// @param Function is ptr on current function
/// @param Line is ptr on curretn line
void Logfile_Hash_Err(Stack * const my_stack, hash_t Cur_Hash_Value, const int Error, const char * const File, const char * const Function, const int Line);
#endif

//---------------------------------------------------------------------------------------------//

/// @brief 
/// @param my_stack 
void Logfile_Stack_Code_Error(Stack * const my_stack);

//---------------------------------------------------------------------------------------------//

/// @brief Function prints the status of the stack to the logfile
/// @param my_stack is ptr on struct Stack
/// @param File is ptr on current file
/// @param Function is ptr on current function
/// @param Line is ptr on current line
void Logfile_Stack_Dump(Stack * const my_stack, const char * const File, const char * const Function, const int Line);

//---------------------------------------------------------------------------------------------//


#endif