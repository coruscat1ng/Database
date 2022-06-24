#ifndef MESSAGES_H
#define MESSAGES_H
#include <protocol.h>

enum requests {
	tortik_insert_request,
	tortik_remove_request,
	tortik_find_request,
	tortik_range_request,
	
	ingridient_insert_request,
	ingridient_remove_request,
	ingridient_find_request,
	ingridient_range_request
};

enum responses {
	request_failure,
	
	data_tortik_response,
	data_ingridient_response,
	insert_tortik_response,
	insert_ingridient_response,
	remove_tortik_response,
	remove_ingridient_response
};


void create_range_msg(struct message *msg, const char *l, const char *u);
void create_find_msg(struct message *msg, const char *name);

void extract_range_msg(const struct message *msg, char **l, char **u);
void extract_find_msg(const struct message *msg, char **name);

#endif
