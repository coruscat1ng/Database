#include <stdlib.h>
#include <stack.h>

static inline 
struct stack_node *stack_node(const void *data, size_t offset)
{
	return (struct stack_node *)(((char *) data) + offset);
}
static inline 
struct stack_node *stack_data(const struct stack_node *node, size_t offset)
{
	return (struct stack_node *)(((char *) node) - offset);
}

void stack_init(struct stack *stack, size_t offset)
{
	stack->offset = offset;
	stack->top = 0;
}

void stack_push(struct stack *stack, void *data)
{
	
	stack_node(data, stack->offset)->next = stack_node(stack->top, stack->offset);
	stack->top = data;
}

void *stack_pop(struct stack *stack)
{
	if(stack->top == 0)
		return 0;

	void *data = stack->top;
	stack->top = stack_data(stack_node(stack->top, stack->offset)->next, stack->offset);
	return data;
}

void stack_destroy(struct stack *stack)
{
	while(stack->top)
		free(stack_pop(stack));
}
