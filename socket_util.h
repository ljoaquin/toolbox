#ifndef __SOCKET_UTIL_H__
#define __SOCKET_UTIL_H__

#ifdef _WIN32
typedef int socklen_t;
#endif

extern struct sockaddr_in;

namespace toolbox
{

	int set_reuseaddr(int sockfd);
	int bind(int sockfd, unsigned short port);
    int accept(int sockfd, sockaddr_in* addr);
	
    int connect(int sockfd, const char* ip, unsigned short port);

    int set_blocking(int fd);
    int set_nonblocking(int fd);

	bool select_for_read(int fd, int millisecs = 0);
	bool select_for_write(int fd, int millisecs = 0);

    int close_socket(int fd);

    // for windows, do nothing in unix platform
    int socket_init();
    int socket_cleanup();

    const char* get_ipaddress(const char* hostname);

    void sleep(unsigned int millisecs);

}

#endif // __SOCKET_UTIL_H__