#include "dsl.hpp"

DEF_CMD(HLT, CMD_HLT, 3, No_Arg_Type,
{
    printf("The end of cpu\n");
    mode = false;
})

DEF_CMD(PUSH, CMD_PUSH, 4, Digit_Type,
{
    int elem = GET_PUSH_ARG;
    PUSH_STACK(elem);
})

DEF_CMD(POP, CMD_POP, 3, Digit_Type,
{
    int *elem = GET_POP_ARG;
    *elem = POP_STACK;
})

DEF_CMD(ADD, CMD_ADD, 3, No_Arg_Type,
{
    PUSH_STACK(POP_STACK + POP_STACK);
})

DEF_CMD(SUB, CMD_SUB, 3, No_Arg_Type,
{
    PUSH_STACK(POP_STACK - POP_STACK);
})

DEF_CMD(MUL, CMD_MUL, 3, No_Arg_Type,
{
    PUSH_STACK(POP_STACK * POP_STACK);
})

DEF_CMD(DIV, CMD_DIV, 3, No_Arg_Type,
{
    elem_t a = POP_STACK;
    elem_t b = POP_STACK;
    PUSH_STACK(b / a);
})

DEF_CMD(MINUS, CMD_MINUS, 5, No_Arg_Type,
{
    PUSH_STACK(-1 * POP_STACK);
})

DEF_CMD(SQRT, CMD_SQRT, 4, No_Arg_Type,
{
    PUSH_STACK(sqrt(POP_STACK));
})

DEF_CMD(OUT, CMD_OUT, 3, No_Arg_Type,
{
    elem_t value = POP_STACK;
    printf("The result is %.2f\n", value);
})

DEF_CMD(IN, CMD_IN, 2, No_Arg_Type,
{
    int value = 0;
    scanf("%d", &value);
    PUSH_STACK(value);
})

DEF_CMD(CALL, CMD_CALL, 4, Label_Type,
{
    IP++;
    PUSH_RET_STACK(IP + sizeof(int) - 1);
    IP = GET_JUMP_ARG;
})

DEF_CMD(RET, CMD_RET, 3, No_Arg_Type,
{
    IP = POP_RET_STACK;
})

DEF_CMD(JMP, CMD_JMP, 3, Label_Type,
{
    IP++;
    IP = GET_JUMP_ARG;
})

DEF_CMD(JAE, CMD_JAE, 3, Label_Type,
{
    IP++;
    if (POP_STACK <= POP_STACK)
    {
        IP = GET_JUMP_ARG;
    }
    else 
        IP += sizeof(int) - 1;
})

DEF_CMD(JA, CMD_JA, 2, Label_Type,
{
    IP++;
    if (POP_STACK < POP_STACK)
    {
        IP = GET_JUMP_ARG;
    }
    else 
        IP += sizeof(int) - 1;
})

DEF_CMD(JB, CMD_JB, 2, Label_Type,
{
    IP++;
    if (POP_STACK > POP_STACK)
    {
        IP = GET_JUMP_ARG;
    }
    else
        IP += sizeof(int) - 1;
})

DEF_CMD(JBE, CMD_JBE, 3, Label_Type,
{
    IP++;
    if (POP_STACK >= POP_STACK)
    {
        IP = GET_JUMP_ARG;
    }
    else 
        IP += sizeof(int) - 1;
})

DEF_CMD(JNE, CMD_JNE, 3, Label_Type,
{
    IP++;
    if ((int) POP_STACK != (int) POP_STACK)
    {
        IP = GET_JUMP_ARG;
    }
    else 
        IP += sizeof(int) - 1;
})

DEF_CMD(JE, CMD_JE, 2, Label_Type,
{
    IP++;
    if ((int) POP_STACK == (int) POP_STACK)
    {
        IP = GET_JUMP_ARG;
    }
    else 
        IP += sizeof(int) - 1;
})

DEF_CMD(ZERO, CMD_ZERO, 4, No_Arg_Type,
{
    printf("No roots\n");
})

DEF_CMD(BESK, CMD_BESK, 4, No_Arg_Type,
{
    printf("Inf roots\n");
})