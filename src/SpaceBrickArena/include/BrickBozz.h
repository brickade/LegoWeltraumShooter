#ifndef _BRICKBOZZ_H_
#define _BRICKBOZZ_H_

// Framework specific includes
#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/World.h>

#include <TheBrick/BrickManager.h>

namespace Game
{
    class BrickBozz
    {
    public:
        static BrickBozz* Instance();

        TheBrick::CBrickManager* BrickManager;
        ong::World* World;
        PuRe_Renderer* Renderer;

    private:
        static BrickBozz* g_pInstance;
    
    public:
        void Initialize(PuRe_IGraphics& a_pGraphics, PuRe_SoundPlayer& a_pSoundPlayer);

    private:
        BrickBozz();
        ~BrickBozz();
    };
}

#endif /* _BRICKBOZZ_H_ */