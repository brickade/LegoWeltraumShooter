#ifndef _EDITORMODEMENU_H_
#define _EDITORMODEMENU_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include "Navigation.h"

namespace Editor
{
    class CModeMenu
    {
    public:

    private:
        int m_PlayerIdx;
        sba::CNavigation* m_pNavigation;

        PuRe_Vector2F m_ListStart = PuRe_Vector2F(1920 - 250, 1080 - 300);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(0, -60);
        float m_RightStaticIn = 85.0f;

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
        void Render(PuRe_IGraphics& a_pGraphics, float a_Visibility);
    };
}

#endif /* _EDITORMODEMENU_H_ */