#include <string.h>
#include <serializing.h>
#include <stdlib.h>

#include "messages.h"

void create_range_msg(struct message *msg, const char *l, const char *u)
{
	struct serial_package package = {
		.var_arrays_number = 2,
		.const_arrays_number = 0,
		.base_entries_number = 0,
		.header_entries_number = 0
	};

	package.var_arrays = malloc(package.var_arrays_number
		       	* sizeof(struct serial_array));
	
	package.var_arrays[0].data = l;
	package.var_arrays[0].data_size = strlen(l) + 1;
	package.var_arrays[0].type = serial_char;
	
	package.var_arrays[1].data = u;
	package.var_arrays[1].data_size = strlen(u) + 1;
	package.var_arrays[1].type = serial_char;
	
	msg->data_len = serial_package_size(&package);
	msg->data = malloc(msg->data_len);

	serialize_package(&package, msg->data);

	free_serial_package(&package);
}
void create_find_msg(struct message *msg, const char *name)
{
	struct serial_package package = {
		.var_arrays_number = 1,
		.const_arrays_number = 0,
		.base_entries_number = 0,
		.header_entries_number = 0
	};

	package.var_arrays = malloc(package.var_arrays_number
		       	* sizeof(struct serial_array));
	
	package.var_arrays[0].data = name;
	package.var_arrays[0].type = serial_char;
	package.var_arrays[0].data_size = strlen(name) + 1;
	
	msg->data_len = serial_package_size(&package);
	msg->data = malloc(msg->data_len);

	serialize_package(&package, msg->data);

	free_serial_package(&package);

}

void extract_range_msg(const struct message *msg, char **l, char **u)
{
	struct serial_package package = {
		.var_arrays_number = 2,
		.const_arrays_number = 0,
		.base_entries_number = 0,
		.header_entries_number = 0
	};

	package.var_arrays = malloc(package.var_arrays_number
		       	* sizeof(struct serial_array));

	deserialize_header(&package, msg->data);

	*l = malloc(package.var_arrays[0].data_size);
	*u = malloc(package.var_arrays[1].data_size);
	
	package.var_arrays[0].data = *l;
	package.var_arrays[0].type = serial_char;
	package.var_arrays[1].data = *u;
	package.var_arrays[1].type = serial_char;

	int size = serial_header_size(&package);
	deserialize_data(&package, msg->data + size);

	free_serial_package(&package);
}
void extract_find_msg(const struct message *msg, char **name)
{
	struct serial_package package = {
		.var_arrays_number = 1,
		.const_arrays_number = 0,
		.base_entries_number = 0,
		.header_entries_number = 0
	};

	package.var_arrays = malloc(package.var_arrays_number
		       	* sizeof(struct serial_array));
	
	deserialize_header(&package, msg->data);

	*name = malloc(package.var_arrays[0].data_size);

	package.var_arrays[0].data = *name;
	package.var_arrays[0].type = serial_char;

	deserialize_package(&package, msg->data);

	free_serial_package(&package);
}


