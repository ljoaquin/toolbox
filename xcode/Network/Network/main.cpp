#include <stdio.h>
#include "Network.h"
#include "socketutils.h"

using namespace toolbox;

int main(int argc, const char * argv[]) {
    printf("hello world\n");
    
    const int buf_len = 1024;
    char buf[buf_len];
    
    while(1)
    {
        printf("net> ");
        memset(buf, 0, buf_len);
        
        gets(buf);
//        print_bytes(buf, strlen(buf));
        
        if(strcmp(buf, "start") == 0)
            Network::TestStart();
        else if(strcmp(buf, "stop") == 0)
            Network::TestStop();
        else if(buf[0] == 's')
            Network::TestSend(buf + 2);
        else if(buf[0] == 'r')
            Network::TestRecv();
        else if(buf[0] == 'q')
            break;
        
    }
    
    printf("safe exit\n");
    return 0;
}
