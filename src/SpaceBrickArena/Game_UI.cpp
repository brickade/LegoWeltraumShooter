#include "include/Game_UI.h"

namespace sba
{
    CGUI::CGUI(PuRe_IGraphics* a_pGraphics)
    {
        this->m_pSpriteMaterial = a_pGraphics->LoadMaterial("../data/effects/sprite/default");
        this->m_pCrossHairSprite = new PuRe_Sprite(a_pGraphics, "../data/textures/crosshair.png");
        this->m_pHitMarkerSprite = new PuRe_Sprite(a_pGraphics, "../data/textures/hitmarker.png");
        this->m_pUI = new sba::CSpriteReader(a_pGraphics, "../data/textures/ui/ingame.png", "../data/textures/ui/ingame.txt");
        this->m_pMinimap = new CMinimap(a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CGUI::DisplayUI(PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, float a_EndTime, int a_WonID, bool* a_pDisplayEnd, float a_OriginDistance, std::vector<CGameCamera*>* a_pCameras, bool a_Pause, bool a_PauseEnd)
    {
        PuRe_Color c = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
        PuRe_Vector3F size = PuRe_Vector3F(36.0f, 36.0f, 0.0f);
        PuRe_Vector3F pos = PuRe_Vector3F(100.0f, 1080.0f - 100.0f, 0.0f);
        int local = 0;
        //scale for the shield and life to define where it starts
        float balken_size = 246.0f;
        for (unsigned int i = 0; i<sba_Players.size(); i++)
        {
            if (sba_Players[i]->PadID != -1)
            {
                pos.X = (1920.0f/2.0f)-400.0f;
                pos.Y =  1080.0f/2.0f;
                this->m_pUI->Draw(2, sba_Renderer, "life_shield_tile", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));

                float value = sba_Players[i]->Ship->m_Life / (float)sba_Players[i]->Ship->m_MaxLife;


                pos.Y += 20.0f;
                pos.X += 5.0f*(1.0f-value);

                pos.Y -= balken_size*(1.0f - value)*0.45f;
                this->m_pUI->Draw(2, sba_Renderer, "life_balken_tile", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f*value), PuRe_Vector2F(1.0f, value), PuRe_Vector2F(0.0f, (1.0f - value)*0.225f));
                pos.Y += balken_size*(1.0f - value)*0.45f;

                pos.X -= 5.0f*(1.0f - value);
                pos.Y -= 20.0f;

                pos.Y -= 175.0f;
                this->m_pUI->Draw(2, sba_Renderer, "life_icon_tile", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                pos.Y += 175.0f;

                pos.X += 800.0f;
                this->m_pUI->Draw(2, sba_Renderer, "shield_tile", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));

                value = (sba_Players[i]->Ship->m_Shield / 100.0f);



                pos.Y += 20.0f;
                pos.X += 45.0f - 5.0f*(1.0f - value);

                pos.Y -= balken_size*(1.0f - value)*0.45f;
                this->m_pUI->Draw(2, sba_Renderer, "shield_balken_tile", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f*value), PuRe_Vector2F(1.0f, value), PuRe_Vector2F(0.0f, (1.0f - value)*0.225f));
                pos.Y += balken_size*(1.0f - value)*0.45f;

                pos.X -= 45.0f + 5.0f*(1.0f - value);
                pos.Y -= 20.0f;

                pos.Y -= 175.0f;
                this->m_pUI->Draw(2, sba_Renderer, "shield_icon_tile", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                pos.Y += 175.0f;

                this->m_pMinimap->Draw((*a_pCameras)[local],this->m_pUI, a_pFontMaterial, i, local, a_OriginDistance);

                pos.X = 1920.0f/2.0f;
                pos.Y = 1080.0f-100.0f;

                pos.X -= 250.0f;
                sba_Renderer->Set(2, sba_Players[i]->LaserCD/std::stof(sba_Balancing->GetValue("Laser_CD")), "verlauf");
                this->m_pUI->Draw(2, sba_Renderer, "icon_kasten", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                pos.Y += 10.0f;
                this->m_pUI->Draw(2, sba_Renderer, "laser_icon", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                pos.Y -= 10.0f;

                pos.X += 150.0f;
                sba_Renderer->Set(2, sba_Players[i]->MineCD / std::stof(sba_Balancing->GetValue("Mine_CD")), "verlauf");
                this->m_pUI->Draw(2, sba_Renderer, "icon_kasten", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                this->m_pUI->Draw(2, sba_Renderer, "mines_icon", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));

                pos.X += 150.0f;
                sba_Renderer->Set(2, sba_Players[i]->RocketCD / std::stof(sba_Balancing->GetValue("Rocket_CD")), "verlauf");
                this->m_pUI->Draw(2, sba_Renderer, "icon_kasten", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                this->m_pUI->Draw(2, sba_Renderer, "rocket_icon", a_pFontMaterial, pos, PuRe_Vector3F(), local, PuRe_Vector2F(0.5f, 0.5f));
                sba_Renderer->Set(2, 0.0f, "verlauf");

                pos.X = 1920.0f / 2.0f;
                pos.Y = 1080.0f / 2.0f;
                sba_Renderer->Set(2, 1.0f, "alpha");
                sba_Renderer->Draw(2, false, this->m_pCrossHairSprite, this->m_pSpriteMaterial, pos, PuRe_MatrixF(), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(0.5f, 0.5f, 0.5f), PuRe_Color(1.0f, 1.0f, 1.0f), PuRe_Vector2F(), PuRe_Vector2F(), local);

                if (sba_Players[i]->Marker != 0.0f)
                {
                    sba_Renderer->Set(2, sba_Players[i]->Marker, "alpha");
                    sba_Renderer->Draw(2, false, this->m_pHitMarkerSprite, this->m_pSpriteMaterial, pos, PuRe_MatrixF(), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(0.2f, 0.2f, 0.2f), PuRe_Color(1.0f, 1.0f, 1.0f), PuRe_Vector2F(), PuRe_Vector2F(), local);
                }
                sba_Renderer->Set(2, 1.0f, "alpha");
                pos.Y = 1080.0f-100.0f;
                pos.X -= 200.0f;
                if (a_pDisplayEnd[local])
                {
                    sba_Renderer->Draw(2, false, a_pFont, a_pFontMaterial, "Turn around", pos, PuRe_MatrixF(), size*1.2f, 36.0f*1.2f, PuRe_Color(1.0f, 1.0f, 1.0f, (sin(a_EndTime) + 1.0f) / 2.0f), local);
                    pos.Y -= 200.0f;
                    int time = 11 - (int)std::ceil(sba_Players[i]->OutTime);
                    sba_Renderer->Draw(2, false, a_pFont, a_pFontMaterial, std::to_string(time), pos, PuRe_MatrixF(), size*2.5f, 36.0f*2.5f, PuRe_Color(1.0f, 0.0f, 0.0f, (float)((sin((a_EndTime*3.14 * 2) - 5) + 1) / 2)), local);
                }

                if (sba_Players[i]->KilledTimer != 0.0f)
                {
                    float timer = sba_Players[i]->KilledTimer / 1.0f;
                    size = PuRe_Vector3F(48.0f*timer, 48.0f*timer, 0.0f);
                    pos = PuRe_Vector3F((1920/2.0f)-100.0f + (1.0f-timer)*100, 1080.0f - 300.0f- (100 * timer), 0.0f);
                    sba_Renderer->Set(2, 1.0f, "alpha");
                    sba_Renderer->Draw(2, false, a_pFont, a_pFontMaterial, "+100", pos, PuRe_MatrixF(), size, 48.0f*timer, PuRe_Color(1.0f, 1.0f, 1.0f, timer), local);

                }

                local++;
            }
        }

        if (a_Pause)
        {
            size = PuRe_Vector3F(48.0f, 48.0f, 0.0f);
            pos = PuRe_Vector3F(300.0f, 600.0f, 0.0f);
            if (a_PauseEnd)
                c = PuRe_Color(0.5f, 0.5f, 0.5f, 0.5f);
            else
                c = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
            sba_Renderer->Draw(3, false, a_pFont, a_pFontMaterial, "Continue", pos, PuRe_MatrixF(), size, 42.0f, c,0);
            pos.Y -= 150.0f;
            if (!a_PauseEnd)
                c = PuRe_Color(0.5f, 0.5f, 0.5f, 0.5f);
            else
                c = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
            sba_Renderer->Draw(3, false, a_pFont, a_pFontMaterial, "End Game", pos, PuRe_MatrixF(), size, 42.0f, c, 0);
            c = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
        }

        if (a_EndTime < 0.0f)
        {
            size = PuRe_Vector3F(48.0f, 48.0f, 0.0f);
            pos = PuRe_Vector3F(300.0f, 500.0f, 0.0f);
            sba_Renderer->Draw(3, false, a_pFont, a_pFontMaterial, "Player " + std::to_string(a_WonID+1) + " won!", pos, PuRe_MatrixF(), size, 42.0f, c);
        }
        else
        {
            int minLeft = (int)std::floor(a_EndTime / 60.0f);
            int secLeft = (int)(a_EndTime - (minLeft * 60.0f) );
            std::string minString = std::to_string(minLeft);
            std::string secString = std::to_string(secLeft);
            if (secLeft < 10.0f)
                secString = "0" + secString;
            size = PuRe_Vector3F(36.0f, 36.0f, 0.0f);
            pos = PuRe_Vector3F(100.0f, 980.0f, 0.0f);
            sba_Renderer->Draw(3, false, a_pFont, a_pFontMaterial, minString + ":" + secString, pos, PuRe_MatrixF(), size, 36.0f, c);
        }
    }


    // **************************************************************************
    // **************************************************************************
    CGUI::~CGUI()
    {
        SAFE_DELETE(this->m_pSpriteMaterial);
        SAFE_DELETE(this->m_pHitMarkerSprite);
        SAFE_DELETE(this->m_pCrossHairSprite);
        SAFE_DELETE(this->m_pMinimap);
        SAFE_DELETE(this->m_pUI);
    }
}