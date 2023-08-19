#ifndef ARCH_HPP_
#define ARCH_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define LISTING

#include "config.hpp"

enum error_code {
    No_Error,
    Bad_Alloc,
    File_Error,
    Cmd_Line_Arg_Error,
    Extra_Arg,
    Asm_Failed,
    Incorrect_Arg,
    Incorrect_Label,
    Incorrect_Command,
};

enum arg_type {
    Num_Type,
    Word_Type,
    Bracket_Type,
    Plus_Type,
    Reg_Type,
    Call_Label_Type,
    Start_Label_Type,
};

enum Registers {
    rax,
    rbx,
    rcx,
    rdx,
    rsi,
    rdi,
    rbp,
    rsp,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,
};

enum cmd_code {
    CMD_HLT,

    CMD_OUT,
    CMD_IN,

    CMD_PUSH,
    CMD_POP,

    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_SQRT,
    CMD_MINUS,

    CMD_RET,
    CMD_CALL,

    CMD_JMP,
    CMD_JB,
    CMD_JBE,
    CMD_JA,
    CMD_JAE,
    CMD_JE,
    CMD_JNE,

    CMD_ZERO,
    CMD_BESK,
};

enum Args {
    CMD_MASK   = 0x1F,
    ARG_IMMED  = 1 << 5,
    ARG_REG    = 1 << 6,
    ARG_RAM    = 1 << 7,
};

enum Args_Type {
    Digit_Type,
    Label_Type,
    No_Arg_Type,
};

#endif