#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <tcp_server.h>
#include <db.h>
#include <dbgen.h>
#include <dbserial.h>
#include <protocol.h>
#include <queue.h>
#include <avl_tree.h>

#include "messages.h"
#include "server_core.h"

#define MAX_CLIENTS 30

static char *prefix = "**server: ";
static struct db *selected_db;
static struct avl_tree *selected_clients;

void client_closed(int sockfd);
void client_connected(int sockfd);
void read_msg(int sockfd, char *buffer, int *len);
const char *next_msg(int sockfd, int *size);

void handle_msg(struct client *client, struct message *message);
int client_cmp(const void *l, const void *r);

int main(void)
{
	struct db db;
	init_db(&db);
	generate_db(&db, 100);

	struct avl_tree clients;
	avl_init(&clients, client_cmp, offsetof(struct client, clients));
	
	selected_db = &db;
	selected_clients = &clients;
	
	name_range_tortiks(&db, 0, 0, t_print);
	
	struct tcp_server server;

	if(tcp_listen(&server, "127.0.0.1", 6667) < 0)
		return -1;

	tcp_event_loop(&server, 10, 
		read_msg, 
		client_closed,
		client_connected, 
		next_msg);
	
	tcp_server_close(&server);
	
	return 0;
}

void client_connected(int sockfd)
{
	struct client *client = new_client();
	client->sockfd = sockfd;
	avl_insert(selected_clients, client);	

	printf("client connected %d\n", sockfd);
}

void client_closed(int sockfd) {
	struct client *client = avl_remove(selected_clients, &sockfd);
	free_client(client);

	printf("client %d disconnected\n", sockfd);
}

const char *next_msg(int sockfd, int *size)
{
	struct client *client = avl_find(selected_clients, &sockfd);
	struct message_node *node = queue_pop(&(client->messages)); 
	
	if(!node) {
		*size = 0;
		return 0;
	}
	
	*size = write_message(&(node->message), client->sendbuffer);

	printf("sending %d butes\n", *size);
	
	free(node->message.data);
	free(node);

	return client->sendbuffer;	
}

int client_cmp(const void *l, const void *r)
{
	return *(int *)l - *(int *)r;
}
void read_msg(int sockfd, char *buffer, int *len)
{
	printf("received %d bytes from %d\n", *len, sockfd);
	
	struct client *client = avl_find(selected_clients, &sockfd);
	struct message message;
	
	memcpy(client->readbuffer + client->readsize, buffer, *len);
	client->readsize += *len;

	int size = message_size_from_header(client->readbuffer, client->readsize);
	
	if(size > client->readsize)
		return;

	int offset = 0;
	
	do {
		read_message(&message, client->readbuffer + offset);
		printf("message type: %d\n", message.type);
		printf("message size: %d\n", message.data_len);

		handle_msg(client, &message);	
		client->readsize -= size;
		offset += size;

		size = message_size_from_header(client->readbuffer + offset, client->readsize);

	} while(size <= client->readsize);

	for(int i = 0; i < client->readsize; i++)
	       		client->readbuffer[i] = client->readbuffer[i + offset];

}

static struct client *current_client = 0;

void queue_tortik_msg(void *t)
{
	struct tortik *tortik = t;
	t_print(tortik);
	int size = tortik_size_from_data(tortik);
	struct message_node *node = malloc(sizeof(struct message_node));
	
	node->message.type = data_tortik_response;
	node->message.data_len = size;
	node->message.data = malloc(size);

	write_tortik(tortik, node->message.data);

	queue_push(&(current_client->messages), node);
}
void queue_ingridient_msg(void *i)
{
	struct ingridient *ingridient = i;
	i_print(ingridient);
	int size = ingridient_size_from_data(ingridient);
	struct message_node *node = malloc(sizeof(struct message_node));
	
	node->message.type = data_ingridient_response;
	node->message.data_len = size;
	node->message.data = malloc(size);

	size = write_ingridient(ingridient, node->message.data);
	printf("written size %d\n", size);
	queue_push(&(current_client->messages), node);
}

void tortik_remove_handler(struct message *message, struct queue *queue)
{
	char *name;
	extract_find_msg(message, &name);

	printf("finding tortik %s...\n", name);

	struct tortik *tortik = remove_tortik(selected_db, name);
	struct message_node *node = malloc(sizeof(struct message_node));

	if(!tortik) {
		node->message.type = request_failure;

		char temp[] = "couldn't remove: no such tortik found";

		create_find_msg(&(node->message), temp);
		node->message.data = temp;
		node->message.data_len = sizeof(temp);

		printf("%s\n", temp);
	} else {
		node->message.data_len = 0;
		node->message.data = 0;

		free(tortik->name);
		free(tortik->recipe.entries);
		free(tortik);

		node->message.type = remove_tortik_response;
	}

	free(name);
	queue_push(queue, node);
}

void ingridient_insert_handler(struct message *message, struct queue *queue)
{
	struct ingridient ingridient;

	read_ingridient(&ingridient, message->data);

	struct ingridient *res = insert_ingridient(selected_db, &ingridient);

	struct message_node *node = malloc(sizeof(struct message_node));

	node->message.data_len = 0;
	node->message.data = 0;

	if(res) {
		node->message.type = request_failure;

		char temp[] = "couldn't insert: such ingridient exists";
		create_find_msg(&(node->message), temp);
		printf("%s\n", temp);

	} else {
		printf("ingridient inserted!\n");
		node->message.type = insert_ingridient_response;
	}

	queue_push(queue, node);
}


