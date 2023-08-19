#include "asm.hpp"
#include "token.hpp"

int main(int argc, char **argv)
{
    if (CkeckCmdLineArgs(argc) != No_Error)
        return Cmd_Line_Arg_Error;

    FILE *asm_file = fopen(argv[1], "r");
    
    if (asm_file == nullptr)
    {
        fprintf(stderr, "Failed openning %s file\n", argv[1]);
        return File_Error;
    }

    Text_Info onegin = {};

    Onegin_Read(&onegin, asm_file);

    token_s tokens = {};

    TokenCtor(&tokens, &onegin);

    Tokenizer(&tokens, &onegin);

    printf("Successed tokenizer\n");

    for (int i = 0; i < tokens.capacity; i++)
    {
        printf("%s\n", tokens.array[i].name);
    }

    my_asm assembler = {};

    AsmCtor(&assembler, &onegin);    

    FirstAsmCompile(&assembler, &tokens);

    printf("Successed first compilation\n");

    SecondAsmCompile(&assembler, &tokens);

    printf("Second second compilation\n");

#   ifdef LISTING

        Text_Info list_onegin = {};

        fseek(asm_file, 0L, SEEK_SET);

        Onegin_Read(&list_onegin, asm_file);

        ListingCtor();

        printf("Making lsiting\n");

        ListingMake(&assembler, &list_onegin);

        ListingDtor();

#   endif

    if (fclose(asm_file) == EOF)
    {
        fprintf(stderr, "Failed closing %s file\n", argv[1]);
        return File_Error;
    }

    FILE *bin = fopen(argv[2], "w");
    if (bin == nullptr)
    {
        fprintf(stderr, "Failed openning bin file\n");
        return File_Error;
    }

    if (fwrite(&assembler.file_size, sizeof(int), 1, bin) < 1)
    {
        fprintf(stderr, "Failed writing array size to bin file\n");
        return File_Error;
    }

    if (fwrite(assembler.array, sizeof(code_t), (size_t) assembler.file_size, bin) < (size_t) assembler.file_size)
    {
        fprintf(stderr, "Failed writing arrat to bin file\n");
        return File_Error;
    }

    if (fclose(bin) == EOF)
    {
        fprintf(stderr, "Failed closing bin file\n");
        return File_Error;
    }

    AsmDtor(&assembler);

    TokenDtor(&tokens);

    Onegin_Dtor(&list_onegin);

    Onegin_Dtor(&onegin);

    return 0;
}