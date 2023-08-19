#define Token_Type tokens->array[tokens->size].type

#define Token_Data tokens->array[tokens->size].data

#define Token_Name tokens->array[tokens->size].name

#define PUSH_STACK(elem)  \
    Stack_Push(&cpu->stk, elem)

#define POP_STACK   \
    Stack_Pop(&cpu->stk)

#define PUSH_RET_STACK(elem)    \
    Stack_Push(&cpu->ret_stk, elem)

#define POP_RET_STACK   \
    Stack_Pop(&cpu->ret_stk)

#define GET_PUSH_ARG    \
    GetPushArg(cpu, cpu->array[cpu->ip]);

#define GET_POP_ARG     \
    GetPopArg(cpu, cpu->array[cpu->ip]);

#define GET_JUMP_ARG    \
    JumpArg(cpu);

#define IP  \
    cpu->ip