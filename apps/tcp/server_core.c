#include "server_core.h"
#include "stdlib.h"

struct client *new_client()
{
	struct client *client = malloc(sizeof(struct client));
	queue_init(&(client->messages), offsetof(struct message_node, queue_node));
	client->readsize = 0;

	return client;
}
void free_client(struct client *client)
{
	struct message_node *node;
	while((node = queue_pop(&(client->messages))))
		free(node);

	free(client);	
};

struct message_node *new_message(void)
{
	struct message_node *node = malloc(sizeof(struct message_node));
	node->message.data = 0;
	node->message.data_len = 0;
	return node;
}

void free_message(struct message_node *node)
{
	free(node);
}
