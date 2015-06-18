#ifndef _NETWORKHANDLER_H_
#define _NETWORKHANDLER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/TheBrick.h>
#include "include/INIReader.h"

#include <mutex>

// Declare namespace Game
namespace sba
{
    enum { MaxPlayers = 10, Delay = 10, BroadcastPort =13370, MaxName=12, MaxLength=15 };

    enum EPacket { Broadcast,Join, Left, IAm, CJoin, Start, STick, CTick };

    /// @brief Struct which handles the Input one Player does
    ///
    struct SInputData
    {
        unsigned char Player;
        bool Shoot;
        bool Thrust;
        unsigned char Spin;
        unsigned char MoveX;
        unsigned char MoveY;

    };
    /// @brief HeaderPacket, used to tell which Packet we got
    ///
    struct SHeadPacket
    {
        EPacket Type;
    };
    /// @brief Packet send over broadcast
    ///
    struct SBroadcastPacket
    {
        SHeadPacket Head;
        char Name[MaxName];
        char IP[MaxLength];
        char Port[MaxLength];
    };

    /// @brief Packet used when Data is received, this checks the Header
    ///
    struct SReceivePacket
    {
        SHeadPacket Head;
        char* Buffer;
    };

    /// @brief Packet used to detect who left
    ///
    struct SLeftPacket
    {
        SHeadPacket Head;
        int Who;
    };

    /// @brief Packet for Inputs
    ///
    struct SInputPacket
    {
        SHeadPacket Head;
        int Frame;
        SInputData Input;
    };

    /// @brief Packet for all Inputs
    ///
    struct SInputsPacket
    {
        SHeadPacket Head;
        int Players;
        int Frame;
        SInputData Input[MaxPlayers]; //we send 
    };

    /// @brief What to Update
    ///
    enum EUpdate { IP,Port,Name};

    class CNetworkHandler
    {
    public:
        std::string m_IP;
        std::string m_Port;
        std::string m_Name;
        std::mutex m_Mutex;

    private:
        PuRe_Socket* m_pSocket;
        PuRe_Socket* m_pBSocket;
        bool m_Host;
        bool m_Connected;
    public:
        CNetworkHandler();
        ~CNetworkHandler();
    public:
        /// @brief Sets whether the calls are blocking or not
        ///
        void SetBlockMode(bool a_Block);
        /// @brief If he is connected or not
        ///
        bool IsConnected();
        /// @brief Returns if he is hosting or not
        ///
        bool GetHost();
        /// @brief Create a Broadcast Socket
        ///
        void CreateBroadcast(bool a_Broadcast,int a_Port);
        /// @brief Broadcast Data
        ///
        void Broadcast(char* a_pBuffer, int a_Size);
        /// @brief Disconnect
        ///
        void Disconnect();
        /// @brief Delete the Broadcast Socket
        ///
        void DeleteBroadcast();
        /// @brief Listen for Clients
        ///
        bool Listen();
        /// @brief Accept Clients
        ///
        SOCKET Accept();
        /// @brief Initialite Socket
        ///
        bool Connect(bool a_Host);
        /// @brief Update Data
        ///
        void Update(PuRe_IInput* a_pInput, EUpdate a_What);
        /// @brief Return this networkstate
        ///
        int GetState();
        /// @brief Return the Broadcast Socket
        ///
        int GetBroadcast();
        /// @brief Return this Socket
        ///
        int GetSocket();
        /// @brief Receive Data 
        ///
        long Receive(char* a_pBuffer, int a_Size, SOCKET a_pSender);
        /// @brief Send Data
        ///
        void Send(char* a_pBuffer, int a_Size, SOCKET a_Receiver);
        /// @brief Send Data to Host
        ///
        void SendHost(char* a_pBuffer, int a_Size);
    };
}
#endif /* _NETWORKHANDLER_H_ */