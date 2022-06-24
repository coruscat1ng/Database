#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <avl_tree.h>
#include <db.h>
#include <dbserial.h>
#include <serializing.h>
#include <protocol.h>
#include <tcp_client.h>
#include <queue.h>

#include <pthread.h>
#include "client_core.h"
#include "messages.h"

#define MAXLEN 30

static int flags;
static char prefix[] = "** client: ";
static char s_prefix[] = "** server: ";
static struct queue *messages;

void handle_cmd(enum commands cmd);
void handle_msg(struct message *message);
void *connect_client(void *id);

void read_msg(char *buffer, int *len);
const char *next_msg(int *size);

int getinput(char *buffer, int maxsize);

int main()
{
	char buffer[257];

	struct queue queue;
	queue_init(&queue, offsetof(struct message_node, node));

	create_commands_tree();

	flags = run | ~connected;
	messages = &queue;

	handle_cmd(connect_cmd);

	do {
		int len = getinput(buffer, MAXLEN) + 1;

		compress_buffer(' ', buffer, &len);

		handle_cmd(find_command(buffer));

	} while(flags & run);



	return 0;
}

void read_msg(char *buffer, int *len)
{
	static char readbuffer[1024];
	static int readsize = 0;
	static struct message message;

	memcpy(readbuffer + readsize, buffer, *len);
	readsize += *len;

	int size = message_size_from_header(readbuffer, readsize);

	if(size > readsize) {
		return;
	}

	int offset = 0;

	do {
		read_message(&message, readbuffer + offset);
		handle_msg(&message);
		readsize -= size;
		offset += size;
		if(!readsize)
			break;
		size = message_size_from_header(readbuffer + offset, readsize);

	} while(size <= readsize);

	for(int i = 0; i < readsize; i++)
	       		readbuffer[i] = readbuffer[i + offset];

}
const char *next_msg(int *size)
{
	static char sendbuffer[1024];
	static struct message_node *node = 0;

	if(node) {
		free(node->message.data);
		free(node);
	}

	node = queue_pop(messages);

	if(!node) {
		*size = 0;
		return 0;
	}

	*size = write_message(&(node->message), sendbuffer);

	return sendbuffer;
}

void *connect_client(void * id)
{
	printf("%sconnecting...\n", prefix);

	struct tcp_client client;

	if(tcp_connect(&client, "127.0.0.1", 6667) < 0) {
		printf("%sconnection failed\n", prefix);
		return 0;
	}

	flags |= connected;

	printf("%sconnected\n", prefix);

	tcp_event_loop(&client, read_msg, next_msg);
	tcp_client_close(&client);

	printf("%sdisconnected\n", prefix);

	flags &= ~connected;

	return 0;
}

int getinput(char *buffer, int maxsize)
{
	fgets(buffer, maxsize, stdin);

	int len = strlen(buffer);
	if(len > 0 && buffer[len - 1] == '\n')
		buffer[--len] = '\0';
	return len;
}
void handle_msg(struct message *message)
{
	enum responses response = message->type;
	switch(response) {
		case remove_tortik_response:
			printf("%stortik successfuly removed\n", s_prefix);
			break;
		case remove_ingridient_response:
			printf("%singridient successfuly removed\n", s_prefix);
			break;
		case insert_tortik_response:
			printf("%stortik successfuly inserted\n", s_prefix);
			break;
		case insert_ingridient_response:
			printf("%singridient successfuly inserted\n", s_prefix);
			break;
		case data_tortik_response:
		       	{
				struct tortik tortik;
				read_tortik(&tortik, message->data);
				t_print(&tortik);

				free(tortik.name);
				for(int i = 0; i < tortik.recipe.entries_number; i++) {
					free(tortik.recipe.entries[i].ingridient->name);
					free(tortik.recipe.entries[i].ingridient);
				}

				free(tortik.recipe.entries);
			}
			break;
		case data_ingridient_response:
		       	{
				struct ingridient ingridient;
				read_ingridient(&ingridient, message->data);
				printf("%s ", s_prefix);
				i_print(&ingridient);

				free(ingridient.name);
			}
			break;

		case request_failure:
			{
				char *error;
				extract_find_msg(message, &error);
				printf("%s%s\n", s_prefix, error);
				free(error);
			}
			break;
	}
}
void insert_ingridient_handler(struct queue *queue)
{
	char name[MAXLEN];
	struct ingridient ingridient;

	printf("name: ");

	if(!getinput(name, MAXLEN)) {
		printf("%sname must be nonempty", prefix);
		return;
	}
	ingridient.name = name;

	printf("calories: "); scanf("%d", &ingridient.calories);
	printf("taste: "); scanf("%d", &ingridient.taste);

	getchar();

	int size = ingridient_size_from_data(&ingridient);
	struct message_node *node = malloc(sizeof(struct message_node));

	node->message.type = ingridient_insert_request;
	node->message.data_len = size;
	node->message.data = malloc(size);

	size = write_ingridient(&ingridient, node->message.data);

	queue_push(queue, node);

	printf("%sinserting ingridient...\n", prefix);
}

void remove_ingridient_handler(struct queue *queue)
{
	char name[MAXLEN];
	getinput(name, MAXLEN);

	struct message_node *node = malloc(sizeof(struct message_node));

	create_find_msg(&(node->message), name);
	node->message.type = ingridient_remove_request;
	queue_push(queue, node);

	printf("%sremoving ingridient...\n", prefix);
}

