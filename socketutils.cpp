#include "socketutils.h"
#include <stdio.h>
#ifdef _WIN32
#include <WinSock2.h>
#endif

namespace toolbox
{

	void print_bytes(unsigned char* buf, int len)
	{
		printf("[ ");
		for (int i = 0; i < len; ++i)
		{
			printf("0x%.2x ", buf[i]);
		}
		printf("]\n");
	}

	int set_reuseaddr(int sockfd)
	{
		if (sockfd == -1)
			return -1;

		int opt = 1;
#ifdef _WIN32
		const char* opt_val = (const char*)&opt;
#else
		const void* opt_val = (const void*)&opt;
#endif
		return setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, opt_val, sizeof(opt));
	}

	static bool select_rw(int fd, int millisecs, char rw)
	{
		timeval tv;
		tv.tv_sec = millisecs / 1000;
		tv.tv_usec = (millisecs % 1000) * 1000;

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		int r;
		if (rw == 'r')
		{
			r = select(fd, &fds, NULL, NULL, &tv);
		}
		else if (rw == 'w')
		{
			r = select(fd, NULL, &fds, NULL, &tv);
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