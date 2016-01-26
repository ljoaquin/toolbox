#ifndef __TOOLBOX_NETWORK_H__
#define __TOOLBOX_NETWORK_H__

#include "TcpSocket.h"
#include "packet.h"

#include <queue>
#include <thread>
#include <mutex>

namespace toolbox
{

    class Network
    {
    public:
        
        Network();
        virtual ~Network();
    
        void Send(packet_t* packet);
        void Recv(packet_t* packet);

        void SetEnabled(bool b);
        bool IsEnabled();

        void Start();
        void Stop();
        
        // debug, to be deleted
        static void TestStart();
        static void TestStop();
        static void TestRecv();
        static void TestSend(const char* msg);
        
    private:
        
        void NetLoop();
        bool SendPacket();
        bool RecvPakcet();

    private:
        
        unsigned int m_nLoopInterval;
        
        toolbox::TcpSocket m_socket;

        std::thread m_netThread;

        bool m_bEnabled;
        std::mutex m_mutexEnabled;
        
        std::queue<packet_t*> m_qSend;
        std::mutex m_mutexSend;
        
        std::queue<packet_t*> m_qRecv;
        std::mutex m_mutexRecv;
    };

}

#endif // __TOOLBOX_NETWORK_H__