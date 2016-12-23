#include <stdio.h>
#include <WinSock2.h>

#include "socketutil.h"
#include "ClientNetwork.h"

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

        gets(input_buf);

        bool break_loop = false;
        switch(input_buf[0])
        {
            case 'q':
                puts("quit");
                break_loop = true;
                break;
            default:
                puts(input_buf);
                break;
        }

        if(break_loop)
        {
            break;
        }
    }
    /*int sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    int r = toolbox::connect(sockfd, "127.0.0.1", 12345);
*/
}
