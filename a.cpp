#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <queue>

typedef struct
{
	unsigned char* base;
	int len;
}buf_t;

typedef std::queue<unsigned char*> queue_t;
static queue_t send_queue;
static queue_t recv_queue;

void clear_queue(queue_t& queue)
{
	while(queue.size())
	{
		unsigned char* buf = queue.front();
		queue.pop();
		delete[] buf;
	}
}

void print_binary(char* buf, int len)
{
	printf("[ ");
	for (int i = 0; i < len; ++i)
	{	
		printf("0x%.2x ", buf[i]);
	}
	printf("]\n");
}

void* run(void* arg)
{
	printf("run\n");

	// int val = *((int*)arg) * 2;
	// int* ret_ptr = (int*)malloc(sizeof(int));
	// *ret_ptr = val;
	// return ret_ptr;

	while(1)
	{
		fd_set rfds, wfds;
		timeval tv = { 0, 0 };
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

	}
}

int main(int argc, char const *argv[])
{
	printf("hello\n");

	assert(argc == 3);
	for (int i = 0; i < argc; ++i)
	{
		printf("[%d]: %s\n", i, argv[i]);
	}
	const char* ip = argv[1];
	short port = (short)atoi(argv[2]);
	printf("connect ip: %s, port: %d\n", ip, port);

	int sockfd = -1;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	inet_aton(ip, &server_addr.sin_addr);
	server_addr.sin_port = htons(port);

	assert(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0);



	// pthread_t net_thread;
	// assert(pthread_create(&net_thread, NULL, run, &val) == 0);
	// assert(pthread_detach(net_thread) == 0);



	const int input_buf_len = 1024;
	char input_buf[input_buf_len];

	while(1)
	{
		printf("input> ");
		memset(input_buf, 0, input_buf_len);
		fscanf(stdin, "%s", input_buf);
		// printf("just input: %s\n", input_buf);
		// print_binary(input_buf, strlen(input_buf) + 1);

		int send_buf_len = strlen(input_buf) + 1;
		unsigned char* send_buf = new unsigned char[send_buf_len];
		memset(send_buf, 0, send_buf_len);
		memcpy(send_buf, input_buf, strlen(input_buf));

		int nsent = send(sockfd, send_buf, send_buf_len - 1, 0);
		printf("nsent: %d\n", nsent);

		send_queue.push(send_buf);
		// printf("queue len: %d\n", send_queue.size());

		if(input_buf[0] == 'q')
		{
			break;
		}
	}

	printf("safe quit\n");

	clear_queue(send_queue);
	clear_queue(recv_queue);

	// printf("return from thread 1: %d\n", *((int*)ret_ptr));

	assert(close(sockfd) == 0);

	return 0;
}