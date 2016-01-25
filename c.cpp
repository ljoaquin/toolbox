// test tcp socket

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

#include "socketutils.h"
#include "TcpSocket.h"


using namespace toolbox;

int main(int argc, char const *argv[])
{
	// signal(SIGPIPE, SIG_IGN);
	//=========== client ===========
	if(argc == 3)
	{
		const char* ip = argv[1];
		unsigned short port = (unsigned short)atoi(argv[2]);

		printf("ip %s\nport %hu\n", ip, port);

		TcpSocket sock;
		assert(sock.Connect(ip, port));

		assert(sock.SetNonBlocking());

		getchar();

		char msg[] = "hello server";
		int nsent;
		if(sock.CanSend())
		{
			nsent = sock.Send((unsigned char*)msg, strlen(msg));
			printf("nsent:%d\n", nsent);
		}
		else
		{
			printf("can not send\n");
		}
		getchar();

		if(1/*sock.CanSend()*/)
		{
			nsent = sock.Send((unsigned char*)msg, strlen(msg));
			printf("nsent:%d\n", nsent);
		}
		else
		{
			printf("can not send\n");
		}
		getchar();

		if(sock.CanRecv())
		{
			const int buf_len = 1024;
			char buf[buf_len];
			memset(buf, 0, buf_len);
			int nread = sock.Recv((unsigned char*)buf, buf_len);

			printf("recv(%d): %s\n", nread, buf);
		}
		else
		{
			printf("can not recv\n");
		}
	}
	//=========== server ===========
	else
	{
		printf("tcp echo server\n");
		bool r = false;

		unsigned short port = 9999;
		TcpSocket sock;
		r = sock.Listen(port);

		assert(r == true);

		// sock.SetNonBlocking();
		
		while(1)
		{
			printf("waiting for connection...\n");
			TcpSocket client;
			char ip_buf[32];
			memset(ip_buf, 0, 32);
			bool b = sock.Accept(client, ip_buf);
			printf("client ip: %s\n", ip_buf);

			// client.SetNonBlocking();

			while(b)
			{
				const int buf_len = 1024;
				char buf[buf_len];
				memset(buf, 0, buf_len);

				if(1/*client.CanRecv()*/)
				{
					int nread = client.Recv((unsigned char*)buf, buf_len);
					printf("recv(%d): %s\n", nread, buf);
					toolbox::print_bytes(buf, strlen(buf) + 2);
					if(nread <= 0)
					{
						break;
					}

					if(client.CanSend())
					{
						int nsent = client.Send((unsigned char*)buf, strlen(buf));
						printf("send(%d): %s\n", nsent, buf);
						if(nsent <= 0)
						{
							break;
						}
					}
					else
					{
						printf("can not send\n");
					}
				}
				else
				{
					printf("can not recv\n");
				}

				sleep(1);
			}
			sleep(1);
		}

	}

	return 0;
}
