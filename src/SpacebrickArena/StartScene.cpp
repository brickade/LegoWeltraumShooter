#include "include/StartScene.h"
#include "include/Space.h"

namespace sba
{
    CStartScene::CStartScene(PuRe_Application* a_pApplication, int* a_pPlayerIdx)
    {
        this->m_pPlayerIdx = a_pPlayerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    void CStartScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_Timer* timer = a_pApplication->GetTimer();
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_IWindow* window = a_pApplication->GetWindow();
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();
        this->m_pUICamera = new PuRe_Camera(PuRe_Vector2F((float)1920, (float)1080), PuRe_CameraProjection::Orthogonal);
        this->m_pTeamLogo = new PuRe_Sprite(graphics, "../data/textures/teamlogo.png");
        this->m_pGALogo = new PuRe_Sprite(graphics, "../data/textures/GA_Logo.dds");
        this->m_pMaterial = graphics->LoadMaterial("../data/effects/sprite/default");
        this->m_Timer = 0.0f;
        this->m_Loading = false;

    }

    // **************************************************************************
    // **************************************************************************
    int CStartScene::Update(PuRe_Application* a_pApplication)
    {
        PuRe_IInput* input = a_pApplication->GetInput();
        PuRe_Timer* timer = a_pApplication->GetTimer();
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_IPlatform* platform = a_pApplication->GetPlatform();
        PuRe_IWindow* window = a_pApplication->GetWindow();
        this->m_Timer += 0.01f;

        //We should have drawn something, so now initialize
        float time = PuRe_PI*1.2f;
        if (!this->m_Loading&&this->m_Timer > time)
        {
            this->m_Loading = true;
            return 6;
        }

        time = PuRe_PI*1.7f;
        if (this->m_Timer > time)
            return 7;
        return 1;
    }

    // **************************************************************************
    // **************************************************************************
    void CStartScene::Render(PuRe_Application* a_pApplication)
    {
        PuRe_IGraphics* graphics = a_pApplication->GetGraphics();
        PuRe_Timer* timer = a_pApplication->GetTimer();
        PuRe_GraphicsDescription gdesc = graphics->GetDescription();
        graphics->Clear(PuRe_Color(0.0f,0.0f,0.0f,1.0f));
        PuRe_BoundingBox box;
        box.m_Position = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        box.m_Size = PuRe_Vector3F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight, 0.0f);
        graphics->Begin(box);
        PuRe_Vector3F size = PuRe_Vector3F(1920.0f / 4.0f, 1080.0f / 4.0f, 0.0f);

        graphics->SetMaterial(this->m_pMaterial);
        this->m_pMaterial->SetFloat(sin(this->m_Timer*2.0f), "alpha");
        if (this->m_Timer > PuRe_PI)
            this->m_pTeamLogo->Draw(this->m_pUICamera, this->m_pMaterial, size*2, size*2, PuRe_MatrixF::Identity());
        else
            this->m_pGALogo->Draw(this->m_pUICamera, this->m_pMaterial, size+size, size, PuRe_MatrixF::Identity());

        graphics->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CStartScene::Exit()
    {
        SAFE_DELETE(this->m_pMaterial);
        SAFE_DELETE(this->m_pUICamera);
        SAFE_DELETE(this->m_pTeamLogo);
        SAFE_DELETE(this->m_pGALogo);
    }
}