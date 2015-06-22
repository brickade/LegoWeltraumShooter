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
        int RWidth = std::stoi(sba::CIniReader::Instance()->GetValue("ResolutionWidth"));
        int RHeight = std::stoi(sba::CIniReader::Instance()->GetValue("ResolutionHeight"));
        this->m_Resolution = 0;
        for (int i = 0; i < 7; i++)
        {
            if (this->m_Resolutions[i][0] == RWidth&&this->m_Resolutions[i][1] == RHeight)
                this->m_Resolution = i;
        }
        std::string DisplayMode = sba::CIniReader::Instance()->GetValue("DisplayMode");
        if (DisplayMode == "Windowed")
            this->m_DMode = PuRe_DisplaySetting::Windowed;
        else if (DisplayMode == "FullscreenWindowed")
            this->m_DMode = PuRe_DisplaySetting::FullscreenWindowed;
        else
            this->m_DMode = PuRe_DisplaySetting::Fullscreen;
        std::string ssao = sba::CIniReader::Instance()->GetValue("SSAO");
        this->m_SSAO = false;
        if (ssao == "On")
            this->m_SSAO = true;
    }

    COptions::~COptions()
    {
        SAFE_DELETE(this->m_pNavigation);
    }

    int COptions::Update(PuRe_Renderer* a_pRenderer,PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IGraphics* a_pGraphics, int a_PlayerIdx)
    {
        PuRe_Vector2F nav = sba_Input->Direction(sba_Direction::Navigate, a_PlayerIdx);
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
        PuRe_Vector2I size = PuRe_Vector2I(gdesc.ResolutionWidth, gdesc.ResolutionHeight);
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
                    if (nav.X > 0.0f)
                    {
                        printf("Change X > 0\n");
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
        if (sba_Input->ButtonPressed(sba_Button::NaviagtionSelect, a_PlayerIdx))
        {
            if (this->m_pNavigation->GetFocusedElementId() == 3)
                return 0;
            else
                this->m_Focus = true;
        }
        if (sba_Input->ButtonPressed(sba_Button::NavigationBack, a_PlayerIdx))
        {
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
                    sba::CIniReader::Instance()->SetValue("ResolutionWidth", std::to_string(size.X));
                    sba::CIniReader::Instance()->SetValue("ResolutionHeight", std::to_string(size.Y));
                    sba::CIniReader::Instance()->Save();
                    break;
                case 1: //DisplayMode
                    a_pWindow->ChangeDisplayMode(this->m_DMode);
                    a_pGraphics->ChangeResolution(size);
                    switch (this->m_DMode)
                    {
                    case PuRe_DisplaySetting::Windowed:
                        sba::CIniReader::Instance()->SetValue("DisplayMode", "Windowed");
                        break;
                    case PuRe_DisplaySetting::FullscreenWindowed:
                        sba::CIniReader::Instance()->SetValue("DisplayMode", "FullscreenWindowed");
                        break;
                    default:
                        sba::CIniReader::Instance()->SetValue("DisplayMode", "Fullscreen");
                        break;
                    }
                    break;
                case 2: //SSAO
                    if (this->m_SSAO)
                    {
                        sba_Renderer->SetSSAO(0, sba::Space::Instance()->m_SSAOMaterial, sba::Space::Instance()->m_pNoiseTexture);
                        sba_Renderer->SetSSAO(1, sba::Space::Instance()->m_SSAOMaterial, sba::Space::Instance()->m_pNoiseTexture);
                        sba::CIniReader::Instance()->SetValue("SSAO", "On");
                    }
                    else
                    {
                        sba_Renderer->SetSSAO(0, NULL,NULL);
                        sba_Renderer->SetSSAO(1, NULL, NULL);
                        sba::CIniReader::Instance()->SetValue("SSAO", "Off");
                    }
                    break;
                }
                sba::CIniReader::Instance()->Save();
            }
            else
                this->m_pNavigation->SetFocusedElementId(3);
            if (this->m_pNavigation->GetFocusedElementId() == 0)
                return 2;
        }
        return 1;
    }

    void COptions::Render(PuRe_Renderer* a_pRenderer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        Position.X = 1920 / 2.0f;
        Position.Y = 1080 / 2.0f;
        Position.X -= 800.0f;
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

        switch (this->m_DMode)
        {
        case PuRe_DisplaySetting::Windowed:
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Windowed", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
            break;
        case PuRe_DisplaySetting::FullscreenWindowed:
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "FullscreenWindowed", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
            break;
        default:
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Fullscreen", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
            break;
        }
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
        if (this->m_SSAO)
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "On", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        else
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Off", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.X -= 800.0f;

        Position.Y -= 128.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 3)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Back", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
    }
}