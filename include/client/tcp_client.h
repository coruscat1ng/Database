#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <arpa/inet.h>

struct tcp_client {
	int sockfd;
	struct sockaddr_in serv_addr;
};

int tcp_connect(struct tcp_client *client, char *ip, unsigned short port);

void tcp_event_loop(struct tcp_client *client,
	void (*read_message)(char *buffer, int *len),
	const char *(*next_message)(int *size));
	     
void tcp_client_close(struct tcp_client *client);

#endif
