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
    void CModeMenu::Render(PuRe_IGraphics& a_pGraphics, sba::CSpriteReader& a_rSpriteReader, float a_Visibility)
    {
        PuRe_Vector2F startPos = this->m_ListStart;
        startPos.X += (1920 - this->m_ListStart.X + 50) * (1.0f - a_Visibility);
        int focus = this->m_pNavigation->GetFocusedElementId();
        for (int i = 0; i < 2; i++)
        {
            PuRe_Vector2F tmpPos = startPos + this->m_ListStep * static_cast<float>(i);
            tmpPos.X += 80;
            a_rSpriteReader.Draw(1, sba_Renderer, "editor_buttons", sba_Space->FontMaterial, PuRe_Vector3F(tmpPos, 0), PuRe_Vector3F::Zero(), -1, PuRe_Vector2F(0.5f, 0.5f));
        }
        sba_Space->RenderFont("Build", startPos + this->m_ListStep * 0, focus == 0 ? 24.0f : 14.0f);
        sba_Space->RenderFont("Delete", startPos + this->m_ListStep * 1, focus == 1 ? 24.0f : 14.0f);


        PuRe_Vector2F startPosInfo = this->m_InfoStart;
        sba_Space->RenderFont("TestInfo", startPosInfo + this->m_InfoStep * 0, 18.0f);
        sba_Space->RenderFont("TestInfo1", startPosInfo + this->m_InfoStep * 1, 18.0f);
        sba_Space->RenderFont("TestInfo2", startPosInfo + this->m_InfoStep * 2, 18.0f);
        sba_Space->RenderFont("TestInfo3", startPosInfo + this->m_InfoStep * 3, 18.0f);
        sba_Space->RenderFont("TestInfo4", startPosInfo + this->m_InfoStep * 4, 18.0f);
        sba_Space->RenderFont("TestInfo5", startPosInfo + this->m_InfoStep * 5, 18.0f);
    }
}