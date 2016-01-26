#include "Network.h"

#include <stdio.h> // debug
#include "socketutils.h"

namespace toolbox
{
    
    static Network s_net;
    
    void Network::TestStart()
    {
        s_net.Start();
    }
    
    void Network::TestStop()
    {
        s_net.Stop();
    }
    
    void Network::TestRecv()
    {
        printf("%s: recv queue %ld\n", __FUNCTION__, s_net.m_qRecv.size());
        
        packet_t packet;
        packet_init(&packet);
        s_net.Recv(&packet);
        
        if(packet.len)
            printf("[%d]%s\n", packet.len, packet.buf);
        
        packet_destory(&packet);
    }
    
    void Network::TestSend(const char* msg)
    {
        packet_t packet;
        packet_init(&packet);
        
        packet_set(&packet, (void*)msg, strlen(msg));
        
        s_net.Send(&packet);
        
        packet_destory(&packet);
    }
    
    static void clearPacketQueue(std::queue<packet_t*>& queue)
    {
        while(queue.size())
        {
            packet_t* p = queue.front();
            queue.pop();
            packet_destory(p);
            delete p;
        }
    }
    
    static void popPacketQueue(std::queue<packet_t*>& queue)
    {
        if(queue.size())
        {
            packet_t* p = queue.front();
            queue.pop();
            packet_destory(p);
            delete p;
        }
    }

	Network::Network()
	{
		m_nLoopInterval = 100;
        m_bEnabled = false;
	}

	Network::~Network()
	{
        if(IsEnabled())
        {
            Stop();
        }
        
        clearPacketQueue(m_qSend);
        clearPacketQueue(m_qRecv);
	}
    
	void Network::SetEnabled(bool b)
	{
		std::lock_guard<std::mutex> lock(m_mutexEnabled);
		m_bEnabled = b;
	}

	bool Network::IsEnabled()
	{
		std::lock_guard<std::mutex> lock(m_mutexEnabled);
		return m_bEnabled;
	}

	void Network::Stop()
	{
		SetEnabled(false);
        
        if(m_netThread.joinable())
            m_netThread.join();
        
        m_socket.Close();
	}

    void Network::Start()
	{
        // debug
        m_socket.Init();
        m_socket.Connect("127.0.0.1", 9999);
        m_socket.SetNonBlocking();
		
		std::thread netThread(&Network::NetLoop, this);
		m_netThread = std::move(netThread);
		SetEnabled(true);
	}
    
    bool Network::SendPacket()
    {
        std::lock_guard<std::mutex> lock(m_mutexSend);
        while(m_qSend.size())
        {
            packet_t* p = m_qSend.front();

            int nsent = m_socket.Send(p->buf, p->len);
            if(nsent != p->len)
            {
                if(is_blocking_errno())
                {
                    return true;
                }
                else
                {
                    perror(__FUNCTION__);
                    return false;
                }
            }
            
            popPacketQueue(m_qSend);
        }
        return true;
    }
    
    bool Network::RecvPakcet()
    {
        packet_t* packet = new packet_t();
        packet_init(packet);
        
        int nread = m_socket.Recv(packet->buf, packet->capacity);
        if(nread <= 0)
        {
            if(is_blocking_errno())
            {
                return true;
            }
            else
            {
                perror(__FUNCTION__);
                return false;
            }
        }
        packet->len = nread;
        {
            std::lock_guard<std::mutex> lock(m_mutexRecv);
            m_qRecv.push(packet);
        }
        return true;
    }

    void Network::NetLoop()
	{
		while(IsEnabled())
		{
			bool b = false;
            
            b = RecvPakcet();
            if(!b)
            {
                SetEnabled(false);
                printf("RecvPacket error, stop net thread\n");
                break;
            }

            b = SendPacket();
            if(!b)
            {
                SetEnabled(false);
                printf("SendPacket error, stop net thread\n");
                break;
            }

			usleep(1000 * m_nLoopInterval);
		}
        
		printf("netloop exit\n"); // debug
	}

	void Network::Send(packet_t* packet)
	{
        std::lock_guard<std::mutex> lock(m_mutexSend);
        
        packet_t* p = new packet_t();
        packet_init(p);
        packet_set(p, packet->buf, packet->len);
        
        m_qSend.push(p);
	}
    
    void Network::Recv(packet_t* packet)
    {
        std::lock_guard<std::mutex> lock(m_mutexRecv);
        if(m_qRecv.size())
        {
            packet_t* p = m_qRecv.front();
            packet_set(packet, p->buf, p->len);
            
            popPacketQueue(m_qRecv);
        }
    }

}