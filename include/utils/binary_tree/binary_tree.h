 #ifndef BINARY_H
#define BINARY_H
#include <stddef.h>

enum binary_directions {
	binary_left,
	binary_right
};

struct binary_node {
	struct binary_node *children[2];	
};

enum binary_directions binary_direction(int result);

void *binary_insert(struct binary_node *root, void *data, size_t offset,
	       	int(*compare)(const void *, const void *));
void *binary_remove(struct binary_node *root, void *data, size_t offset,
	       	int(*compare)(const void *, const void *));\
	       	
void *binary_pathfind(const struct binary_node *root, size_t offset, size_t path, size_t length);
void *binary_find(const struct binary_node *root, const void *data, size_t offset, 
		int(*compare)(const void *, const void *));
void binary_left_ray(struct binary_node *root, const void *origin, size_t offset, 
		int(*compare)(const void *, const void *), void(*callback)(void *));
void binary_right_ray(struct binary_node *root, const void *origin, size_t offset, 
		int(*compare)(const void *, const void *), void(*callback)(void *));

void binary_range(struct binary_node *root, 
		const void *lower, const void *upper, size_t offset, 
		int(*compare)(const void *, const void *), void(*callback)(void *));

void binary_inorder(struct binary_node *root, enum binary_directions direction, 
		size_t offset, void(*callback)(void *));

void binary_preorder(struct binary_node *root, enum binary_directions direction, 
		size_t offset, void(*callback)(void *));

void binary_postorder(struct binary_node *root, enum binary_directions direction, 
		size_t offset, void(*callback)(void *));

#endif
