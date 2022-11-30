#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

#include "text_functions.h"

int main(int argc, char ** argv)
{
    Text_Info Onegin = {};
    
    FILE *input_file = NULL, *sortout_file = NULL, *backsortout_file = NULL, *res_file = NULL;

    if (Check_Cmdline_Arg(argc) == Cmdline_Error)
        return Cmdline_Error;

    if ((input_file = fopen(argv[1], "rb")) == NULL)
    {
        ONEGIN_ERROR();
        return File_Is_Missed;
    }

    if ((sortout_file = fopen(argv[2], "wb")) == NULL)
    {
        ONEGIN_ERROR();
        return File_Is_Missed;
    }

    if ((backsortout_file = fopen(argv[3], "wb")) == NULL)
    {
        ONEGIN_ERROR();
        return File_Is_Missed;
    }
    
    if ((res_file = fopen(argv[4], "wb")) == NULL)
    {
        ONEGIN_ERROR();
        return File_Is_Missed;
    }

    if (Onegin_Read(&Onegin, input_file) == Reading_File_Error)
        return Reading_File_Error;

    Onegin_Print_To_File(&Onegin, res_file); ///adding orig text to res file

#if SORT_TYPE == C_QSORT
    Onegin_Qsort(&Onegin, 0, Onegin.n_str - 1, Direct_Lex_Cmp);
#elif SORT_TYPE == MY_QSORT
    qsort(Onegin.Strings, Onegin.n_str, sizeof(Onegin_Line_Info), Direct_Lex_Cmp);
#else
    return -1;
#endif

    Onegin_Print_To_File(&Onegin, sortout_file); /// adding fisrt sorted text to the sortout_file

    Onegin_Print_To_File(&Onegin, res_file); /// adding fisrt sorted text to the res file

#if SORT_TYPE == C_SORT
    Onegin_Qsort(&Onegin, 0, Onegin.n_str - 1, Reverse_Lex_Cmp);
#elif SORT_TYPE == MY_QSORT
    qsort(Onegin.Strings, Onegin.n_str, sizeof(Onegin_Line_Info), Reverse_Lex_Cmp);
#else
    return - 1;
#endif

    Onegin_Print_To_File(&Onegin, backsortout_file); /// adding second sorted text to the backsortout_file

    Onegin_Print_To_File(&Onegin, res_file); /// adding second sorted text to the res file

    Onegin_Dtor(&Onegin);

    if (fclose(input_file) != 0)
        fprintf(stderr, "Error, file %s wasn't closed %s\n", argv[1], strerror(errno));

    if (fclose(sortout_file) != 0)
        fprintf(stderr, "Error, file %s wasn't closed %s\n", argv[2], strerror(errno));

    if (fclose(backsortout_file) != 0)
        fprintf(stderr, "Error, file %s wasn't closed %s\n", argv[3], strerror(errno));
    
    if (fclose(res_file) != 0)
        fprintf(stderr, "Error, file %s wasn't closed %s\n", argv[4], strerror(errno));

    return 0;
}
