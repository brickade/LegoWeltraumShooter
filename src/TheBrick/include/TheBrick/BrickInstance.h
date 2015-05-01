#ifndef _BRICKINSTANCE_H_
#define _BRICKINSTANCE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>
#include <Onager/myMath.h>

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
        ong::Transform m_Transform;
        ong::Collider* m_pCollider;

    private:

    public:
        CBrickInstance(CBrick* a_pBrick);
        ~CBrickInstance();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

        PuRe_List<SNub>* GetWorldSpaceNubs();
    };
}

#endif /* _BRICKINSTANCE_H_ */