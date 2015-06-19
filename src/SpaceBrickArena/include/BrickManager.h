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
}

namespace sba
{
    class CBrickManager
    {
    public:
        
    private:
        std::array<TheBrick::CBrick*, 200> m_bricks;
        PuRe_IMaterial* m_pBrickMaterial;
        PuRe_IMaterial* m_pBrickUIMaterial;
        std::string m_FolderPath = "";

    public:
        CBrickManager(const char* a_pFolder);
        ~CBrickManager();

        void Initialize();
        void Load();
        void Render();
        void RebuildRenderInstances();

        TheBrick::CBrick& GetBrick(size_t a_BrickId);
        TheBrick::CBrick** GetCategoryStart(int a_CategoryId);
        int GetCategoryCount();

        PuRe_IMaterial* GetBrickMaterial();
        PuRe_IMaterial* GetBrickUIMaterial();
    };
}

#endif /* _BRICKMANAGER_H_ */