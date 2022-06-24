#ifndef LIST_H
#define LIST_H
#include <stddef.h>
#include <binary_node.h>

enum list_nodes {
	list_left,
	list_right
};

struct list {
	struct binary_node *edges[2];
	struct binary_node *current;

	size_t offset;
};

void list_init(struct list *list, size_t offset);
void list_destroy(struct list *list);

struct list *list_new(size_t offset);
void list_delete(struct list *list);

void *list_current(struct list *list);

void list_move(struct list *list, const int d);

void list_append(struct list *list, const int d, void *data);
void list_remove_current(struct list *list);

void list_traverse(struct list *list, const int d, void (*f)(void *data));
void list_traverse_m(struct list *list, const int d, void (*f)(void *data, void *m), void *m);

#endif
