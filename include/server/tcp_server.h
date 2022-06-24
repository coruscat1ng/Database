#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <arpa/inet.h>
#include <stddef.h>

struct tcp_server {
	int sockfd;
	struct sockaddr_in addr;

	void *recv_msgs;		
	void *send_msgs;
};

int tcp_listen(struct tcp_server *server, char *ip, unsigned short port);
void tcp_event_loop(struct tcp_server *server, 
		int max_clients,
		void (*read_message)(int sockfd, char *buffer, int *len),
		void (*client_closed)(int sockfd),
		void (*client_connected)(int sockfd),
		const char *(*get_message)(int sockfd, int *size));

void tcp_server_close(struct tcp_server *server);

#endif
