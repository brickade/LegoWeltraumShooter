#ifndef _NETWORKHANDLER_H_
#define _NETWORKHANDLER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/TheBrick.h>

// Declare namespace Game
namespace Game
{
    /// @brief HeaderPacket, used to tell which Packet we got
    ///
    struct HeadPacket
    {
        int Type;
    };

    /// @brief Packet used when Data is received, this checks the Header
    ///
    struct ReceivePacket
    {
        HeadPacket Head;
        char* Buffer;
    };

    /// @brief Packet used to detect who left
    ///
    struct LeftPacket
    {
        HeadPacket Head;
        int Who;
    };

    /// @brief Base Packet for Inputs
    ///
    struct InputBasePacket
    {
        HeadPacket Head;
        int Who;
        int Input;
    };

    /// @brief Base Packet for Movement Input
    ///
    struct MovePacket
    {
        InputBasePacket InputBase;
        PuRe_Vector2F Move;
    };

    /// @brief Base Packet for Movement Input
    ///
    struct ThrustPacket
    {
        InputBasePacket InputBase;
        float Thrust;
    };


    class CNetworkHandler
    {
    public:
        std::string m_IP;
        std::string m_Port;
        bool m_Host;
        int m_NetworkState;

        private:
            PuRe_Socket* m_pSocket;
        public:
            CNetworkHandler();
            ~CNetworkHandler();
        public:
            void Connect();
            void Update(PuRe_IInput* a_pInput);
            int GetState();
            long Receive(char* a_pBuffer, int a_Size, SOCKADDR_IN* a_pSender);
            void Send(char* a_pBuffer, int a_Size, SOCKADDR_IN a_Receiver);
            void SendHost(char* a_pBuffer, int a_Size);
    };
}
#endif /* _NETWORKHANDLER_H_ */