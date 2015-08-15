#include "include/Menu_Options.h"

namespace Menu
{

    COptions::COptions()
    {
        this->m_pNavigation = new sba::CNavigation(1, 5);
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
        this->m_Resolutions[7][0] = 2560;
        this->m_Resolutions[7][1] = 1440;
        this->m_Resolutions[8][0] = 3840;
        this->m_Resolutions[8][1] = 2160;
        int RWidth = std::stoi(sba_Options->GetValue("ResolutionWidth"));
        int RHeight = std::stoi(sba_Options->GetValue("ResolutionHeight"));
        this->m_Resolution = 0;
        for (int i = 0; i < 9; i++)
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
        this->m_Sound = std::stof(sba_Options->GetValue("SoundVolume"));
        this->m_Music = std::stof(sba_Options->GetValue("MusicVolume"));
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

    int COptions::Update(PuRe_IInput* a_pInput,PuRe_Renderer* a_pRenderer, PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, int a_PlayerIdx)
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
                int pads = a_pInput->GetGamepads() + 1;
                if (pads > 4) pads = 4;
                for (int i = 0; i < pads; i++)
                {
                    bool left = sba_Input->ButtonPressed(sba::Input::EButton::MenuLeft, i);
                    bool right = sba_Input->ButtonPressed(sba::Input::EButton::MenuRight, i);
                    bool up = sba_Input->ButtonPressed(sba::Input::EButton::MenuUp, i);
                    bool down = sba_Input->ButtonPressed(sba::Input::EButton::MenuDown, i);
                    if (!this->m_CFocus[i])
                    {
                        cnav.X = 0.0f;
                        int element = this->m_pControls[i]->GetFocusedElementId();
                        if (up)
                            element--;
                        else if (down)
                            element++;
                        if (i == 0)
                        {
                            if (element > 2)
                                this->m_pNavigation->SetFocusedElementId(5);
                            else if (element < 0)
                                this->m_pNavigation->SetFocusedElementId(0);
                            else
                                this->m_pNavigation->SetFocusedElementId(3);
                        }
                        else
                        {
                            if (element > 2)
                                element = 0;
                            else if (element < 0)
                                element = 2;
                        }
                        this->m_pControls[i]->SetFocusedElementId(element);
                    }
                    else if (left||right)
                    {
                        switch (this->m_pControls[i]->GetFocusedElementId())
                        {
                        case 0: //Thrust
                            if (right)
                                sba_Controls[i].Thrust++;
                            else if (left)
                                sba_Controls[i].Thrust--;
                            if (sba_Controls[i].Thrust < 1) sba_Controls[i].Thrust = 1; else if (sba_Controls[i].Thrust > 3) sba_Controls[i].Thrust = 3;
                            sba_Options->SetValue("Thrust" + std::to_string(i + 1), std::to_string(sba_Controls[i].Thrust));
                            sba_Options->Save();
                            break;
                        case 1: //Move
                            if (right)
                                sba_Controls[i].Move++;
                            else if (left)
                                sba_Controls[i].Move--;
                            if (sba_Controls[i].Move < 1) sba_Controls[i].Move = 1; else if (sba_Controls[i].Move > 4) sba_Controls[i].Move = 4;
                            sba_Options->SetValue("Move" + std::to_string(i + 1), std::to_string(sba_Controls[i].Move));
                            sba_Options->Save();
                            break;
                        case 2: //Spin
                            if (right)
                                sba_Controls[i].Spin++;
                            else if (left)
                                sba_Controls[i].Spin--;
                            if (sba_Controls[i].Spin < 1) sba_Controls[i].Spin = 1; else if (sba_Controls[i].Spin > 2) sba_Controls[i].Spin = 2;
                            sba_Options->SetValue("Spin" + std::to_string(i + 1), std::to_string(sba_Controls[i].Spin));
                            sba_Options->Save();
                            break;
                        }
                    }
                }
            }
            else
            {
                this->m_pNavigation->Update(*a_pTimer, nav);
            }
        }
        else
        {
            bool left = sba_Input->ButtonPressed(sba::Input::EButton::MenuLeft, a_PlayerIdx);
            bool right = sba_Input->ButtonPressed(sba::Input::EButton::MenuRight, a_PlayerIdx);
            if (nav.X == 0.0f&&!left&&!right)
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
                    if (this->m_Resolution > 8)
                        this->m_Resolution = 0;
                    else if (this->m_Resolution < 0)
                        this->m_Resolution = 8;
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
                case 3: //Music
                    if (right)
                        this->m_Music += 0.01f;
                    else if (left)
                        this->m_Music -= 0.01f;

                    if (this->m_Music > 1.0f)
                        this->m_Music = 1.0f;
                    else if (this->m_Music < 0.0f)
                        this->m_Music = 0.0f;
                    break;
                case 4: //Sound
                    if (right)
                        this->m_Sound += 0.01f;
                    else if (left)
                        this->m_Sound -= 0.01f;

                    if (this->m_Sound > 1.0f)
                        this->m_Sound = 1.0f;
                    else if (this->m_Sound < 0.0f)
                        this->m_Sound = 0.0f;
                    break;
                }
            }
        }


        bool p1focus = this->m_CFocus[0];
        if (this->m_Head == Controls)
        {
            for (int i = 0; i < 4; i++)
            {
                if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, i))
                {
                    sba_SoundPlayer->PlaySound("menu_confirm", false, true, std::stof(sba_Options->GetValue("SoundVolume")));
                    if (i == 0 && this->m_pNavigation->GetFocusedElementId() != 5)
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
            if (this->m_pNavigation->GetFocusedElementId() == 5)
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
                    sba_Options->SetValue("ResolutionWidth", std::to_string(size.X));
                    sba_Options->SetValue("ResolutionHeight", std::to_string(size.Y));
                    sba_Space->ChangeRenderResolution();
                    sba_Options->Save();
                    break;
                case 1: //DisplayMode
                    a_pWindow->ChangeDisplayMode(this->m_DMode, PuRe_Vector2I(std::stoi(sba_Options->GetValue("WindowWidth")), std::stoi(sba_Options->GetValue("WindowHeight"))));
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
                        sba_Options->SetValue("SSAO", "On");
                    }
                    else
                    {
                        sba_Renderer->SetSSAO(0, NULL, NULL);
                        sba_Options->SetValue("SSAO", "Off");
                    }
                    break;
                case 3: //Music
                    for (unsigned i = 0; i<32; i++)
                        sba_SoundPlayer->SetVolume(0, this->m_Music);
                    sba_Options->SetValue("MusicVolume", std::to_string(this->m_Music));
                    break;
                case 4: //Sound
                    sba_Options->SetValue("SoundVolume", std::to_string(this->m_Sound));
                    break;
                }
                sba_Options->Save();
            }
            else if (!p1focus)
            {
                this->m_pNavigation->SetFocusedElementId(5);
                this->m_pControls[0]->SetFocusedElementId(5);
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

        PuRe_Vector3F highlight = Position;
        highlight.X += 125.0f;

        PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
        if (this->m_pNavigation->GetFocusedElementId() == 5)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "BACK", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        //DRAW HEADER
        Position.X = 100.0f;
        Position.Y = 1080.0f;
        Position.Y -= 150.0f;

        highlight = Position;
        highlight.X += 200.0f;
        if (this->m_Head == Settings)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "SETTINGS", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        Position.X += 500.0f;
        highlight.X += 500.0f;
        if (this->m_Head == Controls)
            a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "CONTROLS", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
        //DRAW SPECIFIC
        Position.Y -= 100.0f;
        Position.X = 100.0f;
        switch (this->m_Head)
        {
        case Settings:
            highlight = Position;
            highlight.X += 200.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 0 && !this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "RESOLUTION", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            highlight.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 0 && this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, std::to_string(this->m_Resolutions[this->m_Resolution][0]) + "x" + std::to_string(this->m_Resolutions[this->m_Resolution][1]), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X -= 800.0f;
            highlight.X -= 800.0f;
            highlight.Y -= 64.0f;
            Position.Y -= 64.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 1 && !this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "DISPLAYMODE", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            highlight.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 1 && this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));

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
            highlight.X -= 800.0f;
            highlight.Y -= 64.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 2 && !this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "SSAO", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            highlight.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 2 && this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            if (this->m_SSAO)
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "ON", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            else
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "OFF", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X -= 800.0f;
            Position.Y -= 64.0f;
            highlight.X -= 800.0f;
            highlight.Y -= 64.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 3 && !this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Music", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            highlight.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 3 && this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, std::to_string((int)(100 * this->m_Music)), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X -= 800.0f;
            Position.Y -= 64.0f;
            highlight.X -= 800.0f;
            highlight.Y -= 64.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 4 && !this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Sound", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);
            Position.X += 800.0f;
            highlight.X += 800.0f;
            if (this->m_pNavigation->GetFocusedElementId() == 4 && this->m_Focus)
                a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.5f, 0.5f));
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, std::to_string((int)(100 * this->m_Sound)), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 46.0f, color);

            break;
        case Controls:
            for (int i = 0; i < 4; i++)
            {
                Position.Y = 800.0f - 300.0f*(float)std::floor(i / 2);
                Position.X = 100.0f + (i % 2)*850.0f;
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player" + std::to_string(i + 1), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(30.0f, 30.0f, 0.0f), 40.0f, color);
                Position.Y -= 64.0f;
                highlight = Position;
                highlight.X += 150.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 0 && !this->m_CFocus[i])
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.3f, 0.5f));
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Thrust", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(28.0f, 28.0f, 0.0f), 38.0f, color);
                Position.X += 320.0f;
                highlight.X += 320.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 0 && this->m_CFocus[i])
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.4f, 0.5f));
                Position.X += 50.0f;
                switch (sba_Controls[i].Thrust)
                {
                case 1:
                    sba_Buttons->Draw(1, a_pRenderer, "LT", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    Position.X += 80.0f;
                    Position.Y -= 5.0f;
                    sba_Buttons->Draw(1, a_pRenderer, "left_shoulder", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    Position.X -= 80.0f;
                    Position.Y += 5.0f;
                    break;
                case 2:
                    sba_Buttons->Draw(1, a_pRenderer, "right_stick", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                case 3:
                    sba_Buttons->Draw(1, a_pRenderer, "left_stick", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                }
                highlight.X -= 320.0f;
                Position.X -= 370.0f;
                Position.Y -= 64.0f;
                highlight.Y -= 64.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 1 && !this->m_CFocus[i])
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.3f, 0.5f));
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Movement", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(28.0f, 28.0f, 0.0f), 38.0f, color);
                Position.X += 320.0f;
                highlight.X += 320.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 1 && this->m_CFocus[i])
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.4f, 0.5f));
                Position.X += 50.0f;
                switch (sba_Controls[i].Move)
                {
                case 1:
                    sba_Buttons->Draw(1, a_pRenderer, "right_stick", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                case 2:
                    sba_Buttons->Draw(1, a_pRenderer, "left_stick", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                case 3:
                    sba_Buttons->Draw(1, a_pRenderer, "right_stick", a_pFontMaterial, Position, PuRe_Vector3F(0.0f, 0.0f, 180 * PuRe_DegToRad), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                case 4:
                    sba_Buttons->Draw(1, a_pRenderer, "left_stick", a_pFontMaterial, Position, PuRe_Vector3F(0.0f, 0.0f, 180 * PuRe_DegToRad), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                }
                Position.X -= 370.0f;
                Position.Y -= 64.0f;
                highlight.X -= 320.0f;
                highlight.Y -= 64.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 2 && !this->m_CFocus[i])
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.3f, 0.5f));
                a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Spin", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(28.0f, 28.0f, 0.0f), 38.0f, color);
                Position.X += 320.0f;
                highlight.X += 320.0f;
                if (this->m_pControls[i]->GetFocusedElementId() == 2 && this->m_CFocus[i])
                    a_pSpriteReader->Draw(1, a_pRenderer, "auswahl_highlight_menue", a_pFontMaterial, highlight, PuRe_Vector3F(), -1, PuRe_Vector2F(0.4f, 0.5f));
                Position.X += 50.0f;
                switch (sba_Controls[i].Spin)
                {
                case 1:
                    sba_Buttons->Draw(1, a_pRenderer, "left_stick", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                case 2:
                    sba_Buttons->Draw(1, a_pRenderer, "right_stick", a_pFontMaterial, Position, PuRe_Vector3F(), -1, PuRe_Vector2F(0.25f, 0.25f));
                    break;
                }
            }
            break;
        }
    }
}