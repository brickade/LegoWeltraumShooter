#ifndef _BRICKMANAGER_H_
#define _BRICKMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <array>

#include "Brick.h"

namespace TheBrick
{
    class CBrickManager
    {
    public:
        
    private:
        std::array<CBrick*, 500> m_bricks;

    public:
        CBrickManager();
        ~CBrickManager();

        void Load(const char* a_pFolder);

        CBrick* GetBrick(int a_BrickId);
        CBrick** GetCategoryStart(int a_CategoryId);
        int GetCategoryCount();
    };
}

#endif /* _BRICKMANAGER_H_ */