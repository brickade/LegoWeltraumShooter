#ifndef _MENU_LOBBY_H_
#define _MENU_LOBBY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "TheBrick/Serializer.h"
#include "Space.h"

namespace Menu
{
    class CLobby
    {
    private:
        sba::CNavigation* m_pNavigation;
        int m_LocalPlayers;
        bool m_Run;
        bool m_Start;
        std::vector<std::thread*> m_Threads;
        PuRe_IWindow* m_pWindow;
        bool m_GameEnd;
        bool m_Focus;
    public:
        /// @brief Constructor
        ///
        CLobby(PuRe_IWindow* a_pWindow);
        /// @brief Destructor
        ~CLobby();
    public:
        /// @brief Receive Data
        ///
        /// @param Socket we receive from
        ///
        void ReceiveData(SOCKET s);
        /// @brief Listen for incoming Users
        ///
        void ListenLoop();
        /// @brief Start Connections
        ///
        void Start();
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
        /// @param Font to use
        /// @param Material to display the Font
        /// @param ScreenSize
        ///
        void Render(PuRe_Renderer* a_pRenderer, PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution);
    };
}


#endif /* _MENU_LOBBY_H_ */