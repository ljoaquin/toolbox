#include "UdpSocket.h"
#include "socketutils.h"

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace toolbox
{

	UdpSocket::UdpSocket()
	{
#ifdef _WIN32
		winsock_init();
#endif
	}

	UdpSocket::UdpSocket(short local_port)
	{
#ifdef _WIN32
		winsock_init();
#endif
		init(local_port);
	}

	UdpSocket::UdpSocket(short local_port, const std::string& ip, short port)
	{
#ifdef _WIN32
		winsock_init();
#endif
		init(local_port);
		setRemote(ip, port);
	}

	UdpSocket::~UdpSocket()
	{
		closeSocket();
#ifdef _WIN32
		winsock_cleanup();
#endif
	}

	void UdpSocket::closeSocket()
	{
		close(m_sockfd);
	}

	bool UdpSocket::init(short local_port)
	{
		int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1)
		{
			m_eError = ERR::INVALID_SOCKFD;
			closeSocket();
			return false;
		}

		m_sockfd = sockfd;

		int r;
		r = set_reuseaddr(m_sockfd);
		if (r)
		{
			m_eError = ERR::SETSOCKOPT_ERROR;
			closeSocket();
			return false;
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		inet_aton("0.0.0.0", &addr.sin_addr);
		addr.sin_port = htons(9999);

		r = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
		if (r)
		{
			m_eError = ERR::BIND_ERROR;
			closeSocket();
			return false;
		}

		return true;
	}

	void UdpSocket::setRemote(const std::string& ip, short port)
	{
		m_remoteAddr.sin_family = AF_INET;
		inet_aton(ip.c_str(), &m_remoteAddr.sin_addr);
		m_remoteAddr.sin_port = htons(port);
	}

	bool UdpSocket::sendPacket(
		unsigned char* data, unsigned int len)
	{
		if (!data)
		{
			m_eError = ERR::INVALID_ARG;
			return false;
		}

		int nsent = sendto(m_sockfd, (const char*)data, len, 0, (struct sockaddr*)&m_remoteAddr, sizeof(m_remoteAddr));

		if (nsent == 0 || nsent == -1)
		{
			m_eError = ERR::SEND_NONE;
			return false;
		}

		return true;
	}

	bool UdpSocket::recvPacket(unsigned char* buf, unsigned int buf_len)
	{
		if (!buf)
		{
			m_eError = ERR::INVALID_ARG;
			return false;
		}

		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);

		int nread = recvfrom(m_sockfd, (char*)buf, buf_len, 0, (sockaddr*)&addr, &addr_len);
		if (nread == 0 || nread == -1)
		{
			m_eError = ERR::RECV_NONE;
			return false;
		}
		return true;
	}

	bool UdpSocket::waitForPacket(int millisecs)
	{
		if (!select_for_read(m_sockfd, millisecs))
		{
			m_eError = ERR::SELECT_READ_ERROR;
			return false;
		}
		return true;
	}

	void UdpSocket::debugPrint()
	{
		printf("[UdpSocket] err: %d\n", m_eError);
	}

}