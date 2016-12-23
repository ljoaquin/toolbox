#include "ClientNetwork.h"

#include <WinSock2.h>
#include <thread>

#include "socketutil.h"

Packet::Packet()
{
    this->buf = new unsigned char[DEFAULT_SIZE];
    this->size = DEFAULT_SIZE;
}

Packet::~Packet()
{
    if(buf)
    {
        delete[] this->buf;
    }
}

Packet::Packet(int buf_size)
{
    this->buf = new unsigned char[buf_size];
    this->size = buf_size;
}

void Packet::resize()
{
    int new_size = this->size * 2;
    this->resize(new_size);
}

void Packet::resize(int new_size)
{
    if(this->buf && new_size > this->size)
    {
        unsigned char* new_buf = new unsigned char[new_size];
        memset(new_buf, 0, new_size);

        memcpy(new_buf, this->buf, this->size);

        delete[] this->buf;

        this->buf = new_buf;
        this->size = new_size;
    }
}

ClientNetwork::ClientNetwork()
{
    m_connected = false;
    m_socket.fd = -1;
    m_ip = "0.0.0.0";
    m_port = 0;
}

bool ClientNetwork::connect(std::string ip, unsigned short port)
{
    if(m_connected)
    {
        return false;
    }

    m_ip = ip;
    m_port = port;

    m_socket.fd = ::socket(PF_INET, SOCK_STREAM, 0);
    if(m_socket.fd == -1)
    {
        return false;
    }

    int r = toolbox::connect(m_socket.fd, ip.c_str(), port);
    if(r != 0)
    {
        return false;
    }

    m_connected = true;

    std::thread network_thread(std::bind(&ClientNetwork::networkLoop, this));

    return true;
}

bool ClientNetwork::reconnect()
{
    this->disconnect();
    return this->connect(m_ip, m_port);
}

bool ClientNetwork::disconnect()
{
    if(!m_connected)
    {
        return false;
    }

    int r = toolbox::close_socket(m_socket.fd);
    if(r != 0)
    {
        return false;
    }

    m_connected = false;
    return true;
}

void ClientNetwork::networkLoop()
{
    while(m_connected)
    {
        if(toolbox::select_for_read(m_socket.fd, 1))
        {
            std::lock_guard<std::mutex> lock(m_recv_queue.m);

            int num_to_recv = ::recv(m_socket.fd, NULL, 0, MSG_PEEK);
            if(num_to_recv > 0)
            {
                Packet p(num_to_recv);
                int num_recv = ::recv(m_socket.fd, (char*)p.buf, p.size, 0);
                m_recv_queue.q.push(p);

                printf("[recv]%s\n", p.buf);
            }
            if(num_to_recv == -1)
            {
                puts("[error] recv -1");
            }
        }

        if(toolbox::select_for_write(m_socket.fd, 1) && m_send_queue.q.size() > 0)
        {
            std::lock_guard<std::mutex> lock(m_send_queue.m);

            Packet p = m_send_queue.q.front();
            int num_send = ::send(m_socket.fd, (char*)p.buf, p.size, 0);
            if(num_send == p.size)
            {
                m_send_queue.q.pop();
            }
            if(num_send == -1)
            {
                puts("[error] send -1");
            }
        }
    }
}

//bool ClientNetwork::send(Packet* p)
//{
//    
//}
//
//bool ClientNetwork::recv(Packet* p)
//{
//
//}