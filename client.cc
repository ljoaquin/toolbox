#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "socket_util.h"

void run_client();

int main()
{
    toolbox::socket_init();

    run_client();

    toolbox::socket_cleanup();

    return 0;
}

void run_client()
{
    while(true)
    {
        printf("input>");
        char input_buf[128];
        memset(input_buf, 0, 128);

        gets(input_buf);
        puts(input_buf);

        bool break_loop = false;
        switch(input_buf[0])
        {
            case 'q':
            {
                puts("quit");
                break_loop = true;
                break;
            }
            // recv buffer too small
            case 'a':
            {
                int sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
                int r = toolbox::connect(sockfd, "127.0.0.1", 12345);
                printf("connect:%d\n", r);

                getchar();

                int len = strlen(input_buf + 2);
                r = ::send(sockfd, input_buf + 2, len, 0);
                printf("send:%d, msg:%s, len:%d\n", r, input_buf + 2, len);

                getchar();
                
                const int buf_size = 10;
                char recv_buf[buf_size + 1];
                memset(recv_buf, 0, buf_size + 1);
                r = ::recv(sockfd, recv_buf, buf_size, 0);
                printf("recv:%d, msg:%s, len:%d\n", r, recv_buf, strlen(recv_buf));

                ::close(sockfd);

                break;
            }
            default:
                break;
        }

        if(break_loop)
        {
            break;
        }
    }

}
