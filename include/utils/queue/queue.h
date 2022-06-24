#ifndef QUEUE_H
#define QUEUE_H
#include <stddef.h>

struct queue_node {
	struct queue_node *next;
};

struct queue {
	struct queue_node *first;
	struct queue_node *last;

	size_t offset;
};

void queue_init(struct queue *queue, size_t offset);
void queue_destroy(struct queue *queue, void (*destroy) (void *));
void queue_push(struct queue *queue, void *data);

void *queue_pop(struct queue *queue);
#endif
