#include <stdio.h>
#include <assert.h>

#include "../includes/debug.h"
#include "../includes/log.h"
#include "../includes/stack.h"

static FILE * logfile = nullptr;

static const char * const LOGFILE_NAME = "logfile.txt";

//---------------------------------------------------------------------------------------------//

int Logfile_Ctor()
{
    logfile = fopen(LOGFILE_NAME, "a");

    if (logfile == nullptr)
        return Logfile_Error;

    return No_Err;
}

//---------------------------------------------------------------------------------------------//

int Logfile_Dtor()
{
    if (fclose(logfile) == 0)
        return Logfile_Error;

    return No_Err;
}

//---------------------------------------------------------------------------------------------//

void Logfile_Stack_Print_Err(Stack * const my_stack, const int Err_Code, const char * const File, const char * const Function, const int Line)
{
    assert(my_stack);
    assert(logfile);

    if (Err_Code == Underflow)
        fprintf(logfile, "THE VALUE OF SIZE : %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                my_stack->size, File, Function, Line);

    if (Err_Code == Overflow)
        fprintf(logfile, "THE VALUE OF SIZE %zu IS BIGGER THAN VALUE OF CAPACITY %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n",
                my_stack->size, my_stack->capacity, File, Function, Line);
    
    if (Err_Code == Alloc_Err)
        fprintf(logfile,  "MEMEORY ALLOCATION WAS FAILED IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n",
                File, Function, Line);

#ifdef CANARY_PROTECT
    if (Err_Code == Canary_Left_Failed) 
        fprintf(logfile, "THE VALUE OF LEFT CANARY IN my_stack WAS CHANGED FROM %zu TO %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, my_stack->canary_left, File, Function, Line);

    if (Err_Code == Canary_Right_Failed) 
        fprintf(logfile, "THE VALUE OF RIGHT CANARY IN my_stack WAS CHANGED FROM %zu TO: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, my_stack->canary_right, File, Function, Line);

    if (Err_Code == Canary_Data_Left_Failed)
        fprintf(logfile, "THE VALUE OF CANARY IN THE START OF DATA WAS CHANGED FROM %zu TO: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, *my_stack->canary_left_data, File, Function, Line);

    if (Err_Code == Canary_Data_Right_Failed)
        fprintf(logfile, "THE VALUE OF CANARY IN THE END OF DATA WAS CHANGED FROM %zu TO: %zu IN FILE %s; IN FUNCTION %s; IN LINE %d\n\n", 
                Canary_Value, *my_stack->canary_right_data, File, Function, Line);
#endif
}

//---------------------------------------------------------------------------------------------//

void Logfile_Hash_Err(Stack * const my_stack, hash_t Cur_Hash_Value, const int Error, const char * const File, const char * const Function, const int Line)
{
    if (Error == Stack_Hash_Err)
        fprintf(logfile, "ERROR IN FUNCION %s, IN FILE %s, IN LINE %d.\nTHE VALUE OF STACK_HASH WAS CHANGED FROM %zu TO %zu\n\n",
                        Function, File, Line, Cur_Hash_Value, my_stack->stack_hash);
    
    if (Error == Data_Hash_Err)
        fprintf(logfile, "ERROR IN FUNCION %s, IN FILE %s, IN LINE %d.\nTHE VALUE OF DATA_HASH WAS CHANGED FROM %zu TO %zu\n\n",
                        Function, File, Line, Cur_Hash_Value, my_stack->data_hash);
}

//---------------------------------------------------------------------------------------------//

void Logfile_Stack_Code_Error(Stack * const my_stack)
{
    assert(my_stack);

    switch (my_stack->error)
    {
        case No_Err:
        fprintf(logfile, "The value of stack->error is No_Err\n\n");
        break;

        case Undef_Capacity:
        fprintf(logfile, "The value od stack->error is Undef_Capacity\n\n");
        break;

        case Alloc_Err:
        fprintf(logfile, "The value od stack->error is Alloc_Err\n\n");
        break;

        case Underflow:
        fprintf(logfile, "The value od stack->error is Underflow\n\n");
        break;

        case Overflow:
        fprintf(logfile, "The value od stack->error is Overflow\n\n");
        break;

        default:
        fprintf(logfile, "Undefines value of stack->error %d\n\n", my_stack->error);
        break;
    }
}

//---------------------------------------------------------------------------------------------//

void Logfile_Stack_Dump(Stack * const my_stack, const char * const File, const char * const Function, const int Line)
{
    assert(my_stack->data);
    assert(File);
    assert(Function);
    assert(Line);
    assert(my_stack);
    assert(logfile);

    fprintf(logfile, "//---------------------------------------------------------------------------------------------//\n\n");
                                                                                                                                    
    fprintf(logfile, "Stack_Dump was called at %s\nAt %s\nIn file %s\nIn function %s\nIn line %d\n\n", __TIME__, __DATE__, File, Function, Line);  
                                                                                                                                    
    fprintf(logfile, "Stack has an adress %p. \n\n", my_stack);                        

#ifdef CANARY_PROTECT                                                                                                            
    fprintf(logfile, "The value of left Canary is %zu. It's adress is %p. \n\n",           
            my_stack->canary_left, &my_stack->canary_left);                                                   
#endif

    fprintf(logfile, "The capacity of stack is %zu\nThe size of stack is %zu\n\n",                                            
            my_stack->capacity, my_stack->size);                                                                                      

#ifdef CANARY_PROTECT                                                                                                                  
    fprintf(logfile, "The value of canary_left_data is %zu. It's adress is %p. \n\n",      
            *my_stack->canary_left_data, my_stack->canary_left_data);                                     
#endif
                                                                                                             
    for (int i = 0; i < (elem_t) my_stack->capacity; i++)                                                                     
    {                                                                                                                         
        fprintf(logfile, "The [%d] elem of data is %d. It's adress is %p. \n",             
                i, my_stack->data[i], &my_stack->data[i]);                                                        
    }                                                                                                                         
                                                                                                                        
    fprintf(logfile, "\n");       

    Logfile_Stack_Code_Error(my_stack);                                                                                          

#ifdef CANARY_PROTECT                                                                                                                      
    fprintf(logfile, "The value of canary_right_data is %zu. It's adress is %p. \n\n",     
            *(my_stack->canary_right_data), my_stack->canary_right_data);                                
                                                                                                                        
    fprintf(logfile, "The value of right Canary is %zu. It's adress is %p. \n\n",          
            my_stack->canary_right, &my_stack->canary_right_data);     
#endif

#ifdef HASH_PROTECT
    fprintf(logfile, "The value of stack_hash is %zu. It's adress is %p.\n\n",
            my_stack->stack_hash, &my_stack->stack_hash);
    
    fprintf(logfile, "The value of data_hash is %zu. It's adress is %p.\n\n",
            my_stack->data_hash, &my_stack->data_hash);
#endif
}

//---------------------------------------------------------------------------------------------//