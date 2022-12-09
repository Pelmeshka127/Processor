#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "../disasm_includes/disasm.h"
#include "../../proc_config.h"

int main(void)
{
    disasm_info * disasm = {};

    FILE * src_file = nullptr, * dst_file = nullptr;

    if ((src_file = fopen("../Assembler/prog.bin", "rb")) == nullptr)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return Src_File_Err;
    }

    if ((dst_file = fopen("disasm_file.txt", "w")) == nullptr)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return Src_File_Err;
    }

    

    return 0;
}