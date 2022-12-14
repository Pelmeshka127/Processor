#define PUSH_STACK(elem)  \
    Stack_Push(&cpu->stack, elem)

#define POP_STACK   \
    Stack_Pop(&cpu->stack)

#define PUSH_RET_STACK(elem)    \
    Stack_Push(&cpu->ret_stack, elem)

#define POP_RET_STACK   \
    Stack_Pop(&cpu->ret_stack)
