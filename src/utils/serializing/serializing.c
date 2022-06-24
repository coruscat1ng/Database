#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <serializing.h>

#define _serialize_number(type, data, buffer) \
	for(int i = 0; i < sizeof(type); i++) { \
		buffer[i] = *data >> (8 * i); \
	} \

#define _deserialize_number(type, data, buffer) \
	*data = (unsigned char) *buffer; \
	for(int i = 1; i < sizeof(type); i++) { \
		*data |= (unsigned char) buffer[i] << (8 * i); \
	} \

static const size_t const_sizes [serial_number] = {
	[serial_char] = sizeof(char),
	[serial_short] = sizeof(short),
	[serial_int] = sizeof(int),
	[serial_long] = sizeof(long),
	[serial_size] = sizeof(size_t)
};

void free_serial_package(struct serial_package *package)
{
	if(package->var_arrays_number > 0) free(package->var_arrays);
	if(package->const_arrays_number > 0)free(package->const_arrays);
	if(package->base_entries_number > 0)free(package->base_entries);
	if(package->header_entries_number > 0)free(package->header_entries);
};

void *get_deserialize(enum serial_types type) {

	void (*deserialize_temp)();

	switch(type) {
		case serial_char: 
			deserialize_temp = deserialize_char; break;
		case serial_short: 
			deserialize_temp = deserialize_short; break;
		case serial_int: 
			deserialize_temp = deserialize_int; break;
		case serial_long: 
			deserialize_temp = deserialize_long; break;
		case serial_size: 
			deserialize_temp = deserialize_size; break;
		case serial_number: 
			deserialize_temp = 0;
	}

	return deserialize_temp;
};

void *get_serialize(enum serial_types type) {

	void (*serialize_temp)();

	switch(type) {
		case serial_char: 
			serialize_temp = serialize_char; break;
		case serial_short: 
			serialize_temp = serialize_short; break;
		case serial_int: 
			serialize_temp = serialize_int; break;
		case serial_long: 
			serialize_temp = serialize_long; break;
		case serial_size: 
			serialize_temp = serialize_size; break;
		case serial_number: 
			serialize_temp = 0;
	}

	return serialize_temp;
}

size_t serial_package_size(const struct serial_package *package)
{
	return serial_header_size(package) + serial_data_size(package);
}

size_t serial_header_size(const struct serial_package *package)
{

	return const_sizes[serial_size] * 
		(package->var_arrays_number + package->header_entries_number);
}

size_t serial_data_size(const struct serial_package *package)
{
	
	size_t size = 0;

	for(int i = 0; i < package->base_entries_number; i++) {
		size += const_sizes[package->base_entries[i].type];
	}
	
	for(int i = 0; i < package->var_arrays_number; i++) {
		size += package->var_arrays[i].data_size;
	}

	for(int i = 0; i < package->const_arrays_number; i++) {
		size += package->const_arrays[i].data_size;
	}

	return size;
}

void deserialize_header(struct serial_package *package, const char* header)
{
	for(int i = 0; i < package->header_entries_number; i++) {
		deserialize_base(package->header_entries + i, header);
		header += const_sizes[package->header_entries[i].type];
	}
	
	const size_t size = const_sizes[serial_size];

	for(int i = 0; i < package->var_arrays_number; i++) {
		deserialize_size(&package->var_arrays[i].data_size, header);
		header += size;
	}
}
	
void deserialize_data(struct serial_package *package, const char* data)
{
	for(int i = 0; i < package->base_entries_number; i++) {
		deserialize_base(package->base_entries + i, data);
		data += const_sizes[package->base_entries[i].type];
	}
	for(int i = 0; i < package->const_arrays_number; i++) {
		deserialize_array(package->const_arrays + i, data);
		data += package->const_arrays[i].data_size;
	}
	for(int i = 0; i < package->var_arrays_number; i++) {
		deserialize_array(package->var_arrays + i, data);
		data += package->var_arrays[i].data_size;
		
	}

}

