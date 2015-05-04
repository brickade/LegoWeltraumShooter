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

    private:
        static BrickBozz* m_pInstance;
    
    private:
        BrickBozz();
        ~BrickBozz();
    };
}

#endif /* _BRICKBOZZ_H_ */