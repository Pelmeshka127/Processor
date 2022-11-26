#ifndef PROC_CONFIG_H_
#define PROC_CONFIG_H_

enum CMD {
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_ADD  = 3,
    CMD_SUB  = 4,
    CMD_MUL  = 5,
    CMD_DIV  = 6,
    CMD_JUMP = 7,
    CMD_DUP  = 8,
    CMD_OUT  = 9,
    CMD_HLT  = 0,
};

enum Erros {
    No_Error           = 1 << 10,
    CP_Error           = 1 << 11,
    Src_File_Err       = 1 << 12,
    Interpret_File_Err = 1 << 13,
    Reading_File_Err   = 1 << 14,
    Alloc_Err          = 1 << 15,
    Cmd_Line_Arg_Err   = 1 << 16,
    Input_File_Err     = 1 << 17,
    Undefined_Cmd      = 1 << 18,
    Extra_Arg          = 1 << 19,
    Logfile_Error      = 1 << 20,
    Incorrext_Cmd_Arg = 1 << 21,
    Asm_Compile_Error  = 1 << 22,
};

const int DEF_CMD_LEN = 20;

const int DEF_CP = 0xABCD;

#endif