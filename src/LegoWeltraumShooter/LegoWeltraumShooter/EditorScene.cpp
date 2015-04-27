#include "include/EditorScene.h"
namespace Game
{
    CEditorScene::CEditorScene(PuRe_Application* a_pApplication, int32 a_playerIdx, CBrickManager* a_pBrickManager)
    {
        this->m_pApplication = a_pApplication;
        this->m_playerIdx = a_playerIdx;
        this->m_pBrickManager = a_pBrickManager;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Camera
        this->m_pCamera = new CEditorCamera(PuRe_Vector2F((float32)gdesc.ResolutionWidth, (float32)gdesc.ResolutionHeight), PuRe_Camera_Perspective, this->m_playerIdx);
        this->m_pCamera->Initialize();
        this->m_pCamera->Rotate(10, 0, 0);
        this->m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float32)gdesc.ResolutionWidth, (float32)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/default/default");
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyBoxMaterial = a_pGraphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pModel = new PuRe_Model(a_pGraphics, this->m_pMaterial, "../data/models/brick1.obj");
        this->m_pRenderTarget = a_pGraphics->CreateRendertarget(this->m_pPostMaterial);

        std::string* a_pCubePaths = new std::string[6];
        a_pCubePaths[0] = "../data/textures/skybox/posx.png";
        a_pCubePaths[1] = "../data/textures/skybox/negx.png";
        a_pCubePaths[2] = "../data/textures/skybox/posy.png";
        a_pCubePaths[3] = "../data/textures/skybox/negy.png";
        a_pCubePaths[4] = "../data/textures/skybox/posz.png";
        a_pCubePaths[5] = "../data/textures/skybox/negz.png";
        this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, this->m_pSkyBoxMaterial, a_pCubePaths);

        this->textureID = 0;

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

        //Calculate the camera direction (muss in der engine ordentlich umgesetzt werden und dann hier ersetzt werden!)
        PuRe_Vector3F cameraLook = PuRe_Vector3F(0, 0, 1);
        PuRe_Vector3F cameraRotation = this->m_pCamera->GetRotation();
        cameraLook = cameraLook * PuRe_QuaternionF(cameraRotation.X*0.0174532925f, cameraRotation.Y*0.0174532925f, cameraRotation.Z*0.0174532925f).GetMatrix();

        this->m_pBrickWorker->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer, cameraLook);

        for (int32 i = 0; i < 4; i++)
        {
            if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
                printf("A pressed by %i\n", i);
        }


        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Color clear = PuRe_Color(0.1f, 0.1f, 0.1f);
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        this->m_pRenderTarget->Apply(clear);


        this->m_pSkyBox->Draw(this->m_pCamera, PuRe_Vector3F(0.0f, 0.0f, 0.0f));
        /*int32 c = 5;
        for (int x = -c; x < c; x++)
        {
            for (int y = -c; y < c; y++)
            {
                for (int z = -c; z < c; z++)
                {
                    this->m_pModel->Draw(this->m_pCamera, PuRe_Primitive::Triangles, PuRe_Vector3F(x*0.39f, y*0.39f, z*0.39f), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f));
                }
            }
        }*/
        this->m_pBrickWorker->Render(a_pGraphics, this->m_pCamera);

        //this->m_pModel->Draw(this->m_pCamera, PuRe_Primitive::Triangles, PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f));
        a_pGraphics->Begin(clear);
        this->m_pPostMaterial->Apply();
        this->m_pPostMaterial->SetFloat((float32)textureID, "textureID");
        this->m_pPostMaterial->SetVector3(PuRe_Vector3F(0.01f, 0.01f, 0.01f), "ambient");
        this->m_pRenderTarget->Draw(this->m_pPostCamera, PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f), PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f));

        a_pGraphics->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Exit()
    {
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pRenderTarget);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pMaterial);
        SAFE_DELETE(this->m_pPostCamera);
        SAFE_DELETE(this->m_pCamera);
        SAFE_DELETE(this->m_pModel);
        SAFE_DELETE(this->m_pBrickWorker);
    }
}