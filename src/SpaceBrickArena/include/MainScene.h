#pragma once

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"

#include "EditorScene.h"
#include "GameScene.h"
#include "MenuScene.h"

// Declare namespace Game
namespace sba
{
    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CMainScene : public PuRe_IScene
    {
    private:
        int m_PlayerIdx;
        Editor::CEditorScene* m_pEditorScene;
        Menu::CMenuScene* m_pMenuScene;
        Game::CGameScene* m_pGameScene;
        PuRe_IScene* m_pActiveScene;
        /// @brief Engine's Application
        ///
        PuRe_Application* m_pApplication;

    public:
        /// @brief Constructor to Initialize the MainScene
        ///
        CMainScene(PuRe_Application* a_pApplication);
    public:
        /// @brief Initializes the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Initialize(PuRe_Application* a_pApplication);

        /// @brief Updates the scene.
        ///
        /// @param The graphic interface.
        /// @param The window interface.
        /// @param The input interface.
        /// @param The Time Object
        ///
        /// @returns if it is still running or not
        ///
        int Update(PuRe_Application* a_pApplication);

        /// @brief Renders the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Render(PuRe_Application* a_pApplication);

        /// @brief Exists the scene.
        ///
        void Exit();
    };

}
