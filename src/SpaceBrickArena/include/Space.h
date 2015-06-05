#ifndef _SPACE_H_
#define _SPACE_H_

//####################################################################################################
//####################################################################################################
                                    /*COMMENT OUT FOR GAME SCENE*/
                                            #define EDITOR
//####################################################################################################
//####################################################################################################

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/World.h>

#include <TheBrick/BrickManager.h>
#include "InputManager.h"#include "INIReader.h"
namespace sba
{
    class Space
    {
    public:
        static Space* Instance();

        TheBrick::CBrickManager* BrickManager;
        ong::World* World;
        PuRe_Renderer* Renderer;
        sba::CInputManager* InputManager;

    private:
        static Space* g_pInstance;

        PuRe_IMaterial* m_SSAOMaterial;

        double m_LastPhysicsUpdate;
        float m_PhysicsFramerate = 60.0f;
    
    public:
        void Initialize(PuRe_IGraphics& a_pGraphics, PuRe_IInput& a_pInput, PuRe_SoundPlayer& a_pSoundPlayer);
        void UpdatePhysics(PuRe_Timer* a_pTimer);

    private:
        Space();
        ~Space();
    };
}

#endif /* _SPACE_H_ */