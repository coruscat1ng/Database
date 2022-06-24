#include <tcp_server.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void client_closed(int sockfd);
void read_msg(int sockfd, char *buffer, int *len);
const char *get_msg(int sockfd, int *size);

static char buffer[1231] = "testa";
int test_number = 10;
int len = 5;
int prevfd = 0;
int main(void)
{
	struct tcp_server server;

	if(tcp_listen(&server, "127.0.0.1", 6668) < 0)
		return -1;

	tcp_event_loop(&server, 10, 
		read_msg, 
		0,
		0, 
		get_msg);
	
	tcp_server_close(&server);
	
	return 0;
}

void client_closed(int sockfd) {
	printf("%d disconnected", sockfd);
	return;
}

const char *get_msg(int sockfd, int *size)
{

	if(prevfd == sockfd || test_number <= 0) {
		*size = 0;
		return 0;
	}
	test_number--;
	prevfd = sockfd;
	
	static char send_buffer[1024] = "hi";
	static char intro[] = "server: ";

	
	*size = len + strlen(intro) + 1;
	
	memcpy(send_buffer, intro, strlen(intro));
       	memcpy(send_buffer + strlen(intro), buffer, len);
	send_buffer[*size - 1] = '\0';
	
	return send_buffer;
};

void read_msg(int sockfd, char *b, int *l)
{
	test_number = 10;
	len = *l;
	memcpy(buffer, b, *l);
}
