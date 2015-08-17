#include "include/Editor_ModeMenu.h"

#include "include/Space.h"
#include "include/InputManager.h"

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
        SControlInfo space = { std::string(), std::string(), std::string(), std::string(), std::string() };
        this->m_pNavigation = new sba::CNavigation(1, 1);
        this->m_ControlInfo.push_back({ "Rotate Camera", "right_stick", "", "mouse_right", "" });
        this->m_ControlInfo.push_back({ "Zoom Camera", "LT", "RT", "mouse_wheel", "" });
        this->m_ControlInfo.push_back({ space });
        this->m_ControlInfo.push_back({ "Move Brick", "left_stick", "", "mouse", "" });
        this->m_ControlInfo.push_back({ "Rotate Brick", "left_shoulder", "right_shoulder", "key_QE", "" });
        this->m_ControlInfo.push_back({ "Switch Side", "B", "", "key_V", "" });
        this->m_ControlInfo.push_back({ "Place Brick", "A", "", "mouse_left", "" });
        this->m_ControlInfo.push_back({ space });
        this->m_ControlInfo.push_back({ "Show Menus", "Y", "dpad", "key_shift", "key_WSAD" });
        this->m_ControlInfo.push_back({ "Navigate Menu", "dpad", "", "key_WSAD", "" });
        this->m_ControlInfo.push_back({ space });
        this->m_ControlInfo.push_back({ "Undo/Redo", "X", "dpad", "key_ctrl", "key_ZY" });
        this->m_ControlInfo.push_back({ "Save Ship", "start", "", "key_ctrl", "key_S" });
        this->m_ControlInfo.push_back({ "Exit", "back", "", "key_escape", "" });
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
        PuRe_Vector2F fadeIn = PuRe_Vector2F(750 * (1.0f - a_Visibility), 0);
        PuRe_Vector2F menuStartPos = this->m_ListStart + fadeIn;
        int focus = this->m_pNavigation->GetFocusedElementId();
        a_rSpriteReader.Draw(1, sba_Renderer, "editor_button", sba_Space->FontMaterial, PuRe_Vector3F(menuStartPos + this->m_ListStep * (focus == 0 ? 0 : 1) + PuRe_Vector2F(80, 0), 0), PuRe_Vector3F::Zero(), -1, PuRe_Vector2F(0.5f, 0.5f));
        sba_Space->RenderFont("Build", menuStartPos + this->m_ListStep * 0, 22.0f);
        sba_Space->RenderFont("Delete", menuStartPos + this->m_ListStep * 1, 22.0f);

        PuRe_Vector2F infoStartPos = this->m_InfoStart + fadeIn;
        float size = 0.12f;
        for (size_t i = 0; i < this->m_ControlInfo.size(); i++)
        {
            if (this->m_ControlInfo[i].InfoText != "")
            {
                sba_Space->RenderFont(this->m_ControlInfo[i].InfoText, infoStartPos + this->m_InfoStep * i, 14.0f);
                if (sba_Input->FirstPlayerHasGamepad())
                {
                    if (this->m_ControlInfo[i].Gamepad2 == "")
                    {
                        sba_ButtonsDraw(this->m_ControlInfo[i].Gamepad1.c_str(), infoStartPos + PuRe_Vector2F(275, 0) + this->m_InfoStep * i, size * 1.5f);
                    }
                    else
                    {
                        sba_ButtonsDraw(this->m_ControlInfo[i].Gamepad1.c_str(), infoStartPos + PuRe_Vector2F(250, 0) + this->m_InfoStep * i, size * 1.5f);
                        sba_ButtonsDraw(this->m_ControlInfo[i].Gamepad2.c_str(), infoStartPos + PuRe_Vector2F(300, 0) + this->m_InfoStep * i, size * 1.5f);
                    }
                }
                else
                {
                    if (this->m_ControlInfo[i].Keyboard2 == "")
                    {
                        sba_ButtonsDraw(this->m_ControlInfo[i].Keyboard1.c_str(), infoStartPos + PuRe_Vector2F(275, 0) + this->m_InfoStep * i, size);
                    }
                    else
                    {
                        sba_ButtonsDraw(this->m_ControlInfo[i].Keyboard1.c_str(), infoStartPos + PuRe_Vector2F(245, 0) + this->m_InfoStep * i, size);
                        sba_ButtonsDraw(this->m_ControlInfo[i].Keyboard2.c_str(), infoStartPos + PuRe_Vector2F(310, 0) + this->m_InfoStep * i, size);
                    }
                }
            }
        }
    }
}