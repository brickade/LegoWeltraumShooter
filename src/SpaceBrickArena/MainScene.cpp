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
    void CMainScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();

        Space::Instance()->Initialize(*a_pApplication->GetGraphics(), *a_pApplication->GetInput(), *a_pApplication->GetSoundPlayer());
        Space::Instance()->BrickManager->Load(*a_pApplication->GetGraphics(), *a_pApplication->GetWindow(), *Space::Instance()->World, *Space::Instance()->BrickManager->GetBrickMaterial(), "../data/bricks/");

        //Scenes
#ifdef EDITOR
        this->m_pEditorScene->Initialize(a_pApplication);
        this->m_pActiveScene = this->m_pEditorScene;
#else
        this->m_pGameScene->Initialize(a_pApplication);
        this->m_pActiveScene = this->m_pGameScene;
#endif
    }

    // **************************************************************************
    // **************************************************************************
    bool CMainScene::Update(PuRe_Application* a_pApplication)
    {
        //Handle ESC Button
        if (a_pApplication->GetInput()->KeyPressed(a_pApplication->GetInput()->ESC))
        {
            return true;
        }

        //Drop update after lag to avoid strange camera movement jumps etc
        if (a_pApplication->GetTimer()->GetElapsedMilliseconds() > 200)
        {
            return false;
        }

        //Update Active Scene
        if (!this->m_pActiveScene->Update(a_pApplication))
        {
            return false;
        }
        return true;
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Render(PuRe_Application* a_pApplication)
    {
        this->m_pActiveScene->Render(a_pApplication);
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