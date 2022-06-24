#include <stdlib.h>
#include <queue.h>

static inline 
struct queue_node *queue_node(const void *data, size_t offset)
{
	return (struct queue_node *)(((char *) data) + offset);
}
static inline 
struct queue_node *queue_data(const struct queue_node *node, size_t offset)
{
	return (struct queue_node *)(((char *) node) - offset);
}

void queue_init(struct queue *queue, size_t offset)
{
	queue->offset = offset;
	queue->first = 0;
	queue->last = 0;
}

void queue_push(struct queue *queue, void *data)
{
	queue_node(data, queue->offset)->next = 0;
	if(queue->first == 0) {
		queue->first = queue->last = queue_node(data, queue->offset);
	} else {
		queue->last->next = queue_node(data, queue->offset);
		queue->last = queue_node(data, queue->offset);
	}
}

void *queue_pop(struct queue *queue)
{
	if(queue->first == 0)
		return 0;

	if(queue->first == queue->last)
		queue->last = 0;

	void *data = queue_data(queue->first, queue->offset);
	queue->first = queue->first->next;
	
	return data;
}

void queue_destroy(struct queue *queue, void (*destroy)(void *))
{
	while(queue->first)
		destroy(queue_pop(queue));
}
