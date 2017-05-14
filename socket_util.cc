#include "socket_util.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#endif

namespace toolbox
{

#ifdef _WIN32
    // internal
    static int inet_aton(const char* ip, in_addr* buf)
    {
        return inet_pton(AF_INET, ip, buf); // inet_pton is in Ws2tcpip.h
    }
#endif

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

    int accept(int sockfd, sockaddr_in* addr)
    {
        socklen_t addrlen = sizeof(addr);
        return ::accept(sockfd, (sockaddr*)addr, &addrlen);
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
        struct timeval tv;
        tv.tv_sec = millisecs / 1000;
        tv.tv_usec = (millisecs % 1000) * 1000;

        struct fd_set fds;
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

    int set_blocking(int fd)
    {
#ifdef _WIN32
        u_long argp = 0;
        return ioctlsocket(fd, FIONBIO, &argp);
#else
        int flags = fcntl(fd, F_GETFL, 0);
        flags &= (~(O_NONBLOCK));
        return fcntl(fd, F_SETFL, flags);
#endif
    }

    int set_nonblocking(int fd)
    {
#ifdef _WIN32
        u_long argp = 1;
        return ioctlsocket(fd, FIONBIO, &argp);
#else
        int flags = fcntl(fd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        return fcntl(fd, F_SETFL, flags);
#endif
    }

    int close_socket(int fd)
    {
#ifdef _WIN32
        return ::closesocket(fd);
#else
        return ::close(fd);
#endif
    }

    int socket_init()
    {
#ifdef _WIN32
        WSADATA wsaData;
        WORD version = MAKEWORD(2, 0);
        return WSAStartup(version, &wsaData);
#else
        return 0;
#endif
    }

    int socket_cleanup()
    {
#ifdef _WIN32
        return WSACleanup();
#else
        return 0;
#endif
    }

    const char* hostname_to_ip(const char* hostname)
    {
        struct hostent* hp = ::gethostbyname(hostname);
        if(!hp) return NULL;
        in_addr addr;
        memcpy(&addr, hp->h_addr, sizeof(addr));
        return inet_ntoa(addr);
    }

    void sleep(unsigned int millisecs)
    {
#ifdef _WIN32
        Sleep(millisecs);
#else
        usleep(millisecs * 1000);
#endif
    }

}
