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

typedef struct
{
	std::queue<unsigned char*> queue;
	pthread_mutex_t lock;
}queue_t;

static const int READ_AVAILABLE = 1 << 1;
static const int WRITE_AVAILABLE = 1 << 2;

// typedef std::queue<unsigned char*> queue_t;
static queue_t send_queue;
static queue_t recv_queue;

// delete items and clear
void clear_queue(queue_t& queue)
{
	while(queue.queue.size())
	{
		unsigned char* buf = queue.queue.front();
		queue.queue.pop();
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

void peek(int fd, int& result)
{
	int sockfd = fd;
	fd_set rfds, wfds;
	timeval tv = { 0, 0 };
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_SET(sockfd, &rfds);
	FD_SET(sockfd, &wfds);
	int flag = select(sockfd + 1, &rfds, &wfds, NULL, &tv);
	// printf("select %d\n", flag);

	result = 0;
	if(FD_ISSET(sockfd, &rfds))
	{
		result = result | READ_AVAILABLE;
	}
	if(FD_ISSET(sockfd, &wfds))
	{
		result = result | WRITE_AVAILABLE;
	}
}

typedef struct
{
	pthread_cond_t cond;
	pthread_mutex_t lock;
}cond_signal_t;

static cond_signal_t s_signal;

void thread_wait()
{
	pthread_mutex_lock(&s_signal.lock);
	printf("net thread waiting...\n");
	pthread_cond_wait(&s_signal.cond, &s_signal.lock);
	pthread_mutex_unlock(&s_signal.lock);
}

void* run(void* arg)
{
	int sockfd = *((int*)arg);

	printf("run sockfd %d\n", sockfd);

	pthread_cond_init(&s_signal.cond, NULL);
	pthread_mutex_init(&s_signal.lock, NULL);

	thread_wait();

	bool is_response = false;
	while(1)
	{
		// printf("loop\n");

		int r = 0;
		peek(sockfd, r);
		if(r & READ_AVAILABLE)
		{
			// printf("read available\n");
			const int buf_len = 32;
			char buf[buf_len];
			memset(buf, 0, buf_len);
			int nread = recv(sockfd, buf, buf_len, 0);
			printf("recv %d, %s\n", nread, buf);
			is_response = true;
		}
		if(r & WRITE_AVAILABLE)
		{
			pthread_mutex_lock(&send_queue.lock);
			if(send_queue.queue.size() > 0)
			{
				for (int i = 0; i < send_queue.queue.size(); ++i)
				{
					char* buf = (char*)send_queue.queue.front();
					int sent = send(sockfd, buf, strlen(buf), 0);
					printf("sent %d, %s\n", sent, buf);
				}
				clear_queue(send_queue);
			}
			pthread_mutex_unlock(&send_queue.lock);
		}

		if(is_response)
		{
			thread_wait();
			is_response = false;
		}

		usleep(1 * 1000);
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



	pthread_t net_thread;
	assert(pthread_create(&net_thread, NULL, run, &sockfd) == 0);
	assert(pthread_detach(net_thread) == 0);

	// print_select(sockfd);


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

		// int nsent = send(sockfd, send_buf, send_buf_len - 1, 0);
		// printf("nsent: %d\n", nsent);
		// print_select(sockfd);

		pthread_mutex_lock(&send_queue.lock);
		send_queue.queue.push(send_buf);
		// printf("queue len: %d\n", send_queue.queue.size());
		pthread_mutex_unlock(&send_queue.lock);

		pthread_mutex_lock(&s_signal.lock);
		printf("wake net thread\n");
		pthread_cond_signal(&s_signal.cond);
		pthread_mutex_unlock(&s_signal.lock);

		if(input_buf[0] == 'q')
		{
			break;
		}

		sleep(1);
	}

	printf("safe quit\n");

	clear_queue(send_queue);
	clear_queue(recv_queue);

	// printf("return from thread 1: %d\n", *((int*)ret_ptr));

	assert(close(sockfd) == 0);

	return 0;
}