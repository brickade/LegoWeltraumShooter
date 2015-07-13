#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#define Skybox

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"
#include <Onager/BVH.h>

#include "GameCamera.h"
#include "NetworkHandler.h"
#include "Asteroid.h"
#include "Item.h"
#include "Game_UI.h"
#include "Game_Network.h"

// Declare namespace Game
namespace sba
{

    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CGameScene : public PuRe_IScene
    {
    private:
        unsigned int m_Test;
        CGameNetwork* m_pNetwork;
        //Determinate who won and save him even when others are still playing
        bool m_Won;
        int m_WonID;
        int m_WonIndex;
        //End time
        float m_TimeLimit;
        float m_EndTime;
        //used for networking
        int m_LocalPlayers;
        //multiple players
        PuRe_Camera* m_pUICam;
        //whether we are still in menu or not
        int m_TextureID;


        PuRe_Vector3F m_Origin = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        float m_OriginDistance = 500.0f;

        //Game UI
        CGUI* m_pUI;

        PuRe_Sphere* m_pSphere;
        PuRe_Font* m_pFont;
        PuRe_PointLight* m_pPointLight;
        PuRe_DirectionalLight* m_pDirectionalLight;
        PuRe_SkyBox* m_pSkyBox;
        PuRe_IMaterial* m_pShieldMaterial;
        PuRe_IMaterial* m_pFontMaterial;
        PuRe_IMaterial* m_pUIMaterial;
        PuRe_IMaterial* m_pSkyMaterial;
        PuRe_IMaterial* m_pPostMaterial;
        PuRe_IMaterial* m_pPointLightMaterial;
        PuRe_IMaterial* m_pDirectionalLightMaterial;
        PuRe_IMaterial* m_pParticleMaterial;
        PuRe_IMaterial* m_pSpriteMaterial;

        PuRe_Sprite* m_pLockSprite;
        PuRe_Sprite* m_pParticle1Sprite;
        PuRe_Sprite* m_pParticle2Sprite;
        PuRe_Sprite* m_pParticle3Sprite;
        std::vector<PuRe_ParticleEmitter*> m_SpaceEmitter;
        std::vector<CGameCamera*> m_Cameras;
        std::vector<sba::CBullet*> m_Bullets;
        std::vector<sba::CAsteroid*> m_Asteroids;
        std::vector<sba::CItem*> m_Items;
        /// @brief Engine's Application
        ///
        PuRe_Application* m_pApplication;
    public:
        /// @brief Constructor to Initialize the MainScene
        ///
        CGameScene(PuRe_Application* a_pApplication, int a_playerIdx, bool a_Network);
    public:
        /// @brief Handle Input Data
        ///
        static sba::SInputData HandleInput(int a_PlayerIdx, PuRe_IInput* a_pInput);
        /// @brief Process Input Data
        ///
        static void ProcessInput(std::vector<CBullet*>& a_rBullets, SPlayer* a_pPlayer, sba::SInputData* a_Input, float a_DeltaTime);
        /// @brief Update a Game
        ///
        static void UpdateGame(std::vector<CBullet*>& a_rBullets, std::vector<CItem*>& a_rItems, float a_Deltatime);
        /// @brief Handle Local Data
        ///
        void HandleLocal();
        /// @brief Handle Network Data
        ///
        void HandleNetwork();
        /// @brief Start the Game
        ///
        void StartGame();
        /// @brief Receives Data which were send via Network.
        ///
        void ReceiveData(SOCKET s);
        /// @brief Initializes the scene.
        ///
        /// @param graphics The graphics interface.
        ///
        void Initialize(PuRe_Application* a_pApplicationr);

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

#endif /* _GAMESCENE_H_ */