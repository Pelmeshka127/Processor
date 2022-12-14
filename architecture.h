#ifndef ARCHITECTURE_H_
#define ARCHITECTURE_H_

enum CMD {
    CMD_PUSH     = 1,
    CMD_POP      = 2,
    CMD_ADD      = 3,
    CMD_SUB      = 4,
    CMD_MUL      = 5,
    CMD_DIV      = 6,
    CMD_OUT      = 9,

    CMD_PUSH_REG = 11,
    CMD_POP_REG  = 12,

    CMD_JUMP     = 21,
    CMD_JB       = 22,
    CMD_JBE      = 23,
    CMD_JA       = 24,
    CMD_JAE      = 25,
    CMD_JE       = 26,
    CMD_JNE      = 27,

    CMD_CALL     = 31,
    CMD_RET      = 32,

    CMD_HLT      = 0,
};

enum Regs {
    REG_RAX = 1,
    REG_RBX = 2,
    REG_RCX = 3,
    REG_RDX = 4,
};

enum Erros {
    No_Error           = 1 << 10,
    Cmd_Line_Arg_Err   = 1 << 11,
    Src_File_Err       = 1 << 12,
    Reading_File_Err   = 1 << 13,
    Alloc_Err          = 1 << 14,
    Undefined_Cmd      = 1 << 15,
    Undefined_Arg      = 1 << 16,
    Extra_Arg          = 1 << 17,
    Incorrext_Cmd_Arg  = 1 << 18,
    Asm_Compile_Error  = 1 << 19,
    CP_Error           = 1 << 20,
    CPU_Compile_Error  = 1 << 21,
};

enum Args {
    Digit = 1,
    Reg   = 2,
    Label = 3,
};

#endif