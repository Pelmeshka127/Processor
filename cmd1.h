#ifndef CMD1_H_
#define CMD1_H_

DEF_CMD(HLT, CMD_HLT, 0, 
{
    printf("The end is near... It's there\n");
    mode = false;
})

DEF_CMD(PUSH, CMD_PUSH, Digit, 
{
    Stack_Push(My_Stack, cpu->code[ip + 1]);
    ip++;
})

DEF_CMD(PUSH, CMD_PUSH_REG, Reg,
{
    Stack_Push(My_Stack, cpu->registers[cpu->code[ip+1]]);
    ip++;
})

DEF_CMD(POP, CMD_POP, Digit, 
{
    Stack_Pop(My_Stack);
})

DEF_CMD(POP, CMD_POP_REG, Reg, 
{
    cpu->registers[cpu->code[ip+1]] = Stack_Pop(My_Stack);
    ip++;
})

DEF_CMD(ADD, CMD_ADD, 0, 
{
    Stack_Push(My_Stack, Stack_Pop(My_Stack) + Stack_Pop(My_Stack));
})

DEF_CMD(SUB, CMD_SUB, 0, 
{
    Stack_Push(My_Stack, Stack_Pop(My_Stack) - Stack_Pop(My_Stack));
})

DEF_CMD(MUL, CMD_MUL, 0,
{
    Stack_Push(My_Stack, Stack_Pop(My_Stack) * Stack_Pop(My_Stack));
})

DEF_CMD(DIV, CMD_DIV, 0,
{
    int a = Stack_Pop(My_Stack);
    int b = Stack_Pop(My_Stack);
    int c = b / a;
    Stack_Push(My_Stack, c);
})

DEF_CMD(OUT, CMD_OUT, 0,
{
    int value = Stack_Pop(My_Stack);
    printf("The result is %d\n", value);
})

DEF_CMD(JUMP, CMD_JUMP, Label, 
{
    ip = cpu->code[ip + 1];
})  

DEF_CMD(JB, CMD_JB, Label, 
{
    if (Stack_Pop(My_Stack) < Stack_Pop(My_Stack))
        ip = cpu->code[ip + 1];
    else
        ip++;
})

DEF_CMD(JBE, CMD_JBE, Label, 
{
    if (Stack_Pop(My_Stack) <= Stack_Pop(My_Stack))
        ip = cpu->code[ip + 1];
    else 
        ip++;
})

DEF_CMD(JA, CMD_JA, Label, 
{
    if (Stack_Pop(My_Stack) > Stack_Pop(My_Stack))
        ip = cpu->code[ip + 1];
    else 
        ip++;
})

DEF_CMD(JAE, CMD_JAE, Label, 
{
    if (Stack_Pop(My_Stack) >= Stack_Pop(My_Stack))
        ip = cpu->code[ip + 1];
    else 
        ip++;
})

DEF_CMD(JE, CMD_JE, Label,
{
    if (Stack_Pop(My_Stack) == Stack_Pop(My_Stack))
        ip = cpu->code[ip + 1];
    else 
        ip++;
})

DEF_CMD(JNE, CMD_JNE, Label,
{
    if (Stack_Pop(My_Stack) != Stack_Pop(My_Stack))
        ip = cpu->code[ip + 1];
    else 
        ip++;
})

#endif