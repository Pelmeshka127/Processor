#ifndef JUMPS_H_
#define JUMPS_H_

DEF_CMD(JUMP, CMD_JUMP, Label, 
{
    ip = cpu->code[ip + 1];
    ip--;
})  

DEF_CMD(JB, CMD_JB, Label, 
{
    if (Stack_Pop(My_Stack) < Stack_Pop(My_Stack))
    {
        ip = cpu->code[ip + 1];
        ip--;
    }
    else
        ip++;
})

DEF_CMD(JBE, CMD_JBE, Label, 
{
    if (Stack_Pop(My_Stack) <= Stack_Pop(My_Stack))
    {
        ip = cpu->code[ip + 1];
        ip--;
    }
    else 
        ip++;
})

DEF_CMD(JA, CMD_JA, Label, 
{
    if (Stack_Pop(My_Stack) > Stack_Pop(My_Stack))
    {
        ip = cpu->code[ip + 1];
        ip--;
    }
    else 
        ip++;
})

DEF_CMD(JAE, CMD_JAE, Label, 
{
    if (Stack_Pop(My_Stack) >= Stack_Pop(My_Stack))
    {
        ip = cpu->code[ip + 1];
        ip--;
    }
    else 
        ip++;
})

DEF_CMD(JE, CMD_JE, Label,
{
    if (Stack_Pop(My_Stack) == Stack_Pop(My_Stack))
    {
        ip = cpu->code[ip + 1];
        ip--;
    }
    else 
        ip++;
})

DEF_CMD(JNE, CMD_JNE, Label,
{
    if (Stack_Pop(My_Stack) != Stack_Pop(My_Stack))
    {
        ip = cpu->code[ip + 1];
        ip--;
    }
    else 
        ip++;
})

#endif