#ifndef _BRICKMANAGER_H_
#define _BRICKMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <array>
#include <cstring>

#include "Brick.h"

namespace TheBrick
{
    class CBrickManager
    {
    public:
        
    private:
        std::array<CBrick*, 500> m_bricks;
        PuRe_IMaterial* m_pBrickMaterial;

    public:
        CBrickManager();
        ~CBrickManager();

        void Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer);
        void Load(PuRe_IGraphics* a_pGraphics, ong::World* a_pWorld, const char* a_pFolder);

        CBrick* GetBrick(int a_BrickId);
        CBrick** GetCategoryStart(int a_CategoryId);
        int GetCategoryCount();
    };
}

#endif /* _BRICKMANAGER_H_ */