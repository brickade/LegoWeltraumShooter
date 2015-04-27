#ifndef _BRICKMANAGER_H_
#define _BRICKMANAGER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <array>

#include "Brick.h"

namespace Game
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

        CBrick* GetBrick(int32 a_id);
    };
}

#endif /* _BRICKMANAGER_H_ */