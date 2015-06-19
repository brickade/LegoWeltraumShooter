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
#include "InputManager.h"
#include "INIReader.h"
#include "ShipManager.h"

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
        ong::World* World;
        PuRe_Renderer* Renderer;
        TheBrick::CBrickManager* BrickManager;
        sba::CInputManager* InputManager;
        sba::CShipManager* ShipManager;
        PuRe_Font* Font;
        PuRe_IMaterial* FontMaterial;

    private:
        static Space* g_pInstance;

        PuRe_IMaterial* m_SSAOMaterial;

        double m_LastPhysicsUpdate;
        float m_PhysicsFramerate = 60.0f;
    
    public:
        void Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer, PuRe_Application& a_rpApplication);
        void UpdatePhysics(PuRe_Timer* a_pTimer);

        void RenderFont(std::string a_Text, PuRe_Vector2F a_Position, float a_Size = 24, float a_Width = 1, unsigned int a_RendertargetIndex = 2);

    private:
        Space();
        ~Space();
    };
}
#define sba_Space sba::Space::Instance()
#define sba_Application sba::Space::Instance()->Application
#define sba_Width sba::Space::Instance()->Application->GetGraphics()->GetDescription().ResolutionWidth
#define sba_Height sba::Space::Instance()->Application->GetGraphics()->GetDescription().ResolutionHeight

#define sba_BrickManager sba::Space::Instance()->BrickManager
#define sba_World sba::Space::Instance()->World
#define sba_Renderer sba::Space::Instance()->Renderer
#define sba_Input sba::Space::Instance()->InputManager
#define sba_ShipManager sba::Space::Instance()->ShipManager
#endif /* _SPACE_H_ */