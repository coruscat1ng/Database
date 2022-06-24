#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#include <avl_tree.h>
#include <queue.h>
#include <protocol.h>

enum commands {
	connect_cmd,
	
	ingridients_range_cmd,
	find_ingridient_cmd,
	insert_ingridient_cmd,
	remove_ingridient_cmd,
	
	tortiks_range_cmd,
	find_tortik_cmd,
	insert_tortik_cmd,
	remove_tortik_cmd,
	
	commands_number
};

void compress_buffer(char c, char *buffer, int *size);
void create_commands_tree(void);
enum commands find_command(char *name);

struct message_node {
	struct queue_node node;
	struct message message;
};

enum client_app_flags {
	run = 1,
	connected = (1 << 1)
};


int commands_cmp(const void *l, const void *r);

#endif
