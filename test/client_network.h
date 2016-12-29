#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <queue>
#include <mutex>
#include <string>
#include <thread>

namespace toolbox
{

    class packet
    {
    public:
        static const int DEFAULT_SIZE = 16;

    public:
        packet();
        ~packet();

        packet(int buf_size);

        // double the size
        void resize();
        // if new_size is less, data will be truncated
        void resize(int new_size);

    public:
        unsigned char* buf;
        int size;
    };

    struct packet_queue_t
    {
        std::queue<packet*> q;
        std::mutex m;
    };

    struct socket_t
    {
        int         fd;
        std::mutex  m;
    };

    class client_network
    {
    public:
        client_network();
        ~client_network();

        bool connect(std::string ip, unsigned short port);
        bool disconnect();
        bool reconnect();

        void send(unsigned char* data, unsigned int len);
        // get the front packet from recv queue
        packet* recv();
        // recv queue front dequeue, and delete the packet
        bool pop_recv();

    private:
        void network_loop();

        template<typename... Targs>
        static void log(std::string fmt, Targs... args);

    private:
        packet_queue_t m_send_queue;
        packet_queue_t m_recv_queue;

        bool            m_connected;
        socket_t        m_socket;

        std::string     m_ip;
        unsigned short  m_port;

        std::thread     m_network_thread;
    };
}

#endif // __CLIENT_NETWORK_H__
