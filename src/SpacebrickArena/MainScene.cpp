#include "include/MainScene.h"
#include "include/BrickManager.h"
#include "include/ShipManager.h"

namespace sba
{   
    CMainScene::CMainScene(PuRe_Application* a_pApplication)
    {
        this->m_PlayerIdx = 0;
        this->m_pApplication = a_pApplication;
        this->m_pStartScene = new sba::CStartScene(a_pApplication, &this->m_PlayerIdx);
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Initialize(PuRe_Application* a_pApplication)
    {
        PuRe_GraphicsDescription gdesc = a_pApplication->GetGraphics()->GetDescription();
        this->m_pStartScene->Initialize(a_pApplication);
        this->m_pActiveScene = this->m_pStartScene;
    }

    // **************************************************************************
    // **************************************************************************
    int CMainScene::Update(PuRe_Application* a_pApplication)
    {
        //Drop update after lag to avoid strange camera movement jumps etc
        if (a_pApplication->GetTimer()->GetElapsedMilliseconds() > 200 && this->m_pActiveScene != this->m_pGameScene)
            return 1;
        //Update Active Scene
        int update = this->m_pActiveScene->Update(a_pApplication);
        switch (update)
        {
        case 1: //Menu Scene
            break;
        case 2: //Game Local Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);
            sba_SoundPlayer->StopAll();
            sba_SoundPlayer->PlaySound("Ingame", true,true, std::stof(sba_Options->GetValue("MusicVolume")));

            this->m_pGameScene = new sba::CGameScene(a_pApplication, this->m_PlayerIdx, false);
            this->m_pGameScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pGameScene;
            break;
        case 3: //Game Network Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);
            sba_SoundPlayer->StopAll();
            sba_SoundPlayer->PlaySound("Ingame", true, true, std::stof(sba_Options->GetValue("MusicVolume")));

            this->m_pGameScene = new sba::CGameScene(a_pApplication, this->m_PlayerIdx, true);
            this->m_pGameScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pGameScene;
            break;
        case 4: //Editor Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);
            sba_SoundPlayer->StopAll();
            sba_SoundPlayer->PlaySound("Editor", true,true, std::stof(sba_Options->GetValue("MusicVolume")));

            this->m_pEditorScene = new Editor::CEditorScene(a_pApplication, this->m_PlayerIdx);
            this->m_pEditorScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pEditorScene;
            break;
        case 5: //Menu Initialize
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);
            sba_SoundPlayer->StopAll();
            sba_SoundPlayer->PlaySound("Theme", true, true, std::stof(sba_Options->GetValue("MusicVolume")));

            this->m_pMenuScene = new Menu::CMenuScene(a_pApplication, &this->m_PlayerIdx);
            this->m_pMenuScene->Initialize(a_pApplication);
            this->m_pActiveScene = this->m_pMenuScene;
            break;
        case 6: //All Initialize only
            Space::Instance()->Initialize(*a_pApplication->GetGraphics(), *a_pApplication->GetInput(), *a_pApplication->GetSoundPlayer(), *a_pApplication);
            sba_BrickManager->Load();
            sba_ShipManager->Load();
            sba_SoundPlayer->SetListeners(1);
            sba_SoundPlayer->SetListenPosition(0, PuRe_Vector3F(), PuRe_Vector3F(), PuRe_Vector3F(0.0f, 0.0f, 1.0f), PuRe_Vector3F(0.0f, 1.0f, 0.0f));
            sba_SoundPlayer->PlaySound("Theme", true, true, std::stof(sba_Options->GetValue("MusicVolume")));
            //Menu Initialize
            this->m_pMenuScene = new Menu::CMenuScene(a_pApplication, &this->m_PlayerIdx);
            this->m_pMenuScene->Initialize(a_pApplication);
            break;
        case 7: //Delete StartScene
            this->m_pActiveScene->Exit();
            SAFE_DELETE(this->m_pActiveScene);
            this->m_pActiveScene = this->m_pMenuScene;
            break;
        default:
            return 0;
            break;
        }
        sba_SoundPlayer->Update();
        return 1;
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