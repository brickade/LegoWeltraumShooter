#pragma once

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "BrickBozz.h"

#include "EditorScene.h"
#include "GameScene.h"

// Declare namespace Game
namespace Game
{
    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CMainScene : public PuRe_IScene
    {
    private:
        CEditorScene* m_pEditorScene;
        CGameScene* m_pGameScene;
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
        void Initialize(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer);

        /// @brief Updates the scene.
        ///
        /// @param The graphic interface.
        /// @param The window interface.
        /// @param The input interface.
        /// @param The Time Object
        ///
        /// @returns if it is still running or not
        ///
        bool Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer);

        /// @brief Renders the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Render(PuRe_IGraphics* a_pGraphics);

        /// @brief Exists the scene.
        ///
        void Exit();
    };

}
