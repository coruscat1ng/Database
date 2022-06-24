#include <stdlib.h>

#include <binary_tree.h>
#include <stack.h>

static inline struct binary_node *binary_node(const void *data, size_t offset)
{
        return (struct binary_node *)(((char *) data) + offset);
}

static inline void *binary_data(const struct binary_node *node, size_t offset)
{
        return (void *)(((char *) node) - offset);
}

enum binary_directions binary_direction(int result)
{
	return result > 0 ? binary_left : binary_right;
}

struct binary_node *binary_remove_root(struct binary_node *root)
{
	if(!root->children[binary_right])
		return root->children[binary_left];

	struct binary_node *roots[2] = {
		root,
		root->children[binary_right]
	};
	
	int i = 1;

	while(roots[i & 1]->children[binary_left]) {
		roots[1 - (i & 1)] = roots[i & 1]->children[binary_left]; 
		i++;
	}
	
	roots[i & 1]->children[binary_left] = root->children[binary_left];
	roots[1 - (i & 1)]->children[binary_left] = roots[i & 1]->children[binary_right];
	
	return roots[i & 1];

}

void *binary_pathfind(const struct binary_node *root, size_t offset, size_t path, size_t length)
{

	for(int i = 0; (i < length) && root; i++)
		root = root->children[(path >> i) & 1];

	return binary_data(root, offset);
}

void *binary_insert(struct binary_node *root, void *data, size_t offset, 
		int(*compare)(const void *, const void *))
{

	int result = compare(binary_data(root, offset), data);
	if(result == 0)
		return binary_data(root, offset);
	
	enum binary_directions direction = binary_direction(result);

	while(root->children[direction]) {
		
		root = root->children[direction];

		result = compare(binary_data(root, offset), data);
		if(result == 0)
			return binary_data(root, offset);

		direction = binary_direction(result);
	
	};

	root->children[direction] = binary_node(data, offset);

	return 0;	
}
void *binary_remove(struct binary_node *root, void *data, size_t offset, 
		int(*compare)(const void *, const void*))
{
	int result = compare(binary_data(root, offset), data);
	if(result == 0)
		return binary_data(binary_remove_root(root), offset);
	
	enum binary_directions direction = binary_direction(result);

	while(root->children[direction]) {
		
		result = compare(binary_data(root, offset), data);
		
		if(result == 0) {
			struct binary_node *temp = root->children[direction];
			root->children[direction] = binary_data(binary_remove_root(temp), offset);
			return temp;
		}

		root = root->children[direction];

		direction = binary_direction(result);
	
	} while(root);

	return 0;	
}

void *binary_find(const struct binary_node *root, const void *data, size_t offset, 
		int(*compare)(void const*, void const*))
{
	while(root) {
		int result = compare(binary_data(root, offset), data);
		if(result == 0)
			return binary_data(root, offset);

		root = root->children[binary_direction(result)];
	}

	return 0;
}
void binary_right_ray(struct binary_node *root, const void *origin, size_t offset, 
		int(*compare)(const void *, const void *), void (*callback)(void *))
{
	struct stack stack;
  
        struct _node {
                struct binary_node *binary;
                struct stack_node stack;    
        } *node;
  
        stack_init(&stack, offsetof(struct _node, stack)); 

	while(root) {
		int result = compare(binary_data(root, offset), origin);
		
		if(result == 0) {
			node = malloc(sizeof(struct _node));
			node->binary = root;
			stack_push(&stack, node);

			break;
		}


		if(binary_direction(result) == binary_right) {
			root = root->children[binary_right];
			continue;
		}

		node = malloc(sizeof(struct _node));
		node->binary = root;
		stack_push(&stack, node);

		root = root->children[binary_left];
		
	}

	while(stack.top) {
		node = stack_pop(&stack);
		root = node->binary;

		callback(binary_data(root, offset));

		if(root->children[binary_right])
			binary_inorder(root->children[binary_right], binary_left,
					offset, callback);
	}
}

