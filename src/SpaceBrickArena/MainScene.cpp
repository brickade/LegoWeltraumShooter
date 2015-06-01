#include "include/MainScene.h"
namespace sba
{   
    CMainScene::CMainScene(PuRe_Application* a_pApplication)
    {
        this->m_pApplication = a_pApplication;
#ifdef EDITOR
        this->m_pEditorScene = new Editor::CEditorScene(a_pApplication, 0);
#else
        this->m_pGameScene = new Game::CGameScene(a_pApplication, 0);
#endif
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        Space::Instance()->Initialize(*a_pGraphics, *a_pSoundPlayer);
        Space::Instance()->BrickManager->Load(*a_pGraphics, *a_pWindow, *Space::Instance()->World, *Space::Instance()->BrickManager->GetBrickMaterial(), "../data/bricks/");

        //Scenes
#ifdef EDITOR
        this->m_pEditorScene->Initialize(a_pGraphics, a_pWindow, a_pSoundPlayer);
        this->m_pActiveScene = this->m_pEditorScene;
#else
        this->m_pGameScene->Initialize(a_pGraphics, a_pWindow, a_pSoundPlayer);
        this->m_pActiveScene = this->m_pGameScene;
#endif
    }

    // **************************************************************************
    // **************************************************************************
    bool CMainScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC))
        {
            return true;
        }

        //Drop update after lag to avoid strange camera movement jumps etc
        if (a_pTimer->GetElapsedMilliseconds() > 200)
        {
            return false;
        }

        //Update Active Scene
        if (!this->m_pActiveScene->Update(a_pGraphics, a_pWindow, a_pInput, a_pTimer, a_pSoundPlayer))
        {
            return false;
        }
        return true;
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
#ifdef EDITOR
        this->m_pEditorScene->Exit();
        SAFE_DELETE(this->m_pEditorScene);
#else
        this->m_pGameScene->Exit();
        SAFE_DELETE(this->m_pGameScene);
#endif
    }
}