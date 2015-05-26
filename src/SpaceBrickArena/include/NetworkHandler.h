#ifndef _NETWORKHANDLER_H_
#define _NETWORKHANDLER_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/TheBrick.h>

// Declare namespace Game
namespace Game
{
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
    };
}
#endif /* _NETWORKHANDLER_H_ */