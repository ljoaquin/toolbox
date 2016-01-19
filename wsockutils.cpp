#include "wsockutils.h"
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

int winsock_init()
{
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 0);
	return WSAStartup(version, &wsaData);
}

int winsock_cleanup()
{
	return WSACleanup();
}

int inet_aton(const char* ip, in_addr* buf)
{
	return inet_pton(AF_INET, ip, buf); // inet_pton is in Ws2tcpip.h
}

int close(int sockfd)
{
	return closesocket(sockfd);
}