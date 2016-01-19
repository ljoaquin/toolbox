#include <stdio.h>
#include <assert.h>
#include "UdpSocket.h"

int main(int argc, char const *argv[])
{
	printf("hello world\n");

	assert(argc == 4);

	short local_port = (short)atoi(argv[1]);
	const char* ip = argv[2];
	short remote_port = (short)atoi(argv[3]);
	printf("local_port: %hd\n", local_port);
	printf("remote: %s:%hd\n", ip, remote_port);

	toolbox::UdpSocket mysocket(local_port, ip, remote_port);

	const int buf_len = 1024;
	char buf[buf_len];

	while(1)
	{
		memset(buf, 0, buf_len);
		printf("lj> ");
		fgets(buf, buf_len, stdin);

		if(buf[0] == 'q')
			break;

		if(buf[0] == 'r')
		{
			memset(buf, 0, buf_len);
			if(!mysocket.recvPacket((unsigned char*)buf, buf_len))
			{
				printf("recv error\n");
				mysocket.debugPrint();
				break;
			}
			printf("recv(%d): ", strlen(buf));
			toolbox::print_bytes(buf, strlen(buf));
			continue;
		}

		buf[strlen(buf) - 1] = 0;
		int send_len = strlen(buf);

		printf("send(%d): ", send_len);
		toolbox::print_bytes(buf, strlen(buf));

		if(!mysocket.sendPacket((unsigned char*)buf, send_len))
		{
			printf("send error\n");
			mysocket.debugPrint();
			break;
		}
	}


	return 0;
}