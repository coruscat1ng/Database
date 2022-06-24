#ifndef SERIALIZING_H
#define SERIALIZING_H
#include <stddef.h>

enum serial_types {

	serial_char,	
	serial_short,
	serial_int,
	serial_long,
	serial_size,
	
	serial_number
};

struct serial_base {
	enum serial_types type;
	void *data;
};

struct serial_array {
	enum serial_types type;
	void *data;
	size_t data_size;
};

struct serial_package {
	
	struct serial_base *header_entries;
	size_t header_entries_number;

	struct serial_base *base_entries;
	size_t base_entries_number;
			
	struct serial_array *var_arrays;
	size_t var_arrays_number;
	
	struct serial_array *const_arrays;
	size_t const_arrays_number;

};

void free_serial_package(struct serial_package *package);

size_t serial_package_size(const struct serial_package *package);
size_t serial_header_size(const struct serial_package *package);
size_t serial_data_size(const struct serial_package *package);

void deserialize_header(struct serial_package *package, const char* header);
void deserialize_data(struct serial_package *package, const char* data);

void deserialize_package(struct serial_package *package, const char* buffer);

void serialize_header(const struct serial_package *package, char* header);
void serialize_data(const struct serial_package *package, char* data);
void serialize_package(const struct serial_package *package, char* buffer);

void deserialize_base(struct serial_base *entry, const char* buffer);
void deserialize_array(struct serial_array *entry, const char* buffer);

void serialize_base(const struct serial_base *entry, char* buffer);
void serialize_array(const struct serial_array *entry, char* buffer);

void serialize_char(const char *data, char *buffer);
void serialize_short(const short *data, char *buffer);
void serialize_int(const int *data, char *buffer);
void serialize_long(const long *data, char *buffer);
void serialize_size(const size_t *data, char *buffer);

void deserialize_char(char *data, const char *buffer);
void deserialize_short(short *data, const char *buffer);
void deserialize_int(int *data, const char *buffer);
void deserialize_long(long *data, const char *buffer);
void deserialize_size(size_t *data, const char *buffer);

#define serialize_number(Data, Buffer)\
_Generic((Data),\
	int: serialize_int,\
	long: serialize_long,\
	short: serialize_short,\
	size_t: serialize_size\
)(Data, Buffer)

#define deserialize_number(Data, Buffer)\
_Generic((Data),\
	int *: deserialize_int,\
	long *: deserialize_long,\
	short *: deserialize_short,\
	size_t *: deserialize_size\
)(Data, Buffer)


#endif
