#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include <protocol.h>
#include <queue.h>
#include <avl_tree.h>

struct message_node {
	struct message message;
	struct queue_node queue_node;
};

struct client {
	int sockfd;
	char sendbuffer[2048];
	char readbuffer[2048];

	int sendsize;
	int readsize;

	struct queue messages;
	struct avl_node clients;
};


struct client *new_client(void);
struct message_node *new_message(void);

void free_message(struct message_node *node);
void free_client(struct client *client);




#endif
