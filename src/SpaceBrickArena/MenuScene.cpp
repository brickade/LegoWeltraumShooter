#include "include/MenuScene.h"

namespace Menu
{
    CMenuScene::CMenuScene(PuRe_Application* a_pApplication, int* a_pPlayerIdx)
    {
        this->m_pApplication = a_pApplication;
        this->m_pPlayerIdx = a_pPlayerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    void CMenuScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_IWindow* window = a_pApplication->GetWindow();
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();

        this->m_pSkyMaterial = graphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pPostMaterial = graphics->LoadMaterial("../data/effects/Post/default");
        this->m_pFontMaterial = graphics->LoadMaterial("../data/effects/font/default");

        this->m_pSkyBox = new PuRe_SkyBox(graphics, "../data/textures/cube/");
        this->m_pFont = new PuRe_Font(graphics, "../data/textures/font.png");

        //Camera
        sba_Renderer->DeleteTargets();
        for (int i = 0; i < 3; i++)
        {
            sba_Renderer->AddTarget(PuRe_Vector2I(graphics->GetDescription().ResolutionWidth, graphics->GetDescription().ResolutionHeight));
        }
        if (sba_Options->GetValue("SSAO") == "On")
        {
            sba_Renderer->SetSSAO(0, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
            sba_Renderer->SetSSAO(1, sba_Space->m_SSAOMaterial, sba_Space->m_pNoiseTexture);
        }

        this->m_pSceneCamera = new PuRe_Camera(PuRe_Vector2F((float)1920, (float)1080), PuRe_CameraProjection::Perspective);
        this->m_pUICamera = new PuRe_Camera(PuRe_Vector2F((float)1920, (float)1080), PuRe_CameraProjection::Orthogonal);

        this->m_Displayed = Main;
        this->m_pMainMenu = new CMain();
        this->m_pOptions = new COptions();
        this->m_pLobby = new CLobby(window);
        this->m_pNetwork = new CNetwork(a_pApplication->GetTimer());

    }

    // **************************************************************************
    // **************************************************************************
    int CMenuScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_IInput* input = a_pApplication->GetInput();
        PuRe_Timer* timer = a_pApplication->GetTimer();
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_IPlatform* platform = a_pApplication->GetPlatform();
        PuRe_IWindow* window = a_pApplication->GetWindow();

        int result = 0;
        switch (this->m_Displayed)
        {
        case Main:
            if (sba_Input->ButtonPressed(sba_Button::Exit, *this->m_pPlayerIdx))
            {
                return 0;
            }
            result = this->m_pMainMenu->Update(timer, *this->m_pPlayerIdx);
            switch (result)
            {
            case 0: //Quit
                return 0;
                break;
            case 2: //local
                    //create first player
                this->m_Displayed = Lobby;
                sba_Space->CreatePlayer(0, window);
                break;
            case 3: //Multiplayer
                this->m_Displayed = Network;
                this->m_pNetwork->Start();
                break;
            case 4: //Editor
                return 4;
                break;
            case 5: //Editor
                this->m_Displayed = Options;
                break;
            default:
                break;
            }
            break;
        case Options:
            result = this->m_pOptions->Update(sba_Renderer, timer, window, graphics, *this->m_pPlayerIdx);
            if (result == 0)
                this->m_Displayed = Main;
            break;
        case Lobby:
            result = this->m_pLobby->Update(timer, window, input, *this->m_pPlayerIdx);
            if (result == 2)
                return 2; //game Local
            else if (result == 0)
            {   //delete all players that are connected yet
                for (unsigned int i = 0; i < sba_Players.size(); i++)
                {
                    sba_Space->DeletePlayer(i);
                    --i;
                }
                sba_Network->Disconnect();
                this->m_Displayed = Main;
            }
            break;
        case Network:
            result = this->m_pNetwork->Update(timer, window, input, *this->m_pPlayerIdx);
            if (result == 2)
            {
                this->m_Displayed = Lobby;
                this->m_pLobby->Start();
            }
            if (result == 0)
                this->m_Displayed = Main;
            break;
        default:
            break;
        }

        if (input->KeyPressed(a_pApplication->GetInput()->Comma))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 4;
        }
        else if (input->KeyPressed(a_pApplication->GetInput()->Period))
        {
            this->textureID++;
            if (this->textureID > 4)
                this->textureID = 0;
        }
        return 1;
    }

    // **************************************************************************
    // **************************************************************************
    void CMenuScene::Render(PuRe_Application* a_pApplication)
    {
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_Timer* timer = a_pApplication->GetTimer();
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();
        PuRe_Renderer* renderer = sba::Space::Instance()->Renderer;

        PuRe_Vector2F resolution = PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight);

        renderer->Begin(PuRe_Color(0.0f, 0.0f, 0.0f));

        renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyMaterial);
        sba_BrickManager->Render();

        switch (this->m_Displayed)
        {
        case Main:
            this->m_pMainMenu->Render(renderer, this->m_pFont, this->m_pFontMaterial, resolution);
            break;
        case Options:
            this->m_pOptions->Render(renderer, this->m_pFont, this->m_pFontMaterial, resolution);
            break;
        case Lobby:
            this->m_pLobby->Render(renderer, timer, this->m_pFont, this->m_pFontMaterial, resolution);
            break;
        case Network:
            this->m_pNetwork->Render(renderer, timer, this->m_pFont, this->m_pFontMaterial, resolution);
            break;
        default:
            break;
        }
        renderer->Render(0, 0, this->m_pSceneCamera, this->m_pPostMaterial, sba_FinalMaterial);
        renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
        renderer->Render(0, 1, this->m_pUICamera, this->m_pPostMaterial, sba_FinalMaterial);
        renderer->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CMenuScene::Exit()
    {
        /////// CAMERAS ///////
        SAFE_DELETE(this->m_pLobby);
        SAFE_DELETE(this->m_pOptions);
        SAFE_DELETE(this->m_pNetwork);
        SAFE_DELETE(this->m_pMainMenu);
        SAFE_DELETE(this->m_pSceneCamera);
        SAFE_DELETE(this->m_pUICamera);
        /////// OBJECTS ///////
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pFont);
        /////// MATERIALS ///////
        SAFE_DELETE(this->m_pSkyMaterial);
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
    }
}