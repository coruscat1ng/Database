#ifndef PROTOCOL_H
#define PROTOCOL_H
enum protocol_result {
	error_header_size = -2,
	error_package_size = -1,
	success
};

struct message {
	int type;
	int data_len;
	char *data;
};

int message_size_from_data(const struct message *message);
int message_size_from_header(const char *header, int size);

int read_message(struct message *message, const char *buffer);
int write_message(const struct message *message, char *buffer);

#endif
