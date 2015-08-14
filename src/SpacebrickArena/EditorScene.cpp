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
        
        this->InitCommonRenderComponents(&this->m_pPostMaterial, &this->m_pDirectionalLight, &this->m_pDirectionalLightMaterial, &this->m_pPointLight, &this->m_pPointLightMaterial, &this->m_UICamera);
        this->m_pUIMaterial = a_pApplication->GetGraphics()->LoadMaterial("../data/effects/UI/default");

        this->m_pSpriteReader = new sba::CSpriteReader(a_pApplication->GetGraphics(), "../data/textures/ui/editor.png", "../data/textures/ui/editor.txt");

        this->m_pBrickSupervisor = new Editor::CBrickSupervisor(this->m_PlayerIdx);
        this->m_pBrickSupervisor->Initialize(*a_pApplication->GetGraphics());

        this->m_pWorker = new Editor::CWorker(this->m_PlayerIdx);
        this->m_pWorker->Initialize(*a_pApplication->GetGraphics());

        this->m_pShipHandler = new Editor::CShipHandler(this->m_PlayerIdx);

        this->m_pColorFields = new Editor::CColorFields(this->m_PlayerIdx);
        this->m_pColorFields->Initialize(*a_pApplication->GetGraphics());

        this->m_pModeMenu = new Editor::CModeMenu(this->m_PlayerIdx);
        this->m_pModeMenu->Initialize(*a_pApplication->GetGraphics());

        this->m_pBrickSupervisorFader = new sba::CUIElementFader(sba_Button::EditorFadeSupervisor, this->m_PlayerIdx);
        this->m_pColorFieldsFader = new sba::CUIElementFader(sba_Button::EditorFadeColors, this->m_PlayerIdx);
        this->m_pModeMenuFader = new sba::CUIElementFader(sba_Button::EditorFadeMenu, this->m_PlayerIdx);
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
            if (sba_Input->ButtonPressed(sba_Button::NavigationSelect, this->m_PlayerIdx) && this->m_pShipHandler->GetCurrentState() == CShipHandler::ShipHandlerState::Select && this->m_pShipHandler->GetCurrentSpaceShip() != nullptr)
            {
                this->m_pWorker->Resume(*this->m_pShipHandler);
                this->m_State = EEditorState::EditShip;
                break;
            }
            this->m_pShipHandler->Update();
            break;
        case EEditorState::EditShip:
            //------------------------------
            // Edit
            //------------------------------
            if (sba_Input->ButtonIsPressed(sba_Button::EditorFadeHold, this->m_PlayerIdx) && sba_Input->ButtonPressed(sba_Button::EditorFadeDummy, this->m_PlayerIdx))
            {
                this->m_pBrickSupervisorFader->Hide();
                this->m_pColorFieldsFader->Hide();
                this->m_pModeMenuFader->Hide();
            }
            if (this->m_pBrickSupervisorFader->Update(*a_pApplication->GetTimer()))
            {
                this->m_pColorFieldsFader->Hide();
                this->m_pModeMenuFader->Hide();
            }
            if (this->m_pColorFieldsFader->Update(*a_pApplication->GetTimer()))
            {
                this->m_pBrickSupervisorFader->Hide();
                this->m_pModeMenuFader->Hide();
            }
            if (this->m_pModeMenuFader->Update(*a_pApplication->GetTimer()))
            {
                this->m_pBrickSupervisorFader->Hide();
                this->m_pColorFieldsFader->Hide();
            }
            if (!sba_Input->ButtonIsPressed(sba_Button::EditorFadeHold, this->m_PlayerIdx) && !sba_Input->ButtonIsPressed(sba_Button::EditorUndoRedoHold, this->m_PlayerIdx))
            {
                if (this->m_pBrickSupervisorFader->GetVisibility() > 1.0f - FLT_EPSILON)
                {
                    this->m_pBrickSupervisor->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer());
                }
                if (this->m_pColorFieldsFader->GetVisibility() > 1.0f - FLT_EPSILON)
                {
                    this->m_pColorFields->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer());
                }
                if (this->m_pModeMenuFader->GetVisibility() > 1.0f - FLT_EPSILON)
                {
                    this->m_pModeMenu->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer());
                }
            }

            this->m_pWorker->Update(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *a_pApplication->GetTimer(), *a_pApplication->GetSoundPlayer(), this->m_pBrickSupervisor->GetSelectedBrick(), this->m_pColorFields->GetCurrentColor(), *this->m_pShipHandler, (this->m_pModeMenu->SelectedMode() == 1));
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
        this->PreRender(this->m_pDirectionalLight, this->m_pDirectionalLightMaterial,true);

        //Bricks
        switch (this->m_State)
        {
        case EEditorState::SelectShip:
            //---------Select---------
            this->m_pShipHandler->Render(*this->m_pSpriteReader);
            break;
        case EEditorState::EditShip:
            //----------Edit----------
            sba_BrickManager->Render();
            this->m_pWorker->Render(*this->m_pShipHandler, *this->m_pSpriteReader);
            this->m_pBrickSupervisor->Render(*a_pApplication->GetGraphics(), *this->m_pSpriteReader, this->m_pBrickSupervisorFader->GetVisibility(), this->m_pColorFields->GetCurrentColor());
            this->m_pColorFields->Render(*a_pApplication->GetGraphics(), *this->m_pSpriteReader, this->m_pColorFieldsFader->GetVisibility());
            this->m_pModeMenu->Render(*a_pApplication->GetGraphics(), *this->m_pSpriteReader, this->m_pModeMenuFader->GetVisibility());
            break;
        }
        //Post
        this->PostRender(this->m_pWorker->GetCamera(), this->m_UICamera, this->m_pUIMaterial, this->m_pPostMaterial);
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::PreRender(PuRe_DirectionalLight* a_pDirectionalLight, PuRe_IMaterial* a_pDirectionalLightMaterial, bool a_Skybox)
    {
        sba_Renderer->Begin(PuRe_Color(0.1f, 0.5f, 0.1f));
        //Lights
        sba_Renderer->Draw(0, true, a_pDirectionalLight, a_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, -0.5f, 0.0f), PuRe_Color(1.0f, 1.0f, 1.0f));
        sba_Renderer->Draw(2, false, a_pDirectionalLight, a_pDirectionalLightMaterial, PuRe_Vector3F(1.0f, 0.0f, 1.0f), PuRe_Color(0.3f, 0.3f, 0.3f));

        //Skybox
        if (a_Skybox)
        {
            sba_Renderer->Draw(0, true, sba_SkyBox, sba_SkyBoxMaterial);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::PostRender(Editor::CCamera* a_pSceneCamera, PuRe_Camera* a_pUICamera, PuRe_IMaterial* a_pUIMaterial, PuRe_IMaterial* a_pPostMaterial)
    {
        //Post
        sba_Renderer->Set(0, PuRe_Vector3F(0.6f, 0.6f, 0.6f), "ambient");
        sba_Renderer->Set(1, PuRe_Vector3F(1, 1, 1), "ambient");
        sba_Renderer->Set(2, PuRe_Vector3F(1, 1, 1), "ambient");
        sba_Renderer->Render(0, 0, a_pSceneCamera, a_pPostMaterial, sba_FinalMaterial); //Scene
        if (a_pUICamera != nullptr)
        {
            sba_Renderer->Render(0, 1, a_pUICamera, a_pUIMaterial, sba_FinalMaterial); //UI   
            sba_Renderer->Render(0, 2, a_pUICamera, a_pUIMaterial, sba_FinalMaterial); //Font & Bricks & Colors
        }
        sba_Renderer->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CEditorScene::InitCommonRenderComponents(PuRe_IMaterial** a_ppPostMaterial, PuRe_DirectionalLight** a_ppDirectionalLight, PuRe_IMaterial** a_ppDirectionalLightMaterial, PuRe_PointLight** a_ppPointLight, PuRe_IMaterial** a_ppPointLightMaterial, PuRe_Camera** a_ppUICamera)
    {

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
        SAFE_DELETE(this->m_pModeMenuFader);
        SAFE_DELETE(this->m_pColorFieldsFader);
        SAFE_DELETE(this->m_pBrickSupervisorFader);
        SAFE_DELETE(this->m_pModeMenu);
        SAFE_DELETE(this->m_pColorFields);
        SAFE_DELETE(this->m_pShipHandler);
        SAFE_DELETE(this->m_pWorker);
        SAFE_DELETE(this->m_pBrickSupervisor);
        SAFE_DELETE(this->m_pSpriteReader);
        SAFE_DELETE(this->m_pUIMaterial);
        SAFE_DELETE(this->m_UICamera);
        SAFE_DELETE(this->m_pDirectionalLightMaterial);
        SAFE_DELETE(this->m_pDirectionalLight);
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pPointLight);
        SAFE_DELETE(this->m_pPostMaterial);
        sba_BrickManager->RebuildRenderInstances();
    }
}