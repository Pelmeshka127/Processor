#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "../includes/stack.h"
#include "../includes/debug.h"
#include "../includes/log.h"

int main(void)
{
	Stack my_stack = {};

#ifdef USE_LOGS
	if (Logfile_Ctor() == Logfile_Error)
        return Logfile_Error;
#endif

    if (Stack_Ctor(&my_stack) != No_Err)
	{
		fprintf(stderr, "FAILED THE INITIALIZATION OF STACK\n");
		return Alloc_Err;
	}

	Stack_Pop(&my_stack);
	if (my_stack.error != No_Err)
		return Alloc_Err;


#ifdef USE_LOGS
	Logfile_Dtor();
#endif

    Stack_Dtor(&my_stack);

	return 0;
}