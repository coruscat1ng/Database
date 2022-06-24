#include <string.h>
#include <avl_tree.h>
#include <stdlib.h>

#include "client_core.h"

static char *commands_names[commands_number] = {
		[connect_cmd] = "connect",
		
		[ingridients_range_cmd] = "ingridients range",
		[find_ingridient_cmd] = "find ingridient",
		[insert_ingridient_cmd] = "insert ingridient",
		[remove_ingridient_cmd] = "remove ingridient",
	
		[tortiks_range_cmd] = "tortiks range",
		[find_tortik_cmd] = "find tortik",
		[insert_tortik_cmd] = "insert tortik",
		[remove_tortik_cmd] = "remove tortik"	
	};

static struct avl_tree commands_tree; 

struct command_node {
	char *name;
	enum commands code;
	struct avl_node commands_node;
};

void create_commands_tree(void) 
{

	avl_init(&commands_tree, commands_cmp, offsetof(struct command_node, commands_node));
	
	for(int i = 0; i < commands_number; i++) {
		struct command_node *node = malloc(sizeof(struct command_node));
		node->name = commands_names[i];
		node->code = i;
		avl_insert(&commands_tree, node); 
	}
}

enum commands find_command(char *name)
{
	struct command_node *node = avl_find(&commands_tree, &name);
	
	if(!node)
		return commands_number; 

	return node->code;
}

void compress_buffer(char c, char *buffer, int *size)
{
	int len;
	int i;

	for(i = 0; i < *size && buffer[i] == c; i++);	
	for(len = 0; i == len && i < (*size - 1); i++)
		if(buffer[i] != c || buffer[i + 1] != c)
			len++;

	for(;i < *size; i++) {
		if(buffer[i - 1] == c && buffer[i] == c)
			continue;
		buffer[len] = buffer[i];
		len++;
	}

	*size = len;
}

int commands_cmp(const void *l, const void *r)
{	
	const struct command_node *left = l, *right = r;
	return strcmp(left->name, right->name);
};
