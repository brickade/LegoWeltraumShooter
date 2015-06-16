#include "include/Menu_Main.h"

namespace Menu
{

    CMain::CMain()
    {
        this->m_pNavigation = new sba::CNavigation(1, 4);
    }

    CMain::~CMain()
    {
        SAFE_DELETE(this->m_pNavigation);
    }

    int CMain::Update(PuRe_Timer* a_pTimer, int a_PlayerIdx)
    {
        this->m_pNavigation->Update(*a_pTimer, sba_Input->Direction(sba_Direction::MenuMove, a_PlayerIdx));
        if (sba_Input->ButtonPressed(sba_Button::MenuClick, a_PlayerIdx))
        {
            switch (this->m_pNavigation->GetFocusedElementId())
            {
            case 0: //Local
                return 2;
                break;
            case 1: //Multiplayer
                return 3;
                break;
            case 2: //Editor
                return 4;
                break;
            case 3: //Options
                return 5;
                break;
            default:
                return 0;
                break;
            }
        }
        return 1;
    }

    void CMain::Render(PuRe_Renderer* a_pRenderer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        Position.X = 1920 / 2.0f;
        Position.Y = 1080 / 2.0f;
        Position.X -= 100.0f;
        Position.Y += 200.0f;

        PuRe_Color color = PuRe_Color();

        if (this->m_pNavigation->GetFocusedElementId() == 0)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Local", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 1)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Multiplayer", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 2)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Editor", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 3)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Options", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 4)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Quit", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);

    }
}