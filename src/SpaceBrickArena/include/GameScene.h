#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"

#include <math.h>
#include <algorithm>
#include <mutex>

#include "GameCamera.h"
#include "Minimap.h"
#include "NetworkHandler.h"

// Declare namespace Game
namespace Game
{
    enum { BufferSize = 32 };
    struct PlayOutBuffer
    {
        int Frame;
        InputData Inputs[MaxPlayers]; //frame saved in here
    };
    struct Player
    {
        unsigned char ID;
        SOCKADDR_IN NetworkInformation;
        TheBrick::CSpaceship* Ship;
    };
    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CGameScene : public PuRe_IScene
    {
    private:
        std::mutex m_Mutex;
        FILE* replay;
        //host only
        int m_numReceived[BufferSize];
        int m_send[BufferSize];
        //All players
        PlayOutBuffer m_buffer[BufferSize];
        //used for networking
        float m_PhysicTime;
        int m_ID;
        int m_ArrayID;
        int m_PhysicFrame;
        //multiple players
        std::vector<Player*> m_Players;
        std::vector<CGameCamera*> m_Cameras;
        PuRe_Camera* m_pUICam;
        //whether we are still in menu or not
        bool gameStart;
        int m_TextureID;
        CNetworkHandler* m_pNetwork;

        PuRe_Font* m_pFont;
        PuRe_PointLight* m_pPointLight;
        PuRe_DirectionalLight* m_pDirectionalLight;
        PuRe_SkyBox* m_pSkyBox;
        PuRe_IMaterial* m_pFontMaterial;
        PuRe_IMaterial* m_pUIMaterial;
        PuRe_IMaterial* m_pSkyMaterial;
        PuRe_IMaterial* m_pPostMaterial;
        PuRe_IMaterial* m_pPointLightMaterial;
        PuRe_IMaterial* m_pDirectionalLightMaterial;
        CMinimap* m_pMinimap;

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
        bool Update(PuRe_Application* a_pApplication);

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