#ifndef _SPACE_H_
#define _SPACE_H_

//####################################################################################################
//####################################################################################################
                                    /*COMMENT OUT FOR GAME SCENE*/
                                            #define MENU
//####################################################################################################
//####################################################################################################
// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/World.h>

#include <TheBrick/BrickManager.h>
#include <TheBrick/Serializer.h>
#include "InputManager.h"
#include "INIReader.h"
#include "Player.h"

namespace sba
{
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
        TheBrick::CBrickManager* BrickManager;
        ong::World* World;
        PuRe_Renderer* Renderer;
        sba::CInputManager* InputManager;
        PuRe_Font* Font;
        PuRe_IMaterial* FontMaterial;
        PuRe_IMaterial* m_SSAOMaterial;
        PuRe_Sprite* m_pNoiseTexture;
        std::vector<Player*> m_Players;

    private:
        static Space* g_pInstance;

        double m_LastPhysicsUpdate;
        float m_PhysicsFramerate = 60.0f;
    
    public:
        void Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer, PuRe_Application& a_rpApplication);
        void UpdatePhysics(PuRe_Timer* a_pTimer);

        void RenderFont(std::string a_Text, PuRe_Vector2F a_Position, float a_Size = 24, float a_Width = 1, unsigned int a_RendertargetIndex = 2);

        void CreatePlayer(int a_Pad, PuRe_IWindow* a_pWindow);
        void DeletePlayer(int a_Index);

    private:
        Space();
        ~Space();
    };
}
#define sba_Space sba::Space::Instance()
#define sba_Application sba::Space::Instance()->Application
#define sba_Width sba::Space::Instance()->Application->GetGraphics()->GetDescription().ResolutionWidth
#define sba_Height sba::Space::Instance()->Application->GetGraphics()->GetDescription().ResolutionHeight

#define sba_Players sba::Space::Instance()->m_Players

#define sba_BrickManager sba::Space::Instance()->BrickManager
#define sba_World sba::Space::Instance()->World
#define sba_Renderer sba::Space::Instance()->Renderer
#define sba_Input sba::Space::Instance()->InputManager
#endif /* _SPACE_H_ */