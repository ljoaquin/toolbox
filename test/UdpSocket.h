#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include <string>
#include <functional>
#include "socketutils.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif

namespace toolbox
{

	/*================================

	@description:
	udp util, will bind local_port for receiving

	@usage:
	toolbox::UdpSocket mysocket(local_port);
	mysocket setRemote(remote_ip, remote_port);
	mysocket.send(str, sizeof(str));
	mysocket.recv(on_recv);
	mysocket.closeSocket();

	================================*/

	//typedef void(*recv_callback)(unsigned char* buf, unsigned int nread);
	typedef std::function<void(unsigned char* buf, unsigned int nread)> recv_callback;

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
		// default 1024 bytes
		static void setRecvBufLen(unsigned int);

		UdpSocket();
		UdpSocket(unsigned short local_port);
		UdpSocket(unsigned short local_port, const std::string& ip, unsigned short port);
		virtual ~UdpSocket();

		bool init(unsigned short local_port);
		void setRemote(const std::string& ip, unsigned short port);

		bool sendPacket(unsigned char* data, unsigned int len);

		// @cb: only when success, cb will be called
		// callback should copy the buffer data
		bool recvPacket(recv_callback cb);

		// @millisecs: timeout, if packet available, return immediately
		bool isRecvReady(int millisecs = 0);

		void closeSocket();

		ERR getError(){ return m_eError; };
		void debugPrint();

	private:
		int m_sockfd;
		struct sockaddr_in m_remoteAddr;

		static unsigned int m_nRecvBufLen;

		ERR m_eError;
	};

}

#endif // __UDP_SOCKET_H__