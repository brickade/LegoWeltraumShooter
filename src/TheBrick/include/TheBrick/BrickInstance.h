#ifndef _BRICKINSTANCE_H_
#define _BRICKINSTANCE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>

#include "Brick.h"
#include "Nub.h"

namespace TheBrick
{
    //Forward declaration
    class CBrick;

    class CBrickInstance
    {
    public:
        CBrick* m_pBrick;
        PuRe_Vector3F m_position;
        float m_rotation;
        float m_scale;
    
    private:
        ong::Collider* m_pCollider;

    public:
        CBrickInstance(CBrick* a_pBrick);
        ~CBrickInstance();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

        PuRe_List<SNub>* GetWorldSpaceNubs();
    };
}

#endif /* _BRICKINSTANCE_H_ */