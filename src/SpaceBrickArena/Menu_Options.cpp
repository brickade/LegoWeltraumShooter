#include "include/Menu_Options.h"

namespace Menu
{

    COptions::COptions()
    {
        this->m_pNavigation = new sba::CNavigation(1, 3);
        this->m_Focus = 0;
        this->m_Resolutions[0][0] = 1024;
        this->m_Resolutions[0][1] = 768;
        this->m_Resolutions[1][0] = 1280;
        this->m_Resolutions[1][1] = 1024;
        this->m_Resolutions[2][0] = 1360;
        this->m_Resolutions[2][1] = 768;
        this->m_Resolutions[3][0] = 1440;
        this->m_Resolutions[3][1] = 900;
        this->m_Resolutions[4][0] = 1600;
        this->m_Resolutions[4][1] = 900;
        this->m_Resolutions[5][0] = 1680;
        this->m_Resolutions[5][1] = 1050;
        this->m_Resolutions[6][0] = 1920;
        this->m_Resolutions[6][1] = 1080;
        int m_Resolution = 6;

    }

    COptions::~COptions()
    {
        SAFE_DELETE(this->m_pNavigation);
    }

    int COptions::Update(PuRe_Timer* a_pTimer, int a_PlayerIdx)
    {
        PuRe_Vector2F nav = sba_Input->Direction(sba_Direction::MenuMove, a_PlayerIdx);
        if (!this->m_Focus)
        {
            nav.X = 0.0f;
            this->m_pNavigation->Update(*a_pTimer, nav);
        }
        else
        {
            if (nav.X == 0.0f)
                this->m_Switched = false;
            if (!this->m_Switched)
            {
                switch (this->m_pNavigation->GetFocusedElementId())
                {
                case 0: //Resolution
                    if (nav.X > 0.0f)
                    {
                        this->m_Resolution++;
                        this->m_Switched = true;
                    }
                    if (nav.X < 0.0f)
                    {
                        this->m_Resolution--;
                        this->m_Switched = true;
                    }
                    if (this->m_Resolution > 6)
                        this->m_Resolution = 0;
                    else if (this->m_Resolution < 0)
                        this->m_Resolution = 6;
                    break;
                case 1: //DisplayMode
                    break;
                case 2: //Fullscreen
                    break;
                }
            }
        }
        if (sba_Input->ButtonPressed(sba_Button::MenuClick, a_PlayerIdx))
        {
            if (this->m_pNavigation->GetFocusedElementId() == 3)
                return 0;
            else
                this->m_Focus = true;
        }
        if (sba_Input->ButtonPressed(sba_Button::MenuBack, a_PlayerIdx))
        {
            if (this->m_Focus)
                this->m_Focus = false;
            else
                this->m_pNavigation->SetFocusedElementId(3);
            if (this->m_pNavigation->GetFocusedElementId() == 0)
                return 2;
        }
        return 1;
    }

    void COptions::Reset()
    {

    }

    void COptions::Render(PuRe_Renderer* a_pRenderer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        Position.X = a_Resolution.X / 2.0f;
        Position.Y = a_Resolution.Y / 2.0f;
        Position.X -= 400.0f;
        Position.Y += 200.0f;
        PuRe_Color color = PuRe_Color();

        if (this->m_pNavigation->GetFocusedElementId() == 0 && !this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Resolution", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X += 800.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 0&&this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, std::to_string(this->m_Resolutions[this->m_Resolution][0]) + "x" + std::to_string(this->m_Resolutions[this->m_Resolution][1]), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X -= 800.0f;

        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 1 && !this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "DisplayMode", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X += 800.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Windowed", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X -= 800.0f;

        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 2 && !this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "SSAO", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X += 800.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 2 && this->m_Focus)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "On", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X -= 800.0f;

        Position.Y -= 128.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 3)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Back", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
    }
}