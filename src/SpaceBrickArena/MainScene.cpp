#include "include/MainScene.h"
namespace Game
{
    CMainScene::CMainScene(PuRe_Application* a_pApplication)
    {
        this->m_pApplication = a_pApplication;
        this->m_pEditorScene = new CEditorScene(a_pApplication, 0);
        this->m_pGameScene = new CGameScene(a_pApplication, 0);
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Initialize(PuRe_IGraphics* a_pGraphics,PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        BrickBozz::Instance()->Initialize(a_pGraphics, a_pSoundPlayer);
        BrickBozz::Instance()->BrickManager->Load(a_pGraphics, BrickBozz::Instance()->World, BrickBozz::Instance()->BrickManager->GetBrickMaterial(), "../data/bricks/");

        //Scenes
        this->m_pEditorScene->Initialize(a_pGraphics, a_pWindow, a_pSoundPlayer);
        this->m_pGameScene->Initialize(a_pGraphics, a_pWindow, a_pSoundPlayer);

        this->m_pActiveScene = this->m_pGameScene;
        //this->m_pActiveScene = this->m_pEditorScene; //Comment out for Init GameScene
    }

    // **************************************************************************
    // **************************************************************************
    bool CMainScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, 0)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, 1)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, 2)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, 3))
        {
            return true;
        }

        if (a_pTimer->GetElapsedMilliseconds() > 200)
        {
            return false;
        }

        if (this->m_pActiveScene->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer))
        {
            if (this->m_pActiveScene == this->m_pGameScene)
            {
                this->m_pActiveScene = this->m_pEditorScene;
            }
            else
            {
                this->m_pActiveScene = this->m_pGameScene;
            }
        }
        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        this->m_pActiveScene->Render(a_pGraphics);
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Exit()
    {
        this->m_pEditorScene->Exit();
        this->m_pGameScene->Exit();
        SAFE_DELETE(this->m_pEditorScene);
        SAFE_DELETE(this->m_pGameScene);
    }
}