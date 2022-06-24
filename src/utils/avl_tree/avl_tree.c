#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <avl_tree.h>
#include <binary_tree.h> 
#include <stack.h>

#include "avl_defs.h"

static const short avl_balances[2] = {
	[binary_left] = 1,
	[binary_right] = -1
};

static const short avl_imbalances[2] = {
	[binary_left] = 2,
	[binary_right] = -2
};


void avl_init(struct avl_tree *tree, 
	int (*compare)(const void *, const void *),
	size_t offset)
{
	tree->offset = offset;

	tree->compare = compare;
	tree->root = 0;	
	
	tree->nodes_number = 0;
} 

void *avl_insert(struct avl_tree *tree, void *data)
{

	if(!tree->root) {
		
		tree->root = avl_node(data, tree->offset);
		memset(tree->root, 0, sizeof(struct avl_node));
		
		tree->nodes_number = 1;
		
		return 0;
	}


	enum binary_directions direction; 
	
	struct path path = {
		.directions = 0,
		.length = 0	
	};

	struct avl_node *previous = 0, *current = tree->root;

	while(current != 0) {

		int result = tree->compare(avl_data(current, tree->offset), data);
		if(result == 0) 
			return avl_data(current, tree->offset);
		
		direction = binary_direction(result);

		path_append(&path, direction);

		previous = current;
		current = (struct avl_node *) current->node.children[direction];

	}

	avl_insert_leaf(tree, previous, direction, data);

	avl_update(tree, &path, 1);
		
	return 0;
}

void *avl_remove(struct avl_tree *tree, void *data)
{
	if(tree->nodes_number < 1) 
		return 0;

	
	struct path path = {
		.directions = 0,
		.length = 0	
	};
	
	struct avl_node *previous = 0, *current = tree->root;
	enum binary_directions direction; 
	int result;

	while((result = tree->compare(avl_data(current, tree->offset), data))) {
	
		direction = binary_direction(result);

		path_append(&path, direction);

		previous = current;
		
		current = (struct avl_node *) current->node.children[direction];
		
		if(!current) 
			return 0;
		
	}

	tree->nodes_number--;

	struct avl_node *replace_node, *removed_node = current;
	short rl = !current->node.children[binary_left] + (!(current->node.children[binary_right]) << 1);

	switch(rl) {
		case 0:
			replace_node = (struct avl_node *) current->node.children[binary_right];
			path_append(&path, binary_right);

			while(replace_node->node.children[binary_left]) {
				
				path_append(&path, binary_left);
				
				current = replace_node;
				replace_node = (struct avl_node *) replace_node->node.children[binary_left];
	
			};

			if(current != removed_node) {
				current->node.children[binary_left] = replace_node->node.children[binary_right];
				replace_node->node.children[binary_right] = removed_node->node.children[binary_right];

			}

			replace_node->node.children[binary_left] = removed_node->node.children[binary_left];
			replace_node->balance = removed_node->balance;
			break;

		case 1: 
			replace_node = (struct avl_node *) current->node.children[binary_right];

			break;
		case 2:
			replace_node = (struct avl_node *) current->node.children[binary_left];

			break;
		default:
			replace_node = 0;


	}


	if(removed_node == tree->root) 
		tree->root = replace_node;

	if(previous)
		previous->node.children[direction] = &(replace_node->node);

	avl_update(tree, &path, -1);
	
	return avl_data(removed_node, tree->offset);

}
void avl_destroy(struct avl_tree *tree, void (*destroy)(void *))
{
	avl_postorder(tree, destroy);
	tree->root = 0;
	tree->nodes_number = 0;
}

struct avl_node *avl_rotate_one(struct avl_node *node, enum binary_directions direction)
{

	struct avl_node *child = (struct avl_node *) node->node.children[direction];
	node->node.children[direction] = child->node.children[1 - direction];
	child->node.children[1 - direction] = &(node->node);
	
	if(child->balance == 0) {
		child->balance = -avl_balances[direction];
		node->balance = avl_balances[direction];
		return child;
	}

	int B[2];

	if(child->balance > 0) {
		B[binary_left] = child->balance;
		B[binary_right] = 0;
	} else {
		B[binary_left] = 0;
		B[binary_right] = -child->balance;
	}


	child->balance = -avl_balances[direction] * B[1 - direction];
	node->balance  = avl_balances[direction] * (1 - B[direction]);

	return child;

}

struct avl_node *avl_rotate_two(struct avl_node *node, enum binary_directions direction)
{
	struct avl_node *childB = (struct avl_node *) node->node.children[direction];
	struct avl_node *childC = (struct avl_node *) childB->node.children[1 - direction];

