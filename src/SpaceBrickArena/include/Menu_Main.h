#ifndef _MENU_MAIN_H_
#define _MENU_MAIN_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "SpriteReader.h"
#include "Space.h"

namespace Menu
{

    class CMain
    {
    private:
        sba::CNavigation* m_pNavigation;
        int* m_PlayerIdx;
    public:
        /// @brief Constructor
        ///
        /// @param Graphics for graphic initialization
        ///
        CMain(PuRe_IGraphics* a_pGraphics);
        /// @brief Destructor
        ///
        ~CMain();
    public:
        /// @brief Update the MainMenu
        ///
        /// @param Timer for deltatime
        /// @param Index of the player who handles the menu
        ///
        int Update(PuRe_Timer* a_pTimer, int a_PlayerIdx);
        /// @brief Display the MainMenu
        ///
        /// @param Renderer to use
        /// @param SpriteReader to display UI
        /// @param Font to use
        /// @param Material to display the Font
        /// @param ScreenSize
        ///
        void Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader,PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution);
    };
}


#endif /* _MENU_MAIN_H_ */