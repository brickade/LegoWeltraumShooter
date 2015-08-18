#include "include/Menu_Credits.h"
#include "include/InputManager.h"

namespace Menu
{

    CCredits::CCredits()
    {
        int i = 0;
        this->m_pCredited[i].Text = "Game Artists";
        this->m_pCredited[i].Caption = true;
        ++i;
        this->m_pCredited[i].Text = "Aaron Zielinski";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Nadine Uzoglu";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Tamina Bach";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Game Programmer";
        this->m_pCredited[i].Caption = true;
        ++i;
        this->m_pCredited[i].Text = "Andre Braun";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Jan Oldsen";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Jens Richter";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Game Designer";
        this->m_pCredited[i].Caption = true;
        ++i;
        this->m_pCredited[i].Text = "Jost Foerster";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Audio Engineer";
        this->m_pCredited[i].Caption = true;
        ++i;
        this->m_pCredited[i].Text = "Pascal Reichenberger";
        this->m_pCredited[i].Caption = false;
        ++i;
        this->m_pCredited[i].Text = "Composer";
        this->m_pCredited[i].Caption = true;
        ++i;
        this->m_pCredited[i].Text = "Christian Ort";
        this->m_pCredited[i].Caption = false;
    }

    CCredits::~CCredits()
    {
    }

    int CCredits::Update(PuRe_IInput* a_pInput, PuRe_Renderer* a_pRenderer, PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, int a_PlayerIdx)
    {

        if (sba_Input->ButtonPressed(sba_Button::Exit, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            return 0;
        }

        if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
            return 0;
        return 1;
    }

    void CCredits::Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        ////DRAW UI
        //Position.X = 1920 / 2.0f;
        //Position.Y = 1080 / 2.0f;
        //a_pSpriteReader->Draw(1, a_pRenderer, "kasten_gross", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        //a_pSpriteReader->Draw(1, a_pRenderer, "options_kasten", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        //Draw Bottom
        Position.X = 150.0f;
        Position.Y = 100.0f;

        PuRe_Vector3F highlight = Position;
        highlight.X += 125.0f;

        PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        //DRAW HEADER
        Position.X = 1920.0f/2.0f;
        Position.Y = 1080.0f;

        float Offset,size;
        for (unsigned int i = 0; i< CreditedPerson; i++)
        {
            if (this->m_pCredited[i].Caption)
            {
                Position.Y -= 100.0f;
                Offset = 36.0f;
                size = 30.0f;
                color = PuRe_Color(0.75f, 0.75f, 0.75f);
            }
            else
            {
                Position.Y -= 50.0f;
                Offset = 26.0f;
                size = 20.0f;
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
            }
            Position.X -= (this->m_pCredited[i].Text.length()*Offset) / 2;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, this->m_pCredited[i].Text.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(size, size, 0.0f), Offset, color);
            Position.X += (this->m_pCredited[i].Text.length()*Offset) / 2;

        }
    }
}