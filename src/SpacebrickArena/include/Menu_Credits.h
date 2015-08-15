#ifndef _MENU_CREDITS_H_
#define _MENU_CREDITS_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "Space.h"
#include "SpriteReader.h"

namespace Menu
{
    class CCredits
    {
    private:
        enum { CreditedPerson = 14 };
        struct Credited
        {
            std::string Text;
            bool Caption;
        };
        Credited m_pCredited[CreditedPerson];
    public:
        /// @brief Constructor
        ///
        CCredits();
        /// @brief Destructor
        ~CCredits();
    public:
        /// @brief Update the Credits
        ///
        /// @param Input Handler
        /// @param Timer for deltatime
        /// @param Window to change DisplayMode
        /// @param Graphics to Change Resolution
        /// @param Index of the player who handles the menu
        ///
        int Update(PuRe_IInput* a_pInput, PuRe_Renderer* a_pRenderer, PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, int a_PlayerIdx);
        /// @brief Display the Credits
        ///
        /// @param Renderer to use
        /// @param SpriteReader to display UI
        /// @param Font to use
        /// @param Material to display the Font
        /// @param ScreenSize
        ///
        void Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution);
    };
}


#endif /* _MENU_CREDITS_H_ */