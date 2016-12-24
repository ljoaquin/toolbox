#include <queue>
#include <mutex>
#include <string>
#include <thread>

class Packet
{
public:
    static const int DEFAULT_SIZE = 16;

public:
    Packet();
    ~Packet();

    Packet(int buf_size);

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
    std::queue<Packet*> q;
    std::mutex m;
};

struct socket_t
{
    int         fd;
    std::mutex  m;
};

class ClientNetwork
{
public:
    ClientNetwork();
    ~ClientNetwork();

public:
    bool connect(std::string ip, unsigned short port);
    bool disconnect();
    bool reconnect();

    void send(unsigned char* data, unsigned int len);
    // get the front packet from recv queue
    Packet* recv();
    // recv queue front dequeue, and delete the packet
    bool pop_recv();

private:
    void networkLoop();

private:
    packet_queue_t m_send_queue;
    packet_queue_t m_recv_queue;

    bool            m_connected;
    socket_t        m_socket;

    std::string     m_ip;
    unsigned short  m_port;

    std::thread     m_network_thread;
};
