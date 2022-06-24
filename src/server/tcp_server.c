#include <tcp_server.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

enum server_flags {
	run = 1,
	compress = (1 << 1)
};

static void _read_message(int sockfd, char *buffer, int *len)
{
	printf("message from %d: %.*s\n", sockfd, *len, buffer);
}
static void _client_closed(int sockfd)
{
	printf("client %d closed\n", sockfd);	
}
static void _client_connected(int sockfd)
{
	printf("client %d connected\n", sockfd);
}
static const char *_get_message(int sockfd, int *size)
{
	return 0;		
}


int compress_fds(struct pollfd *fds, int fds_number)
{
	int j = 0;
	for(int i = 0; i < fds_number; i++) {
		if(fds[i].fd == -1) {
			j++;
			continue;
		}
	
		if(j != 0) 
			fds[i - j].fd = fds[i].fd;
	}

	return fds_number - j;

}

int tcp_listen(struct tcp_server *server, char *ip, unsigned short port)
{

	if(inet_pton(AF_INET, ip, &(server->addr.sin_addr)) <= 0) {
		perror("invalid ip");
		return -1;	
	}

	server->sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(server->sockfd < 0) {
		perror("socket creation failed\n");
		return -1;
	}
	
	int on = 1;	
	int res = setsockopt(server->sockfd, SOL_SOCKET,  SO_REUSEADDR, 
			(char *)&on, sizeof(on));  	
	if(res < 0) {
		perror("setsockopt() failed");
		close(server->sockfd);
		return -1;
	}

	res = fcntl(server->sockfd, F_SETFL, 
			fcntl(server->sockfd, F_GETFL) | O_NONBLOCK);
	
	if (res < 0) {
		perror("ioctl() failed");
		close(server->sockfd);
		return -1;
 	}
	server->addr.sin_family = AF_INET;
	server->addr.sin_port = htons(port);
	memset(server->addr.sin_zero, 0, sizeof(server->addr.sin_zero));

	if(bind(server->sockfd, (struct sockaddr *) &(server->addr), 
				sizeof(server->addr)) < 0) {
        	perror("bind failed");
        	return -1;
	}
	
	if(listen(server->sockfd, 6) < 0) {
		perror("listen failed");
		return -1;
	}

	return 0;
}
void tcp_event_loop(struct tcp_server *server, 
	int max_clients, 
	void (*read_message)(int sockfd, char *buffer, int *len),
	void (*client_closed)(int sockfd),
	void (*client_connected)(int sockfd),
	const char *(*next_message)(int sockfd, int *size))
{
	if(!read_message) read_message = _read_message;
	if(!client_closed) client_closed = _client_closed;
	if(!client_connected) client_connected = _client_connected;
	if(!next_message) next_message = _get_message;

	struct pollfd fds[max_clients + 1];

	memset(fds, 0 , sizeof(fds));

	fds[0].fd = server->sockfd;
	fds[0].events = POLLIN;

	int fds_number = 1;
	int timeout = 3 * 1000;

	const char *messages[max_clients];
	int sizes[max_clients];

	memset(sizes, 0, sizeof(sizes));

	char buffer[256]; 
	int res = 0;
	int flags = run;

	do {
		for(int i = 1; i < fds_number;) {
			int c = i - 1;

			if(sizes[c] <= 0) {
				messages[c] = next_message(fds[i].fd, sizes + c);
				if(sizes[c] <= 0) {
					i++;
					continue;
				}
			}

			do {
				res = send(fds[i].fd, messages[c], sizes[c], 0);
				if(res <= 0) {
					if(errno != EWOULDBLOCK) {
						perror("send() failed");
						close(fds[i].fd);
						client_closed(fds[i].fd);
						fds[i].fd = -1;
						flags |= compress;
					}
					i++;
					break;
				}
				messages[c] += res;
				sizes[c] -= res;
			} while(sizes[c] > 0);
		}

		if(flags & compress) {
			fds_number = compress_fds(fds, fds_number);
			flags &= ~compress;
		}

		res = poll(fds, fds_number, timeout);

		if(res < 0) {
			perror("poll() failed"); 
			break;
		}

    		if(res == 0) {
			//timeout
			continue;
		}

		for(int i = 1; i < fds_number; i++) {
			
			if(fds[i].revents != POLLIN)
				continue;
				
			while((res = recv(fds[i].fd, buffer, sizeof(buffer), 0)) > 0)
				read_message(fds[i].fd, buffer, &res);

			if(errno != EWOULDBLOCK || res == 0) {
				printf("recv failed");
				close(fds[i].fd);
				client_closed(fds[i].fd);
				fds[i].fd = -1;
				flags |= compress;
			}
			fds[i].revents = 0;
		}

		if(fds_number < max_clients && fds[0].revents == POLLIN) {

			int new_sockfd = accept(server->sockfd, 0, 0);
			
			if(new_sockfd < 0) {
				perror("accept() failed");
				break;
			}
          		
			fcntl(new_sockfd, F_SETFL, 
				fcntl(new_sockfd, F_GETFL) | O_NONBLOCK);

			fds[fds_number].fd = new_sockfd;
			fds[fds_number].events = POLLIN;
			fds_number++;

			client_connected(new_sockfd);

			fds[0].revents = 0;
		}

		if(flags & compress) {
			fds_number = compress_fds(fds, fds_number);
			flags &= ~compress;
		}

	} while(flags & run);

	for(int i = 0; i < fds_number; i++)
		close(fds[i].fd);
}

void tcp_server_close(struct tcp_server *server)
{
	close(server->sockfd);
}
