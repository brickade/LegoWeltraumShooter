#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "BrickBozz.h"

#include <math.h>
#include <algorithm>

#include "GameCamera.h"
#include "Minimap.h"
#include "NetworkHandler.h"

// Declare namespace Game
namespace Game
{
    struct Player
    {
        int ID;
        SOCKADDR_IN NetworkInformation;
        TheBrick::CSpaceship* Ship;
    };
    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CGameScene : public PuRe_IScene
    {
    private:

        int m_ID;
        int m_ArrayID;

        std::vector<Player*> m_Players;
        std::vector<CGameCamera*> m_Cameras;

        float physicsTimer;
        int m_playerIdx;
        int textureID;

        bool gameStart;

        CNetworkHandler* m_pNetwork;
        PuRe_Font* m_pFont;
        PuRe_PointLight* m_pPointLight;
        PuRe_DirectionalLight* m_pDirectionalLight;
        PuRe_SkyBox* m_pSkyBox;
        PuRe_Model* m_pModel;
        PuRe_IMaterial* m_pMaterial;
        PuRe_IMaterial* m_pFontMaterial;
        PuRe_IMaterial* m_pUIMaterial;
        PuRe_IMaterial* m_pSkyMaterial;
        PuRe_IMaterial* m_pPostMaterial;
        PuRe_IMaterial* m_pPointLightMaterial;
        PuRe_IMaterial* m_pDirectionalLightMaterial;
        CMinimap* m_pMinimap;
        PuRe_BoundingBox m_MapBoundaries;

        std::vector<TheBrick::CBullet*> m_Bullets;
        std::vector<TheBrick::CAsteroid*> m_Asteroids;
        /// @brief Engine's Application
        ///
        PuRe_Application* m_pApplication;
    public:
        /// @brief Constructor to Initialize the MainScene
        ///
        CGameScene(PuRe_Application* a_pApplication, int a_playerIdx);
    public:
        /// @brief Start the Game
        ///
        void StartGame();
        /// @brief Receives Data which were send via Network.
        ///
        void ReceiveData();
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

#endif /* _GAMESCENE_H_ */