void tortik_insert_handler(struct message *message, struct queue *queue)
{
	struct tortik tortik;
	read_tortik(&tortik, message->data);

	struct tortik *res = name_find_tortik(selected_db, tortik.name);
	struct message_node *node = malloc(sizeof(struct message_node));

	node->message.data_len = 0;
	node->message.data = 0;

	int i = 0;
	if(res) {
		node->message.type = request_failure;

		char temp[] = "couldn't insert: such tortik exists";
		create_find_msg(&(node->message), temp);
		printf("%s\n", temp);

	} else {
		for(; i < tortik.recipe.entries_number; i++) {
			struct ingridient *ingridient = name_find_ingridient(selected_db, tortik.recipe.entries[i].ingridient->name);
			if(!ingridient) {
				node->message.type = request_failure;

				char temp[] = "couldn't insert: no such ingridient";
				create_find_msg(&(node->message), temp);
				printf("%s\n", temp);
				res = &tortik;
			}

			free(tortik.recipe.entries[i].ingridient->name);
			free(tortik.recipe.entries[i].ingridient);

			tortik.recipe.entries[i].ingridient = ingridient;
		}
		if(!res) {
			printf("ingridient inserted!\n");
			node->message.type = insert_tortik_response;
			insert_tortik(selected_db, &tortik);
		}
	}

	if(res) {

		for(;i < tortik.recipe.entries_number; i++) {
			free(tortik.recipe.entries[i].ingridient->name);
			free(tortik.recipe.entries[i].ingridient);
		}
		free(tortik.recipe.entries);
		free(tortik.name);
	}

	queue_push(queue, node);
}

void ingridient_remove_handler(struct message *message, struct queue *queue)
{
	char *name;
	extract_find_msg(message, &name);

	printf("finding ingridient %s...\n", name);

	struct ingridient *ingridient = remove_ingridient(selected_db, name);
	struct message_node *node = malloc(sizeof(struct message_node));

	node->message.data_len = 0;
	node->message.data = 0;

	if(!ingridient) {
		node->message.type = request_failure;

		char temp[] = "couldn't remove: no such ingridient";

		create_find_msg(&(node->message), temp);
		printf("%s\n", temp);

	} else {
		printf("ingridient removed!\n");
		node->message.type = remove_ingridient_response;
	}

	free(name);
	queue_push(queue, node);
}

void tortik_range_handler(struct message *message,struct queue *queue)
{
	printf("tortiks range...\n");
	char *l, *u;
	extract_range_msg(message, &l, &u);

	printf("name ranging\n");
	name_range_tortiks(selected_db, l, u, queue_tortik_msg);
}

void ingridient_range_handler(struct message *message,struct queue *queue)
{
	printf("ingridients range...\n");
	char *l, *u;
	extract_range_msg(message, &l, &u);

	printf("name ranging\n");
	name_range_ingridients(selected_db, l, u, queue_ingridient_msg);
}


void ingridient_find_handler(struct message *message,struct queue *queue)
{
	char *name;
	extract_find_msg(message, &name);

	printf("finding ingridient %s...\n", name);

	struct ingridient *ingridient = name_find_ingridient(selected_db, name);
	struct message_node *node = malloc(sizeof(struct message_node));

	if(!ingridient) {
		node->message.type = request_failure;

		char temp[] = "no such ingridient found";
		create_find_msg(&(node->message), temp);

		printf("%s\n", temp);
	} else {
		i_print(ingridient);

		int size = ingridient_size_from_data(ingridient);

		printf("ingridient size %d\n", size);

		node->message.type = data_ingridient_response;
		node->message.data_len = size;
		node->message.data = malloc(size);

		size = write_ingridient(ingridient, node->message.data);
		printf("written size %d\n", size);
	}

	free(name);
	queue_push(queue, node);
}

void tortik_find_handler(struct message *message,struct queue *queue)
{

	char *name;
	extract_find_msg(message, &name);
	printf("finding tortik %s...\n", name);

	struct tortik *tortik = name_find_tortik(selected_db, name);
	struct message_node *node = malloc(sizeof(struct message_node));

	if(!tortik) {
		node->message.type = request_failure;

		char temp[] = "no such tortik found";
		create_find_msg(&(node->message), temp);

		printf("%s\n", temp);
	} else {
		t_print(tortik);
		int size = tortik_size_from_data(tortik);

		printf("size %d\n", size);

		node->message.type = data_tortik_response;
		node->message.data_len = size;
		node->message.data = malloc(size);

		write_tortik(tortik, node->message.data);
	}

	queue_push(queue, node);
}

static void (*request_handlers[])(struct message *message, struct queue *queue) = {
	[tortik_remove_request] = tortik_remove_handler,
	[tortik_insert_request] = tortik_insert_handler,
	[tortik_find_request] = tortik_find_handler,
	[tortik_range_request] = tortik_range_handler,

	[ingridient_insert_request] = ingridient_insert_handler,
	[ingridient_remove_request] = ingridient_remove_handler,
	[ingridient_find_request] = ingridient_find_handler,
	[ingridient_range_request] = ingridient_range_handler

};

void handle_msg(struct client *client, struct message *message)
{
	printf("%s", prefix);
	enum requests request = message->type;
	current_client = client;
	request_handlers[request](message, &(client->messages));
}


