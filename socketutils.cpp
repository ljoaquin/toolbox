#include "socketutils.h"
#include <stdio.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#endif

namespace toolbox
{

	void print_bytes(void* buf, int len)
	{
		printf("[ ");
		for (int i = 0; i < len; ++i)
		{
			printf("0x%.2x ", ((unsigned char*)buf)[i]);
		}
		printf("]\n");
	}

	int set_reuseaddr(int sockfd)
	{
		int opt = 1;
#ifdef _WIN32
		const char* opt_val = (const char*)&opt;
#else
		const void* opt_val = (const void*)&opt;
#endif
		return setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, opt_val, sizeof(opt));
	}

	int bind(int sockfd, unsigned short port)
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		inet_aton("0.0.0.0", &addr.sin_addr);
		addr.sin_port = htons(port);

		return ::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	}

	int connect(int sockfd, const char* ip, unsigned short port)
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		inet_aton(ip, &addr.sin_addr);
		addr.sin_port = htons(port);

		return ::connect(sockfd, (const struct sockaddr*)&addr, sizeof(addr));
	}

	static bool select_rw(int fd, int millisecs, char rw)
	{
		timeval tv;
		tv.tv_sec = millisecs / 1000;
		tv.tv_usec = (millisecs % 1000) * 1000;

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		int r = -1;
		if (rw == 'r')
		{
			r = select(fd + 1, &fds, NULL, NULL, &tv);
		}
		else if (rw == 'w')
		{
			r = select(fd + 1, NULL, &fds, NULL, &tv);
		}
		if (r == -1 || r == 0)
		{
			return false;
		}
		else
		{
			r = FD_ISSET(fd, &fds);
			if (r)
			{
				return true;
			}
		}
		return false;
	}

	bool select_for_read(int fd, int millisecs)
	{
		return select_rw(fd, millisecs, 'r');
	}

	bool select_for_write(int fd, int millisecs)
	{
		return select_rw(fd, millisecs, 'w');
	}

	/*
	int peek(int sockfd, int timeout)
	{
		int result = 0;

		timeval tv; // = { 0, 0 };
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;

		fd_set rfds, wfds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(sockfd, &rfds);
		FD_SET(sockfd, &wfds);

		int r = select(sockfd, &rfds, &wfds, NULL, &tv);
		if (r == -1)
		{
			result = result | PEEK_RESULT_ERROR;
		}
		else if (r == 0)
		{
			result = result | PEEK_RESULT_TIMEOUT;
		}
		else
		{
			r = FD_ISSET(sockfd, &rfds);
			if (r)
			{
				result = result | PEEK_RESULT_READ_AVAILABLE;
			}
			r = FD_ISSET(sockfd, &wfds);
			if (r)
			{
				result = result | PEEK_RESULT_WRITE_AVAILABLE;
			}
		}

		return result;
	}
	*/

}
