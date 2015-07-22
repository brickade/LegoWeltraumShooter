#ifndef _MENU_NETWORK_H
#define _MENU_NETWORK_H

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "Space.h"
#include "Menu_Server.h"
#include "SpriteReader.h"

namespace Menu
{
    class CNetwork
    {
    private:
        sba::CNavigation* m_pNavigation;
        bool m_Focus;
        bool m_Focus2;
        std::vector<SServer> m_Servers;
        PuRe_Timer* m_pTimer;
        int m_Server;
        float m_ServerWait; 
    public:
        bool m_getList;
    public:
        /// @brief Constructor
        ///
        /// @param Timer for Server Messages
        ///
        CNetwork(PuRe_Timer* a_pTimer);
        /// @brief Destructor
        ~CNetwork();
    private:
        /// @brief Connect to a Server
        ///
        /// @param Window to change DisplayMode
        ///
        bool ConnectToServer(PuRe_IWindow* a_pWindow);
    public:
        /// @brief Delete all Servers
        ///
        void DeleteServers();
        /// @brief Start the Network Broadcast
        ///
        void Start();
        /// @brief Receive Data, run as thread
        ///
        void ReceiveData();
        /// @brief Update theCOptions
        ///
        /// @param Timer for deltatime
        /// @param Window to change DisplayMode
        /// @param Graphics to Change Resolution
        /// @param Index of the player who handles the menu
        ///
        int Update(PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, int a_PlayerIdx);
        /// @brief Display the Options
        ///
        /// @param Renderer to use
        /// @param SpriteReader to display UI
        /// @param Font to use
        /// @param Material to display the Font
        /// @param ScreenSize
        ///
        void Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution);
    };
}


#endif /* _MENU_NETWORK_H */