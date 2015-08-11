#ifndef _NETWORKHANDLER_H_
#define _NETWORKHANDLER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/BrickData.h>
#include "include/INIReader.h"

#include <mutex>

// Declare namespace Game
namespace sba
{
    enum { MaxPlayers = 4, Delay = 10, BroadcastPort =13370, MaxName=12, MaxLength=15, MaxBricks=200};

    enum EPacket { Broadcast, Join, User, IAm,WhoamI, Left,LobbyEnd,Ship,Menu,Brick,SendCommand,Start,Init,Map, STick, CTick };

    /// @brief Struct which handles the Input one Player does
    ///
    struct SInputData
    {
        unsigned char Player;
        bool MG;
        bool Rocket;
        bool Laser;
        bool Mine;
        char Thrust;
        char Spin;
        char MoveX;
        char MoveY;

    };
    /// @brief HeaderPacket, used to tell which Packet we got
    ///
    struct SHeadPacket
    {
        EPacket Type;
    };

    /// @brief Packet used when Data is received, this checks the Header
    ///
    struct SReceivePacket
    {
        SHeadPacket Head;
        char* Buffer;
    };
    /// @brief Map packet
    ///
    struct SMapPacket
    {
        SHeadPacket Head;
        char Name[MaxName];

    };
    /// @brief Packet send over broadcast
    ///
    struct SBroadcastPacket
    {
        SHeadPacket Head;
        unsigned int Players;
        char Name[MaxName];
        char IP[MaxLength];
        char Port[MaxLength];
    };
    /// @brief Who am I
    ///
    struct SWhoamIPacket
    {
        SHeadPacket Head;
        int Pad;
    };
    /// @brief Packet that tells the User who he is
    ///
    struct SIamPacket
    {
        SHeadPacket Head;
        int ID;
        int Pad;
        char Map[MaxName];
    };
    /// @brief UserData's packet
    ///
    struct SUserPacket
    {
        SHeadPacket Head;
        int ID;
        char Name[MaxName+1];
    };
    /// @brief BrickData's packet
    ///
    struct SBrickPacket
    {
        SHeadPacket Head;
        int ID;
        int BrickNum;
        TheBrick::SBrickData Bricks[MaxBricks];
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
        CNetworkHandler(CIniReader* a_pIniReader);
        ~CNetworkHandler();
    public:
        /// @brief Sets whether the calls are blocking or not
        ///
        void SetBlockMode(SOCKET s,bool a_Block);
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
        long Receive(char* a_pBuffer, int a_Size, SOCKET a_pSender, bool a_Select);
        /// @brief Send Data
        ///
        void Send(char* a_pBuffer, int a_Size, SOCKET a_Receiver, bool a_Select);
        /// @brief Send Data to Host
        ///
        void SendHost(char* a_pBuffer, int a_Size, bool a_Select);
    };
}
#endif /* _NETWORKHANDLER_H_ */