void deserialize_package(struct serial_package *package, const char* buffer)
{
	const int size = const_sizes[serial_size];

	for(int i = 0; i < package->header_entries_number; i++) {
		deserialize_base(package->header_entries + i, buffer);
		buffer += const_sizes[package->header_entries[i].type];
	}

	for(int i = 0; i < package->var_arrays_number; i++) {
		deserialize_size(&(package->var_arrays[i].data_size), buffer);
		buffer += size;
	}

	deserialize_data(package, buffer);
}

void serialize_header(const struct serial_package *package, char* header)
{
	for(int i = 0; i < package->header_entries_number; i++) {
		serialize_base(package->header_entries + i, header);
		header += const_sizes[package->base_entries[i].type];
	}

	const size_t size = const_sizes[serial_size];
	
	for(int i = 0; i < package->var_arrays_number; i++) {
		serialize_size(&(package->var_arrays[i].data_size), header);
		header += size;
	}

}
void serialize_data(const struct serial_package *package, char* data)
{
	for(int i = 0; i < package->base_entries_number; i++) {
		serialize_base(package->base_entries + i, data);
		data += const_sizes[package->base_entries[i].type];
	}
	

	for(int i = 0; i < package->const_arrays_number; i++) {
		serialize_array(package->const_arrays + i, data);
		data += package->const_arrays[i].data_size;
	}


	for(int i = 0; i < package->var_arrays_number; i++) {
		serialize_array(package->var_arrays + i, data);
		data += package->var_arrays[i].data_size;
	}
	
}

void serialize_package(const struct serial_package *package, char* buffer)
{
	for(int i = 0; i < package->header_entries_number; i++) {
		serialize_base(package->header_entries + i, buffer);
		buffer += const_sizes[package->base_entries[i].type];
	}
	const size_t size = const_sizes[serial_size];
	
	for(int i = 0; i < package->var_arrays_number; i++) {
		serialize_size(&(package->var_arrays[i].data_size), buffer);
		buffer += size;
	}

	serialize_data(package, buffer);
}


void deserialize_base(struct serial_base *entry, const char* data)
{
	void (*deserialize)() = get_deserialize(entry->type);
	deserialize(entry->data, data);
}	
void deserialize_array(struct serial_array *entry, const char* data)
{
	const int size = entry->data_size;
	char *d = entry->data;

	const int inc = const_sizes[entry->type];
	
	void (*deserialize)() = get_deserialize(entry->type);
	
	for(int i = 0; i < size; i += inc) {
		deserialize(d + i, data + i);	
	}
}

void serialize_base(const struct serial_base *entry, char* data)
{
	void (*serialize)() = get_serialize(entry->type);
	serialize(entry->data, data);

}
void serialize_array(const struct serial_array *entry, char* data)
{
	const int size = entry->data_size;
	const char *d = entry->data;

	const int inc = const_sizes[entry->type];
	
	void (*serialize)() = get_serialize(entry->type);
	
	for(int i = 0; i < size; i += inc)
		serialize(d + i, data + i);	
}

void serialize_char(const char *data, char *buffer)
{
	*buffer = *data;
}
void serialize_short(const short *data, char *buffer)
{
	_serialize_number(short, data, buffer);
}
void serialize_int(const int *data, char *buffer)
{

	_serialize_number(int, data, buffer);	
}
void serialize_long(const long *data, char *buffer)
{
	_serialize_number(long, data, buffer);	
}

void serialize_size(const size_t *data, char *buffer)
{	
	_serialize_number(size_t, data, buffer);
}

void deserialize_char(char *data, const char *buffer)
{
	*data = *buffer;	
}

void deserialize_short(short *data, const char *buffer)
{
	_deserialize_number(short, data, buffer);	
}

void deserialize_int(int *data, const char *buffer)
{

	_deserialize_number(int, data, buffer);
}

void deserialize_long(long *data, const char *buffer)
{
	_deserialize_number(long, data, buffer);
}

void deserialize_size(size_t *data, const char *buffer)
{
	_deserialize_number(size_t, data, buffer);
}
