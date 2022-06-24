#include <protocol.h>
#include <serializing.h>
#include <stdlib.h>

void message_package(struct serial_package *package)
{
	package->header_entries_number = 0;
	package->const_arrays_number = 0;
	package->base_entries_number = 1;
	package->var_arrays_number = 1;
	
	package->base_entries = malloc(package->base_entries_number 
			* sizeof(struct serial_base));
	
	package->var_arrays = malloc(package->var_arrays_number *
			sizeof(struct serial_array));
			
	package->base_entries[0].type = serial_int;
	package->var_arrays[0].type = serial_char;
}

int message_size_from_data(const struct message *message)
{
	struct serial_package package;
	message_package(&package);
	
	package.var_arrays[0].data_size = message->data_len;
	
	int size = serial_package_size(&package);
	
	printf("message size: %d\n", size);
	
	free_serial_package(&package);
	
	return size;
}
int message_size_from_header(const char *header, int size)
{
	struct serial_package package;
	
	message_package(&package);
	
	int temp = serial_header_size(&package);
	
	if(size > temp) {
		deserialize_header(&package, header);
		size = serial_package_size(&package);
	} else {
		size = temp;
	}
	
	free_serial_package(&package);
	
	return size;
}

int read_message(struct message *message, const char *buffer)
{
	struct serial_package package;
	
	message_package(&package);
	
	package.base_entries[0].data = &(message->type);

	int size = serial_header_size(&package);
	deserialize_header(&package, buffer);
	
	buffer += size;
	size += serial_data_size(&package);
	
	message->data_len = package.var_arrays[0].data_size;
	message->data = malloc(message->data_len);
	
	package.var_arrays[0].data = message->data;

	deserialize_data(&package, buffer);

	return size;
};

int write_message(const struct message *message, char *buffer)
{
	struct serial_package package;
	
	message_package(&package);
	
	package.base_entries[0].data = &(message->type);

	package.var_arrays[0].data = message->data;
	package.var_arrays[0].data_size = message->data_len;

	int package_size = serial_package_size(&package);

	serialize_package(&package, buffer);

	return package_size;
};
