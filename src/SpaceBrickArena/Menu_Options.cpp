#include "include/Menu_Options.h"

namespace Menu
{

    COptions::COptions()
    {
        this->m_pNavigation = new sba::CNavigation(1, 3);
        this->m_pControls[0] = new sba::CNavigation(1, 3);
        this->m_pControls[1] = new sba::CNavigation(1, 3);
        this->m_pControls[2] = new sba::CNavigation(1, 3);
        this->m_pControls[3] = new sba::CNavigation(1, 3);
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
        int RWidth = std::stoi(sba_Options->GetValue("ResolutionWidth"));
        int RHeight = std::stoi(sba_Options->GetValue("ResolutionHeight"));
        this->m_Resolution = 0;
        for (int i = 0; i < 7; i++)
        {
            if (this->m_Resolutions[i][0] == RWidth&&this->m_Resolutions[i][1] == RHeight)
                this->m_Resolution = i;
        }
        std::string DisplayMode = sba_Options->GetValue("DisplayMode");
        if (DisplayMode == "Windowed")
            this->m_DMode = PuRe_DisplaySetting::Windowed;
        else if (DisplayMode == "FullscreenWindowed")
            this->m_DMode = PuRe_DisplaySetting::FullscreenWindowed;
        else
            this->m_DMode = PuRe_DisplaySetting::Fullscreen;
        std::string ssao = sba_Options->GetValue("SSAO");
        this->m_SSAO = false;
        if (ssao == "On")
            this->m_SSAO = true;
        this->m_Head = EHead::Settings;
    }

    COptions::~COptions()
    {
        SAFE_DELETE(this->m_pNavigation);
        SAFE_DELETE(this->m_pControls[0]);
        SAFE_DELETE(this->m_pControls[1]);
        SAFE_DELETE(this->m_pControls[2]);
        SAFE_DELETE(this->m_pControls[3]);
    }

