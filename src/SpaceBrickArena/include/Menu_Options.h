#ifndef _MENU_OPTIONS_H_
#define _MENU_OPTIONS_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "Space.h"

namespace Menu
{
    class COptions
    {
    private:
        sba::CNavigation* m_pNavigation;
        int* m_PlayerIdx;
        bool m_Focus;
        bool m_Switched;
    public:
        int m_Resolution;
        int m_Resolutions[7][2];
    public:
        /// @brief Constructor
        ///
        COptions();
        /// @brief Destructor
         ~COptions();
    public:
        /// @brief Reset the Options
        ///
        void Reset();
        /// @brief Update theCOptions
        ///
        /// @param Timer for deltatime
        /// @param Index of the player who handles the menu
        ///
        int Update(PuRe_Timer* a_pTimer, int a_PlayerIdx);
        /// @brief Display the Options
        ///
        /// @param Renderer to use
        /// @param Font to use
        /// @param Material to display the Font
        /// @param ScreenSize
        ///
        void Render(PuRe_Renderer* a_pRenderer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution);
    };
}


#endif /* _MENU_OPTIONS_H_ */