#include "include/EditorScene.h"

#include <random>

namespace Editor
{
    CEditorScene::CEditorScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_PlayerIdx = a_playerIdx;
        this->m_State = EEditorState::SelectShip;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        
        this->InitCommonRenderComponents(&this->m_pPostMaterial, &this->m_pDirectionalLight, &this->m_pDirectionalLightMaterial, &this->m_pPointLight, &this->m_pPointLightMaterial, &this->m_pSkyBox, &this->m_pSkyBoxMaterial, &this->m_UICamera);
        this->m_pUIMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/UI/default");

        this->m_pBrickSupervisor = new Editor::CBrickSupervisor(this->m_PlayerIdx);
        this->m_pBrickSupervisor->Initialize(*a_pApplication->GetGraphics());

        this->m_pWorker = new Editor::CWorker(this->m_PlayerIdx);
        this->m_pWorker->Initialize(*a_pApplication->GetGraphics());

        this->m_pShipHandler = new Editor::CShipHandler(this->m_PlayerIdx);

        this->m_pColorFields = new Editor::CColorFields(this->m_PlayerIdx);
        this->m_pColorFields->Initialize(*a_pApplication->GetGraphics());

        this->m_pBrickSupervisorFader = new sba::CUIElementFader(sba_Button::EditorFadeSupervisor, this->m_PlayerIdx);
        this->m_pColorFieldsFader = new sba::CUIElementFader(sba_Button::EditorFadeColors, this->m_PlayerIdx);
    }

    // **************************************************************************
    // **************************************************************************
    int CEditorScene::Update(PuRe_Application* a_pApplication)
    {
        //Handle ESC Button
        if (sba_Input->ButtonPressed(sba_Button::Exit, this->m_PlayerIdx))
        {
            if (this->m_State == EEditorState::EditShip)
            {
                this->m_pWorker->Suspend();
                this->m_State = EEditorState::SelectShip;
            }
            else if (this->m_pShipHandler->GetCurrentState() == CShipHandler::ShipHandlerState::Select)
            {
                return 5;
            }
        }

        switch (this->m_State)
        {
        case EEditorState::SelectShip:
            //------------------------------
            // Select
            //------------------------------
            if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, this->m_PlayerIdx) && this->m_pShipHandler->GetCurrentState() == CShipHandler::ShipHandlerState::Select)
            {
                this->m_State = EEditorState::EditShip;
                break;
            }
            this->m_pShipHandler->Update();
            break;
        case EEditorState::EditShip:
            //------------------------------
            // Edit
            //------------------------------
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

            this->m_pWorker->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer(), this->m_pBrickSupervisor->GetSelectedBrick(), this->m_pColorFields->GetCurrentColor(), *this->m_pShipHandler);
            break;
        }
        sba_BrickManager->RebuildRenderInstances(); //Update RenderInstances
        return 1;
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Render(PuRe_Application* a_pApplication)
    {
        //Pre
        this->PreRender(this->m_pDirectionalLight, this->m_pDirectionalLightMaterial, this->m_pSkyBox, this->m_pSkyBoxMaterial);

        //Bricks
        switch (this->m_State)
        {
        case EEditorState::SelectShip:
            //---------Select---------
            this->m_pShipHandler->Render();
            break;
        case EEditorState::EditShip:
            //----------Edit----------
            this->m_pWorker->Render(*this->m_pShipHandler);
            sba_BrickManager->Render();
            this->m_pBrickSupervisor->Render(*a_pApplication->GetGraphics(), this->m_pBrickSupervisorFader->GetVisibility());
            this->m_pColorFields->Render(*a_pApplication->GetGraphics(), this->m_pColorFieldsFader->GetVisibility());
            break;
        }
        //Post
        this->PostRender(this->m_pWorker->GetCamera(), this->m_UICamera, this->m_pUIMaterial, this->m_pPostMaterial);
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::PreRender(PuRe_DirectionalLight* a_pDirectionalLight, PuRe_IMaterial* a_pDirectionalLightMaterial, PuRe_SkyBox* a_pSkyBox, PuRe_IMaterial* a_pSkyBoxMaterial)
    {
        sba_Renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));
        //Lights
        sba_Renderer->Draw(0, true, a_pDirectionalLight, a_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 0.0f), PuRe_Color(0.3f, 0.3f, 0.3f));
        sba_Renderer->Draw(1, false, a_pDirectionalLight, a_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 1.0f), PuRe_Color(0.3f, 0.3f, 0.3f));

        //Skybox
        if (a_pSkyBox != nullptr)
        {
            sba_Renderer->Draw(0, true, a_pSkyBox, a_pSkyBoxMaterial);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::PostRender(Editor::CCamera* a_pSceneCamera, PuRe_Camera* a_pUICamera, PuRe_IMaterial* a_pUIMaterial, PuRe_IMaterial* a_pPostMaterial)
    {
        //Post
        sba_Renderer->Set(0, PuRe_Vector3F(0.2f, 0.2f, 0.2f), "ambient");
        sba_Renderer->Set(1, PuRe_Vector3F(1, 1, 1), "ambient");
        sba_Renderer->Set(2, PuRe_Vector3F(1, 1, 1), "ambient");
        sba_Renderer->Render(0, 0, a_pSceneCamera, a_pPostMaterial, sba_FinalMaterial); //Scene
        if (a_pUICamera != nullptr)
        {
            sba_Renderer->Render(0, 1, a_pUICamera, a_pPostMaterial, sba_FinalMaterial); //UI    
            sba_Renderer->Render(0, 2, a_pUICamera, a_pUIMaterial, sba_FinalMaterial); //Font
        }
        sba_Renderer->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::InitCommonRenderComponents(PuRe_IMaterial** a_ppPostMaterial, PuRe_DirectionalLight** a_ppDirectionalLight, PuRe_IMaterial** a_ppDirectionalLightMaterial, PuRe_PointLight** a_ppPointLight, PuRe_IMaterial** a_ppPointLightMaterial, PuRe_SkyBox** a_ppSkyBox, PuRe_IMaterial** a_ppSkyBoxMaterial, PuRe_Camera** a_ppUICamera)
    {
        //Skybox
        if (a_ppSkyBox != nullptr)
        {
            *a_ppSkyBox = new PuRe_SkyBox(sba_Application->GetGraphics(), "../data/textures/cube/");
            *a_ppSkyBoxMaterial = sba_Application->GetGraphics()->LoadMaterial("../data/effects/skybox/default");
        }

        //PostMaterial
        if (a_ppPostMaterial != nullptr)
        {
            *a_ppPostMaterial = sba_Application->GetGraphics()->LoadMaterial("../data/effects/Post/default");
        }

        //Lights
        if (a_ppPointLight != nullptr)
        {
            *a_ppPointLight = new PuRe_PointLight(sba_Application->GetGraphics());
            *a_ppPointLightMaterial = sba_Application->GetGraphics()->LoadMaterial("../data/effects/PointLight/default");
        }

        if (a_ppDirectionalLight != nullptr)
        {
            *a_ppDirectionalLight = new PuRe_DirectionalLight(sba_Application->GetGraphics());
            *a_ppDirectionalLightMaterial = sba_Application->GetGraphics()->LoadMaterial("../data/effects/DirectionalLight/default");
        }

        //UICamera
        if (a_ppUICamera != nullptr)
        {
            *a_ppUICamera = new PuRe_Camera(PuRe_Vector2F(1920, 1080), PuRe_CameraProjection::Orthogonal);
            (*a_ppUICamera)->setNearFar(PuRe_Vector2F(0.1f, 1000));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::Exit()
    {
        SAFE_DELETE(this->m_pColorFieldsFader);
        SAFE_DELETE(this->m_pBrickSupervisorFader);
        SAFE_DELETE(this->m_pColorFields);
        SAFE_DELETE(this->m_pShipHandler);
        SAFE_DELETE(this->m_pWorker);
        SAFE_DELETE(this->m_pBrickSupervisor);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pUIMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        sba_BrickManager->RebuildRenderInstances();
    }
}