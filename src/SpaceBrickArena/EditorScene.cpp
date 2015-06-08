#include "include/EditorScene.h"

#include <random>

namespace Editor
{
    CEditorScene::CEditorScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_pApplication = a_pApplication;
        this->m_playerIdx = a_playerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        this->m_pPostMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/Post/default");
        this->m_pSkyBoxMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/skybox/default");

        this->m_pPointLightMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalLightMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pPointLight = new PuRe_PointLight(a_pApplication->GetGraphics());
        this->m_pDirectionalLight = new PuRe_DirectionalLight(a_pApplication->GetGraphics());

        this->m_UICamera = new PuRe_Camera(PuRe_Vector2F(a_pApplication->GetGraphics()->GetDescription().ResolutionWidth, a_pApplication->GetGraphics()->GetDescription().ResolutionHeight), PuRe_CameraProjection::Orthogonal);

        this->m_pSkyBox = new PuRe_SkyBox(a_pApplication->GetGraphics(), "../data/textures/cube/");

        this->textureID = 0;

        this->m_pBrickSupervisor = new Editor::CBrickSupervisor(this->m_playerIdx);
        this->m_pBrickSupervisor->Initialize(*a_pApplication->GetGraphics());

        this->m_pWorker = new Editor::CWorker(this->m_playerIdx);
        this->m_pWorker->Initialize(*a_pApplication->GetGraphics());
        this->m_CurrentColor = PuRe_Color(0.5f, 0.6f, 1.0f);
    }

    // **************************************************************************
    // **************************************************************************
    bool CEditorScene::Update(PuRe_Application* a_pApplication)
    {
        //Handle ESC Button
        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC)
            || a_pApplication->GetInput()->GamepadPressed(a_pApplication->GetInput()->Pad_Back, this->m_playerIdx)
            || a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->F1)
            || a_pApplication->GetInput()->GamepadPressed(a_pApplication->GetInput()->Pad_Start, this->m_playerIdx))
        {
            return true;
        }

        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Comma))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 3;
        }
        else if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->Period))
        {
            this->textureID++;
            if (this->textureID > 3)
                this->textureID = 0;
        }

        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->C))
        {
            this->m_CurrentColor = PuRe_Color((std::rand() / (float)RAND_MAX), (std::rand() / (float)RAND_MAX), (std::rand() / (float)RAND_MAX));
        }

        this->m_pBrickSupervisor->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer());
        this->m_pWorker->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer(), this->m_pBrickSupervisor->GetSelectedBrick(), this->m_CurrentColor);
        sba::Space::Instance()->BrickManager->RebuildRenderInstances(); //Update RenderInstances
        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Render(PuRe_Application* a_pApplication)
    {
        PuRe_Renderer* renderer = sba::Space::Instance()->Renderer;
        renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));
        //Lights
        renderer->Draw(0, true, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        renderer->Draw(1, false, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 1.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        //renderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(0.0f, 5.0f, 0.0f), PuRe_Color(1, 1, 1), 0.1f, 0.01f, 0.01f);
        
        //Skybox
        renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyBoxMaterial);
        //Bricks
        this->m_pWorker->Render();
        sba::Space::Instance()->BrickManager->Render(*sba::Space::Instance()->Renderer);
        this->m_pBrickSupervisor->Render(*a_pApplication->GetGraphics());
        this->m_pWorker->DrawDebug(a_pApplication->GetGraphics());
        //Post
        renderer->Set(0, (float)this->textureID, "textureID");
        renderer->Set(0, PuRe_Vector3F(0.2f, 0.2f, 0.2f), "ambient");
        renderer->Set(1, (float)this->textureID, "textureID");
        renderer->Set(1, PuRe_Vector3F(0.2f, 0.2f, 0.2f), "ambient");
        renderer->Render(0, this->m_pWorker->GetCamera(), this->m_pPostMaterial);
        renderer->Render(1, this->m_UICamera, this->m_pPostMaterial);
        renderer->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Exit()
    {
        SAFE_DELETE(this->m_pWorker);
        SAFE_DELETE(this->m_pBrickSupervisor);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pPostMaterial);
    }
}