void binary_left_ray(struct binary_node *root, const void *origin, size_t offset,
	       	int(*compare)(const void *, const void *), void (*callback)(void *))
{
	
	while(root) {
		int result = compare(binary_data(root, offset), origin);

		if(result == 0) {
			if(root->children[binary_left])
				binary_inorder(root->children[binary_left], binary_left,
					offset, callback);

			callback(binary_data(root, offset));
		
			return;
		}

		if(binary_direction(result) == binary_left) {
			root = root->children[binary_left];
			continue;
		}
		
		if(root->children[binary_left])
			binary_inorder(root->children[binary_left], binary_left,
					offset, callback);

		callback(binary_data(root, offset));
		
		root = root->children[binary_right];
		
	}

}

void binary_range(struct binary_node *root, const void *lower, const void *upper, size_t offset, 
                int(*compare)(const void *, const void *), void(*callback)(void *))
{
	int result = compare(binary_data(root, offset), lower);

	while(result != 0 && binary_direction(result) == binary_right) {
		root = root->children[binary_right]; 
		if(!root)
			return;
		result = compare(binary_data(root, offset), lower);
	}

	result = compare(binary_data(root, offset), upper);
	
	while(result != 0 && binary_direction(result) == binary_left) {
		root = root->children[binary_left]; 
		if(!root)
			return;
		result = compare(binary_data(root, offset), upper);
	}

	if(root->children[binary_left])
		binary_right_ray(root->children[binary_left], 
				lower, offset, compare, callback);

	callback(binary_data(root, offset));

	if(root->children[binary_right])
		binary_left_ray(root->children[binary_right], 
				upper, offset, compare, callback);
}

void binary_inorder(struct binary_node *root, enum binary_directions direction, 
                size_t offset, void(*callback)(void *))
{
	struct stack stack;
  
        struct _node {
                struct binary_node *binary;
                struct stack_node stack;    
        } *node;
  
        stack_init(&stack, offsetof(struct _node, stack)); 
  
        do {
		while(root) {
			node = malloc(sizeof(struct _node));
			node->binary = root;
			stack_push(&stack, node);

			root = root->children[direction];
		}

		node = stack_pop(&stack);
		root = node->binary;

		callback(binary_data(root, offset));

		free(node);

		root = root->children[1 - direction];


	} while(stack.top || root);

}

void binary_preorder(struct binary_node *root, enum binary_directions direction, 
                size_t offset, void(*callback)(void *))
{

  	struct stack stack;
  
        struct _node {
                struct binary_node *binary;
                struct stack_node stack;    
        } *node;
  
        stack_init(&stack, offsetof(struct _node, stack)); 
  	
	do {
 
		callback(binary_data(root, offset));
  
		if(root->children[1 - direction]) {
			node = malloc(sizeof(struct _node));
			node->binary = root->children[1 - direction];
			stack_push(&stack, node);
		}
  
		if(root->children[direction]) {
			node = malloc(sizeof(struct _node));
			node->binary = root->children[direction];
			stack_push(&stack, node);
		}

		if(!stack.top)
			return;

		node = stack_pop(&stack);
		root = node->binary;
		free(node);

	} while(root);

}

void binary_postorder(struct binary_node *root, enum binary_directions direction, 
                size_t offset, void(*callback)(void *))
{

	struct stack stack;
  
        struct _node {
                struct binary_node *binary;
		int visited : 1;
                struct stack_node stack;    
        } *node;
  
        stack_init(&stack, offsetof(struct _node, stack));
  	
	do {
  
		while(root->children[direction]) {
			
			node = malloc(sizeof(struct _node));
			node->binary = root;
			node->visited = !(root->children[1 - direction]);

			stack_push(&stack, node);

			root = root->children[direction];
		}

  
		if(root->children[1 - direction]) {
			node = malloc(sizeof(struct _node));
			node->binary = root;
			node->visited = 1;

			stack_push(&stack, node);
		
			root = root->children[1 - direction];

			continue;
		}

		callback(binary_data(root, offset));

		
		while((node = stack.top) && node->visited) {

			root = node->binary;

			callback(binary_data(root, offset));
			
			stack_pop(&stack);
			
			free(node);

		}

		if(!stack.top)
			return;

		root = node->binary;

		node->visited = 1;
		
		root = root->children[1 - direction];
		
	} while(root || stack.top);


}

