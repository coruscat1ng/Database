#ifndef AVL_TREE_H
#define AVL_TREE_H
#include <stddef.h>
#include <binary_tree.h> 

struct avl_tree {
	struct avl_node *root;
	
	size_t nodes_number;

	int (*compare)(const void *, const void *);
	
	size_t offset;
};

struct avl_node {
	struct binary_node node;
	short balance;
};

void avl_init(struct avl_tree *tree, 
	int (*compare)(const void *l, const void *r),
	size_t offset
);

void *avl_pathfind(struct avl_tree *tree, size_t path, short length);
void *avl_find(struct avl_tree *tree, const void *data);

void avl_range(struct avl_tree *tree, const void *lower, const void *upper, 
	void (*callback)(void *));
	
void avl_left_ray(struct avl_tree *tree, const void *origin, void (*callback)(void *));
void avl_right_ray(struct avl_tree *tree, const void *origin, void (*callback)(void *));

void avl_preorder(struct avl_tree *tree, void (*callback)(void *));	
void avl_inorder(struct avl_tree *tree, void (*callback)(void *));
void avl_postorder(struct avl_tree *tree, void (*callback)(void *));
	
void *avl_insert(struct avl_tree *tree, void *data);
void *avl_remove(struct avl_tree *tree, void *data);
void avl_destroy(struct avl_tree *tree, void (*destroy)(void *));

#endif
