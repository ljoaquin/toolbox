#include <stdio.h>
#include <assert.h>
#include "UdpSocket.h"


#define LOCAL_PORT 33123
#define REMOTE_IP "127.0.0.1"
#define REMOTE_PORT 9999


void on_recv(unsigned char* buf, unsigned int nread)
{
	printf("on_recv(%d): ", strlen((char*)buf));
	toolbox::print_bytes(buf, strlen((char*)buf));
}

void test_select()
{
	printf("%s\n", __FUNCTION__);

	toolbox::UdpSocket mysocket(LOCAL_PORT);
	mysocket.setRemote(REMOTE_IP, REMOTE_PORT);

	bool b = mysocket.isRecvReady(1000 * 10);
	printf("ready: %d\n", b);

	mysocket.recvPacket(on_recv);

	getchar();
}

class Coder
{
public:
	Coder();
	~Coder();

	void on_recv(unsigned char* buf, unsigned int nread){
		printf("Coder on_recv(%d): ", strlen((char*)buf));
		toolbox::print_bytes(buf, strlen((char*)buf));
	};

private:
	int age;
	int coding_age;
};

Coder::Coder()
{
}

Coder::~Coder()
{
}

void test_recv2()
{
	Coder coder;
	auto func = std::bind(&Coder::on_recv, coder, std::placeholders::_1, std::placeholders::_2);

	toolbox::UdpSocket mysocket(LOCAL_PORT);

	if (!mysocket.recvPacket(func))
	{
		printf("recv error\n");
		mysocket.debugPrint();
	}
	getchar();
}

void test_recv()
{
	toolbox::UdpSocket mysocket(LOCAL_PORT);

	if (!mysocket.recvPacket(on_recv))
	{
		printf("recv error\n");
		mysocket.debugPrint();
	}
	getchar();
}

void test_both(int argc, char const *argv[])
{
	unsigned short local_port = LOCAL_PORT;
	const char* ip = REMOTE_IP;
	unsigned short remote_port = REMOTE_PORT;

	if (argc == 4)
	{
		local_port = (unsigned short)atoi(argv[1]);
		ip = argv[2];
		remote_port = (unsigned short)atoi(argv[3]);
	}

	printf("local_port: %hu\n", local_port);
	printf("remote: %s:%hu\n", ip, remote_port);

	toolbox::UdpSocket mysocket(local_port, ip, remote_port);

	//toolbox::UdpSocket mysocket(LOCAL_PORT, REMOTE_IP, REMOTE_PORT);

	const int buf_len = 1024;
	char buf[buf_len];

	while (1)
	{
		memset(buf, 0, buf_len);
		printf("lj> ");
		fgets(buf, buf_len, stdin);

		if (buf[0] == 'q')
			break;

		if (buf[0] == 'r')
		{
			if (!mysocket.recvPacket(on_recv))
			{
				printf("recv error\n");
				mysocket.debugPrint();
				break;
			}
			continue;
		}

		if (buf[0] == 's')
		{
			char* send_buf = buf + 2;

			send_buf[strlen(send_buf) - 1] = 0;
			int send_len = strlen(send_buf);

			printf("send(%d): ", send_len);
			toolbox::print_bytes(send_buf, strlen(send_buf));

			if (!mysocket.sendPacket((unsigned char*)send_buf, send_len))
			{
				printf("send error\n");
				mysocket.debugPrint();
				break;
			}
		}
	}
	getchar();
}

int main(int argc, char const *argv[])
{
	printf("hello world\n");

	test_both(argc, argv);

	// test_select();

	return 0;
}