#include <stdio.h>
#include <string.h>

#include "client_network.h"

int main(int argc, char const *argv[])
{
    puts("client_network test");

    toolbox::client_network cn;
    cn.connect("127.0.0.1", 12345);
    getchar();
    const char* msg = "helloworld";
    cn.send((unsigned char*)msg, strlen(msg));
    getchar();
    msg = "helloworldhelloworld";
    cn.send((unsigned char*)msg, strlen(msg));
    getchar();

    toolbox::packet* p = NULL;
    while((p = cn.recv()) != NULL)
    {
        printf("recv buf:%s, size:%d\n", p->buf, p->size);
        cn.pop_recv();
    }
    getchar();
    msg = "yoyoyo";
    cn.send((unsigned char*)msg, strlen(msg));

    getchar();
    cn.disconnect();

    return 0;
}
