#ifndef _SPACE_H_
#define _SPACE_H_


// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>



#include <Onager/World.h>
#include <TheBrick/Serializer.h>
#include "InputManager.h"
#include "BrickManager.h"
#include "ShipManager.h"
#include "INIReader.h"
#include "Player.h"
#include "NetworkHandler.h"
#include "SpriteReader.h"
#include "GameMap.h"
#include "DestructionManager.h"

namespace sba
{

    struct SControl
    {
        unsigned char Thrust;
        unsigned char Spin;
        unsigned char Move;
    };

    class Space
    {
    public:
        static Space* Instance()
        {
            if (Space::g_pInstance == nullptr)
            {
                Space::g_pInstance = new Space();
            }
            return Space::g_pInstance;
        }
        PuRe_Application* Application;
        ong::World* World;
        PuRe_Renderer* Renderer;
        sba::CInputManager* InputManager;
        sba::CBrickManager* BrickManager;
        sba::CShipManager* ShipManager;
		sba::CDestructionManager* DestructionManager;
        PuRe_Font* Font;
        PuRe_IMaterial* m_pFinalMaterial;
        PuRe_IMaterial* SpriteMaterial;
        PuRe_IMaterial* FontMaterial;
        PuRe_IMaterial* m_SSAOMaterial;
        PuRe_IMaterial* m_pSkyBoxMaterial;
        PuRe_Sprite* m_pNoiseTexture;
        PuRe_SoundPlayer* m_pSoundPlayer;
        CSpriteReader* m_pButtonSprite;
        std::vector<SPlayer*> m_Players;
        CNetworkHandler* m_pNetworkhandler;
        CGameMap* m_pMap;
        CIniReader* m_pIniReader;
        PuRe_SkyBox* m_pSkybox;
        std::string m_pSkyBoxName;
        SControl m_Controls[4];
        TheBrick::CCSVParser* m_pBalancing;
        int m_Delay;

    private:
        static Space* g_pInstance;

        double m_LastPhysicsUpdate;
        float m_PhysicsFramerate = 60.0f;

		std::vector<TheBrick::CGameObject*> m_MiscObjects;
    
    public:
        void Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer, PuRe_Application& a_rpApplication);
        void UpdatePhysics(PuRe_Timer* a_pTimer);
        void LoadCSV();

        void RenderFont(std::string a_Text, PuRe_Vector2F a_Position, float a_Size = 24, float a_Width = 1, unsigned int a_RendertargetIndex = 2);

        void CreatePlayer(int a_Pad, PuRe_IWindow* a_pWindow);
        void DeletePlayer(unsigned int a_Index);

		// add global non updated objects (used for destruction)
        void AddMiscObject(TheBrick::CGameObject* a_pObject);
        void DelMiscOutside(ong::vec3& a_rOrigin, float& a_rDistance);
		void ClearMiscObjects();

        void ChangeRenderResolution(int a_Width = std::stoi(Instance()->m_pIniReader->GetValue("ResolutionWidth")), int a_Height = std::stoi(Instance()->m_pIniReader->GetValue("ResolutionHeight")), bool a_SSAO = (Instance()->m_pIniReader->GetValue("SSAO") == "On"), bool a_DeleteTargets = true);

    private:
        Space();
        ~Space();
    };
}
#define sba_Space sba::Space::Instance()
#define sba_Application sba::Space::Instance()->Application
#define sba_Width 1920
#define sba_Height 1080

#define sba_Delay sba::Space::Instance()->m_Delay
#define sba_Balancing sba::Space::Instance()->m_pBalancing
#define sba_SoundPlayer sba::Space::Instance()->m_pSoundPlayer
#define sba_Players sba::Space::Instance()->m_Players
#define sba_FinalMaterial sba::Space::Instance()->m_pFinalMaterial
#define sba_SkyBox sba::Space::Instance()->m_pSkybox
#define sba_SkyBoxName sba::Space::Instance()->m_pSkyBoxName
#define sba_SkyBoxMaterial sba::Space::Instance()->m_pSkyBoxMaterial
#define sba_Network sba::Space::Instance()->m_pNetworkhandler
#define sba_Options sba::Space::Instance()->m_pIniReader
#define sba_Buttons sba::Space::Instance()->m_pButtonSprite
#define sba_ButtonsDraw(btn, pos, size) sba::Space::Instance()->m_pButtonSprite->Draw(2, sba::Space::Instance()->Renderer, btn, sba::Space::Instance()->FontMaterial, PuRe_Vector3F(pos, 0), PuRe_Vector3F(), -1, PuRe_Vector2F(size, size))
#define sba_Map sba::Space::Instance()->m_pMap
#define sba_Controls sba::Space::Instance()->m_Controls

#define sba_World sba::Space::Instance()->World
#define sba_Renderer sba::Space::Instance()->Renderer
#define sba_Input sba::Space::Instance()->InputManager
#define sba_BrickManager sba::Space::Instance()->BrickManager
#define sba_ShipManager sba::Space::Instance()->ShipManager

#endif /* _SPACE_H_ */