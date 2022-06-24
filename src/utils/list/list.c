#include <list.h>
#include <stdio.h>
#include <stdlib.h>

void list_init(struct list *list, size_t offset)
{
	*list = (struct list) {
		.offset = offset,
		.edges = {0, 0},
		.current = 0
	};
}
void list_destroy(struct list *list)
{
	
}

struct list *list_new(size_t offset) 
{
	struct list *list = malloc(sizeof(struct list));
	
	list_init(list, offset);
	
	return list;
}
void list_delete(struct list *list)
{
	list_destroy(list);
	free(list);
}

void *list_current(struct list *list)
{
	if(!list->current)
		return 0;

	return binary_data(list->current, list->offset);
}

void list_move(struct list *list, const int node)
{
	if(!list->current)
		return;

	list->current = list->current->nodes[node];
}

void list_append(struct list *list, const int node, void *data)
{
	struct binary_node *new_node = binary_node(data, list->offset);
	
	new_node->nodes[node] = 0;
	new_node->nodes[1 - node] = list->edges[node];

	if(list->edges[node])
		list->edges[node]->nodes[node] = new_node;
	else
		list->current = list->edges[1 - node] = new_node;

	list->edges[node] = new_node;
}

void list_remove_current(struct list *list)
{
	enum possible_removes {
		remove_nonedge_node,
		remove_left_edge,
		remove_right_edge,
		remove_both_edges
	} remove = !list->current->nodes[list_left] + 2 * !list->current->nodes[list_right];

	switch(remove) {
	case remove_nonedge_node:
		list->current->nodes[list_right]->nodes[list_left] = 
		list->current->nodes[list_left];

		list->current->nodes[list_left]->nodes[list_right] = 
		list->current->nodes[list_right];

		list->current = list->current->nodes[list_right];

	break;
	case remove_left_edge:
		list->edges[list_left] = list->current = list->current->nodes[list_right];
		list->edges[list_left]->nodes[list_left] = 0;
	break;
	case remove_right_edge:
		list->edges[list_right] = list->current = list->current->nodes[list_left];
		list->edges[list_right]->nodes[list_right] = 0;
	break;
	case remove_both_edges:
		list->edges[0] = 0;
		list->edges[1] = 0;
		list->current = 0;
	break;
	}
}

void list_traverse(struct list *list, const int d, void (*f)(void *data))
{
	struct binary_node *node = list->edges[1 - d];

	while(node) {
		f(binary_data(node, list->offset));
		node = node->nodes[d];
	}
}
void list_traverse_m(struct list *list, const int d, void (*f)(void *data, void *m), void *m)
{
	struct binary_node *node = list->edges[1 - d];

	while(node) {
		f(binary_data(node, list->offset), m);
		node = node->nodes[d];
	}

}





