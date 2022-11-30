#ifndef TEXT_FUNCTIONS_H_
#define TEXT_FUNCTIONS_H_

#define C_QSORT 1

#define MY_QSORT 2

#define SORT_TYPE 1

struct Onegin_Line_Info {
    char * string;
    int len_str;
};

struct Text_Info {
    char * text_string;
    unsigned long n_str, n_symb;
    Onegin_Line_Info * Strings;
};

#define ONEGIN_ERROR() \
fprintf(stderr, "There is an error in %s, in %d: %s\n", \
        __PRETTY_FUNCTION__, __LINE__, strerror(errno))

enum Onegin_Coderr {
    Cmdline_Error = 1,
    File_Is_Missed = 2,
    Reading_File_Error = 3,
    Making_Str_Error = 4,
    Alloc_Error = 5,
};

/// function compares two strings
int Direct_Lex_Cmp(const void * s1, const void * s2);

/// functions compares two strings in reverse alphabetical order
int Reverse_Lex_Cmp(const void * p1, const void * p2);

/// function checks command lines argument
int Check_Cmdline_Arg(int argc);

/// function reads input file
int Onegin_Read(struct Text_Info * Onegin, FILE *);

/// functions sorts file using string comporator
void Onegin_Sort(struct Text_Info * Onegin, int Comp(const void *, const void *));

/// functions sorts file using quick sort with string comporator
void Onegin_Qsort(struct Text_Info * Onegin, int first, int last, int Comp (const void *, const void *));

/// function writes strings to the out file 
void Onegin_Print_To_File(struct Text_Info * Onegin, FILE * fp);

/// function closes files and clear dedicated memory
void Onegin_Dtor(struct Text_Info * Onegin);

#endif
