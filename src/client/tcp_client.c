#include <tcp_client.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <errno.h>

enum client_flags {
	run = 1,
	send_msg = (1 << 1)
};



int tcp_connect(struct tcp_client *client, char *ip, unsigned short port)
{
	if(inet_pton(AF_INET, ip, &(client->serv_addr.sin_addr)) <= 0) {
		perror("Invalid ip\n");
		return -1;
	}

	client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(client->sockfd < 0) {
		perror("Socket creation error\n");
		return -1;
	}
	
	
   	client->serv_addr.sin_family = AF_INET;
	client->serv_addr.sin_port = htons(port);

	memset(client->serv_addr.sin_zero, 0, sizeof(client->serv_addr.sin_zero));

	if(connect(client->sockfd, (struct sockaddr *) &(client->serv_addr),
			       	sizeof(client->serv_addr)) < 0) {
		perror("Connection Failed\n");
		return -1;
	}
	
	int res = fcntl(client->sockfd, F_SETFL,  
                        fcntl(client->sockfd, F_GETFL) | O_NONBLOCK);

 	if (res < 0) {
                perror("ioctl() failed");
                close(client->sockfd);
                return -1;
        }

	return 0;
}
void tcp_event_loop(struct tcp_client *client,
		void (*read_message)(char *buffer, int *len),
		const char *(*next_message)(int *size))
{
	char buffer[256];
	int res = 0;

	const char *message;
	int size = 0;

	int flags = run;

	do {
		
		while((res = recv(client->sockfd, buffer, sizeof(buffer), 0)) > 0)
			read_message(buffer, &res);
		
		if(errno != EWOULDBLOCK || res == 0) {
				printf("recv failed");
				close(client->sockfd);
				flags &= !run;
				break;
		}

		do {
			if(size <= 0) {
				message = next_message(&size);
				if(size <= 0)
					break;
			}

			res = send(client->sockfd, message, size, 0);

			if(res <= 0) {
				if(errno != EWOULDBLOCK) {
					perror("send() failed");
					close(client->sockfd);
					flags &= ~run;
				}
				break;
			}
			
			size -= res;
			message += res;

		} while(flags & run);

	} while(flags & run);
	

}

void tcp_client_close(struct tcp_client *client)
{
	close(client->sockfd);
}
