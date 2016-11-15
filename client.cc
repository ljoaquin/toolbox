#include <stdio.h>

#include "socketutil.h"

int main()
{
    toolbox::socket_init();

    run_client();

    toolbox::socket_cleanup();

    return 0;
}

void run_client()
{
    
}
