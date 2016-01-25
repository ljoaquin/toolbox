#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include <string>

namespace toolbox
{

	class TcpSocket
	{
	public:
		TcpSocket();
		virtual ~TcpSocket();

		// for sever
		bool Listen(unsigned short port, int backlog = 1);
		bool Accept(TcpSocket& client, char* ip_buf);
		// for client
		bool Connect(const std::string& ip, unsigned short port);

		bool SetNonBlocking();

		int Send(unsigned char* data, unsigned int len);
		int Recv(unsigned char* buf, unsigned int len);

		bool CanSend();
		bool CanRecv();

	// 	int GetError(){ return m_error; };

	private:
		void Error();

	private:
		int m_sockfd;
		// int m_error;
	};

}

#endif // __TCP_SOCKET_H__
