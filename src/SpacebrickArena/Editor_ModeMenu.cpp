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
        this->AddInfo("Rotate Camera", "Right Thumbstick", "Right Mouse");
        this->AddInfo("Zoom Camera", "Trigger", "Mouse Wheel");
        this->AddInfoSpace();
        this->AddInfo("Move Brick", "Left Thumbstick", "Mouse");
        this->AddInfo("Rotate Brick", "Shoulders", "Q/E");
        this->AddInfo("Switch Side", "B", "V");
        this->AddInfo("Place Brick", "A", "Left Mouse");
        this->AddInfoSpace();
        this->AddInfo("Show Menus", "Y+DPAD", "Shift+WSAD");
        this->AddInfo("Navigate Menu", "DPAD", "WSAD");
        this->AddInfoSpace();
        this->AddInfo("Undo/Redo", "X+DPAD", "Ctrl+Z/Y");
        this->AddInfo("Save Ship", "Start", "Ctrl+S");
        this->AddInfo("Exit", "Back", "Escape");
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
        PuRe_Vector2F fadeIn = PuRe_Vector2F((this->m_InfoStart.X + 50) * (1.0f - a_Visibility), 0);
        PuRe_Vector2F menuStartPos = this->m_ListStart + fadeIn;
        int focus = this->m_pNavigation->GetFocusedElementId();
        for (int i = 0; i < 2; i++)
        {
            PuRe_Vector2F tmpPos = menuStartPos + this->m_ListStep * static_cast<float>(i);
            tmpPos.X += 80;
            a_rSpriteReader.Draw(1, sba_Renderer, "editor_button", sba_Space->FontMaterial, PuRe_Vector3F(tmpPos, 0), PuRe_Vector3F::Zero(), -1, PuRe_Vector2F(0.5f, 0.5f));
        }
        sba_Space->RenderFont("Build", menuStartPos + this->m_ListStep * 0, focus == 0 ? 24.0f : 14.0f);
        sba_Space->RenderFont("Delete", menuStartPos + this->m_ListStep * 1, focus == 1 ? 24.0f : 14.0f);

        PuRe_Vector2F infoStartPos = this->m_InfoStart + fadeIn;
        for (size_t i = 0; i < this->m_InfoText.size(); i++)
        {
            sba_Space->RenderFont(this->m_InfoText[i], infoStartPos + this->m_InfoStep * i, 12.0f);
            sba_Space->RenderFont(this->m_InfoControlGamepad[i], infoStartPos + PuRe_Vector2F(250, 0) + this->m_InfoStep * i, 12.0f);
            sba_Space->RenderFont(this->m_InfoControlKeyboard[i], infoStartPos + PuRe_Vector2F(520, 0) + this->m_InfoStep * i, 12.0f);
        }
    }
}