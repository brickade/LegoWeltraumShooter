#ifndef _BRICKINSTANCE_H_
#define _BRICKINSTANCE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Brick.h"

namespace Game
{
    //Forward declaration
    class CBrick;

    class CBrickInstance
    {
    public:
        CBrick* m_pBrick;
        PuRe_Vector3F m_position;
        float32 m_rotation;
        float32 m_scale;
    
    private:
            

    public:
        CBrickInstance(CBrick* a_pBrick);
        ~CBrickInstance();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);
    };
}

#endif /* _BRICKINSTANCE_H_ */