#ifndef _MENU_OPTIONS_H_
#define _MENU_OPTIONS_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "Space.h"
#include "SpriteReader.h"

namespace Menu
{
    enum EHead { Settings,Controls};
    class COptions
    {
    private:
        sba::CNavigation* m_pNavigation;
        sba::CNavigation* m_pControls[4];
        bool m_CFocus[4];
        bool m_Focus;
        bool m_Switched;
        bool m_SSAO;
        float m_Music,m_Sound;
        PuRe_DisplaySetting m_DMode;
        char m_Head;
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
        /// @brief Update theCOptions
        ///
        /// @param Timer for deltatime
        /// @param Window to change DisplayMode
        /// @param Graphics to Change Resolution
        /// @param Index of the player who handles the menu
        ///
        int Update(PuRe_Renderer* a_pRenderer, PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, int a_PlayerIdx);
        /// @brief Display the Options
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


#endif /* _MENU_OPTIONS_H_ */