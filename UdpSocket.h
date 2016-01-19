#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include <string>
#include "socketutils.h"

namespace toolbox
{

	class UdpSocket
	{
	public:
		enum class ERR
		{
			NONE				= 0,
			INVALID_ARG,
			INVALID_SOCKFD,
			SETSOCKOPT_ERROR,
			BIND_ERROR,
			SEND_NONE,
			RECV_NONE,
			SELECT_READ_ERROR,
		};

	public:
		UdpSocket();
		UdpSocket(short local_port);
		UdpSocket(short local_port, const std::string& ip, short port);
		virtual ~UdpSocket();

		bool init(short local_port);
		void setRemote(const std::string& ip, short port);

		bool sendPacket(unsigned char* data, unsigned int len);
		bool recvPacket(unsigned char* buf, unsigned int buf_len);
		bool waitForRecv(int millisecs);

		void closeSocket();

		ERR getError(){ return m_eError; };
		void debugPrint();

	private:
		int m_sockfd;
		struct sockaddr_in m_remoteAddr;

		ERR m_eError;
	};

}

#endif // __UDP_SOCKET_H__