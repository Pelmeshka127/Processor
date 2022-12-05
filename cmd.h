#ifndef CMD_H_
#define CMD_H_

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

#endif