#include "include/Editor_ModeMenu.h"

#include "include/Space.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CModeMenu::CModeMenu(int a_playerIdx)
    {
        this->m_PlayerIdx = a_playerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    CModeMenu::~CModeMenu()
    {
    }


    // **************************************************************************
    // **************************************************************************
    void CModeMenu::Initialize(PuRe_IGraphics& a_pGraphics)
    {
        this->m_pNavigation = new sba::CNavigation(1, 1);
    }

    // **************************************************************************
    // **************************************************************************
    void CModeMenu::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer)
    {
        this->m_pNavigation->Update(a_pTimer, sba_Input->Direction(sba_Direction::EditorChooseElement, this->m_PlayerIdx));
    }

    // **************************************************************************
    // **************************************************************************
    void CModeMenu::Render(PuRe_IGraphics& a_pGraphics, float a_Visibility)
    {
        PuRe_Vector2F startPos = this->m_ListStart;
        startPos.X += (1920 - this->m_ListStart.X - this->m_RightStaticIn) * (1.0f - a_Visibility) - this->m_RightStaticIn;
        int focus = this->m_pNavigation->GetFocusedElementId();
        sba_Space->RenderFont("Build", startPos + this->m_ListStep * 0, focus == 0 ? 24.0f : 18.0f);
        sba_Space->RenderFont("Delete", startPos + this->m_ListStep * 1, focus == 1 ? 24.0f : 18.0f);
    }
}