    int COptions::Update(PuRe_Renderer* a_pRenderer,PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, int a_PlayerIdx)
    {
        PuRe_Vector2F nav = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);

        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
        PuRe_Vector2I size = PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight);
        if (!this->m_Focus)
        {
            if (nav.X == 0.0f)
                this->m_Switched = false;
            else if (!this->m_Switched&&!this->m_CFocus[0])
            {
                if (nav.X > 0.0f)
                {
                    this->m_Head++;
                    if (this->m_Head > Controls)
                        this->m_Head = 0;

                }
                else if (nav.X < 0.0f)
                {
                    this->m_Head--;
                    if (this->m_Head < Settings)
                        this->m_Head = Controls;

                }
                this->m_Switched = true;
            }
            nav.X = 0.0f;
            if (this->m_Head == Controls)
            {
                nav.Y = 0.0f;
                PuRe_Vector2F cnav;
                for (int i=0;i<4;i++)
                {
                    cnav = sba_Input->Direction(sba_Direction::Navigate, i);
                    if (!this->m_CFocus[i])
                    {
                        cnav.X = 0.0f;
                        if (i != 0)
                        {
                            if (cnav.Y < 0.0f&&this->m_pControls[i]->GetFocusedElementId() == 2)
                                this->m_pControls[i]->SetFocusedElementId(3);
                            else if (cnav.Y > 0.0f&&this->m_pControls[i]->GetFocusedElementId() == 0)
                                this->m_pControls[i]->SetFocusedElementId(3);
                        }
                        if (this->m_pControls[i]->Update(*a_pTimer, cnav))
                            sba_SoundPlayer->PlaySound("menu_over", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    }
                    else if (cnav.X != 0.0f)
                    {
                        switch (this->m_pControls[i]->GetFocusedElementId())
                        {
                        case 0: //Thrust
                            if (cnav.X > 0.0f)
                                sba_Controls[i].Thrust++;
                            else if (cnav.X < 0.0f)
                                sba_Controls[i].Thrust--;
                            if (sba_Controls[i].Thrust < 1) sba_Controls[i].Thrust = 1; else if (sba_Controls[i].Thrust > 3) sba_Controls[i].Thrust = 3;
                            sba_Options->SetValue("Thrust" + std::to_string(i + 1), std::to_string(sba_Controls[i].Thrust));
                            sba_Options->Save();
                            break;
                        case 1: //Move
                            if (cnav.X > 0.0f)
                                sba_Controls[i].Move++;
                            else if (cnav.X < 0.0f)
                                sba_Controls[i].Move--;
                            if (sba_Controls[i].Move < 1) sba_Controls[i].Move = 1; else if (sba_Controls[i].Move > 4) sba_Controls[i].Move = 4;
                            sba_Options->SetValue("Move" + std::to_string(i + 1), std::to_string(sba_Controls[i].Move));
                            sba_Options->Save();
                            break;
                        case 2: //Spin
                            if (cnav.X > 0.0f)
                                sba_Controls[i].Spin++;
                            else if (cnav.X < 0.0f)
                                sba_Controls[i].Spin--;
                            if (sba_Controls[i].Spin < 1) sba_Controls[i].Spin = 1; else if (sba_Controls[i].Spin > 2) sba_Controls[i].Spin = 2;
                            sba_Options->SetValue("Spin" + std::to_string(i + 1), std::to_string(sba_Controls[i].Spin));
                            sba_Options->Save();
                            break;
                        }
                    }
                }
                if (this->m_pControls[0]->GetFocusedElementId() == 3)
                    this->m_pNavigation->SetFocusedElementId(3);
                else
                    this->m_pNavigation->SetFocusedElementId(1);
            }
            else
            {
                if (this->m_pNavigation->Update(*a_pTimer, nav))
                    sba_SoundPlayer->PlaySound("menu_over", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            }
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
                    if (nav.X > 0.0f)
                    {
                        switch (this->m_DMode)
                        {
                        case PuRe_DisplaySetting::Windowed:
                            this->m_DMode = PuRe_DisplaySetting::FullscreenWindowed;
                            break;
                        case PuRe_DisplaySetting::FullscreenWindowed:
                            this->m_DMode = PuRe_DisplaySetting::Fullscreen;
                            break;
                        case PuRe_DisplaySetting::Fullscreen:
                            this->m_DMode = PuRe_DisplaySetting::Windowed;
                            break;
                        }
                        this->m_Switched = true;
                    }
                    else if (nav.X < 0.0f)
                    {
                        switch (this->m_DMode)
                        {
                        case PuRe_DisplaySetting::Windowed:
                            this->m_DMode = PuRe_DisplaySetting::Fullscreen;
                            break;
                        case PuRe_DisplaySetting::FullscreenWindowed:
                            this->m_DMode = PuRe_DisplaySetting::Windowed;
                            break;
                        case PuRe_DisplaySetting::Fullscreen:
                            this->m_DMode = PuRe_DisplaySetting::FullscreenWindowed;
                            break;
                        }
                        this->m_Switched = true;
                    }
                    break;
                case 2: //SSAO
                    if (nav.X != 0.0f)
                    {
                        if (this->m_SSAO)
                            this->m_SSAO = false;
                        else
                            this->m_SSAO = true;
                        this->m_Switched = true;
                    }
                    break;
                }
            }
        }


        bool p1focus = this->m_CFocus[0];
        if (this->m_Head == Controls)
        {
            for (int i=0;i<4;i++)
            {
                if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, i))
                {
                    sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (i == 0&&this->m_pNavigation->GetFocusedElementId() != 3)
                        this->m_CFocus[i] = true;
                    else
                        this->m_CFocus[i] = true;

                }
                if (sba_Input->ButtonPressed(sba_Button::NavigationBack, i))
                {
                    sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (this->m_CFocus[i])
                        this->m_CFocus[i] = false;
                }
            }
        }
        if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            if (this->m_pNavigation->GetFocusedElementId() == 3)
                return 0;
            if (this->m_Head != Controls)
                this->m_Focus = true;
        }
        if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
        {
            sba_SoundPlayer->PlaySound("menu_back", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
            if (this->m_Focus)
            {
                this->m_Focus = false;
                switch (this->m_pNavigation->GetFocusedElementId())
                {
                case 0: //Resolution
                    size = PuRe_Vector2I(this->m_Resolutions[this->m_Resolution][0], this->m_Resolutions[this->m_Resolution][1]);
                    a_pGraphics->ChangeResolution(size);
                    a_pRenderer->DeleteTargets();
                    a_pRenderer->AddTarget(size);
                    a_pRenderer->AddTarget(size);
                    if (this->m_SSAO)
                    {
                        a_pRenderer->SetSSAO(0, sba::Space::Instance()->m_SSAOMaterial, sba::Space::Instance()->m_pNoiseTexture);
                        a_pRenderer->SetSSAO(1, sba::Space::Instance()->m_SSAOMaterial, sba::Space::Instance()->m_pNoiseTexture);
                    }
                    sba_Options->SetValue("ResolutionWidth", std::to_string(size.X));
                    sba_Options->SetValue("ResolutionHeight", std::to_string(size.Y));
                    sba_Options->Save();
                    break;
                case 1: //DisplayMode
                    a_pWindow->ChangeDisplayMode(this->m_DMode);
                    a_pGraphics->ChangeResolution(size);
                    switch (this->m_DMode)
                    {
                    case PuRe_DisplaySetting::Windowed:
                        sba_Options->SetValue("DisplayMode", "Windowed");
                        break;
                    case PuRe_DisplaySetting::FullscreenWindowed:
                        sba_Options->SetValue("DisplayMode", "FullscreenWindowed");
                        break;
                    default:
                        sba_Options->SetValue("DisplayMode", "Fullscreen");
                        break;
                    }
                    break;
                case 2: //SSAO
                    if (this->m_SSAO)
                    {
                        sba_Renderer->SetSSAO(0, sba::Space::Instance()->m_SSAOMaterial, sba::Space::Instance()->m_pNoiseTexture);
                        sba_Renderer->SetSSAO(1, sba::Space::Instance()->m_SSAOMaterial, sba::Space::Instance()->m_pNoiseTexture);
                        sba_Options->SetValue("SSAO", "On");
                    }
                    else
                    {
                        sba_Renderer->SetSSAO(0, NULL,NULL);
                        sba_Renderer->SetSSAO(1, NULL, NULL);
                        sba_Options->SetValue("SSAO", "Off");
                    }
                    break;
                }
                sba_Options->Save();
            }
            else if (!p1focus)
            {
                this->m_pNavigation->SetFocusedElementId(3);
                this->m_pControls[0]->SetFocusedElementId(3);
                if (this->m_pNavigation->GetFocusedElementId() == 0)
                    return 2;
            }
        }
        return 1;
    }

    void COptions::Render(PuRe_Renderer* a_pRenderer, sba::CSpriteReader* a_pSpriteReader, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        //DRAW UI
        Position.X = 1920 / 2.0f;
        Position.Y = 1080 / 2.0f;
        a_pSpriteReader->Draw(1, a_pRenderer, "kasten_gross", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pSpriteReader->Draw(1, a_pRenderer, "options_kasten", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        //Draw Bottom
        Position.X = 150.0f;
        Position.Y = 100.0f;
        PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
        if (this->m_pNavigation->GetFocusedElementId() == 3)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        //DRAW HEADER
        Position.X = 100.0f;
        Position.Y = 1080.0f;
        Position.Y -= 150.0f;
        if (this->m_Head == Settings)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "SETTINGS", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        Position.X += 500.0f;
        if (this->m_Head == Controls)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "CONTROLS", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        //DRAW SPECIFIC
        Position.Y -= 100.0f;
        Position.X = 100.0f;
        switch (this->m_Head)
        {
        case Settings:
            if (this->m_pNavigation->GetFocusedElementId() == 0 && !this->m_Focus)
                color = PuRe_Color(1.0f, 0.0f, 0.0f);
            else
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "RESOLUTION", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 0 && this->m_Focus)
                color = PuRe_Color(1.0f, 0.0f, 0.0f);
            else
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, std::to_string(this->m_Resolutions[this->m_Resolution][0]) + "x" + std::to_string(this->m_Resolutions[this->m_Resolution][1]), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X -= 800.0f;

            Position.Y -= 64.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 1 && !this->m_Focus)
                color = PuRe_Color(1.0f, 0.0f, 0.0f);
            else
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "DISPLAYMODE", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
                color = PuRe_Color(1.0f, 0.0f, 0.0f);
            else
                color = PuRe_Color(1.0f, 1.0f, 1.0f);

            switch (this->m_DMode)
            {
            case PuRe_DisplaySetting::Windowed:
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "WINDOWED", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
                break;
            case PuRe_DisplaySetting::FullscreenWindowed:
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "FULLSCREENWINDOWED", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
                break;
            default:
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "FULLSCREEN", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
                break;
            }
            Position.X -= 800.0f;

            Position.Y -= 64.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 2 && !this->m_Focus)
                color = PuRe_Color(1.0f, 0.0f, 0.0f);
            else
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "SSAO", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 2 && this->m_Focus)
                color = PuRe_Color(1.0f, 0.0f, 0.0f);
            else
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
            if (this->m_SSAO)
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "ON", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            else
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "OFF", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            break;
        case Controls:
            for (int i=0;i<4;i++)
            {
                Position.Y = 800.0f - 300.0f*(float)std::floor(i/2);
                Position.X = 100.0f + (i % 2)*850.0f;
                color = PuRe_Color(1.0f, 1.0f, 1.0f);
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player" + std::to_string(i+1), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(30.0f, 30.0f, 0.0f), 40.0f, color);
                Position.Y -= 64.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 0 && !this->m_CFocus[i])
                    color = PuRe_Color(1.0f, 0.0f, 0.0f);
                else
                    color = PuRe_Color(1.0f, 1.0f, 1.0f);
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Thrust", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(28.0f, 28.0f, 0.0f), 38.0f, color);
                Position.X += 320.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 0 && this->m_CFocus[i])
                    color = PuRe_Color(1.0f, 0.0f, 0.0f);
                else
                    color = PuRe_Color(1.0f, 1.0f, 1.0f);
                switch (sba_Controls[i].Thrust)
                {
                case 1:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Left Trigger", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                case 2:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Right Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                case 3:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Left Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                }
                Position.X -= 320.0f;
                Position.Y -= 64.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 1 && !this->m_CFocus[i])
                    color = PuRe_Color(1.0f, 0.0f, 0.0f);
                else
                    color = PuRe_Color(1.0f, 1.0f, 1.0f);
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Movement", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(28.0f, 28.0f, 0.0f), 38.0f, color);
                Position.X += 320.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 1 && this->m_CFocus[i])
                    color = PuRe_Color(1.0f, 0.0f, 0.0f);
                else
                    color = PuRe_Color(1.0f, 1.0f, 1.0f);
                switch (sba_Controls[i].Move)
                {
                case 1:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Right Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                case 2:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Left Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                case 3:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Inv Right Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                case 4:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Inv Left Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                }
                Position.X -= 320.0f;
                Position.Y -= 64.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 2 && !this->m_CFocus[i])
                    color = PuRe_Color(1.0f, 0.0f, 0.0f);
                else
                    color = PuRe_Color(1.0f, 1.0f, 1.0f);
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Spin", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(28.0f, 28.0f, 0.0f), 38.0f, color);
                Position.X += 320.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 2 && this->m_CFocus[i])
                    color = PuRe_Color(1.0f, 0.0f, 0.0f);
                else
                    color = PuRe_Color(1.0f, 1.0f, 1.0f);
                switch (sba_Controls[i].Spin)
                {
                case 1:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Left Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                case 2:
                    a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Right Thumb", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(20.0f, 20.0f, 0.0f), 30.0f, color);
                    break;
                }
            }
            break;
        }
    }
}