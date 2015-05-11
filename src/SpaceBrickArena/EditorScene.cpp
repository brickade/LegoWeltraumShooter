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

        //Camera
        this->m_pCamera = new CEditorCamera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective, this->m_playerIdx);
        this->m_pCamera->Initialize();
        this->m_pCamera->Rotate(10, 0, 0);
        this->m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyBoxMaterial = a_pGraphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pRenderTarget = a_pGraphics->CreateRendertarget();

        this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, "../data/textures/skybox/");

        this->textureID = 0;

        this->m_pBrickSupervisor = new CBrickSupervisor(this->m_playerIdx);
        this->m_pBrickSupervisor->Initialize();
        this->m_pBrickSupervisor->m_pActiveCategory = this->m_pBrickSupervisor->m_Categories[0];

        this->m_pBrickWorker = new CBrickWorker(this->m_playerIdx);
        this->m_pBrickWorker->Initialize(a_pGraphics);
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

        if (a_pInput->KeyPressed(a_pInput->Left))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 2;
        }
        else if (a_pInput->KeyPressed(a_pInput->Right))
        {
            this->textureID++;
            if (this->textureID > 2)
                this->textureID = 0;
        }

        this->m_pCamera->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer);

        this->m_pBrickWorker->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer, this->m_pCamera->GetForward());
        this->m_pBrickSupervisor->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer);

        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Color clearColor = PuRe_Color(0.1f, 0.5f, 0.1f);
        
        a_pGraphics->Clear(clearColor);
        this->m_pRenderTarget->ApplyGeometryPass(PuRe_Color(0.0f,0.0f,0.0f,0.0f));
        this->m_pSkyBox->Draw(this->m_pCamera, this-> m_pSkyBoxMaterial, PuRe_Vector3F(0.0f, 0.0f, 0.0f));
        //this->m_pSkyBox->Draw(this->m_pCamera, PuRe_MatrixF::Identity());
        this->m_pBrickWorker->Render(a_pGraphics, this->m_pCamera);
        this->m_pBrickSupervisor->Render(a_pGraphics, this->m_pCamera);
        this->m_pRenderTarget->ApplyLightPass(PuRe_Color(0.0f, 0.0f, 0.0f, 0.0f));
        
        a_pGraphics->Begin();
        this->m_pPostMaterial->Apply();
        this->m_pPostMaterial->SetFloat((float)textureID, "textureID");
        this->m_pPostMaterial->SetVector3(PuRe_Vector3F(0.01f, 0.01f, 0.01f), "ambient");
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
        this->m_pRenderTarget->Draw(this->m_pPostCamera, this->m_pPostMaterial, PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f), PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f));

        a_pGraphics->End();
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
        SAFE_DELETE(this->m_pPostCamera);
        SAFE_DELETE(this->m_pCamera);
    }
}