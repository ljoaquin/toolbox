#include "client_network.h"
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#endif

#include "socket_util.h"
#include "debug_util.h"

Packet::Packet()
{
    this->buf = new unsigned char[DEFAULT_SIZE];
    this->size = DEFAULT_SIZE;
    memset(this->buf, 0, this->size);
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
    memset(this->buf, 0, this->size);
}

void Packet::resize()
{
    int new_size = this->size * 2;
    this->resize(new_size);
}

void Packet::resize(int new_size)
{
    if(this->buf)
    {
        unsigned char* new_buf = new unsigned char[new_size];
        memset(new_buf, 0, new_size);

        memcpy(new_buf, this->buf, this->size < new_size ? this->size : new_size);

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

ClientNetwork::~ClientNetwork()
{
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

    m_network_thread = std::thread(std::bind(&ClientNetwork::networkLoop, this));

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

    std::lock_guard<std::mutex> lock(m_socket.m);
    int r = toolbox::close_socket(m_socket.fd);
    if(r != 0)
    {
        return false;
    }

    m_connected = false;

    if(m_network_thread.joinable())
    {
        m_network_thread.join();
    }
    
    return true;
}

void ClientNetwork::networkLoop()
{
    while(m_connected)
    {
        std::lock_guard<std::mutex> lock(m_socket.m);

        if(toolbox::select_for_read(m_socket.fd, 1))
        {
            std::lock_guard<std::mutex> lock(m_recv_queue.m);

            Packet* p = new Packet();
            int r = -1;
            int num_recv = 0;
            while((r = ::recv(m_socket.fd, (char*)p->buf + num_recv, p->size - num_recv, 0)) > 0)
            {
                // toolbox::log("recv:%d, num_recv:%d", r, num_recv);

                num_recv += r;
                if(num_recv < p->size)
                {
                    break;
                }

                p->resize();
            }

            if(num_recv > 0)
            {
                p->resize(num_recv);
                m_recv_queue.q.push(p);

                toolbox::log("m_recv_queue.push p->size:%d", p->size);
            }
            else if(r == 0)
            {
                m_connected = false;
                toolbox::log("[error] svr close, recv:%d", r);
            }
            else
            {
                toolbox::log("[error] recv:%d", r);
            }
        }

        if(toolbox::select_for_write(m_socket.fd, 1))
        {
            if(m_send_queue.q.size() > 0)
            {
                std::lock_guard<std::mutex> lock(m_send_queue.m);

                Packet* p = m_send_queue.q.front();
                int r = ::send(m_socket.fd, (char*)p->buf, p->size, 0);
                if(r == p->size)
                {
                    m_send_queue.q.pop();
                    delete p;

                    toolbox::log("send:%d, p->size:%d", r, p->size);
                }
                else if(r == 0)
                {
                    m_connected = false;
                    toolbox::log("[error] svr close, send:%d", r);
                }
                else
                {
                    toolbox::log("[error] send:%d", r);
                }
            }
        }

        toolbox::sleep(1);
    }
}

void ClientNetwork::send(unsigned char* data, unsigned int len)
{
    Packet* p = new Packet(len);
    memcpy(p->buf, data, len);
    
    std::lock_guard<std::mutex> lock(m_send_queue.m);
    m_send_queue.q.push(p);
}

Packet* ClientNetwork::recv()
{
    if(m_recv_queue.q.size() == 0)
    {
        return NULL;
    }

    std::lock_guard<std::mutex> lock(m_recv_queue.m);
    Packet* p = m_recv_queue.q.front();
    return p;
}

bool ClientNetwork::pop_recv()
{
    if(m_recv_queue.q.size() == 0)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_recv_queue.m);
    Packet* p = m_recv_queue.q.front();
    m_recv_queue.q.pop();
    delete p;

    return true;
}
