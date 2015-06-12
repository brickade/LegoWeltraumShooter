#include "include/EditorScene.h"

#include <random>

namespace Editor
{
    CEditorScene::CEditorScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_PlayerIdx = a_playerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        
        //Skybox
        this->m_pSkyBox = new PuRe_SkyBox(a_pApplication->GetGraphics(), "../data/textures/cube/");
        this->m_pSkyBoxMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/skybox/default");

        //PostMaterial
        this->m_pPostMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/Post/default");

        //Lights
        this->m_pPointLight = new PuRe_PointLight(a_pApplication->GetGraphics());
        this->m_pPointLightMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/PointLight/default");
        
        this->m_pDirectionalLight = new PuRe_DirectionalLight(a_pApplication->GetGraphics());
        this->m_pDirectionalLightMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/DirectionalLight/default");

        //UICamera
        this->m_UICamera = new PuRe_Camera(PuRe_Vector2F(a_pApplication->GetGraphics()->GetDescription().ResolutionWidth, a_pApplication->GetGraphics()->GetDescription().ResolutionHeight), PuRe_CameraProjection::Orthogonal);
        this->m_UICamera->setNearFar(PuRe_Vector2F(0.1f, 1000));


        this->m_pBrickSupervisor = new Editor::CBrickSupervisor(this->m_PlayerIdx);
        this->m_pBrickSupervisor->Initialize(*a_pApplication->GetGraphics());

        this->m_pWorker = new Editor::CWorker(this->m_PlayerIdx);
        this->m_pWorker->Initialize(*a_pApplication->GetGraphics());

        this->m_pColorFields = new Editor::CColorFields(this->m_PlayerIdx);
        this->m_pColorFields->Initialize(*a_pApplication->GetGraphics());

        this->m_pBrickSupervisorFader = new sba::CUIElementFader(sba_Button::EditorFadeSupervisor, this->m_PlayerIdx);
        this->m_pColorFieldsFader = new sba::CUIElementFader(sba_Button::EditorFadeColors, this->m_PlayerIdx);
    }

    // **************************************************************************
    // **************************************************************************
    bool CEditorScene::Update(PuRe_Application* a_pApplication)
    {
        //Handle ESC Button
        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC)
            || a_pApplication->GetInput()->GamepadPressed(a_pApplication->GetInput()->Pad_Back, this->m_PlayerIdx)
            || a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->F1)
            || a_pApplication->GetInput()->GamepadPressed(a_pApplication->GetInput()->Pad_Start, this->m_PlayerIdx))
        {
            return true;
        }

        if (this->m_pBrickSupervisorFader->Update(*a_pApplication->GetTimer()))
        {
            this->m_pColorFieldsFader->Hide();
        }
        if (this->m_pColorFieldsFader->Update(*a_pApplication->GetTimer()))
        {
            this->m_pBrickSupervisorFader->Hide();
        }
        if (!sba_Input->ButtonIsPressed(sba_Button::EditorFadeHold, this->m_PlayerIdx) && !sba_Input->ButtonIsPressed(sba_Button::EditorUndoRedoHold, this->m_PlayerIdx))
        {
            if (this->m_pBrickSupervisorFader->IsVisible())
            {
                this->m_pBrickSupervisor->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer());
            }
            if (this->m_pColorFieldsFader->IsVisible())
            {
                this->m_pColorFields->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer());
            }
        }
        this->m_pWorker->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer(), this->m_pBrickSupervisor->GetSelectedBrick(), this->m_pColorFields->GetCurrentColor());
        sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances
        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Render(PuRe_Application* a_pApplication)
    {
        sba_Renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));
        //Lights
        sba_Renderer->Draw(0, true, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        sba_Renderer->Draw(1, false, this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 1.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        
        //Skybox
        sba_Renderer->Draw(0, true, this->m_pSkyBox, this->m_pSkyBoxMaterial);
        //Bricks
        this->m_pWorker->Render();
        sba_BrickManager->Render(*sba_Renderer);
        this->m_pBrickSupervisor->Render(*a_pApplication->GetGraphics(), this->m_pBrickSupervisorFader->GetVisibility());
        this->m_pColorFields->Render(*a_pApplication->GetGraphics(), this->m_pColorFieldsFader->GetVisibility());
        //this->m_pWorker->DrawDebug(a_pApplication->GetGraphics());

        //Post
        sba_Renderer->Set(0, PuRe_Vector3F(0.2f, 0.2f, 0.2f), "ambient");
        sba_Renderer->Set(1, PuRe_Vector3F(0.2f, 0.2f, 0.2f), "ambient");
        sba_Renderer->Render(0, this->m_pWorker->GetCamera(), this->m_pPostMaterial);
        sba_Renderer->Render(1, this->m_UICamera, this->m_pPostMaterial);
        sba_Renderer->End();
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