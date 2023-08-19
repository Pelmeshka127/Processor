#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "../Architecture/arch.hpp"
#include "Parsering/parser.h"

struct token_info {
    type_t type;
    data_t data;
    char  *name;
};

struct token_s {
    token_info *array;
    int capacity;
    int size;
};

int TokenCtor(token_s *const tokens, Text_Info *const onegin);

int Tokenizer(token_s *const tokens, Text_Info *const onegin);

int TokenCreateNumber(token_s *const tokens, Text_Info *const onegin);

int TokenCreateBracket(token_s *const tokens, Text_Info *const onegin);

int TokenCreatePlus(token_s *const tokens, Text_Info *const onegin);

int TokenCreateWord(token_s *const tokens, Text_Info *const onegin, const int length, const int number);

int TokenCreateRegister(token_s *const tokens, Text_Info *const onegin, const int length, const int number);

int TokenCreateLabel(token_s *const tokens, Text_Info *const onegin);

int TokenAllocMem(token_s *const tokens, const size_t size);

int TokenDtor(token_s *const tokens);

int Strincmp(const char * str1, const char * str2, int const len);

#endif