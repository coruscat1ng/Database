#include <tcp_client.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static char *hello = "wat up";
int should_send = 10;

void read_message(char *buffer, int *len);
const char *next_message(int *size);

int main()
{
	struct tcp_client client;
	if(tcp_connect(&client, "127.0.0.1", 6667) < 0)
		return -1;

	tcp_event_loop(&client, read_message, next_message);

	tcp_client_close(&client);

	return 0;
}

void read_message(char *buffer, int *len)
{
	printf("new Message %d butes :", *len);
	for(int i = 0; i < *len; i++) {
		putchar(buffer[i]);
	}
	should_send = 10;
	puts("\n");
	sleep(1);
}
const char *next_message(int *size)
{
	if(should_send <= 0) {
		*size = 0;
		return 0;
	}
	
	should_send--;
	
	*size = strlen(hello);
	return hello;
}
