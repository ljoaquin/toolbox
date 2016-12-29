#include "client_network.h"
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/socket.h>
#endif

#include "socket_util.h"
#include "string_formater.h"

namespace toolbox
{

    packet::packet()
    {
        this->buf = new unsigned char[DEFAULT_SIZE];
        this->size = DEFAULT_SIZE;
        memset(this->buf, 0, this->size);
    }

    packet::~packet()
    {
        if(buf)
        {
            delete[] this->buf;
        }
    }

    packet::packet(int buf_size)
    {
        this->buf = new unsigned char[buf_size];
        this->size = buf_size;
        memset(this->buf, 0, this->size);
    }

    void packet::resize()
    {
        int new_size = this->size * 2;
        this->resize(new_size);
    }

    void packet::resize(int new_size)
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

    client_network::client_network()
    {
        m_connected = false;
        m_socket.fd = -1;
        m_ip = "0.0.0.0";
        m_port = 0;
    }

    client_network::~client_network()
    {
    }

    template<typename... Targs>
    void client_network::log(std::string fmt, Targs... args)
    {
        std::string msg = toolbox::string_formater::format(fmt, args...);
        puts(("[client_network]" + msg).c_str());
    }

    bool client_network::connect(std::string ip, unsigned short port)
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

        m_network_thread = std::thread(std::bind(&client_network::network_loop, this));

        return true;
    }

    bool client_network::reconnect()
    {
        this->disconnect();
        return this->connect(m_ip, m_port);
    }

    bool client_network::disconnect()
    {
        if(!m_connected)
        {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(m_socket.m);
            int r = toolbox::close_socket(m_socket.fd);
            if(r != 0)
            {
                return false;
            }
        }
        m_connected = false;

        if(m_network_thread.joinable())
        {
            m_network_thread.join();
        }
        
        return true;
    }

    void client_network::network_loop()
    {
        while(m_connected)
        {
            std::lock_guard<std::mutex> lock(m_socket.m);

            if(toolbox::select_for_read(m_socket.fd, 1))
            {
                std::lock_guard<std::mutex> lock(m_recv_queue.m);

                packet* p = new packet();
                int r = -1;
                int num_recv = 0;
                while((r = ::recv(m_socket.fd, (char*)p->buf + num_recv, p->size - num_recv, 0)) > 0)
                {
                    // log("recv:%d, num_recv:%0", r, num_recv);

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

                    log("m_recv_queue.push p->size:%0", p->size);
                }
                else if(r == 0)
                {
                    m_connected = false;
                    log("[error] svr close, recv:%0", r);
                }
                else
                {
                    log("[error] recv:%0", r);
                }
            }

            if(toolbox::select_for_write(m_socket.fd, 1))
            {
                if(m_send_queue.q.size() > 0)
                {
                    std::lock_guard<std::mutex> lock(m_send_queue.m);

                    packet* p = m_send_queue.q.front();
                    int r = ::send(m_socket.fd, (char*)p->buf, p->size, 0);
                    if(r == p->size)
                    {
                        m_send_queue.q.pop();
                        delete p;

                        log("send:%0, p->size:%1", r, p->size);
                    }
                    else if(r == 0)
                    {
                        m_connected = false;
                        log("[error] svr close, send:%0", r);
                    }
                    else
                    {
                        log("[error] send:%0", r);
                    }
                }
            }

            toolbox::sleep(1);
        }
        log("thread end");
    }

    void client_network::send(unsigned char* data, unsigned int len)
    {
        packet* p = new packet(len);
        memcpy(p->buf, data, len);
        
        std::lock_guard<std::mutex> lock(m_send_queue.m);
        m_send_queue.q.push(p);
    }

    packet* client_network::recv()
    {
        if(m_recv_queue.q.size() == 0)
        {
            return NULL;
        }

        std::lock_guard<std::mutex> lock(m_recv_queue.m);
        packet* p = m_recv_queue.q.front();
        return p;
    }

    bool client_network::pop_recv()
    {
        if(m_recv_queue.q.size() == 0)
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(m_recv_queue.m);
        packet* p = m_recv_queue.q.front();
        m_recv_queue.q.pop();
        delete p;

        return true;
    }

}
