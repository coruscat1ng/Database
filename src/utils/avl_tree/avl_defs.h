#include <stddef.h>
#include <avl_tree.h>

struct path { 
	struct avl_node **nodes; 
	
	size_t directions;
	unsigned short length;
};

static inline
void path_append(struct path *path, enum binary_directions direction)
{
	path->directions |= (direction << path->length);
        path->length++;
}

void avl_rebalance(struct avl_tree *tree, struct path *path, short height_change);
void avl_update(struct avl_tree *tree, struct path *path, short height_change);

struct avl_node *avl_rotate_one(struct avl_node *node, enum binary_directions direction);
struct avl_node *avl_rotate_two(struct avl_node *node, enum binary_directions direction);

void avl_insert_leaf(struct avl_tree *tree, 
		struct avl_node *node, 
		enum binary_directions direction, 
		void *data);



static inline enum binary_directions path_direction(size_t path, unsigned short index)
{
	return (enum binary_directions)((path >> index) & 1); 
}

static inline struct avl_node *avl_node(void *data, size_t offset)
{
	return (struct avl_node *)(((char *) data) + offset);
}

static inline void *avl_data(struct avl_node *node, size_t offset)
{
	return (void *)(((char *) node) - offset);
}
