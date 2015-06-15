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
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();

        this->m_pSkyMaterial = graphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pPostMaterial = graphics->LoadMaterial("../data/effects/Post/default");
        this->m_pFontMaterial = graphics->LoadMaterial("../data/effects/font/default");

        this->m_pSkyBox = new PuRe_SkyBox(graphics, "../data/textures/cube/");
        this->m_pFont = new PuRe_Font(graphics, "../data/textures/font.png");

        this->m_pSceneCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_CameraProjection::Perspective);
        this->m_pUICamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_CameraProjection::Orthogonal);

        this->m_Displayed = Main;
        this->m_pMainMenu = new CMain();
        this->m_pOptions = new COptions();

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

        if (input->KeyPressed(a_pApplication->GetInput()->ESC))
            return 0;

        int result = 0;
        switch (this->m_Displayed)
        {
        case Main:
            result = this->m_pMainMenu->Update(timer, *this->m_pPlayerIdx);
            if (result == 4)
                this->m_Displayed = Options;
            else
                return result;
            break;
        case Options:
            result = this->m_pOptions->Update(timer, *this->m_pPlayerIdx);
            if (result == 0)
                this->m_Displayed = Main;
            if (result == 2)
            {
                //SAFE_DELETE(graphics);
                //PuReEngine::Core::SGraphicsDescription gdesc;
                //gdesc.Module = PuRe_GraphicsModuleType::OpenGL;
                //gdesc.ResolutionWidth = this->m_pOptions->m_Resolutions[this->m_pOptions->m_Resolution][0];
                //gdesc.ResolutionHeight = this->m_pOptions->m_Resolutions[this->m_pOptions->m_Resolution][1];
                //platform->PlatformCreateGraphics(window, gdesc);
            }
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

        renderer->Begin(PuRe_Color(0.1f, 0.4f, 1.0f));

        renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyMaterial);

        if (*this->m_pPlayerIdx == -1)
        {
            float offset = 36.0f;
            std::string text = "Press A Button ...";
            PuRe_Vector3F position;
            position.X = resolution.X / 2.0f;
            position.Y = resolution.Y / 2.0f;
            position.X -= text.length()*offset / 2.0f;
            PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
            color.A = sin(timer->GetTotalElapsedSeconds()*2.0f) + 1.2f;
            if (color.A > 1.0f)
                color.A = 1.0f;
            renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, text, position, PuRe_MatrixF::Identity(), PuRe_Vector3F(offset, offset, 0.0f), offset, color);
        }
        else
        {
            switch (this->m_Displayed)
            {
            case Main:
                this->m_pMainMenu->Render(renderer, this->m_pFont, this->m_pFontMaterial, resolution);
            break;
            case Options:
                this->m_pOptions->Render(renderer, this->m_pFont, this->m_pFontMaterial, resolution);
            break;
            default:
            break;
            }
        }

        renderer->Render(0, this->m_pSceneCamera, this->m_pPostMaterial);
        renderer->Set(1, PuRe_Vector3F(1.0f, 1.0f, 1.0f), "ambient");
        renderer->Render(1, this->m_pUICamera, this->m_pPostMaterial);
        renderer->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CMenuScene::Exit()
    {
        /////// CAMERAS ///////
        SAFE_DELETE(this->m_pOptions);
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