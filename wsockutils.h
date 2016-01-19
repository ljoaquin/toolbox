#ifndef __WINSOCK_TOOLS_H__
#define __WINSOCK_TOOLS_H__

/*
	just making win32 socket api unix style,
	not in toolbox namespace,
	thus can be used seperatly
*/

#include <WinSock2.h>

typedef int socklen_t;

int winsock_init();
int winsock_cleanup();

int inet_aton(const char* ip, in_addr* buf);

int close(int sockfd);

#endif // __WINSOCK_TOOLS_H__