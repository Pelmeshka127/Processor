#ifndef TEXT_FUNCTIONS_H_
#define TEXT_FUNCTIONS_H_

struct Text_Info {
    char * buffer;
    unsigned long symbols_count;
    unsigned long lines_count;
    char ** pointers;
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

/// function reads input file
int Onegin_Read(struct Text_Info * Onegin, FILE * fp);

/// function writes strings to the out file 
void Onegin_Print_To_File(struct Text_Info * Onegin, FILE * fp);

/// function closes files and clear dedicated memory
void Onegin_Dtor(struct Text_Info * Onegin);

#endif
