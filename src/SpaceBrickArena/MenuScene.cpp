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

        this->m_pNavigation = new sba::CNavigation(1, 3);

    }

    // **************************************************************************
    // **************************************************************************
    bool CMenuScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_IInput* input = a_pApplication->GetInput();
        PuRe_Timer& timer = *a_pApplication->GetTimer();
        if (input->KeyPressed(a_pApplication->GetInput()->ESC))
            return true;
        
        if (*this->m_pPlayerIdx == -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (input->GamepadPressed(input->Pad_A, i))
                {
                    *this->m_pPlayerIdx = i;
                    break;
                }
            }
        }
        else
        {
            this->m_pNavigation->Update(timer, sba_Input->Direction(sba_Direction::EditorMoveBrick, *this->m_pPlayerIdx));
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
        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CMenuScene::Render(PuRe_Application* a_pApplication)
    {
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();
        PuRe_Renderer* renderer = sba::Space::Instance()->Renderer;


        renderer->Begin(PuRe_Color(0.1f, 0.4f, 1.0f));

        renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyMaterial);

        PuRe_Vector3F Position;
        Position.X =  gdesc.ResolutionWidth/2.0f;
        Position.Y = gdesc.ResolutionHeight/2.0f;
        Position.X -= 100.0f;
        Position.Y += 200.0f;

        PuRe_Color color = PuRe_Color();

        if (this->m_pNavigation->GetFocusedElementId() == 0)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f,1.0f,1.0f);
        renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Game", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 1)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Editor", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 2)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Options", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);
        Position.Y -= 64.0f;
        if (this->m_pNavigation->GetFocusedElementId() == 3)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        renderer->Draw(1, false, this->m_pFont, this->m_pFontMaterial, "Quit", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);

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
        SAFE_DELETE(this->m_pNavigation);
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