	node->node.children[direction] = childC->node.children[1 - direction];
	
	childB->node.children[1 - direction] = childC->node.children[direction];
	
	childC->node.children[1 - direction] = &(node->node);
	childC->node.children[direction] = &(childB->node);
	

	int B[2], C[2];

	if(childB->balance > 0) {
		B[binary_left] = childB->balance;
		B[binary_right] = 0;
	} else {
		B[binary_left] = 0;
		B[binary_right] = -childB->balance;
	}
	
	if(childC->balance > 0) {
		C[binary_left] = childC->balance;
		C[binary_right] = 0;
	} else {
		C[binary_left] = 0;
		C[binary_right] = -childC->balance;
	}

	node->balance = -avl_balances[direction] * (B[direction] + C[direction]);
	childC->balance = node->balance + childB->balance + 
		avl_balances[direction] * (1 + C[direction]);
	childB->balance += avl_balances[direction] * (1 + C[1 - direction]);


	return childC;	
}


void avl_insert_leaf(struct avl_tree *tree, 
		struct avl_node *node, 
		enum binary_directions direction, 
		void *data)
{
	node->node.children[direction] = &(avl_node(data, tree->offset)->node);
	memset(node->node.children[direction], 0, sizeof(struct avl_node));
	
	tree->nodes_number++;
}

void avl_rebalance(struct avl_tree *tree, struct path *path, short h)
{
	struct avl_node *current;
	enum binary_directions direction;
	int i;

	for(i = path->length - 1; (i >= 0) && (h != 0); i--) {
		
		current = path->nodes[i];
		direction = path_direction(path->directions, i);

		int d = avl_balances[direction], b = current->balance;

		current->balance += h * d;
		h = ((h + abs(b + h * d) - abs(b)) >> 1);

		if(current->balance == avl_imbalances[1 - direction]) 
			direction = 1 - direction;

		if(current->balance == avl_imbalances[direction]) {
			
			b = ((struct avl_node *) current->node.children[direction])->balance;

			if(b == avl_balances[1 - direction])
				current = avl_rotate_two(current, direction);
			else
				current = avl_rotate_one(current, direction);

			if(b != 0)
				h += ((abs(current->balance) + avl_balances[direction] * 
								current->balance) >> 1) - 1;

			if(i > 0) {
				direction = path_direction(path->directions, i - 1);
				path->nodes[i - 1]->node.children[direction] = &(current->node);
			}
		}

	}

	if((path->nodes[i + 1] == tree->root) && (current != tree->root))
		tree->root = current;
	
}

void avl_update(struct avl_tree *tree, struct path *path, short height_change)
{

	path->nodes = malloc(sizeof(struct avl_node *) * path->length);
	path->nodes[0] = tree->root;	
	
	enum binary_directions direction = path_direction(path->directions, 0);

	for(int i = 1; i < path->length; i++) {

		direction = path_direction(path->directions, i - 1);
		path->nodes[i] = (struct avl_node *) path->nodes[i - 1]->node.children[direction]; 
	
	};

	avl_rebalance(tree, path, height_change);

	free(path->nodes);
}

void *avl_pathfind(struct avl_tree *tree, size_t path, short length)
{
	return binary_pathfind(&(tree->root->node), tree->offset, path, length);
}
void *avl_find(struct avl_tree *tree, const void *data)
{

	return binary_find(&(tree->root->node), data,tree->offset, tree->compare);
}

void avl_range(struct avl_tree *tree, const void *lower, const void *upper, 
		void (*callback)(void *))
{
	binary_range(&(tree->root->node), lower, upper, tree->offset, tree->compare, callback);
}
	
void avl_left_ray(struct avl_tree *tree, const void *origin, void (*callback)(void *))
{
	binary_left_ray(&(tree->root->node), origin, tree->offset, tree->compare, callback);
}

void avl_right_ray(struct avl_tree *tree, const void *origin, void (*callback)(void *))
{
	binary_right_ray(&(tree->root->node), origin, tree->offset, tree->compare, callback);
}

void avl_preorder(struct avl_tree *tree, void (*callback)(void *))
{
	binary_preorder(&(tree->root->node), binary_left, tree->offset, callback);
}	
void avl_inorder(struct avl_tree *tree, void (*callback)(void *))
{
	binary_inorder(&(tree->root->node), binary_left, tree->offset, callback);
}
void avl_postorder(struct avl_tree *tree, void (*callback)(void *))
{

	binary_postorder(&(tree->root->node), binary_left, tree->offset, callback);
}
