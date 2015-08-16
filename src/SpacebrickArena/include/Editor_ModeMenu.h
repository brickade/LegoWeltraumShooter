#ifndef _EDITORMODEMENU_H_
#define _EDITORMODEMENU_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Navigation.h"
#include "SpriteReader.h"

namespace Editor
{
    class CModeMenu
    {
        struct SControlInfo
        {
            std::string InfoText;
            std::string Gamepad1;
            std::string Gamepad2;
            std::string Keyboard1;
            std::string Keyboard2;
        };
    public:

    private:
        int m_PlayerIdx;
        sba::CNavigation* m_pNavigation;

        PuRe_Vector2F m_ListStart = PuRe_Vector2F(1920 - 280, 1080 - 350);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(0, -60);

        PuRe_Vector2F m_InfoStart = PuRe_Vector2F(1920 - 400, 1080 - 520);
        PuRe_Vector2F m_InfoStep = PuRe_Vector2F(0, -40);

        std::vector<SControlInfo> m_ControlInfo;

    public:
        int SelectedMode() const
        {
            return this->m_pNavigation->GetFocusedElementId();
        }

    public:
        CModeMenu(int a_playerIdx);
        ~CModeMenu();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer);
        void Render(PuRe_IGraphics& a_pGraphics, sba::CSpriteReader& a_rSpriteReader, float a_Visibility);
    };
}

#endif /* _EDITORMODEMENU_H_ */