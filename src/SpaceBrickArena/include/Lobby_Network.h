#ifndef _LOBBY_NETWORK_H_
#define _LOBBY_NETWORK_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"

namespace sba
{

    class CLobbyNetwork
    {
    private:
        std::vector<std::thread*> m_Threads;
        bool m_Running;
        bool m_Started;
        bool m_End;
        bool m_ShipSelect;
        PuRe_IWindow* m_pWindow;
    public:
        CLobbyNetwork(PuRe_IWindow* a_pWindow);
        ~CLobbyNetwork();
    public:
        void GoShip();
        void GoBack();
        bool IsShip() const;
        bool IsStarted() const;
        bool IsEnded() const;
        bool DeletePlayer(int a_Index);
        bool JoinPlayer(int a_Index);
        bool CheckSend();
        void SendUserData(sba::SPlayer* a_Player);
        void Start();
        void Exit();
        void SendMap(std::string a_Map);
        void SendCommand();
        void Receive(SOCKET s);
        void Listen();
        void Broadcast(PuRe_Timer* a_pTimer);
    };
}
#endif /* _LOBBY_NETWORK_H_ */