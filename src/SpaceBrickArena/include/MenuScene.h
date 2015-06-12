#ifndef _MENUSCENE_H_
#define _MENUSCENE_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"
#include "Navigation.h"

#include "Menu_Main.h"
#include "Menu_Options.h"


// Declare namespace Game
namespace Menu
{
    enum Screen
    {
        Main,
        Options
    };
    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CMenuScene : public PuRe_IScene
    {
    private:

        COptions* m_pOptions;

        CMain* m_pMainMenu;
        /// @brief to display different targets
        ///
        int textureID;
        /// @brief PlayerIndex of the Player who first pressed sth
        ///
        int* m_pPlayerIdx;
        /// @brief Which Screen is displaxed
        ///
        Screen m_Displayed;
        /// @brief Engine's Application
        ///
        PuRe_Application* m_pApplication;
        /// @brief Font Object
        ///
        PuRe_Font* m_pFont;
        /// @brief Skybox Object
        ///
        PuRe_SkyBox* m_pSkyBox;
        /// @brief Skybox Material
        ///
        PuRe_IMaterial* m_pSkyMaterial;
        /// @brief PostScreen Material
        ///
        PuRe_IMaterial* m_pPostMaterial;
        /// @brief Font Material
        ///
        PuRe_IMaterial* m_pFontMaterial;
        /// @brief Scene's Camera
        ///
        PuRe_Camera* m_pSceneCamera;
        /// @brief UI's Camera
        ///
        PuRe_Camera* m_pUICamera;
    public:
        /// @brief Constructor to Initialize the MainScene
        ///
        /// @param Engine's Data
        ///
        CMenuScene(PuRe_Application* a_pApplication,int* a_pPlayerIdx);
    public:
        /// @brief Initializes the scene.
        ///
        /// @param Engine's Data
        ///
        void Initialize(PuRe_Application* a_pApplicationr);

        /// @brief Updates the scene.
        ///
        /// @param Engine's Data
        ///
        /// @returns if it is still running or not
        ///
        int Update(PuRe_Application* a_pApplication);

        /// @brief Renders the scene.
        ///
        /// @param Engine's Data
        ///
        void Render(PuRe_Application* a_pApplication);

        /// @brief Exists the scene.
        ///
        void Exit();
    };

}

#endif /* _MENUSCENE_H_ */