#include "TcpSocket.h"
#include "socketutils.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

namespace toolbox
{

    TcpSocket::TcpSocket()
    {
//        m_error = 0;
    }

	bool TcpSocket::Init()
	{
		m_sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
        if(m_sockfd == -1)
        {
            Error();
            return false;
        }

        // may cause sigpipe, hard to detect
        int opt = 1;
        int optlen = sizeof(opt);
        if(::setsockopt(m_sockfd, SOL_SOCKET, SO_NOSIGPIPE, &opt, optlen))
        {
            Error();
            return false;
        }
        
        return true;
	}

	TcpSocket::~TcpSocket()
	{
		if(m_sockfd != -1)
            Close();
	}

	void TcpSocket::Error()
	{
		// m_error = errno;
		perror("error");
	}

    void TcpSocket::Close()
    {
        if(::close(m_sockfd))
            Error();
        else
            m_sockfd = -1;
    }

	bool TcpSocket::Listen(unsigned short port, int backlog)
	{
        if(toolbox::set_reuseaddr(m_sockfd))
        {
            Error();
            return false;
        }

		if(toolbox::bind(m_sockfd, port))
		{
			Error();
			return false;
		}

		if(::listen(m_sockfd, backlog))
		{
			Error();
			return false;
		}

		return true;
	}

	bool TcpSocket::Accept(TcpSocket& client, char* ip_buf)
	{
		int r;

		struct sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);
		r = ::accept(m_sockfd, (struct sockaddr*)&addr, &addrlen);
		if(r == -1)
		{
			Error();
			return false;
		}

		client.m_sockfd = r;
		strcpy(ip_buf, inet_ntoa(addr.sin_addr));
		return true;
	}

	bool TcpSocket::Connect(const std::string& ip, unsigned short port)
	{
		if(toolbox::connect(m_sockfd, ip.c_str(), port))
		{
			Error();
			return false;
		}

		return true;
	}

	bool TcpSocket::SetNonBlocking()
	{
		int flags = ::fcntl(m_sockfd, F_GETFL, 0);
		if(flags == -1)
		{
			Error();
			return false;
		}

		flags = flags | O_NONBLOCK;
		if(::fcntl(m_sockfd, F_SETFL, flags))
		{
			Error();
			return false;
		}
		
		return true;
	}

    bool is_blocking_errno()
    {
        return (errno == 0 || errno == EAGAIN || errno == EWOULDBLOCK);
    }

	int TcpSocket::Send(unsigned char* data, unsigned int len)
	{
		int nsent = ::write(m_sockfd, data, len);
		// int sent = ::send(m_sockfd, data, len, MSG_NOSIGNAL);
		if(nsent < 0 && !is_blocking_errno())
		{
			Error();
		}
		return nsent;
	}

	int TcpSocket::Recv(unsigned char* buf, unsigned int len)
	{
		int nread = ::read(m_sockfd, buf, len);
		if(nread < 0 && !is_blocking_errno())
		{
			Error();
		}
		return nread;
	}

	bool TcpSocket::CanSend()
	{
		return select_for_write(m_sockfd, 0);
	}

	bool TcpSocket::CanRecv()
	{
		return select_for_read(m_sockfd, 0);
	}

}