void insert_tortik_handler(struct queue *queue)
{
	char name[MAXLEN];
	struct tortik tortik;

	printf("name: ");

	if(!getinput(name, MAXLEN)) {
		printf("%sname must be nonempty", prefix);
		return;
	}

	tortik.name = name;

	printf("ingridients number (1 - 10): "); scanf("%d", &tortik.recipe.entries_number);
	getchar();
	printf("%screating recipe:\n", prefix);

	if((tortik.recipe.entries_number < 0) || (tortik.recipe.entries_number > 10)) {
		printf("%singridients must be between 1 and 10\n", prefix);
		return;
	}

	tortik.recipe.entries = malloc(sizeof(struct recipe_entry)
		* tortik.recipe.entries_number);

	for(int i = 0; i < tortik.recipe.entries_number; i++) {

		char temp[MAXLEN];
		printf("name: "); int len = getinput(temp, MAXLEN);
		if(len < 1) {
			printf("%sname must be nonempty", prefix);
			i--;
			continue;
		}

		struct ingridient *ingridient = malloc(sizeof(struct ingridient));
		ingridient->name = malloc(len + 1);
		memcpy(ingridient->name, temp, len + 1);
		printf("weight: "); scanf("%d", &tortik.recipe.entries[i].weight);
		getchar();

		tortik.recipe.entries[i].ingridient = ingridient;
	}

	int size = tortik_size_from_data(&tortik);
	struct message_node *node = malloc(sizeof(struct message_node));

	node->message.type = tortik_insert_request;
	node->message.data_len = size;
	node->message.data = malloc(size);

	write_tortik(&tortik, node->message.data);

	for(int i = 0; i < tortik.recipe.entries_number; i++) {
		free(tortik.recipe.entries[i].ingridient->name);
		free(tortik.recipe.entries[i].ingridient);
	}

	free(tortik.recipe.entries);

	queue_push(queue, node);

	printf("%sinserting tortik...\n", prefix);
}

void remove_tortik_handler(struct queue *queue)
{
	char name[MAXLEN];
	getinput(name, MAXLEN);

	struct message_node *node = malloc(sizeof(struct message_node));

	create_find_msg(&(node->message), name);
	node->message.type = tortik_remove_request;
	queue_push(queue, node);
	printf("%sremoving tortik...\n", prefix);
}

void tortiks_range_handler(struct queue *queue) {
	char l[MAXLEN];
	char u[MAXLEN];

	if(!getinput(l, MAXLEN)) {
		printf("%slower name must be nonempty", prefix);
		return;
	}


	if(!getinput(u, MAXLEN)) {
		printf("%supper name must be nonempty", prefix);
		return;
	}

	struct message_node *node = malloc(sizeof(struct message_node));

	create_range_msg(&(node->message), l, u);
	node->message.type = tortik_range_request;

	queue_push(queue, node);
	printf("%stortiks range...\n", prefix);
}

void find_tortik_handler(struct queue *queue)
{
	char name[MAXLEN];
	getinput(name, MAXLEN);

	struct message_node *node = malloc(sizeof(struct message_node));

	create_find_msg(&(node->message), name);
	node->message.type = tortik_find_request;
	queue_push(queue, node);

	printf("%sfinding tortik...\n", prefix);
}

void ingridients_range_handler(struct queue *queue)
{
	char l[MAXLEN];
	char u[MAXLEN];

	if(!getinput(l, MAXLEN)) {
		printf("%slower name must be nonempty", prefix);
		return;
	}


	if(!getinput(u, MAXLEN)) {
		printf("%supper name must be nonempty", prefix);
		return;
	}

	struct message_node *node = malloc(sizeof(struct message_node));

	create_range_msg(&(node->message), l, u);
	node->message.type = ingridient_range_request;
	queue_push(queue, node);

	printf("%singridients range...\n", prefix);
}

void find_ingridient_handler(struct queue *queue)
{
	char name[MAXLEN];
	if(!getinput(name, MAXLEN)) {
		printf("%sname must be nonempty", prefix);
		return;
	}

	struct message_node *node = malloc(sizeof(struct message_node));

	create_find_msg(&(node->message), name);
	node->message.type = ingridient_find_request;
	queue_push(queue, node);

	printf("%sfinding ingridient...\n", prefix);
}
void connect_handler(struct queue *queue)
{
	if(flags & connected) {
		printf("%salready connected\n", prefix);
		return;
	}

	pthread_t input_thread;
	pthread_create(&input_thread, 0, connect_client, 0);
}

void commands_number_handler(struct queue *queue)
{
	printf("%sunknown command\n", prefix);
}

static void (*cmd_handlers[])(struct queue *queue) = {
	[connect_cmd] = connect_handler,
	[ingridients_range_cmd] = ingridients_range_handler,
	[find_ingridient_cmd] = find_ingridient_handler,
	[insert_ingridient_cmd] = insert_ingridient_handler,
	[remove_ingridient_cmd] = remove_ingridient_handler,

	[tortiks_range_cmd] = tortiks_range_handler,
	[find_tortik_cmd] = find_tortik_handler,
	[insert_tortik_cmd] = insert_tortik_handler,
	[remove_tortik_cmd] = remove_tortik_handler,
	[commands_number] = commands_number_handler
};

void handle_cmd(enum commands cmd)
{
	cmd_handlers[cmd](messages);
}
