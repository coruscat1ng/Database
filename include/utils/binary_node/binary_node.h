#ifndef BINARY_NODE_H
#define BINARY_NODE_H

static inline struct binary_node *binary_node(const void *data, size_t offset)
{
        return (struct binary_node *)(((char *) data) + offset);
}

static inline void *binary_data(const struct binary_node *node, size_t offset)
{
        return (void *)(((char *) node) - offset);
}

struct binary_node {
	struct binary_node *nodes[2];
};

#endif
