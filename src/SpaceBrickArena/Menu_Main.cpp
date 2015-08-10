#include "include/Menu_Main.h"
#include "include/Spaceship.h"

namespace Menu
{

    CMain::CMain(PuRe_IGraphics* a_pGraphics)
    {
        this->m_pNavigation = new sba::CNavigation(1, 4);
        this->m_HasShip = this->hasShip();
    }

    CMain::~CMain()
    {
        SAFE_DELETE(this->m_pNavigation);
    }

    bool CMain::hasShip()
    {
        bool hasShip = false;
        sba::CSpaceship* ship = NULL;
        int ID = 0;
        std::vector<TheBrick::CBrickInstance**> engines;
        std::vector<TheBrick::CBrickInstance**> cockpits;
        std::vector<TheBrick::CBrickInstance**> weapons;
        if (sba_ShipManager->GetShipCount() != 0)
        {
            while (ship == NULL)
            {
                bool last = false;
                int lastID = ID;
                ID++;
                if (lastID == sba_ShipManager->GetShipCount() - 1)
                {
                    ID = 0;
                    last = true;
                }
                engines.clear();
                cockpits.clear();
                weapons.clear();

                ship = sba_ShipManager->GetShip(ID);
                ship->GetEngines(engines);
                ship->GetCockpits(cockpits);
                ship->GetWeapons(weapons);
                SAFE_DELETE(ship);
                ship = NULL;
                if (engines.size() != 0 && cockpits.size() != 0 && weapons.size() != 0)
                {
                    hasShip = true;
                    break;
                }
                if (last)
                    break;
            }
        }
        return hasShip;
    }

    int CMain::Update(PuRe_Timer* a_pTimer, int a_PlayerIdx)
    {
        PuRe_Vector2F nav = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);
        nav.X = 0.0f;

        this->m_pNavigation->Update(*a_pTimer, nav);
        if (!this->m_HasShip)
        {
            int id = this->m_pNavigation->GetFocusedElementId();
            if (id == 0||id == 1)
            {
                if (nav.Y > 0.0f)
                    this->m_pNavigation->SetFocusedElementId(4);
                else
                    this->m_pNavigation->SetFocusedElementId(2);
            }
        }

        if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            this->m_HasShip = this->hasShip();
            switch (this->m_pNavigation->GetFocusedElementId())
            {
            case 0: //Local
                if (this->m_HasShip)
                    return 2;
                break;
            case 1: //Multiplayer
                if (this->m_HasShip)
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

    void CMain::Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        Position.X = 1920 / 2.0f;
        Position.Y = 1080;
        Position.Y -= 200.0f;

        a_pSpriteReader->Draw(1, a_pRenderer, "logo", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        Position.Y -= 450.0f;
        a_pSpriteReader->Draw(1, a_pRenderer, "menu_kasten", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        Position.Y += 200.0f;
        Position.X -= 125.0f;

        PuRe_Vector3F highlight = Position;
        PuRe_Vector3F size = PuRe_Vector3F(36.0f,36.0f,0.0f);
        highlight.X += 125.0f;
        PuRe_Color color = PuRe_Color(1.0f,1.0f,1.0f);
        if (!this->m_HasShip)
            color = PuRe_Color(0.3f, 0.3f, 0.3f);
        if (this->m_pNavigation->GetFocusedElementId() == 0)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "LOCAL", Position, PuRe_MatrixF::Identity(), size, 40.0f, color);
        Position.Y -= 96.0f;
        Position.X -= 100.0f;
        highlight.Y = Position.Y;
        if (this->m_pNavigation->GetFocusedElementId() == 1)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "MULTIPLAYER", Position, PuRe_MatrixF::Identity(), size, 40.0f, color);
        Position.X += 75.0f;
        Position.Y -= 96.0f;
        highlight.Y = Position.Y;
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        if (!this->m_HasShip)
        {
            color = PuRe_Color(0.9f, 1.0f, 0.9f);
            size = PuRe_Vector3F(40.0f, 40.0f, 0.0f);
        }
        if (this->m_pNavigation->GetFocusedElementId() == 2)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "EDITOR", Position, PuRe_MatrixF::Identity(), size, 40.0f, color);
        Position.Y -= 96.0f;
        highlight.Y = Position.Y;
        size = PuRe_Vector3F(36.0f, 36.0f, 0.0f);
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        if (this->m_pNavigation->GetFocusedElementId() == 3)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "OPTIONS", Position, PuRe_MatrixF::Identity(), size, 40.0f, color);
        Position.Y -= 96.0f;
        highlight.Y = Position.Y;
        if (this->m_pNavigation->GetFocusedElementId() == 4)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "QUIT", Position, PuRe_MatrixF::Identity(), size, 40.0f, color);

    }
}