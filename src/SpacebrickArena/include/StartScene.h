#ifndef _STARTSCENE_H_
#define _STARTSCENE_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>


// Declare namespace Game
namespace sba
{
    /// @brief StartScene which displays the Logos
    ///
    class CStartScene : public PuRe_IScene
    {
    private:
        bool m_Loading;
        /// @brief Timer when to show the next Logo or when to go to the next Screen
        ///
        float m_Timer;
        /// @brief The GA's Logo
        ///
        PuRe_Sprite* m_pGALogo;
        /// @brief The Team's Logo
        ///
        PuRe_Sprite* m_pTeamLogo;
        /// @brief Sprite Material
        ///
        PuRe_IMaterial* m_pMaterial;
        /// @brief PlayerIndex of the Player who first pressed sth
        ///
        int* m_pPlayerIdx;
        /// @brief UI's Camera
        ///
        PuRe_Camera* m_pUICamera;
    public:
        /// @brief Constructor to Initialize the MainScene
        ///
        /// @param Engine's Data
        ///
        CStartScene(PuRe_Application* a_pApplication, int* a_pPlayerIdx);
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

#endif /* _STARTSCENE_H_ */