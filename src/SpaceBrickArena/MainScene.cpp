#include "include/MainScene.h"
namespace sba
{   
    CMainScene::CMainScene(PuRe_Application* a_pApplication)
    {
        this->m_PlayerIdx = 0;
        this->m_pApplication = a_pApplication;
        this->m_pMenuScene = new Menu::CMenuScene(a_pApplication,&this->m_PlayerIdx);
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();

        Space::Instance()->Initialize(*a_pApplication->GetGraphics(), *a_pApplication->GetInput(), *a_pApplication->GetSoundPlayer(), *a_pApplication);
        sba_BrickManager->Load();
        sba_ShipManager->Load();

        this->m_pMenuScene->Initialize(a_pApplication);
        this->m_pActiveScene = this->m_pMenuScene;
    }

    // **************************************************************************
    // **************************************************************************
    int CMainScene::Update(PuRe_Application* a_pApplication)
    {
        //Handle ESC Button
            //Drop update after lag to avoid strange camera movement jumps etc
        if (a_pApplication->GetTimer()->GetElapsedMilliseconds() > 200)
            return 1;

        //Update Active Scene
        int update = this->m_pActiveScene->Update(a_pApplication);
        switch (update)
        {
        case 1:
            return 1;
            break;
        case 2: //Game Local Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);

            this->m_pGameScene = new Game::CGameScene(a_pApplication, this->m_PlayerIdx, false);
            this->m_pGameScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pGameScene;
            return 1;
            break;
        case 3: //Game Network Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);

            this->m_pGameScene = new Game::CGameScene(a_pApplication, this->m_PlayerIdx, true);
            this->m_pGameScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pGameScene;
            return 1;
            break;
        case 4: //Editor Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);

            this->m_pEditorScene = new Editor::CEditorScene(a_pApplication, this->m_PlayerIdx);
            this->m_pEditorScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pEditorScene;
            return 1;
            break;
        case 5: //Menu Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);

            this->m_pMenuScene = new Menu::CMenuScene(a_pApplication, &this->m_PlayerIdx);
            this->m_pMenuScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pMenuScene;
            return 1;
            break;
        default:
            return 0;
            break;
        }
        return 0;
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
        this->m_pActiveScene->Exit();
        SAFE_DELETE(this->m_pActiveScene);
    }
}