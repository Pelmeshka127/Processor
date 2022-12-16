#include "dsl.h"

DEF_CMD(HLT, CMD_HLT, 0, 
{
    printf("The end\n");
    mode = false;
})

DEF_CMD(PUSH, CMD_PUSH, Digit, 
{
    int elem = Get_Push_Arg(cpu, cpu->code[cpu->ip]);
    PUSH_STACK(elem);
})

DEF_CMD(PUSH, CMD_PUSH_REG, Reg,
{
    PUSH_STACK(cpu->registers[cpu->code[cpu->ip+1]]);
    cpu->ip++;
})

DEF_CMD(POP, CMD_POP, Digit, 
{
    int elem = POP_STACK;
    Get_Pop_Arg(cpu, cpu->code[cpu->ip], elem);
})

DEF_CMD(POP, CMD_POP_REG, Reg, 
{
    cpu->registers[cpu->code[cpu->ip+1]] = POP_STACK;
    cpu->ip++;
})

DEF_CMD(ADD, CMD_ADD, 0, 
{
    PUSH_STACK(POP_STACK + POP_STACK);
})

DEF_CMD(SUB, CMD_SUB, 0, 
{
    PUSH_STACK(POP_STACK - POP_STACK);
})

DEF_CMD(MUL, CMD_MUL, 0,
{
    PUSH_STACK(POP_STACK * POP_STACK);
})

DEF_CMD(DIV, CMD_DIV, 0,
{
    int a = POP_STACK;
    int b = POP_STACK; // div 0
    int c = b / a;
    PUSH_STACK(c);
})

DEF_CMD(SQRT, CMD_SQRT, 0,
{
    PUSH_STACK(sqrt(POP_STACK));
})

DEF_CMD(MINUS, CMD_MINUS, 0,
{
    PUSH_STACK(-1 * POP_STACK);
})

DEF_CMD(IN, CMD_IN, 0,
{
    int value = 0;
    scanf("%d", &value);
    PUSH_STACK(value);
})

DEF_CMD(OUT, CMD_OUT, 0,
{
    int value = POP_STACK;
    printf("The result is %d\n", value);
})

DEF_CMD(CALL, CMD_CALL, Label,
{
    PUSH_RET_STACK(cpu->ip + 1);
    cpu->ip = cpu->code[cpu->ip + 1];
    cpu->ip--;
})

DEF_CMD(RET, CMD_RET, 0,
{
    cpu->ip = POP_RET_STACK;
})

DEF_CMD(JUMP, CMD_JUMP, Label, 
{
    cpu->ip = cpu->code[cpu->ip + 1];
    cpu->ip--;
})  

DEF_CMD(JB, CMD_JB, Label, 
{
    if (POP_STACK < POP_STACK)
    {
        cpu->ip = cpu->code[cpu->ip + 1];
        cpu->ip--;
    }
    else
        cpu->ip++;
})

DEF_CMD(JBE, CMD_JBE, Label, 
{
    if (POP_STACK <= POP_STACK)
    {
        cpu->ip = cpu->code[cpu->ip + 1];
        cpu->ip--;
    }
    else 
        cpu->ip++;
})

DEF_CMD(JA, CMD_JA, Label, 
{
    if (POP_STACK > POP_STACK)
    {
        cpu->ip = cpu->code[cpu->ip + 1];
        cpu->ip--;
    }
    else 
        cpu->ip++;
})

DEF_CMD(JAE, CMD_JAE, Label, 
{
    if (POP_STACK >= POP_STACK)
    {
        cpu->ip = cpu->code[cpu->ip + 1];
        cpu->ip--;
    }
    else 
        cpu->ip++;
})

DEF_CMD(JE, CMD_JE, Label,
{
    if (POP_STACK == POP_STACK)
    {
        cpu->ip = cpu->code[cpu->ip + 1];
        cpu->ip--;
    }
    else 
        cpu->ip++;
})

DEF_CMD(JNE, CMD_JNE, Label,
{
    if (POP_STACK != POP_STACK)
    {
        cpu->ip = cpu->code[cpu->ip + 1];
        cpu->ip--;
    }
    else 
        cpu->ip++;
})