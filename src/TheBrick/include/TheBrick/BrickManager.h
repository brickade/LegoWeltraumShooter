#ifndef _BRICKMANAGER_H_
#define _BRICKMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Onager/World.h"

#include <array>
#include <cstring>

namespace TheBrick
{
    class CBrick;

    class CBrickManager
    {
    public:
        
    private:
        std::array<CBrick*, 200> m_bricks;
        PuRe_IMaterial* m_pBrickMaterial;
        PuRe_IMaterial* m_pBrickUIMaterial;

    public:
        CBrickManager();
        ~CBrickManager();

        void Initialize(PuRe_IGraphics& a_pGraphics, PuRe_SoundPlayer& a_pSoundPlayer);
        void Load(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, ong::World& a_pWorld, PuRe_IMaterial& a_pMaterial, const char* a_pFolder);
        void Render(PuRe_Renderer& a_rRenderer);
        void RebuildRenderInstances();

        CBrick& GetBrick(int a_BrickId);
        CBrick** GetCategoryStart(int a_CategoryId);
        int GetCategoryCount();

        PuRe_IMaterial* GetBrickMaterial();
        PuRe_IMaterial* GetBrickUIMaterial();
    };
}

#endif /* _BRICKMANAGER_H_ */