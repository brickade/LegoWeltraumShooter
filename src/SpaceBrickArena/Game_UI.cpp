#include "include/Game_UI.h"

namespace sba
{
    CGUI::CGUI(PuRe_IGraphics* a_pGraphics)
    {
        this->m_pSpriteMaterial = a_pGraphics->LoadMaterial("../data/effects/sprite/default");
        this->m_pCrossHairSprite = new PuRe_Sprite(a_pGraphics, "../data/textures/crosshair.png");
        this->m_pMinimap = new CMinimap(a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CGUI::DisplayUI(PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, float a_EndTime,int a_WonID)
    {
        PuRe_Color c = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
        PuRe_Vector3F size = PuRe_Vector3F(32.0f, 32.0f, 0.0f);
        PuRe_Vector3F pos = PuRe_Vector3F(100.0f, 1080.0f - 100.0f, 0.0f);
        int local = 0;
        for (unsigned int i = 0; i<sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                pos.X = 100.0f;
                pos.Y = 1080.0f - 100.0f;
                sba_Renderer->Draw(2, false, a_pFont, a_pFontMaterial, "Life: " + std::to_string(sba_Players[i]->Ship->m_Life), pos, PuRe_MatrixF(), size, 36.0f, c, local);
                pos.Y -= 100.0f;
                sba_Renderer->Draw(2, false, a_pFont, a_pFontMaterial, "Shield: " + std::to_string(sba_Players[i]->Ship->m_Shield), pos, PuRe_MatrixF(), size, 36.0f, c, local);
                pos.Y -= 100.0f;
                sba_Renderer->Draw(2, false, a_pFont, a_pFontMaterial, "Points: " + std::to_string(sba_Players[i]->m_Points), pos, PuRe_MatrixF(), size, 36.0f, c, local);
                pos.X = 1920.0f / 2.0f;
                pos.Y = 1080.0f / 2.0f;
                sba_Renderer->Draw(2, false, this->m_pCrossHairSprite, this->m_pSpriteMaterial, pos, PuRe_MatrixF(), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(0.5f, 0.5f, 0.5f), PuRe_Color(1.0f, 1.0f, 1.0f), PuRe_Vector2F(), PuRe_Vector2F(), local);
                local++;
            }
        }



        if (a_EndTime < 0.0f)
        {
            size = PuRe_Vector3F(64.0f, 64.0f, 0.0f);
            pos = PuRe_Vector3F(1920.0f / 4.0f, 1080.0f / 2.0f, 0.0f);
            sba_Renderer->Draw(3, false, a_pFont, a_pFontMaterial, "Player " + std::to_string(a_WonID) + " won!", pos, PuRe_MatrixF(), size, 72.0f, c);
        }
        else
        {
            int minLeft = (int)std::floor(a_EndTime / 60.0f);
            int secLeft = (int)(a_EndTime - (minLeft * 60.0f) );
            std::string minString = std::to_string(minLeft);
            std::string secString = std::to_string(secLeft);
            if (secLeft < 10.0f)
                secString = "0" + secString;
            std::string timeLeft = "Left Time: " + minString + ":" + secString;
            size = PuRe_Vector3F(32.0f, 32.0f, 0.0f);
            pos = PuRe_Vector3F(1920.0f / 4.0f, 1080.0f - 150.0f, 0.0f);
            sba_Renderer->Draw(3, false, a_pFont, a_pFontMaterial, timeLeft, pos, PuRe_MatrixF(), size, 32.0f, c);
        }
    }


    // **************************************************************************
    // **************************************************************************
    CGUI::~CGUI()
    {
        SAFE_DELETE(this->m_pSpriteMaterial);
        SAFE_DELETE(this->m_pCrossHairSprite);
        SAFE_DELETE(this->m_pMinimap);
    }
}