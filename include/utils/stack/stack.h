#ifndef STACK_H
#define STACK_H
#include <stddef.h>


struct stack {
	void *top;
	size_t offset;
};

struct stack_node {
	struct stack_node *next;
};

void stack_init(struct stack *stack, size_t offset);

void stack_push(struct stack *stack, void *data);
void *stack_pop(struct stack *stack);

void stack_destroy(struct stack *stack);

#endif
