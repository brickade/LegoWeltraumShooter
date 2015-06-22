#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#define Skybox

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

    //#define Skybox

    enum { BufferSize = 32 };
    struct PlayOutBuffer
    {
        int Frame;
        sba::SInputData Inputs[sba::MaxPlayers]; //frame saved in here
    };
    struct GotBuffer
    {
        bool Player[sba::MaxPlayers];
    };
    /// @brief MainScene where the game functions are in, inherits from the Scene interface
    ///
    class CGameScene : public PuRe_IScene
    {
    private:
        int m_Test;
        float m_Timeout;
        bool m_Run;
        std::mutex m_Mutex;
        //host only
        unsigned int m_numReceived[BufferSize];
        GotBuffer m_numGot[BufferSize];
        bool m_send[BufferSize];
        //All players
        PlayOutBuffer m_buffer[BufferSize];
        sba::SInputPacket m_inputs[sba::Delay];
        //used for networking
        float m_PhysicTime;
        int m_ID;
        int m_LocalPlayers;
        int m_PhysicFrame;
        //multiple players
        PuRe_Camera* m_pUICam;
        //whether we are still in menu or not
        bool gameStart;
        int m_TextureID;

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
        PuRe_IMaterial* m_pParticleMaterial;
        CMinimap* m_pMinimap;
        PuRe_Sprite* m_pParticleSprite;

        std::vector<PuRe_ParticleEmitter*> m_Emitters;
        std::vector<CGameCamera*> m_Cameras;
        std::vector<TheBrick::CBullet*> m_Bullets;
        std::vector<TheBrick::CAsteroid*> m_Asteroids;
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
        sba::SInputData HandleInput(int a_PlayerIdx);
        /// @brief Process Input Data
        ///
        void ProcessInput(TheBrick::CSpaceship* a_Ship, sba::SInputData* a_Input, float a_DeltaTime);
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