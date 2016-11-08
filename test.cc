#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#else // unix header
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "socketutil.h"

void run_server();


int main(int argc, char const *argv[])
{
    puts("test server");

    toolbox::socket_init();

    run_server();

    //printf("%s\n", toolbox::get_ipaddress("google.com"));

    toolbox::socket_cleanup();


    return 0;
}

void run_server()
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    int r = toolbox::set_reuseaddr(sockfd);

    const short port = 12345;
    r = toolbox::bind(sockfd, port);
    r = ::listen(sockfd, 5);

    // r = toolbox::set_nonblocking(sockfd);

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    const int buf_max = 1024;
    char buf[buf_max];

    while(true)
    {
        int clientfd = ::accept(sockfd, (sockaddr*)&addr, &addrlen);

        printf("clientfd:%d, ip:%s\n", clientfd, inet_ntoa(addr.sin_addr));

        memset(buf, 0, buf_max);
        int n = ::recv(clientfd, buf, buf_max, 0);
        printf("recv");
        printf("%d, %s\n", n, buf);

        n = ::send(clientfd, buf, strlen(buf), 0);
        printf("send");
        printf("%d, %s\n", n, buf);

        toolbox::close_socket(clientfd);

        if(buf[0] == 'Q')
        {
            break;
        }
    }

    toolbox::close_socket(sockfd);
    puts("end");

}