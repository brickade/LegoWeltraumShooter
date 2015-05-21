#include "include/EditorScene.h"
namespace Game
{
    CEditorScene::CEditorScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_pApplication = a_pApplication;
        this->m_playerIdx = a_playerIdx;

        /*
        TheBrick::CSerializer serializer;
        serializer.OpenWrite("test.sbab");
        TheBrick::CBrick brick;
        brick.m_BrickId = 2;
        brick.m_Pivotoffset = PuRe_Vector3F(5, 5, 5);
        brick.Serialize(&serializer);
        serializer.Close();*/
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Initialize(PuRe_IGraphics* a_pGraphics,PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyBoxMaterial = a_pGraphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pRenderTarget = a_pGraphics->CreateRendertarget();

        this->m_pPointLightMaterial = a_pGraphics->LoadMaterial("../data/effects/PointLight/default");
        this->m_pPointLight = new PuRe_PointLight(a_pGraphics);

        this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, "../data/textures/skybox/");

        this->textureID = 0;

        this->m_pBrickSupervisor = new CBrickSupervisor(this->m_playerIdx);
        this->m_pBrickSupervisor->Initialize(*a_pGraphics);

        this->m_pBrickWorker = new CBrickWorker(this->m_playerIdx);
        this->m_pBrickWorker->Initialize(*a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    bool CEditorScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, this->m_playerIdx)
            || a_pInput->KeyPressed(a_pInput->F1)
            || a_pInput->GamepadPressed(a_pInput->Pad_Start, this->m_playerIdx))
        {
            return true;
        }

        if (a_pInput->KeyPressed(a_pInput->Comma))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 3;
        }
        else if (a_pInput->KeyPressed(a_pInput->Period))
        {
            this->textureID++;
            if (this->textureID > 3)
                this->textureID = 0;
        }

        this->m_pBrickSupervisor->Update(*a_pGraphics, *a_pWindow, *a_pInput, *a_pTimer, *a_pSoundPlayer);
        this->m_pBrickWorker->Update(*a_pGraphics, *a_pWindow, *a_pInput, *a_pTimer, *a_pSoundPlayer, this->m_pBrickSupervisor->GetSelectedBrick(), PuRe_Color(0.5f, 0.6f, 1.0f));

        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Renderer* renderer = BrickBozz::Instance()->Renderer;
        renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));
        //Lights
        renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(5.0f, 0.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f));
        renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(0.0f, 5.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f));
        renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(0.0f, 0.0f, 5.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f));
        renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(5.0f, 5.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f));
        renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(0.0f, 5.0f, 5.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f));
        renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0));
        //Skybox
        //renderer->Draw(this->m_pSkyBox, this->m_pSkyBoxMaterial);
        //Bricks
        //this->m_pBrickWorker->Render();
        BrickBozz::Instance()->BrickManager->Render(*BrickBozz::Instance()->Renderer);
        this->m_pBrickSupervisor->Render(*a_pGraphics);
        //Post
        renderer->Set((float)this->textureID, "textureID");
        renderer->Set(PuRe_Vector3F(0.02f, 0.02f, 0.02f), "ambient");
        PuRe_Vector3F size = PuRe_Vector3F(a_pGraphics->GetDescription().ResolutionWidth, a_pGraphics->GetDescription().ResolutionHeight, 0.0f);
        renderer->Render(this->m_pBrickWorker->GetCamera(), this->m_pPostMaterial, PuRe_Vector3F::Zero(), size);
        renderer->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Exit()
    {
        SAFE_DELETE(this->m_pBrickWorker);
        SAFE_DELETE(this->m_pBrickSupervisor);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pRenderTarget);
        SAFE_DELETE(this->m_pPostMaterial);
    }
}